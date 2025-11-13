#ifndef UCOMPONENT_FACTORY_CPP
#define UCOMPONENT_FACTORY_CPP

#include "UComponentFactory.h"
#include "UStorage.h"

namespace RDK
{

 UVirtualMethodFactory::UVirtualMethodFactory(std::shared_ptr<UContainer> comp)
  : UComponentAbstractFactory(comp && comp->GetStorage() ? comp->GetStorage().get() : nullptr)
 {
  Component = comp;
  if(Component)
  {
//   Component->Default();
   Component->SetClass(ClassId);
  }
 }

 UVirtualMethodFactory::~UVirtualMethodFactory()
 {
  // Component is now managed by std::shared_ptr, no manual deletion needed
 }

 std::shared_ptr<UContainer> UVirtualMethodFactory::New()
 {
  if(!Component)
   return nullptr;

  try
  {
   std::shared_ptr<UContainer> obj(Component->New());
   if(!obj)
    return nullptr;
    
   obj->SetStorage(Storage);
   obj->Default();
   std::shared_ptr<UStorage> storage_ptr(Storage, [](UStorage*){});
   Component->Copy(obj, storage_ptr);
   return obj;
  }
  catch(...)
  {
   return nullptr;
  }
 }

 std::shared_ptr<UContainer> UVirtualMethodFactory::Prototype(std::shared_ptr<UContainer> prototype)
 {
  if(!Component)
   return nullptr;

  try
  {
   std::shared_ptr<UContainer> obj(Component->New());
   if(!obj)
    return nullptr;
    
   obj->SetStorage(Storage);
   obj->Default();
   std::shared_ptr<UStorage> storage_ptr(Storage, [](UStorage*){});
   prototype->Copy(obj, storage_ptr);
   return obj;
  }
  catch(...)
  {
   return nullptr;
  }
 }

 void UVirtualMethodFactory::ResetComponent(std::shared_ptr<UContainer> component) const
 {
  if(Component)
   Component->Copy(component, Component->GetStorage());
 }

 std::shared_ptr<UContainer> UVirtualMethodFactory::GetComponent()
 {
  return Component;
 }

void UVirtualMethodFactory::FreeComponent()
{
 Component = nullptr;
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
