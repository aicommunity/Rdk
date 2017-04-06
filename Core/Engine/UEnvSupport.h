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

/* *********************************************************************** */
/* *********************************************************************** */
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
UIdVector(UId id1);
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
/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
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
/* *********************************************************************** */
/* *********************************************************************** */

//extern ULongId ULongIdemp;


/* *********************************************************************** */
/* *********************************************************************** */
// Описание одной из сторон связи между объектами
template<typename T>
struct ULinkSideT
{
// Id объекта
T Id;

// Индекс связываемого входа/выхода
int Index;

// Имя связываемого входа/выхода
std::string Name;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinkSideT(void);
ULinkSideT(const T &id, int index);
ULinkSideT(const T &id, const std::string &property_name);
ULinkSideT(const T &id, const std::string &property_name, int index);
ULinkSideT(const ULinkSideT &link);
virtual ~ULinkSideT(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
bool operator < (const ULinkSideT<T> &linkside2) const;
bool operator == (const ULinkSideT<T> &linkside2) const;
bool operator != (const ULinkSideT<T> &linkside2) const;
//friend bool operator < (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2);
//friend bool operator == (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2);
//friend bool operator != (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2);
// --------------------------
};

typedef ULinkSideT<ULongId> ULinkSide;
typedef ULinkSideT<std::string> UStringLinkSide;


/* *********************************************************************** */
/* *********************************************************************** */

// Описание одиночной связи
template<typename T>
struct ULinkT
{
 ULinkSideT<T> Item;
 std::vector<ULinkSideT<T> > Connector;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinkT(void);
ULinkT(const ULinkSideT<T> &item, const ULinkSideT<T> &conn);
ULinkT(const T &item_id, int item_index,const T &conn_id, int conn_index);
ULinkT(const ULinkT &link);
virtual ~ULinkT(void);
// --------------------------


// --------------------------
// Методы управления данными
// --------------------------
// Ищет заданный элемент с приемником connector и возвращает индекс или отрицательное число, если не
// найдено
int FindConnector(const ULinkSideT<T> &connector);
// --------------------------


// --------------------------
// Операторы
// --------------------------
//friend bool operator < (const ULinkT<T> &link1, const ULinkT<T> &link2);
//friend bool operator == (const ULinkT<T> &link1, const ULinkT<T> &link2);
//friend bool operator != (const ULinkT<T> &link1, const ULinkT<T> &link2);
// --------------------------
};

typedef ULinkT<ULongId> ULink;
typedef ULinkT<std::string> UStringLink;


/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */

// Массив связей
template<typename T>
class ULinksListT
{
protected: // Данные
// Данные
mutable std::vector<ULinkT<T> > Data;

// Размер массива
int Size;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULinksListT(void);
ULinksListT(const ULinksListT<T> &copy);
virtual ~ULinksListT(void);
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
int Add(const ULinkT<T> &link);

// Объединяет элемент уже с существующим
int Merge(const ULinkT<T> &link);

// Заменяет элемент (если элемент не существует - он создается)
int Set(const ULinkT<T> &link);

// Удаляет произвольный элемент по индексу
void Del(int index);

// Ищет заданный элемент и возвращает индекс или отрицательное число, если не
// найдено
int Find(const ULinkT<T> &link);

// Ищет заданный элемент с источником item и возвращает индекс или отрицательное число, если не
// найдено
int FindItem(const ULinkSideT<T> &item);

// Ищет заданный элемент с приемником connector и возвращает индекс или отрицательное число, если не
// найдено
int FindConnector(const ULinkSideT<T> &connector);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает размер массива
int GetSize(void) const;
int size(void) const;

// Возвращает указатель на начало данных
ULinkT<T>* GetData(void);
// --------------------------


// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
ULinksListT<T>& operator = (const ULinksListT<T> &copy);

// Оператор доступа
ULinkT<T>& operator [] (int index);
const ULinkT<T>& operator [] (int index) const;
// --------------------------
};


template<typename CharT>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const UIdVector &data)
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

typedef ULinksListT<ULongId> ULinksList;
typedef ULinksListT<std::string> UStringLinksList;
/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<typename T>
ULinkSideT<T>::ULinkSideT(void)
 : Index(0)
{
}

template<typename T>
ULinkSideT<T>::ULinkSideT(const T &id, int index)
 : Id(id), Index(index)
{
}

template<typename T>
ULinkSideT<T>::ULinkSideT(const T &id, const std::string &property_name)
 :Id(id), Index(-1), Name(property_name)
{
}

template<typename T>
ULinkSideT<T>::ULinkSideT(const T &id, const std::string &property_name, int index)
 :Id(id), Index(index), Name(property_name)
{
}


template<typename T>
ULinkSideT<T>::ULinkSideT(const ULinkSideT<T> &link)
{
 Id=link.Id;
 Index=link.Index;
 Name=link.Name;
}

template<typename T>
ULinkSideT<T>::~ULinkSideT(void)
{
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
/*template<typename T>
bool operator < (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2)
{
 return (linkside1.Id<linkside2.Id)
	|| (linkside1.Id == linkside2.Id && linkside1.Index < linkside2.Index);
}

template<typename T>
bool operator == (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2)
{
 return (linkside1.Id == linkside2.Id) & (linkside1.Index == linkside2.Index);
}

template<typename T>
bool operator != (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2)
{
 return (!(linkside1 == linkside2));
} */

template<typename T>
bool ULinkSideT<T>::operator < (const ULinkSideT<T> &linkside2) const
{
 return (Id<linkside2.Id)
	|| (Id == linkside2.Id && Name < linkside2.Name) ||
	   (Id == linkside2.Id && Name == linkside2.Name && Index<linkside2.Index);
}

template<typename T>
bool ULinkSideT<T>::operator == (const ULinkSideT<T> &linkside2) const
{
 return (Id == linkside2.Id) & (Name == linkside2.Name) & (Index == linkside2.Index);
}

template<typename T>
bool ULinkSideT<T>::operator != (const ULinkSideT<T> &linkside2) const
{
 return (!(*this == linkside2));
}
// --------------------------
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<typename T>
ULinkT<T>::ULinkT(void)
{
}

template<typename T>
ULinkT<T>::ULinkT(const ULinkSideT<T> &item, const ULinkSideT<T> &conn)
{
 Item=item;
 Connector.push_back(conn);
}

template<typename T>
ULinkT<T>::ULinkT(const T &item_id, int item_index,const T &conn_id, int conn_index)
{
 Item.Id=item_id;
 Item.Index=item_index;
 Connector.resize(1);
 Connector[0].Id=conn_id;
 Connector[0].Index=conn_index;
}

template<typename T>
ULinkT<T>::ULinkT(const ULinkT<T> &link)
{
 Item=link.Item;
 Connector=link.Connector;
}

template<typename T>
ULinkT<T>::~ULinkT(void)
{
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Ищет заданный элемент с приемником connector и возвращает индекс или отрицательное число, если не
// найдено
template<typename T>
int ULinkT<T>::FindConnector(const ULinkSideT<T> &connector)
{
  for(size_t j=0;j<Connector.size();j++)
   if(Connector[j] == connector)
	return j;

 return -1;
}
// --------------------------



// --------------------------
// Операторы
// --------------------------
template<typename T>
bool operator < (const ULinkT<T> &link1, const ULinkT<T> &link2)
{
 return (link1.Item<link2.Item) & (link1.Connector<link2.Connector);
}

template<typename T>
bool operator == (const ULinkT<T> &link1, const ULinkT<T> &link2)
{
 return (link1.Item == link2.Item) & (link1.Connector == link2.Connector);
}

template<typename T>
bool operator != (const ULinkT<T> &link1, const ULinkT<T> &link2)
{
 return !(link1 == link2);
}
// --------------------------


/******************************************************************************/
// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<typename T>
ULinksListT<T>::ULinksListT(void)
{
 Size=0;
}

template<typename T>
ULinksListT<T>::ULinksListT(const ULinksListT<T> &copy)
{
 Size=0;

 *this=copy;
}

template<typename T>
ULinksListT<T>::~ULinksListT(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Очищает массив
template<typename T>
void ULinksListT<T>::Clear(void)
{
 Data.clear();
 Size=0;
}

// Изменяет размер массива с сохранением прежних данных
template<typename T>
void ULinksListT<T>::Resize(int newsize)
{
 Data.resize(newsize);
 Size=Data.size();
}

// Добавляет элемент в конец массива
// Возвращает индекс элемента
template<typename T>
int ULinksListT<T>::Add(const ULinkT<T> &link)
{
 Data.push_back(link);
 ++Size;
 return Size-1;
}

// Объединяет элемент уже с существующим
template<typename T>
int ULinksListT<T>::Merge(const ULinkT<T> &link)
{
 int id=FindItem(link.Item);

 if(id < 0)
  return Add(link);

 for(size_t j=0;j<link.Connector.size();j++)
 {
  typename std::vector<ULinkSideT<T> >::iterator I=find(Data[id].Connector.begin(),Data[id].Connector.end(),link.Connector[j]);
  if(I == Data[id].Connector.end())
  {
   Data[id].Connector.push_back(link.Connector[j]);
  }
 }
 return id;
}

// Заменяет элемент (если элемент не существует - он создается)
template<typename T>
int ULinksListT<T>::Set(const ULinkT<T> &link)
{
 int id=FindItem(link.Item);

 if(id < 0)
  return Add(link);

 Data[id]=link;
 return id;
}


// Удаляет произвольный элемент по индексу
template<typename T>
void ULinksListT<T>::Del(int index)
{
 if(index<0 || index>=Size)
  return;

 Data.erase(Data.begin()+index);
 --Size;
}

// Ищет заданный элемент и возвращает индекс или отрицательное число, если не
// найдено
template<typename T>
int ULinksListT<T>::Find(const ULinkT<T> &link)
{
 if(!Size)
  return -1;
 ULinkT<T> *pdata=&Data[0];
 for(int i=0;i<Size;i++,pdata++)
  if(*pdata == link)
   return i;

 return -1;
}

// Ищет заданный элемент с источником item и возвращает индекс или отрицательное число, если не
// найдено
template<typename T>
int ULinksListT<T>::FindItem(const ULinkSideT<T> &item)
{
 if(!Size)
  return -1;

 ULinkT<T> *pdata=&Data[0];
 for(int i=0;i<Size;i++,pdata++)
  if(pdata->Item == item)
   return i;

 return -1;
}

// Ищет заданный элемент с приемником connector и возвращает индекс или отрицательное число, если не
// найдено
template<typename T>
int ULinksListT<T>::FindConnector(const ULinkSideT<T> &connector)
{
 if(!Size)
  return -1;

 ULinkT<T> *pdata=&Data[0];
 for(int i=0;i<Size;i++,pdata++)
 {
  for(size_t j=0;j<pdata->Connector.size();j++)
   if(pdata->Connector[j] == connector)
	return i;
 }

 return -1;
}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает размер массива
template<typename T>
int ULinksListT<T>::GetSize(void) const
{
 return Size;
}

template<typename T>
int ULinksListT<T>::size(void) const
{
 return Size;
}


// Возвращает указатель на начало данных
template<typename T>
ULinkT<T>* ULinksListT<T>::GetData(void)
{
 if(!Size)
  return 0;

 return &Data[0];
}
// --------------------------


// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
template<typename T>
ULinksListT<T>& ULinksListT<T>::operator = (const ULinksListT<T> &copy)
{
 Data=copy.Data;
 Size=copy.Size;

 return *this;
}

// Оператор доступа
template<typename T>
ULinkT<T>& ULinksListT<T>::operator [] (int index)
{
 return Data[index];
}

template<typename T>
const ULinkT<T>& ULinksListT<T>::operator [] (int index) const
{
 return Data[index];
}
// --------------------------
/* *********************************************************************** */
/* *********************************************************************** */

}

#endif
