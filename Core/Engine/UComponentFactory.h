#ifndef UCOMPONENTABSTRACTFACTORY_H
#define UCOMPONENTABSTRACTFACTORY_H

#include <memory>
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
  virtual std::shared_ptr<UContainer> New() = 0;

  /// �������� ���������� � ������������ � ���� ���������� @param prototype
  virtual std::shared_ptr<UContainer> Prototype(std::shared_ptr<UContainer> prototype) = 0;

  /// ���������� ������ � ��������� ���������, ������� ������� �� ����������� �������
  virtual void ResetComponent(std::shared_ptr<UContainer> component) const = 0;

  // ClassId set/get
  void SetClassId(const UId id);
  UId GetClassId() const;
 };

/// �������, ���������� �� ����������� ������ (������ �������� ������)
class UVirtualMethodFactory : public UComponentAbstractFactory
{
protected:
  std::weak_ptr<UContainer> Component;  // Use weak_ptr - prototype stays in ObjectsStorage until factory is destroyed

public:
  UVirtualMethodFactory(std::shared_ptr<UContainer> comp);
  virtual ~UVirtualMethodFactory();

  virtual std::shared_ptr<UContainer> New();

  virtual std::shared_ptr<UContainer> Prototype(std::shared_ptr<UContainer> prototype);

  virtual void ResetComponent(std::shared_ptr<UContainer> component) const;

  std::shared_ptr<UContainer> GetComponent();

  void FreeComponent();
};

 /// �������, ���������� �� ����������� ��������� ������
 class UComponentFactoryMethod : public UComponentAbstractFactory
 {
 protected:
  UContainer* (*Method)(void);
  std::string DefaultComponentName;

 public:
  UComponentFactoryMethod(const std::shared_ptr<UStorage> &storage, UContainer* (*funcPointer)(void), const std::string &default_component_name);
  virtual ~UComponentFactoryMethod();

  virtual std::shared_ptr<UContainer> New();

  virtual std::shared_ptr<UContainer> Prototype(std::shared_ptr<UContainer> prototype);

  virtual void ResetComponent(std::shared_ptr<UContainer> component) const;
 };

 /*class UXMLDescriptionFactory: public UComponentAbstractFactory
 {
 protected:
  bool CreateStandartAfterInit;
  std::string XMLDescription;

 public:
  UXMLDescriptionFactory(std::string xml_description, std::shared_ptr<UComponent> comp = nullptr, bool create_structure_immediately = false);
  virtual ~UXMLDescriptionFactory();

  virtual std::shared_ptr<UComponent> New();

  virtual std::shared_ptr<UComponent> Prototype(std::shared_ptr<UComponent> prototype, std::shared_ptr<UStorage> storage);

  virtual void ResetComponent(std::shared_ptr<UComponent> component) const;
 };*/

}

#endif // UCOMPONENTABSTRACTFACTORY_H
