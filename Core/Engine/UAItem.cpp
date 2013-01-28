/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAItemCPP
#define UAItemCPP

#include <string.h>
#include "UAItem.h"

namespace RDK {

/* *************************************************************************** */
// ��������� - ������ ���������� �� ������������ ��������
// class UAConnectorVector
// --------------------------
// ������������ � �����������
// --------------------------
UAConnectorVector::UAConnectorVector(void)
{
 // ������ ����������
 Size=0;

 // ������ ���������
 Data=0;

 // �������� ������ ����������
 RealSize=0;
}

UAConnectorVector::UAConnectorVector(const UAConnectorVector &copy)
{
 *this=copy;
}


UAConnectorVector::~UAConnectorVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void UAConnectorVector::Clear(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}

// ������ ������ ���������� � ����������� ���������
void UAConnectorVector::Resize(int newsize)
{
 if(Size == newsize)
  return;

 if(RealSize<newsize || !Data)
 {
  PUAConnector* newbuffer=new PUAConnector[newsize];
  memcpy(newbuffer,Data,sizeof(PUAConnector)*Size);

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
// � ���������� ��� ������ ��� -1 ���� ������� �� ������
int UAConnectorVector::Find(const PUAConnector &item, int index) const
{
 PUAConnector *pdata=Data;

 for(int i=index;i<Size;i++,pdata++)
  if(*pdata == item)
   return i;

 return -1;
}

// ��������� � ����� ���������� �������
void UAConnectorVector::Add(const PUAConnector &item)
{
 Resize(Size+1);
 Data[Size-1]=item;
}

// ��������� � ����� ���������� ������� ������ ���� ����� ��������� ��� ���
// ���������� true � ������ ��������� ����������
bool UAConnectorVector::AddUnique(const PUAConnector &item)
{
 if(Find(item)>=0)
  return false;

 Add(item);
 return true;
}

// ������� ������� �� �������
void UAConnectorVector::Del(int index)
{
 if(index<0 || index >= Size)
  return;

 if(index < Size-1)
 {
  memcpy(Data+index,Data+index+1,(Size-index-1)*sizeof(PUAConnector));
 }

 Resize(Size-1);
}

// ������� ������� �� ���������
void UAConnectorVector::Del(PUAConnector *item)
{
 int index=Find(*item);
 Del(index);
}
// --------------------------

// --------------------------
// ������ �������
// --------------------------
// �������� ������������
UAConnectorVector& UAConnectorVector::operator = (const UAConnectorVector &copy)
{
 Resize(copy.Size);
 memcpy(Data,copy.Data,Size*sizeof(PUAConnector));

 return *this;
}

// �������� ������� � ��������
PUAConnector& UAConnectorVector::operator [] (int index)
{
 return Data[index];
}

const PUAConnector& UAConnectorVector::operator [] (int index) const
{
 return Data[index];
}

// �������� ������� � �������
PUAConnector* UAConnectorVector::GetData(void)
{
 return Data;
}

// �������� ������� � �������
int UAConnectorVector::GetSize(void) const
{
 return Size;
}
// --------------------------
/* *************************************************************************** */

/* *************************************************************************** */
// ��������� - ������ ���������� �� ������������ ��������
// class UAConnector2DVector
// --------------------------
// ������������ � �����������
// --------------------------
UAConnector2DVector::UAConnector2DVector(void)
{
 // ������ ����������
 Size=0;

 // ������ ���������
 Data=0;

 // �������� ������ ����������
 RealSize=0;
}

UAConnector2DVector::UAConnector2DVector(const UAConnector2DVector &copy)
{
 *this=copy;
}


UAConnector2DVector::~UAConnector2DVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void UAConnector2DVector::Clear(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}

// ������ ������ ���������� � ����������� ���������
void UAConnector2DVector::Resize(int newsize)
{
 if(Size == newsize)
  return;

 if(RealSize<newsize || !Data)
 {
  UAConnectorVector* newbuffer=new UAConnectorVector[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Data[i];
//  memcpy(newbuffer,Data,sizeof(UAConnectorVector)*Size);

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
// � ���������� ��� ������ ��� -1 ���� ������� �� ������
/*int UAConnector2DVector::Find(const UAConnectorVector &item, int index) const
{
 UAConnectorVector *pdata=Data;

 for(int i=index;i<Size;i++,pdata++)
  if(*pdata == item)
   return i;

 return -1;
} */

// ��������� � ����� ���������� �������
void UAConnector2DVector::Add(const UAConnectorVector &item)
{
 Resize(Size+1);
 Data[Size-1]=item;
}

// ��������� � ����� ���������� ������� ������ ���� ����� ��������� ��� ���
// ���������� true � ������ ��������� ����������
/*bool UAConnector2DVector::AddUnique(const UAConnectorVector &item)
{
 if(Find(item)>=0)
  return false;

 Add(item);
 return true;
} */

// ������� ������� �� �������
void UAConnector2DVector::Del(int index)
{
 if(index<0 || index >= Size)
  return;

 memcpy(Data+index,Data+index+1,(Size-1)*sizeof(UAConnectorVector));
 Resize(Size-1);
}
/*
// ������� ������� �� ���������
void UAConnector2DVector::Del(UAConnectorVector *item)
{
 int index=Find(*item);
 Del(index);
}  */
// --------------------------

// --------------------------
// ������ �������
// --------------------------
// �������� ������������
UAConnector2DVector& UAConnector2DVector::operator = (const UAConnector2DVector &copy)
{
 Resize(copy.Size);
 memcpy(Data,copy.Data,Size*sizeof(UAConnectorVector));

 return *this;
}

// �������� ������� � ��������
UAConnectorVector& UAConnector2DVector::operator [] (int index)
{
 return Data[index];
}

const UAConnectorVector& UAConnector2DVector::operator [] (int index) const
{
 return Data[index];
}

// �������� ������� � �������
UAConnectorVector* UAConnector2DVector::GetData(void)
{
 return Data;
}

// �������� ������� � �������
int UAConnector2DVector::GetSize(void) const
{
 return Size;
}
// --------------------------
/* *************************************************************************** */

/* *************************************************************************** */
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UAItem::UAItem(void)
{
 NumOutputs=0;
 AddLookupProperty("NumOutputs",ptParameter & pgSystem,new UVProperty<int,UAItem>(this,&UAItem::SetNumOutputs,&UAItem::GetNumOutputs));
 AddLookupProperty("AutoNumOutputs",ptParameter & pgSystem,new UVProperty<bool,UAItem>(this,&UAItem::SetAutoNumOutputs,&UAItem::GetAutoNumOutputs));
}

UAItem::~UAItem(void)
{
 DisconnectAll();
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ���������� ����� ������������ ��������� item
int UAItem::GetNumOutputs(void) const
{
 return NumOutputs;//AssociatedConnectors.GetSize();
}

// ������������� ����� ������������ ��������� item
bool UAItem::SetNumOutputs(int value)
{
 if(NumOutputs == value)
  return true;

// if(value == AssociatedConnectors.GetSize())
//  return true;

 if(value > AssociatedConnectors.GetSize())
 {
  for(int i=value;i<AssociatedConnectors.GetSize();i++)
   for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
    Disconnect(AssociatedConnectors[i][j]);
 }

 AssociatedConnectors.Resize(value);
 NumOutputs=value;
 Ready = false;
 return true;
}

// ������� ���������/���������� ������ ��������������� ���������� ����� ������
// ��� ����������� ������ item.
bool UAItem::GetAutoNumOutputs(void) const
{
 return AutoNumOutputs;
}

bool UAItem::SetAutoNumOutputs(bool value)
{
 if(AutoNumOutputs == value)
  return true;

 AutoNumOutputs=value;
 return true;
}
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UAItem::Free(void)
{
 DisconnectAll();
 UAConnector::Free();
}
// --------------------------

// ----------------------
// ���������� ���������������� ������
// ----------------------
// ������������� ����� � ����������� 'c'.
bool UAItem::Connect(UEPtr<UAConnector> c, int i_index, int c_index)
{
 if(c == 0)
  return false;

 if(i_index>=NumOutputs)
  SetNumOutputs(i_index+1);

 if(!Build())
  return false;

 if(!c->ConnectToItem(this,i_index,c_index))
  return false;

// if(AssociatedConnectors[i_index].Find(c) < 0)
 AssociatedConnectors[i_index].AddUnique(c);
// PAssociatedConnectors[i_index]=&AssociatedConnectors[i_index][0];
// NumAConnectors[i_index]=AssociatedConnectors[i_index].GetSize();

 return true;
}

// ��������� ����� ������ ����� ������� � ����������� 'c'.
void UAItem::Disconnect(UEPtr<UAConnector> c)
{
 Build();

 if(c)
  c->DisconnectFromItem(this);

 for(int i=0;i<AssociatedConnectors.GetSize();i++)
 {
  int index=AssociatedConnectors[i].Find(c);
  while(index>=0)
  {
   AssociatedConnectors[i].Del(index);
//   NumAConnectors[i]=AssociatedConnectors[i].GetSize();
/*   if(NumAConnectors[i]>0)
    PAssociatedConnectors[i]=&AssociatedConnectors[i][0];
   else
    PAssociatedConnectors[i]=0;
  */
   index=AssociatedConnectors[i].Find(c,index);
  }
 }
}
// ----------------------

// ----------------------
// ���������������� ������
// ----------------------
// ���������� ������� ����� ����������.
int UAItem::GetNumAConnectors(int index) const
{
 return AssociatedConnectors[index].GetSize();//NumAConnectors[index];
}

// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
bool UAItem::Disconnect(const UId &id)
{
 Build();

 for(int j=0;j<AssociatedConnectors.GetSize();j++)
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   if(AssociatedConnectors[j][i]->Id == id)
    {
     Disconnect(AssociatedConnectors[j][i]);
      break;
    }
  }
 return true;
}

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
void UAItem::DisconnectAll(void)
{
 Build();
 for(int i=0;i<AssociatedConnectors.GetSize();i++)
 {
  while(AssociatedConnectors[i].GetSize() > 0)
  {
   Disconnect(AssociatedConnectors[i][AssociatedConnectors[i].GetSize()-1]);
  }
  AssociatedConnectors[i].Clear();
//  PAssociatedConnectors[i]=0;
//  NumAConnectors[i]=0;
 }
}

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UAItem::DisconnectBy(UEPtr<UAContainer> brklevel)
{
 Build();

 for(int i=0;i<AssociatedConnectors.GetSize();i++)
 {
  for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
  {
   for(int k=0;k<AssociatedConnectors[i].GetSize();k++)
    if(!AssociatedConnectors[i][k]->CheckOwner(brklevel))
    {
     Disconnect(AssociatedConnectors[i][k]);
     break;
    }
  }
 }
}

// ����������������� ��� ����� ����� item �� ����� connectors ������� ��������
// ������ �� ����� item
void UAItem::BuildLinks(void)
{
// PAssociatedConnectors.resize(NumOutputs);
// NumAConnectors.resize(NumOutputs);

 for(int i=0;i<AssociatedConnectors.GetSize();i++)
 {
  // ��������!!! ��� ������ �� NumOutputs != AssociatedConnectors[i].Size
//  NumAConnectors[i]=AssociatedConnectors[i].GetSize();
//  if(NumAConnectors[i]>0)
//   PAssociatedConnectors[i]=&AssociatedConnectors[i][0];

  for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
  {
   if(!AssociatedConnectors[i][j])
    continue;
   UCLink indexes=AssociatedConnectors[i][j]->GetCLink(this);
   AssociatedConnectors[i][j]->ConnectToItem(this,indexes.Output,indexes.Input);
  }
 }
}

// ���������� ��������� �� ��������� �� ������ �����������
// �� ����� 'name'.

UEPtr<UAConnector> UAItem::GetAConnector(const UId &id, int index) const
{
 for(int i=0;i<AssociatedConnectors[index].GetSize();i++)
  if(AssociatedConnectors[index][i]->Id == id)
   return AssociatedConnectors[index][i];
 return 0;
}

// ����������  ��������� �� ������ �����������.
UEPtr<UAConnector> UAItem::GetAConnectorByIndex(int output, int index) const
{
 return AssociatedConnectors[output][index];
}


// ���������, ���������� �� ����� � �������� �����������
bool UAItem::CheckLink(const UEPtr<UAConnector> &connector) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Output>=0 && link.Input >=0)
  return true;
/*
 for(int i=0;i<AssociatedConnectors.GetSize();i++)
  for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
   if(AssociatedConnectors[i][j] == connector)
	return true;
  */
 return false;
}

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UAItem::CheckLink(const UEPtr<UAConnector> &connector, int item_index) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Input >=0)
 {
  if(link.Output == item_index || item_index <0)
   return true;
 }

/*
 if(item_index>= AssociatedConnectors.GetSize())
  return false;

 if(item_index >=0)
 {
  for(int j=0;j<AssociatedConnectors[item_index].GetSize();j++)
   if(AssociatedConnectors[item_index][i] == connector)
	 return true;
 }
 else
 {
  return CheckLink(connector);
 }
  */
 return false;
}

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UAItem::CheckLink(const UEPtr<UAConnector> &connector, int item_index, int conn_index) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Output>=0)
 {
  if(link.Input == conn_index || conn_index<0)
   return true;
 }
/*
 if(item_index>= AssociatedConnectors.GetSize())
  return false;

 if(item_index >=0)
 {
  for(int j=0;j<AssociatedConnectors[item_index].GetSize();j++)
   if(AssociatedConnectors[item_index][i] == connector)
   {
	if(conn_index <0)
	 return true;

	UCLink link=connector->GetCLink(this);
	if(conn_index == link.Input)
	 return true;
   }
 }
 else
 {
  for(int i=0;i<AssociatedConnectors.GetSize();i++)
   for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
	if(AssociatedConnectors[i][j] == connector)
	{
	 if(conn_index <0)
	  return true;

	 UCLink link=connector->GetCLink(this);
	 if(conn_index == link.Input)
	  return true;
	}

 }
	*/
 return false;
}

// ���������� ������ �����������
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
template<typename T>
ULinksListT<T>& UAItem::GetLinks(ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel, bool exclude_internals, UEPtr<UAContainer> internal_level) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 GetLongId(netlevel,item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 for(int j=0;j<AssociatedConnectors.GetSize();j++)
 {
  link.Connector.clear();
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   if(exclude_internals)
   {
	if(AssociatedConnectors[j][i]->CheckOwner(internal_level))
	 continue;
   }
   AssociatedConnectors[j][i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=AssociatedConnectors[j][i]->GetCLink(UEPtr<UAItem>(const_cast<UAItem*>(this)));
	item.Index=indexes.Output;
	connector.Index=indexes.Input;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }
 }

 return linkslist;
}

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& UAItem::GetPersonalLinks(UEPtr<UAContainer> cont, ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 GetLongId(netlevel,item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 for(int j=0;j<AssociatedConnectors.GetSize();j++)
 {
  link.Connector.clear();
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   if(AssociatedConnectors[j][i] != cont)
	continue;
   AssociatedConnectors[j][i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=AssociatedConnectors[j][i]->GetCLink(UEPtr<UAItem>(const_cast<UAItem*>(this)));
	item.Index=indexes.Output;
	connector.Index=indexes.Input;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }
 }

 return linkslist;
}

// ���������� ������ ����������� ����� ���������� � ���� �������� ���������
// � ��������� ���������� comp � ���� ��� �������� �����������
template<typename T>
ULinksListT<T>& UAItem::GetFullItemLinks(ULinksListT<T> &linkslist, UEPtr<UAItem> comp,
                                     UEPtr<UAContainer> netlevel) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 if(!comp)
  return linkslist;

 GetLongId(netlevel,item.Id);
 if(link.Item.Id.GetSize() == 0)
  return linkslist;
 link.Item=item;


 for(int j=0;j<AssociatedConnectors.GetSize();j++)
  for(int i=0;i<AssociatedConnectors[j].GetSize();i++)
  {
   if(!AssociatedConnectors[j][i]->CheckOwner(static_pointer_cast<UAContainer>(comp)) && AssociatedConnectors[j][i] != comp)
	continue;
   AssociatedConnectors[j][i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.GetSize() != 0)
   {
	UCLink indexes=AssociatedConnectors[j][i]->GetCLink(UEPtr<UAItem>(const_cast<UAItem*>(this)));
	item.Index=indexes.Output;
	connector.Index=indexes.Input;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UAItem> item=dynamic_cast<UAItem*>(PComponents[i].operator->());
  item->GetFullItemLinks(linkslist, comp, netlevel);
 }

 return linkslist;
}
// ----------------------


// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UAItem::Default(void)
{
 SetNumOutputs(1);
 SetAutoNumOutputs(true);
 return UAConnector::Default();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UAItem::Build(void)
{
 if(IsReady())
  return true;

 BuildLinks();

 return UAConnector::Build();
}

// ���������� ��� ����������, ������������ � ������ ����� �������.
/*void UAItem::ActivateAll(void)
{
 Build();
 for(size_t j=0;j<AssociatedConnectors.size();j++)
  for(size_t i=0;i<AssociatedConnectors[j].size();i++)
   AssociatedConnectors[j][i]->Activate(this);
} */
/* *************************************************************************** */


}

#endif



