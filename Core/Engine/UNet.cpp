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

#include "UNet.h"

namespace RDK {

/* *************************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UNet::UNet(void)
{
}

UNet::~UNet(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
template<typename T>
ULinksListT<T>& UNet::GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
 GetLinks(const_cast<UNet*>(this), linkslist, netlevel, exclude_internals, internal_level);

 return linkslist;
}

// Возращает все связи между двумя компонентами в виде xml в буфер buffer
// включая связи этого компонента
// если 'sublevel' == -1, то возвращает также все связи между объектом и любым дочерним компонентом
// второго объекта. Работает симметрично в обе стороны.
// если 'sublevel' == 0, то возвращает связи только между этими объектами
template<typename T>
ULinksListT<T>& UNet::GetPersonalLinks(UEPtr<RDK::UNet> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, int sublevel)
{
 GetPersonalLinks(const_cast<UNet*>(this), cont, linkslist, netlevel);
/*
 for(int i=0;i<linkslist.GetSize();i++)
 {
  ULinkT<T> &link=linkslist[i];
  ULinkSideT<T> &item=link.item;
  ULinkSideT<T> &connector=link.connector;

  if()
 }
 // Прореживаем
// linkslist.
  */
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
bool UNet::CheckComponentType(UEPtr<UContainer> comp) const
{
 return (dynamic_pointer_cast<UItem>(comp) ||
 dynamic_pointer_cast<UNet>(comp) || dynamic_pointer_cast<UConnector>(comp))?true:false;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool UNet::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool UNet::ADelComponent(UEPtr<UContainer> comp)
{
 if(dynamic_pointer_cast<UItem>(comp))
  static_pointer_cast<UItem>(comp)->DisconnectBy(this);
 else
 if(dynamic_pointer_cast<UNet>(comp))
  static_pointer_cast<UNet>(comp)->BreakLinks(this);
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
bool UNet::Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor, bool copystate) const
{
 ULinksList linkslist;
 ULinksList oldlinkslist;

 if(!dynamic_pointer_cast<UNet>(target))
  return false;

 if(UADItem::Copy(target,stor,copystate))
  {
   static_pointer_cast<UNet>(target)->BreakLinks();

   if(static_pointer_cast<UNet>(target)->CreateLinks(GetLinks(linkslist,const_cast<UNet*>(this))))
    return true;
  }

 return false;
}

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UNet::Free(void)
{
 UADItem::Free();
}
// --------------------------

// ----------------------
// Методы управления связями
// ----------------------
// Устанавливает новую связь 'link'
template<typename T>
bool UNet::CreateLink(const ULinkT<T> &link)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
 {
  res &=CreateLink(link.Item, link.Connector[i]);
 }

 return res;
/*
 UEPtr<UItem> pitem;
 bool res=true;
 if(!CheckLongId(link.Item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(link.Item.Id));

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+link.Item.Name);
  return false;
 }

 for(size_t i=0;i<link.Connector.size();i++)
 {
  UEPtr<UConnector> pconnector=0;
  const ULinkSideT<T> &connector=link.Connector[i];
  if(!CheckLongId(connector.Id))
   pconnector=this;
  else
   pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector.Id,true));

  if(!pconnector)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.Name);
   res=false;
  }

  if(!link.Item.Name.empty() && !connector.Name.empty())
  {
   int c_index;
   if(!(pitem->Connect(pconnector,link.Item.Name,connector.Name,c_index)))
	res=false;
  }
  else
  {
   if(link.Item.Index < 0)
   {
	LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, "Item index < 0");
	return false;
   }

   if(!(pitem->Connect(pconnector,link.Item.Index,connector.Index)))
	res=false;
  }
 }
 return res;  */
}

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
template<typename T>
bool UNet::CreateLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 UEPtr<UADItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UADItem>(GetComponentL(item.Id,true));

 if(item.Index < 0)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, "Item index < 0");
  return false;
 }

 UEPtr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector.Id,true));

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.Name);
  return false;
 }

 if(!item.Name.empty()/* && !connector.Name.empty()*/)
 {
  int c_index=-1;
  if(!(pitem->Connect(pconnector,item.Name,connector.Name,c_index)))
   return false;
 }
 else
 {
  if(item.Index < 0)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, "Item index < 0");
   return false;
  }

  if(!(pitem->Connect(pconnector,item.Index,connector.Index)))
   return false;
 }

 return true;
}

bool UNet::CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return CreateLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
bool UNet::CreateLink(const NameT &item, int item_index,
						const NameT &connector, int connector_index)
{
 return CreateLink(UStringLinkSide(item,item_index),UStringLinkSide(connector,connector_index));

/*
 UEPtr<UItem> pitem=0;
 UEPtr<UConnector> pconnector=0;
 if(!item.size())
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(item));

 if(!connector.size())
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector));

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector);
  return false;
 }

 if(item_index < 0)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, "Item index < 0");
  return false;
 }

 if(!(pitem->Connect(pconnector,item_index,connector_index)))
  return false;

 return true;    */
}

bool UNet::CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index)
{
 return CreateLink(UStringLinkSide(item,item_index),UStringLinkSide(connector,connector_index));

/* UEPtr<UItem> pitem=0;
 UEPtr<UConnector> pconnector=0;
 if(!item.size())
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(item,true));

 if(!connector.size())
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector,true));

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector);
  return false;
 }


 int i_item_index=pitem->FindOutputIndex(item_index);
 int i_connector_index=pconnector->FindInputIndex(connector_index);

 if(i_item_index < 0)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, "Item index < 0");
  return false;
 }

 if(!(pitem->Connect(pconnector,i_item_index,i_connector_index)))
  return false;

 return true;   */
}
/*
bool UNet::CreateLink(const NameT &item, const NameT &item_property_name,
						const NameT &connector, const NameT &connector_property_name)
{
 UEPtr<UItem> pitem=0;
 UEPtr<UConnector> pconnector=0;
 if(!item.size())
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(item,true));

 if(!connector.size())
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector,true));

 if(!pitem)
  return false;

 if(!pconnector)
  return false;

 int c_index;
 if(!(pitem->Connect(pconnector,item_property_name,connector_property_name, c_index)))
  return false;

 return true;
}            */


// Устанавливает все связи из массива 'linkslist'.
template<typename T>
bool UNet::CreateLinks(const ULinksListT<T> &linkslist, UEPtr<UNet> owner_level)
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

// Разрывает все связи с выходом элемента сети, если
// 'id' - есть Id элемента сети.
// Иначе, если 'id' - Id коннектора, то метод разрывает
// связи этого коннектора.
template<typename T>
bool UNet::BreakLink(const ULinkSideT<T> &id)
{
 UEPtr<UContainer> pointer=GetComponentL(id.Id,true);

 UEPtr<UConnector> connector=dynamic_pointer_cast<UConnector>(pointer);
 if(connector)
  {
   connector->DisconnectAllItems();
   return true;
  }
// !!! Переделать! Сейчас никогда не доходит до кода ниже, т.к. каждый Item
// является еще и коннектором
 UEPtr<UItem> item=dynamic_pointer_cast<UItem>(pointer);
 if(item)
  {
   item->DisconnectAll();
   return true;
  }
 return false;
}

// Разрывает связь 'link'
template<typename T>
bool UNet::BreakLink(const ULinkT<T> &link)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
  res&=BreakLink(link.Item,link.Connector[i]);

 return res;
/*
 UEPtr<UItem> pitem=0;
 if(!CheckLongId(Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(link.Item.Id,true));

 if(!pitem)
  return false;

 for(size_t i=0;i<link.Connector.size();i++)
 {
  UEPtr<UConnector> pconnector=0;
  const ULinkSide &connector=link.Connector[i];

  if(!CheckLongId(connector.Id))
   pconnector=this;
  else
   pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector.Id,true));

  if(!pitem)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+link.Item.Name);
   return false;
  }

  if(!pconnector)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.Name);
   return false;
  }

  pitem->Disconnect(pconnector, link.Item.Index,connector.Index);
 }

 return true;  */
}

// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
template<typename T>
bool UNet::BreakLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 UEPtr<UADItem> pitem=0;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UADItem>(GetComponentL(item.Id,true));

 UEPtr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector.Id,true));

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.Name);
  return false;
 }

 if(!item.Name.empty()/* && !connector.Name.empty()*/)
 {
  pitem->Disconnect(pconnector, item.Name, connector.Name);
 }
 else
 {
  pitem->Disconnect(pconnector, item.Index, connector.Index);
 }

 return true;
}

bool UNet::BreakLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return BreakLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}


// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
bool UNet::BreakLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index)
{
 return BreakLink(UStringLinkSide(itemname,item_index),UStringLinkSide(connectorname,connector_index));
/*
 UEPtr<UItem> item;
 UEPtr<UConnector> connector;
 if(itemname.size() == 0)
  item=this;
 else
  item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));

 if(connectorname.size() == 0)
  connector=this;
 else
  connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname,true));

 item->Disconnect(connector, item_index, connector_index);

 return true;  */
}

// Разрывает все связи между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
bool UNet::BreakLink(const NameT &itemname, const NameT &connectorname)
{
 UEPtr<UItem> item;
 UEPtr<UConnector> connector;
 if(itemname.size() == 0)
  item=this;
 else
  item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));

 if(connectorname.size() == 0)
  connector=this;
 else
  connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname,true));

 item->Disconnect(connector);

 return true;
}


bool UNet::BreakLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name)
{
 return BreakLink(UStringLinkSide(itemname,item_property_name), UStringLinkSide(connectorname,connector_property_name));

/* UEPtr<UItem> item;
 UEPtr<UConnector> connector;
 if(itemname.size() == 0)
  item=this;
 else
  item=dynamic_pointer_cast<UItem>(GetComponentL(itemname));

 if(connectorname.size() == 0)
  connector=this;
 else
  connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname));

 item->Disconnect(connector);

 return true;      */
}

// Разрывает все связи сети
// исключая ее внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
void UNet::BreakLinks(UEPtr<UContainer> brklevel)
{
 for(int i=0;i<NumComponents;i++)
  {
   if(dynamic_pointer_cast<UItem>(PComponents[i]))
	static_pointer_cast<UItem>(PComponents[i])->DisconnectBy(brklevel);
   else
   if(dynamic_pointer_cast<UNet>(PComponents[i]))
    static_pointer_cast<UNet>(PComponents[i])->BreakLinks(brklevel);
  }
}

// Разрывает заданные связи сети
bool UNet::BreakLinks(const ULinksList &linkslist)
{
 bool res=true;

 for(int i=0;i<linkslist.GetSize();i++)
  res&=BreakLink(linkslist[i]);

 return res;
}

// Разрывает все внутренние связи сети.
void UNet::BreakLinks(void)
{
 for(int i=0;i<NumComponents;i++)
  if(dynamic_pointer_cast<UNet>(PComponents[i]))
   static_pointer_cast<UNet>(PComponents[i])->BreakLinks();
  else
  {
   if(dynamic_pointer_cast<UItem>(PComponents[i]))
	static_pointer_cast<UItem>(PComponents[i])->DisconnectAll();
   if(dynamic_pointer_cast<UConnector>(PComponents[i]))
	static_pointer_cast<UConnector>(PComponents[i])->DisconnectAllItems();
  }

 if(dynamic_cast<UItem* const>(this))
  ((UItem* const)this)->DisconnectAll();
 if(dynamic_cast<UConnector* const>(this))
  ((UConnector* const)this)->DisconnectAllItems();
}

// Разрывает связь ко входу connector_index коннектора 'connectorid'
void UNet::BreakConnectorLink(const NameT &connectorname, int connector_index)
{
 UEPtr<UADItem> connector;
 if(connectorname.size() == 0)
  connector=this;
 else
  connector=dynamic_pointer_cast<UADItem>(GetComponentL(connectorname,true));

 if(!connector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return;
 }

 if(connector->GetNumInputs() <= connector_index)
  return;

 const UCItem &item = connector->GetCItem(connector_index);
// item.Item->Disconnect(connector);
// std::string item_name;
// item.Item->GetLongName(this,item_name);
// BreakLink(
  if(!item.Item)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
   return;
  }

  UEPtr<UADItem> ad_item=dynamic_cast<UADItem*>(item.Item);

  if(!ad_item)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Dynamic cast failed"));
   return;
  }

  ad_item->Disconnect(connector,item.Index, connector_index);
// connector->DisconnectFromIndex(connector_index);
}

void UNet::BreakConnectorLink(const NameT &connectorname, const NameT &connector_index)
{
 UEPtr<UConnector> connector;
 if(connectorname.size() == 0)
  connector=this;
 else
  connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname,true));

 if(!connector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return;
 }

// if(connector->GetNumInputs() <= connector_index)
//  return;

 std::vector<UCItem> buffer;
 connector->GetCItem(connector_index, buffer);

 for(size_t i=0;i<buffer.size();i++)
 {
  if(!buffer[i].Item)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+buffer[i].Name);
   return;
  }

  buffer[i].Item->Disconnect(connector,buffer[i].Name, connector_index);
 }
}

// Проверяет, существует ли заданная связь
bool UNet::CheckLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return CheckLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
}

template<typename T>
bool UNet::CheckLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector)
{
 UEPtr<UADItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UADItem>(GetComponentL(item.Id,true));

 UEPtr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector.Id,true));

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.Name);
  return false;
 }
  if(!item.Name.empty() && !connector.Name.empty())
  {
   if(pitem->CheckLink(pconnector,item.Name, connector.Name))
	return true;
  }
  else
  {
   if(pitem->CheckLink(pconnector,item.Index, connector.Index))
	return true;
  }

 return false;
}

bool UNet::CheckLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index)
{
 return CheckLink(UStringLinkSide(itemname,item_index),UStringLinkSide(connectorname,connector_index));
/*
 UEPtr<UItem> item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));
 UEPtr<UConnector> connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname,true));
 if(!item || !connector)
  return false;

 if(item->CheckLink(connector,item_index, connector_index))
  return true;

 return true;*/
}

bool UNet::CheckLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name)
{
 return CheckLink(UStringLinkSide(itemname,item_property_name),UStringLinkSide(connectorname,connector_property_name));

/*
 UEPtr<UItem> item=dynamic_pointer_cast<UItem>(GetComponentL(itemname));
 UEPtr<UConnector> connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname));
 if(!item || !connector)
  return false;

 if(item->CheckLink(connector,item_property_name, connector_property_name))
  return true;

 return false;*/
}

bool UNet::CheckLink(const NameT &itemname,const NameT &connectorname)
{
 UEPtr<UItem> item=dynamic_pointer_cast<UItem>(GetComponentL(itemname,true));
 UEPtr<UConnector> connector=dynamic_pointer_cast<UConnector>(GetComponentL(connectorname,true));
 if(!item)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+itemname);
  return false;
 }

 if(!connector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return false;
 }


 if(item->CheckLink(connector))
  return true;

 return false;
}
// ----------------------


// --------------------------
// Методы сериализации компонент
// --------------------------
// Возвращает свойства компонента по идентификатору
bool UNet::GetComponentProperties(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
  if(!cont || !serstorage)
   return false;

  const RDK::UContainer::VariableMapT &props=cont->GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
	try
	{
	 cont->GetProperty(I->first,serstorage);
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 cont->ProcessException(exception);
	}

	std::string paramname=I->first;//I->second.Property->GetName();
    if(serstorage->SelectNode(paramname))
    {
     serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
     serstorage->SelectUp();
	}
   }
   ++I;
  }

 return true;
}

// Возвращает выборочные свойства компонента по идентификатору
// Память для buffer должна быть выделена!
bool UNet::GetComponentSelectedProperties(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage)
{
  if(!cont || !serstorage)
   return false;

 return true;
}

// Возвращает свойства компонента по идентификатору с описаниями
// Память для buffer должна быть выделена!
bool UNet::GetComponentPropertiesEx(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
  if(!cont || !serstorage)
   return false;

  const RDK::UContainer::VariableMapT &props=cont->GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  UEPtr<UContainerDescription> descr=dynamic_pointer_cast<UContainerDescription>(Storage->GetClassDescription(Storage->FindClassName(cont->GetClass())));

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
	try
	{
	 cont->GetProperty(I->first,serstorage);
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 cont->ProcessException(exception);
	}

	std::string paramname=I->first;//I->second.Property->GetName();
	if(serstorage->SelectNode(paramname))
	{
	 serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
	 if(descr)
	 {
	  serstorage->SetNodeAttribute("Header",descr->GetDescription(paramname).Header);
	 }
	 serstorage->SelectUp();
	}
   }
   ++I;
  }

 return true;
}


// устанавливает свойства компонента по идентификатору
int UNet::SetComponentProperties(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage)
{
  if(!cont || !serstorage)
   return 1;

  std::string name;

  const RDK::UContainer::VariableMapT &props=cont->GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   try
   {
	cont->SetProperty(I->first,serstorage);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	cont->ProcessException(exception);
   }
   ++I;
  }
 return 0;
}


// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
bool UNet::SaveComponent(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask)
{
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",/*RDK::sntoa(cont->GetClass())*/Storage->FindClassName(cont->GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(ptParameter));
  if(!GetComponentProperties(cont, serstorage,params_type_mask))
   return false;
  serstorage->SelectUp();

  if(links)
  {
   serstorage->AddNode("Links");
   if(GetComponentInternalLinks(cont,serstorage,0))
    return false;
   serstorage->SelectUp();
  }

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!SaveComponent(dynamic_pointer_cast<RDK::UNet>(cont->GetComponentByIndex(i)),serstorage,false,params_type_mask))
   {
	std::string name;
	LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Sub component not found: ")+cont->GetFullName(name));
//	return false;
   }
  }
  serstorage->SelectUp();

  serstorage->SelectUp();

 return true;
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
bool UNet::LoadComponent(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage, bool links)
{
  if(!serstorage || !cont)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);

  if(cont->GetClass() != id)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Wrong class id: expected ")+sntoa(cont->GetClass())+std::string(" found ")+sntoa(id));
   return false;
  }

  cont->SetName(serstorage->GetNodeName());

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	try
	{
	 if(SetComponentProperties(cont, serstorage))
	 {
	  std::string name;
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("SetComponentProperties failed: ")+cont->GetFullName(name));
//	  return false;
	 }
	}
	catch(UException &exception)
	{
	 cont->ProcessException(exception);
    }
	serstorage->SelectUp();
   }
  }

  cont->DelAllComponents();

  if(!serstorage->SelectNode("Components"))
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Components section not found"));
   return false;
  }
  UStorage* storage=cont->GetStorage();
  for(int i=0;i<serstorage->GetNumNodes();i++)
  {
   serstorage->SelectNode(i);
   std::string nodename=serstorage->GetNodeName();
   name=serstorage->GetNodeAttribute("Class");
   try
   {
    id=Storage->FindClassId(name);
	UEPtr<UNet> newcont=dynamic_pointer_cast<UNet>(storage->TakeObject(id));
	if(!newcont)
	 continue;
	if(cont->AddComponent(static_pointer_cast<UContainer>(newcont)) == ForbiddenId)
	 continue;

	if(!LoadComponent(newcont,serstorage,false))
	{
	 std::string name;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("LoadComponent failed: ")+newcont->GetFullName(name));
//	 return false;
    }
   }
   catch(UException &exception)
   {
	cont->ProcessException(exception);
   }
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  if(links)
  {
   serstorage->SelectNode("Links");
   if(!SetComponentInternalLinks(cont,serstorage,0))
	return false;
   serstorage->SelectUp();
  }

 return true;
}


// Сохраняет все свойства компонента и его дочерних компонент в xml
bool UNet::SaveComponentProperties(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(cont->GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(type_mask));
  if(!GetComponentProperties(cont, serstorage,type_mask))
   return false;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   try
   {
	if(!SaveComponentProperties(dynamic_pointer_cast<RDK::UNet>(cont->GetComponentByIndex(i)),serstorage,type_mask))
	{
	 std::string name;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("SaveComponentProperties failed: ")+cont->GetFullName(name));
//	 return false;
	}
   }
   catch (UException &exception)
   {
	cont->ProcessException(exception);
   }
  }
  serstorage->SelectUp();

  serstorage->SelectUp();

 return true;
}

// Загружает все свойства компонента и его дочерних компонент из xml
bool UNet::LoadComponentProperties(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage)
{
  if(!cont || !serstorage)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);
  if(cont->GetClass() != id)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Wrong class id: expected ")+sntoa(cont->GetClass())+std::string(" found ")+sntoa(id));
   return false;
  }

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	try
	{
	 if(SetComponentProperties(cont, serstorage))
	 {
	  std::string name;
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("SetComponentProperties failed: ")+cont->GetFullName(name));
//	  return false;
	 }
	}
	catch (UException &exception)
	{
	 cont->ProcessException(exception);
    }
    serstorage->SelectUp();
   }
  }

  serstorage->SelectNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!serstorage->SelectNode(cont->GetComponentByIndex(i)->GetName()))
	continue;
   std::string nodename=serstorage->GetNodeName();
   try
   {
	if(!LoadComponentProperties(dynamic_pointer_cast<RDK::UNet>(cont->GetComponentByIndex(i)),serstorage))
	{
	 std::string name;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("LoadComponentProperties failed: ")+cont->GetFullName(name));
//	 return false;
	}
   }
   catch (UException &exception)
   {
	cont->ProcessException(exception);
   }
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

 return true;
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
void UNet::SetGlobalComponentPropertyValue(RDK::UContainer* cont, UId classid, const char *paramname, const char *buffer)
{
  if(!cont || classid == ForbiddenId)
   return;

  if(cont->GetClass() == classid)
  {
   try
   {
	cont->SetPropertyValue(paramname,buffer);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	cont->ProcessException(exception);
   }
  }

  for(int i=0;i<cont->GetNumComponents();i++)
  {
   SetGlobalComponentPropertyValue(cont->GetComponentByIndex(i), classid, paramname, buffer);
  }
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
void UNet::SetGlobalOwnerComponentPropertyValue(RDK::UContainer* cont, UId classid, UId owner_classid, const char *paramname, const char *buffer)
{
  if(!cont || classid == ForbiddenId)
   return;

  if(cont->GetClass() == classid && cont->GetOwner() && cont->GetOwner()->GetClass() == owner_classid)
  {
   try
   {
	cont->SetPropertyValue(paramname,buffer);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	cont->ProcessException(exception);
   }
  }

  for(int i=0;i<cont->GetNumComponents();i++)
  {
   SetGlobalOwnerComponentPropertyValue(cont->GetComponentByIndex(i), classid, owner_classid, paramname, buffer);
  }
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UNet::GetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
  if(owner_level)
   cont->GetLinks(linkslist, owner_level);
  else
   cont->GetLinks(linkslist, cont);


  *serstorage<<linkslist;
 return 0;
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются до уровня компонента owner_level
// Если owner_level не задан, то имена применяются до уровня текущего компонента
int UNet::SetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
  *serstorage>>linkslist;

  cont->BreakLinks();
  cont->CreateLinks(linkslist, owner_level);

 return true;
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UNet::GetComponentInputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel)
{
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
//  cont->GetInputLinks(linkslist, cont);

  *serstorage<<linkslist;
 return 0;
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UNet::GetComponentOutputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel)
{
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
//  cont->GetOutputLinks(linkslist, cont);

  *serstorage<<linkslist;
 return 0;
}

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UNet::GetComponentPersonalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
  if(owner_level)
   cont->GetLinks(linkslist, owner_level, true, cont);
  else
   cont->GetLinks(linkslist, cont->GetOwner(), true, cont);

  *serstorage<<linkslist;
 return 0;
}

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
bool UNet::SaveComponentDrawInfo(RDK::UNet* cont, RDK::USerStorageXML *serstorage)
{
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(cont->GetClass()));

  serstorage->AddNode("Links");

  UStringLinksList linkslist;
//  cont->GetLinks(linkslist, cont);

  for(int i=0;i<cont->GetNumComponents();i++)
  {
   UEPtr<UNet> sub_cont=static_pointer_cast<UNet>(cont->GetComponentByIndex(i));
   sub_cont->GetLinks(linkslist, cont,true,sub_cont);
  }
  *serstorage<<linkslist;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   UEPtr<UNet> sub_cont=static_pointer_cast<UNet>(cont->GetComponentByIndex(i));
   serstorage->AddNode(sub_cont->GetName());
   serstorage->SetNodeAttribute("Class",Storage->FindClassName(sub_cont->GetClass()));
   serstorage->AddNode("Parameters");
   try
   {
	if(!cont->GetComponentProperties(sub_cont,serstorage,ptPubParameter))
	 return false;
//	if(!cont->GetComponentProperties(sub_cont,serstorage,ptParameter|pgAny))
//	 return false;
   }
   catch (UException &exception)
   {
	cont->ProcessException(exception);
   }
   serstorage->SelectUp();
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  serstorage->SelectUp();

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы доступа к свойствам
// --------------------------
template<typename T>
ULinksListT<T>& UNet::GetLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
 if(dynamic_pointer_cast<UItem>(cont))
 {
  static_pointer_cast<UConnector>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);
  static_pointer_cast<UItem>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);
 }
 else
 if(dynamic_pointer_cast<UConnector>(cont))
  static_pointer_cast<UConnector>(cont)->GetLinks(linkslist,netlevel, exclude_internals,internal_level);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetLinks(cont->GetComponentByIndex(i), linkslist, netlevel, exclude_internals,internal_level);

 return linkslist;
}

template<typename T>
ULinksListT<T>& UNet::GetPersonalLinks(UEPtr<UContainer> cont, UEPtr<UContainer> cont2, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const
{
 if(dynamic_pointer_cast<UItem>(cont))
 {
  static_pointer_cast<UConnector>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);
  static_pointer_cast<UItem>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);
 }
 else
 if(dynamic_pointer_cast<UConnector>(cont))
  static_pointer_cast<UConnector>(cont)->GetPersonalLinks(cont2,linkslist,netlevel);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetPersonalLinks(cont->GetComponentByIndex(i), cont2, linkslist, netlevel);

 return linkslist;
}
// --------------------------

}
#endif

