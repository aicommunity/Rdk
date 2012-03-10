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
// Конструкторы и деструкторы
// --------------------------
UANet::UANet(void)
{
}

UANet::~UANet(void)
{
// BreakLinks(this);
}
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
ULinksList& UANet::GetLinks(ULinksList &linkslist, UEPtr<UAContainer> netlevel) const
{
 GetLinks(const_cast<UANet*>(this), linkslist, netlevel);
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
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool UANet::CheckComponentType(UEPtr<UAContainer> comp) const
{
 return (dynamic_pointer_cast<UAItem>(comp) ||
 dynamic_pointer_cast<UANet>(comp) || dynamic_pointer_cast<UAConnector>(comp))?true:false;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool UANet::AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
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
// Системные методы управления объектом
// --------------------------
// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
bool UANet::Copy(UEPtr<UAContainer> target, UEPtr<UAContainerStorage> stor, bool copystate) const
{
 ULinksList linkslist;
 ULinksList oldlinkslist;

 if(!dynamic_pointer_cast<UANet>(target))
  return false;

 if(UADItem::Copy(target,stor,copystate))
  {
//   ((UANet*)target)->GetLinks(oldlinkslist,target);
   static_pointer_cast<UANet>(target)->BreakLinks();
//   oldlinkslist.Clear();
//   ((UANet*)target)->GetLinks(oldlinkslist,target);

   if(static_pointer_cast<UANet>(target)->CreateLinks(GetLinks(linkslist,const_cast<UANet*>(this))))
    return true;
  }

 return false;
}

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UANet::Free(void)
{
// BreakLinks(this);
 UADItem::Free();
}
// --------------------------

// ----------------------
// Методы управления связями
// ----------------------
// Устанавливает новую связь 'link'
bool UANet::CreateLink(const ULink &link)
{
 UEPtr<UAItem> pitem;
 bool res=true;
 if(link.Item.Id.GetSize() == 0 || link.Item.Id[0] == ForbiddenId)
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(link.Item.Id));

 if(!pitem)
  return false;

 for(size_t i=0;i<link.Connector.size();i++)
 {
  UEPtr<UAConnector> pconnector=0;
  const ULinkSide &connector=link.Connector[i];
  if(connector.Id.GetSize() == 0 || connector.Id[0] == ForbiddenId)
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

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
bool UANet::CreateLink(const ULinkSide &item, const ULinkSide &connector)
{
 UEPtr<UAItem> pitem;
 if(item.Id.GetSize() == 0 || item.Id[0] == ForbiddenId)
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(item.Id));

 UEPtr<UAConnector> pconnector=0;
 if(connector.Id.GetSize() == 0 || connector.Id[0] == ForbiddenId)
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


// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
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

// Устанавливает все связи из массива 'linkslist'.
bool UANet::CreateLinks(const ULinksList &linkslist)
{
 bool res=true;

 int i=0;
 for(i=0;i<linkslist.GetSize();i++)
 {
  res&=CreateLink(linkslist[i]);
  if(!res)
   break;
 }

 if(!res)
  return false;

 return res;
}

// Разрывает все связи с выходом элемента сети, если
// 'id' - есть Id элемента сети.
// Иначе, если 'id' - Id коннектора, то метод разрывает
// связи этого коннектора.
bool UANet::BreakLink(const ULinkSide &id)
{
 UEPtr<UAContainer> pointer=GetComponentL(id.Id);

 UEPtr<UAConnector> connector=dynamic_pointer_cast<UAConnector>(pointer);
 if(connector)
  {
   connector->DisconnectAllItems();
   return true;
  }
// !!! Переделать! Сейчас никогда не доходит до кода ниже, т.к. каждый Item
// является еще и коннектором
 UEPtr<UAItem> item=dynamic_pointer_cast<UAItem>(pointer);
 if(item)
  {
   item->DisconnectAll();
   return true;
  }
 return false;
}

// Разрывает связь 'link'
bool UANet::BreakLink(const ULink &link)
{
 UEPtr<UAItem> pitem=0;
 if(link.Item.Id.GetSize() == 0 || link.Item.Id[0] == ForbiddenId)
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(link.Item.Id));

 if(!pitem)
  return false;

 for(size_t i=0;i<link.Connector.size();i++)
 {
  UEPtr<UAConnector> pconnector=0;
  const ULinkSide &connector=link.Connector[i];

  if(connector.Id.GetSize() == 0 || connector.Id[0] == ForbiddenId)
   pconnector=this;
  else
   pconnector=dynamic_pointer_cast<UAConnector>(GetComponentL(connector.Id));

  if(!pitem || !pconnector)
   return false;

  pitem->Disconnect(pconnector);
 }

 return true;
}

// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
bool UANet::BreakLink(const ULinkSide &item, const ULinkSide &connector)
{
 UEPtr<UAItem> pitem=0;
 if(item.Id.GetSize() == 0 || item.Id[0] == ForbiddenId)
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UAItem>(GetComponentL(item.Id));

 UEPtr<UAConnector> pconnector=0;
 if(connector.Id.GetSize() == 0 || connector.Id[0] == ForbiddenId)
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


// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
bool UANet::BreakLink(const NameT &itemname, int item_index,
                        const NameT &connectorname, int connector_index)
{
 UEPtr<UAItem> item=dynamic_pointer_cast<UAItem>(GetComponentL(itemname));
 UEPtr<UAConnector> connector=dynamic_pointer_cast<UAConnector>(GetComponentL(connectorname));

 item->Disconnect(connector);

 return true;
}

// Разрывает все связи сети
// исключая ее внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
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

// Разрывает заданные связи сети
bool UANet::BreakLinks(const ULinksList &linkslist)
{
 bool res=true;

 for(int i=0;i<linkslist.GetSize();i++)
  res&=BreakLink(linkslist[i]);

 return res;
}

// Разрывает все внутренние связи сети.
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
// ----------------------

// --------------------------
// Скрытые методы доступа к свойствам
// --------------------------
ULinksList& UANet::GetLinks(UEPtr<UAContainer> cont, ULinksList &linkslist, UEPtr<UAContainer> netlevel) const
{
/* if(dynamic_cast<const UANet*>(cont))
  static_cast<const UANet*>(cont)->GetLinks(linkslist,netlevel);
 else*/
 {
  if(dynamic_pointer_cast<UAItem>(cont))
  {
   static_pointer_cast<UAConnector>(cont)->GetLinks(linkslist,netlevel);
   static_pointer_cast<UAItem>(cont)->GetLinks(linkslist,netlevel);
  }
  else
  if(dynamic_pointer_cast<UAConnector>(cont))
   static_pointer_cast<UAConnector>(cont)->GetLinks(linkslist,netlevel);
 }

 for(int i=0;i<cont->GetNumComponents();i++)
  GetLinks(cont->GetComponentByIndex(i), linkslist, netlevel);

 return linkslist;
}
// --------------------------

}
#endif

