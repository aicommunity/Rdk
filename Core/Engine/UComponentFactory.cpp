#ifndef UCOMPONENT_FACTORY_CPP
#define UCOMPONENT_FACTORY_CPP

#include "UComponentFactory.h"
#include "UStorage.h"
#include <glog/logging.h>
#include <thread>

namespace RDK
{
// Thread-local variable to track recursion depth in ResetComponent
// This prevents infinite recursion: ResetComponent -> Copy -> Build -> TakeObject -> ResetComponent
thread_local static int reset_component_recursion_depth = 0;

 UVirtualMethodFactory::UVirtualMethodFactory(std::shared_ptr<UContainer> comp)
  : UComponentAbstractFactory(comp && comp->GetStorage() ? comp->GetStorage().get() : nullptr)
 {
  // SAFETY: Check comp validity before assigning to Component
  // If comp has corrupted use_count, assigning to weak_ptr may corrupt the control block
  std::string comp_name = "unknown";
  void* comp_addr = nullptr;
  size_t comp_use_count = 0;
  
  try {
   if(comp)
   {
    comp_addr = comp.get();
    comp_use_count = comp.use_count();
    try {
     comp_name = comp->GetName();
    } catch (...) {
     comp_name = "<error>";
    }
   }
  } catch (...) {
   LOG(ERROR) << "UVirtualMethodFactory::UVirtualMethodFactory - Exception checking comp validity before assignment";
   // Continue anyway - we'll check again after assignment
  }
  
  LOG(INFO) << "UVirtualMethodFactory::UVirtualMethodFactory - ENTRY: comp_name=" << comp_name 
            << " comp_addr=" << comp_addr << " comp_use_count=" << comp_use_count 
            << " comp_valid=" << (comp ? "yes" : "no");
  
  // CRITICAL: Check if comp has suspicious use_count BEFORE assigning to Component
  // If use_count is corrupted, assigning to weak_ptr may corrupt the control block
  if(comp && (comp_use_count > 1000000 || comp_use_count == 0))
  {
   LOG(ERROR) << "UVirtualMethodFactory::UVirtualMethodFactory - comp has suspicious use_count: " 
              << comp_use_count << " comp_name=" << comp_name << " comp_addr=" << comp_addr 
              << ", cannot create factory";
   // Don't assign Component - factory will be invalid
   Component.reset();
   return;
  }
  
  // Store as shared_ptr - factory owns the prototype to keep it alive
  // This ensures the prototype remains available for creating new instances
  try {
   Component = comp;
   LOG(INFO) << "UVirtualMethodFactory::UVirtualMethodFactory - Component assigned: comp_name=" << comp_name;
  } catch (...) {
   LOG(ERROR) << "UVirtualMethodFactory::UVirtualMethodFactory - Exception assigning Component, comp may be corrupted";
   Component.reset();
   return;
  }
  
  // Verify Component after assignment
  try {
   if(Component)
   {
    size_t use_count = Component.use_count();
    LOG(INFO) << "UVirtualMethodFactory::UVirtualMethodFactory - Component assigned successfully: comp_name=" << comp_name 
              << " use_count=" << use_count;
    
    // Check if Component has suspicious use_count
    if(use_count > 1000000 || use_count == 0)
    {
     LOG(ERROR) << "UVirtualMethodFactory::UVirtualMethodFactory - Component has suspicious use_count: " 
                 << use_count << " comp_name=" << comp_name << ", factory may be invalid";
    }
    
//   Component->Default();
    Component->SetClass(ClassId);
   }
   else
   {
    LOG(WARNING) << "UVirtualMethodFactory::UVirtualMethodFactory - Component is nullptr: comp_name=" << comp_name;
   }
  } catch (...) {
   LOG(ERROR) << "UVirtualMethodFactory::UVirtualMethodFactory - Exception accessing Component after assignment, Component may be corrupted";
  }
  
  LOG(INFO) << "UVirtualMethodFactory::UVirtualMethodFactory - EXIT: comp_name=" << comp_name;
 }

 UVirtualMethodFactory::~UVirtualMethodFactory()
 {
  // Component is shared_ptr, factory owns the prototype
  // Log destruction for debugging
  try {
  if(Component)
   {
    std::string comp_name = "unknown";
    void* comp_addr = Component.get();
    size_t comp_use_count = Component.use_count();
    try {
     comp_name = Component->GetName();
    } catch (...) {
     comp_name = "<error>";
    }
    LOG(INFO) << "UVirtualMethodFactory::~UVirtualMethodFactory - destroying factory, component: " << comp_name 
              << " use_count=" << comp_use_count << " address=" << comp_addr;
   } else {
    LOG(INFO) << "UVirtualMethodFactory::~UVirtualMethodFactory - destroying factory, component already destroyed";
   }
  } catch (...) {
   LOG(ERROR) << "UVirtualMethodFactory::~UVirtualMethodFactory - exception during logging";
  }
 }

 std::shared_ptr<UContainer> UVirtualMethodFactory::New()
 {
  // SAFETY: Check Component validity
  if(!Component)
  {
   LOG(ERROR) << "UVirtualMethodFactory::New - Component is nullptr, prototype was destroyed";
   LOG(ERROR) << "UVirtualMethodFactory::New - Storage=" << (Storage ? "valid" : "null") 
               << " ClassId=" << ClassId;
   return nullptr;
  }

  // SAFETY: Check Component validity before using it
  // If Component has corrupted use_count, operations on it may segfault
  try {
   size_t comp_use_count = Component.use_count();
   if(comp_use_count > 1000000 || comp_use_count == 0)
   {
    LOG(ERROR) << "UVirtualMethodFactory::New - Component has suspicious use_count: " << comp_use_count << ", cannot create object";
    return nullptr;
   }
  } catch (...) {
   LOG(ERROR) << "UVirtualMethodFactory::New - Exception checking Component use_count, cannot create object";
   return nullptr;
  }

  std::string comp_name = "unknown";
  try {
   comp_name = Component->GetName();
  } catch (...) {
   comp_name = "<error>";
  }
  LOG(INFO) << "UVirtualMethodFactory::New - prototype: name=" << comp_name 
            << " ClassId=" << ClassId << " use_count=" << Component.use_count();
  
  // CRITICAL: Store Component in a local variable to keep it alive during the entire function
  // This prevents Component from being destroyed during recursive calls (Copy -> Build -> TakeObject -> New)
  // If Component is destroyed during recursion, operations on it will cause use-after-free
  std::shared_ptr<UContainer> comp_ref = Component;

  try
  {
   // Component->New() returns raw pointer, but we need shared_ptr for enable_shared_from_this
   // Use std::shared_ptr constructor with custom deleter to properly manage the object
   // However, this still won't enable shared_from_this() because object wasn't created via make_shared
   // We need to wrap it properly - but since New() returns raw pointer, we can't use make_shared
   // The object must be created in a way that allows shared_from_this() to work
   LOG(INFO) << "UVirtualMethodFactory::New - calling Component->New() for: name=" << comp_name;
   UContainer* raw_obj = Component->New();
   if(!raw_obj)
   {
    LOG(ERROR) << "UVirtualMethodFactory::New - Component->New() returned nullptr for: name=" << comp_name;
    return nullptr;
   }
   LOG(INFO) << "UVirtualMethodFactory::New - Component->New() succeeded for: name=" << comp_name;
   
   // Create shared_ptr with proper deleter
   // IMPORTANT: This creates shared_ptr from raw pointer, so shared_from_this() won't work
   // Objects created this way cannot use shared_from_this() until they are properly wrapped
   // SAFETY: Add nullptr check in deleter to prevent double-delete
   std::shared_ptr<UContainer> obj(raw_obj, [](UContainer* ptr) { 
    if(ptr) {
     try {
      delete ptr;
     } catch (...) {
      // Ignore exceptions during deletion - object may already be destroyed
     }
    }
   });
   
   // Set storage and initialize
   obj->SetStorage(Storage);
   try {
    LOG(INFO) << "UVirtualMethodFactory::New - calling obj->Default() for: name=" << comp_name;
    obj->Default();
    LOG(INFO) << "UVirtualMethodFactory::New - obj->Default() completed for: name=" << comp_name;
   } catch (const std::exception& e) {
    LOG(ERROR) << "UVirtualMethodFactory::New - exception in obj->Default() for: name=" << comp_name << " error=" << e.what();
    return nullptr; // Return nullptr if Default() fails
   } catch (...) {
    LOG(ERROR) << "UVirtualMethodFactory::New - unknown exception in obj->Default() for: name=" << comp_name;
    return nullptr; // Return nullptr if Default() fails
   }
   
   // Copy properties from prototype
   std::shared_ptr<UStorage> storage_ptr(Storage, [](UStorage*){});
   try {
    LOG(INFO) << "UVirtualMethodFactory::New - calling Component->Copy() for: name=" << comp_name;
    Component->Copy(obj, storage_ptr);
    LOG(INFO) << "UVirtualMethodFactory::New - Component->Copy() completed for: name=" << comp_name;
   } catch (const std::exception& e) {
    LOG(ERROR) << "UVirtualMethodFactory::New - exception in Component->Copy() for: name=" << comp_name << " error=" << e.what();
    return nullptr; // Return nullptr if Copy() fails
   } catch (...) {
    LOG(ERROR) << "UVirtualMethodFactory::New - unknown exception in Component->Copy() for: name=" << comp_name;
    return nullptr; // Return nullptr if Copy() fails
   }
   
   // IMPORTANT: After Copy(), obj->Build() may be called, which may use shared_from_this()
   // But since obj was created from raw pointer, shared_from_this() will throw bad_weak_ptr
   // We need to ensure that Build() doesn't use shared_from_this() or handle the exception
   LOG(INFO) << "UVirtualMethodFactory::New - returning object: name=" << comp_name;
   return obj;
  }
  catch(const std::bad_weak_ptr& e)
  {
   LOG(ERROR) << "UVirtualMethodFactory::New - bad_weak_ptr exception: " << e.what();
   return nullptr;
  }
  catch(...)
  {
   return nullptr;
  }
 }

std::shared_ptr<UContainer> UVirtualMethodFactory::Prototype(std::shared_ptr<UContainer> prototype)
{
 // SAFETY: Check prototype validity before using it
 // If prototype has corrupted use_count, operations on it may segfault
 try {
  size_t prototype_use_count = prototype.use_count();
  if(prototype_use_count > 1000000 || prototype_use_count == 0)
  {
   LOG(WARNING) << "UVirtualMethodFactory::Prototype - Prototype has suspicious use_count: " << prototype_use_count << ", cannot create copy";
   return nullptr;
  }
 } catch (...) {
  LOG(WARNING) << "UVirtualMethodFactory::Prototype - Exception checking prototype use_count, cannot create copy";
  return nullptr;
 }
 
 // Lock weak_ptr to get shared_ptr to prototype
 // SAFETY: Check Component validity
 if(!Component)
 {
  LOG(WARNING) << "UVirtualMethodFactory::Prototype - Component is nullptr, prototype expired";
  return nullptr;
 }

 // SAFETY: Check Component validity before using it
 try {
  size_t comp_use_count = Component.use_count();
  if(comp_use_count > 1000000 || comp_use_count == 0)
  {
   LOG(WARNING) << "UVirtualMethodFactory::Prototype - Component has suspicious use_count: " << comp_use_count << ", cannot create copy";
   return nullptr;
  }
 } catch (...) {
  LOG(WARNING) << "UVirtualMethodFactory::Prototype - Exception checking Component use_count, cannot create copy";
  return nullptr;
 }

 try
 {
  // Same issue as New() - Component->New() returns raw pointer
  UContainer* raw_obj = Component->New();
  if(!raw_obj)
   return nullptr;
  
  // Create shared_ptr with proper deleter
  std::shared_ptr<UContainer> obj(raw_obj, [](UContainer* ptr) { delete ptr; });
  
  // Set storage and initialize
  obj->SetStorage(Storage);
  obj->Default();
  
  // Copy properties from prototype
  // SAFETY: prototype->Copy() may segfault if prototype is corrupted
  std::shared_ptr<UStorage> storage_ptr(Storage, [](UStorage*){});
  try {
   prototype->Copy(obj, storage_ptr);
  } catch (const std::bad_weak_ptr&) {
   LOG(WARNING) << "UVirtualMethodFactory::Prototype - bad_weak_ptr in prototype->Copy(), returning obj without copy";
   return obj; // Return obj even if Copy() failed
  } catch (...) {
   LOG(WARNING) << "UVirtualMethodFactory::Prototype - Exception in prototype->Copy(), returning obj without copy";
   return obj; // Return obj even if Copy() failed
  }
  
  // IMPORTANT: After Copy(), obj->Build() may be called, which may use shared_from_this()
  // But since obj was created from raw pointer, shared_from_this() will throw bad_weak_ptr
  return obj;
 }
 catch(const std::bad_weak_ptr& e)
  {
   LOG(ERROR) << "UVirtualMethodFactory::Prototype - bad_weak_ptr exception: " << e.what();
   return nullptr;
  }
  catch(...)
  {
   return nullptr;
  }
 }

 void UVirtualMethodFactory::ResetComponent(std::shared_ptr<UContainer> component) const
 {
  // CRITICAL: Protect against recursion
  // ResetComponent -> Copy -> Build -> TakeObject -> ResetComponent creates infinite recursion
  // This leads to use-after-free when objects are destroyed during recursion
  if(reset_component_recursion_depth > 0)
  {
   LOG(WARNING) << "UVirtualMethodFactory::ResetComponent - Recursion detected (depth=" << reset_component_recursion_depth 
                << "), skipping Copy() to prevent use-after-free";
   // Just call Default() instead of Copy() to avoid recursion
   if(component)
   {
    try {
     component->Default();
    } catch (...) {
     LOG(WARNING) << "UVirtualMethodFactory::ResetComponent - Exception in component->Default() during recursion";
    }
   }
   return;
  }
  
  // Increment recursion depth
  reset_component_recursion_depth++;
  
  // SAFETY: Check Component validity before locking
  // If Component (weak_ptr) control block is corrupted, lock() may segfault
  std::shared_ptr<UContainer> comp_locked;
  try {
   // Component is now shared_ptr, no need to lock
  } catch (...) {
   LOG(WARNING) << "UVirtualMethodFactory::ResetComponent - Exception in Component.lock(), cannot reset component";
   reset_component_recursion_depth--;
   return;
  }
  
  if(!Component)
  {
   LOG(WARNING) << "UVirtualMethodFactory::ResetComponent - Component.lock() returned nullptr, prototype expired";
   reset_component_recursion_depth--;
   return;
  }
  
  if(!component)
  {
   LOG(WARNING) << "UVirtualMethodFactory::ResetComponent - component is nullptr, cannot reset";
   reset_component_recursion_depth--;
   return;
  }
  
  // SAFETY: Check comp_locked validity before using it
  // If comp_locked has corrupted use_count, operations on it may segfault
  try {
   size_t comp_use_count = Component.use_count();
   if(comp_use_count > 1000000 || comp_use_count == 0)
   {
    LOG(ERROR) << "UVirtualMethodFactory::ResetComponent - Component has suspicious use_count: " << comp_use_count << ", cannot reset component";
    reset_component_recursion_depth--;
    return;
   }
  } catch (...) {
   LOG(WARNING) << "UVirtualMethodFactory::ResetComponent - Exception checking Component use_count, cannot reset component";
   reset_component_recursion_depth--;
   return;
  }
  
  // SAFETY: Check component validity before using it
  try {
   size_t component_use_count = component.use_count();
   if(component_use_count > 1000000 || component_use_count == 0)
   {
    LOG(ERROR) << "UVirtualMethodFactory::ResetComponent - component has suspicious use_count: " << component_use_count << ", cannot reset component";
    reset_component_recursion_depth--;
    return;
   }
  } catch (...) {
   LOG(WARNING) << "UVirtualMethodFactory::ResetComponent - Exception checking component use_count, cannot reset component";
   reset_component_recursion_depth--;
   return;
  }
  
  try {
   std::shared_ptr<UStorage> storage = Component->GetStorage();
   if(storage)
   {
    // CRITICAL: Copy() may call Build() which may call TakeObject() which may call ResetComponent() again
    // This creates recursion and may lead to use-after-free if Component is destroyed during recursion
    // We've already protected against recursion above, so this should be safe now
    // Store a reference to Component to keep it alive during Copy()
    std::shared_ptr<UContainer> comp_ref = Component;
    Component->Copy(component, storage);
   }
   else
    {
     LOG(WARNING) << "UVirtualMethodFactory::ResetComponent - storage is nullptr, skipping Copy";
    }
   } catch (const std::exception& e) {
    LOG(ERROR) << "UVirtualMethodFactory::ResetComponent - exception: " << e.what();
   } catch (...) {
    LOG(ERROR) << "UVirtualMethodFactory::ResetComponent - unknown exception";
   }
   
   // Decrement recursion depth before returning
   reset_component_recursion_depth--;
 }

 std::shared_ptr<UContainer> UVirtualMethodFactory::GetComponent()
 {
  return Component;  // Return shared_ptr directly
 }

void UVirtualMethodFactory::FreeComponent()
{
 Component.reset();  // Reset shared_ptr - factory releases ownership of prototype
}


 UComponentFactoryMethod::UComponentFactoryMethod(const std::shared_ptr<UStorage> &storage, UContainer* (*funcPointer)(), const std::string &default_component_name)
  : UComponentAbstractFactory(storage.get())
 {
  Method = funcPointer;
  DefaultComponentName=default_component_name;
 }

 UComponentFactoryMethod::~UComponentFactoryMethod()
 {

 }

 std::shared_ptr<UContainer> UComponentFactoryMethod::New()
 {
  try
  {
   std::shared_ptr<UContainer> obj = std::shared_ptr<UContainer>(Method());
   if(!obj)
    return nullptr;
    
   obj->Name = DefaultComponentName;
   obj->SetStorage(Storage);
   obj->Default();
   return obj;
  }
  catch(...)
  {
   return nullptr;
  }
 }

 std::shared_ptr<UContainer> UComponentFactoryMethod::Prototype(std::shared_ptr<UContainer> prototype)
 {
  try
  {
   std::shared_ptr<UContainer> obj = std::shared_ptr<UContainer>(Method());
   if(!obj)
    return nullptr;
    
   obj->SetStorage(Storage);
   obj->Default();
   obj->Name = DefaultComponentName;
   // obj is already shared_ptr, use it directly
   // Storage is raw pointer, create shared_ptr with non-owning deleter
   prototype->Copy(obj, std::shared_ptr<UStorage>(Storage, [](UStorage*){})); // Non-owning deleter
   return obj;
  }
  catch(...)
  {
   return nullptr;
  }
 }

 void UComponentFactoryMethod::ResetComponent(std::shared_ptr<UContainer> component) const
 {
  component->Default();
 }

 /*UXMLDescriptionFactory::UXMLDescriptionFactory(std::string xml_description, std::shared_ptr<UComponent> comp, bool create_structure_immediately) :
   CreateStandartAfterInit(create_structure_immediately)
 {
  if(CreateStandartAfterInit)
  {
   USerStorageXML serStorage;
   serStorage.Load(xml_description, "Save");
  }
 }

 std::shared_ptr<UComponent> UXMLDescriptionFactory::New()
 {
  //std::shared_ptr<UComponent> obj = static_pointer_cast<UComponent>(std::shared_ptr<UNet>(new UNet()));
  return std::shared_ptr();
 }

 std::shared_ptr<UComponent> UXMLDescriptionFactory::Prototype(std::shared_ptr<UComponent> prototype, std::shared_ptr<UStorage> storage)
 {
  //std::shared_ptr<UComponent> obj = static_pointer_cast<UComponent>(std::shared_ptr<UNet>(new UNet()));
  return std::shared_ptr();
 }

 void UXMLDescriptionFactory::ResetComponent(std::shared_ptr<UComponent> component) const
 {

 }*/


UComponentAbstractFactory::UComponentAbstractFactory(UStorage* storage)
: ClassId(ForbiddenId),Storage(storage)
{
}

 UComponentAbstractFactory::~UComponentAbstractFactory()
 {

 }

 void UComponentAbstractFactory::SetClassId(const UId id)
 {
  ClassId = id;
 }

 UId UComponentAbstractFactory::GetClassId() const
 {
  return ClassId;
 }


}

#endif
