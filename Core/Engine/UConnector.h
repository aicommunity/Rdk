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

namespace RDK {

//class UIPropertyIO;



class UItem;

// Описание подключаемого элемента "UConnectedITEM"
struct RDK_LIB_TYPE UCItem
{
// Подключаемый элемент
UItem* Item;

// Индекс выхода
int Index;

// Имя выхода
std::string Name;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCItem(void);
UCItem(const UCItem &copy);
// --------------------------

// --------------------------
// Операторы
// --------------------------
bool operator == (const UCItem &value);
bool operator != (const UCItem &value);
// --------------------------
};

// Описание входящей связи с заданным item "UConnectedLINK"
struct RDK_LIB_TYPE UCLink
{
// Индекс выхода
int Output;

// Индекс входа
int Input;

// Имя выхода
std::string OutputName;

// Имя входа
std::string InputName;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCLink(void);
UCLink(const UCLink &copy);
// --------------------------
};

// Контейнер - список указателей на подключенные элементы
class RDK_LIB_TYPE UCItemList
{
protected: // Параметры
// Размер контейнера
int Size;

protected: // Данные
// Массив элементов
UCItem* Data;

protected: // Временные переменные
// Реальный размер контейнера
int RealSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCItemList(void);
UCItemList(const UCItemList &copy);
virtual ~UCItemList(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает контейнер
void Clear(void);

// Меняет размер контейнера с сохранением элементов
void Resize(int newsize);

// Ищет в контейнере первый заданный элемент начиная с индекса index
// и возвращает его описание
UCItem Find(const UEPtr<UItem> &item, int index=0) const;
UCItem Find(const UItem *const item, int index=0) const;

// Ищет в контейнере первый заданный элемент начиная с индекса index
// и возвращает его индекс или -1 если элемент не найден
int Find(const UCItem &item, int index=0) const;

// Добавляет в конец контейнера элемент
void Add(const UCItem &item);

// Добавляет в конец контейнера элемент только если таких элементов еще нет
// Возвращает true в случае успешного добавления
bool AddUnique(const UCItem &item);

// Удаляет элемент по индексу
void Del(int index);

// Удаляет элемент по указателю
void Del(UCItem *item);
// --------------------------

// --------------------------
// Методы доступа
// --------------------------
// Оператор присваивания
UCItemList& operator = (const UCItemList &copy);

// Оператор доступа к элементу
UCItem& operator [] (int index);
const UCItem& operator [] (int index) const;
//const UCItem& operator () (int index) const;

// Оператор доступа к массиву
UCItem* GetData(void);

// Оператор доступа к размеру
int GetSize(void) const;
// --------------------------
};

class RDK_LIB_TYPE UConnector: public UContainer
{
friend class UItem;
protected: // Хранилище связей
// Список указателей на подключенные элементы сети в формате
// <имя свойства к которому подключены, список подключенных компонент>
std::map<std::string, std::vector<UCItem> > ConnectedItemList;

// Заглушка, возвращаемая в случае, когда корректную ссылку на CItem вернуть невозможно
UCItem DummyItem;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UConnector(void);
virtual ~UConnector(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
public:
/// Возвращает число свойств в которым что-то подключено
virtual int GetNumActiveInputs(void) const;

/// Возвращает число реально подключенных элементов к заданному свойству
virtual int GetNumActiveInputs(const NameT &connector_property_name) const;
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
virtual void Free(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Ищет первый свободный вход
virtual NameT FindFreeInputName(void)=0;

// Возвращает подключенный к этому коннектору объект по индексу
const UCItem& GetCItem(const NameT &connector_property_name, const UEPtr<UItem> &item, int &index) const;
const UCItem& GetCItem(const NameT &connector_property_name, int index) const;
void GetCItem(const NameT &connector_property_name, std::vector<UCItem> &buffer) const;

// Возвращает информацию об индексах связей с этим item или -1, -1
// если такая связь отсутствует
virtual UCLink GetCLink(const UEPtr<UItem> &item) const;
virtual UCLink GetCLink(const UItem* const item) const;
// --------------------------

// --------------------------
// Методы доступа к описанию входов и выходов
// --------------------------
/// Ищет свойство-вход по заданному индексу
virtual void FindInputProperty(const NameT &connector_property_name, UIProperty* &property) const;

/// Возвращает индекс входа с заданным именем
virtual int FindInputIndex(const NameT &input_name) const;
// --------------------------

// ----------------------
// Коммуникационные методы
// ----------------------
protected:
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Возвращает false если na уже подключен к этому входу.
// При успешном подключении c_index содержит реальный индекс подключенного входа
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

// Разрывает все связи с элементом сети 'na'
virtual void DisconnectFromItem(UEPtr<UItem> na);

/// Разрывает связь с элементом сети 'na', подключенную от i_index
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name);

/// Разрывает связь с элементом сети 'na', подключенную от i_index к c_index
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index);

// Выполняет действия после физически установленой связи
virtual bool AConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name);

// Выполняет действия после физически разорваной связи
virtual void ADisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name);

public:
// Разрывает связь с элементом сети подключенным ко входу 'index'
virtual void DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index);

virtual void DisconnectFromIndex(const NameT &connector_property_name);

// Разрывает все текущие связи
virtual void DisconnectAllItems(void);

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual void DisconnectByObject(UEPtr<UContainer> brklevel);

public:
// Возвращает интерфейс входа
//virtual UEInterface* GetInputInterface(int c_index);

// Возвращает интерфейс входа
//NameT GetInputInterfaceTypeName(int c_index);

// Проверяет, допустимо ли подключение заданного item к этому коннектору
bool CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name);

// Проверяет, существует ли связь с заданным коннектором
bool CheckLink(const UEPtr<UItem> &item) const;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name) const;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name, const NameT &connector_property_name) const;

// Возвращает список подключений
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const;

// Возвращает список подключений непосредственно коннектора cont
template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const;
// ----------------------

// --------------------------
// Методы управления счетом
// --------------------------
public:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool Default(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool Build(void);
// --------------------------

public: // Классы описания исключений
// Некорректный индекс входа
struct EInvalidInputIndex: public EInvalidIndex
{
EInvalidInputIndex(int index) : EInvalidIndex(index) {};
};

// Такой вход не существует
struct EInputIndexNotExist: public EInvalidIndex
{
EInputIndexNotExist(int index) : EInvalidIndex(index) {};
};
};

	   /*
class UIPropertyIO
{
public:
// --------------------------
// Методы управления данными
// --------------------------
/// Возвращает тип свойства ввода-вывода
virtual int GetIoType(void) const=0;

/// Возвращает диапазон индексов входа/выхода
virtual bool CheckRange(int index)=0;
// --------------------------

// --------------------------
// Методы управления указателем
// --------------------------
/// Возвращает указатель на данные
virtual void const* GetPointer(int index) const=0;

/// Устанавливает указатель на данные
virtual bool SetPointer(int index, void* value)=0;

/// Первичная инициализация указателя
virtual void Init(void)=0;
// --------------------------
};     */

class RDK_LIB_TYPE UIPropertyInput
{
protected: // Данные
/// Указатель на компонент-источник данных
UEPtr<UItem> Item;

/// Имя выхода компнента-источника данных
std::string ItemOutputName;

public:
/// Возвращает указатель на компонент-источник
UEPtr<UItem> GetItem(void);

/// Возвращает имя подключенного выхода
const std::string& GetItemOutputName(void);
};

class RDK_LIB_TYPE UIPropertyOutput
{
protected: // Данные
/// Указатели на компоненты-приемники данных
std::vector<UEPtr<UItem> > Connectors;

/// Имя выхода компнента-источника данных
std::vector<std::string> ConnectorInputNames;

public:
/// Возвращает число подключенных входов
size_t GetNumConnectors(void);

/// Возвращает указатель на компонент-приемник
UEPtr<UConnector> GetConnector(int index);

/// Возвращает имя подключенного входа компонента-приемника
const std::string& GetConnectorInputName(int index);
};

}

#include "UPropertyIO.h"
#endif
