#include "UComponentFactory.h"

namespace RDK
{


 UVirtualMethodFactory::UVirtualMethodFactory(UEPtr<UComponent> comp)
 {
  Component = dynamic_pointer_cast<UContainer>comp;
 }

 UVirtualMethodFactory::~UVirtualMethodFactory()
 {
  delete Component;
 }

 UEPtr<UComponent> UVirtualMethodFactory::New()
 {
  UEPtr<UContainer> obj = Component->New();
  Component->Copy(obj, Component->GetStorage());
  return dynamic_pointer_cast<UComponent>(obj);
 }

 UEPtr<UComponent> UVirtualMethodFactory::Prototype(const UEPtr<UComponent> &prototype, UEPtr<UStorage> storage)
 {
  UEPtr<UContainer> obj = Component->New();
  dynamic_pointer_cast<const UContainer>(prototype)->Copy(obj, storage);
  return dynamic_pointer_cast<UComponent>(obj);
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
  return Method();
 }

 UEPtr<UComponent> UComponentFactoryMethod::Prototype(const UEPtr<UComponent> &prototype, UEPtr<UStorage> storage)
 {
  UEPtr<UContainer> obj = Method();
  dynamic_pointer_cast<const UContainer>(prototype)->Copy(obj, storage);
  return dynamic_pointer_cast<UComponent>(obj);
 }

 UXMLDescriptionFactory::UXMLDescriptionFactory(std::string xml_description, UEPtr<UComponent> comp, bool create_structure_immediately) :
   UComponentFactoryMethod(comp),
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

 }


}
