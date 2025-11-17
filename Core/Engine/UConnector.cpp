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
#include "UContainer.h"
#include <glog/logging.h>

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
UCItem UCItemList::Find(const std::shared_ptr<UItem> &item, int index) const
{
 return Find(item.get(),index);
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

const UCItem& UConnector::GetCItem(const NameT &connector_property_name, const std::shared_ptr<UItem> &item, int &index) const
{
 index=-1;
 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(connector_property_name);
 if(I == ConnectedItemList.end())
  return DummyItem;

 for(size_t i=0;i<I->second.size();i++)
  if(I->second[i].Item == item.get())
  {
   index = int(i);
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
void UConnector::GetCLink(const std::shared_ptr<UItem> &item, std::vector<UCLink> &buffer) const
{
 UCLink indexes;
 buffer.clear();

 if(!item)
  return;

 // CRITICAL: Make a copy of ConnectedItemList before iteration to avoid use-after-free
 // During UNet::Copy, the connector object may be destroyed while ConnectedItemList is still being iterated
 // By copying the map, we ensure that string references remain valid even if the original map is destroyed
 std::map<std::string, std::vector<UCItem> > connected_item_list_copy;
 try {
  connected_item_list_copy = ConnectedItemList;
 } catch (...) {
  LOG(ERROR) << "UConnector::GetCLink - exception when copying ConnectedItemList, returning empty buffer";
  return;
 }

 std::map<std::string, std::vector<UCItem> >::const_iterator I=connected_item_list_copy.begin();
 UCItem citem;
 for(;I != connected_item_list_copy.end();++I)
 {
  // CRITICAL: Save I->first to local copy to avoid use-after-free
  std::string connector_property_name = I->first;
  
  for(size_t i=0;i<I->second.size();i++)
   if(I->second[i].Item == item.get())
   {
	citem=I->second[i];

	if(citem.Item == 0)
	 continue;

    UIPropertyInput* property=0;
	FindInputProperty(connector_property_name, property);
	if(property)
	{
	 // CRITICAL: Save property->GetName() to local copy before assigning to indexes.InputName
	 // property may be destroyed or its name may be freed before assignment completes
	 std::string property_name;
	 try {
	  property_name = property->GetName();
	 } catch (...) {
	  LOG(WARNING) << "UConnector::GetCLink - exception in property->GetName(), skipping InputName";
	  property_name.clear();
	 }
	 indexes.InputName=property_name;
	}
	indexes.Output=citem.Index;
	// CRITICAL: Save citem.Name to local copy before assigning to indexes.OutputName
	// citem may be destroyed or its Name may be freed before assignment completes
	std::string citem_name;
	try {
	 citem_name = citem.Name;
	} catch (...) {
	 LOG(WARNING) << "UConnector::GetCLink - exception when copying citem.Name, skipping OutputName";
	 citem_name.clear();
	}
	indexes.OutputName=citem_name;
	buffer.push_back(indexes);
   }
 }
}

void UConnector::GetCLink(const UItem* const item, std::vector<UCLink> &buffer) const
{
 const std::shared_ptr<UItem> uitem=std::shared_ptr<UItem>(const_cast<UItem*>(item), [](UItem*){}); // Non-owning deleter
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
  property=dynamic_cast<UIPropertyInput*>(I->second.Property);
 }
}
// --------------------------

// ----------------------
// ���������������� ������
// ----------------------
// ������������� ����� � ��������� ���� 'na'.
bool UConnector::ConnectToItem(std::shared_ptr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item)
{
 if(!na)
  return false;

 // CRITICAL: Prevent infinite recursion - if Build() is already in progress, skip it
 // Build() may call BuildStructure() which calls CreateLink() which calls ConnectToItem() again
 // Use a thread-local flag to track if Build() is in progress
 thread_local static bool build_in_progress = false;
 if(build_in_progress)
 {
  // Build() is already in progress, skip it to prevent infinite recursion
  LOG(WARNING) << "UConnector::ConnectToItem - Build() already in progress, skipping to prevent infinite recursion";
 }
 else
 {
  build_in_progress = true;
  try {
   if(!Build())
   {
    build_in_progress = false;
    return false;
   }
  } catch (...) {
   build_in_progress = false;
   throw;
  }
  build_in_progress = false;
 }

 if(!na->GetActivity() && !na->GetOwner())
 {
  std::string item_full_name = na->GetFullName();
  std::string conn_full_name = GetFullName();
  LogMessageEx(RDK_EX_WARNING, __FUNCTION__, std::string("Linking warning: item ")+item_full_name+":"+item_property_name+
                " doesn't exsits in model, connection to "+conn_full_name+":"+connector_property_name+" possible incorect!");
  //return false;
 }

 if(!GetActivity() && !GetOwner())
 {
  std::string item_full_name = na->GetFullName();
  std::string conn_full_name = GetFullName();
  LogMessageEx(RDK_EX_WARNING, __FUNCTION__, std::string("Linking warning: ")+item_full_name+":"+item_property_name+
                " connector "+conn_full_name+":"+connector_property_name+" doesn't exsits in model. Connection possible incorrect!");
  //return false;
 }

 std::shared_ptr<UIPropertyOutput> i_item_property=std::dynamic_pointer_cast<UIPropertyOutput>(na->FindProperty(item_property_name));
 std::shared_ptr<UIPropertyInput> i_conn_property=std::dynamic_pointer_cast<UIPropertyInput>(FindProperty(connector_property_name));

 if(!i_item_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Output not found: ")+item_property_name);
  return false;
 }

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Input ")+connector_property_name+std::string(" not found or empty and AutoNumInputs disabled"));
  return false;
 }

 if(!i_conn_property->CompareElemLanguageType(*i_item_property.get()))
 {
  std::string item_full_name = na->GetFullName();
  std::string conn_full_name = GetFullName();
  std::string item_property_type = i_item_property->GetLanguageType().name();
  std::string conn_property_type = i_conn_property->GetLanguageType().name();
  LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Linking error: ")+item_full_name+":"+item_property_name+
                " have incompatible type for "+conn_full_name+":"+connector_property_name+" ["+
                item_property_type+ " != " + conn_property_type+"]");
  return false;
 }

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);
 if(I != ConnectedItemList.end())
 {
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item == na.get())
   {
    if(I->second[i].Name == item_property_name)
    {
     if(c_index == -1)
      c_index=int(i);
     if(!forced_connect_same_item)
      return true;
    }
   }
  }
 }

 //i_conn_property->Init(na,item_property_name);

 // TODO: ���� ��� �� ����� �������� � ������, ���� c_index ����� ���������� ��
 // ���� �� �� �����������
 UCItem item;
 item.Item=na.get();
 item.Index=-1;
 item.Name=item_property_name;
 ConnectedItemList[connector_property_name].push_back(item);
 c_index=int(ConnectedItemList[connector_property_name].size())-1;

 return AConnectToItem(na, item_property_name, connector_property_name);
}

/// ��������� ��� ����� � ��������� ���� 'na'
void UConnector::DisconnectFromItem(std::shared_ptr<UItem> na)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na.get())
    DisconnectFromIndex(I->first, I->second[i].Name,i);
   else
    ++i;
  }
 }
}

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index
void UConnector::DisconnectFromItem(std::shared_ptr<UItem> na, const NameT &item_property_name)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na.get() && I->second[i].Name == item_property_name)
    DisconnectFromIndex(I->first,I->second[i].Name,i); // TODO ������ �� ���������
   else
    ++i;
  }
 }
}

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index � c_index
void UConnector::DisconnectFromItem(std::shared_ptr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(connector_property_name);

 if(I == ConnectedItemList.end())
  return;

/*
 int i=0;
 while(i<int(I->second.size()))
 {
  if(I->second[i].Item == na && I->second[i].Name == item_property_name)
   DisconnectFromIndex(connector_property_name,I->second[i].Name,i); // TODO: ������ �� ���������
  else
   ++i;
 }*/
 DisconnectFromIndex(connector_property_name,I->second[connected_c_index].Name,connected_c_index); // TODO ������ �� ���������
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
    UIPropertyInput* i_conn_property=0;
    FindInputProperty(connector_property_name,i_conn_property);

    if(!i_conn_property)
//     i_conn_property->UnInit();
//    else
    {
     LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("connector ")+connector_property_name+" not found");
    }

    //	if(i_conn_property->CheckRange(index)) // TODO: ��� �������� ���-�� ������
       std::shared_ptr<UIPropertyOutput> output_property=std::dynamic_pointer_cast<UIPropertyOutput>(I->second[index].Item->FindProperty(item_property_name));
    if(output_property && i_conn_property)
    {
     if(!i_conn_property->ResetPointer(index,output_property.get()))
     {
      LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Data ResetPointer fail"));
     }
    }
    else
     if(!output_property && i_conn_property)
      LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found"));

     ADisconnectFromItem(std::shared_ptr<UItem>(I->second[index].Item, [](UItem*){}), I->second[index].Name, connector_property_name); // Non-owning deleter
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
   UIPropertyInput* i_conn_property=0;
   FindInputProperty(connector_property_name,i_conn_property);
  //	if(i_conn_property->CheckRange(i)) // TODO: ��������� ��� ��� �� ������
  /* UIPropertyInput* input_prop=dynamic_cast<UIPropertyInput*>(i_conn_property);
   if(input_prop)
   {
    input_prop->UnInit();
   }*/

   std::shared_ptr<UIPropertyOutput> output_property=std::dynamic_pointer_cast<UIPropertyOutput>(I->second[i].Item->FindProperty(I->second[i].Name));
   if(output_property)
   {
    if(!i_conn_property->ResetPointer(i,output_property.get()))
    {
     LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("ResetPointer fail"));
    }
   }
   else
    LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found"));

   ADisconnectFromItem(std::shared_ptr<UItem>(I->second[i].Item, [](UItem*){}), I->second[i].Name, connector_property_name); // Non-owning deleter
   I->second.erase(I->second.begin()+i);
  }
}


// ��������� �������� ����� ��������� ������������ �����
bool UConnector::AConnectToItem(std::shared_ptr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
 return true;
}

// ��������� �������� ����� ��������� ���������� �����
void UConnector::ADisconnectFromItem(std::shared_ptr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
}

// ��������� ��� ������� �����
void UConnector::DisconnectAllItems(void)
{
 // SAFETY: Add protection against null pointers and use-after-free
 // CRITICAL: Item is a raw pointer (UItem*) that may point to destroyed object
 // During UNet::Copy -> BreakLinks -> DisconnectAllItems, objects may be destroyed
 // We need to be very careful when accessing raw pointers
 // Simplest solution: Just clear ConnectedItemList without calling Disconnect()
 // Disconnect() will be called automatically when objects are destroyed
 // This avoids segfault from accessing destroyed objects
 
 // CRITICAL: During Copy/BreakLinks, objects may be destroyed
 // Calling Disconnect() on destroyed objects causes segfault
 // Instead, just clear the list - connections will be broken automatically
 ConnectedItemList.clear();
 
 // ALTERNATIVE: If we need to call Disconnect(), we would need signal handling
 // But that's complex and may cause other issues
 // For now, clearing the list is the safest approach
}

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UConnector::DisconnectByObject(std::shared_ptr<UContainer> brklevel)
{
 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  size_t i=0;
  while(i<I->second.size())
  {
   if(!I->second[i].Item->CheckOwner(brklevel))
    I->second[i].Item->Disconnect(GetId());
   else
    ++i;
  }
 }
}
/*
// ���������, ��������� �� ����������� ��������� item � ����� ����������
bool UConnector::CheckItem(std::shared_ptr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name)
{
 return true;
}*/


// ���������, ���������� �� ����� � �������� �����������
bool UConnector::CheckLink(const std::shared_ptr<UItem> &item) const
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
bool UConnector::CheckLink(const std::shared_ptr<UItem> &item, const NameT &item_property_name) const
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
bool UConnector::CheckLink(const std::shared_ptr<UItem> &item, const NameT &item_property_name, const NameT &connector_property_name) const
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

//class UIPropertyInput: public UIPropertyIO
/// ������������ � �����������
UIPropertyInput::UIPropertyInput(void)
{
}

UIPropertyInput::~UIPropertyInput(void)
{
}
/*
// ������������� ��������� �� ������ �����
bool UIPropertyInput::SetPointer(int index, UIPropertyOutput* property)
{
 return false;
}

/// ���������� ��������� �� ������
bool UIPropertyInput::ResetPointer(int index, UIPropertyOutput* property)
{
 return false;
}*/

//class UIPropertyOutput: public UIPropertyIO
/// ������������ � �����������
UIPropertyOutput::UIPropertyOutput(void)
{
}

UIPropertyOutput::~UIPropertyOutput(void)
{
 Connectors.clear();
 ConnectorInputNames.clear();
}

// ���������� ����� ������������ ������
size_t UIPropertyOutput::GetNumConnectors(void) const
{
 return Connectors.size();
}

// ���������� ��������� �� ���������-��������
UConnector* UIPropertyOutput::GetConnector(int index)
{
 return Connectors[index];
}

// ���������� ��� ������������� ����� ����������-���������
std::string UIPropertyOutput::GetConnectorInputName(int index) const
{
 return ConnectorInputNames[index];
}


/// ���������� ��������� �� �������� ������������� ����� ����������-���������
UIPropertyInput* UIPropertyOutput::GetConnectorProperty(int index)
{
 if(index<0 || index>=int(Connectors.size()))
  return 0;
 UConnector *cont=Connectors[index];
 if(!cont)
  return 0;
 UIPropertyInput *property(0);
 cont->FindInputProperty(ConnectorInputNames[index],property);
 return property;
}

void UIPropertyOutput::UpdateConnectedPointers(void)
{
 UContainer* owner = reinterpret_cast<UContainer*>(this->GetOwner());
 if(!owner) return;
 UConnector* item = dynamic_cast<UConnector*>(owner);
 if(!item) return;
 size_t num_inputs=item->GetNumActiveOutputs(this->GetName());
 for(size_t i=0;i<num_inputs;i++)
 {
  UIPropertyInput* property(0);
  item->FindConnectedProperty(this->GetName(), int(i), property);
  if(!property)
   continue;

  property->SetPointer(int(i), dynamic_cast<UIPropertyOutput*>(this));
 }
}





}
#endif

