/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UEnvSupportH
#define UEnvSupportH

#include <string>
#include <vector>
#include "../Utilities/UException.h"

namespace RDK {

typedef int UId;
extern const UId ForbiddenId;

typedef std::string NameT;
extern const NameT ForbiddenName;
/*
// Массив id компонент
class RDK_LIB_TYPE UIdVector
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
explicit UIdVector(UId id1);
UIdVector(UId id1, UId id2, ...);
virtual ~UIdVector(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Возвращает размер массива
int GetSize(void) const;
int size(void) const;

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
// Методы ввода вывода идентификаторов
// --------------------------
// !!! Исключения
UIdVector& DecodeFromString(const std::string &str);
std::string& EncodeToString(std::string &str) const;
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UIdVector& operator = (const UIdVector &copy);
UIdVector& operator = (const UId &copy);

// Оператор доступа
UId& operator [] (int index);
const UId& operator [] (int index) const;

// Операторы сравнения
bool operator == (const UIdVector &copy) const;
bool operator != (const UIdVector &copy) const;
bool operator < (const UIdVector &copy) const;
// --------------------------

// --------------------------
// Исключения
// --------------------------
// Интерфейс описания исключение
class EILongId;

// Ошибка декодирования строкового id
class EDecodeFail;
// --------------------------
};

typedef UIdVector ULongId;


// --------------------------
// Исключения
// --------------------------
// Интерфейс описания исключение
class RDK_LIB_TYPE UIdVector::EILongId
{
};

// Ошибка декодирования строкового id
class RDK_LIB_TYPE UIdVector::EDecodeFail: public EError, public UIdVector::EILongId
{
public:
// Строка с ошибкой
std::string StringId;

// Позиция ошибки в строке
int Position;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//EDecodeFail(void);
EDecodeFail(const std::string &stringid, int position);
virtual ~EDecodeFail(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};
// --------------------------

// Массив id компонент
class RDK_LIB_TYPE ULongIdVector
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
*/

// Описание одной из сторон связи между объектами
struct ULinkSide
{
// Id объекта
std::string ComponentName;

// Имя связываемого входа/выхода
std::string PropertyName;

// Индекс связываемого входа/выхода (опционально)
int Index;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinkSide(void);
ULinkSide(const std::string &comp_name, const std::string &property_name);
ULinkSide(const std::string &comp_name, const std::string &property_name, int index);
ULinkSide(const ULinkSide &link);
virtual ~ULinkSide(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
bool operator < (const ULinkSide &linkside2) const;
bool operator == (const ULinkSide &linkside2) const;
bool operator != (const ULinkSide &linkside2) const;
// --------------------------
};

/* *********************************************************************** */
/* *********************************************************************** */

// Описание одиночной связи
struct ULink
{
 ULinkSide Item;
 std::vector<ULinkSide> Connector;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULink(void);
ULink(const ULinkSide &item, const ULinkSide &conn);
//ULink(const std::string &item_id, int item_index,const T &conn_id, int conn_index);
ULink(const ULink &link);
virtual ~ULink(void);
// --------------------------


// --------------------------
// Методы управления данными
// --------------------------
// Ищет заданный элемент с приемником connector и возвращает индекс или отрицательное число, если не
// найдено
int FindConnector(const ULinkSide &connector);
// --------------------------
};

/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */

// Массив связей
class ULinksList
{
protected: // Данные
// Данные
mutable std::vector<ULink> Data;

// Размер массива
int Size;

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

// Объединяет элемент уже с существующим
int Merge(const ULink &link);

// Заменяет элемент (если элемент не существует - он создается)
int Set(const ULink &link);

// Удаляет произвольный элемент по индексу
void Del(int index);

// Ищет заданный элемент и возвращает индекс или отрицательное число, если не
// найдено
int Find(const ULink &link);

// Ищет заданный элемент с источником item и возвращает индекс или отрицательное число, если не
// найдено
int FindItem(const ULinkSide &item);

// Ищет заданный элемент с приемником connector и возвращает индекс или отрицательное число, если не
// найдено
int FindConnector(const ULinkSide &connector);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает размер массива
int GetSize(void) const;
int size(void) const;

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

/*
template<typename CharT>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const UIdVector &data)
 : Item(item)
{
 size_t size=data.GetSize();

 if(size > 0)
 {
  for(size_t i=0;i<size;i++)
  {
   stream<<data[i];
   if(i != size-1)
	stream<<".";
  }
 }

 return stream;
}
  */
/* *********************************************************************** */
/* *********************************************************************** */

}

#endif
