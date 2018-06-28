#ifndef UCOMPONENTABSTRACTFACTORY_H
#define UCOMPONENTABSTRACTFACTORY_H

#include "UEPtr.h"
#include "UContainer.h"

namespace RDK {

 extern const UId ForbiddenId;

 /// ����������� �������
 class UComponentAbstractFactory
 {
 protected:
  UId ClassId;

 public:
  UComponentAbstractFactory();
  virtual ~UComponentAbstractFactory();

  /// �������� ����������
  virtual UEPtr<UComponent> New() = 0;

  /// �������� ���������� � ������������ � ���� ���������� @param prototype
  virtual UEPtr<UComponent> Prototype(UEPtr<UComponent> prototype, UEPtr<UStorage> storage) = 0;

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

  virtual UEPtr<UComponent> Prototype(UEPtr<UComponent> prototype, const UEPtr<UStorage> storage);

  virtual void ResetComponent(UEPtr<UComponent> component) const;

  UEPtr<UContainer> GetComponent();
 };

 /// �������, ���������� �� ����������� ��������� ������
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
