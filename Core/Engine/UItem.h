/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAItemH
#define UAItemH

#include "UConnector.h"
//#include "UEInterface.h"

namespace RDK {

typedef UEPtr<UConnector> PUAConnector;

class UAConnectorVector
{
protected: // Параметры
// Размер контейнера
int Size;

protected: // Данные
// Массив элементов
PUAConnector* Data;

protected: // Временные переменные
// Реальный размер контейнера
int RealSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAConnectorVector(void);
UAConnectorVector(const UAConnectorVector &copy);
virtual ~UAConnectorVector(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает контейнер
void Clear(void);

// Меняет размер контейнера с сохранением элементов
void Resize(int newsize);

// Ищет в контейнере первый заданный элемент начиная с индекса index
// и возвращает его индекс или -1 если элемент не найден
int Find(const PUAConnector &item, int index=0) const;

// Добавляет в конец контейнера элемент
void Add(const PUAConnector &item);

// Добавляет в конец контейнера элемент только если таких элементов еще нет
// Возвращает true в случае успешного добавления
bool AddUnique(const PUAConnector &item);

// Удаляет элемент по индексу
void Del(int index);

// Удаляет элемент по указателю
void Del(PUAConnector *item);
// --------------------------

// --------------------------
// Методы доступа
// --------------------------
// Оператор присваивания
UAConnectorVector& operator = (const UAConnectorVector &copy);

// Оператор доступа к элементу
PUAConnector& operator [] (int index);
const PUAConnector& operator [] (int index) const;

// Оператор доступа к массиву
PUAConnector* GetData(void);

// Оператор доступа к размеру
int GetSize(void) const;
// --------------------------
};

class UAConnector2DVector
{
protected: // Параметры
// Размер контейнера
int Size;

protected: // Данные
// Массив элементов
UAConnectorVector* Data;

protected: // Временные переменные
// Реальный размер контейнера
int RealSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAConnector2DVector(void);
UAConnector2DVector(const UAConnector2DVector &copy);
virtual ~UAConnector2DVector(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает контейнер
void Clear(void);

// Меняет размер контейнера с сохранением элементов
void Resize(int newsize);

// Ищет в контейнере первый заданный элемент начиная с индекса index
// и возвращает его индекс или -1 если элемент не найден
//int Find(const UAConnectorVector &item, int index=0) const;

// Добавляет в конец контейнера элемент
void Add(const UAConnectorVector &item);

// Добавляет в конец контейнера элемент только если таких элементов еще нет
// Возвращает true в случае успешного добавления
//bool AddUnique(const UAConnectorVector &item);

// Удаляет элемент по индексу
void Del(int index);

// Удаляет элемент по указателю
//void Del(UAConnectorVector *item);
// --------------------------

// --------------------------
// Методы доступа
// --------------------------
// Оператор присваивания
UAConnector2DVector& operator = (const UAConnector2DVector &copy);

// Оператор доступа к элементу
UAConnectorVector& operator [] (int index);
const UAConnectorVector& operator [] (int index) const;

// Оператор доступа к массиву
UAConnectorVector* GetData(void);

// Оператор доступа к размеру
int GetSize(void) const;
// --------------------------
};

class UItem: public UConnector
{
protected: // Основные свойства
// Список коннекторов подключенных к выходам этого объекта
//UAConnector2DVector AssociatedConnectors;
std::map<std::string, std::vector<PUAConnector> > RelatedConnectors;

protected: // Общедоступные свойства
// Число выходов
int NumOutputs;

// Признак включения/выключения режима автоматического увеличения числа выходов
// при подключении нового connector.
bool AutoNumOutputs;

protected: // Временные переменные. Read Only!
// Указатель на первый элемент списка подключенных коннекторов
//vector<UConnector**> PAssociatedConnectors;

// Число элементов списка подключенных коннекторов
//vector<size_t> NumAConnectors;

protected: // Переменные быстрого доступа к даннным выходов. Read only!
// Число выходов
//size_t CNumOutputs;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UItem(void);
virtual ~UItem(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
public:
// Возвращает число подключенных элементов item
const int& GetNumOutputs(void) const;

// Устанавливает число подключенных элементов item
bool SetNumOutputs(const int &value);

// Признак включения/выключения режима автоматического увеличения числа выходов
const bool& GetAutoNumOutputs(void) const;
bool SetAutoNumOutputs(const bool &value);

/// Возвращает число выходов к которым кто-то подключен
int GetNumActiveOutputs(void) const;

/// Возвращает число коннекторов к которым подключено заданное свойство
int GetNumActiveOutputs(const NameT &item_property_name) const;
// --------------------------

// --------------------------
// Методы доступа к описанию входов и выходов
// --------------------------
/// Ищет свойство-выход по заданному индексу
void FindOutputProperty(int index, UIProperty* &property) const;

/// Ищет свойство-вход по заданному индексу
//void FindInputProperty(int index, UIProperty* &property);
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
virtual void Free(void);
// --------------------------


public:
// ----------------------
// Коммуникационные методы
// ----------------------
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Возвращает false если na уже подключен к этому входу.
// При успешном подключении c_index содержит реальный индекс подключенного входа
virtual bool ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index);
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

// Устанавливает связь с коннектором 'c'
virtual bool Connect(UEPtr<UConnector> c, int i_index, int c_index=-1);
virtual bool Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

// Разрывает связь выхода этого объекта с коннектором 'c'
virtual void Disconnect(UEPtr<UConnector> c);

// Возвращает текущее число соединений для заданного выхода.
int GetNumAConnectors(int index) const;
int GetNumAConnectors(const NameT &item_property_name) const;

// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
virtual bool Disconnect(const UId &id);

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
void DisconnectAll(void);

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual void DisconnectBy(UEPtr<UContainer> brklevel);

// Переустанавливает все связи этого item со всеми connectors которые получают
// данные от этого item
virtual void BuildLinks(void);

// Возвращает указатель на коннектор из списка подключений
// по Id 'id'.
UEPtr<UConnector> GetAConnector(const UId &id, int index) const;

// Возвращает  коннектор из списка подключений.
UEPtr<UConnector> GetAConnectorByIndex(int output, int index) const;
UEPtr<UConnector> GetAConnectorByIndex(const NameT &item_property_name, int index) const;

// Проверяет, существует ли связь с заданным коннектором
bool CheckLink(const UEPtr<UConnector> &connector) const;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool CheckLink(const UEPtr<UConnector> &connector, int item_index) const;
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool CheckLink(const UEPtr<UConnector> &connector, int item_index, int conn_index) const;
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name) const;

// Возвращает список подключений
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const;

// Возвращает список подключений непосредственно коннектора cont
template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const;

// Возвращает список подключений этого компонента и всех дочерних компонент
// к заданному компоненту comp и всем его дочерним компонентам
template<typename T>
ULinksListT<T>& GetFullItemLinks(ULinksListT<T> &linkslist, UEPtr<UItem> comp,
                            UEPtr<UContainer> netlevel) const;
// ----------------------

public:
// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool Default(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool Build(void);

// Активирует все коннекторы, подключенные к выходу этого объекта.
//virtual void ActivateAll(void);
// --------------------------

};

}
#endif

