/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UANET_CPP
#define UANET_CPP

#include "UNet.h"
#include "UXMLEnvSerialize.h"

namespace RDK {

/* *************************************************************************** */
// --------------------------
// ������������ � �����������
// --------------------------
UNet::UNet(void)
{
}

UNet::~UNet(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool UNet::CheckComponentType(UEPtr<UContainer> comp) const
{
 return (dynamic_pointer_cast<UItem>(comp) ||
 dynamic_pointer_cast<UNet>(comp) || dynamic_pointer_cast<UConnector>(comp))?true:false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool UNet::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool UNet::ADelComponent(UEPtr<UContainer> comp)
{
 if(!comp->IsMoving())
 {
  if(dynamic_pointer_cast<UItem>(comp))
   static_pointer_cast<UItem>(comp)->DisconnectBy(this);
  else
  if(dynamic_pointer_cast<UNet>(comp))
   static_pointer_cast<UNet>(comp)->BreakLinks(this);
 }
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
bool UNet::Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor, bool copystate) const
{
 ULinksList linkslist;
 ULinksList oldlinkslist;

 if(!dynamic_pointer_cast<UNet>(target))
  return false;

 if(UADItem::Copy(target,stor,copystate))
  {
   static_pointer_cast<UNet>(target)->BreakLinks();

   if(static_pointer_cast<UNet>(target)->CreateLinks(GetLinks(linkslist,const_cast<UNet*>(this))))
    return true;
  }

 return false;
}

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UNet::Free(void)
{
 UADItem::Free();
}
// --------------------------

// ----------------------
// ������ ���������� �������
// ----------------------
// ������������� ����� ����� 'link'
template<typename T>
bool UNet::CreateLink(const ULinkT<T> &link)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
 {
  res &=CreateLink(link.Item, link.Connector[i]);
 }

 return res;
}

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
template<typename T>
bool UNet::CreateLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 UEPtr<UADItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UADItem>(GetComponentL(item.Id,true));

 UEPtr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector.Id,true));

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
  if(!(pitem->Connect(pconnector,item.Name,connector.Name,c_index)))
   return false;
 }
 else
 {
  if(item.Index < 0)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, "Item index < 0");
   return false;
  }

  if(!(pitem->Connect(pconnector,item.Index,connector.Index)))
   return false;
 }

 return true;
}

bool UNet::CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return CreateLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
bool UNet::CreateLink(const NameT &item, int item_index,
						const NameT &connector, int connector_index)
{
 return CreateLink(UStringLinkSide(item,item_index),UStringLinkSide(connector,connector_index));
}

bool UNet::CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index, int connector_c_index)
{
 return CreateLink(UStringLinkSide(item,item_index),UStringLinkSide(connector,connector_index, connector_c_index));
}

// ������������� ��� ����� �� ������� 'linkslist'.
template<typename T>
bool UNet::CreateLinks(const ULinksListT<T> &linkslist, UEPtr<UNet> owner_level)
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
 /*
// ��������� ��� ����� �� ������
template<typename T>
bool UNet::BreakLink(const ULinkSideT<T> &id)
{
 UEPtr<UContainer> pointer=GetComponentL(id.Id,true);

 UEPtr<UConnector> connector=dynamic_pointer_cast<UConnector>(pointer);
 if(connector)
  {
   connector->DisconnectAllItems();
   return true;
  }

 return false;
}
   */
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
 UEPtr<UADItem> pitem=0;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UADItem>(GetComponentL(item.Id,true));

 UEPtr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector.Id,true));

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
 }
 else
 {
  pitem->Disconnect(pconnector, item.Index, connector.Index);
 }

 return true;
}

bool UNet::BreakLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return BreakLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}


// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
bool UNet::BreakLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index)
{
 return BreakLink(UStringLinkSide(itemname,item_index),UStringLinkSide(connectorname,connector_index));
}

// ��������� ��� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
bool UNet::BreakLink(const NameT &itemname, const NameT &connectorname)
{
 UEPtr<UItem> item;
 UEPtr<UConnector> connector;
 if(itemname.size() == 0)
  item=this;
 else
  item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));

 if(connectorname.size() == 0)
  connector=this;
 else
  connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname,true));

 item->Disconnect(connector);

 return true;
}

// ��������� ��� ����� ����� ������� �������� ���� � ������ ������������
bool UNet::BreakAllOutgoingLinks(const NameT &itemname)
{
 UEPtr<UItem> item;
 if(itemname.size() == 0)
  item=this;
 else
  item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));

 if(!item)
  return false;
 item->DisconnectAll();

 return true;
}

bool UNet::BreakAllOutgoingLinks(const NameT &itemname, const NameT &item_property_name)
{
 UEPtr<UItem> item;
 if(itemname.size() == 0)
  item=this;
 else
  item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));

 if(!item)
  return false;
 item->DisconnectAll(item_property_name);

 return true;
}

bool UNet::BreakLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index)
{
 return BreakLink(UStringLinkSide(itemname,item_property_name), UStringLinkSide(connectorname,connector_property_name, connector_c_index));
}

// ��������� ��� ����� ����
// �������� �� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UNet::BreakLinks(UEPtr<UContainer> brklevel)
{
 for(int i=0;i<NumComponents;i++)
  {
   if(dynamic_pointer_cast<UItem>(PComponents[i]))
	static_pointer_cast<UItem>(PComponents[i])->DisconnectBy(brklevel);
   else
   if(dynamic_pointer_cast<UNet>(PComponents[i]))
    static_pointer_cast<UNet>(PComponents[i])->BreakLinks(brklevel);
  }
}

// ��������� �������� ����� ����
bool UNet::BreakLinks(const ULinksList &linkslist)
{
 bool res=true;

 for(int i=0;i<linkslist.GetSize();i++)
  res&=BreakLink(linkslist[i]);

 return res;
}

// ��������� ��� ���������� ����� ����.
void UNet::BreakLinks(void)
{
 for(int i=0;i<NumComponents;i++)
  if(dynamic_pointer_cast<UNet>(PComponents[i]))
   static_pointer_cast<UNet>(PComponents[i])->BreakLinks();
  else
  {
   if(dynamic_pointer_cast<UItem>(PComponents[i]))
	static_pointer_cast<UItem>(PComponents[i])->DisconnectAll();
   if(dynamic_pointer_cast<UConnector>(PComponents[i]))
	static_pointer_cast<UConnector>(PComponents[i])->DisconnectAllItems();
  }

 if(dynamic_cast<UItem* const>(this))
  ((UItem* const)this)->DisconnectAll();
 if(dynamic_cast<UConnector* const>(this))
  ((UConnector* const)this)->DisconnectAllItems();
}

// ��������� ����� �� ����� connector_index ���������� 'connectorid'
void UNet::BreakConnectorLink(const NameT &connectorname, int connector_index)
{
 UEPtr<UADItem> connector;
 if(connectorname.size() == 0)
  connector=this;
 else
  connector=dynamic_pointer_cast<UADItem>(GetComponentL(connectorname,true));

 if(!connector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return;
 }

 if(connector->GetNumInputs() <= connector_index)
  return;

 const UCItem &item = connector->GetCItem(connector_index);

  if(!item.Item)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
   return;
  }

  UEPtr<UADItem> ad_item=dynamic_cast<UADItem*>(item.Item);

  if(!ad_item)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Dynamic cast failed"));
   return;
  }

  ad_item->Disconnect(connector,item.Index, connector_index);
}

void UNet::BreakConnectorLink(const NameT &connectorname, const NameT &connector_index, int connector_c_index)
{
 UEPtr<UConnector> connector;
 if(connectorname.size() == 0)
  connector=this;
 else
  connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname,true));

 if(!connector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return;
 }

 std::vector<UCItem> buffer;
 connector->GetCItem(connector_index, buffer);

 for(size_t i=0;i<buffer.size();i++)
 {
  if(!buffer[i].Item)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+buffer[i].Name);
   return;
  }

  buffer[i].Item->Disconnect(connector,buffer[i].Name, connector_index, connector_c_index);
 }
}

// ���������, ���������� �� �������� �����
bool UNet::CheckLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return CheckLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}

template<typename T>
bool UNet::CheckLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 UEPtr<UADItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UADItem>(GetComponentL(item.Id,true));

 UEPtr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector.Id,true));

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
  else
  {
   if(pitem->CheckLink(pconnector,item.Index, connector.Index))
	return true;
  }

 return false;
}

bool UNet::CheckLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index)
{
 return CheckLink(UStringLinkSide(itemname,item_index),UStringLinkSide(connectorname,connector_index));
}

bool UNet::CheckLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index)
{
 return CheckLink(UStringLinkSide(itemname,item_property_name),UStringLinkSide(connectorname,connector_property_name, connector_c_index));
}

bool UNet::CheckLink(const NameT &itemname,const NameT &connectorname, int connector_c_index)
{
 UEPtr<UItem> item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));
 UEPtr<UConnector> connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname,true));
 if(!item)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+itemname);
  return false;
 }

 if(!connector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return false;
 }


 if(item->CheckLink(connector,connector_c_index))
  return true;

 return false;
}
// ----------------------


// --------------------------
// ������ ������������ ���������
// --------------------------
// ���������� �������� ���������� �� ��������������
bool UNet::GetComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
  if(!serstorage)
   return false;

  const RDK::UContainer::VariableMapT &props=GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
	try
	{
	 GetProperty(I->first,serstorage);
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 ProcessException(exception);
	}

	std::string paramname=I->first;//I->second.Property->GetName();
    if(serstorage->SelectNode(paramname))
    {
     serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
     serstorage->SelectUp();
	}
   }
   ++I;
  }

 return true;
}

// ���������� ���������� �������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
bool UNet::GetComponentSelectedProperties(RDK::USerStorageXML *serstorage)
{
  if(!serstorage)
   return false;

 return true;
}

// ���������� �������� ���������� �� �������������� � ����������
// ������ ��� buffer ������ ���� ��������!
bool UNet::GetComponentPropertiesEx(RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
  if(!serstorage)
   return false;

  const RDK::UContainer::VariableMapT &props=GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  UEPtr<UContainerDescription> descr=dynamic_pointer_cast<UContainerDescription>(Storage->GetClassDescription(Storage->FindClassName(GetClass())));

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
	try
	{
	 GetProperty(I->first,serstorage);
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 ProcessException(exception);
	}

	std::string paramname=I->first;//I->second.Property->GetName();
	if(serstorage->SelectNode(paramname))
	{
	 serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
	 if(descr)
	 {
	  serstorage->SetNodeAttribute("Header",descr->GetPropertyDescription(paramname).Header);
	 }
	 serstorage->SelectUp();
	}
   }
   ++I;
  }

 return true;
}


// ������������� �������� ���������� �� ��������������
int UNet::SetComponentProperties(RDK::USerStorageXML *serstorage)
{
  if(!serstorage)
   return 1;

  const RDK::UContainer::VariableMapT &props=GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   try
   {
	SetProperty(I->first,serstorage);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	std::string value("<ZeroPtr>");
	if(serstorage)
	{
	 value=serstorage->GetNodeText();
	 if(value.empty())
	  serstorage->SaveFromNode(value);
	}

	LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Error set property '")+GetFullName()+std::string(":")+I->first+std::string("' to ")+value+std::string(". Reason: ")+exception.what());
   }
   catch(UException &exception)
   {
	std::string value("<ZeroPtr>");
	if(serstorage)
	{
	 value=serstorage->GetNodeText();
	 if(value.empty())
	  serstorage->SaveFromNode(value);
	}

	LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Error set property '")+GetFullName()+std::string(":")+I->first+std::string("' to ")+value+std::string(". Reason: ")+exception.what());
   }
   catch(std::exception &exception)
   {
	std::string value("<ZeroPtr>");
	if(serstorage)
	{
	 value=serstorage->GetNodeText();
	 if(value.empty())
	  serstorage->SaveFromNode(value);
	}

	LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Error set property '")+GetFullName()+std::string(":")+I->first+std::string("' to ")+value+std::string(". Reason: ")+exception.what());
   }
   catch(...)
   {
	std::string value("<ZeroPtr>");
	if(serstorage)
	{
	 value=serstorage->GetNodeText();
	 if(value.empty())
	  serstorage->SaveFromNode(value);
	}

	LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Error set property '")+GetFullName()+std::string(":")+I->first+std::string("' to ")+value+std::string(". Reason: Undefined exception"));
   }
   ++I;
  }
 return 0;
}


// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
bool UNet::SaveComponent(RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask)
{
  if(!serstorage)
   return false;

  serstorage->AddNode(GetName());
  serstorage->SetNodeAttribute("Class",/*RDK::sntoa(cont->GetClass())*/Storage->FindClassName(GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(ptParameter));
  if(!GetComponentProperties(serstorage,params_type_mask))
   return false;
  serstorage->SelectUp();

  if(links)
  {
   serstorage->AddNode("Links");
   if(GetComponentInternalLinks(serstorage,0))
    return false;
   serstorage->SelectUp();
  }

  serstorage->AddNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
   if(!dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->SaveComponent(serstorage,false,params_type_mask))
   {
	std::string name;
	LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Sub component not found: ")+GetFullName(name));
//	return false;
   }
  }
  serstorage->SelectUp();

  serstorage->SelectUp();

 return true;
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
bool UNet::LoadComponent(RDK::USerStorageXML *serstorage, bool links)
{
  if(!serstorage)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);

  if(GetClass() != id)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Wrong class id: expected ")+sntoa(GetClass())+std::string(" found ")+sntoa(id));
   return false;
  }

  SetName(serstorage->GetNodeName());

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	try
	{
	 if(SetComponentProperties(serstorage))
	 {
	  std::string name;
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("SetComponentProperties failed: ")+GetFullName(name));
//	  return false;
	 }
	}
	catch(UException &exception)
	{
	 ProcessException(exception);
    }
	serstorage->SelectUp();
   }
  }

  DelAllComponents();

  if(!serstorage->SelectNode("Components"))
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Components section not found"));
   return false;
  }
  UStorage* storage=GetStorage();
  for(int i=0;i<serstorage->GetNumNodes();i++)
  {
   serstorage->SelectNode(i);
   std::string nodename=serstorage->GetNodeName();
   name=serstorage->GetNodeAttribute("Class");
   try
   {
    id=Storage->FindClassId(name);
	UEPtr<UNet> newcont=dynamic_pointer_cast<UNet>(storage->TakeObject(id));
	if(!newcont)
	 continue;
	if(AddComponent(static_pointer_cast<UContainer>(newcont)) == ForbiddenId)
	 continue;

	if(!newcont->LoadComponent(serstorage,false))
	{
	 std::string name;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("LoadComponent failed: ")+newcont->GetFullName(name));
//	 return false;
    }
   }
   catch(UException &exception)
   {
	ProcessException(exception);
   }
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  if(links)
  {
   serstorage->SelectNode("Links");
   if(!SetComponentInternalLinks(serstorage,0))
	return false;
   serstorage->SelectUp();
  }

 return true;
}


// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
bool UNet::SaveComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
  if(!serstorage)
   return false;

  serstorage->AddNode(GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(type_mask));
  if(!GetComponentProperties(serstorage,type_mask))
   return false;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
   try
   {
	if(!dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->SaveComponentProperties(serstorage,type_mask))
	{
	 std::string name;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("SaveComponentProperties failed: ")+GetFullName(name));
//	 return false;
	}
   }
   catch (UException &exception)
   {
	ProcessException(exception);
   }
  }
  serstorage->SelectUp();

  serstorage->SelectUp();

 return true;
}

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
bool UNet::LoadComponentProperties(RDK::USerStorageXML *serstorage)
{
  if(!serstorage)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);
  if(GetClass() != id)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Wrong class id: expected ")+sntoa(GetClass())+std::string(" found ")+sntoa(id));
   return false;
  }

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	try
	{
	 if(SetComponentProperties(serstorage))
	 {
	  std::string name;
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("SetComponentProperties failed: ")+GetFullName(name));
//	  return false;
	 }
	}
	catch (UException &exception)
	{
	 ProcessException(exception);
    }
    serstorage->SelectUp();
   }
  }

  serstorage->SelectNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
   if(!serstorage->SelectNode(GetComponentByIndex(i)->GetName()))
	continue;
   std::string nodename=serstorage->GetNodeName();
   try
   {
	if(!dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->LoadComponentProperties(serstorage))
	{
	 std::string name;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("LoadComponentProperties failed: ")+GetFullName(name));
//	 return false;
	}
   }
   catch (UException &exception)
   {
	ProcessException(exception);
   }
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

 return true;
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
void UNet::SetGlobalComponentPropertyValue(UId classid, const char *paramname, const char *buffer)
{
  if(classid == ForbiddenId)
   return;

  if(GetClass() == classid)
  {
   try
   {
	SetPropertyValue(paramname,buffer);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	ProcessException(exception);
   }
  }

  for(int i=0;i<GetNumComponents();i++)
  {
   dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->SetGlobalComponentPropertyValue(classid, paramname, buffer);
  }
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
void UNet::SetGlobalOwnerComponentPropertyValue(UId classid, UId owner_classid, const char *paramname, const char *buffer)
{
  if(classid == ForbiddenId)
   return;

  if(GetClass() == classid && GetOwner() && GetOwner()->GetClass() == owner_classid)
  {
   try
   {
	SetPropertyValue(paramname,buffer);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	ProcessException(exception);
   }
  }

  for(int i=0;i<GetNumComponents();i++)
  {
   dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->SetGlobalOwnerComponentPropertyValue(classid, owner_classid, paramname, buffer);
  }
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UNet::GetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
  if(!serstorage)
   return 1;

  UStringLinksList linkslist;
  if(owner_level)
   GetLinks(linkslist, owner_level);
  else
   GetLinks(linkslist, this);


  *serstorage<<linkslist;
 return 0;
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UNet::SetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
  if(!serstorage)
   return 1;

  UStringLinksList linkslist;
  *serstorage>>linkslist;

  BreakLinks();
  CreateLinks(linkslist, owner_level);

 return true;
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UNet::GetComponentInputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel)
{
  if(!serstorage)
   return 1;

  UStringLinksList linkslist;
//  cont->GetInputLinks(linkslist, cont);

  *serstorage<<linkslist;
 return 0;
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UNet::GetComponentOutputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel)
{
  if(!serstorage)
   return 1;

  UStringLinksList linkslist;
//  cont->GetOutputLinks(linkslist, cont);

  *serstorage<<linkslist;
 return 0;
}

// ��������� ��� ������� ����� c ����������� � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
int UNet::GetComponentPersonalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
  if(!serstorage)
   return 1;

  UStringLinksList linkslist;
  if(owner_level)
   GetLinks(linkslist, owner_level, true, this);
  else
   GetLinks(linkslist, GetOwner(), true, this);

  *serstorage<<linkslist;
 return 0;
}

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
bool UNet::SaveComponentDrawInfo(RDK::USerStorageXML *serstorage)
{
  if(!serstorage)
   return false;

  serstorage->AddNode(GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(GetClass()));

  serstorage->AddNode("Links");

  UStringLinksList linkslist;

  for(int i=0;i<GetNumComponents();i++)
  {
   UEPtr<UNet> sub_cont=static_pointer_cast<UNet>(GetComponentByIndex(i));
   sub_cont->GetLinks(linkslist, this, true, sub_cont);
  }
  *serstorage<<linkslist;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
   UEPtr<UNet> sub_cont=static_pointer_cast<UNet>(GetComponentByIndex(i));
   serstorage->AddNode(sub_cont->GetName());
   serstorage->SetNodeAttribute("Class",Storage->FindClassName(sub_cont->GetClass()));
   serstorage->AddNode("Parameters");
   try
   {
	if(!sub_cont->GetComponentProperties(serstorage,ptPubParameter))
	 return false;
   }
   catch (UException &exception)
   {
	ProcessException(exception);
   }
   serstorage->SelectUp();
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  serstorage->SelectUp();

 return true;
}
// --------------------------


}
#endif

