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
};

class RDK_LIB_TYPE UItem: public UConnector
{
protected: // �������� ��������
// ������ ����������� ������������ � ������� ����� �������
//UAConnector2DVector AssociatedConnectors;
std::map<std::string, std::vector<PUAConnector> > RelatedConnectors;

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
virtual void FindOutputProperty(const NameT &item_property_name, UIProperty* &property) const;
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
// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ���������� false ���� na ��� ��������� � ����� �����.
// ��� �������� ����������� c_index �������� �������� ������ ������������� �����
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

// ������������� ����� � ����������� 'c'
virtual bool Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
virtual void Disconnect(UEPtr<UConnector> c);

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
virtual void Disconnect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index);

// ���������� ������� ����� ���������� ��� ��������� ������.
virtual int GetNumAConnectors(const NameT &item_property_name) const;

// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
virtual bool Disconnect(const UId &id);

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
virtual UEPtr<UConnector> GetAConnector(const UId &id, int index) const;

// ����������  ��������� �� ������ �����������.
virtual UEPtr<UConnector> GetAConnectorByIndex(const NameT &item_property_name, int index) const;

// ���������, ���������� �� ����� � �������� �����������
bool CheckLink(const UEPtr<UConnector> &connector, int connected_c_index) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index) const;
bool CheckLink(const UEPtr<UConnector> &connector, int item_index, int conn_index) const;


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

// Template methods UConnector
// ���������� ������ �����������
template<typename T>
ULinksListT<T>& UConnector::GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
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
	 connector.Index=-1;//property->GetMinRange();
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



}
#endif

