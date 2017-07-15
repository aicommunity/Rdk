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

//class UIPropertyInputBase: public UIPropertyIO
/// ������������ � �����������
UIPropertyInputBase::UIPropertyInputBase(void)
{
}

UIPropertyInputBase::~UIPropertyInputBase(void)
{
}


/// ���������� ��� �������� �����

int UIPropertyInputBase::GetInputType(void) const
{
 return InputType;
}


/// ���������� ��������� �� ���������-��������
const std::vector<UCItem>& UIPropertyInputBase::GetItemsList(void) const
{
 return ItemsList;
}


// ���������� ��������� �� ���������-��������
UNet* UIPropertyInputBase::GetItem(int c_index)
{
 if(ItemsList.empty())
  return 0;

 if(c_index>int(ItemsList.size()))
  return 0;

 return ItemsList[(c_index<0)?0:c_index].Item;
}

// ���������� ���������� �� �������� ������ � ���� item ��� -1, -1
// ���� ����� ����� �����������
void UIPropertyInputBase::GetCLink(const UEPtr<UNet> &item, std::vector<UCLink> &buffer) const
{
 UCLink indexes;
 buffer.clear();

 if(!item)
  return;

  for(size_t i=0;i<ItemsList.size();i++)
   if(ItemsList[i].Item == item)
   {
	UCItem citem;
	citem=ItemsList[i];

	if(citem.Item == 0)
	 continue;

	indexes.InputName=GetName();
	indexes.Output=citem.Index;
	indexes.OutputName=citem.Name;
	buffer.push_back(indexes);
   }
}

void UIPropertyInputBase::GetCLink(const UNet* const item, std::vector<UCLink> &buffer) const
{
 const UEPtr<UNet> uitem=const_cast<UNet*>(item);
 GetCLink(uitem,buffer);
}


/// ���������� ��� ������������� ����������
std::string UIPropertyInputBase::GetItemName(int c_index) const
{
 if(ItemsList.empty())
  return "";

 if(c_index>int(ItemsList.size()))
  return "";

 return ItemsList[(c_index<0)?0:c_index].Item->GetName();
}

/// ���������� ������ ��� ������������� ����������
std::string UIPropertyInputBase::GetItemFullName(int c_index) const
{
 if(ItemsList.empty())
  return "";

 if(c_index>int(ItemsList.size()))
  return "";

 return ItemsList[(c_index<0)?0:c_index].Item->GetFullName();
}

// ���������� ��� ������������� ������
std::string UIPropertyInputBase::GetItemOutputName(int c_index) const
{
 if(ItemsList.empty())
  return "";

 if(c_index>int(ItemsList.size()))
  return "";

 return ItemsList[(c_index<0)?0:c_index].Name;
}

/// ���������� ����� ����������� �� �����
int UIPropertyInputBase::GetNumConnections(void) const
{
 return int(ItemsList.size());
}

/// ���������� true ���� ���� ����� �����������
bool UIPropertyInputBase::IsConnected(void) const
{
 return (ItemsList.empty())?false:true;
}

/// ��������� ����� � ��������� ���� ������������ �� ����� 'item_property_name'
/// ���������� true, ���� �������� �� �������� �����������
bool UIPropertyInputBase::Disconnect(const NameT &item_property_name, int c_index)
{
 if(item_property_name.empty())
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found: ")+item_property_name);
  return false;
 }

 UCItem citem;
 UIPropertyOutput* output_property(0);

 if(c_index>=0 && c_index<int(ItemsList.size()))
 {
  citem=ItemsList[c_index];
 }
 else
 {
  for(size_t i=0;i<ItemsList.size();i++)
  {
   if(ItemsList[i].Name == item_property_name)
   {
	citem=ItemsList[i];
	break; // TODO: ����� ��� ����� �� ����?
   }
  }
 }

 output_property=citem.Item->FindOutputProperty(item_property_name);
 UNet* item=GetItem(c_index);
 if(!item)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Pointer to disconnected property not found: ")+item_property_name);
  return false;
 }

 if(GetInputType() & ipData)
 {
  if(!ResetPointer(0,const_cast<void*>(output_property->GetPointer(0))))
  {
   std::ostringstream stream;
   stream<<"Data ResetPointer fail when disconnect "<<item->GetFullName()<<":"<<item_property_name<<" from "<<GetOwner()->GetFullName()<<":"<<GetName();
   GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
   return false;
  }
 }
 else
 if(GetInputType() & ipComp)
 {
  if(!ResetPointer(0,reinterpret_cast<void*>(item)))
  {
   GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Comp ResetPointer fail"));
   return false;
  }
 }
 else
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("ResetPointer fail"));
  return false;
 }
 return true;
/*
 if(!item_property_name.empty())
 {
  if(index<0 || index >=int(ItemsList.size()))
   return;

   if(ItemsList[index].Name == item_property_name)
   {
	UIProperty* output_property=I->second[index].Item->FindProperty(item_property_name);
	if(output_property)
	{
	 if(GetIoType() & ipData)
	 {
	  if(!ResetPointer(index,const_cast<void*>(output_property->GetPointer(0))))
	  {
	   std::ostringstream stream;
	   stream<<"Data ResetPointer fail when disconnect "<<ItemsList[index].Item->GetFullName()<<":"<<item_property_name<<" from "<<GetOwner()->GetFullName()<<":"<<connector_property_name;
	   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
	  }
	 }
	 else
	 if(GetIoType() & ipComp)
	 {
	  if(!ResetPointer(index,reinterpret_cast<void*>(ItemsList[index].Item)))
	  {
	   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Comp ResetPointer fail"));
	  }

	 }
	 else
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("ResetPointer fail"));
	}
	else
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found"));

//	ADisconnectFromItem(I->second[index].Item,I->second[index].Name,connector_property_name);
	ItemsList.erase(ItemsList.begin()+index);
   }
 }   */
}

/// ��������� ����� � �������� c_index, ��� ��� ����� ���� c_index == -1
/// ���� c_index ����� �� ���������� ��������, �� �� ������ ������
void UIPropertyInputBase::Disconnect(int c_index)
{
 if(c_index<0)
 {
  DisconnectAll();
  return;
 }

 if(c_index>=int(ItemsList.size()))
  return;

// ItemsList[c_index].Item->Disconnect(this); // TODO: ��������
}

/// ��������� ��� ����� �� ���������
void UIPropertyInputBase::DisconnectAll(void)
{
 std::vector<UCItem>::iterator I=ItemsList.begin();
 for(;I != ItemsList.end();++I)
 {
//  I->Item->Disconnect(this); // TODO: ��� ����� ��� ������
 }

/*
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  while(!I->second.empty())
  {
   int index=int(I->second.size())-1;
   I->second[index].Item->Disconnect(this);
  }
 }
 */
}

// ���������, ���������� �� ����� � �������� �����������
bool UIPropertyInputBase::IsConnectedTo(const UEPtr<UNet> &item) const
{
 std::vector<UCLink> buffer;
 GetCLink(item,buffer);
 for(size_t i=0;i<buffer.size();i++)
 {
  UCLink &link=buffer[i];
  if((link.Output>=0 && link.Input >=0) || (!link.InputName.empty() && !link.OutputName.empty()))
   return true;
 }

 return false;
}

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UIPropertyInputBase::IsConnectedTo(const UEPtr<UNet> &item, const NameT &item_property_name, int c_index) const
{
 std::vector<UCLink> buffer;
 GetCLink(item,buffer);
 for(size_t i=0;i<buffer.size();i++)
 {
  UCLink &link=buffer[i];
  if(!link.InputName.empty())
  {
   if(link.OutputName == item_property_name)
	return true;
  }
 }
 return false;
}

/// ���������� �����
bool UIPropertyInputBase::Connect(UNet* item, const std::string &output_name, int &c_index, bool forced_connect_same_item)
{
 UIPropertyOutput* i_item_property=item->FindOutputProperty(output_name);

 if(!i_item_property && !(GetInputType() & ipComp))
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+output_name);
  return false;
 }

 /*
 if(!(GetInputType() & ipComp) && !CheckItem(item, item_property_name))
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Checking fail"));
  return false;
 } */

 if(IsConnected())
 {
  if(!(GetInputType() & ipRange))
  {
   std::ostringstream stream;
   stream<<"Connector already in use ["<<GetItemFullName()<<":"<<GetItemOutputName()<<"] when connecting "<<item->GetFullName()<<":"<<output_name<<" to "<<GetOwner()->GetFullName()<<":"<<GetName();
   GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
   return false;
  }
 }

  for(size_t i=0;i<ItemsList.size();i++)
  {
   if(ItemsList[i].Item == item)
   {
	if(ItemsList[i].Name == output_name)
	{
	 if(c_index == -1)
	  c_index=i;
	 if(!forced_connect_same_item)
	  return true;

//	 if(!(i_conn_property->GetIoType() & ipRange))
//      return true;
	}
   }
  }

// Init(item,output_name); // TODO: ��������

 // TODO: ���� ��� �� ����� �������� � ������, ���� c_index ����� ���������� ��
 // ���� �� �� �����������
 UCItem citem;
 citem.Item=item;
 citem.Index=-1;
 citem.Name=output_name;
 ItemsList.push_back(citem);
 c_index=int(ItemsList.size())-1;

 return true;

 /*

 UIPropertyOutput* i_item_property(0);
 na->FindOutputProperty(item_property_name,i_item_property);
 UIPropertyInput* i_conn_property(0);
 FindInputProperty(connector_property_name,i_conn_property);

 if(!i_item_property && !(i_conn_property && (i_conn_property->GetIoType() & ipComp)))
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item_property_name);
  return false;
 }

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector is empty and AutoNumInputs disabled"));
  return false;
 }

 if(!(i_conn_property->GetIoType() & ipComp) && !CheckItem(na, item_property_name, connector_property_name))
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Checking fail"));
  return false;
 }

 UIPropertyInput* i_conn_property_input=dynamic_cast<UIPropertyInput*>(i_conn_property);
 if(i_conn_property_input && i_conn_property_input->IsConnected())
 {
  if(!(i_conn_property->GetIoType() & ipRange))
  {
   std::ostringstream stream;
   stream<<"Connector already in use ["<<i_conn_property_input->GetItemFullName()<<":"<<i_conn_property_input->GetItemOutputName()<<"] when connecting "<<na->GetFullName()<<":"<<item_property_name<<" to "<<GetFullName()<<":"<<connector_property_name;
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
   return false;
  }
 }

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I != ConnectedItemList.end())
 {
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item == na)
   {
	if(I->second[i].Name == item_property_name)
	{
	 if(c_index == -1)
	  c_index=i;
	 if(!forced_connect_same_item)
	  return true;

//	 if(!(i_conn_property->GetIoType() & ipRange))
//      return true;
	}
   }
  }
 }

 UEPtr<UIPropertyInput> i_conn_property_casted=dynamic_pointer_cast<UIPropertyInput>(FindProperty(connector_property_name));
 if(i_conn_property_casted)
  i_conn_property_casted->Init(na,item_property_name);

 // TODO: ���� ��� �� ����� �������� � ������, ���� c_index ����� ���������� ��
 // ���� �� �� �����������
 UCItem item;
 item.Item=na;
 item.Index=-1;
 item.Name=item_property_name;
 ConnectedItemList[connector_property_name].push_back(item);
 c_index=int(ConnectedItemList[connector_property_name].size())-1;

 return AConnectToItem(na, item_property_name, connector_property_name, c_index, forced_connect_same_item);
 */
}

// ��������� ��� ����� � ��������� ���� 'na'
void UIPropertyInputBase::Disconnect(UEPtr<UNet> na)
{
 std::vector<UCItem>::iterator I=ItemsList.begin();
 for(;I != ItemsList.end();++I)
 {
  int i=0;
  while(i<int(ItemsList.size()))
  {
   if(ItemsList[i].Item == na)
	Disconnect(ItemsList[i].Name,i);
   else
	++i;
  }
 }

/*
������� 2
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na)
	DisconnectFromIndex(I->first, I->second[i].Name,i);
   else
    ++i;
  }
 }        */
}

/// ��������� ����� � ��������� ���� 'na' � ������� 'item_property_name'
void UIPropertyInputBase::Disconnect(UEPtr<UNet> na, const NameT &item_property_name)
{
 std::vector<UCItem>::iterator I=ItemsList.begin();
 for(;I != ItemsList.end();++I)
 {
  int i=0;
  while(i<int(ItemsList.size()))
  {
   if(ItemsList[i].Item == na && ItemsList[i].Name == item_property_name)
	Disconnect(ItemsList[i].Name,i); // TODO ������ �� ���������
   else
    ++i;
  }
 }
/*
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na && I->second[i].Name == item_property_name)
	DisconnectFromIndex(I->first,I->second[i].Name,i); // TODO ������ �� ���������
   else
    ++i;
  }
 }  */
}

/// ��������� ����� � ��������� ���� 'na', ������� 'item_property_name' � ������ 'connector_property_name'
void UIPropertyInputBase::Disconnect(UEPtr<UNet> na, const NameT &item_property_name, const NameT &connector_property_name, int c_index)
{
 if(c_index<0 || c_index>= int(ItemsList.size()))
  return;

 Disconnect(ItemsList[c_index].Name,c_index); // TODO ������ �� ���������

/*
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);

 if(I == ConnectedItemList.end())
  return;

 DisconnectFromIndex(connector_property_name,I->second[connected_c_index].Name,connected_c_index); // TODO ������ �� ���������
 */
}
/* *************************************************************************** */
//class UIPropertyOutputBase: public UIPropertyIO
/// ������������ � �����������
UIPropertyOutputBase::UIPropertyOutputBase(void)
{
}

UIPropertyOutputBase::~UIPropertyOutputBase(void)
{
 Connectors.clear();
 ConnectorInputNames.clear();
}

// ���������� ����� ������������ ������
size_t UIPropertyOutputBase::GetNumConnectors(void) const
{
 return Connectors.size();
}

// ���������� ��������� �� ���������-��������
UNet* UIPropertyOutputBase::GetConnector(int index)
{
 return Connectors[index];
}

// ���������� ��� ������������� ����� ����������-���������
std::string UIPropertyOutputBase::GetConnectorInputName(int index) const
{
 return ConnectorInputNames[index];
}
  /*
// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
bool UIPropertyOutputBase::Disconnect(const UId &id)
{
}   */

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
void UIPropertyOutputBase::DisconnectAll(void)
{
 /*
 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=int(I->second.size())-1;
  while(i>=0)
  {
   Disconnect(I->second[i]);
   i=int(I->second.size())-1;
  }
 }   */
}

// ������������� ����� � ����������� 'c'
bool UIPropertyOutputBase::Connect(UEPtr<UNet> c, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item)
{
/*
 std::vector<PUAConnector> &vec=RelatedConnectors[item_property_name];
 for(size_t i=0;i<vec.size();i++)
  if(vec[i] == c)
   return true;

 vec.push_back(c);
 return true;
  */

 return false;
}

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
void UIPropertyOutputBase::Disconnect(UEPtr<UNet> c)
{
/*
 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i] == c)
	I->second.erase(I->second.begin()+i);
   else
	++i;
  }
 }
 */
}

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
void UIPropertyOutputBase::Disconnect(UEPtr<UNet> c, const NameT &connector_property_name, int c_index)
{
/*
 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.find(item_property_name);

 if(I == RelatedConnectors.end())
  return;

 UCItem citem=c->GetCItem(connector_property_name,this,connected_c_index);
 int i=0;
 while(i<int(I->second.size()))
 {
  if(I->second[i] == c)
  {
   if(citem.Name == item_property_name && citem.Item == this)
   {
	I->second.erase(I->second.begin()+i);
	c->DisconnectFromItem(this, item_property_name, connector_property_name, connected_c_index);
   }
   else
	++i;
  }
  else
   ++i;
 } */
}
							 /*
// ����������  ��������� �� ������ �����������.
UEPtr<UConnector> UIPropertyOutputBase::GetAConnectorByIndex(int c_index) const
{
}
                               */
// ���������, ���������� �� ����� � �������� �����������
bool UIPropertyOutputBase::CheckLink(const UEPtr<UNet> &connector, int c_index) const
{
	 return false;

}

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UIPropertyOutputBase::CheckLink(const UEPtr<UNet> &connector, const NameT &connector_property_name, int c_index) const
{
	 return false;

}

// ����������������� ��� ����� ����� ������ �� ����� connectors
void UIPropertyOutputBase::BuildLinks(void)
{

/*
 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i])
	continue;
   std::vector<UCLink> buffer;
   I->second[i]->GetCLink(this,buffer);
   for(size_t k=0;k<buffer.size();k++)
   {
	UCLink &indexes=buffer[k];
	if(I->first == indexes.OutputName)
	{
	 int c_index(-1);
	 I->second[i]->ConnectToItem(this,indexes.OutputName,indexes.InputName,c_index);
	}
   }
  }
 */

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
bool UNet::CheckComponentType(UEPtr<UContainer> comp) const
{
 return (dynamic_pointer_cast<UNet>(comp))?true:false;
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

 if(UContainer::Copy(target,stor,copystate))
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
 UContainer::Free();
}
// --------------------------

// ----------------------
// ������ ���������� �������
// ----------------------
/*
bool UNet::CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return CreateLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
} */

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
/*bool UNet::CreateLink(const NameT &item, int item_index,
						const NameT &connector, int connector_index, bool forced_connect_same_item)
{
 return CreateLink(UStringLinkSide(item,item_index),UStringLinkSide(connector,connector_index), forced_connect_same_item);
} */

// ������������� ����� ����� 'link'
bool UNet::CreateLink(const ULink &link, bool forced_connect_same_item)
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
bool UNet::CreateLink(const ULinkSide &item, const ULinkSide &connector, bool forced_connect_same_item)
{
 UEPtr<UNet> pitem;
 if(!CheckLongId(item.ComponentName))
  pitem=this;
 else
  pitem=GetComponentL<UNet>(item.ComponentName,true);

 UEPtr<UNet> pconnector=0;
 if(!CheckLongId(connector.ComponentName))
  pconnector=this;
 else
  pconnector=GetComponentL<UNet>(connector.ComponentName,true);

 if(!pitem)
 {
  std::ostringstream stream;
  stream<<"Item not found when connect "<<item.ComponentName<<":"<<item.PropertyName<<" to "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
  return false;
 }

 if(!pconnector)
 {
  std::ostringstream stream;
  stream<<"Connector not found when connect "<<item.ComponentName<<":"<<item.PropertyName<<" to "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());

  return false;
 }

 UIPropertyInputBase *input_property=pconnector->FindInputProperty(connector.PropertyName);
 UIPropertyOutputBase *output_property=pconnector->FindOutputProperty(item.PropertyName);

 if(!output_property)
 {
  std::ostringstream stream;
  stream<<"Item`s output property not found when connect "<<item.ComponentName<<":"<<item.PropertyName<<" to "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
  return false;
 }

 if(!input_property)
 {
  std::ostringstream stream;
  stream<<"Connector`s input property not found when connect "<<item.ComponentName<<":"<<item.PropertyName<<" to "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());

  return false;
 }

 return output_property->Connect(input_property);
}

bool UNet::CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index, int connector_c_index, bool forced_connect_same_item)
{
 return CreateLink(ULinkSide(item,item_index),ULinkSide(connector,connector_index, connector_c_index), forced_connect_same_item);
}

// ������������� ��� ����� �� ������� 'linkslist'.
bool UNet::CreateLinks(const ULinksList &linkslist, UEPtr<UNet> owner_level)
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
bool UNet::BreakLink(const ULink &link)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
  res&=BreakLink(link.Item,link.Connector[i]);

 return res;
}

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
bool UNet::BreakLink(const ULinkSide &item, const ULinkSide &connector)
{
 UEPtr<UItem> pitem=0;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(item.Id,true));

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

 UIPropertyInputBase *input_property=pconnector->FindInputProperty(connector.PropertyName);
 UIPropertyOutputBase *output_property=pconnector->FindOutputProperty(item.PropertyName);

 if(!output_property)
 {
  std::ostringstream stream;
  stream<<"Item`s output property not found when disconnect "<<item.ComponentName<<":"<<item.PropertyName<<" from "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
  return false;
 }

 if(!input_property)
 {
  std::ostringstream stream;
  stream<<"Connector`s input property not found when disconnect "<<item.ComponentName<<":"<<item.PropertyName<<" from "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());

  return false;
 }

 return output_property->Disconnect(input_property);
}

bool UNet::BreakLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index)
{
 return BreakLink(ULinkSide(itemname,item_property_name), ULinkSide(connectorname,connector_property_name, connector_c_index));
}

// ��������� ��� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
bool UNet::BreakLink(const NameT &itemname, const NameT &connectorname)
{
 UEPtr<UItem> pitem=0;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(item.Id,true));

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

 std::vector<UEPtr<UIPropertyInput> > input_properties;
 std::vector<UEPtr<UIPropertyOutput> > output_properties;
 pconnector->FindPropertiesByType<UIPropertyInput>(input_properties);
 pitem->FindPropertiesByType<UIPropertyOutput>(output_properties);
 bool res(true);
 for(size_t i=0;i<output_properties.size();i++)
 {
  for(size_t j=0;j<input_properties.size();j++)
  {
   res&=output_properties[i]->BreakLink(input_properties[j]);
  }
 }

 return res;
}



// ��������� ��� ����� ����� ������� �������� ���� � ������ ������������
bool UNet::BreakOutputLinks(const NameT &itemname)
{
 UEPtr<UNet> pitem=0;
 if(!itemname.empty())
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
  return false;
 }

 std::vector<UEPtr<UIPropertyOutput> > output_properties;
 pitem->FindPropertiesByType<UIPropertyOutput>(output_properties);
 bool res(true);
 for(size_t i=0;i<output_properties.size();i++)
 {
  res&=output_properties[i]->DisconnectAll();
 }


 return res;
}

bool UNet::BreakOutputLinks(const NameT &itemname, const NameT &item_property_name)
{
 UEPtr<UNet> pitem=0;
 if(!itemname.empty())
  pitem=this;
 else
  pitem=GetComponentL<UNet>(itemname,true);

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+itemname);
  return false;
 }

 UIPropertyOutputBase *output_property=pconnector->FindOutputProperty(item_property_name);

 if(!output_property)
 {
  std::ostringstream stream;
  stream<<"Item`s output property not found when disconnect "<<itemname<<":"<<item_property_name;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
  return false;
 }


 return output_property->DisconnectAll();
}

void UNet::BreakInputLinks(const NameT &connectorname, const NameT &connector_property_name, int connector_c_index)
{
 UEPtr<UNet> pconnector=0;
 if(!connectorname.empty())
  pconnector=this;
 else
  pconnector=GetComponentL<UNet>(connectorname,true));

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return false;
 }

 UIPropertyInputBase *input_property=pconnector->FindInputProperty(connector_property_name);

 if(!input_property)
 {
  std::ostringstream stream;
  stream<<"Connector`s input property not found when disconnect all from"<<connectorname<<":"<<connector_property_name;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());

  return false;
 }
 return input_property->DisconnectAll();
}


// ��������� ��� ����� ����
// �������� �� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
/*void UNet::BreakLinks(UEPtr<UContainer> brklevel)
{
 for(int i=0;i<NumComponents;i++)
  {
   if(dynamic_pointer_cast<UItem>(PComponents[i]))
	static_pointer_cast<UItem>(PComponents[i])->DisconnectBy(brklevel);
   else
   if(dynamic_pointer_cast<UNet>(PComponents[i]))
	static_pointer_cast<UNet>(PComponents[i])->BreakLinks(brklevel);
  }
} */

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

 DisconnectAllInputs();
 DisconnectAllOutputs();
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
   /*
   bool UNet::BreakLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return BreakLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}    */


// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
/*bool UNet::BreakLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index)
{
 return BreakLink(ULinkSide(itemname,item_index),ULinkSide(connectorname,connector_index));
} */



 /*
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
}     */

// ���������, ���������� �� �������� �����
bool UNet::IsLinkExists(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index)
{
 return IsLinkExists(ULinkSide(itemname,item_property_name),ULinkSide(connectorname,connector_property_name, connector_c_index));
}
/*
bool UNet::IsLinkExists(const NameT &itemname,const NameT &connectorname)
{
 UEPtr<UNet> item=GetComponentL<UNet>(itemname,true);
 UEPtr<UNet> connector=GetComponentL<UNet>(connectorname,true);
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


 if(item->CheckLink(connector))
  return true;

 return false;
} */
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

  ULinksList linkslist;
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

  ULinksList linkslist;
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

  ULinksList linkslist;
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

  ULinksList linkslist;
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

  ULinksList linkslist;
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

  ULinksList linkslist;

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

// --------------------------
// ������ ������� � ���������
// --------------------------
ULinksList& UNet::GetLinks(ULinksList &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
 GetLinks(const_cast<UNet*>(this), linkslist, netlevel, exclude_internals, internal_level);

 return linkslist;
}

// ��������� ��� ����� ����� ����� ������������ � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
ULinksList& UNet::GetPersonalLinks(UEPtr<RDK::UNet> cont, ULinksList &linkslist, UEPtr<UContainer> netlevel, int sublevel)
{
 GetPersonalLinks(const_cast<UNet*>(this), cont, linkslist, netlevel);

 return linkslist;
}


			  /*
bool UNet::IsLinkExists(const ULinkSide &item, const ULinkSide &connector)
{
 UEPtr<UItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(item.Id,true));

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
   if(pitem->IsLinkExists(pconnector,item.Name, connector.Name, connector.Index))
	return true;
  }
  else
  {
   LogMessageEx(RDK_EX_ERROR, __FUNCTION__, "Check links by index unsupported now!");
//   if(pitem->CheckLink(pconnector,item.Index, connector.Index))
//	return true;
  }

 return false;
}               */
// --------------------------

// --------------------------
// ������� ������ ������� � ���������
// --------------------------
/*
ULinksList& UNet::GetLinks(UEPtr<UContainer> cont, ULinksList &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
 if(dynamic_pointer_cast<UItem>(cont))
 {
  static_pointer_cast<UConnector>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);
  static_pointer_cast<UItem>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);
 }
 else
 if(dynamic_pointer_cast<UConnector>(cont))
  static_pointer_cast<UConnector>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetLinks(cont->GetComponentByIndex(i), linkslist, netlevel, exclude_internals,internal_level);

 return linkslist;
}

ULinksList& UNet::GetPersonalLinks(UEPtr<UContainer> cont, UEPtr<UContainer> cont2, ULinksList &linkslist, UEPtr<UContainer> netlevel) const
{
 if(dynamic_pointer_cast<UItem>(cont))
 {
  static_pointer_cast<UConnector>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);
  static_pointer_cast<UItem>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);
 }
 else
 if(dynamic_pointer_cast<UConnector>(cont))
  static_pointer_cast<UConnector>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetPersonalLinks(cont->GetComponentByIndex(i), cont2, linkslist, netlevel);

 return linkslist;
} */
// --------------------------


}
#endif

