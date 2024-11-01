#ifndef UCOMPONENT_FACTORY_CPP
#define UCOMPONENT_FACTORY_CPP

#include "UComponentFactory.h"

namespace RDK
{

 UVirtualMethodFactory::UVirtualMethodFactory(UEPtr<UComponent> comp)
  : UComponentAbstractFactory(comp->GetStorage())
 {
  Component = dynamic_pointer_cast<UContainer>(comp);
  if(Component)
  {
//   Component->Default();
   Component->SetClass(ClassId);
  }
 }

 UVirtualMethodFactory::~UVirtualMethodFactory()
 {
  if(Component)
   delete Component;
 }

 UEPtr<UComponent> UVirtualMethodFactory::New()
 {
  if(!Component)
   return 0;

  UEPtr<UContainer> obj = Component->New();
  obj->SetStorage(Storage);
  obj->Default();
  Component->Copy(obj, Storage);
  return static_pointer_cast<UComponent>(obj);
 }

 UEPtr<UComponent> UVirtualMethodFactory::Prototype(UEPtr<UComponent> prototype)
 {
  if(!Component)
   return 0;

  UEPtr<UContainer> obj = Component->New();
  obj->SetStorage(Storage);
  obj->Default();
  dynamic_pointer_cast<UContainer>(prototype)->Copy(obj, Storage);
  return static_pointer_cast<UComponent>(obj);
 }

 void UVirtualMethodFactory::ResetComponent(UEPtr<UComponent> component) const
 {
  if(Component)
   Component->Copy(dynamic_pointer_cast<UContainer>(component), Component->GetStorage());
 }

 UEPtr<UContainer> UVirtualMethodFactory::GetComponent()
 {
  return Component;
 }

void UVirtualMethodFactory::FreeComponent()
{
 Component=0;
}


 UComponentFactoryMethod::UComponentFactoryMethod(const UEPtr<UStorage> &storage, UComponent* (*funcPointer)(), const std::string &default_component_name)
  : UComponentAbstractFactory(storage)
 {
  Method = funcPointer;
  DefaultComponentName=default_component_name;
 }

 UComponentFactoryMethod::~UComponentFactoryMethod()
 {

 }

 UEPtr<UComponent> UComponentFactoryMethod::New()
 {
  UEPtr<UComponent> obj = Method();
  dynamic_pointer_cast<UContainer>(obj)->Name = DefaultComponentName;
  obj->SetStorage(Storage);
  obj->Default();
  return obj;
 }

 UEPtr<UComponent> UComponentFactoryMethod::Prototype(UEPtr<UComponent> prototype)
 {
  UEPtr<UContainer> obj = dynamic_cast<UContainer*>(Method());
  obj->SetStorage(Storage);
  obj->Default();
  obj->Name = DefaultComponentName;
  dynamic_pointer_cast<const UContainer>(prototype)->Copy(obj, Storage);
  return static_pointer_cast<UComponent>(obj);
 }

 void UComponentFactoryMethod::ResetComponent(UEPtr<UComponent> component) const
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
