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

class RDK_LIB_TYPE UAConnectorVector
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

class RDK_LIB_TYPE UAConnector2DVector
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

// Удаляет элемент по индексу
void Del(int index);
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

class RDK_LIB_TYPE UItem: public UConnector
{
protected: // Основные свойства
// Список коннекторов подключенных к выходам этого объекта
//UAConnector2DVector AssociatedConnectors;
std::map<std::string, std::vector<PUAConnector> > RelatedConnectors;

protected: // Временные переменные. Read Only!

protected: // Переменные быстрого доступа к даннным выходов. Read only!

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
/// Возвращает число выходов к которым кто-то подключен
virtual int GetNumActiveOutputs(void) const;

/// Возвращает число коннекторов к которым подключено заданное свойство
virtual int GetNumActiveOutputs(const NameT &item_property_name) const;
// --------------------------

// --------------------------
// Методы доступа к описанию входов и выходов
// --------------------------
/// Ищет свойство-выход по заданному индексу
virtual void FindOutputProperty(const NameT &item_property_name, UIProperty* &property) const;
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
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

// Устанавливает связь с коннектором 'c'
virtual bool Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

/// Разрывает все связи выхода этого объекта с коннектором 'c'.
virtual void Disconnect(UEPtr<UConnector> c);

// Разрывает связь выхода этого объекта с коннектором 'c' по индексу
virtual void Disconnect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index);

// Возвращает текущее число соединений для заданного выхода.
virtual int GetNumAConnectors(const NameT &item_property_name) const;

// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
virtual bool Disconnect(const UId &id);

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
virtual void DisconnectAll(void);
virtual void DisconnectAll(const NameT &item_property_name);

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual void DisconnectBy(UEPtr<UContainer> brklevel);

// Переустанавливает все связи этого item со всеми connectors которые получают
// данные от этого item
virtual void BuildLinks(void);

// Возвращает указатель на коннектор из списка подключений
// по Id 'id'.
virtual UEPtr<UConnector> GetAConnector(const UId &id, int index) const;

// Возвращает  коннектор из списка подключений.
virtual UEPtr<UConnector> GetAConnectorByIndex(const NameT &item_property_name, int index) const;

// Проверяет, существует ли связь с заданным коннектором
bool CheckLink(const UEPtr<UConnector> &connector, int connected_c_index) const;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index) const;
bool CheckLink(const UEPtr<UConnector> &connector, int item_index, int conn_index) const;


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
// --------------------------

};




// Возвращает список подключений
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
template<typename T>
ULinksListT<T>& UItem::GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 GetLongId(netlevel,item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
 {
  link.Connector.clear();
  for(size_t i=0;i<I->second.size();i++)
  {
   if(exclude_internals)
   {
	if(I->second[i]->CheckOwner(internal_level))
	 continue;
   }
   I->second[i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=I->second[i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
	link.Item.Index=indexes.Output;
	link.Item.Name=indexes.OutputName;
	connector.Index=indexes.Input;
	connector.Name=indexes.InputName;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }
 }

 return linkslist;

}

// Возвращает список подключений непосредственно коннектора cont
template<typename T>
ULinksListT<T>& UItem::GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 GetLongId(netlevel,item.Id);
 if(item.Id.size() == 0)
  return linkslist;
 link.Item=item;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
 {
  link.Connector.clear();
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i] != cont)
	continue;
   I->second[i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.size() != 0)
   {
	UCLink indexes=I->second[i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
	link.Item.Index=indexes.Output;
	link.Item.Name=indexes.OutputName;
	connector.Index=indexes.Input;
	connector.Name=indexes.InputName;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }
 }

 return linkslist;
}

// Возвращает список подключений этого компонента и всех дочерних компонент
// к заданному компоненту comp и всем его дочерним компонентам
template<typename T>
ULinksListT<T>& UItem::GetFullItemLinks(ULinksListT<T> &linkslist, UEPtr<UItem> comp,
                                     UEPtr<UContainer> netlevel) const
{
 ULinkT<T> link;
 ULinkSideT<T> item;
 ULinkSideT<T> connector;

 if(!comp)
  return linkslist;

 GetLongId(netlevel,item.Id);
 if(link.Item.Id.GetSize() == 0)
  return linkslist;
 link.Item=item;


 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i]->CheckOwner(static_pointer_cast<UContainer>(comp)) && I->second[i] != comp)
	continue;
   I->second[i]->GetLongId(netlevel,connector.Id);
   if(connector.Id.GetSize() != 0)
   {
	UCLink indexes=I->second[i]->GetCLink(UEPtr<UItem>(const_cast<UItem*>(this)));
	link.Item.Index=indexes.Output;
	link.Item.Name=indexes.OutputName;
	connector.Index=indexes.Input;
	connector.Name=indexes.InputName;

	link.Connector.push_back(connector);
	linkslist.Set(link);
   }
  }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UItem> item=dynamic_cast<UItem*>(PComponents[i].operator->());
  item->GetFullItemLinks(linkslist, comp, netlevel);
 }

 return linkslist;
}

// Template methods UConnector
// Возвращает список подключений
template<typename T>
ULinksListT<T>& UConnector::GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level) const
{
 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;


 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item)
   {
	if(exclude_internals)
	{
	 if(I->second[i].Item->CheckOwner(internal_level))
	  continue;
	}
   I->second[i].Item->GetLongId(netlevel,item.Id);
   UIProperty* property=0;
   FindInputProperty(I->first, property);
   if(property)
	connector.Index=-1;//property->GetMinRange();
   else
    connector.Index=-1;//i;
   connector.Name=I->first;

   item.Index=-1;//CItemList[i].Index;
   item.Name=I->second[i].Name;//CItemList[i].Name;
   if(connector.Id.size() != 0)
   {
	int item_id=linkslist.FindItem(item);
	if(item_id >= 0)
	{
	 if(linkslist[item_id].FindConnector(connector) >= 0)
	  continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;
}

// Возвращает список подключений непосредственно коннектора cont
template<typename T>
ULinksListT<T>& UConnector::GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const
{
 ULinkT<T> link;
 ULinkSideT<T> connector;
 ULinkSideT<T> item;
 GetLongId(netlevel,connector.Id);
 if(connector.Id.size()==0)
  return linkslist;

 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item == cont)
   {
	I->second[i].Item->GetLongId(netlevel,item.Id);
	UIProperty* property=0;
	FindInputProperty(I->first, property);
	if(property)
	 connector.Index=-1;//property->GetMinRange();
	else
	 connector.Index=-1;//i; // TODO тут неопределенность
	connector.Name=I->first;
	item.Index=-1;//CItemList[i].Index;
	item.Name=I->second[i].Name;
	if(connector.Id.size() != 0)
	{
	 int item_id=linkslist.FindItem(item);
	 if(item_id >= 0)
	 {
	  if(linkslist[item_id].FindConnector(connector) >= 0)
	   continue;
	 linkslist[item_id].Connector.push_back(connector);
	}
	else
	{
	 link.Item=item;
	 link.Connector.clear();
	 link.Connector.push_back(connector);
	 linkslist.Add(link);
	}
   }
  }
 }

 return linkslist;
}



}
#endif

