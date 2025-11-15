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
#include <glog/logging.h>

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
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void UAConnectorVector::Clear(void)
{
 Size=0;
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

// ��������� � ����� ���������� �������
void UAConnector2DVector::Add(const UAConnectorVector &item)
{
 Resize(Size+1);
 Data[Size-1]=item;
}

// ������� ������� �� �������
void UAConnector2DVector::Del(int index)
{
 if(index<0 || index >= Size)
  return;

 for(int i=index;i<Size-1;i++)
  Data[i]=Data[i+1];
 Resize(Size-1);
}
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
}

UItem::~UItem(void)
{
 try {
  DisconnectAll();
 } catch (const std::bad_weak_ptr&) {
  // Object is not managed by shared_ptr or already destroyed, skip
  // This can happen during destruction when shared_from_this() is called
  LOG(WARNING) << "UItem::~UItem - bad_weak_ptr in DisconnectAll(), skipping";
 } catch (...) {
  // Ignore other exceptions during destruction
  LOG(WARNING) << "UItem::~UItem - exception in DisconnectAll(), skipping";
 }
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ������� � �������� ������ � �������
// --------------------------
/// ���� ��������-����� �� ��������� �������
void UItem::FindOutputProperty(const NameT &item_property_name, UIPropertyOutput* &property) const
{
 // ���� ��������� �� �������� ������
 property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.find(item_property_name);
// // TODO: ������� ��������� ������
// VariableMapCIteratorT I=PropertiesLookupTable.end();
// if(CheckAlias(item_property_name))
//  I=PropertiesLookupTable.find(GetPropertyNameByAlias(item_property_name));
// else
//  I=PropertiesLookupTable.find(item_property_name);

 if(I == PropertiesLookupTable.end())
  return;

 if(I->second.Type & ptOutput)
 {
  property=dynamic_cast<UIPropertyOutput*>(I->second.Property);
 }
}


/// ���������� ��������� �� �������� ������������� ����� ����������-���������
void UItem::FindConnectedProperty(const NameT &item_property_name, int index, UIPropertyInput* &property) const
{
 property=0;
 NameT connector_property_name;
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);

 if(I == RelatedConnectors.end())
  return;

 const std::vector<PUAConnector> &vec=I->second;

 if(index<0 || index>=int(vec.size()))
  return;

 if(!vec[index])
  return;

 std::vector<UCLink> buffer;
 vec[index]->GetCLink(this,buffer);
 for(size_t k=0;k<buffer.size();k++)
 {
  UCLink &link=buffer[k];
  if(!link.InputName.empty())
  {
   if(link.OutputName == item_property_name)
   {
          property=dynamic_pointer_cast<UIPropertyInput>(vec[index]->FindProperty(link.InputName)).get();
    return;
   }
  }
 }

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

bool UItem::ConnectToItem(std::shared_ptr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item)
{
 if(!UConnector::ConnectToItem(na, item_property_name, connector_property_name,c_index, forced_connect_same_item))
  return false;

 // ���� ��������� �� �������� ������ (��� �������������� ����������, �� ��� ��������� ����)
 UIPropertyOutput* output_property=dynamic_pointer_cast<UIPropertyOutput>(na->FindProperty(item_property_name)).get();

 // ���� ��������� �� ������� ������ (��� �������������� ����������, �� ��� ��������� ����)
 UIPropertyInput* input_property=dynamic_pointer_cast<UIPropertyInput>(FindProperty(connector_property_name)).get();


 int size=int(ConnectedItemList[connector_property_name].size());
 if(size>c_index)
  ConnectedItemList[connector_property_name][c_index].Name=item_property_name;
 else
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Fatal: c_index incorrect: ")+sntoa(c_index));

  if(!input_property->SetPointer(c_index,output_property))
  {
   LogMessageEx(RDK_EX_FATAL, __FUNCTION__, std::string("SetPointer fail"));
  }

 return true;
}

// ������������� ����� � ����������� 'c'.
bool UItem::Connect(std::shared_ptr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item)
{
 if(!c)
  return false;

 // CRITICAL: Prevent infinite recursion - if Build() is already in progress, skip it
 // Build() may call BuildStructure() which calls CreateLink() which calls Connect() again
 // Use a thread-local flag to track if Build() is in progress
 thread_local static bool build_in_progress = false;
 if(build_in_progress)
 {
  // Build() is already in progress, skip it to prevent infinite recursion
  LOG(WARNING) << "UItem::Connect - Build() already in progress, skipping to prevent infinite recursion";
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

 if(!c->ConnectToItem(get_shared_from_this(),item_property_name, connector_property_name, c_index, forced_connect_same_item))
  return false;

 std::vector<PUAConnector> &vec=RelatedConnectors[item_property_name];
 for(size_t i=0;i<vec.size();i++)
  if(vec[i] == c)
   return true;

 vec.push_back(c);

 return true;
}

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
void UItem::Disconnect(std::shared_ptr<UConnector> c)
{
 Build();

 if(c)
  c->DisconnectFromItem(get_shared_from_this());

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
void UItem::Disconnect(std::shared_ptr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index)
{
 if(!c)
  return;

 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.find(item_property_name);

 if(I == RelatedConnectors.end())
  return;

 UCItem citem=c->GetCItem(connector_property_name,get_shared_from_this(),connected_c_index);
 int i=0;
 while(i<int(I->second.size()))
 {
  if(I->second[i] == c)
  {
   if(citem.Name == item_property_name && citem.Item == this)
   {
	I->second.erase(I->second.begin()+i);
    c->DisconnectFromItem(get_shared_from_this(), item_property_name, connector_property_name, connected_c_index);
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
int UItem::GetNumAConnectors(const NameT &item_property_name) const
{
 return GetNumActiveOutputs(item_property_name);
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
 try {
  Build();
 } catch (const std::bad_weak_ptr&) {
  // Object is not managed by shared_ptr or already destroyed, skip Build()
  LOG(WARNING) << "UItem::DisconnectAll - bad_weak_ptr in Build(), skipping";
 } catch (...) {
  // Ignore other exceptions during destruction
  LOG(WARNING) << "UItem::DisconnectAll - exception in Build(), skipping";
 }

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=int(I->second.size())-1;
  while(i>=0)
  {
   try {
    Disconnect(I->second[i]);
   } catch (const std::bad_weak_ptr&) {
    // Object is not managed by shared_ptr or already destroyed, skip Disconnect()
    LOG(WARNING) << "UItem::DisconnectAll - bad_weak_ptr in Disconnect(), skipping";
    break; // Exit loop if we can't disconnect
   } catch (...) {
    // Ignore other exceptions during destruction
    LOG(WARNING) << "UItem::DisconnectAll - exception in Disconnect(), skipping";
    break; // Exit loop if we can't disconnect
   }
   i=int(I->second.size())-1;
  }
 }
}

void UItem::DisconnectAll(const NameT &item_property_name)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.find(item_property_name);

 if(I!= RelatedConnectors.end())
 {
  int i=int(I->second.size())-1;
  while(i>=0)
  {
   Disconnect(I->second[i]);
   i=int(I->second.size())-1;
  }
 }
}


// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UItem::DisconnectBy(std::shared_ptr<UContainer> brklevel)
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
   std::vector<UCLink> buffer;
   I->second[i]->GetCLink(this,buffer);
   for(size_t k=0;k<buffer.size();k++)
   {
	UCLink &indexes=buffer[k];
	if(I->first == indexes.OutputName)
	{
	 int c_index(-1);
	 I->second[i]->ConnectToItem(get_shared_from_this(),indexes.OutputName,indexes.InputName,c_index);
	}
   }
  }

}

// ���������� ��������� �� ��������� �� ������ �����������
// �� ����� 'name'.
std::shared_ptr<UConnector> UItem::GetAConnector(const UId &id, int index) const
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
std::shared_ptr<UConnector> UItem::GetAConnectorByIndex(const NameT &item_property_name, int index) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);
 if(I == RelatedConnectors.end())
  return 0;

 if(index<0 || index>= int(I->second.size()))
  return 0;
 return I->second[index];
}


// ���������, ���������� �� ����� � �������� �����������
bool UItem::CheckLink(const std::shared_ptr<UConnector> &connector, int connected_c_index) const
{
 std::vector<UCLink> buffer;
 connector->GetCLink(this,buffer);
 for(size_t k=0;k<buffer.size();k++)
 {
  UCLink &link=buffer[k];
  if(!link.InputName.empty() && !link.OutputName.empty()
    && (connected_c_index<0 || connected_c_index == link.Input) )
   return true;
  }

 return false;
}

bool UItem::CheckLink(const std::shared_ptr<UConnector> &connector, const NameT &item_property_name) const
{
 std::vector<UCLink> buffer;
 connector->GetCLink(this,buffer);
 for(size_t k=0;k<buffer.size();k++)
 {
  UCLink &link=buffer[k];
  if(!link.InputName.empty())
  {
   if(link.OutputName == item_property_name)
	return true;
  }
 }

 return false;
}

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool UItem::CheckLink(const std::shared_ptr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index) const
{
 std::vector<UCLink> buffer;
 connector->GetCLink(this,buffer);
 for(size_t k=0;k<buffer.size();k++)
 {
  UCLink &link=buffer[k];
  if(!link.OutputName.empty())
  {
   if((connector_property_name.empty() || link.InputName == connector_property_name) && ((connected_c_index < 0 || connected_c_index == link.Input) || link.Input<0))
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
bool UItem::Default(void)
{
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



