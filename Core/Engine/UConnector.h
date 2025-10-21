/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAConnectorH
#define UAConnectorH

#include "UContainer.h"
#include "UEPtr.h"

namespace RDK {

//class UIPropertyIO;



class UItem;

// �������� ������������� �������� "UConnectedITEM"
struct RDK_LIB_TYPE UCItem
{
// ������������ �������
UItem* Item;

// ������ ������
int Index;

// ��� ������
std::string Name;

// --------------------------
// ������������ � �����������
// --------------------------
UCItem(void);
UCItem(const UCItem &copy);
// --------------------------

// --------------------------
// ���������
// --------------------------
bool operator == (const UCItem &value);
bool operator != (const UCItem &value);
// --------------------------
};

// �������� �������� ����� � �������� item "UConnectedLINK"
struct RDK_LIB_TYPE UCLink
{
// ������ ������
int Output;

// ������ �����
int Input;

// ��� ������
std::string OutputName;

// ��� �����
std::string InputName;

// --------------------------
// ������������ � �����������
// --------------------------
UCLink(void);
UCLink(const UCLink &copy);
// --------------------------
};

// ��������� - ������ ���������� �� ������������ ��������
class RDK_LIB_TYPE UCItemList
{
protected: // ���������
// ������ ����������
int Size;

protected: // ������
// ������ ���������
UCItem* Data;

protected: // ��������� ����������
// �������� ������ ����������
int RealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UCItemList(void);
UCItemList(const UCItemList &copy);
virtual ~UCItemList(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void Clear(void);

// ������ ������ ���������� � ����������� ���������
void Resize(int newsize);

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ��������
UCItem Find(const UEPtr<UItem> &item, int index=0) const;
UCItem Find(const UItem *const item, int index=0) const;

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ������ ��� -1 ���� ������� �� ������
int Find(const UCItem &item, int index=0) const;

// ��������� � ����� ���������� �������
void Add(const UCItem &item);

// ��������� � ����� ���������� ������� ������ ���� ����� ��������� ��� ���
// ���������� true � ������ ��������� ����������
bool AddUnique(const UCItem &item);

// ������� ������� �� �������
void Del(int index);

// ������� ������� �� ���������
void Del(UCItem *item);
// --------------------------

// --------------------------
// ������ �������
// --------------------------
// �������� ������������
UCItemList& operator = (const UCItemList &copy);

// �������� ������� � ��������
UCItem& operator [] (int index);
const UCItem& operator [] (int index) const;
//const UCItem& operator () (int index) const;

// �������� ������� � �������
UCItem* GetData(void);

// �������� ������� � �������
int GetSize(void) const;
// --------------------------
};

class UIPropertyInput;
class UIPropertyOutput;

class RDK_LIB_TYPE UConnector: public UContainer
{
friend class UItem;
protected: // ��������� ������
// ������ ���������� �� ������������ �������� ���� � �������
// <��� �������� � �������� ����������, ������ ������������ ���������>
std::map<std::string, std::vector<UCItem> > ConnectedItemList;

// ��������, ������������ � ������, ����� ���������� ������ �� CItem ������� ����������
UCItem DummyItem;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UConnector(void);
virtual ~UConnector(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
/// ���������� ����� ������� � ������� ���-�� ����������
virtual int GetNumActiveInputs(void) const;

/// ���������� ����� ������� ������������ ��������� � ��������� ��������
virtual int GetNumActiveInputs(const NameT &connector_property_name) const;

/// ���������� ����� ����������� � ������� ���������� �������� ��������
virtual int GetNumActiveOutputs(const NameT &item_property_name) const=0;
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ���� ������ ��������� ����
//virtual NameT FindFreeInputName(void)=0;

// ���������� ������������ � ����� ���������� ������ �� �������
const UCItem& GetCItem(const NameT &connector_property_name, const UEPtr<UItem> &item, int &index) const;
const UCItem& GetCItem(const NameT &connector_property_name, int index) const;
void GetCItem(const NameT &connector_property_name, std::vector<UCItem> &buffer) const;

// ���������� ���������� �� �������� ������ � ���� item ��� -1, -1
// ���� ����� ����� �����������
virtual void GetCLink(const UEPtr<UItem> &item, std::vector<UCLink> &buffer) const;
virtual void GetCLink(const UItem* const item, std::vector<UCLink> &buffer) const;
// --------------------------

// --------------------------
// ������ ������� � �������� ������ � �������
// --------------------------
/// ���� ��������-���� �� ��������� �������
virtual void FindInputProperty(const NameT &connector_property_name, UIPropertyInput* &property) const;

/// ���������� ��������� �� �������� ������������� ����� ����������-���������
virtual void FindConnectedProperty(const NameT &item_property_name, int index, UIPropertyInput* &property) const=0;
// --------------------------

// ----------------------
// ���������������� ������
// ----------------------
protected:
// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ���������� false ���� na ��� ��������� � ����� �����.
// ��� �������� ����������� c_index �������� �������� ������ ������������� �����
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

// ��������� ��� ����� � ��������� ���� 'na'
virtual void DisconnectFromItem(UEPtr<UItem> na);

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name);

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index � c_index
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index);

// ��������� �������� ����� ��������� ������������ �����
virtual bool AConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name);

// ��������� �������� ����� ��������� ���������� �����
virtual void ADisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name);

public:
// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
virtual void DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index);

virtual void DisconnectFromIndex(const NameT &connector_property_name);

// ��������� ��� ������� �����
virtual void DisconnectAllItems(void);

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual void DisconnectByObject(UEPtr<UContainer> brklevel);

public:
// ���������, ��������� �� ����������� ��������� item � ����� ����������
//bool CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name);

// ���������, ���������� �� ����� � �������� �����������
bool CheckLink(const UEPtr<UItem> &item) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name, const NameT &connector_property_name) const;

// ���������� ������ �����������
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const;

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const;
// ----------------------

// --------------------------
// ������ ���������� ������
// --------------------------
public:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool Default(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool Build(void);
// --------------------------

public: // ������ �������� ����������
// ������������ ������ �����
struct EInvalidInputIndex: public EInvalidIndex
{
 explicit EInvalidInputIndex(int index) : EInvalidIndex(index) {}
};

// ����� ���� �� ����������
struct EInputIndexNotExist: public EInvalidIndex
{
 explicit EInputIndexNotExist(int index) : EInvalidIndex(index) {}
};
};


// Template methods UConnector
// ���������� ������ �����������
template<typename T>
ULinksListT<T>& UConnector::GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(std::shared_ptr<UContainer>(netlevel.Get()),connector.Id);
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
     if(reinterpret_cast<UContainer*>(I->second[i].Item)->CheckOwner(internal_level))
      continue;
    }
    reinterpret_cast<UContainer*>(I->second[i].Item)->GetLongId(std::shared_ptr<UContainer>(netlevel.Get()),item.Id);
    UIPropertyInput* property=0;
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
   if(reinterpret_cast<UContainer*>(I->second[i].Item) == cont)
   {
    reinterpret_cast<UContainer*>(I->second[i].Item)->GetLongId(netlevel,item.Id);
    UIPropertyInput* property=0;
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
