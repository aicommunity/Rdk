/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UACONNECTOR_CPP
#define UACONNECTOR_CPP

#include <string.h>
#include "UConnector.h"
#include "UStorage.h"
#include "UItem.h"

namespace RDK {

/* *************************************************************************** */
/* *************************************************************************** */

/* *************************************************************************** */
// �������� �������� ����� � �������� item "UConnectedLINK"
//struct UCLink: public UCItem
// --------------------------
// ������������ � �����������
// --------------------------
UCLink::UCLink(void)
{
 // ������ �����
 Input=Output=-1;
}

UCLink::UCLink(const UCLink &copy)
{
 Input=copy.Input;
 Output=copy.Output;
 InputName=copy.InputName;
 OutputName=copy.OutputName;
}
// --------------------------
/* *************************************************************************** */

/* *************************************************************************** */
/*
// ��������� - ������ ���������� �� ������������ ��������
// class UCItemList
// --------------------------
// ������������ � �����������
// --------------------------
UCItemList::UCItemList(void)
{
 // ������ ����������
 Size=0;

 // ������ ���������
 Data=0;

 // �������� ������ ����������
 RealSize=0;
}

UCItemList::UCItemList(const UCItemList &copy)
{
 *this=copy;
}


UCItemList::~UCItemList(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void UCItemList::Clear(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}

// ������ ������ ���������� � ����������� ���������
void UCItemList::Resize(int newsize)
{
 if(Size == newsize)
  return;

 if(RealSize<newsize || !Data)
 {
  UCItem* newbuffer=0;
  if(newsize)
  {
   newbuffer=new UCItem[newsize];
   for(int i=0;i<Size;i++)
	newbuffer[i]=Data[i];
//   memcpy(newbuffer,Data,sizeof(UCItem)*Size);
  }

  if(Data)
   delete []Data;
  Data=newbuffer;
  RealSize=newsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ��������
UCItem UCItemList::Find(const UEPtr<UItem> &item, int index) const
{
 return Find(item.Get(),index);
}

UCItem UCItemList::Find(const UItem *const item, int index) const
{
 UCItem *pdata=Data;

 for(int i=index;i<Size;i++,pdata++)
  if(pdata->Item == item)
   return *pdata;

 return UCItem();
}

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ������ ��� -1 ���� ������� �� ������
int UCItemList::Find(const UCItem &item, int index) const
{
 UCItem *pdata=Data;

 for(int i=index;i<Size;i++,pdata++)
  if(*pdata == item)
   return i;

 return -1;
}

// ��������� � ����� ���������� �������
void UCItemList::Add(const UCItem &item)
{
 Resize(Size+1);
 Data[Size-1]=item;
}

// ��������� � ����� ���������� ������� ������ ���� ����� ��������� ��� ���
// ���������� true � ������ ��������� ����������
bool UCItemList::AddUnique(const UCItem &item)
{
 if(Find(item)>=0)
  return false;

 Add(item);
 return true;
}

// ������� ������� �� �������
void UCItemList::Del(int index)
{
 if(index<0 || index >= Size)
  return;

 memcpy(Data+index,Data+index+1,(Size-1)*sizeof(UCItem));
 Resize(Size-1);
}

// ������� ������� �� ���������
void UCItemList::Del(UCItem *item)
{
 int index=Find(*item);
 Del(index);
}
// --------------------------

// --------------------------
// ������ �������
// --------------------------
// �������� ������������
UCItemList& UCItemList::operator = (const UCItemList &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Data[i]=copy.Data[i];

 return *this;
}

// �������� ������� � ��������
UCItem& UCItemList::operator [] (int index)
{
 return Data[index];
}

const UCItem& UCItemList::operator [] (int index) const
{
 return Data[index];
}

// �������� ������� � �������
UCItem* UCItemList::GetData(void)
{
 return Data;
}

// �������� ������� � �������
int UCItemList::GetSize(void) const
{
 return Size;
}
// --------------------------
*/
/* *************************************************************************** */

/* *************************************************************************** */
// --------------------------
// ������������ � �����������
// --------------------------
UConnector::UConnector(void)
{
 DummyItem.Index=-1;
 DummyItem.Item=0;
}

UConnector::~UConnector(void)
{
 DisconnectAllItems();
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
/*/// ���������� ����� ������� ������������ ���������
int UConnector::GetNumActiveInputs(void) const
{
 return int(ConnectedItemList.size());
} */

/// ���������� ����� ������� ������������ ��������� � ��������� ��������
int UConnector::GetNumActiveInputs(const NameT &connector_property_name) const
{
 UIPropertyInput* property(0);
 FindInputProperty(connector_property_name, property);
 if(!property)
  return 0;

 return property->GetNumConnections();
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UConnector::Free(void)
{
 DisconnectAllItems();
 UContainer::Free();
}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ���������� ������ �������-������
void UConnector::ReadInputPropertiesList(std::vector<UEPtr<UIPropertyInput> > &buffer) const
{
 buffer.clear();
 VariableMapCIteratorT I=PropertiesLookupTable.begin();

 for(;I != PropertiesLookupTable.end();++I)
 {
  if(I->second.Type & ptInput)
   buffer.push_back(dynamic_cast<UIPropertyInput*>(I->second.Property.Get()));
 }
}

/// ���������� ����� ������
int UConnector::GetNumInputs(void) const
{
 int res(0);
 VariableMapCIteratorT I=PropertiesLookupTable.begin();

 for(;I != PropertiesLookupTable.end();++I)
 {
  if(I->second.Type & ptInput)
   ++res;
 }
 return res;
}


/*
// ���������� ������������ � ����� ���������� ������ �� �������
const UCItem& UConnector::GetCItem(const NameT &connector_property_name, int index) const
{
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return DummyItem;

 if(index>=int(I->second.size()))
  return DummyItem;
 return I->second[index];
}

const UCItem& UConnector::GetCItem(const NameT &connector_property_name, const UEPtr<UItem> &item, int &index) const
{
 index=-1;
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return DummyItem;

 for(size_t i=0;i<I->second.size();i++)
  if(I->second[i].Item == item)
  {
   index = i;
   return I->second[i];
  }

 return DummyItem;
}

void UConnector::GetCItem(const NameT &connector_property_name, std::vector<UCItem> &buffer) const
{
 buffer.clear();
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I != ConnectedItemList.end())
  buffer=I->second;
}
          */

// ���������� ���������� �� �������� ������ � ���� item ��� -1, -1
// ���� ����� ����� �����������
void UConnector::GetCLink(const UEPtr<UItem> &item, std::vector<UCLink> &buffer) const
{
 UCLink indexes;
 buffer.clear();

 if(!item)
  return;

 std::vector<UEPtr<UIPropertyInput> > properties;
 ReadInputPropertiesList(properties);
 for(size_t j=0;j<properties.size();j++)
 {
  const std::vector<UCItem>& items=properties[j]->GetItemsList();
  for(size_t i=0;i<items.size();i++)
   if(items[i].Item == item)
   {
    UCItem citem;
	citem=items[i];

	if(citem.Item == 0)
	 continue;

	indexes.InputName=properties[i]->GetName();
	indexes.Output=citem.Index;
	indexes.OutputName=citem.Name;
	buffer.push_back(indexes);
   }
 }
/*
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 UCItem citem;
 for(;I != ConnectedItemList.end();++I)
 {
  for(size_t i=0;i<I->second.size();i++)
   if(I->second[i].Item == item)
   {
	citem=I->second[i];

	if(citem.Item == 0)
	 continue;

	UIProperty* property=0;
	FindInputProperty(I->first, property);
	if(property)
	{
	 indexes.InputName=property->GetName();
	}
	indexes.Output=citem.Index;
	indexes.OutputName=citem.Name;
	buffer.push_back(indexes);
   }
 }  */
}

void UConnector::GetCLink(const UItem* const item, std::vector<UCLink> &buffer) const
{
 const UEPtr<UItem> uitem=const_cast<UItem*>(item);
 GetCLink(uitem,buffer);
}
// --------------------------

// --------------------------
// ������ ������� � �������� ������ � �������
// --------------------------
/// ���� ��������-���� �� ��������� �������
void UConnector::FindInputProperty(const NameT &connector_property_name, UIPropertyInput* &property) const
{
 // ���� ��������� �� ������� ������
 property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.find(connector_property_name);
// // TODO: ������� ��������� ������
// VariableMapCIteratorT I=PropertiesLookupTable.end();
// if(CheckAlias(connector_property_name))
//  I=PropertiesLookupTable.find(GetPropertyNameByAlias(connector_property_name));
// else
//  I=PropertiesLookupTable.find(connector_property_name);

 if(I == PropertiesLookupTable.end())
  return;

 if(I->second.Type & ptInput)
 {
  property=dynamic_cast<UIPropertyInput*>(I->second.Property.Get());
 }
}
// --------------------------

// ----------------------
// ���������������� ������
// ----------------------
// ������������� ����� � ��������� ���� 'na'.
bool UConnector::ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item)
{
 if(!na)
  return false;

 if(!Build())
  return false;

 UIPropertyInput* i_conn_property(0);
 FindInputProperty(connector_property_name,i_conn_property);

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector_property_name);
  return false;
 }

 if(i_conn_property->Connect(na, item_property_name, c_index))
  return AConnectToItem(na, item_property_name, connector_property_name, c_index, forced_connect_same_item);

 return false;

/*
 if(!(i_conn_property->GetIoType() & ipComp) && !CheckItem(na, item_property_name, connector_property_name))
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Checking fail"));
  return false;
 }*/

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

/// ��������� ��� ����� � ��������� ���� 'na'
void UConnector::DisconnectFromItem(UEPtr<UItem> na)
{
 if(!na)
  return;

 std::vector<UEPtr<UIPropertyInput> > properties;
 ReadInputPropertiesList(properties);
 for(size_t j=0;j<properties.size();j++)
 {
  properties[j]->Disconnect(na);
 }

/*
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

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index
void UConnector::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name)
{
 if(!na)
  return;

 std::vector<UEPtr<UIPropertyInput> > properties;
 ReadInputPropertiesList(properties);
 for(size_t j=0;j<properties.size();j++)
 {
  properties[j]->Disconnect(na, item_property_name);
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

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index � c_index
void UConnector::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index)
{
 if(!na)
  return;

 std::vector<UEPtr<UIPropertyInput> > properties;
 ReadInputPropertiesList(properties);
 for(size_t j=0;j<properties.size();j++)
 {
  properties[j]->Disconnect(na, item_property_name, connector_property_name, connected_c_index);
 }

/*
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);

 if(I == ConnectedItemList.end())
  return;

 DisconnectFromIndex(connector_property_name,I->second[connected_c_index].Name,connected_c_index); // TODO ������ �� ���������
 */
}
         /*
// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
void UConnector::DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index)
{
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return;

 if(item_property_name.empty())
 {
  DisconnectFromIndex(connector_property_name);
 }
 else
 {
  if(index<0 || index >=int(I->second.size()))
   return;

   if(I->second[index].Name == item_property_name)
   {
	UIPropertyInput* i_conn_property=0;
	FindInputProperty(connector_property_name,i_conn_property);

	UEPtr<UIPropertyInput> i_conn_property_casted=dynamic_pointer_cast<UIPropertyInput>(FindProperty(connector_property_name));
	if(i_conn_property_casted)
	 i_conn_property_casted->UnInit();

	//	if(i_conn_property->CheckRange(index)) // TODO: ��� �������� ���-�� ������
	UIProperty* output_property=I->second[index].Item->FindProperty(item_property_name);
	if(output_property)
	{
	 if(i_conn_property->GetIoType() & ipData)
	 {
	  if(!i_conn_property->ResetPointer(index,const_cast<void*>(output_property->GetPointer(0))))
	  {
	   std::ostringstream stream;
	   stream<<"Data ResetPointer fail when disconnect "<<I->second[index].Item->GetFullName()<<":"<<item_property_name<<" from "<<GetFullName()<<":"<<connector_property_name;
	   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
	  }
	 }
	 else
	 if(i_conn_property->GetIoType() & ipComp)
	 {
	  if(!i_conn_property->ResetPointer(index,reinterpret_cast<void*>(I->second[index].Item)))
	  {
	   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Comp ResetPointer fail"));
	  }

	 }
	 else
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("ResetPointer fail"));
	}
	else
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found"));

	ADisconnectFromItem(I->second[index].Item,I->second[index].Name,connector_property_name);
	I->second.erase(I->second.begin()+index);
   }
 }
}

void UConnector::DisconnectFromIndex(const NameT &connector_property_name)
{
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return;

  int i=0;
  while(i<int(I->second.size()))
  {
	UIProperty* i_conn_property=0;
	FindInputProperty(connector_property_name,i_conn_property);
//	if(i_conn_property->CheckRange(i)) // TODO: ��������� ��� ��� �� ������
	UIPropertyInput* input_prop=dynamic_cast<UIPropertyInput*>(i_conn_property);
	if(input_prop)
	{
	 input_prop->UnInit();
	}

	UIProperty* output_property=I->second[i].Item->FindProperty(I->second[i].Name);
	if(output_property)
	{
	 if(!i_conn_property->ResetPointer(i,const_cast<void*>(output_property->GetPointer(0))))
	 {
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("ResetPointer fail"));
	 }
	}
	else
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found"));

	ADisconnectFromItem(I->second[i].Item,I->second[i].Name,connector_property_name);
	I->second.erase(I->second.begin()+i);
  }
}
          */

// ��������� �������� ����� ��������� ������������ �����
bool UConnector::AConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item)
{
 return true;
}

// ��������� �������� ����� ��������� ���������� �����
void UConnector::ADisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int c_index)
{
}

// ��������� ��� ������� �����
void UConnector::DisconnectAllItems(void)
{
 std::vector<UEPtr<UIPropertyInput> > properties;
 ReadInputPropertiesList(properties);
 for(size_t j=0;j<properties.size();j++)
 {
  properties[j]->DisconnectAll();
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

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UConnector::DisconnectByObject(UEPtr<UContainer> brklevel)
{
 std::vector<UEPtr<UIPropertyInput> > properties;
 ReadInputPropertiesList(properties);
 for(size_t j=0;j<properties.size();j++)
 {
  size_t i=0;
  while(i<properties[j]->GetItemsList().size())
  {
   const std::vector<UCItem>& items=properties[j]->GetItemsList();
   if(!items[i].Item->CheckOwner(brklevel))
	items[i].Item->Disconnect(this);
   else
	++i;
  }
 }
/*
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  size_t i=0;
  while(i<I->second.size())
  {
   if(!I->second[i].Item->CheckOwner(brklevel))
	I->second[i].Item->Disconnect(this);
   else
	++i;
  }
 }*/
}

// ���������, ��������� �� ����������� ��������� item � ����� ����������
bool UConnector::CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name)
{
 return true;
}


// ���������, ���������� �� ����� � �������� �����������
bool UConnector::CheckLink(const UEPtr<UItem> &item) const
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
bool UConnector::CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name) const
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

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UConnector::CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name, const NameT &connector_property_name) const
{
 std::vector<UCLink> buffer;
 GetCLink(item,buffer);
 for(size_t i=0;i<buffer.size();i++)
 {
  UCLink &link=buffer[i];
  if(!link.OutputName.empty())
  {
   if(link.InputName == connector_property_name)
	return true;
  }
 }

 return false;
}
// ----------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UConnector::Default(void)
{
 return UContainer::Default();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UConnector::Build(void)
{
 return UContainer::Build();
}
// --------------------------

/* *************************************************************************** */

//class UIPropertyInputBase: public UIPropertyIO
/// ������������ � �����������
UIPropertyInputBase::UIPropertyInputBase(void)
{
}

UIPropertyInputBase::~UIPropertyInputBase(void)
{
}

/// ���������� ��������� �� ���������-��������
const std::vector<UCItem>& UIPropertyInputBase::GetItemsList(void) const
{
 return ItemsList;
}

// ���������� ��������� �� ���������-��������
UItem* UIPropertyInputBase::GetItem(int c_index)
{
 if(ItemsList.empty())
  return 0;

 if(c_index>int(ItemsList.size()))
  return 0;

 return ItemsList[(c_index<0)?0:c_index].Item;
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
}

/// ��������� ��� ����� �� ���������
void UIPropertyInputBase::DisconnectAll(void)
{
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
bool UIPropertyInputBase::CheckLink(const UEPtr<UItem> &item) const
{
}

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UIPropertyInputBase::CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name, int c_index) const
{
}

/// ���������� �����
bool UIPropertyInputBase::Connect(UItem* item, const std::string &output_name, int c_index, bool forced_connect_same_item)
{
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
void UIPropertyInputBase::Disconnect(UEPtr<UItem> na)
{
/*
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
void UIPropertyInputBase::Disconnect(UEPtr<UItem> na, const NameT &item_property_name)
{

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
void UIPropertyInputBase::Disconnect(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int c_index)
{

/*
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);

 if(I == ConnectedItemList.end())
  return;

 DisconnectFromIndex(connector_property_name,I->second[connected_c_index].Name,connected_c_index); // TODO ������ �� ���������
 */
}

/*
/// ���������� true ���� ���� ����� �����������
bool UIPropertyInputBase::IsConnected(void) const
{
 return (Item)?true:false;
} */




}
#endif

