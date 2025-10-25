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
#include <memory>
//#include "UEInterface.h"

namespace RDK {

typedef std::shared_ptr<UConnector> PUAConnector;

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

// Helper методы для безопасного получения shared_ptr на this
std::shared_ptr<UItem> get_shared_from_this() {
    return std::static_pointer_cast<UItem>(
        UComponent::shared_from_this()
    );
}

std::weak_ptr<UItem> get_weak_from_this() {
    return std::static_pointer_cast<UItem>(
        UComponent::shared_from_this()
    );
}

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

/// ���������� ��������� �� �������� ������������� ����� ����������-���������
virtual void FindConnectedProperty(const NameT &item_property_name, int index, UIPropertyInput* &property) const;
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
virtual bool ConnectToItem(std::shared_ptr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

// ������������� ����� � ����������� 'c'
virtual bool Connect(std::shared_ptr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
virtual void Disconnect(std::shared_ptr<UConnector> c);

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
virtual void Disconnect(std::shared_ptr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index);

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
virtual void DisconnectBy(std::shared_ptr<UContainer> brklevel);

// ����������������� ��� ����� ����� item �� ����� connectors ������� ��������
// ������ �� ����� item
virtual void BuildLinks(void);

// ���������� ��������� �� ��������� �� ������ �����������
// �� Id 'id'.
virtual std::shared_ptr<UConnector> GetAConnector(const UId &id, int index) const;

// ����������  ��������� �� ������ �����������.
virtual std::shared_ptr<UConnector> GetAConnectorByIndex(const NameT &item_property_name, int index) const;

// ���������, ���������� �� ����� � �������� �����������
bool CheckLink(const std::shared_ptr<UConnector> &connector, int connected_c_index) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const std::shared_ptr<UConnector> &connector, const NameT &item_property_name) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const std::shared_ptr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index) const;
//bool CheckLink(const std::shared_ptr<UConnector> &connector, int item_index, int conn_index) const;


// ���������� ������ �����������
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel, bool exclude_internals=false, std::shared_ptr<UContainer> internal_level=0) const;

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& GetPersonalLinks(std::shared_ptr<UContainer> cont, ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel) const;

// ���������� ������ ����������� ����� ���������� � ���� �������� ���������
// � ��������� ���������� comp � ���� ��� �������� �����������
template<typename T>
ULinksListT<T>& GetFullItemLinks(ULinksListT<T> &linkslist, std::shared_ptr<UItem> comp,
                            std::shared_ptr<UContainer> netlevel) const;
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
ULinksListT<T>& UItem::GetLinks(ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel, bool exclude_internals, std::shared_ptr<UContainer> internal_level) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

  GetLongId(safe_shared_cast<UContainer>(netlevel.get()),item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
 {
  link.Connector.clear();
  for(size_t i=0;i<I->second.size();i++)
  {
   UConnector* curr_conn=I->second[i].get();
   if(exclude_internals)
   {
	if(curr_conn->CheckOwner(internal_level))
	 continue;
   }
    curr_conn->GetLongId(std::shared_ptr<UContainer>(netlevel.get()),connector.Id);
   if(connector.Id.size() != 0)
   {
	std::vector<UCLink> buffer;
	curr_conn->GetCLink(std::shared_ptr<UItem>(const_cast<UItem*>(this)),buffer);
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

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& UItem::GetPersonalLinks(std::shared_ptr<UContainer> cont, ULinksListT<T> &linkslist, std::shared_ptr<UContainer> netlevel) const
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
   UConnector* curr_conn=I->second[i].get();
   if(curr_conn != cont.get())
	continue;
   curr_conn->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	std::vector<UCLink> buffer;
	curr_conn->GetCLink(std::shared_ptr<UItem>(const_cast<UItem*>(this)),buffer);
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
ULinksListT<T>& UItem::GetFullItemLinks(ULinksListT<T> &linkslist, std::shared_ptr<UItem> comp,
                                     std::shared_ptr<UContainer> netlevel) const
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
   UConnector* curr_conn=I->second[i].get();
   if(!curr_conn->CheckOwner(comp) && curr_conn != comp.get())
	continue;
   curr_conn->GetLongId(netlevel,connector.Id);
   if(connector.Id.GetSize() != 0)
   {
	std::vector<UCLink> buffer;
	curr_conn->GetCLink(std::shared_ptr<UItem>(const_cast<UItem*>(this)),buffer);
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
  std::shared_ptr<UItem> item=dynamic_cast<UItem*>(PComponents[i].operator->());
  item->GetFullItemLinks(linkslist, comp, netlevel);
 }

 return linkslist;
}


}
#endif

