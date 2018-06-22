#ifndef UCOMPONENTABSTRACTFACTORY_H
#define UCOMPONENTABSTRACTFACTORY_H

#include "UEPtr.h"
#include "UContainer.h"

namespace RDK {

 /// Абстрактная фабрика
 class UComponentAbstractFactory
 {
 public:
  virtual UEPtr<UComponent> New() = 0;


  virtual UEPtr<UComponent> Prototype(const UEPtr<UComponent> &prototype, UEPtr<UStorage> storage) = 0;
 };

 /// Фабрика, основанная на виртуальном методе (старый механизм работы)
 class UVirtualMethodFactory : public UComponentAbstractFactory
 {
 protected:
  UEPtr<UContainer> Component;

 public:
  UVirtualMethodFactory(UEPtr<UComponent> comp);
  virtual ~UVirtualMethodFactory();

  virtual UEPtr<UComponent> New();

  virtual UEPtr<UComponent> Prototype(const UEPtr<UComponent> &prototype, UEPtr<UStorage> storage);
 };

 /// Фабрика, основанная на статическом фабричном методе
 class UComponentFactoryMethod : public UComponentAbstractFactory
 {
 protected:
  UEPtr<UComponent> (*Method)(void);

 public:
  UComponentFactoryMethod(UEPtr<UComponent> (*funcPointer)(void));
  virtual ~UComponentFactoryMethod();

  virtual UEPtr<UComponent> New();

  virtual UEPtr<UComponent> Prototype(const UEPtr<UComponent> &prototype, UEPtr<UStorage> storage);
 };

 class UXMLDescriptionFactory: public UComponentFactoryMethod
 {
 protected:
  bool CreateStandartAfterInit;
  std::string XMLDescription;

 public:
  UXMLDescriptionFactory(std::string xml_description, UEPtr<UComponent> comp = nullptr, bool create_structure_immediately = false);
  virtual ~UXMLDescriptionFactory();

  virtual UEPtr<UComponent> New();
 };

}

#endif // UCOMPONENTABSTRACTFACTORY_H
