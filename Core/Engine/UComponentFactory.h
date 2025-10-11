#ifndef UCOMPONENTABSTRACTFACTORY_H
#define UCOMPONENTABSTRACTFACTORY_H

#include "UEPtr.h"
#include "UContainer.h"
#include "ModernSmartPointers.h"

namespace RDK {

 extern const UId ForbiddenId;

 /// ����������� �������
 class UComponentAbstractFactory
 {
 protected:
  UId ClassId;

  UStorage* Storage;

 public:
  UComponentAbstractFactory(UStorage* storage);
  virtual ~UComponentAbstractFactory();

  /// �������� ����������
  virtual UEPtr<UComponent> New() = 0;

  /// �������� ���������� � ������������ � ���� ���������� @param prototype
  virtual UEPtr<UComponent> Prototype(UEPtr<UComponent> prototype) = 0;

  /// ���������� ������ � ��������� ���������, ������� ������� �� ����������� �������
  virtual void ResetComponent(UEPtr<UComponent> component) const = 0;

  // ClassId set/get
  void SetClassId(const UId id);
  UId GetClassId() const;
 };

 /// �������, ���������� �� ����������� ������ (������ �������� ������)
 class UVirtualMethodFactory : public UComponentAbstractFactory
 {
 protected:
  UEPtr<UContainer> Component;

 public:
  UVirtualMethodFactory(UEPtr<UComponent> comp);
  virtual ~UVirtualMethodFactory();

  virtual UEPtr<UComponent> New();

  virtual UEPtr<UComponent> Prototype(UEPtr<UComponent> prototype);

  virtual void ResetComponent(UEPtr<UComponent> component) const;

  UEPtr<UContainer> GetComponent();

  void FreeComponent();
 };

 /// �������, ���������� �� ����������� ��������� ������
 class UComponentFactoryMethod : public UComponentAbstractFactory
 {
 protected:
  UComponent* (*Method)(void);
  std::string DefaultComponentName;

 public:
  UComponentFactoryMethod(const UEPtr<UStorage> &storage, UComponent* (*funcPointer)(void), const std::string &default_component_name);
  virtual ~UComponentFactoryMethod();

  virtual UEPtr<UComponent> New();

  virtual UEPtr<UComponent> Prototype(UEPtr<UComponent> prototype);

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
