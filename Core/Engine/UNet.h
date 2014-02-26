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

namespace RDK {

class UNet: public UADItem
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
bool CreateLink(const ULinkT<T> &link);

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
template<typename T>
bool CreateLink(const ULinkSideT<T> &itemid, const ULinkSideT<T> &connectorid);
virtual bool CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
virtual bool CreateLink(const NameT &item, int item_index,
						const NameT &connector, int connector_index=-1);

// Устанавливает все связи из массива 'linkslist'
template<typename T>
bool CreateLinks(const ULinksListT<T> &linkslist, UEPtr<UNet> owner_level=0);

// Разрывает все связи с выходом элемента сети, если
// 'id' - есть Id элемента сети.
// Иначе, если 'id' - Id коннектора, то метод разрывает
// связи этого коннектора
template<typename T>
bool BreakLink(const ULinkSideT<T> &id);

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

// Проверяет, существует ли заданная связь
template<typename T>
bool CheckLink(const ULinkSideT<T> &item, const ULinkSideT<T> &connector);

virtual bool CheckLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);
virtual bool CheckLink(const NameT &itemname, int item_index,
						const NameT &connectorname, int connector_index);
virtual bool CheckLink(const NameT &itemname,
						const NameT &connectorname);
// ----------------------

// --------------------------
// Методы сериализации компонент
// --------------------------
// Возвращает свойства компонента по идентификатору
virtual bool GetComponentProperties(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// Возвращает выборочные свойства компонента по идентификатору
// Память для buffer должна быть выделена!
virtual bool GetComponentSelectedProperties(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage);

// Возвращает свойства компонента по идентификатору с описаниями
// Память для buffer должна быть выделена!
virtual bool GetComponentPropertiesEx(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// устанавливает свойства компонента по идентификатору
virtual int SetComponentProperties(UEPtr<RDK::UContainer> cont, RDK::USerStorageXML *serstorage);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
virtual bool SaveComponent(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
virtual bool LoadComponent(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage, bool links);

// Сохраняет все свойства компонента и его дочерних компонент в xml
virtual bool SaveComponentProperties(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// Загружает все свойства компонента и его дочерних компонент из xml
virtual bool LoadComponentProperties(UEPtr<RDK::UNet> cont, RDK::USerStorageXML *serstorage);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
virtual void SetGlobalComponentPropertyValue(RDK::UContainer* cont, UId classid, const char *paramname, const char *buffer);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
virtual void SetGlobalOwnerComponentPropertyValue(RDK::UContainer* cont, UId classid, UId owner_classid, const char *paramname, const char *buffer);

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int GetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются до уровня компонента owner_level
// Если owner_level не задан, то имена применяются до уровня текущего компонента
virtual int SetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int GetComponentInputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int GetComponentOutputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int GetComponentPersonalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
virtual bool SaveComponentDrawInfo(RDK::UNet* cont, RDK::USerStorageXML *serstorage);
// --------------------------

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

}

#endif
