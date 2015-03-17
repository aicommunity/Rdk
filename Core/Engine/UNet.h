/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UANetH
#define UANetH

#include "UADItem.h"
#include "UStorage.h"


namespace RDK {

class RDK_LIB_TYPE UNet: public UADItem
{
protected: // �������� ��������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UNet(void);
virtual ~UNet(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const;

// ��������� ��� ����� ����� ����� ������������ � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<RDK::UNet> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, int sublevel=-1);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------

// ----------------------
// ������ ���������� �������
// ----------------------
public:
// ������������� ����� ����� 'link'
template<typename T>
bool CreateLink(const ULinkT<T> &link);

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
template<typename T>
bool CreateLink(const ULinkSideT<T> &itemid, const ULinkSideT<T> &connectorid);
virtual bool CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
virtual bool CreateLink(const NameT &item, int item_index,
						const NameT &connector, int connector_index=-1);
virtual bool CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index);
//virtual bool CreateLink(const NameT &item, const NameT &item_property_name,
//						const NameT &connector, const NameT &connector_property_name);

// ������������� ��� ����� �� ������� 'linkslist'
template<typename T>
bool CreateLinks(const ULinksListT<T> &linkslist, UEPtr<UNet> owner_level=0);

// ��������� ��� ����� � ������� �������� ����, ����
// 'id' - ���� Id �������� ����.
// �����, ���� 'id' - Id ����������, �� ����� ���������
// ����� ����� ����������
template<typename T>
bool BreakLink(const ULinkSideT<T> &id);

// ��������� ����� 'link'
template<typename T>
bool BreakLink(const ULinkT<T> &link);

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
template<typename T>
bool BreakLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector);
virtual bool BreakLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
virtual bool BreakLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index);
virtual bool BreakLink(const NameT &item, const NameT &item_property_name,
						const NameT &connector, const NameT &connector_property_name);

// ��������� ��� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
virtual bool BreakLink(const NameT &itemname, const NameT &connectorname);

// ��������� ��� ����� ����
// �������� �� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual void BreakLinks(UEPtr<UContainer> brklevel);

// ��������� �������� ����� ����
virtual bool BreakLinks(const ULinksList &linkslist);

// ��������� ��� ���������� ����� ����.
virtual void BreakLinks(void);

// ��������� ����� �� ����� connector_index ���������� 'connectorid'
virtual void BreakConnectorLink(const NameT &connectorname, int connector_index);
virtual void BreakConnectorLink(const NameT &connectorname, const NameT &connector_index);

// ���������, ���������� �� �������� �����
template<typename T>
bool CheckLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector);

bool CheckLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);
bool CheckLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index);
bool CheckLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name);
bool CheckLink(const NameT &itemname,
						const NameT &connectorname);
// ----------------------

// --------------------------
// ������ ������������ ���������
// --------------------------
// ���������� �������� ���������� �� ��������������
virtual bool GetComponentProperties(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ���������� ���������� �������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual bool GetComponentSelectedProperties(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage);

// ���������� �������� ���������� �� �������������� � ����������
// ������ ��� buffer ������ ���� ��������!
virtual bool GetComponentPropertiesEx(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ������������� �������� ���������� �� ��������������
virtual int SetComponentProperties(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
virtual bool SaveComponent(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
virtual bool LoadComponent(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage, bool links);

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
virtual bool SaveComponentProperties(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
virtual bool LoadComponentProperties(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
virtual void SetGlobalComponentPropertyValue(RDK::UContainer* cont, UId classid, const char *paramname, const char *buffer);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
virtual void SetGlobalOwnerComponentPropertyValue(RDK::UContainer* cont, UId classid, UId owner_classid, const char *paramname, const char *buffer);

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int SetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentInputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentOutputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// ��������� ��� ������� ����� c ����������� cont � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentPersonalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
virtual bool SaveComponentDrawInfo(RDK::UNet* cont, RDK::USerStorageXML *serstorage);
// --------------------------

// ----------------------
// ������ ���������� ������������ �������� ������
// ----------------------
/// ��������� ������������� ���������� � �������� ������
template<typename T>
UEPtr<T> FindComponentByNameAndType(const NameT &component_name);

/// ��������� ������������� ���������� � �������� ������ � ������� ���
/// ��� �������������.
/// ���������� ��������� �� ��������� ���������, ���� �� ��� ��������
/// ��� 0
template<typename T>
UEPtr<T> AddMissingComponent(const NameT &component_name, const NameT &class_name);
// ----------------------

// --------------------------
// ������� ������ ������� � ���������
// --------------------------
protected:
template<typename T>
ULinksListT<T>& GetLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level=0) const;

template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<UContainer> cont, UEPtr<UContainer> cont2, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const;
// --------------------------
};

// ----------------------
// ������ ���������� ������������ �������� ������
// ----------------------
/// ��������� ������������� ���������� � �������� ������
template<typename T>
UEPtr<T> UNet::FindComponentByNameAndType(const NameT &component_name)
{
 UEPtr<T> comp=dynamic_pointer_cast<T>(GetComponent(component_name,true));
 if(comp)
  return comp;

 return 0;
}


/// ��������� ������������� ���������� � �������� ������ � ������� ���
/// ��� �������������.
/// ���������� ��������� �� ��������� ���������, ���� �� ��� ��������
/// ��� 0
template<typename T>
UEPtr<T> UNet::AddMissingComponent(const NameT &component_name, const NameT &class_name)
{
 UEPtr<T> comp=dynamic_pointer_cast<T>(GetComponent(component_name,true));
 if(comp)
  return comp;

 if(!Storage)
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - Storage not found"));
  return comp;
 }

 UEPtr<UComponent> proto=Storage->TakeObject(class_name);
 if(!proto)
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - Component not found in storage. ClassName=")+class_name);
  return comp;
 }
 comp=dynamic_pointer_cast<T>(proto);
 comp->SetName(component_name);
 comp->SetTimeStep(TimeStep);
 if(!AddComponent(comp))
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - AddComponent failed. ClassName=")+class_name);
  Storage->ReturnObject(comp);
  comp=0;
  return comp;
 }
 return comp;
}
// ----------------------



}

#endif
