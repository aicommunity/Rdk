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

#include "UItem.h"
#include "UStorage.h"


namespace RDK {

class RDK_LIB_TYPE UNet: public UItem
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
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel, bool exclude_internals=false, std::shared_ptr<UContainer> internal_level=0) const;

// ��������� ��� ����� ����� ����� ������������ � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
template<typename T>
ULinksListT<T>& GetPersonalLinks(std::shared_ptr<UNet> cont, ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel, int sublevel=-1);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
virtual UContainer* New(void);

// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
virtual bool Copy(std::shared_ptr<UContainer> target, std::shared_ptr<UStorage> stor=0, bool copystate=false) const;

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
virtual bool CheckComponentType(std::shared_ptr<UContainer> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
virtual bool AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(std::shared_ptr<UContainer> comp);

// Helper methods to create non-owning shared_ptr for 'this'
std::shared_ptr<UNet> GetThisAsSharedNet();
std::shared_ptr<UItem> GetThisAsSharedItem();  
std::shared_ptr<UConnector> GetThisAsSharedConnector();
std::shared_ptr<UContainer> GetThisAsSharedContainer();
// --------------------------

// ----------------------
// ������ ���������� �������
// ----------------------
public:
// ������������� ����� ����� 'link'
template<typename T>
bool CreateLink(const ULinkT<T> &link, bool forced_connect_same_item=false);

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
template<typename T>
bool CreateLink(const ULinkSideT<T> &itemid, const ULinkSideT<T> &connectorid, bool forced_connect_same_item=false);

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
virtual bool CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index, int connector_c_index=-1, bool forced_connect_same_item=false);

// ������������� ��� ����� �� ������� 'linkslist'
template<typename T>
bool CreateLinks(const ULinksListT<T> &linkslist, std::shared_ptr<UNet> owner_level=0);

// ��������� ����� 'link'
template<typename T>
bool BreakLink(const ULinkT<T> &link);

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
template<typename T>
bool BreakLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector);

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
virtual bool BreakLink(const NameT &item, const NameT &item_property_name,
						const NameT &connector, const NameT &connector_property_name, int connector_c_index=-1);

// ��������� ��� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
virtual bool BreakLink(const NameT &itemname, const NameT &connectorname);

// ��������� ��� ����� ����� ������� �������� ���� � ������ ������������
virtual bool BreakAllOutgoingLinks(const NameT &itemname);
virtual bool BreakAllOutgoingLinks(const NameT &itemname, const NameT &item_property_name);

// ��������� ��� ����� ����
// �������� �� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual void BreakLinks(std::shared_ptr<UContainer> brklevel);

// ��������� �������� ����� ����
virtual bool BreakLinks(const ULinksList &linkslist);

// ��������� ��� ���������� ����� ����.
virtual void BreakLinks(void);

// ���������, ���������� �� �������� �����
template<typename T>
bool CheckLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector);

bool CheckLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index=-1);
bool CheckLink(const NameT &itemname,
						const NameT &connectorname, int connector_c_index=-1);

/// ����������� ��������� ����� �� 'output_name1' ������ ���������� 'item1' �� ����� 'output_name2' ���������� 'item2'
bool SwitchOutputLinks(const UStringLinkSide &item1, const UStringLinkSide &item2);
bool SwitchOutputLinks(const NameT &itemname1, const NameT &output_name1,
						const NameT &itemname2, const NameT &output_name2);
// ----------------------

// --------------------------
// ������ ������������ ���������
// --------------------------
// ���������� �������� ���������� �� ��������������
virtual bool GetComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ���������� ���������� �������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual bool GetComponentSelectedProperties(RDK::USerStorageXML *serstorage);

// ���������� �������� ���������� �� �������������� � ����������
// ������ ��� buffer ������ ���� ��������!
virtual bool GetComponentPropertiesEx(RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ������������� �������� ���������� �� ��������������
virtual int SetComponentProperties(RDK::USerStorageXML *serstorage);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
virtual bool SaveComponent(RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
virtual bool LoadComponent(RDK::USerStorageXML *serstorage, bool links);

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
virtual bool SaveComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
virtual bool LoadComponentProperties(RDK::USerStorageXML *serstorage);

/// ��������� ������ ��������� ����������
virtual bool SaveComponentStructure(RDK::USerStorageXML *serstorage, bool links, unsigned int type_mask);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
virtual void SetGlobalComponentPropertyValue(UId classid, const char *paramname, const char *buffer);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
virtual void SetGlobalOwnerComponentPropertyValue(UId classid, UId owner_classid, const char *paramname, const char *buffer);

// ��������� ��� ����� ������ ���������� � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int SetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentInputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentOutputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// ��������� ��� ������� ����� c ����������� cont � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentPersonalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
virtual bool SaveComponentDrawInfo(RDK::USerStorageXML *serstorage);
// --------------------------

// ----------------------
// ������ ���������� ������������ �������� ������
// ----------------------
/// ��������� ������������� ���������� � �������� ������
template<typename T>
std::shared_ptr<T> FindComponentByNameAndType(const NameT &component_name);

/// ��������� ������������� ���������� � �������� ������ � ������� ���
/// ��� �������������.
/// ���������� ��������� �� ��������� ���������, ���� �� ��� ��������
/// ��� 0
template<typename T>
std::shared_ptr<T> AddMissingComponent(const NameT &component_name, const NameT &class_name, std::shared_ptr<UIPointer> pointer=0);
// ----------------------

// --------------------------
// ������� ������ ������� � ���������
// --------------------------
protected:
template<typename T>
ULinksListT<T>& GetLinks(std::shared_ptr<UContainer> cont, ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel, bool exclude_internals, std::shared_ptr<UContainer> internal_level=0) const;

template<typename T>
ULinksListT<T>& GetPersonalLinks(std::shared_ptr<UContainer> cont, std::shared_ptr<UContainer> cont2, ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel) const;
// --------------------------
};

// --------------------------
// ������ ������� � ���������
// --------------------------
template<typename T>
ULinksListT<T>& UNet::GetLinks(ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel, bool exclude_internals, std::shared_ptr<UContainer> internal_level) const
{
 GetLinks(const_cast<UNet*>(this)->GetThisAsSharedContainer(), linkslist, netlevel, exclude_internals, internal_level);

 return linkslist;
}

// ��������� ��� ����� ����� ����� ������������ � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
template<typename T>
ULinksListT<T>& UNet::GetPersonalLinks(std::shared_ptr<UNet> cont, ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel, int sublevel)
{
 GetPersonalLinks(const_cast<UNet*>(this), cont, linkslist, netlevel);

 return linkslist;
}

// ������������� ����� ����� 'link'
template<typename T>
bool UNet::CreateLink(const ULinkT<T> &link, bool forced_connect_same_item)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
 {
  res &=CreateLink(link.Item, link.Connector[i], forced_connect_same_item);
 }

 return res;
}

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
template<typename T>
bool UNet::CreateLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector, bool forced_connect_same_item)
{
 std::shared_ptr<UItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=GetThisAsSharedItem();
 else
  pitem=std::shared_ptr<UItem>(std::dynamic_pointer_cast<UItem>(std::shared_ptr<UContainer>(GetComponentL(item.Id,true).get())).get());

 std::shared_ptr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=GetThisAsSharedConnector();
 else
  pconnector=std::shared_ptr<UConnector>(std::dynamic_pointer_cast<UConnector>(std::shared_ptr<UContainer>(GetComponentL(connector.Id,true).get())).get());

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.Name);
  return false;
 }

// bool res=(item.Name == "DcOrientation");
// bool res2=(connector.Name == "InputReliability");
 if(!item.Name.empty() || !connector.Name.empty())
 {
  int c_index=connector.Index;
  if(!(pitem->Connect(pconnector,item.Name,connector.Name,c_index, forced_connect_same_item)))
   return false;
 }

 return true;
}

// ������������� ��� ����� �� ������� 'linkslist'.
template<typename T>
bool UNet::CreateLinks(const ULinksListT<T> &linkslist, std::shared_ptr<UNet> owner_level)
{
 bool res=true;

 int i=0;
 for(i=0;i<linkslist.GetSize();i++)
 {
  if(owner_level)
   res&=owner_level->CreateLink(linkslist[i]);
  else
   res&=CreateLink(linkslist[i]);
 }

 if(!res)
  return false;

 return res;
}

// ��������� ����� 'link'
template<typename T>
bool UNet::BreakLink(const ULinkT<T> &link)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
  res&=BreakLink(link.Item,link.Connector[i]);

 return res;
}

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
template<typename T>
bool UNet::BreakLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 std::shared_ptr<UItem> pitem=0;
 if(!CheckLongId(item.Id))
  pitem=GetThisAsSharedItem();
 else
  pitem=std::shared_ptr<UItem>(std::dynamic_pointer_cast<UItem>(std::shared_ptr<UContainer>(GetComponentL(item.Id,true).get())).get());

 std::shared_ptr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=GetThisAsSharedConnector();
 else
  pconnector=std::shared_ptr<UConnector>(std::dynamic_pointer_cast<UConnector>(std::shared_ptr<UContainer>(GetComponentL(connector.Id,true).get())).get());

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.Name);
  return false;
 }

 if(!item.Name.empty() || !connector.Name.empty())
 {
  pitem->Disconnect(pconnector, item.Name, connector.Name, connector.Index);
  return true;
 }

 return false;
}

template<typename T>
bool UNet::CheckLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 std::shared_ptr<UItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=GetThisAsSharedItem();
 else
  pitem=std::shared_ptr<UItem>(std::dynamic_pointer_cast<UItem>(std::shared_ptr<UContainer>(GetComponentL(item.Id,true).get())).get());

 std::shared_ptr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=GetThisAsSharedConnector();
 else
  pconnector=std::shared_ptr<UConnector>(std::dynamic_pointer_cast<UConnector>(std::shared_ptr<UContainer>(GetComponentL(connector.Id,true).get())).get());

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.Name);
  return false;
 }
  if(!item.Name.empty() || !connector.Name.empty())
  {
   if(pitem->CheckLink(pconnector,item.Name, connector.Name, connector.Index))
	return true;
  }

 return false;
}
// --------------------------

// ----------------------
// ������ ���������� ������������ �������� ������
// ----------------------
/// ��������� ������������� ���������� � �������� ������
template<typename T>
std::shared_ptr<T> UNet::FindComponentByNameAndType(const NameT &component_name)
{
 std::shared_ptr<T> comp=std::dynamic_pointer_cast<T>(GetComponent(component_name,true));
 if(comp)
  return comp;

 return 0;
}


/// ��������� ������������� ���������� � �������� ������ � ������� ���
/// ��� �������������.
/// ���������� ��������� �� ��������� ���������, ���� �� ��� ��������
/// ��� 0
template<typename T>
std::shared_ptr<T> UNet::AddMissingComponent(const NameT &component_name, const NameT &class_name, std::shared_ptr<UIPointer> pointer)
{
 std::shared_ptr<UContainer> found_comp=GetComponent(component_name,true);
 std::shared_ptr<T> comp;
 if(found_comp)
 {
  comp=std::shared_ptr<T>(std::dynamic_pointer_cast<T>(std::shared_ptr<UContainer>(found_comp.get())).get());
  if(comp && comp->GetCompClassName() == class_name)
   return comp;
  else
   DelComponent(component_name);
 }

 auto storage = Storage.lock();
 if(!storage)
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - Storage not found"));
  return comp;
 }

 std::shared_ptr<UComponent> proto=storage->TakeObject(class_name);
 if(!proto)
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - Component not found in the storage. ClassName=")+class_name);
  return comp;
 }

 comp=std::shared_ptr<T>(std::dynamic_pointer_cast<T>(std::shared_ptr<UComponent>(proto.get())).get());
 if(!comp)
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - component found in the storage but cannot convert to ")+std::string(typeid(T).name())+std::string(". ClassName=")+class_name);
  return comp;
 }

 comp->SetName(component_name);
 comp->SetTimeStep(TimeStep);
 if(!AddComponent(comp, pointer))
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - AddComponent failed. ClassName=")+class_name);
  storage->ReturnObject(comp);
  comp=0;
  return comp;
 }
 return comp;
}
// ----------------------


// --------------------------
// ������� ������ ������� � ���������
// --------------------------
template<typename T>
ULinksListT<T>& UNet::GetLinks(std::shared_ptr<UContainer> cont, ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel, bool exclude_internals, std::shared_ptr<UContainer> internal_level) const
{
 if(std::dynamic_pointer_cast<UItem>(cont))
 {
  std::static_pointer_cast<UConnector>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);
  std::static_pointer_cast<UItem>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);
 }
 else
 if(std::dynamic_pointer_cast<UConnector>(cont))
  std::static_pointer_cast<UConnector>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetLinks(cont->GetComponentByIndex(i), linkslist, netlevel, exclude_internals,internal_level);

 return linkslist;
}

template<typename T>
ULinksListT<T>& UNet::GetPersonalLinks(std::shared_ptr<UContainer> cont, std::shared_ptr<UContainer> cont2, ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel) const
{
 if(std::dynamic_pointer_cast<UItem>(cont))
 {
  std::static_pointer_cast<UConnector>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);
  std::static_pointer_cast<UItem>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);
 }
 else
 if(std::dynamic_pointer_cast<UConnector>(cont))
  std::static_pointer_cast<UConnector>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetPersonalLinks(cont->GetComponentByIndex(i), cont2, linkslist, netlevel);

 return linkslist;
}
// --------------------------

}

#endif
