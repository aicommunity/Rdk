/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UANET_CPP
#define UANET_CPP

#include "UANet.h"

namespace RDK {

/* *************************************************************************** */
// --------------------------
// ������������ � �����������
// --------------------------
UANet::UANet(void)
{
}

UANet::~UANet(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
template<typename T>
ULinksListT<T>& UANet::GetLinks(ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel, bool exclude_internals, UEPtr<UAContainer> internal_level) const
{
 GetLinks(const_cast<UANet*>(this), linkslist, netlevel, exclude_internals, internal_level);

 return linkslist;
}

// ��������� ��� ����� ����� ����� ������������ � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
template<typename T>
ULinksListT<T>& UANet::GetPersonalLinks(UEPtr<RDK::UANet> cont, ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel, int sublevel)
{
 GetPersonalLinks(const_cast<UANet*>(this), cont, linkslist, netlevel);
/*
 for(int i=0;i<linkslist.GetSize();i++)
 {
  ULinkT<T> &link=linkslist[i];
  ULinkSideT<T> &item=link.item;
  ULinkSideT<T> &connector=link.connector;

  if()
 }
 // �����������
// linkslist.
  */
 return linkslist;
}

// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool UANet::CheckComponentType(UEPtr<UAContainer> comp) const
{
 return (dynamic_pointer_cast<UAItem>(comp) ||
 dynamic_pointer_cast<UANet>(comp) || dynamic_pointer_cast<UAConnector>(comp))?true:false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool UANet::AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool UANet::ADelComponent(UEPtr<UAContainer> comp)
{
 if(dynamic_pointer_cast<UAItem>(comp))
  static_pointer_cast<UAItem>(comp)->DisconnectBy(this);
 else
 if(dynamic_pointer_cast<UANet>(comp))
  static_pointer_cast<UANet>(comp)->BreakLinks(this);
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
bool UANet::Copy(UEPtr<UAContainer> target, UEPtr<UAContainerStorage> stor, bool copystate) const
{
 ULinksList linkslist;
 ULinksList oldlinkslist;

 if(!dynamic_pointer_cast<UANet>(target))
  return false;

 if(UADItem::Copy(target,stor,copystate))
  {
   static_pointer_cast<UANet>(target)->BreakLinks();

   if(static_pointer_cast<UANet>(target)->CreateLinks(GetLinks(linkslist,const_cast<UANet*>(this))))
    return true;
  }

 return false;
}

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UANet::Free(void)
{
 UADItem::Free();
}
// --------------------------

// ----------------------
// ������ ���������� �������
// ----------------------
// ������������� ����� ����� 'link'
template<typename T>
bool UANet::CreateLink(const ULinkT<T> &link)
{
 UEPtr<UAItem> pitem;
 bool res=true;
 if(!CheckLongId(link.Item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(link.Item.Id));

 if(!pitem)
  return false;

 for(size_t i=0;i<link.Connector.size();i++)
 {
  UEPtr<UAConnector> pconnector=0;
  const ULinkSideT<T> &connector=link.Connector[i];
  if(!CheckLongId(connector.Id))
   pconnector=this;
  else
   pconnector=dynamic_pointer_cast<UAConnector>(GetComponentL(connector.Id));

  if(!pconnector)
   res=false;

  if(!(pitem->Connect(pconnector,link.Item.Index,connector.Index)))
   res=false;
 }
 return res;
}

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
template<typename T>
bool UANet::CreateLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 UEPtr<UAItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(item.Id));

 UEPtr<UAConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UAConnector>(GetComponentL(connector.Id));

 if(!pitem || !pconnector)
  return false;

 if(!(pitem->Connect(pconnector,item.Index,connector.Index)))
  return false;

 return true;
}

bool UANet::CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return CreateLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}


// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
bool UANet::CreateLink(const NameT &item, int item_index,
                        const NameT &connector, int connector_index)
{
 UEPtr<UAItem> pitem=0;
 UEPtr<UAConnector> pconnector=0;
 if(!item.size())
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(item));

 if(!connector.size())
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UAConnector>(GetComponentL(connector));

 if(!pitem)
  return false;

 if(!pconnector)
  return false;

 if(!(pitem->Connect(pconnector,item_index,connector_index)))
  return false;

 return true;
}

// ������������� ��� ����� �� ������� 'linkslist'.
template<typename T>
bool UANet::CreateLinks(const ULinksListT<T> &linkslist, UEPtr<UANet> owner_level)
{
 bool res=true;

 int i=0;
 for(i=0;i<linkslist.GetSize();i++)
 {
  if(owner_level)
   res&=owner_level->CreateLink(linkslist[i]);
  else
   res&=CreateLink(linkslist[i]);
 }

 if(!res)
  return false;

 return res;
}

// ��������� ��� ����� � ������� �������� ����, ����
// 'id' - ���� Id �������� ����.
// �����, ���� 'id' - Id ����������, �� ����� ���������
// ����� ����� ����������.
template<typename T>
bool UANet::BreakLink(const ULinkSideT<T> &id)
{
 UEPtr<UAContainer> pointer=GetComponentL(id.Id);

 UEPtr<UAConnector> connector=dynamic_pointer_cast<UAConnector>(pointer);
 if(connector)
  {
   connector->DisconnectAllItems();
   return true;
  }
// !!! ����������! ������ ������� �� ������� �� ���� ����, �.�. ������ Item
// �������� ��� � �����������
 UEPtr<UAItem> item=dynamic_pointer_cast<UAItem>(pointer);
 if(item)
  {
   item->DisconnectAll();
   return true;
  }
 return false;
}

// ��������� ����� 'link'
template<typename T>
bool UANet::BreakLink(const ULinkT<T> &link)
{
 UEPtr<UAItem> pitem=0;
 if(!CheckLongId(Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(link.Item.Id));

 if(!pitem)
  return false;

 for(size_t i=0;i<link.Connector.size();i++)
 {
  UEPtr<UAConnector> pconnector=0;
  const ULinkSide &connector=link.Connector[i];

  if(!CheckLongId(connector.Id))
   pconnector=this;
  else
   pconnector=dynamic_pointer_cast<UAConnector>(GetComponentL(connector.Id));

  if(!pitem || !pconnector)
   return false;

  pitem->Disconnect(pconnector);
 }

 return true;
}

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
template<typename T>
bool UANet::BreakLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 UEPtr<UAItem> pitem=0;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(item.Id));

 UEPtr<UAConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UAConnector>(GetComponentL(connector.Id));

 if(!pitem || !pconnector)
  return false;

 pitem->Disconnect(pconnector);

 return true;
}

bool UANet::BreakLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return BreakLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}


// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
bool UANet::BreakLink(const NameT &itemname, int item_index,
                        const NameT &connectorname, int connector_index)
{
 UEPtr<UAItem> item=dynamic_pointer_cast<UAItem>(GetComponentL(itemname));
 UEPtr<UAConnector> connector=dynamic_pointer_cast<UAConnector>(GetComponentL(connectorname));

 item->Disconnect(connector);

 return true;
}

// ��������� ��� ����� ����
// �������� �� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UANet::BreakLinks(UEPtr<UAContainer> brklevel)
{
 for(int i=0;i<NumComponents;i++)
  {
   if(dynamic_pointer_cast<UAItem>(PComponents[i]))
	static_pointer_cast<UAItem>(PComponents[i])->DisconnectBy(brklevel);
   else
   if(dynamic_pointer_cast<UANet>(PComponents[i]))
    static_pointer_cast<UANet>(PComponents[i])->BreakLinks(brklevel);
  }
}

// ��������� �������� ����� ����
bool UANet::BreakLinks(const ULinksList &linkslist)
{
 bool res=true;

 for(int i=0;i<linkslist.GetSize();i++)
  res&=BreakLink(linkslist[i]);

 return res;
}

// ��������� ��� ���������� ����� ����.
void UANet::BreakLinks(void)
{
 for(int i=0;i<NumComponents;i++)
  if(dynamic_pointer_cast<UANet>(PComponents[i]))
   static_pointer_cast<UANet>(PComponents[i])->BreakLinks();
  else
  {
   if(dynamic_pointer_cast<UAItem>(PComponents[i]))
	static_pointer_cast<UAItem>(PComponents[i])->DisconnectAll();
   if(dynamic_pointer_cast<UAConnector>(PComponents[i]))
	static_pointer_cast<UAConnector>(PComponents[i])->DisconnectAllItems();
  }

 if(dynamic_cast<UAItem* const>(this))
  ((UAItem* const)this)->DisconnectAll();
 if(dynamic_cast<UAConnector* const>(this))
  ((UAConnector* const)this)->DisconnectAllItems();
}

// ���������, ���������� �� �������� �����
bool UANet::CheckLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return CheckLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}

template<typename T>
bool UANet::CheckLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 UEPtr<UAItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(item.Id));

 UEPtr<UAConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UAConnector>(GetComponentL(connector.Id));

 if(!pitem || !pconnector)
  return false;

 if(pitem->CheckLink(pconnector,item.Index, connector.Index))
  return true;

 return true;
}

bool UANet::CheckLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index)
{
 UEPtr<UAItem> item=dynamic_pointer_cast<UAItem>(GetComponentL(itemname));
 UEPtr<UAConnector> connector=dynamic_pointer_cast<UAConnector>(GetComponentL(connectorname));
 if(!item || !connector)
  return false;

 if(item->CheckLink(connector,item_index, connector_index))
  return true;

 return true;
}
// ----------------------

// --------------------------
// ������� ������ ������� � ���������
// --------------------------
template<typename T>
ULinksListT<T>& UANet::GetLinks(UEPtr<UAContainer> cont, ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel, bool exclude_internals, UEPtr<UAContainer> internal_level) const
{
 if(dynamic_pointer_cast<UAItem>(cont))
 {
  static_pointer_cast<UAConnector>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);
  static_pointer_cast<UAItem>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);
 }
 else
 if(dynamic_pointer_cast<UAConnector>(cont))
  static_pointer_cast<UAConnector>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetLinks(cont->GetComponentByIndex(i), linkslist, netlevel, exclude_internals,internal_level);

 return linkslist;
}

template<typename T>
ULinksListT<T>& UANet::GetPersonalLinks(UEPtr<UAContainer> cont, UEPtr<UAContainer> cont2, ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel) const
{
 if(dynamic_pointer_cast<UAItem>(cont))
 {
  static_pointer_cast<UAConnector>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);
  static_pointer_cast<UAItem>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);
 }
 else
 if(dynamic_pointer_cast<UAConnector>(cont))
  static_pointer_cast<UAConnector>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetPersonalLinks(cont->GetComponentByIndex(i), cont2, linkslist, netlevel);

 return linkslist;
}
// --------------------------

}
#endif

