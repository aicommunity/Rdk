/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAEnvSupportH
#define UAEnvSupportH

namespace RDK {

typedef int UId;
extern UId ForbiddenId;

typedef std::string NameT;
extern NameT ForbiddenName;

/* *********************************************************************** */
/* *********************************************************************** */
// Массив id компонент
class UIdVector
{
protected: // Параметры
// Размер массива
int Size;

protected: // Данные
// Массив id
mutable UId* Buffer;

// Реальный размер массива
int RealSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UIdVector(void);
UIdVector(const UIdVector &copy);
UIdVector(UId id1);
UIdVector(UId id1, UId id2, ...);
virtual ~UIdVector(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Возвращает размер массива
int GetSize(void) const;

// Изменяет размер массива с сохранением прежних данных
void Resize(int newsize);

// Очищает массив
void Clear(void);

// Возвращает указатель на начало данных
UId* GetBuffer(void);

// Добавляет элемент в конец массива
// Возвращает индекс элемента
int Add(UId id);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UIdVector& operator = (const UIdVector &copy);

// Оператор доступа
UId& operator [] (int index);
const UId& operator [] (int index) const;

// Операторы сравнения
bool operator == (const UIdVector &copy) const;
bool operator != (const UIdVector &copy) const;
bool operator < (const UIdVector &copy) const;
// --------------------------
};

typedef UIdVector ULongId;
/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
// Массив id компонент
class ULongIdVector
{
protected: // Параметры
// Размер массива
int Size;

protected: // Данные
// Массив указателей
mutable ULongId* Buffer;

// Реальный размер массива
int RealSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULongIdVector(void);
ULongIdVector(const ULongIdVector &copy);
virtual ~ULongIdVector(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Возвращает размер массива
int GetSize(void) const;

// Изменяет размер массива с сохранением прежних данных
void Resize(int newsize);

// Очищает массив
void Clear(void);

// Возвращает указатель на начало данных
ULongId* GetBuffer(void);

// Добавляет элемент в конец массива
// Возвращает индекс элемента
int Add(const ULongId &id);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
ULongIdVector& operator = (const ULongIdVector &copy);

// Оператор доступа
ULongId& operator [] (int index);
const ULongId& operator [] (int index) const;
// --------------------------
};
/* *********************************************************************** */
/* *********************************************************************** */

extern ULongId ULongIdemp;


/* *********************************************************************** */
/* *********************************************************************** */
// Описание одной из сторон связи между объектами
struct ULinkSide
{
// Id объекта
ULongId Id;

// Индекс связываемого входа/выхода
int Index;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinkSide(void);
ULinkSide(const ULongId &id, int index);
ULinkSide(const ULinkSide &link);
virtual ~ULinkSide(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
friend bool operator < (const ULinkSide &linkside1, const ULinkSide &linkside2);
friend bool operator == (const ULinkSide &linkside1, const ULinkSide &linkside2);
friend bool operator != (const ULinkSide &linkside1, const ULinkSide &linkside2);
// --------------------------
};

// Описание одиночной связи
struct ULink
{
 ULinkSide Item;
 ULinkSide Connector;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULink(void);
ULink(const ULinkSide &item, const ULinkSide &conn);
ULink(const ULongId &item_id, int item_index,const ULongId &conn_id, int conn_index);
ULink(const ULink &link);
virtual ~ULink(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
friend bool operator < (const ULink &link1, const ULink &link2);
friend bool operator == (const ULink &link1, const ULink &link2);
friend bool operator != (const ULink &link1, const ULink &link2);
// --------------------------
};

// Массив связей
class ULinksList
{
protected: // Данные
// Данные
mutable ULink *Data;

// Размер массива
int Size;

// Реальный размер массива
int RealSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinksList(void);
ULinksList(const ULinksList &copy);
virtual ~ULinksList(void);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Очищает массив
void Clear(void);

// Изменяет размер массива с сохранением прежних данных
void Resize(int newsize);

// Добавляет элемент в конец массива
// Возвращает индекс элемента
int Add(const ULink &link);

// Удаляет произвольный элемент по индексу
void Del(int index);

// Ищет заданный элемент и возвращает индекс или отрицательное число, если не
// найдено
int Find(const ULink &link);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает размер массива
int GetSize(void) const;

// Возвращает указатель на начало данных
ULink* GetData(void);
// --------------------------


// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
ULinksList& operator = (const ULinksList &copy);

// Оператор доступа
ULink& operator [] (int index);
const ULink& operator [] (int index) const;
// --------------------------
};
/* *********************************************************************** */
/* *********************************************************************** */
}

#endif
