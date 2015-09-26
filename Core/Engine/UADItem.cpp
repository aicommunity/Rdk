/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UADITEM_CPP
#define UADITEM_CPP

#include <algorithm>
#include "UXMLEnvSerialize.h"
#include "UADItem.h"
#include "UPropertyIO.h"
#include "UStorage.h"

namespace RDK {

/* *************************************************************************** */
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UADItem::UADItem(void)
{
 NumInputs=0;
 AddLookupProperty("NumInputs",ptParameter & pgSystem,new UVProperty<int,UADItem>(this,&UADItem::SetNumInputs,&UADItem::GetNumInputs));
 AddLookupProperty("AutoNumInputs",ptParameter & pgSystem,new UVProperty<bool,UADItem>(this,&UADItem::SetAutoNumInputs,&UADItem::GetAutoNumInputs));
 NumOutputs=0;
 AddLookupProperty("NumOutputs",ptParameter & pgSystem,new UVProperty<int,UADItem>(this,&UADItem::SetNumOutputs,&UADItem::GetNumOutputs));
 AddLookupProperty("AutoNumOutputs",ptParameter & pgSystem,new UVProperty<bool,UADItem>(this,&UADItem::SetAutoNumOutputs,&UADItem::GetAutoNumOutputs));

 // ��������� �� ������ ������� ������� ���������� �� ������� ������
 POutputData=0;

 // ��������� ����� ���� ������
 FullInputDataSize=0;

 // ����������� ������ ������� ������
 MinInputDataSize=0;

 // ������������ ������ ������� ������
 MaxInputDataSize=0;
}

UADItem::~UADItem(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ���������� ����� ������������ ��������� item
const int& UADItem::GetNumInputs(void) const
{
 return NumInputs;
}

// ������������� ����� ������������ ��������� item
bool UADItem::SetNumInputs(const int &value)
{
 if(NumInputs == value)
  return true;

 for(int i=value;i<NumInputs;i++)
 {
  std::string name=std::string("DataInput")+sntoa(i);
  UEPtr<UIProperty> property=FindProperty(name);
  if(property)
  {
   DisconnectFromIndex(name);
   DelLookupProperty(name);
  }
 }

 Ready=false;
 NumInputs=value;
 InputData.resize(NumInputs);


 for(int i=0;i<NumInputs;i++)
 {
  std::string name=std::string("DataInput")+sntoa(i);
  UEPtr<UIProperty> property=FindProperty(name);
  if(!property)
   AddLookupProperty(name,ptPubInput,new UVPropertyInputData<UItemData,UADItem>(this,&InputData[i]));
  else
   property->UpdatePData(&InputData[i]);
 }

 CalcMinMaxInputDataSize();

 return true;
}

// ������� ���������/���������� ������ ��������������� ���������� ����� ������
// ��� ����������� ������ item.
const bool& UADItem::GetAutoNumInputs(void) const
{
 return AutoNumInputs;
}

bool UADItem::SetAutoNumInputs(const bool &value)
{
 if(AutoNumInputs == value)
  return true;

 AutoNumInputs=value;
 return true;
}

// ���������� ����� ������������ ��������� item
const int& UADItem::GetNumOutputs(void) const
{
 return NumOutputs;
}

// ������������� ����� ������������ ��������� item
bool UADItem::SetNumOutputs(const int &value)
{
 if(NumOutputs == value)
  return true;

 for(int i=value;i<NumOutputs;i++)
 {
  std::string name=std::string("DataOutput")+sntoa(i);
  UEPtr<UIProperty> property=FindProperty(name);
  if(property)
  {
   DisconnectAll(name);
   DelLookupProperty(name);
  }
 }

 Ready = false;
 NumOutputs=value;
 OutputData.resize(NumOutputs);

 for(int i=0;i<NumOutputs;i++)
 {
  std::string name=std::string("DataOutput")+sntoa(i);
  UEPtr<UIProperty> property=FindProperty(name);
  if(!property)
   AddLookupProperty(name,ptPubOutput,new UVPropertyOutputData<UItemData,UADItem>(this,&OutputData[i]));
  else
   property->UpdatePData(&OutputData[i]);
 }


 CalcMinMaxInputDataSize();
 return true;
}

// ������� ���������/���������� ������ ��������������� ���������� ����� ������
// ��� ����������� ������ item.
const bool& UADItem::GetAutoNumOutputs(void) const
{
 return AutoNumOutputs;
}

bool UADItem::SetAutoNumOutputs(const bool &value)
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
// ������ ������� � ���������
// --------------------------
// ���������� ��������� �� ������ ������ InputData �� ��������� �� item
// ���������� 0 ���� citem == 0 ��� �� ������ � ������ �����������
const UEPtr<const UItemData> UADItem::GetInputData(const UEPtr<UItem> &citem) const
{
 UItemData result;

 if(!citem)
  throw new EInputIndexNotExist(-1);

 std::vector<UCLink> buffer;
 GetCLink(citem,buffer);
 for(size_t i=0;i<buffer.size();i++)
 {
  UCLink &indexes=buffer[i];
  if(indexes.Input < 0)
   throw new EInputIndexNotExist(-1);

  return InputData[indexes.Input];
 }
 return UEPtr<const UItemData>();
}

// ���������� ��������� �� ������ ������ InputData �� �������
// ��������� ������ �� ������������ � ���������� 0, ���� ������ ����� ��� ����������
const UEPtr<const UItemData> UADItem::GetInputData(size_t index) const
{
 return InputData.at(index);
}

// ���������� ������ ������� ������ InputData �� �������
// ��������� ������ �� ������������ � ���������� 0, ���� ������ ����� ��� ����������
size_t UADItem::GetInputDataSize(size_t index) const
{
 if(index >= InputData.size())
  return 0;

 if(!InputData[index])
  return 0;

 return InputData[index]->GetSize();
}

// ���������� ��������� ������ ���� �������� ������
size_t UADItem::GetFullInputDataSize(void) const
{
 return FullInputDataSize;
}
// --------------------------

// ----------------------
// ������ ���������� ��������� �������
// ----------------------
bool UADItem::SetOutputDataSize(int index, int size, bool nobuild)
{
 if(index < 0)
  return false;

 if(OutputData.size() > size_t(index))
 {
  if(OutputData[index].GetSize() == size)
   return true;
 }
 else
  OutputData.resize(index+1);

 OutputData[index].Resize(1,size);

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   static_pointer_cast<UADItem>(I->second[i])->UpdatePointers();
   static_pointer_cast<UADItem>(I->second[i])->CalcMinMaxInputDataSize();
  }

 if(!nobuild)
  Ready=false;
 return true;
}
				   /*
bool UADItem::SetOutputDataElementSize(int index, int size)
{
// if(!Build())
//  return false;

 if(index < 0)// || index >= int(OutputData.size()))
  return false;

 if(OutputData.size() > size_t(index))
 {
  if(OutputData[index].DataSize == size)
   return true;
 }
 else
  OutputData.resize(index+1);

 OutputData[index].SetDataSize(size);
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   static_pointer_cast<UADItem>(I->second[i])->UpdatePointers();
   static_pointer_cast<UADItem>(I->second[i])->CalcMinMaxInputDataSize();
  }

 Ready=false;
 return true;
}                    */

// ��������� �������� �������� ������ �������
void UADItem::FillOutputData(int index, const void *data)
{
 if(!Ready && !Build())
  return;

 if(index < 0 || index >= int(OutputData.size()))
  return;

 UItemData &dest=OutputData[index];

 if(!data)
  dest.ToZero();
 else
  dest.Assign(1, dest.GetSize(),(const double*)data);
}

// ��������� ��� �������� ������� ��������� �������
void UADItem::FillOutputData(const void *data)
{
 if(!Ready && !Build())
  return;

 size_t size=OutputData.size();
 if(!size)
  return;

 UItemData* dest=&OutputData[0];
 for(size_t i=0;i<size;++i,++dest)
  if(!data)
  {
   if(dest->GetByteSize() && dest->Void)
    dest->ToZero();
  }
  else
   dest->Assign(1, dest->GetSize(),(const double*)data);
}
// ----------------------

// --------------------------
// ������ �������������� ������� �  �������� ������
// --------------------------
// �������������� ����� ��� ��������� �� ������
bool UADItem::SetOutputDataAsPointer(int index, void *pointer)
{
 if(index<NumOutputs)
 {
//  SetOutputDataElementSize(index,sizeof(void*));
  SetOutputDataSize(index,1);
  OutputData[index].PVoid[0]=pointer;
  return true;
 }
 return false;
}

// ���������� ������ ������ ��� ��������� �� ������
void* UADItem::GetOutputDataAsPointer(int index)
{
 if(index<NumOutputs && GetOutputDataSize(index)>0 && GetOutputDataElementSize(index) == sizeof(void*))
 {
  return POutputData[index].PVoid[0];
 }
 return 0;
}

// ���������� ������ ����� ��� ��������� �� ������
void* UADItem::GetInputDataAsPointer(int index)
{
 if(index<NumInputs && InputData[index] && InputData[index]->GetSize()>0 && InputData[index]->GetDataSize() == int(sizeof(void*)))
 {
  void *pointer=InputData[index]->PVoid[0];
  return pointer;
 }
 return 0;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������� ��������� �������� ������
UContainerDescription* UADItem::ANewDescription(UComponentDescription* description)
{
 return UComponent::ANewDescription(description);
}
// --------------------------

// ----------------------
// ������ ������� � ��������� ���������
// ----------------------
// ������ �������� ��������
vector<size_t> UADItem::GetOutputDataSize(void) const
{
 vector<size_t> result;

 size_t size=OutputData.size();
 result.resize(size);
 for(size_t i=0;i<size;i++)
  result[i]=OutputData[i].GetSize();

 return result;
}

bool UADItem::SetOutputDataSize(const vector<size_t> &value)
{
// if(!Build())
//  return false;

// if(OutputData.size() != value.size())
//  return false;

 size_t size=value.size();
 if(OutputData.size()<size)
  OutputData.resize(size);

 for(size_t i=0;i<size;i++)
  if(!SetOutputDataSize(i,value[i]))
   return false;

 return true;
}

// ������ ���������� ������� ������� ������� � ������
vector<size_t> UADItem::GetOutputDataElementSize(void) const
{
 vector<size_t> result;

 size_t size=OutputData.size();
 result.resize(size);
 for(size_t i=0;i<size;i++)
  result[i]=OutputData[i].GetDataSize();

 return result;
}
/*
bool UADItem::SetOutputDataElementSize(const vector<size_t> &value)
{
// if(OutputData.size() != value.size())
//  return false;

 size_t size=value.size();
 if(OutputData.size()<size)
  OutputData.resize(size);

 for(size_t i=0;i<size;i++)
  if(!SetOutputDataElementSize(i,value[i]))
   return false;

 return true;
} */
// ----------------------

// ----------------------
// ���������������� ������
// ----------------------
// ���������� ������������ � ����� ���������� ������ �� �������
const UCItem& UADItem::GetCItem(int c_index) const
{
 UIProperty* i_conn_property=0;

 FindInputProperty(c_index, i_conn_property);

 if(!i_conn_property)
  return DummyItem;

 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(i_conn_property->GetName());
 if(I == ConnectedItemList.end())
  return DummyItem;

 if(I->second.empty())
  return DummyItem;

 return I->second[0];
}

/// ���� ��������-���� �� ��������� �������
void UADItem::FindInputProperty(int index, UIProperty* &property) const
{
 // ���� ��������� �� ������� ������
 property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
 J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptInput)
  {
   property=I->second.Property.Get();
   if(property/* || !property->CheckRange(index)*/)
   {
	std::string::size_type i=property->GetName().find("DataInput");
	if(i != std::string::npos)
	{
	 int found_index=RDK::atoi(property->GetName().substr(9));
	 if(int(i) == index)
	  break;
	}
   }
  }
 }
}

void UADItem::FindInputProperty(const NameT &connector_property_name, UIProperty* &property) const
{
 UItem::FindInputProperty(connector_property_name, property);
}

/// ���� ��������-����� �� ��������� �������
void UADItem::FindOutputProperty(int index, UIProperty* &property) const
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
   if(property)
   {
	std::string::size_type i=property->GetName().find("DataOutput");
	if(i != std::string::npos)
	{
	 int found_index=RDK::atoi(property->GetName().substr(10));
	 if(int(i) == index)
	  break;
	}
   }
  }
 }
}

/// ���� ��������-����� �� ��������� �������
void UADItem::FindOutputProperty(const NameT &item_property_name, UIProperty* &property) const
{
 return UItem::FindOutputProperty(item_property_name, property);
}

// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ������������� ����� ���� na ��� ���������.
bool UADItem::ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index)
{
 UEPtr<UADItem> nad=dynamic_pointer_cast<UADItem>(na);
 if(!nad)
  return false;

 if(c_index>=NumInputs)
 {
  if(AutoNumInputs)
  {
   if(!SetNumInputs(c_index+1))
	return false;
   if(!Build())
	return false;
  }
  else
   return false;
 }
 else
 if(c_index<0)
 {
  for(int i=NumInputs-1;i>=0;i--)
  {
   if(!GetCItem(i).Item)
   {
	c_index=i;
	break;
   }
  }
  if(c_index<0)
  {
   c_index=NumInputs;

   if(!SetNumInputs(c_index+1))
	return false;
   if(!Build())
	return false;
  }
 }
 InputData[c_index]=&nad->POutputData[i_index];


 UpdatePointers();
 CalcMinMaxInputDataSize();

 UIProperty* i_item_property=0;
 UIProperty* i_conn_property=0;

 if(!na)
  return false;

 nad->FindOutputProperty(i_index,i_item_property);
 FindInputProperty(c_index,i_conn_property);

 if(!i_item_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item by this index not found: ")+sntoa(i_index));
  return false;
 }

 NameT conn_prop_name;
 if(i_conn_property)
  conn_prop_name=i_conn_property->GetName();

 return ConnectToItem(na, i_item_property->GetName(), conn_prop_name, c_index);
}

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index
void UADItem::DisconnectFromItem(UEPtr<UItem> na, int i_index)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na && I->second[i].Index == i_index)
	DisconnectFromIndex(I->first, I->second[i].Name,i);
   else
	++i;
  }
 }
}

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index � c_index
void UADItem::DisconnectFromItem(UEPtr<UItem> na, int i_index, int c_index)
{
 UEPtr<UADItem> nad=dynamic_pointer_cast<UADItem>(na);
 if(!nad)
  return;

 UIProperty* i_item_property=0;
 UIProperty* i_conn_property=0;

 nad->FindOutputProperty(i_index,i_item_property);
 FindInputProperty(c_index,i_conn_property);

 if(!i_item_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item by this index not found: ")+sntoa(i_index));
  return;
 }

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector by this index not found: ")+sntoa(c_index));
  return;
 }

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(i_conn_property->GetName());
 if(I != ConnectedItemList.end())
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na && I->second[i].Name == i_item_property->GetName())
	DisconnectFromIndex(I->first, I->second[i].Name,i);
   else
    ++i;
  }
 }
}

// ��������� ��� ����� � ��������� ���� 'na'
void UADItem::DisconnectFromItem(UEPtr<UItem> na)
{
 UItem::DisconnectFromItem(na);
}

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index
void UADItem::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name)
{
 UItem::DisconnectFromItem(na, item_property_name);
}

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index � c_index
void UADItem::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index)
{
 UItem::DisconnectFromItem(na, item_property_name, connector_property_name,connected_c_index);
}
		   /*
// ���������, ��������� �� ����������� ��������� item � ����� ����������
bool UADItem::CheckItem(UEPtr<UItem> item, int item_index, int conn_index)
{
 return true;
}

// ���������, ��������� �� ����������� ��������� item � ����� ����������
bool UADItem::CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name)
{
 return UItem::CheckItem(item, item_property_name, connector_property_name);
}        */
	 /*
// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UADItem::CheckLink(const UEPtr<UConnector> &connector, int item_index) const
{
 std::string item_property_name=(item_index<0)?std::string(""):(std::string("DataOutput")+sntoa(item_index));
 return UItem::CheckLink(connector, item_property_name);
}

bool UADItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const
{
 return UItem::CheckLink(connector, item_property_name);
}
   /*
// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UADItem::CheckLink(const UEPtr<UConnector> &connector, int item_index, int conn_index) const
{
 std::string connector_property_name=(conn_index<0)?std::string(""):(std::string("DataInput")+sntoa(conn_index));
 std::string item_property_name=(item_index<0)?std::string(""):(std::string("DataOutput")+sntoa(item_index));
 return UItem::CheckLink(connector, item_property_name, connector_property_name, conn_index);
}      */
	/*
bool UADItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index) const
{
 return UItem::CheckLink(connector, item_property_name, connector_property_name, connected_c_index);
}     */


/// ���� ������ ��������� ����
NameT UADItem::FindFreeInputName(void)
{
 Build();
 for(size_t i=0;i<InputData.size();i++)
 {
  if(!InputData[i])
   return std::string("DataInput")+sntoa(i);
 }
 return std::string("");
}

bool UADItem::ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 if(!UItem::ConnectToItem(na, item_property_name, connector_property_name, c_index))
  return false;
 return true;
}

// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
void UADItem::DisconnectFromIndex(int c_index)
{
 if(c_index<0 || c_index >= int(InputData.size()))
  return;

 UIProperty* i_conn_property=0;
 FindInputProperty(c_index,i_conn_property);

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector by this index not found: ")+sntoa(c_index));
  return;
 }

 DisconnectFromIndex(i_conn_property->GetName(), "", -1);

 InputData[c_index]=0;

 UpdatePointers();
 CalcMinMaxInputDataSize();
}

void UADItem::DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index)
{
 UItem::DisconnectFromIndex(connector_property_name, item_property_name, index);

 UpdatePointers();
 CalcMinMaxInputDataSize();
}

void UADItem::DisconnectFromIndex(const NameT &connector_property_name)
{
 UItem::DisconnectFromIndex(connector_property_name);

 UpdatePointers();
 CalcMinMaxInputDataSize();
}


// ������������� ����� � ����������� 'c'.
bool UADItem::Connect(UEPtr<UConnector> c, int i_index, int c_index)
{
 UEPtr<UADItem> ad_c=dynamic_pointer_cast<UADItem>(c);

 if(!ad_c)
  return false;

 if(i_index>=NumOutputs)
  SetNumOutputs(i_index+1);

 if(!Build())
  return false;

 std::string item_property_name=std::string("DataOutput")+sntoa(i_index),
	conn_property_name;

  if(!FindProperty(item_property_name))
   return false;

  if(c_index>=0)
  {
   if(ad_c->GetNumInputs()<=c_index)
   {
	if(ad_c->GetAutoNumInputs())
	 ad_c->SetNumInputs(c_index+1);
	else
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("DataInput index out of range and AutoNumInputs == false: ")+sntoa(c_index));
   }
   conn_property_name=std::string("DataInput")+sntoa(c_index);
  }
  else
  {
   conn_property_name=c->FindFreeInputName();
   if(conn_property_name.empty())
   {
	c_index=ad_c->GetNumInputs();
	ad_c->SetNumInputs(ad_c->GetNumInputs()+1);
	conn_property_name=std::string("DataInput")+sntoa(c_index);
   }
  }

 int cc_index=-1;
 return Connect(c, item_property_name, conn_property_name, cc_index);
}

bool UADItem::Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 std::string conn_property_name;
 UEPtr<UADItem> ad_c=dynamic_pointer_cast<UADItem>(c);
  if(!connector_property_name.empty())
  {
   std::string::size_type i=connector_property_name.find("DataInput");
   if(i != std::string::npos)
   {
	c_index=RDK::atoi(connector_property_name.substr(9));
	if(ad_c->GetNumInputs()<=c_index)
	{
	 if(ad_c->GetAutoNumInputs())
	  ad_c->SetNumInputs(c_index+1);
	 else
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("DataInput index out of range and AutoNumInputs == false: ")+sntoa(c_index));
	}
	conn_property_name=std::string("DataInput")+sntoa(c_index);
   }
   else
	conn_property_name=connector_property_name;
  }
  else
  {
   conn_property_name=c->FindFreeInputName();
   c_index=ad_c->GetNumInputs();
   ad_c->SetNumInputs(ad_c->GetNumInputs()+1);
   conn_property_name=std::string("DataInput")+sntoa(c_index);
  }


 if(!UItem::Connect(c, item_property_name, conn_property_name, c_index))
  return false;
 CalcMinMaxInputDataSize();
 return true;
}

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
void UADItem::Disconnect(UEPtr<UConnector> c, int i_index, int c_index)
{
 UIProperty *i_item_property=0;
 UIProperty *i_conn_property=0;
 FindOutputProperty(i_index,i_item_property);

 UEPtr<UADItem> ad_c=dynamic_pointer_cast<UADItem>(c);

 if(!ad_c)
  return;

 ad_c->FindInputProperty(c_index,i_conn_property);

 if(!i_item_property || !i_conn_property)
  return;

 return Disconnect(ad_c, i_item_property->GetName(), i_conn_property->GetName(), c_index);
}

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
void UADItem::Disconnect(UEPtr<UConnector> c)
{
 UItem::Disconnect(c);
}

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
void UADItem::Disconnect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index)
{
 UItem::Disconnect(c, item_property_name, connector_property_name, connected_c_index);
}

// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
bool UADItem::Disconnect(const UId &id)
{
 return UItem::Disconnect(id);
}

// ���������� ������� ����� ����������.
int UADItem::GetNumAConnectors(int index) const
{
 UIProperty *i_item_property=0;
 FindOutputProperty(index,i_item_property);

 if(!i_item_property)
  return 0;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(i_item_property->GetName());

 if(I==RelatedConnectors.end())
  return 0;

 return int(I->second.size());
}

int UADItem::GetNumAConnectors(const NameT &item_property_name) const
{
 return UItem::GetNumAConnectors(item_property_name);
}

// ����������  ��������� �� ������ �����������.
UEPtr<UConnector> UADItem::GetAConnectorByIndex(int output, int index) const
{
 UIProperty *i_item_property=0;
 FindOutputProperty(output,i_item_property);

 if(!i_item_property)
  return 0;

 return GetAConnectorByIndex(i_item_property->GetName(), index);
}

UEPtr<UConnector> UADItem::GetAConnectorByIndex(const NameT &item_property_name, int index) const
{
 return UItem::GetAConnectorByIndex(item_property_name, index);
}
// --------------------------

// ----------------------
// ������ ������� � �����������
// ----------------------
// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
bool UADItem::Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor, bool copystate) const
{
 if(!UItem::Copy(target,stor,copystate))
  return false;

 UEPtr<UADItem> item=dynamic_pointer_cast<UADItem>(target);
 return true;
}
// ----------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UADItem::Default(void)
{
 SetNumInputs(1);
 SetAutoNumInputs(true);
 SetNumOutputs(1);
 SetAutoNumOutputs(true);
 SetOutputDataSize(0,1);

 return UItem::Default();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UADItem::Build(void)
{
 if(IsReady())
  return true;

 // ��������� ������� ������������ ����� ������ � �������
 int min_num_inputs=0;
 int min_num_outputs=0;
 VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptInput)
  {
   UIProperty* property=dynamic_cast<UIProperty*>(I->second.Property.Get());
   if(!property)
	continue;

   min_num_inputs++;
  }

  if(I->second.Type & ptOutput)
  {
   UIProperty* property=dynamic_cast<UIProperty*>(I->second.Property.Get());
   if(!property)
	continue;

   min_num_outputs++;
  }
 }

 if(NumInputs<min_num_inputs)
  SetNumInputs(min_num_inputs);

 if(NumOutputs<min_num_outputs)
  SetNumOutputs(min_num_outputs);

 UpdatePointers();
 CalcMinMaxInputDataSize();

 if(!UItem::Build())
  return false;

 return true;
}

// ����� �������� ����� ��� ������ ��������
bool UADItem::Reset(void)
{
 if(!UContainer::Reset())
  return false;

 return true;
}
// --------------------------

// ----------------------
// ��������������� ������
// ----------------------
// ��������� ��������� �� ������� ������ � �������
void UADItem::UpdatePointers(void)
{
 if(NumOutputs>0)
 {
  POutputData=&OutputData[0];
 }
 else
 {
  POutputData=0;
 }

 // ��������� ����� ���� ������
 FullInputDataSize=0;
 for(int i=0;i<NumInputs;i++)
  if(InputData[i])
   FullInputDataSize+=InputData[i]->GetSize();
}


// ��������� ����������� � ������������ ������ �������� ������
void UADItem::CalcMinMaxInputDataSize(void)
{
 if(!InputData.size())
 {
  MinInputDataSize=0;
  MaxInputDataSize=0;
 }
 else
 {
  if(InputData[0])
   MaxInputDataSize=MinInputDataSize=InputData[0]->GetSize();
  else
   MaxInputDataSize=MinInputDataSize=0;
  for(int i=1;i<NumInputs;i++)
  {
   if(!InputData[i])
    continue;
   if(int(MinInputDataSize)>InputData[i]->GetSize())
	MinInputDataSize=InputData[i]->GetSize();
   if(int(MaxInputDataSize)<InputData[i]->GetSize())
	MaxInputDataSize=InputData[i]->GetSize();
  }
 }
}
// ----------------------
/* *************************************************************************** */

}
#endif


