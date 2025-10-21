#ifndef UCOMPONENT_FACTORY_CPP
#define UCOMPONENT_FACTORY_CPP

#include "UComponentFactory.h"
#include "UStorage.h"

namespace RDK
{

 UVirtualMethodFactory::UVirtualMethodFactory(std::shared_ptr<UComponent> comp)
  : UComponentAbstractFactory(comp->GetStorage().get())
 {
  Component = std::dynamic_pointer_cast<UContainer>(comp);
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

 std::shared_ptr<UComponent> UVirtualMethodFactory::New()
 {
  if(!Component)
   return nullptr;

  std::shared_ptr<UContainer> obj(Component->New());
  obj->SetStorage(std::shared_ptr<UStorage>(Storage));
  obj->Default();
  Component->Copy(UEPtr<UContainer>(obj.get()), Storage);
  return std::static_pointer_cast<UComponent>(obj);
 }

 std::shared_ptr<UComponent> UVirtualMethodFactory::Prototype(std::shared_ptr<UComponent> prototype)
 {
  if(!Component)
   return nullptr;

  std::shared_ptr<UContainer> obj(Component->New());
  obj->SetStorage(std::shared_ptr<UStorage>(Storage));
  obj->Default();
  std::dynamic_pointer_cast<UContainer>(prototype)->Copy(UEPtr<UContainer>(obj.get()), Storage);
  return std::static_pointer_cast<UComponent>(obj);
 }

 void UVirtualMethodFactory::ResetComponent(std::shared_ptr<UComponent> component) const
 {
  if(Component)
   Component->Copy(UEPtr<UContainer>(std::dynamic_pointer_cast<UContainer>(component).get()), UEPtr<UStorage>(Component->GetStorage().get()));
 }

 std::shared_ptr<UContainer> UVirtualMethodFactory::GetComponent()
 {
  return Component;
 }

void UVirtualMethodFactory::FreeComponent()
{
 Component = nullptr;
}


 UComponentFactoryMethod::UComponentFactoryMethod(const std::shared_ptr<UStorage> &storage, UComponent* (*funcPointer)(), const std::string &default_component_name)
  : UComponentAbstractFactory(storage.get())
 {
  Method = funcPointer;
  DefaultComponentName=default_component_name;
 }

 UComponentFactoryMethod::~UComponentFactoryMethod()
 {

 }

 std::shared_ptr<UComponent> UComponentFactoryMethod::New()
 {
  std::shared_ptr<UComponent> obj = std::shared_ptr<UComponent>(Method());
  std::dynamic_pointer_cast<UContainer>(obj)->Name = DefaultComponentName;
  obj->SetStorage(std::shared_ptr<UStorage>(Storage));
  obj->Default();
  return obj;
 }

 std::shared_ptr<UComponent> UComponentFactoryMethod::Prototype(std::shared_ptr<UComponent> prototype)
 {
  std::shared_ptr<UContainer> obj = std::shared_ptr<UContainer>(dynamic_cast<UContainer*>(Method()));
  obj->SetStorage(std::shared_ptr<UStorage>(Storage));
  obj->Default();
  obj->Name = DefaultComponentName;
  std::dynamic_pointer_cast<const UContainer>(prototype)->Copy(UEPtr<UContainer>(obj.get()), Storage);
  return std::static_pointer_cast<UComponent>(obj);
 }

 void UComponentFactoryMethod::ResetComponent(std::shared_ptr<UComponent> component) const
 {
  component->Default();
 }

 /*UXMLDescriptionFactory::UXMLDescriptionFactory(std::string xml_description, UEPtr<UComponent> comp, bool create_structure_immediately) :
   CreateStandartAfterInit(create_structure_immediately)
 {
  if(CreateStandartAfterInit)
  {
   USerStorageXML serStorage;
   serStorage.Load(xml_description, "Save");
  }
 }

 UEPtr<UComponent> UXMLDescriptionFactory::New()
 {
  //UEPtr<UComponent> obj = static_pointer_cast<UComponent>(UEPtr<UNet>(new UNet()));
  return UEPtr();
 }

 UEPtr<UComponent> UXMLDescriptionFactory::Prototype(UEPtr<UComponent> prototype, UEPtr<UStorage> storage)
 {
  //UEPtr<UComponent> obj = static_pointer_cast<UComponent>(UEPtr<UNet>(new UNet()));
  return UEPtr();
 }

 void UXMLDescriptionFactory::ResetComponent(UEPtr<UComponent> component) const
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
