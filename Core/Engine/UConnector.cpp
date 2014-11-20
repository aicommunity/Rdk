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
// �������� ������������� �������� "UConnectedITEM"
//struct UCItem
// --------------------------
// ������������ � �����������
// --------------------------
UCItem::UCItem(void)
{
 // ������������ �������
 Item=0;

 // ������ ������
 Index=0;
}

UCItem::UCItem(const UCItem &copy)
{
 Item=copy.Item;
 Index=copy.Index;
 Name=copy.Name;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
bool UCItem::operator == (const UCItem &value)
{
 return (Item == value.Item) & (Index == value.Index);
}

bool UCItem::operator != (const UCItem &value)
{
 return !(*this == value);
}
// --------------------------

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
 //memcpy(Data,copy.Data,Size*sizeof(UCItem));

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
/// ���������� ����� ������� ������������ ���������
int UConnector::GetNumActiveInputs(void) const
{
 return int(ConnectedItemList.size());
}

/// ���������� ����� ������� ������������ ��������� � ��������� ��������
int UConnector::GetNumActiveInputs(const NameT &connector_property_name) const
{
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return 0;

 return int(I->second.size());
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


// ���������� ���������� �� �������� ������ � ���� item ��� -1, -1
// ���� ����� ����� �����������
UCLink UConnector::GetCLink(const UEPtr<UItem> &item) const
{
/* UCLink indexes;

 if(!item)
  return indexes;

 UCItem citem=CItemList.Find(item);

 if(citem.Item == 0)
  return indexes;

 indexes.Input=CItemList.Find(citem);
 UIProperty* property=0;
 FindInputProperty(indexes.Input, property);
 if(property)
  indexes.InputName=property->GetName();
 indexes.Output=citem.Index;
 indexes.OutputName=citem.Name;

 return indexes;
 */

 UCLink indexes;

 if(!item)
  return indexes;

 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 UCItem citem;
 bool flag=false;
 for(;I != ConnectedItemList.end();++I)
 {
  for(size_t i=0;i<I->second.size();i++)
   if(I->second[i].Item == item)
   {
	citem=I->second[i];
	flag=true;
	break;
   }
  if(flag)
   break;
 }

 if(I == ConnectedItemList.end())
  return indexes;

 if(citem.Item == 0)
  return indexes;

 UIProperty* property=0;
// if(!citem.Name.empty())
 FindInputProperty(I->first, property);
// else
//  FindInputProperty(citem.Index, property);
 if(property)
 {
  indexes.InputName=property->GetName();
//  indexes.Input=property->GetMinRange();
 }
 indexes.Output=citem.Index;
 indexes.OutputName=citem.Name;

 return indexes;
}

UCLink UConnector::GetCLink(const UItem* const item) const
{
/*
 UCLink indexes;

 if(!item)
  return indexes;

 UCItem citem=CItemList.Find(item);

 if(citem.Item == 0)
  return indexes;

 indexes.Input=CItemList.Find(citem);
 UIProperty* property=0;
 FindInputProperty(indexes.Input, property);
 if(property)
  indexes.InputName=property->GetName();
 indexes.Output=citem.Index;
 indexes.OutputName=citem.Name;

 return indexes;
 */
 const UEPtr<UItem> uitem=const_cast<UItem*>(item);
 return GetCLink(uitem);
}
// --------------------------

// --------------------------
// ������ ������� � �������� ������ � �������
// --------------------------
/// ���� ��������-���� �� ��������� �������
void UConnector::FindInputProperty(const NameT &connector_property_name, UIProperty* &property) const
{
 // ���� ��������� �� ������� ������
 property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.find(connector_property_name);

 if(I == PropertiesLookupTable.end())
  return;

 if(I->second.Type & ptInput)
 {
  property=I->second.Property.Get();
 }
}
	 /*
/// ���������� ������ ����� � �������� ������
int UConnector::FindInputIndex(const NameT &input_name) const
{
 // ���� ��������� �� ������� ������
 UIProperty* property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.find(input_name);
 if(I == PropertiesLookupTable.end())
  return ForbiddenId;

 if(!(I->second.Type & ptInput))
  return ForbiddenId;

 property=I->second.Property.Get();
 if(!property)
  return ForbiddenId;

 return property->GetMinRange();
}
			   */
// --------------------------

// ----------------------
// ���������������� ������
// ----------------------
// ������������� ����� � ��������� ���� 'na'.
bool UConnector::ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 if(!na)
  return false;

 if(!Build())
  return false;

 UIProperty* i_item_property=na->FindProperty(item_property_name);
 UIProperty* i_conn_property=FindProperty(connector_property_name);

 if(!i_item_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item_property_name);
  return false;
 }

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector is empty and AutoNumInputs disabled"));
  return false;
 }

 if(!CheckItem(na, item_property_name, connector_property_name))
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Checking fail"));
  return false;
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
	 c_index=i;
	 return true;
	}
	else
	{
	 I->second[i].Name = item_property_name;
	 c_index=i;
	 return true;
	}
   }
  }
 }


 UCItem item;
 item.Item=na;
 item.Index=-1;
 item.Name=item_property_name;
 ConnectedItemList[connector_property_name].push_back(item);
 c_index=int(ConnectedItemList[connector_property_name].size()-1);

 return AConnectToItem(na, item_property_name, connector_property_name);
}

/// ��������� ��� ����� � ��������� ���� 'na'
void UConnector::DisconnectFromItem(UEPtr<UItem> na)
{
 if(!na)
  return;

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
 }
}

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index
void UConnector::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name)
{
 if(!na)
  return;

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
 }
}

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index � c_index
void UConnector::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);

 if(I == ConnectedItemList.end())
  return;

 int i=0;
 while(i<int(I->second.size()))
 {
// for(size_t i=0;i<I->second.size();i++)
  if(I->second[i].Item == na && I->second[i].Name == item_property_name)
   DisconnectFromIndex(connector_property_name,I->second[i].Name,i); // TODO ������ �� ���������
  else
   ++i;
 }
}

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
	UIProperty* i_conn_property=0;
	FindInputProperty(connector_property_name,i_conn_property);
//	if(i_conn_property->CheckRange(index)) // TODO ��� �������� ���-�� ������
	 i_conn_property->SetPointer(index,0);

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
//	if(i_conn_property->CheckRange(i)) // TODO ��������� ��� ��� �� ������
	 i_conn_property->SetPointer(i,0);

	ADisconnectFromItem(I->second[i].Item,I->second[i].Name,connector_property_name);
	I->second.erase(I->second.begin()+i);
  }
}


// ��������� �������� ����� ��������� ������������ �����
bool UConnector::AConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
/* UIProperty* i_item_property=na->FindProperty(item_property_name);
 UIProperty* i_conn_property=FindProperty(connector_property_name);

 if(!i_item_property || !i_conn_property)
  return false;

 return AConnectToItem(na,i_item_property->GetMinRange(), i_conn_property->GetMinRange());
 */
 return true;
}

// ��������� �������� ����� ��������� ���������� �����
void UConnector::ADisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
/* UIProperty* i_item_property=na->FindProperty(item_property_name);
 UIProperty* i_conn_property=FindProperty(connector_property_name);

 if(!i_item_property || !i_conn_property)
  return;

 AConnectToItem(na,i_item_property->GetMinRange(), i_conn_property->GetMinRange());
 */
}

// ��������� ��� ������� �����
void UConnector::DisconnectAllItems(void)
{
/*
 for(int i=0;i<CItemList.GetSize();i++)
  if(CItemList[i].Item)
   CItemList[i].Item->Disconnect(this);
   */
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  while(!I->second.empty())
  {
   int index=int(I->second.size())-1;
   I->second[index].Item->Disconnect(this);
  }
//  for(size_t i=0;i<I->second.size();i++)
//   I->second[i].Item->Disconnect(this);
 }

}

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UConnector::DisconnectByObject(UEPtr<UContainer> brklevel)
{
/* for(int i=0;i<CItemList.GetSize();i++)
  if(CItemList[i].Item)
   if(!CItemList[i].Item->CheckOwner(brklevel))
	CItemList[i].Item->Disconnect(this);
	*/
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
//  for(size_t i=0;i<I->second.size();i++)
//   if(!I->second[i].Item->CheckOwner(brklevel))
//	I->second[i].Item->Disconnect(this);
 }

}

// ���������, ��������� �� ����������� ��������� item � ����� ����������
bool UConnector::CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name)
{
 return true;
}


// ���������, ���������� �� ����� � �������� �����������
bool UConnector::CheckLink(const UEPtr<UItem> &item) const
{
 UCLink link=GetCLink(item);
 if((link.Output>=0 && link.Input >=0) || (!link.InputName.empty() && !link.OutputName.empty()))
  return true;

 return false;
}

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UConnector::CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name) const
{
 UCLink link=GetCLink(item);
 if(!link.InputName.empty())
 {
  if(link.OutputName == item_property_name)
   return true;
 }
 return false;
}

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UConnector::CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name, const NameT &connector_property_name) const
{
 UCLink link=GetCLink(item);
 if(!link.OutputName.empty())
 {
  if(link.InputName == connector_property_name)
   return true;
 }

 return false;
}

// ���������� ������ �����������
template<typename T>
ULinksListT<T>& UConnector::GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
/*
 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;

 for(int i=0;i<CItemList.GetSize();i++)
 {
  if(CItemList[i].Item)
  {
   if(exclude_internals)
   {
	if(CItemList[i].Item->CheckOwner(internal_level))
	 continue;
   }
   CItemList[i].Item->GetLongId(netlevel,item.Id);
   connector.Index=i;
   UIProperty* property=0;
   FindInputProperty(i, property);
   if(property)
	connector.Name=property->GetName();

   item.Index=CItemList[i].Index;
   item.Name=CItemList[i].Name;
   if(connector.Id.size() != 0)
   {
	int item_id=linkslist.FindItem(item);
	if(item_id >= 0)
	{
	 if(linkslist[item_id].FindConnector(connector) >= 0)
	  continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;
 */

 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;


 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item)
   {
	if(exclude_internals)
	{
	 if(I->second[i].Item->CheckOwner(internal_level))
	  continue;
	}
   I->second[i].Item->GetLongId(netlevel,item.Id);
   UIProperty* property=0;
   FindInputProperty(I->first, property);
   if(property)
	connector.Index=-1;//property->GetMinRange();
   else
    connector.Index=-1;//i;
   connector.Name=I->first;

   item.Index=-1;//CItemList[i].Index;
   item.Name=I->second[i].Name;//CItemList[i].Name;
   if(connector.Id.size() != 0)
   {
	int item_id=linkslist.FindItem(item);
	if(item_id >= 0)
	{
	 if(linkslist[item_id].FindConnector(connector) >= 0)
	  continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;
}

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& UConnector::GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const
{
/*
 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;

 for(int i=0;i<CItemList.GetSize();i++)
 {
  if(CItemList[i].Item == cont)
  {
   CItemList[i].Item->GetLongId(netlevel,item.Id);
   connector.Index=i;
   UIProperty* property=0;
   FindInputProperty(i, property);
   if(property)
	connector.Name=property->GetName();
   item.Index=CItemList[i].Index;
   item.Name=CItemList[i].Name;
   if(connector.Id.size() != 0)
   {
	int item_id=linkslist.FindItem(item);
	if(item_id >= 0)
	{
	 if(linkslist[item_id].FindConnector(connector) >= 0)
	  continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;*/

 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;

 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item == cont)
   {
	I->second[i].Item->GetLongId(netlevel,item.Id);
	UIProperty* property=0;
	FindInputProperty(I->first, property);
	if(property)
	 connector.Index=property->GetMinRange();
	else
	 connector.Index=-1;//i; // TODO ��� ����������������
	connector.Name=I->first;
	item.Index=-1;//CItemList[i].Index;
	item.Name=I->second[i].Name;
	if(connector.Id.size() != 0)
	{
	 int item_id=linkslist.FindItem(item);
	 if(item_id >= 0)
	 {
	  if(linkslist[item_id].FindConnector(connector) >= 0)
	   continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;
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

//class UIPropertyInput: public UIPropertyIO
// ���������� ��������� �� ���������-��������
UEPtr<UItem> UIPropertyInput::GetItem(void)
{
 return Item;
}

// ���������� ��� ������������� ������
const std::string& UIPropertyInput::GetItemOutputName(void)
{
 return ItemOutputName;
}

//class UIPropertyOutput: public UIPropertyIO
// ���������� ����� ������������ ������
size_t UIPropertyOutput::GetNumConnectors(void)
{
 return Connectors.size();
}

// ���������� ��������� �� ���������-��������
UEPtr<UConnector> UIPropertyOutput::GetConnector(int index)
{
 return Connectors[index];
}

// ���������� ��� ������������� ����� ����������-���������
const std::string& UIPropertyOutput::GetConnectorInputName(int index)
{
 return ConnectorInputNames[index];
}

}
#endif

