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
#include "UAConnector.h"
#include "UAStorage.h"
#include "UAItem.h"

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
  UCItem* newbuffer=new UCItem[newsize];
  memcpy(newbuffer,Data,sizeof(UCItem)*Size);

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
UCItem UCItemList::Find(UEPtr<UAItem> item, int index) const
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
 memcpy(Data,copy.Data,Size*sizeof(UCItem));

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
UAConnector::UAConnector(void)
{
 NumInputs=0;
 AddLookupProperty("NumInputs",new UVProperty<int,UAConnector>(this,&UAConnector::SetNumInputs,&UAConnector::GetNumInputs));
 AddLookupProperty("AutoNumInputs",new UVProperty<bool,UAConnector>(this,&UAConnector::SetAutoNumInputs,&UAConnector::GetAutoNumInputs));
}

UAConnector::~UAConnector(void)
{
 DisconnectAllItems();
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ���������� ����� ������������ ��������� item
int UAConnector::GetNumInputs(void) const
{
 return NumInputs;
}

// ������������� ����� ������������ ��������� item
bool UAConnector::SetNumInputs(int value)
{
 if(NumInputs == value)
  return true;

// NumInputs=CItemList.size();
 if(NumInputs > value)
 {
  for(int i=value;i<NumInputs;++i)
   if(CItemList[i].Item)
    CItemList[i].Item->Disconnect(this);
 }

 int index=NumInputs;
 CItemList.Resize(value);
 for(int i=index;i<value;i++)
 {
  CItemList[i].Item=0;
  CItemList[i].Index=-1;
 }

 Ready=false;
 NumInputs=CItemList.GetSize();
 return true;
}

// ������� ���������/���������� ������ ��������������� ���������� ����� ������
// ��� ����������� ������ item.
bool UAConnector::GetAutoNumInputs(void) const
{
 return AutoNumInputs;
}

bool UAConnector::SetAutoNumInputs(bool value)
{
 if(AutoNumInputs == value)
  return true;

 AutoNumInputs=value;
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UAConnector::Free(void)
{
 DisconnectAllItems();
 UAContainer::Free();
}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������������ � ����� ���������� ������ �� �������
const UCItem& UAConnector::GetCItem(int c_index) const
{
 return CItemList[c_index];
}

// ���������� ���������� �� �������� ������ � ���� item ��� -1, -1
// ���� ����� ����� �����������
UCLink UAConnector::GetCLink(UEPtr<UAItem> item) const
{
 UCLink indexes;

 if(!item)
  return indexes;

 UCItem citem=CItemList.Find(item);

 if(citem.Item == 0)
  return indexes;

 indexes.Input=CItemList.Find(citem);
 indexes.Output=citem.Index;

 return indexes;
}
// --------------------------

// ----------------------
// ���������������� ������
// ----------------------
// ������������� ����� � ��������� ���� 'na'.
bool UAConnector::ConnectToItem(UEPtr<UAItem> na, int i_index, int &c_index)
{
 if(!na)
  return false;

 if((c_index < 0 || c_index >= int(NumInputs)) && AutoNumInputs == false)
  return false;

 if(i_index < 0 || i_index >= na->GetNumOutputs())
  return false;

 // ���� ������� ����� ��������������� ���������� ������ �����
 // � c_index<0 �� ��������� ����� ���� ���� ������������ � ����������� ����������
 if(c_index<0)
 {
  int i=0;
  for(i=0;i<CItemList.GetSize();i++)
   if(!CItemList[i].Item)
    break;

  if(i == NumInputs)
  {
   SetNumInputs(NumInputs+1);
   CItemList.Resize(NumInputs+1);
   c_index=NumInputs-1;
  }
  else
   c_index=i;
 }
 else // ���� c_index >= NumInputs �� ��������� ����� ������ �� ����������
 if(c_index >= NumInputs)
 {
  SetNumInputs(c_index+1);
 }

 if(!Build())
  return false;

 if(!CheckItem(na, i_index,c_index))
  return false;

 if(CItemList[c_index].Item == na)
 {
  if(CItemList[c_index].Index == i_index)
   return true;

  CItemList[c_index].Index = i_index;
  return true;
 }

 if(CItemList[c_index].Item)
  return false;

 CItemList[c_index].Item=na;
 CItemList[c_index].Index=i_index;
 return AConnectToItem(na, i_index, c_index);
}

// ��������� ����� � ��������� ���� 'na'
void UAConnector::DisconnectFromItem(UEPtr<UAItem> na)
{
 if(!na)
  return;

 for(int i=0;i<CItemList.GetSize();i++)
 {
  if(CItemList[i].Item == na)
  {
   DisconnectFromIndex(i);
  }
 }
}

// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
void UAConnector::DisconnectFromIndex(int c_index)
{
 if(c_index<0 || c_index >=NumInputs)
  return;

 ADisconnectFromItem(CItemList[c_index].Item,CItemList[c_index].Index,c_index);
 CItemList[c_index].Item=0;
 CItemList[c_index].Index=-1;

 // ��������� ������ �����, ���� ���������
/* if(AutoNumInputs)
 {
  int newsize=NumInputs;
  for(int i=NumInputs-1;i>=0;i--)
  {
   if(CItemList[i].Item != 0)
	break;
   --newsize;
  }
  SetNumInputs(newsize);
 }*/
}

// ��������� �������� ����� ��������� ������������ �����
bool UAConnector::AConnectToItem(UEPtr<UAItem> na, int i_index, int c_index)
{
 return true;
}

// ��������� �������� ����� ��������� ���������� �����
void UAConnector::ADisconnectFromItem(UEPtr<UAItem> na, int i_index, int c_index)
{
}

// ��������� ��� ������� �����
void UAConnector::DisconnectAllItems(void)
{
// Build();
 for(int i=0;i<CItemList.GetSize();i++)
  if(CItemList[i].Item)
   CItemList[i].Item->Disconnect(this);
}

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UAConnector::DisconnectByObject(UEPtr<UAContainer> brklevel)
{
// Build();
 for(int i=0;i<CItemList.GetSize();i++)
  if(CItemList[i].Item)
   if(!CItemList[i].Item->CheckOwner(brklevel))
    CItemList[i].Item->Disconnect(this);
}
                      /*
// ���������� ��������� �����
UEInterface* UAConnector::GetInputInterface(int c_index)
{
 return 0;
}

// ���������� ��������� �����
NameT UAConnector::GetInputInterfaceTypeName(int c_index)
{
 UEInterface* ueinterface=GetInputInterface(c_index);
 if(ueinterface)
  return ueinterface->GetTypeName();

 return NameT("");
}                       */

// ���������, ��������� �� ����������� ��������� item � ����� ����������
bool UAConnector::CheckItem(UEPtr<UAItem> item, int item_index, int conn_index)
{
/* if(item->GetOutputType(item_index) == GetInputType(conn_index))
  return true;
 return false;*/
 return true;
}

// ���������� ������ �����������
ULinksList& UAConnector::GetLinks(ULinksList &linkslist, UEPtr<UAContainer> netlevel) const
{
 ULink link;
 GetLongId(netlevel,link.Connector.Id);
 if(link.Connector.Id.GetSize()==0)
  return linkslist;

 for(int i=0;i<CItemList.GetSize();i++)
 {
  if(CItemList[i].Item)
  {
   CItemList[i].Item->GetLongId(netlevel,link.Item.Id);
   link.Connector.Index=i;
   link.Item.Index=CItemList[i].Index;
   if(link.Connector.Id.GetSize() != 0)
   {
    if(linkslist.Find(link) >= 0)
     continue;
    else
     linkslist.Add(link);
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
bool UAConnector::Default(void)
{
 SetNumInputs(1);
 SetAutoNumInputs(true);
 return UAContainer::Default();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UAConnector::Build(void)
{
 return UAContainer::Build();
}
// --------------------------

/* *************************************************************************** */


}
#endif

