/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAItemH
#define UAItemH

#include "UConnector.h"
//#include "UEInterface.h"

namespace RDK {

typedef UEPtr<UConnector> PUAConnector;
          /*
class RDK_LIB_TYPE UAConnectorVector
{
protected: // ���������
// ������ ����������
int Size;

protected: // ������
// ������ ���������
PUAConnector* Data;

protected: // ��������� ����������
// �������� ������ ����������
int RealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAConnectorVector(void);
UAConnectorVector(const UAConnectorVector &copy);
virtual ~UAConnectorVector(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void Clear(void);

// ������ ������ ���������� � ����������� ���������
void Resize(int newsize);

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ������ ��� -1 ���� ������� �� ������
int Find(const PUAConnector &item, int index=0) const;

// ��������� � ����� ���������� �������
void Add(const PUAConnector &item);

// ��������� � ����� ���������� ������� ������ ���� ����� ��������� ��� ���
// ���������� true � ������ ��������� ����������
bool AddUnique(const PUAConnector &item);

// ������� ������� �� �������
void Del(int index);

// ������� ������� �� ���������
void Del(PUAConnector *item);
// --------------------------

// --------------------------
// ������ �������
// --------------------------
// �������� ������������
UAConnectorVector& operator = (const UAConnectorVector &copy);

// �������� ������� � ��������
PUAConnector& operator [] (int index);
const PUAConnector& operator [] (int index) const;

// �������� ������� � �������
PUAConnector* GetData(void);

// �������� ������� � �������
int GetSize(void) const;
// --------------------------
};

class RDK_LIB_TYPE UAConnector2DVector
{
protected: // ���������
// ������ ����������
int Size;

protected: // ������
// ������ ���������
UAConnectorVector* Data;

protected: // ��������� ����������
// �������� ������ ����������
int RealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAConnector2DVector(void);
UAConnector2DVector(const UAConnector2DVector &copy);
virtual ~UAConnector2DVector(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void Clear(void);

// ������ ������ ���������� � ����������� ���������
void Resize(int newsize);

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ������ ��� -1 ���� ������� �� ������
//int Find(const UAConnectorVector &item, int index=0) const;

// ��������� � ����� ���������� �������
void Add(const UAConnectorVector &item);

// ������� ������� �� �������
void Del(int index);
// --------------------------

// --------------------------
// ������ �������
// --------------------------
// �������� ������������
UAConnector2DVector& operator = (const UAConnector2DVector &copy);

// �������� ������� � ��������
UAConnectorVector& operator [] (int index);
const UAConnectorVector& operator [] (int index) const;

// �������� ������� � �������
UAConnectorVector* GetData(void);

// �������� ������� � �������
int GetSize(void) const;
// --------------------------
};     */


class UIPropertyOutput;

class RDK_LIB_TYPE UItem: public UConnector
{
protected: // �������� ��������
// ������ ����������� ������������ � ������� ����� �������
//UAConnector2DVector AssociatedConnectors;
//std::map<std::string, std::vector<PUAConnector> > RelatedConnectors;

protected: // ��������� ����������. Read Only!

protected: // ���������� �������� ������� � ������� �������. Read only!

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UItem(void);
virtual ~UItem(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
public:
/// ���������� ����� ������� � ������� ���-�� ���������
virtual int GetNumActiveOutputs(void) const;

/// ���������� ����� ����������� � ������� ���������� �������� ��������
virtual int GetNumActiveOutputs(const NameT &item_property_name) const;
// --------------------------

// --------------------------
// ������ ������� � �������� ������ � �������
// --------------------------
/// ���� ��������-����� �� ��������� �������
virtual void FindOutputProperty(const NameT &item_property_name, UIPropertyOutput* &property) const;
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);
// --------------------------


public:
// ----------------------
// ���������������� ������
// ----------------------
/// ���������� ������ �������-������
virtual void ReadOutputPropertiesList(std::vector<UEPtr<UIPropertyOutput> > &buffer) const;

/// ���������� ����� �������
virtual int GetNumOutputs(void) const;

// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ���������� false ���� na ��� ��������� � ����� �����.
// ��� �������� ����������� c_index �������� �������� ������ ������������� �����
//virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

// ������������� ����� � ����������� 'c'
virtual bool Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
virtual void Disconnect(UEPtr<UConnector> c);

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
virtual void Disconnect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index);

// ���������� ������� ����� ���������� ��� ��������� ������.
virtual int GetNumAConnectors(const NameT &item_property_name) const;

// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
//virtual bool Disconnect(const UId &id);

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
virtual void DisconnectAll(void);
virtual void DisconnectAll(const NameT &item_property_name);

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual void DisconnectBy(UEPtr<UContainer> brklevel);

// ����������������� ��� ����� ����� item �� ����� connectors ������� ��������
// ������ �� ����� item
virtual void BuildLinks(void);

// ���������� ��������� �� ��������� �� ������ �����������
// �� Id 'id'.
//virtual UEPtr<UConnector> GetAConnector(const UId &id, int index) const;

// ����������  ��������� �� ������ �����������.
//virtual UEPtr<UConnector> GetAConnectorByIndex(const NameT &item_property_name, int index) const;

// ���������, ���������� �� ����� � �������� �����������
bool CheckLink(const UEPtr<UConnector> &connector, int connected_c_index) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index) const;


// ���������� ������ �����������
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const;

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const;

// ���������� ������ ����������� ����� ���������� � ���� �������� ���������
// � ��������� ���������� comp � ���� ��� �������� �����������
template<typename T>
ULinksListT<T>& GetFullItemLinks(ULinksListT<T> &linkslist, UEPtr<UItem> comp,
                            UEPtr<UContainer> netlevel) const;
// ----------------------

public:
// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool Default(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool Build(void);
// --------------------------

};

class RDK_LIB_TYPE UIPropertyOutputBase: virtual public UIPropertyOutput
{
protected: // ������
std::vector<PUAConnector> RelatedConnectors;

/// ��������� �� ����������-��������� ������
std::vector<UItem*> Connectors;

/// ��� ������ ���������-��������� ������
std::vector<std::string> ConnectorInputNames;

public: // ������������ � �����������
UIPropertyOutputBase(void);
virtual ~UIPropertyOutputBase(void);

public: // ������ ������� � ������������ ������
/// ���������� ����� ������������ ������
virtual size_t GetNumConnectors(void) const;

/// ���������� ��������� �� ���������-��������
virtual UConnector* GetConnector(int index);

/// ���������� ��� ������������� ����� ����������-���������
virtual std::string GetConnectorInputName(int index) const;

// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
//virtual bool Disconnect(const UId &id);

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
virtual void DisconnectAll(void);

public: // ������ ���������� ���������� �� ������� ������
/// ���������� ��������� �� ������
virtual void const* GetPointer(int index) const=0;

/// ������������� ��������� �� ������
//virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// ���������� ��������� �� ������
//virtual bool ResetPointer(int index, void* value)=0;

protected:
// ������������� ����� � ����������� 'c'
virtual bool Connect(UEPtr<UConnector> c, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
virtual void Disconnect(UEPtr<UConnector> c);

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
virtual void Disconnect(UEPtr<UConnector> c, const NameT &connector_property_name, int c_index=-1);

// ����������  ��������� �� ������ �����������.
//virtual UEPtr<UConnector> GetAConnectorByIndex(int c_index=-1) const;

// ���������, ���������� �� ����� � �������� �����������
bool CheckLink(const UEPtr<UConnector> &connector, int c_index) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &connector_property_name, int c_index=-1) const;

// ����������������� ��� ����� ����� ������ �� ����� connectors
virtual void BuildLinks(void);
};




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

 std::vector<UEPtr<UIPropertyOutput> > properties;
 ReadOutputPropertiesList(properties);

// std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
// for(;I != RelatedConnectors.end();++I)
 for(size_t j=0;j<properties.size();j++)
 {
  link.Connector.clear();
  for(size_t i=0;i<properties[j]->GetNumConnectors();i++)
  {
   UConnector* curr_conn=properties[j]->GetConnector(i);
   if(exclude_internals)
   {
	if(curr_conn->CheckOwner(internal_level))
	 continue;
   }
   curr_conn->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	std::vector<UCLink> buffer;
	curr_conn->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)),buffer);
	for(size_t k=0;k<buffer.size();k++)
	{
	 if(buffer[k].OutputName == properties[j]->GetName())
	 {
	  link.Item.Index=buffer[k].Output;
	  link.Item.Name=buffer[k].OutputName;
	  connector.Index=buffer[k].Input;
	  connector.Name=buffer[k].InputName;

	  link.Connector.push_back(connector);
	  linkslist.Set(link);
	 }
	}
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
   UConnector* curr_conn=I->second[i];
   if(curr_conn != cont)
	continue;
   curr_conn->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	std::vector<UCLink> buffer;
	curr_conn->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)),buffer);
	for(size_t k=0;k<buffer.size();k++)
	{
	 if(buffer[k].OutputName == I->first)
	 {
	  link.Item.Index=buffer[k].Output;
	  link.Item.Name=buffer[k].OutputName;
	  connector.Index=buffer[k].Input;
	  connector.Name=buffer[k].InputName;

	  link.Connector.push_back(connector);
	  linkslist.Set(link);
	 }
	}
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
   UConnector* curr_conn=I->second[i];
   if(!curr_conn->CheckOwner(static_pointer_cast<UContainer>(comp)) && curr_conn != comp)
	continue;
   curr_conn->GetLongId(netlevel,connector.Id);
   if(connector.Id.GetSize() != 0)
   {
	std::vector<UCLink> buffer;
	curr_conn->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)),buffer);
	for(size_t k=0;k<buffer.size();k++)
	{
	 if(buffer[k].OutputName == I->first)
	 {
	  link.Item.Index=buffer[k].Output;
	  link.Item.Name=buffer[k].OutputName;
	  connector.Index=buffer[k].Input;
	  connector.Name=buffer[k].InputName;

	  link.Connector.push_back(connector);
	  linkslist.Set(link);
	 }
	}
   }
  }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UItem> item=dynamic_cast<UItem*>(PComponents[i].operator->());
  item->GetFullItemLinks(linkslist, comp, netlevel);
 }

 return linkslist;
}


}
#endif

