/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UANetH
#define UANetH

#include "UADItem.h"
#include "UStorage.h"


namespace RDK {

class RDK_LIB_TYPE UNet: public UADItem
{
protected: // Основные свойства

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UNet(void);
virtual ~UNet(void);
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const;

// Возращает все связи между двумя компонентами в виде xml в буфер buffer
// включая связи этого компонента
// если 'sublevel' == -1, то возвращает также все связи между объектом и любым дочерним компонентом
// второго объекта. Работает симметрично в обе стороны.
// если 'sublevel' == 0, то возвращает связи только между этими объектами
template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<RDK::UNet> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, int sublevel=-1);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
virtual void Free(void);
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
protected:
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------

// ----------------------
// Методы управления связями
// ----------------------
public:
// Устанавливает новую связь 'link'
template<typename T>
bool CreateLink(const ULinkT<T> &link, bool forced_connect_same_item=false);

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
template<typename T>
bool CreateLink(const ULinkSideT<T> &itemid, const ULinkSideT<T> &connectorid, bool forced_connect_same_item=false);
virtual bool CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
virtual bool CreateLink(const NameT &item, int item_index,
						const NameT &connector, int connector_index=-1, bool forced_connect_same_item=false);
virtual bool CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index, int connector_c_index=-1, bool forced_connect_same_item=false);
//virtual bool CreateLink(const NameT &item, const NameT &item_property_name,
//						const NameT &connector, const NameT &connector_property_name);

// Устанавливает все связи из массива 'linkslist'
template<typename T>
bool CreateLinks(const ULinksListT<T> &linkslist, UEPtr<UNet> owner_level=0);

// Разрывает все связи со входом
//template<typename T>
//bool BreakLink(const ULinkSideT<T> &id);

// Разрывает связь 'link'
template<typename T>
bool BreakLink(const ULinkT<T> &link);

// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
template<typename T>
bool BreakLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector);
virtual bool BreakLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);

// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
virtual bool BreakLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index);
virtual bool BreakLink(const NameT &item, const NameT &item_property_name,
						const NameT &connector, const NameT &connector_property_name, int connector_c_index=-1);

// Разрывает все связи между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
virtual bool BreakLink(const NameT &itemname, const NameT &connectorname);

// Разрывает все связи между выходом элемента сети и любыми коннекторами
virtual bool BreakAllOutgoingLinks(const NameT &itemname);
virtual bool BreakAllOutgoingLinks(const NameT &itemname, const NameT &item_property_name);

// Разрывает все связи сети
// исключая ее внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual void BreakLinks(UEPtr<UContainer> brklevel);

// Разрывает заданные связи сети
virtual bool BreakLinks(const ULinksList &linkslist);

// Разрывает все внутренние связи сети.
virtual void BreakLinks(void);

// Разрывает связь ко входу connector_index коннектора 'connectorid'
virtual void BreakConnectorLink(const NameT &connectorname, int connector_index);
virtual void BreakConnectorLink(const NameT &connectorname, const NameT &connector_index, int connector_c_index=-1);

// Проверяет, существует ли заданная связь
template<typename T>
bool CheckLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector);

bool CheckLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);
bool CheckLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index);
bool CheckLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index=-1);
bool CheckLink(const NameT &itemname,
						const NameT &connectorname, int connector_c_index=-1);

/// Переключает исходящие связи от 'output_name1' выхода компонента 'item1' на выход 'output_name2' компонента 'item2'
bool SwitchOutputLinks(const UStringLinkSide &item1, const UStringLinkSide &item2);
bool SwitchOutputLinks(const NameT &itemname1, const NameT &output_name1,
						const NameT &itemname2, const NameT &output_name2);
// ----------------------

// --------------------------
// Методы сериализации компонент
// --------------------------
// Возвращает свойства компонента по идентификатору
virtual bool GetComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask);

// Возвращает выборочные свойства компонента по идентификатору
// Память для buffer должна быть выделена!
virtual bool GetComponentSelectedProperties(RDK::USerStorageXML *serstorage);

// Возвращает свойства компонента по идентификатору с описаниями
// Память для buffer должна быть выделена!
virtual bool GetComponentPropertiesEx(RDK::USerStorageXML *serstorage, unsigned int type_mask);

// устанавливает свойства компонента по идентификатору
virtual int SetComponentProperties(RDK::USerStorageXML *serstorage);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
virtual bool SaveComponent(RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
virtual bool LoadComponent(RDK::USerStorageXML *serstorage, bool links);

// Сохраняет все свойства компонента и его дочерних компонент в xml
virtual bool SaveComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask);

// Загружает все свойства компонента и его дочерних компонент из xml
virtual bool LoadComponentProperties(RDK::USerStorageXML *serstorage);

/// Сохраняет полную структуру компонента
virtual bool SaveComponentStructure(RDK::USerStorageXML *serstorage, bool links, unsigned int type_mask);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
virtual void SetGlobalComponentPropertyValue(UId classid, const char *paramname, const char *buffer);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
virtual void SetGlobalOwnerComponentPropertyValue(UId classid, UId owner_classid, const char *paramname, const char *buffer);

// Возращает все связи внутри компонента в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int GetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются до уровня компонента owner_level
// Если owner_level не задан, то имена применяются до уровня текущего компонента
virtual int SetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int GetComponentInputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int GetComponentOutputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int GetComponentPersonalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
virtual bool SaveComponentDrawInfo(RDK::USerStorageXML *serstorage);
// --------------------------

// ----------------------
// Методы управления компонентами верхнего уровня
// ----------------------
/// Проверяет существование компонента с заданным именем
template<typename T>
UEPtr<T> FindComponentByNameAndType(const NameT &component_name);

/// Проверяет существование компонента с заданным именем и создает его
/// при необходимости.
/// Возвращает указатель на созданный экземпляр, если он был добавлен
/// или 0
template<typename T>
UEPtr<T> AddMissingComponent(const NameT &component_name, const NameT &class_name);
// ----------------------

// --------------------------
// Скрытые методы доступа к свойствам
// --------------------------
protected:
template<typename T>
ULinksListT<T>& GetLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level=0) const;

template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<UContainer> cont, UEPtr<UContainer> cont2, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const;
// --------------------------
};

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

 return linkslist;
}

// Устанавливает новую связь 'link'
template<typename T>
bool UNet::CreateLink(const ULinkT<T> &link, bool forced_connect_same_item)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
 {
  res &=CreateLink(link.Item, link.Connector[i], forced_connect_same_item);
 }

 return res;
}

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
template<typename T>
bool UNet::CreateLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector, bool forced_connect_same_item)
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

// bool res=(item.Name == "DcOrientation");
// bool res2=(connector.Name == "InputReliability");
 if(!item.Name.empty() || !connector.Name.empty())
 {
  int c_index=connector.Index;
  if(!(pitem->Connect(pconnector,item.Name,connector.Name,c_index, forced_connect_same_item)))
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

// Разрывает связь 'link'
template<typename T>
bool UNet::BreakLink(const ULinkT<T> &link)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
  res&=BreakLink(link.Item,link.Connector[i]);

 return res;
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

 if(!item.Name.empty() || !connector.Name.empty())
 {
  pitem->Disconnect(pconnector, item.Name, connector.Name, connector.Index);
 }
 else
 {
  pitem->Disconnect(pconnector, item.Index, connector.Index);
 }

 return true;
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
  if(!item.Name.empty() || !connector.Name.empty())
  {
   if(pitem->CheckLink(pconnector,item.Name, connector.Name, connector.Index))
	return true;
  }
  else
  {
   if(pitem->CheckLink(pconnector,item.Index, connector.Index))
	return true;
  }

 return false;
}
// --------------------------

// ----------------------
// Методы управления компонентами верхнего уровня
// ----------------------
/// Проверяет существование компонента с заданным именем
template<typename T>
UEPtr<T> UNet::FindComponentByNameAndType(const NameT &component_name)
{
 UEPtr<T> comp=dynamic_pointer_cast<T>(GetComponent(component_name,true));
 if(comp)
  return comp;

 return 0;
}


/// Проверяет существование компонента с заданным именем и создает его
/// при необходимости.
/// Возвращает указатель на созданный экземпляр, если он был добавлен
/// или 0
template<typename T>
UEPtr<T> UNet::AddMissingComponent(const NameT &component_name, const NameT &class_name)
{
 UEPtr<T> comp=dynamic_pointer_cast<T>(GetComponent(component_name,true));
 if(comp)
  return comp;

 if(!Storage)
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - Storage not found"));
  return comp;
 }

 UEPtr<UComponent> proto=Storage->TakeObject(class_name);
 if(!proto)
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - Component not found in storage. ClassName=")+class_name);
  return comp;
 }
 comp=dynamic_pointer_cast<T>(proto);
 comp->SetName(component_name);
 comp->SetTimeStep(TimeStep);
 if(!AddComponent(comp))
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - AddComponent failed. ClassName=")+class_name);
  Storage->ReturnObject(comp);
  comp=0;
  return comp;
 }
 return comp;
}
// ----------------------


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
