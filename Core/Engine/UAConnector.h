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

#include "UAContainer.h"

namespace RDK {

class UAItem;

// Описание подключаемого элемента "UConnectedITEM"
struct UCItem
{
// Подключаемый элемент
UAItem* Item;

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
UCItem Find(UEPtr<UAItem> item, int index=0) const;

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

class UAConnector: public UAContainer
{
friend class UAItem;
private: // Хранилище связей
// Список указателей на подключенные элементы сети
UCItemList CItemList;

protected: // Общедоступные свойства
// Число доступных входов
int NumInputs;

// Признак включения/выключения режима автоматического увеличения числа входов
// при подключении нового item.
bool AutoNumInputs;

protected: // Переменные быстрого доступа к даннным входов. Read only!

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAConnector(void);
virtual ~UAConnector(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
public:
// Возвращает число подключенных элементов item
int GetNumInputs(void) const;

// Устанавливает число подключенных элементов item
bool SetNumInputs(int value);

// Признак включения/выключения режима автоматического увеличения числа входов
// при подключении нового item.
bool GetAutoNumInputs(void) const;
bool SetAutoNumInputs(bool value);
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
UCLink GetCLink(UEPtr<UAItem> item) const;
// --------------------------

// ----------------------
// Коммуникационные методы
// ----------------------
protected:
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Возвращает false если na уже подключен к этому входу.
// При успешном подключении c_index содержит реальный индекс подключенного входа
virtual bool ConnectToItem(UEPtr<UAItem> na, int i_index, int &c_index);

// Разрывает связь с элементом сети 'na'
virtual void DisconnectFromItem(UEPtr<UAItem> na);

// Разрывает связь с элементом сети подключенным ко входу 'index'
virtual void DisconnectFromIndex(int c_index);

// Выполняет действия после физически установленой связи
virtual bool AConnectToItem(UEPtr<UAItem> na, int i_index, int c_index);

// Выполняет действия после физически разорваной связи
virtual void ADisconnectFromItem(UEPtr<UAItem> na, int i_index, int c_index);

public:
// Разрывает все текущие связи
virtual void DisconnectAllItems(void);

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual void DisconnectByObject(UEPtr<UAContainer> brklevel);

public:
// Возвращает интерфейс входа
//virtual UEInterface* GetInputInterface(int c_index);

// Возвращает интерфейс входа
//NameT GetInputInterfaceTypeName(int c_index);

// Проверяет, допустимо ли подключение заданного item к этому коннектору
virtual bool CheckItem(UEPtr<UAItem> item, int item_index, int conn_index);

// Возвращает список подключений
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel) const;
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

}
#endif
