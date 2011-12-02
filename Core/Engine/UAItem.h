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

#include "UAConnector.h"
//#include "UEInterface.h"

namespace RDK {

typedef UEPtr<UAConnector> PUAConnector;

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

class UAItem: public UAConnector
{
protected: // Основные свойства
// Список коннекторов подключенных к выходам этого объекта
UAConnector2DVector AssociatedConnectors;

protected: // Общедоступные свойства
// Число выходов
int NumOutputs;

protected: // Временные переменные. Read Only!
// Указатель на первый элемент списка подключенных коннекторов
//vector<UAConnector**> PAssociatedConnectors;

// Число элементов списка подключенных коннекторов
//vector<size_t> NumAConnectors;

protected: // Переменные быстрого доступа к даннным выходов. Read only!
// Число выходов
//size_t CNumOutputs;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAItem(void);
virtual ~UAItem(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
public:
// Возвращает число подключенных элементов item
int GetNumOutputs(void) const;

// Устанавливает число подключенных элементов item
bool SetNumOutputs(int value);
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
// Устанавливает связь с коннектором 'c'
virtual bool Connect(UEPtr<UAConnector> c, int i_index, int c_index=-1);

// Разрывает связь выхода этого объекта с коннектором 'c'
virtual void Disconnect(UEPtr<UAConnector> c);

// Возвращает текущее число соединений для заданного выхода.
int GetNumAConnectors(int index) const;

// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
virtual bool Disconnect(const UId &id);

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
void DisconnectAll(void);

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual void DisconnectBy(UEPtr<UAContainer> brklevel);

// Переустанавливает все связи этого item со всеми connectors которые получают
// данные от этого item
virtual void BuildLinks(void);

// Возвращает указатель на коннектор из списка подключений
// по Id 'id'.
UEPtr<UAConnector> GetAConnector(const UId &id, int index) const;

// Возвращает  коннектор из списка подключений.
UEPtr<UAConnector> GetAConnectorByIndex(int output, int index) const;

// Возвращает список подключений
ULinksList& GetLinks(ULinksList &linkslist, UEPtr<UAContainer> netlevel) const;

// Возвращает список подключений этого компонента и всех дочерних компонент
// к заданному компоненту comp и всем его дочерним компонентам
ULinksList& GetFullItemLinks(ULinksList &linkslist, UEPtr<UAItem> comp,
                            UEPtr<UAContainer> netlevel) const;
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

