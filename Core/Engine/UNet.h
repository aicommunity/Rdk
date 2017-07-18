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

#include "UContainer.h"
#include "UStorage.h"
#include "ULocalProperty.h"


namespace RDK {

class UNet;
typedef UEPtr<UNet> PUAConnector;


class RDK_LIB_TYPE UIPropertyInputBase: virtual public UIPropertyInput
{
protected: // Данные
/// Данные подключенных ко входу источников
//std::vector<UCItem> ItemsList;

/// указатели на свойства-приемники данных
std::list<UEPtr<UIPropertyOutput> > ConnectedProperties;

/// Тип входа
int InputType;

public:
/// Конструкторы и деструкторы
UIPropertyInputBase(void);
virtual ~UIPropertyInputBase(void);

/// Возвращает тип свойства ввода-вывода
virtual int GetInputType(void) const;

public: // Методы доступа к источнику данных
/// Возвращает число подключений ко входу
virtual int GetNumConnections(void) const;

/// Возвращает указатели на свойства-источники данных
virtual const std::list<UEPtr<UIPropertyOutput> > GetConnectedProperties(void) const;

/// Возвращает полное имя подключенного компонента
virtual std::string GetItemFullName(int c_index=-1) const;

/// Возвращает имя подключенного выхода
virtual std::string GetItemOutputName(int c_index=-1) const;

/// Возвращает true если вход имеет подключение
virtual bool IsConnected(void) const;

// Проверяет, существует ли связь с заданным коннектором
virtual bool IsConnectedTo(const UIPropertyOutput *output_property) const;

/// Разрывает связь со свойством output_property
virtual bool Disconnect(UIPropertyOutput *output_property);

/// Разрывает все связи со свойством
virtual bool DisconnectAll(void);

public:
/// Финальные действия по связыванию входа со свойством output_property
virtual bool ConnectToOutput(UIPropertyOutput *output_property);

/// Финальные действия по уничтожению связи со свойством output_property
virtual bool DisconnectFromOutput(UIPropertyOutput *output_property);

public: // Методы управления указателем на входные данные
/// Возвращает указатель на данные
//virtual void const* GetPointer(int index) const=0;

/// Устанавливает указатель на данные
//virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// Сбрасывает указатель на данные
//virtual bool ResetPointer(int index, void* value)=0;
};

class RDK_LIB_TYPE UIPropertyOutputBase: virtual public UIPropertyOutput
{
protected: // Данные
/// указатели на свойства-приемники данных
std::list<UEPtr<UIPropertyInput> > ConnectedProperties;

public: // Конструкторы и деструкторы
UIPropertyOutputBase(void);
virtual ~UIPropertyOutputBase(void);

public: // Методы доступа к подключенным входам
/// Возвращает число подключенных входов
virtual size_t GetNumConnectors(void) const;

/// Возвращает указатели на свойства-приемники данных
virtual const std::list<UEPtr<UIPropertyInput> > GetConnectedProperties(void) const;

/// Устанавливает связь этого выхода со входом input_property
virtual bool Connect(UIPropertyInput *input_property);

/// Разрывает связь этого выхода со входом input_property
virtual bool Disconnect(UIPropertyInput *input_property);

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
virtual bool DisconnectAll(void);

/// Возвращает true если выход подключен к выбранному входу
virtual bool IsConnectedTo(UIPropertyInput *input_property);

/// Возвращает true если выход подключен к одному из входов выбранного компонента
virtual bool IsConnectedTo(UNet *component);

public: // Методы управления указателем на входные данные
/// Возвращает указатель на данные
//virtual void const* GetPointer(int index) const=0;
};



class RDK_LIB_TYPE UNet: public UContainer
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

// --------------------------
// Системные методы управления объектом
// --------------------------
public:
// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
virtual void Free(void);
// --------------------------

// ----------------------
// Методы управления связями
// ----------------------
public:
// Устанавливает новую связь 'link'
bool CreateLink(const ULink &link, bool forced_connect_same_item=false);

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
bool CreateLink(const ULinkSide &itemid, const ULinkSide &connectorid, bool forced_connect_same_item=false);

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
virtual bool CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index, int connector_c_index=-1, bool forced_connect_same_item=false);

// Устанавливает все связи из массива 'linkslist'
bool CreateLinks(const ULinksList &linkslist, UEPtr<UNet> owner_level=0);

// Разрывает связь 'link'
bool BreakLink(const ULink &link);

// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
bool BreakLink(const ULinkSide &item, const ULinkSide &connector);

// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
virtual bool BreakLink(const NameT &item, const NameT &item_property_name,
						const NameT &connector, const NameT &connector_property_name, int connector_c_index=-1);

// Разрывает все связи между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
virtual bool BreakLink(const NameT &itemname, const NameT &connectorname);

// Разрывает все связи между выходом элемента сети и любыми коннекторами
virtual bool BreakOutputLinks(void);
virtual bool BreakOutputLinks(const NameT &itemname);
virtual bool BreakOutputLinks(const NameT &itemname, const NameT &item_property_name);

// Разрывает связь ко входу connector_index коннектора 'connectorid'
virtual bool BreakInputLinks(void);
virtual bool BreakInputLinks(const NameT &connectorname, const NameT &connector_index, int connector_c_index=-1);

// Разрывает все связи сети
// исключая ее внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual bool BreakLinks(UEPtr<UContainer> brklevel);

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual bool DisconnectBy(UEPtr<UContainer> brklevel);

// Разрывает заданные связи сети
virtual bool BreakLinks(const ULinksList &linkslist);

// Разрывает все внутренние связи сети.
virtual void BreakLinks(void);


// Проверяет, существует ли заданная связь
bool IsLinkExists(const ULinkSide &item, const ULinkSide &connector);

bool IsLinkExists(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index=-1);
bool IsLinkExists(const NameT &itemname,
						const NameT &connectorname);
// ----------------------


// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возращает все связи компонента в виде xml в буфер buffer
ULinksList& GetLinks(ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals=false, UEPtr<UNet> internal_level=0) const;

// Возращает все связи компонента в виде xml в буфер buffer
// включая связи этого компонента
// если 'sublevel' == -1, то возвращает также все связи между объектом и любым дочерним компонентом
// второго объекта. Работает симметрично в обе стороны.
// если 'sublevel' == 0, то возвращает связи только между этими объектами
ULinksList& GetPersonalLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, int sublevel=-1);
// --------------------------

public:
/// Ищет свойство-выход по заданному индексу
virtual UIPropertyOutput* FindOutputProperty(const NameT &property_name);

/// Ищет свойство-вход по заданному индексу
virtual UIPropertyInput* FindInputProperty(const NameT &property_name);

/// Возвращает число входов
virtual int GetNumInputs(void);

/// Возвращает число входов
virtual int GetNumOutputs(void);

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
ULinksList& GetLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals, UEPtr<UNet> internal_level=0) const;

ULinksList& GetPersonalLinks(UEPtr<UNet> cont, UEPtr<UNet> cont2, ULinksList &linkslist, UEPtr<UNet> netlevel) const;

// Возращает все связи компонента в виде xml в буфер buffer
virtual ULinksList& GetOutputLinks(ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals=false, UEPtr<UNet> internal_level=0) const;
virtual ULinksList& GetInputLinks(ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals=false, UEPtr<UNet> internal_level=0) const;

// Возращает все связи компонента в виде xml в буфер buffer
// включая связи этого компонента
// если 'sublevel' == -1, то возвращает также все связи между объектом и любым дочерним компонентом
// второго объекта. Работает симметрично в обе стороны.
// если 'sublevel' == 0, то возвращает связи только между этими объектами
virtual ULinksList& GetOutputPersonalLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, int sublevel=-1);
virtual ULinksList& GetInputPersonalLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, int sublevel=-1);
// --------------------------
};

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



}

#endif
