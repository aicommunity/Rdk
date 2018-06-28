#include "UComponentFactory.h"

namespace RDK
{


 UVirtualMethodFactory::UVirtualMethodFactory(UEPtr<UComponent> comp)
 {
  Component = dynamic_pointer_cast<UContainer>(comp);
  if(Component)
   Component->SetClass(ClassId);
 }

 UVirtualMethodFactory::~UVirtualMethodFactory()
 {
  delete Component;
 }

 UEPtr<UComponent> UVirtualMethodFactory::New()
 {
  UEPtr<UContainer> obj = Component->New();
  Component->Copy(obj, Component->GetStorage());
  return static_pointer_cast<UComponent>(obj);
 }

 UEPtr<UComponent> UVirtualMethodFactory::Prototype(UEPtr<UComponent> prototype, const UEPtr<UStorage> storage)
 {
  UEPtr<UContainer> obj = Component->New();
  dynamic_pointer_cast<UContainer>(prototype)->Copy(obj, storage);
  return static_pointer_cast<UComponent>(obj);
 }

 void UVirtualMethodFactory::ResetComponent(UEPtr<UComponent> component) const
 {
  Component->Copy(dynamic_pointer_cast<UContainer>(component), Component->GetStorage());
 }

 UEPtr<UContainer> UVirtualMethodFactory::GetComponent()
 {
  return Component;
 }

 UComponentFactoryMethod::UComponentFactoryMethod(UEPtr<UComponent> (*funcPointer)())
 {
  Method = funcPointer;
 }

 UComponentFactoryMethod::~UComponentFactoryMethod()
 {

 }

 UEPtr<UComponent> UComponentFactoryMethod::New()
 {
  UEPtr<UComponent> obj = Method();
  obj->Default();
  return obj;
 }

 UEPtr<UComponent> UComponentFactoryMethod::Prototype(UEPtr<UComponent> prototype, const UEPtr<UStorage> storage)
 {
  UEPtr<UContainer> obj = dynamic_pointer_cast<UContainer>(Method());
  dynamic_pointer_cast<const UContainer>(prototype)->Copy(obj, storage);
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

 UComponentAbstractFactory::UComponentAbstractFactory()
  : ClassId(ForbiddenId)
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
