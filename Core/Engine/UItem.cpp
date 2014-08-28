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
#include "UItem.h"

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

 for(int i=index;i<Size-1;i++)
  Data[i]=Data[i+1];
// memcpy(Data+index,Data+index+1,(Size-1)*sizeof(UAConnectorVector));
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

 for(int i=0;i<Size;i++)
  Data[i]=copy.Data[i];
// memcpy(Data,copy.Data,Size*sizeof(UAConnectorVector));

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
UItem::UItem(void)
{
 NumOutputs=0;
 AddLookupProperty("NumOutputs",ptParameter & pgSystem,new UVProperty<int,UItem>(this,&UItem::SetNumOutputs,&UItem::GetNumOutputs));
 AddLookupProperty("AutoNumOutputs",ptParameter & pgSystem,new UVProperty<bool,UItem>(this,&UItem::SetAutoNumOutputs,&UItem::GetAutoNumOutputs));
}

UItem::~UItem(void)
{
 DisconnectAll();
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ���������� ����� ������������ ��������� item
const int& UItem::GetNumOutputs(void) const
{
 return NumOutputs;//AssociatedConnectors.GetSize();
}

// ������������� ����� ������������ ��������� item
bool UItem::SetNumOutputs(const int &value)
{
 if(NumOutputs == value)
  return true;
/*
 if(value > AssociatedConnectors.GetSize())
 {
  for(int i=value;i<AssociatedConnectors.GetSize();i++)
   for(int j=0;j<AssociatedConnectors[i].GetSize();j++)
	Disconnect(AssociatedConnectors[i][j]);
 }

 AssociatedConnectors.Resize(value);*/
 NumOutputs=value;
 Ready = false;
 return true;
}

// ������� ���������/���������� ������ ��������������� ���������� ����� ������
// ��� ����������� ������ item.
const bool& UItem::GetAutoNumOutputs(void) const
{
 return AutoNumOutputs;
}

bool UItem::SetAutoNumOutputs(const bool &value)
{
 if(AutoNumOutputs == value)
  return true;

 AutoNumOutputs=value;
 return true;
}

/// ���������� ����� ������� � ������� ���-�� ���������
int UItem::GetNumActiveOutputs(void) const
{
 return int(RelatedConnectors.size());
}

/// ���������� ����� ����������� � ������� ���������� �������� ��������
int UItem::GetNumActiveOutputs(const NameT &item_property_name) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);
 if(I == RelatedConnectors.end())
  return 0;

 return int(I->second.size());
}
// --------------------------

// --------------------------
// ������ ������� � �������� ������ � �������
// --------------------------
/// ���� ��������-����� �� ��������� �������
void UItem::FindOutputProperty(int index, UIProperty* &property) const
{
 // ���� ��������� �� �������� ������
 property=0;
 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptOutput)
  {
   property=I->second.Property.Get();
   if(!property || !property->CheckRange(index))
   {
    property=0;
	continue;
   }

   break;
  }
 }
}

void UItem::FindOutputProperty(const NameT &item_property_name, UIProperty* &property) const
{
 // ���� ��������� �� �������� ������
 property=0;
 VariableMapCIteratorT I=PropertiesLookupTable.find(item_property_name);
 if(I == PropertiesLookupTable.end())
  return;

 if(I->second.Type & ptOutput)
 {
  property=I->second.Property.Get();
 }
}

/// ���������� ������ ����� � �������� ������
int UItem::FindOutputIndex(const NameT &output_name) const
{
 // ���� ��������� �� �������� ������
 UIProperty* property=0;
 VariableMapCIteratorT I=PropertiesLookupTable.find(output_name);
 if(I == PropertiesLookupTable.end())
  return ForbiddenId;

 if(!(I->second.Type & ptOutput))
  return ForbiddenId;

 property=I->second.Property.Get();
 if(!property)
  return ForbiddenId;

 return property->GetMinRange();
}

// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UItem::Free(void)
{
 DisconnectAll();
 UConnector::Free();
}
// --------------------------

// ----------------------
// ���������� ���������������� ������
// ----------------------
// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ������������� ����� ���� na ��� ���������.
bool UItem::ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index)
{
 if(!UConnector::ConnectToItem(na, i_index, c_index))
  return false;

 // ���� ��������� �� �������� ������
 UIProperty* output_property=0;
 na->FindOutputProperty(i_index, output_property);

// if(output_property)
//  CItemList[c_index].Name=output_property->GetName();

 // ���� ��������� �� ������� ������
 UIProperty* input_property=0;
 FindInputProperty(c_index, input_property);
 if(input_property)
 {
  if(input_property->GetIoType() & ipData)
  {
   if(input_property->GetIoType() & ipSingle)
   {
	if(output_property && output_property->CompareLanguageType(*input_property))
	 input_property->SetPointer(c_index,const_cast<void*>(output_property->GetPointer(i_index)));
   }
   else
   if(input_property->GetIoType() & ipRange)
   {
	if(output_property)
	 input_property->SetPointer(c_index,const_cast<void*>(output_property->GetPointer(i_index)));
   }
  }
  else
  if(input_property->GetIoType() & ipComp)
   input_property->SetPointer(c_index,na);
 }

 return true;
}

bool UItem::ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 if(!UConnector::ConnectToItem(na, item_property_name, connector_property_name,c_index))
  return false;

 // ���� ��������� �� �������� ������
 UIProperty* output_property=na->FindProperty(item_property_name);
// na->FindOutputProperty(i_index, output_property);

 if(output_property)
  ConnectedItemList[connector_property_name][c_index].Name=item_property_name;

 // ���� ��������� �� ������� ������
 UIProperty* input_property=FindProperty(connector_property_name);
// FindInputProperty(c_index, input_property);
 if(input_property)
 {
  if(input_property->GetIoType() & ipData)
  {
   if(input_property->GetIoType() & ipSingle)
   {
	if(output_property && output_property->CompareLanguageType(*input_property))
	 input_property->SetPointer(input_property->GetMinRange(),const_cast<void*>(output_property->GetPointer(output_property->GetMinRange())));
   }
   else
   if(input_property->GetIoType() & ipRange)
   {
	if(output_property)
	 input_property->SetPointer(input_property->GetMinRange(),const_cast<void*>(output_property->GetPointer(output_property->GetMinRange())));
   }
  }
  else
  if(input_property->GetIoType() & ipComp)
   input_property->SetPointer(input_property->GetMinRange(),na);
 }

 return true;
}

// ������������� ����� � ����������� 'c'.
bool UItem::Connect(UEPtr<UConnector> c, int i_index, int c_index)
{
 if(c == 0)
  return false;

 if(i_index>=NumOutputs)
  SetNumOutputs(i_index+1);

 if(!Build())
  return false;

 UIProperty *i_item_property=0;
 UIProperty *i_conn_property=0;
 FindOutputProperty(i_index,i_item_property);
 c->FindInputProperty(c_index,i_conn_property);

 if(!i_item_property || !i_conn_property)
  return true;

 int cc_index;
 return Connect(c, i_item_property->GetName(), i_conn_property->GetName(), cc_index);
}

bool UItem::Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 if(!c)
  return false;

 if(!Build())
  return false;

 if(!c->ConnectToItem(this,item_property_name, connector_property_name, c_index))
  return false;

 std::vector<PUAConnector> &vec=RelatedConnectors[item_property_name];
 for(size_t i=0;i<vec.size();i++)
  if(vec[i] == c)
   return true;

 vec.push_back(c);

 return true;
}

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
void UItem::Disconnect(UEPtr<UConnector> c)
{
 Build();

 if(c)
  c->DisconnectFromItem(this);

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
}

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
void UItem::Disconnect(UEPtr<UConnector> c, int i_index, int c_index)
{
 UIProperty *i_item_property=0;
 UIProperty *i_conn_property=0;
 FindOutputProperty(i_index,i_item_property);
 c->FindInputProperty(c_index,i_conn_property);

 if(!i_item_property || !i_conn_property)
  return;

 return Disconnect(c, i_item_property->GetName(), i_conn_property->GetName());
}

void UItem::Disconnect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name)
{
 if(!c)
  return;

 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.find(item_property_name);

 if(I == RelatedConnectors.end())
  return;

 int i=0;
 while(i<I->second.size())
 {
  if(I->second[i] == c)
  {
   UCItem citem=c->GetCItem(connector_property_name);
   if(citem.Name == item_property_name && citem.Item == this)
   {
	I->second.erase(I->second.begin()+i);
    c->DisconnectFromItem(this, item_property_name, connector_property_name);
   }
   else
    ++i;
  }
  else
   ++i;
 }
}
// ----------------------

// ----------------------
// ���������������� ������
// ----------------------
// ���������� ������� ����� ����������.
int UItem::GetNumAConnectors(int index) const
{
 UIProperty *i_item_property=0;
 FindOutputProperty(index,i_item_property);

 if(!i_item_property)
  return 0;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(i_item_property->GetName());

 if(I==RelatedConnectors.end())
  return 0;

 return int(I->second.size());
// return AssociatedConnectors[index].GetSize();//NumAConnectors[index];
}

int UItem::GetNumAConnectors(const NameT &item_property_name) const
{
 return GetNumActiveOutputs(item_property_name);
}

// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
bool UItem::Disconnect(const UId &id)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i]->Id == id)
	Disconnect(I->second[i]);
   else
    ++i;
  }
 }

 return true;
}

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
void UItem::DisconnectAll(void)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=int(I->second.size())-1;
  while(i>0)
  {
   Disconnect(I->second[i]);
   i=int(I->second.size())-1;
  }
 }
}

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UItem::DisconnectBy(UEPtr<UContainer> brklevel)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(!I->second[i]->CheckOwner(brklevel))
	Disconnect(I->second[i]);
   else
    ++i;
  }
 }


}

// ����������������� ��� ����� ����� item �� ����� connectors ������� ��������
// ������ �� ����� item
void UItem::BuildLinks(void)
{

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i])
	continue;
   UCLink indexes=I->second[i]->GetCLink(this);
   int c_index;
   I->second[i]->ConnectToItem(this,indexes.OutputName,indexes.InputName,c_index);
  }

}

// ���������� ��������� �� ��������� �� ������ �����������
// �� ����� 'name'.

UEPtr<UConnector> UItem::GetAConnector(const UId &id, int index) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i])
	continue;

   if(I->second[i]->Id == id)
    return I->second[i];
  }

 return 0;
}

// ����������  ��������� �� ������ �����������.
UEPtr<UConnector> UItem::GetAConnectorByIndex(int output, int index) const
{
 UIProperty *i_item_property=0;
 FindOutputProperty(output,i_item_property);

 if(!i_item_property)
  return 0;

 return GetAConnectorByIndex(i_item_property->GetName(), index);
}

UEPtr<UConnector> UItem::GetAConnectorByIndex(const NameT &item_property_name, int index) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);
 if(I == RelatedConnectors.end())
  return 0;

 if(index<0 || index>= int(I->second.size()))
  return 0;
 return I->second[index];
}

// ���������, ���������� �� ����� � �������� �����������
bool UItem::CheckLink(const UEPtr<UConnector> &connector) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Output>=0 && link.Input >=0)
  return true;

 return false;
}

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UItem::CheckLink(const UEPtr<UConnector> &connector, int item_index) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Input >=0)
 {
  if(link.Output == item_index || item_index <0)
   return true;
 }
 return false;
}

bool UItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const
{
 UCLink link=connector->GetCLink(this);
 if(!link.InputName.empty())
 {
  if(link.OutputName == item_property_name)
   return true;
 }

 return false;
}


// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UItem::CheckLink(const UEPtr<UConnector> &connector, int item_index, int conn_index) const
{
 UCLink link=connector->GetCLink(this);
 if(link.Output>=0)
 {
  if(link.Input == conn_index || conn_index<0)
   return true;
 }

 return false;
}

bool UItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name) const
{
 UCLink link=connector->GetCLink(this);
 if(!link.OutputName.empty())
 {
  if(link.InputName == connector_property_name)
   return true;
 }
 return false;
}

// ���������� ������ �����������
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
template<typename T>
ULinksListT<T>& UItem::GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 GetLongId(netlevel,item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
 {
  link.Connector.clear();
  for(size_t i=0;i<I->second.size();i++)
  {
   if(exclude_internals)
   {
	if(I->second[i]->CheckOwner(internal_level))
	 continue;
   }
   I->second[i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=I->second[i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
	link.Item.Index=indexes.Output;
	link.Item.Name=indexes.OutputName;
	connector.Index=indexes.Input;
	connector.Name=indexes.InputName;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }
 }

 return linkslist;

}

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& UItem::GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 GetLongId(netlevel,item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
 {
  link.Connector.clear();
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i] != cont)
	continue;
   I->second[i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=I->second[i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
	link.Item.Index=indexes.Output;
	link.Item.Name=indexes.OutputName;
	connector.Index=indexes.Input;
	connector.Name=indexes.InputName;

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
ULinksListT<T>& UItem::GetFullItemLinks(ULinksListT<T> &linkslist, UEPtr<UItem> comp,
                                     UEPtr<UContainer> netlevel) const
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


 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i]->CheckOwner(static_pointer_cast<UContainer>(comp)) && I->second[i] != comp)
	continue;
   I->second[i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.GetSize() != 0)
   {
	UCLink indexes=I->second[i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
	link.Item.Index=indexes.Output;
	link.Item.Name=indexes.OutputName;
	connector.Index=indexes.Input;
	connector.Name=indexes.InputName;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UItem> item=dynamic_cast<UItem*>(PComponents[i].operator->());
  item->GetFullItemLinks(linkslist, comp, netlevel);
 }

 return linkslist;

}
// ----------------------


// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UItem::Default(void)
{
 SetNumOutputs(1);
 SetAutoNumOutputs(true);
 return UConnector::Default();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UItem::Build(void)
{
 if(IsReady())
  return true;

 BuildLinks();

 return UConnector::Build();
}
/* *************************************************************************** */


}

#endif



