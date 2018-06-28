#ifndef UCOMPONENTABSTRACTFACTORY_H
#define UCOMPONENTABSTRACTFACTORY_H

#include "UEPtr.h"
#include "UContainer.h"

namespace RDK {

 extern const UId ForbiddenId;

 /// Абстрактная фабрика
 class UComponentAbstractFactory
 {
 protected:
  UId ClassId;

 public:
  UComponentAbstractFactory();
  virtual ~UComponentAbstractFactory();

  /// Создание компонента
  virtual UEPtr<UComponent> New() = 0;

  /// Создание компонента с копированием в него компонента @param prototype
  virtual UEPtr<UComponent> Prototype(UEPtr<UComponent> prototype, UEPtr<UStorage> storage) = 0;

  /// Сбрасывает объект к исходному состоянию, которое зависит от порождающей фабрики
  virtual void ResetComponent(UEPtr<UComponent> component) const = 0;

  // ClassId set/get
  void SetClassId(const UId id);
  UId GetClassId() const;
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

  virtual UEPtr<UComponent> Prototype(UEPtr<UComponent> prototype, const UEPtr<UStorage> storage);

  virtual void ResetComponent(UEPtr<UComponent> component) const;

  UEPtr<UContainer> GetComponent();
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

  virtual UEPtr<UComponent> Prototype(UEPtr<UComponent> prototype, const UEPtr<UStorage> storage);

  virtual void ResetComponent(UEPtr<UComponent> component) const;
 };

 /*class UXMLDescriptionFactory: public UComponentAbstractFactory
 {
 protected:
  bool CreateStandartAfterInit;
  std::string XMLDescription;

 public:
  UXMLDescriptionFactory(std::string xml_description, UEPtr<UComponent> comp = nullptr, bool create_structure_immediately = false);
  virtual ~UXMLDescriptionFactory();

  virtual UEPtr<UComponent> New();

  virtual UEPtr<UComponent> Prototype(UEPtr<UComponent> prototype, UEPtr<UStorage> storage);

  virtual void ResetComponent(UEPtr<UComponent> component) const;
 };*/

}

#endif // UCOMPONENTABSTRACTFACTORY_H
