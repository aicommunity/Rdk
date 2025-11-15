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
#include <glog/logging.h>
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

  GetLongId(netlevel,item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 // CRITICAL: Make a copy of RelatedConnectors before iteration to avoid use-after-free
 // During UNet::Copy, the prototype object may be destroyed while RelatedConnectors is still being iterated
 // By copying the map, we ensure that shared_ptr references remain valid even if the original map is destroyed
 std::map<std::string, std::vector<PUAConnector> > related_connectors_copy;
 try {
  related_connectors_copy = RelatedConnectors;
 } catch (...) {
  LOG(ERROR) << "UItem::GetLinks - exception when copying RelatedConnectors, returning empty linkslist";
  return linkslist;
 }

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=related_connectors_copy.begin();
 for(;I != related_connectors_copy.end();++I)
 {
  // CRITICAL: Save I->first to local copy to avoid use-after-free
  std::string output_name_key = I->first;
  
  link.Connector.clear();
  for(size_t i=0;i<I->second.size();i++)
  {
   // CRITICAL: Check if shared_ptr is valid before calling .get()
   // During UNet::Copy, objects may be destroyed while RelatedConnectors is still being iterated
   if(!I->second[i])
   {
    LOG(WARNING) << "UItem::GetLinks - invalid shared_ptr in RelatedConnectors, skipping";
    continue;
   }
   UConnector* curr_conn = nullptr;
   try {
    curr_conn = I->second[i].get();
   } catch (...) {
    LOG(WARNING) << "UItem::GetLinks - exception when calling .get() on shared_ptr, skipping";
    continue;
   }
   if(!curr_conn)
   {
    LOG(WARNING) << "UItem::GetLinks - curr_conn is nullptr after get(), skipping";
    continue;
   }
   if(exclude_internals)
   {
	try {
	 if(curr_conn->CheckOwner(internal_level))
	  continue;
	} catch (...) {
	 LOG(WARNING) << "UItem::GetLinks - exception in CheckOwner, skipping";
	 continue;
	}
   }
    // Use netlevel directly - it's already a shared_ptr, don't create new one from .get()
    try {
     curr_conn->GetLongId(netlevel,connector.Id);
    } catch (...) {
     LOG(WARNING) << "UItem::GetLinks - exception in GetLongId, skipping";
     continue;
    }
   if(connector.Id.size() != 0)
   {
	std::vector<UCLink> buffer;
	try {
	 // CRITICAL: Use shared_from_this() instead of creating new shared_ptr from raw pointer
	 // Creating shared_ptr from raw pointer causes bad-free if object was created via make_shared
	 // shared_from_this() returns existing shared_ptr, which is safe
	 // Note: const_cast is safe here because we're only calling shared_from_this() which doesn't modify the object
	 std::shared_ptr<UItem> this_shared;
	 try {
	  this_shared = const_cast<UItem*>(this)->get_shared_from_this();
	 } catch (const std::bad_weak_ptr&) {
	  // Object is not managed by shared_ptr, skip GetCLink
	  LOG(WARNING) << "UItem::GetLinks - bad_weak_ptr in get_shared_from_this(), skipping GetCLink";
	  continue;
	 } catch (...) {
	  LOG(WARNING) << "UItem::GetLinks - exception in get_shared_from_this(), skipping GetCLink";
	  continue;
	 }
	 if(!this_shared)
	 {
	  LOG(WARNING) << "UItem::GetLinks - get_shared_from_this() returned nullptr, skipping GetCLink";
	  continue;
	 }
	 curr_conn->GetCLink(this_shared, buffer);
	} catch (...) {
	 LOG(WARNING) << "UItem::GetLinks - exception in GetCLink, skipping";
	 continue;
	}
	for(size_t k=0;k<buffer.size();k++)
	{
	 // CRITICAL: Save buffer[k].OutputName to local copy before comparison
	 // buffer[k] may be destroyed or moved during iteration, so we need a copy
	 std::string buffer_output_name;
	 try {
	  buffer_output_name = buffer[k].OutputName;
	 } catch (...) {
	  LOG(WARNING) << "UItem::GetLinks - exception when copying buffer[k].OutputName, skipping";
	  continue;
	 }
	 // Use saved copy instead of I->first to avoid use-after-free
	 try {
	  if(buffer_output_name == output_name_key)
	  {
	   link.Item.Index=buffer[k].Output;
	   // CRITICAL: Save all string members before using them
	   std::string buffer_output_name_copy;
	   std::string buffer_input_name_copy;
	   try {
	    buffer_output_name_copy = buffer[k].OutputName;
	    buffer_input_name_copy = buffer[k].InputName;
	   } catch (...) {
	    LOG(WARNING) << "UItem::GetLinks - exception when copying buffer string members, skipping";
	    continue;
	   }
	   link.Item.Name=buffer_output_name_copy;
	   connector.Index=buffer[k].Input;
	   connector.Name=buffer_input_name_copy;

	   link.Connector.push_back(connector);
	   linkslist.Set(link);
	  }
	 } catch (...) {
	  LOG(WARNING) << "UItem::GetLinks - exception during string comparison or assignment, skipping";
	  continue;
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
  // CRITICAL: Save I->first to local copy to avoid use-after-free if RelatedConnectors is destroyed
  std::string output_name_key = I->first;
  
  link.Connector.clear();
  for(size_t i=0;i<I->second.size();i++)
  {
   // CRITICAL: Check if shared_ptr is valid before calling .get()
   if(!I->second[i])
   {
    LOG(WARNING) << "UItem::GetPersonalLinks - invalid shared_ptr in RelatedConnectors, skipping";
    continue;
   }
   UConnector* curr_conn=I->second[i].get();
   if(!curr_conn)
   {
    LOG(WARNING) << "UItem::GetPersonalLinks - curr_conn is nullptr after get(), skipping";
    continue;
   }
   if(curr_conn != cont.get())
	continue;
   curr_conn->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	std::vector<UCLink> buffer;
	// CRITICAL: Use shared_from_this() instead of creating new shared_ptr from raw pointer
	// Creating shared_ptr from raw pointer causes bad-free if object was created via make_shared
	// Note: const_cast is safe here because we're only calling shared_from_this() which doesn't modify the object
	std::shared_ptr<UItem> this_shared;
	try {
	 this_shared = const_cast<UItem*>(this)->get_shared_from_this();
	} catch (const std::bad_weak_ptr&) {
	 LOG(WARNING) << "UItem::GetPersonalLinks - bad_weak_ptr in get_shared_from_this(), skipping GetCLink";
	 continue;
	} catch (...) {
	 LOG(WARNING) << "UItem::GetPersonalLinks - exception in get_shared_from_this(), skipping GetCLink";
	 continue;
	}
	if(!this_shared)
	{
	 LOG(WARNING) << "UItem::GetPersonalLinks - get_shared_from_this() returned nullptr, skipping GetCLink";
	 continue;
	}
	curr_conn->GetCLink(this_shared, buffer);
	for(size_t k=0;k<buffer.size();k++)
	{
	 // CRITICAL: Save buffer[k].OutputName to local copy before comparison
	 // buffer[k] may be destroyed or moved during iteration, so we need a copy
	 std::string buffer_output_name;
	 try {
	  buffer_output_name = buffer[k].OutputName;
	 } catch (...) {
	  LOG(WARNING) << "UItem::GetPersonalLinks - exception when copying buffer[k].OutputName, skipping";
	  continue;
	 }
	 // Use saved copy instead of I->first to avoid use-after-free
	 try {
	  if(buffer_output_name == output_name_key)
	  {
	   link.Item.Index=buffer[k].Output;
	   // CRITICAL: Save all string members before using them
	   std::string buffer_output_name_copy;
	   std::string buffer_input_name_copy;
	   try {
	    buffer_output_name_copy = buffer[k].OutputName;
	    buffer_input_name_copy = buffer[k].InputName;
	   } catch (...) {
	    LOG(WARNING) << "UItem::GetPersonalLinks - exception when copying buffer string members, skipping";
	    continue;
	   }
	   link.Item.Name=buffer_output_name_copy;
	   connector.Index=buffer[k].Input;
	   connector.Name=buffer_input_name_copy;

	   link.Connector.push_back(connector);
	   linkslist.Set(link);
	  }
	 } catch (...) {
	  LOG(WARNING) << "UItem::GetPersonalLinks - exception during string comparison or assignment, skipping";
	  continue;
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
 {
  // CRITICAL: Save I->first to local copy to avoid use-after-free if RelatedConnectors is destroyed
  std::string output_name_key = I->first;
  
  for(size_t i=0;i<I->second.size();i++)
  {
   // CRITICAL: Check if shared_ptr is valid before calling .get()
   if(!I->second[i])
   {
    LOG(WARNING) << "UItem::GetFullItemLinks - invalid shared_ptr in RelatedConnectors, skipping";
    continue;
   }
   UConnector* curr_conn=I->second[i].get();
   if(!curr_conn)
   {
    LOG(WARNING) << "UItem::GetFullItemLinks - curr_conn is nullptr after get(), skipping";
    continue;
   }
   if(!curr_conn->CheckOwner(comp) && curr_conn != comp.get())
	continue;
   curr_conn->GetLongId(netlevel,connector.Id);
   if(connector.Id.GetSize() != 0)
   {
	std::vector<UCLink> buffer;
	// CRITICAL: Use shared_from_this() instead of creating new shared_ptr from raw pointer
	// Creating shared_ptr from raw pointer causes bad-free if object was created via make_shared
	// Note: const_cast is safe here because we're only calling shared_from_this() which doesn't modify the object
	std::shared_ptr<UItem> this_shared;
	try {
	 this_shared = const_cast<UItem*>(this)->get_shared_from_this();
	} catch (const std::bad_weak_ptr&) {
	 LOG(WARNING) << "UItem::GetFullItemLinks - bad_weak_ptr in get_shared_from_this(), skipping GetCLink";
	 continue;
	} catch (...) {
	 LOG(WARNING) << "UItem::GetFullItemLinks - exception in get_shared_from_this(), skipping GetCLink";
	 continue;
	}
	if(!this_shared)
	{
	 LOG(WARNING) << "UItem::GetFullItemLinks - get_shared_from_this() returned nullptr, skipping GetCLink";
	 continue;
	}
	curr_conn->GetCLink(this_shared, buffer);
	for(size_t k=0;k<buffer.size();k++)
	{
	 // CRITICAL: Save buffer[k].OutputName to local copy before comparison
	 // buffer[k] may be destroyed or moved during iteration, so we need a copy
	 std::string buffer_output_name;
	 try {
	  buffer_output_name = buffer[k].OutputName;
	 } catch (...) {
	  LOG(WARNING) << "UItem::GetFullItemLinks - exception when copying buffer[k].OutputName, skipping";
	  continue;
	 }
	 // Use saved copy instead of I->first to avoid use-after-free
	 try {
	  if(buffer_output_name == output_name_key)
	  {
	   link.Item.Index=buffer[k].Output;
	   // CRITICAL: Save all string members before using them
	   std::string buffer_output_name_copy;
	   std::string buffer_input_name_copy;
	   try {
	    buffer_output_name_copy = buffer[k].OutputName;
	    buffer_input_name_copy = buffer[k].InputName;
	   } catch (...) {
	    LOG(WARNING) << "UItem::GetFullItemLinks - exception when copying buffer string members, skipping";
	    continue;
	   }
	   link.Item.Name=buffer_output_name_copy;
	   connector.Index=buffer[k].Input;
	   connector.Name=buffer_input_name_copy;

	   link.Connector.push_back(connector);
	   linkslist.Set(link);
	  }
	 } catch (...) {
	  LOG(WARNING) << "UItem::GetFullItemLinks - exception during string comparison or assignment, skipping";
	  continue;
	 }
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

