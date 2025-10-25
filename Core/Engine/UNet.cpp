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

// Helper method implementations
std::shared_ptr<UNet> UNet::GetThisAsSharedNet() {
    return std::static_pointer_cast<UNet>(
        UComponent::shared_from_this()
    );
}

std::shared_ptr<UItem> UNet::GetThisAsSharedItem() {
    return std::static_pointer_cast<UItem>(
        UComponent::shared_from_this()
    );
}

std::shared_ptr<UConnector> UNet::GetThisAsSharedConnector() {
    return std::static_pointer_cast<UConnector>(
        UComponent::shared_from_this()
    );
}

std::shared_ptr<UContainer> UNet::GetThisAsSharedContainer() {
    return std::static_pointer_cast<UContainer>(
        UComponent::shared_from_this()
    );
}

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
bool UNet::CheckComponentType(std::shared_ptr<UContainer> comp) const
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
bool UNet::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool UNet::ADelComponent(std::shared_ptr<UContainer> comp)
{
 if(!comp->IsMoving())
 {
 if(dynamic_pointer_cast<UItem>(comp))
  static_pointer_cast<UItem>(comp)->DisconnectBy(GetThisAsSharedContainer());
  else
 if(dynamic_pointer_cast<UNet>(comp))
  static_pointer_cast<UNet>(comp)->BreakLinks(GetThisAsSharedContainer());
 }
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
UContainer* UNet::New(void)
{
 return new UNet;
}

// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
bool UNet::Copy(std::shared_ptr<UContainer> target, std::shared_ptr<UStorage> stor, bool copystate) const
{
 ULinksList linkslist;
 ULinksList oldlinkslist;

 if(!dynamic_pointer_cast<UNet>(target))
  return false;

 if(UItem::Copy(target,stor,copystate))
  {
   static_pointer_cast<UNet>(target)->BreakLinks();

   if(static_pointer_cast<UNet>(target)->CreateLinks(GetLinks(linkslist,const_cast<UNet*>(this)->GetThisAsSharedContainer())))
    return true;
  }

 return false;
}

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UNet::Free(void)
{
 UItem::Free();
}
// --------------------------

// ----------------------
// ������ ���������� �������
// ----------------------
bool UNet::CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index, int connector_c_index, bool forced_connect_same_item)
{
 return CreateLink(UStringLinkSide(item,item_index),UStringLinkSide(connector,connector_index, connector_c_index), forced_connect_same_item);
}

// ��������� ��� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
bool UNet::BreakLink(const NameT &itemname, const NameT &connectorname)
{
 std::shared_ptr<UItem> item;
 std::shared_ptr<UConnector> connector;
 if(itemname.size() == 0)
  item=GetThisAsSharedItem();
 else
  item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));

 if(connectorname.size() == 0)
  connector=GetThisAsSharedConnector();
 else
  connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname,true));

 item->Disconnect(connector);

 return true;
}

// ��������� ��� ����� ����� ������� �������� ���� � ������ ������������
bool UNet::BreakAllOutgoingLinks(const NameT &itemname)
{
 std::shared_ptr<UItem> item;
 if(itemname.size() == 0)
  item=GetThisAsSharedItem();
 else
  item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));

 if(!item)
  return false;
 item->DisconnectAll();

 return true;
}

bool UNet::BreakAllOutgoingLinks(const NameT &itemname, const NameT &item_property_name)
{
 std::shared_ptr<UItem> item;
 if(itemname.size() == 0)
  item=GetThisAsSharedItem();
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
void UNet::BreakLinks(std::shared_ptr<UContainer> brklevel)
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

 DisconnectAll();
 DisconnectAllItems();
}
/*
// ��������� ����� �� ����� connector_index ���������� 'connectorid'
void UNet::BreakConnectorLink(const NameT &connectorname, int connector_index)
{
 std::shared_ptr<UItem> connector;
 if(connectorname.size() == 0)
  connector=this;
 else
  connector=dynamic_pointer_cast<UItem>(GetComponentL(connectorname,true));

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

  std::shared_ptr<UItem> ad_item=dynamic_cast<UItem*>(item.Item);

  if(!ad_item)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Dynamic cast failed"));
   return;
  }

  ad_item->Disconnect(connector,item.Index, connector_index);
}

void UNet::BreakConnectorLink(const NameT &connectorname, const NameT &connector_index, int connector_c_index)
{
 std::shared_ptr<UConnector> connector;
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
*/
// ���������, ���������� �� �������� �����
/*bool UNet::CheckLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return CheckLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}*/
/*
bool UNet::CheckLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index)
{
 return CheckLink(UStringLinkSide(itemname,item_index),UStringLinkSide(connectorname,connector_index));
}*/

bool UNet::CheckLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index)
{
 return CheckLink(UStringLinkSide(itemname,item_property_name),UStringLinkSide(connectorname,connector_property_name, connector_c_index));
}

bool UNet::CheckLink(const NameT &itemname,const NameT &connectorname, int connector_c_index)
{
 std::shared_ptr<UItem> item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));
 std::shared_ptr<UConnector> connector=GetComponentL<UConnector>(connectorname,true);
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

bool UNet::SwitchOutputLinks(const UStringLinkSide &item1, const UStringLinkSide &item2)
{
 std::shared_ptr<UItem> pitem1,pitem2;
 if(!CheckLongId(item1.Id))
  pitem1=GetThisAsSharedItem();
 else
  pitem1=dynamic_pointer_cast<UItem>(GetComponentL(item1.Id,true));

 if(!CheckLongId(item2.Id))
  pitem2=GetThisAsSharedItem();
 else
  pitem2=dynamic_pointer_cast<UItem>(GetComponentL(item2.Id,true));

 if(!pitem1)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item 1 not found: ")+item1.Name);
  return false;
 }

 if(!pitem2)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item 2 not found: ")+item2.Name);
  return false;
 }
	  /*
 UIProperty* property1(0);
 UIProperty* property2(0);
 pitem1->FindOutputProperty(item1.Name, property1);
 pitem2->FindOutputProperty(item2.Name, property2);

 UIPropertyOutput* output_property1(dynamic_cast<UIPropertyOutput*>(property1));
 UIPropertyOutput* output_property2(dynamic_cast<UIPropertyOutput*>(property2));

 if(!output_property1)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Property 1 not found: ")+item1.Name);
  return false;
 }

 if(!output_property2)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Property 2 not found: ")+item2.Name);
  return false;
 }
		 */
 int num_connectors1=pitem1->GetNumAConnectors(item1.Name);
 std::vector<std::shared_ptr<UConnector>> conns;
 std::vector<std::string> conn_names;
 conn_names.reserve(num_connectors1);
 conns.reserve(num_connectors1);
 for(int i=0;i<num_connectors1;i++)
 {
  conns.push_back(pitem1->GetAConnectorByIndex(item1.Name, i));
  std::vector<UCLink> buffer;
  conns[i]->GetCLink(dynamic_pointer_cast<UItem>(pitem1), buffer);
  for(size_t j=0;j<buffer.size();j++)
   if(buffer[j].OutputName == item1.Name)
   {
	conn_names.push_back(buffer[j].InputName);
    break;
   }
 }

 pitem1->DisconnectAll(item1.Name);

 bool res(true);
 for(size_t i=0;i<conn_names.size();i++)
 {
  int c_index(-1);
  res &=pitem2->Connect(conns[i], item2.Name, conn_names[i], c_index);
 }

 return res;
}

bool UNet::SwitchOutputLinks(const NameT &itemname1, const NameT &output_name1,
						const NameT &itemname2, const NameT &output_name2)
{
 return SwitchOutputLinks(UStringLinkSide(itemname1,output_name1), UStringLinkSide(itemname2,output_name2));
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
	 GetProperty(I->first,std::shared_ptr<USerStorage>(serstorage));
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 ProcessException(exception);
	}

	std::string paramname=I->first;//I->second.Property->GetName();
    if(serstorage->SelectNode(paramname))
    {
     serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
     serstorage->SetNodeAttribute("IoType",sntoa(I->second.Property->GetIoType()));
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

  std::shared_ptr<UContainerDescription> descr=dynamic_pointer_cast<UContainerDescription>(Storage.lock()->GetClassDescription(Storage.lock()->FindClassName(GetClass()),true));


  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
	try
	{
	 GetProperty(I->first,std::shared_ptr<USerStorage>(serstorage));
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 ProcessException(exception);
	}

	std::string paramname=I->first;//I->second.Property->GetName();
	if(serstorage->SelectNode(paramname))
	{
	 serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
     serstorage->SetNodeAttribute("IoType",sntoa(I->second.Property->GetIoType()));
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
	SetProperty(I->first,std::shared_ptr<USerStorage>(serstorage));
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
  auto storage = Storage.lock(); if(storage)
   serstorage->SetNodeAttribute("Class",/*RDK::sntoa(cont->GetClass())*/storage->FindClassName(GetClass()));

  unsigned int prop_type = 0x1;
  unsigned int prop_group = params_type_mask & 0xFFFF00;
  for(int i = 0; i < 5; i++)
  {
   prop_type = params_type_mask & (0x1 << i);
   if(prop_type)
   {
    serstorage->AddNode(UVariable::GetPropertyTypeNameByType(prop_type));
    if(!GetComponentProperties(serstorage, prop_type | prop_group))
    {
     serstorage->SelectUp();
     return false;
    }
    serstorage->SelectUp();
   }
  }
    /*
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(ptParameter));
  if(!GetComponentProperties(serstorage,params_type_mask))
  {
   serstorage->SelectUp();
   return false;
  }
  serstorage->SelectUp();
  */
  if(links)
  {
   serstorage->AddNode("Links");
   if(GetComponentInternalLinks(serstorage,0))
   {
    serstorage->SelectUp();
    return false;
   }

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

/// ��������� ������ ��������� ����������
bool UNet::SaveComponentStructure(RDK::USerStorageXML *serstorage, bool links, unsigned int type_mask)
{
  if(!serstorage)
   return false;

  auto storage2 = Storage.lock(); if(storage2)
   serstorage->SetNodeAttribute("Class",/*RDK::sntoa(cont->GetClass())*/storage2->FindClassName(GetClass()));

  unsigned int prop_type = 0x1;
  unsigned int prop_group = type_mask & 0xFFFF00;
  for(int i = 0; i < 5; i++)
  {
   prop_type = type_mask & (0x1 << i);
   if(prop_type)
   {
    serstorage->AddNode(UVariable::GetPropertyTypeNameByType(prop_type));
    if(!GetComponentProperties(serstorage, prop_type | prop_group))
    {
     serstorage->SelectUp();
     return false;
    }
    serstorage->SelectUp();
   }
  }
  /*
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(type_mask)); // ptParameter
  if(!GetComponentProperties(serstorage,type_mask))
  {
   serstorage->SelectUp();
   return false;
  }
  serstorage->SelectUp();
  */
  if(links)
  {
   serstorage->AddNode("Links");
   if(GetComponentInternalLinks(serstorage,0))
   {
    serstorage->SelectUp();
    return false;
   }
   serstorage->SelectUp();
  }

  serstorage->AddNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
//   if(!serstorage->AddNode(GetComponentByIndex(i)->GetName()))
//    continue;
   if(!dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->SaveComponent(serstorage,false,type_mask))
   {
	std::string name;
	LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Sub component not found: ")+GetFullName(name));
//	return false;
   }
//   serstorage->SelectUp();
  }
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
  UId id=Storage.lock()->FindClassId(name);

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
  auto storage = Storage.lock(); UStorage* storage_ptr=storage.get();
  for(int i=0;i<serstorage->GetNumNodes();i++)
  {
   serstorage->SelectNode(i);
   std::string nodename=serstorage->GetNodeName();
   name=serstorage->GetNodeAttribute("Class");
   try
   {
    id=Storage.lock()->FindClassId(name);
	std::shared_ptr<UNet> newcont=dynamic_pointer_cast<UNet>(storage->TakeObject(id));
	if(!newcont)
	 continue;
	if(FindStaticComponent(name,nodename) == 0) // ��� �� ��� ������������ ����������� ���������
	{
	 if(AddComponent(newcont) == ForbiddenId)
	 {
	  storage->ReturnObject(newcont);
	  continue;
	 }
	}

	if(!newcont->LoadComponent(serstorage,false))
	{
	 std::string tempname;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("LoadComponent failed: ")+newcont->GetFullName(tempname));
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
  serstorage->SetNodeAttribute("Class",Storage.lock()->FindClassName(GetClass()));
  unsigned int prop_type = 0x1;
  unsigned int prop_group = type_mask & 0xFFFF00;
  for(int i = 0; i < 5; i++)
  {
   prop_type = type_mask & (0x1 << i);
   if(prop_type)
   {
    serstorage->AddNode(UVariable::GetPropertyTypeNameByType(prop_type));
    if(!GetComponentProperties(serstorage, prop_type | prop_group))
    {
     serstorage->SelectUp();
     return false;
    }
    serstorage->SelectUp();
   }
  }
//  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(type_mask));
//  if(!GetComponentProperties(serstorage,type_mask))
//   return false;
//  serstorage->SelectUp();

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
  UId id=Storage.lock()->FindClassId(name);
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
   GetLinks(linkslist, safe_shared_cast<UContainer>(owner_level));
  else
   GetLinks(linkslist, GetThisAsSharedContainer());


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
  CreateLinks(linkslist, safe_shared_cast<UNet>(owner_level));

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
   GetLinks(linkslist, safe_shared_cast<UContainer>(owner_level), true, GetThisAsSharedContainer());
  else
   GetLinks(linkslist, safe_shared_cast<UContainer>(GetOwner().get()), true, GetThisAsSharedContainer());
 return 0;
}

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
bool UNet::SaveComponentDrawInfo(RDK::USerStorageXML *serstorage)
{
  if(!serstorage)
   return false;

  serstorage->AddNode(GetName());
  serstorage->SetNodeAttribute("Class",Storage.lock()->FindClassName(GetClass()));

  serstorage->AddNode("Links");

  UStringLinksList linkslist;

  for(int i=0;i<GetNumComponents();i++)
  {
   std::shared_ptr<UNet> sub_cont=static_pointer_cast<UNet>(GetComponentByIndex(i));
   sub_cont->GetLinks(linkslist, GetThisAsSharedContainer(), true, sub_cont);
  }
  *serstorage<<linkslist;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
   std::shared_ptr<UNet> sub_cont=static_pointer_cast<UNet>(GetComponentByIndex(i));
   serstorage->AddNode(sub_cont->GetName());
   serstorage->SetNodeAttribute("Class",Storage.lock()->FindClassName(sub_cont->GetClass()));
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

