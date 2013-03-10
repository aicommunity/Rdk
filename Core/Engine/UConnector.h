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

class UIPropertyIO;

enum { ipData=1, ipComp=2 };
enum { ipSingle=16, ipRange=32, ipList=64 };
enum { ipDataSingle=ipData|ipSingle, ipDataRange=ipData|ipRange,
	   ipDataList=ipData|ipList, ipCompSingle=ipComp|ipSingle,
	   ipCompRange=ipComp|ipRange, ipCompList=ipComp|ipList };

class UItem;

// Описание подключаемого элемента "UConnectedITEM"
struct UCItem
{
// Подключаемый элемент
UItem* Item;

// Индекс выхода
int Index;

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
struct UCLink
{
// Индекс выхода
int Output;

// Индекс входа
int Input;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UCLink(void);
UCLink(const UCLink &copy);
// --------------------------
};

// Контейнер - список указателей на подключенные элементы
class UCItemList
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
UCItem Find(const UEPtr<UItem> item, int index=0) const;
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

class UConnector: public UContainer
{
friend class UItem;
private: // Хранилище связей
// Список указателей на подключенные элементы сети
UCItemList CItemList;

protected: // Общедоступные свойства
// Число доступных входов
int NumInputs;

// Признак включения/выключения режима автоматического увеличения числа входов
// при подключении нового item.
bool AutoNumInputs;

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
// Возвращает число подключенных элементов item
const int& GetNumInputs(void) const;

// Устанавливает число подключенных элементов item
bool SetNumInputs(const int &value);

// Признак включения/выключения режима автоматического увеличения числа входов
// при подключении нового item.
const bool& GetAutoNumInputs(void) const;
bool SetAutoNumInputs(const bool &value);
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
// Возвращает подключенный к этому коннектору объект по индексу
const UCItem& GetCItem(int c_index) const;

// Возвращает информацию об индексах связей с этим item или -1, -1
// если такая связь отсутствует
UCLink GetCLink(const UEPtr<UItem> item) const;
UCLink GetCLink(const UItem* const item) const;
// --------------------------

// ----------------------
// Коммуникационные методы
// ----------------------
protected:
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Возвращает false если na уже подключен к этому входу.
// При успешном подключении c_index содержит реальный индекс подключенного входа
virtual bool ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index);

// Разрывает связь с элементом сети 'na'
virtual void DisconnectFromItem(UEPtr<UItem> na);

// Разрывает связь с элементом сети подключенным ко входу 'index'
virtual void DisconnectFromIndex(int c_index);

// Выполняет действия после физически установленой связи
virtual bool AConnectToItem(UEPtr<UItem> na, int i_index, int c_index);

// Выполняет действия после физически разорваной связи
virtual void ADisconnectFromItem(UEPtr<UItem> na, int i_index, int c_index);

public:
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
virtual bool CheckItem(UEPtr<UItem> item, int item_index, int conn_index);

// Проверяет, существует ли связь с заданным коннектором
bool CheckLink(const UEPtr<UItem> &item) const;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool CheckLink(const UEPtr<UItem> &item, int item_index) const;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool CheckLink(const UEPtr<UItem> &item, int item_index, int conn_index) const;

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
EInputIndexNotExist(void) : EInvalidIndex(-1) {};
};
};


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
};

class UIPropertyInput
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

class UIPropertyOutput
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
