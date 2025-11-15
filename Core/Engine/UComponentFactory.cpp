#ifndef UCOMPONENT_FACTORY_CPP
#define UCOMPONENT_FACTORY_CPP

#include "UComponentFactory.h"
#include "UStorage.h"
#include <glog/logging.h>

namespace RDK
{

 UVirtualMethodFactory::UVirtualMethodFactory(std::shared_ptr<UContainer> comp)
  : UComponentAbstractFactory(comp && comp->GetStorage() ? comp->GetStorage().get() : nullptr)
 {
  // Store as weak_ptr - prototype stays in ObjectsStorage until factory is destroyed
  // This eliminates the need for PopObject before UploadClass
  Component = comp;
  std::shared_ptr<UContainer> comp_locked = Component.lock();
  if(comp_locked)
  {
//   comp_locked->Default();
   comp_locked->SetClass(ClassId);
  }
 }

 UVirtualMethodFactory::~UVirtualMethodFactory()
 {
  // Component is weak_ptr, so prototype will remain in ObjectsStorage
  // Log destruction for debugging
  try {
   std::shared_ptr<UContainer> comp_locked = Component.lock();
   if(comp_locked)
   {
    std::string comp_name = "unknown";
    void* comp_addr = comp_locked.get();
    size_t comp_use_count = comp_locked.use_count();
    try {
     comp_name = comp_locked->GetName();
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
  // Lock weak_ptr to get shared_ptr to prototype
  std::shared_ptr<UContainer> comp_locked = Component.lock();
  if(!comp_locked)
  {
   LOG(ERROR) << "UVirtualMethodFactory::New - Component.lock() returned nullptr, prototype was destroyed";
   LOG(ERROR) << "UVirtualMethodFactory::New - Storage=" << (Storage ? "valid" : "null") 
               << " ClassId=" << ClassId;
   return nullptr;
  }

  std::string comp_name = "unknown";
  try {
   comp_name = comp_locked->GetName();
  } catch (...) {
   comp_name = "<error>";
  }
  LOG(INFO) << "UVirtualMethodFactory::New - prototype locked: name=" << comp_name 
            << " ClassId=" << ClassId << " use_count=" << comp_locked.use_count();

  try
  {
   // comp_locked->New() returns raw pointer, but we need shared_ptr for enable_shared_from_this
   // Use std::shared_ptr constructor with custom deleter to properly manage the object
   // However, this still won't enable shared_from_this() because object wasn't created via make_shared
   // We need to wrap it properly - but since New() returns raw pointer, we can't use make_shared
   // The object must be created in a way that allows shared_from_this() to work
   LOG(INFO) << "UVirtualMethodFactory::New - calling comp_locked->New() for: name=" << comp_name;
   UContainer* raw_obj = comp_locked->New();
   if(!raw_obj)
   {
    LOG(ERROR) << "UVirtualMethodFactory::New - comp_locked->New() returned nullptr for: name=" << comp_name;
    return nullptr;
   }
   LOG(INFO) << "UVirtualMethodFactory::New - comp_locked->New() succeeded for: name=" << comp_name;
   
   // Create shared_ptr with proper deleter
   // IMPORTANT: This creates shared_ptr from raw pointer, so shared_from_this() won't work
   // Objects created this way cannot use shared_from_this() until they are properly wrapped
   std::shared_ptr<UContainer> obj(raw_obj, [](UContainer* ptr) { delete ptr; });
   
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
    LOG(INFO) << "UVirtualMethodFactory::New - calling comp_locked->Copy() for: name=" << comp_name;
    comp_locked->Copy(obj, storage_ptr);
    LOG(INFO) << "UVirtualMethodFactory::New - comp_locked->Copy() completed for: name=" << comp_name;
   } catch (const std::exception& e) {
    LOG(ERROR) << "UVirtualMethodFactory::New - exception in comp_locked->Copy() for: name=" << comp_name << " error=" << e.what();
    return nullptr; // Return nullptr if Copy() fails
   } catch (...) {
    LOG(ERROR) << "UVirtualMethodFactory::New - unknown exception in comp_locked->Copy() for: name=" << comp_name;
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
  // Lock weak_ptr to get shared_ptr to prototype
  std::shared_ptr<UContainer> comp_locked = Component.lock();
  if(!comp_locked)
   return nullptr;

  try
  {
   // Same issue as New() - comp_locked->New() returns raw pointer
   UContainer* raw_obj = comp_locked->New();
   if(!raw_obj)
    return nullptr;
   
   // Create shared_ptr with proper deleter
   std::shared_ptr<UContainer> obj(raw_obj, [](UContainer* ptr) { delete ptr; });
   
   // Set storage and initialize
   obj->SetStorage(Storage);
   obj->Default();
   
   // Copy properties from prototype
   std::shared_ptr<UStorage> storage_ptr(Storage, [](UStorage*){});
   prototype->Copy(obj, storage_ptr);
   
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
  std::shared_ptr<UContainer> comp_locked = Component.lock();
  if(comp_locked && component)
  {
   try {
    std::shared_ptr<UStorage> storage = comp_locked->GetStorage();
    if(storage)
    {
     comp_locked->Copy(component, storage);
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
  }
  else
  {
   LOG(WARNING) << "UVirtualMethodFactory::ResetComponent - comp_locked or component is nullptr";
  }
 }

 std::shared_ptr<UContainer> UVirtualMethodFactory::GetComponent()
 {
  return Component.lock();  // Lock weak_ptr to get shared_ptr
 }

void UVirtualMethodFactory::FreeComponent()
{
 Component.reset();  // Reset weak_ptr - prototype remains in ObjectsStorage
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
