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
class UIPropertyInput;

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
	  /*
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
};       */

class RDK_LIB_TYPE UConnector: public UContainer
{
friend class UItem;
protected: // Хранилище связей
// Список указателей на подключенные элементы сети в формате
// <имя свойства к которому подключены, список подключенных компонент>
//std::map<std::string, std::vector<UCItem> > ConnectedItemList;

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
// Системные методы управления объектом
// --------------------------
// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
virtual void Free(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Возвращает список свойств-входов
virtual void ReadInputPropertiesList(std::vector<UEPtr<UIPropertyInput> > &buffer) const;

/// Возвращает число входов
virtual int GetNumInputs(void) const;

/// Возвращает число свойств в которым что-то подключено
//virtual int GetNumActiveInputs(void) const;

/// Возвращает число реально подключенных элементов к заданному свойству
virtual int GetNumActiveInputs(const NameT &connector_property_name) const;

/// Ищет первый свободный вход
//virtual NameT FindFreeInputName(void)=0;

/// Возвращает массив данных о подключенных к этому коннектору компонент по имени входа
//const UCItem& GetCItem(const NameT &connector_property_name, const UEPtr<UItem> &item, int &index) const;
//const UCItem& GetCItem(const NameT &connector_property_name, int index) const;
void GetCItem(const NameT &connector_property_name, std::vector<UCItem> &buffer) const;

// Возвращает информацию о данных связей с item или пустой массив
// если такая связь отсутствует
virtual void GetCLink(const UEPtr<UItem> &item, std::vector<UCLink> &buffer) const;
virtual void GetCLink(const UItem* const item, std::vector<UCLink> &buffer) const;
// --------------------------

// --------------------------
// Методы доступа к описанию входов и выходов
// --------------------------
/// Ищет свойство-вход по заданному индексу
virtual void FindInputProperty(const NameT &connector_property_name, UIPropertyInput* &property) const;
// --------------------------

// ----------------------
// Коммуникационные методы
// ----------------------
// Разрывает связь с элементом сети подключенным ко входу 'item_property_name'
//virtual void DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index);

//virtual void DisconnectFromIndex(const NameT &connector_property_name);

// Разрывает все текущие связи
virtual void DisconnectAllItems(void);

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
virtual void DisconnectByObject(UEPtr<UContainer> brklevel);

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

protected:
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Возвращает false если na уже подключен к этому входу.
// При успешном подключении c_index содержит реальный индекс подключенного входа
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

/// Выполняет действия после физически установленой связи
virtual bool AConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

// Разрывает все связи с элементом сети 'na'
virtual void DisconnectFromItem(UEPtr<UItem> na);

/// Разрывает связь с элементом сети 'na' и выходом 'item_property_name'
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name);

/// Разрывает связь с элементом сети 'na', выходом 'item_property_name' и входом 'connector_property_name'
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int c_index=-1);

// Выполняет действия после физически разорваной связи
virtual void ADisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int c_index=-1);
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

class RDK_LIB_TYPE UIPropertyInputBase: virtual public UIPropertyInput
{
protected: // Данные
/// Данные подключенных ко входу источников
std::vector<UCItem> ItemsList;

/// Тип входа
int InputType;

public:
/// Конструкторы и деструкторы
UIPropertyInputBase(void);
virtual ~UIPropertyInputBase(void);

/// Возвращает тип свойства ввода-вывода
virtual int GetInputType(void) const;

public: // Методы доступа к источнику данных
/// Возвращает указатель на компонент-источник
virtual const std::vector<UCItem>& GetItemsList(void) const;

/// Возвращает число подключений ко входу
virtual int GetNumConnections(void) const;

// Возвращает указатель на компонент-источник
UItem* GetItem(int c_index=-1);

/// Возвращает имя подключенного компонента
virtual std::string GetItemName(int c_index=-1) const;

/// Возвращает полное имя подключенного компонента
virtual std::string GetItemFullName(int c_index=-1) const;

/// Возвращает имя подключенного выхода
virtual std::string GetItemOutputName(int c_index=-1) const;

/// Возвращает true если вход имеет подключение
virtual bool IsConnected(void) const;

/// Разрывает связь с элементом сети подключенным ко входу 'item_property_name'
/// Возвращает true, если действия по удалению выполнялись
virtual bool Disconnect(const NameT &item_property_name, int c_index=-1);

/// Разрывает связь с индексом c_index, или все связи если c_index == -1
/// Если c_index имеет не корректное значение, то не делает ничего
virtual void Disconnect(int c_index=-1);

/// Разрывает все связи со свойством
virtual void DisconnectAll(void);

// Проверяет, существует ли связь с заданным коннектором
bool CheckLink(const UEPtr<UItem> &item) const;

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name, int c_index=-1) const;

public: // Методы управления указателем на входные данные
/// Возвращает указатель на данные
virtual void const* GetPointer(int index) const=0;

/// Устанавливает указатель на данные
virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// Сбрасывает указатель на данные
virtual bool ResetPointer(int index, void* value)=0;

protected:
/// Подключает выход
virtual bool Connect(UItem* item, const std::string &output_name, int c_index=-1, bool forced_connect_same_item=false);

// Разрывает все связи с элементом сети 'na'
virtual void Disconnect(UEPtr<UItem> na);

/// Разрывает связь с элементом сети 'na' и выходом 'item_property_name'
virtual void Disconnect(UEPtr<UItem> na, const NameT &item_property_name);

/// Разрывает связь с элементом сети 'na', выходом 'item_property_name' и входом 'connector_property_name'
virtual void Disconnect(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int c_index=-1);
};

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

 std::vector<UEPtr<UIPropertyInput> > properties;
 ReadInputPropertiesList(properties);

// std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
// for(;I != ConnectedItemList.end();++I)
 for(size_t j=0;j<properties.size();j++)
 {
  const std::vector<UCItem>& items=properties[j]->GetItemsList();
  for(size_t i=0;i<items.size();i++)
  {
   if(items[i].Item)
   {
	if(exclude_internals)
	{
	 if(reinterpret_cast<UContainer*>(items[i].Item)->CheckOwner(internal_level))
	  continue;
	}
	reinterpret_cast<UContainer*>(items[i].Item)->GetLongId(netlevel,item.Id);
	connector.Index=-1;//i;
	connector.Name=properties[j]->GetName();

	item.Index=-1;//CItemList[i].Index;
	item.Name=items[i].Name;//CItemList[i].Name;
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
   if(reinterpret_cast<UContainer*>(I->second[i].Item) == cont)
   {
	reinterpret_cast<UContainer*>(I->second[i].Item)->GetLongId(netlevel,item.Id);
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

//#include "UPropertyIO.h"
#endif
