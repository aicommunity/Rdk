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
// : NADItem(name)
{
}

UANet::~UANet(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
ULinksList& UANet::GetLinks(ULinksList &linkslist, const UAContainer *netlevel) const
{
 GetLinks(this, linkslist, netlevel);
/*
 for(size_t i=0;i<NumComponents;i++)
  if(dynamic_cast<UANet*>(PComponents[i]))
   static_cast<UANet*>(PComponents[i])->GetLinks(linkslist,netlevel);
  else
  {
   if(dynamic_cast<UAItem*>(PComponents[i]))
   {
    static_cast<UAConnector*>(PComponents[i])->GetLinks(linkslist,netlevel);
    static_cast<UAItem*>(PComponents[i])->GetLinks(linkslist,netlevel);
   }
   else
   if(dynamic_cast<UAConnector*>(PComponents[i]))
    static_cast<UAConnector*>(PComponents[i])->GetLinks(linkslist,netlevel);
  }*/

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
bool UANet::CheckComponentType(const UAContainer* comp) const
{
 return (dynamic_cast<const UAItem*>(comp) ||
 dynamic_cast<const UANet*>(comp) || dynamic_cast<const UAConnector*>(comp))?true:false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool UANet::AAddComponent(UAContainer* comp, UIPointer* pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool UANet::ADelComponent(UAContainer* comp)
{
 if(dynamic_cast<UAItem*>(comp))
  ((UAItem*)comp)->DisconnectBy(this);
 else
 if(dynamic_cast<UANet*>(comp))
  ((UANet*)comp)->BreakLinks(this);
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
bool UANet::Copy(UAContainer *target, UAContainerStorage *stor, bool copystate) const
{
 ULinksList linkslist;
 ULinksList oldlinkslist;

 if(!dynamic_cast<UANet*>(target))
  return false;

 if(UADItem::Copy(target,stor,copystate))
  {
   ((UANet*)target)->BreakLinks();
   if(((UANet*)target)->CreateLinks(GetLinks(linkslist,this)))
    return true;
  }

 return false;
}
// --------------------------

// ----------------------
// ������ ���������� �������
// ----------------------
// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
bool UANet::CreateLink(const ULinkSide &item, const ULinkSide &connector)
{
 UAItem *pitem=0;
 if(item.Id.GetSize() == 0 || item.Id[0] == ForbiddenId)
  pitem=this;
 else
  pitem=dynamic_cast<UAItem*>(GetComponentL(item.Id));

 UAConnector *pconnector=0;
 if(connector.Id.GetSize() == 0 || connector.Id[0] == ForbiddenId)
  pconnector=this;
 else
  pconnector=dynamic_cast<UAConnector*>(GetComponentL(connector.Id));

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
/*bool UANet::CreateLink(const NameT &item, int item_index,
                        const NameT &connector, int connector_index)
{
 UAItem *pitem=dynamic_cast<UAItem*>(GetComponentL(item));
 UAConnector *pconnector=dynamic_cast<UAConnector*>(GetComponentL(connector));

 if(!pitem)
  return false;

 if(!pconnector)
  return false;

 if(!(pitem->Connect(pconnector,item_index,connector_index)))
  return false;

 return true;
}            */

// ������������� ��� ����� �� ������� 'linkslist'.
bool UANet::CreateLinks(const ULinksList &linkslist)
{
 bool res=true;

 for(int i=0;i<linkslist.GetSize();i++)
 {
  res&=CreateLink(linkslist[i].Item,linkslist[i].Connector);
 }

 return res;
}

// ��������� ��� ����� � ������� �������� ����, ����
// 'id' - ���� Id �������� ����.
// �����, ���� 'id' - Id ����������, �� ����� ���������
// ����� ����� ����������.
bool UANet::BreakLink(const ULinkSide &id)
{
 UAContainer *pointer=GetComponentL(id.Id);
 UAItem *item;
 UAConnector *connector;

 connector=dynamic_cast<UAConnector*>(pointer);
 if(connector)
  {
   connector->DisconnectAllItems();
   return true;
  }
// !!! ����������! ������ ������� �� ������� �� ���� ����, �.�. ������ Item
// �������� ��� � �����������
 item=dynamic_cast<UAItem*>(pointer);
 if(item)
  {
   item->DisconnectAll();
   return true;
  }
 return false;
}

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
bool UANet::BreakLink(const ULinkSide &item, const ULinkSide &connector)
{
 UAItem *pitem=0;
 if(item.Id.GetSize() == 0 || item.Id[0] == ForbiddenId)
  pitem=this;
 else
  pitem=dynamic_cast<UAItem*>(GetComponentL(item.Id));

 UAConnector *pconnector=0;
 if(connector.Id.GetSize() == 0 || connector.Id[0] == ForbiddenId)
  pconnector=this;
 else
  pconnector=dynamic_cast<UAConnector*>(GetComponentL(connector.Id));

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
/*bool UANet::BreakLink(const NameT &itemname, int item_index,
                        const NameT &connectorname, int connector_index)
{
 UAItem *item=dynamic_cast<NADItem*>(GetComponentL(itemname));
 UAConnector *connector=dynamic_cast<UAConnector*>(GetComponentL(connectorname));

 if(!item)
  return false;
 if(!connector)
  return false;

 item->Disconnect(connector);

 return true;
}           */

// ��������� ��� ����� ����
// �������� �� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
void UANet::BreakLinks(const UAContainer* brklevel)
{
 for(int i=0;i<NumComponents;i++)
  {
   if(dynamic_cast<UAItem*>(PComponents[i]))
    ((UAItem*)PComponents[i])->DisconnectBy(brklevel);
   else
   if(dynamic_cast<UANet*>(PComponents[i]))
    ((UANet*)PComponents[i])->BreakLinks(brklevel);
  }
}

// ��������� �������� ����� ����
bool UANet::BreakLinks(const ULinksList &linkslist)
{
 bool res=true;

 for(int i=0;i<linkslist.GetSize();i++)
  res&=BreakLink(linkslist[i].Item,linkslist[i].Connector);

 return res;
}

// ��������� ��� ���������� ����� ����.
void UANet::BreakLinks(void)
{
 for(int i=0;i<NumComponents;i++)
  if(dynamic_cast<UANet*>(PComponents[i]))
   ((UANet*)PComponents[i])->BreakLinks();
  else
  {
   if(dynamic_cast<UAItem*>(PComponents[i]))
    ((UAItem*)PComponents[i])->DisconnectAll();
   if(dynamic_cast<UAConnector*>(PComponents[i]))
    ((UAConnector*)PComponents[i])->DisconnectAllItems();
  }

 if(dynamic_cast<UAItem* const>(this))
  ((UAItem* const)this)->DisconnectAll();
 if(dynamic_cast<UAConnector*>(this))
  ((UAConnector* const)this)->DisconnectAllItems();
}
// ----------------------

// --------------------------
// ������� ������ ������� � ���������
// --------------------------
ULinksList& UANet::GetLinks(const UAContainer *cont, ULinksList &linkslist, const UAContainer *netlevel) const
{
/* if(dynamic_cast<const UANet*>(cont))
  static_cast<const UANet*>(cont)->GetLinks(linkslist,netlevel);
 else*/
 {
  if(dynamic_cast<const UAItem*>(cont))
  {
   static_cast<const UAConnector*>(cont)->GetLinks(linkslist,netlevel);
   static_cast<const UAItem*>(cont)->GetLinks(linkslist,netlevel);
  }
  else
  if(dynamic_cast<const UAConnector*>(cont))
   static_cast<const UAConnector*>(cont)->GetLinks(linkslist,netlevel);
 }

 for(int i=0;i<cont->GetNumComponents();i++)
  GetLinks(cont->GetComponentByIndex(i), linkslist, netlevel);

 return linkslist;
}
// --------------------------

}
#endif

