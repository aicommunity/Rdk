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

class UANet: public UADItem
{
protected: // Основные свойства

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UANet(void);
virtual ~UANet(void);
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------
ULinksList& GetLinks(ULinksList &linkslist, const UAContainer *netlevel) const;
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual bool Copy(UAContainer *target, UAContainerStorage *stor=0, bool copystate=false) const;

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
virtual bool CheckComponentType(const UAContainer* comp) const;
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
protected:
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
virtual bool AAddComponent(UAContainer* comp, UIPointer* pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual bool ADelComponent(UAContainer* comp);
// --------------------------

// ----------------------
// Методы управления связями
// ----------------------
public:
// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
virtual bool CreateLink(const ULinkSide &itemid, const ULinkSide &connectorid);
virtual bool CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
virtual bool CreateLink(const NameT &item, int item_index,
						const NameT &connector, int connector_index=-1);

// Устанавливает все связи из массива 'linkslist'
virtual bool CreateLinks(const ULinksList &linkslist);

// Разрывает все связи с выходом элемента сети, если
// 'id' - есть Id элемента сети.
// Иначе, если 'id' - Id коннектора, то метод разрывает
// связи этого коннектора
virtual bool BreakLink(const ULinkSide &id);

// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
virtual bool BreakLink(const ULinkSide &item, const ULinkSide &connector);
virtual bool BreakLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);

// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
virtual bool BreakLink(const NameT &itemname, int item_index,
                        const NameT &connectorname, int connector_index);

// Разрывает все связи сети
// исключая ее внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual void BreakLinks(const UAContainer* brklevel);

// Разрывает заданные связи сети
virtual bool BreakLinks(const ULinksList &linkslist);

// Разрывает все внутренние связи сети.
virtual void BreakLinks(void);
// ----------------------

// --------------------------
// Скрытые методы доступа к свойствам
// --------------------------
protected:
ULinksList& GetLinks(const UAContainer *cont, ULinksList &linkslist, const UAContainer *netlevel) const;
// --------------------------


};

}

#endif
