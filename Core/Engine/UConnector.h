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

/// Возвращает число коннекторов к которым подключено заданное свойство
virtual int GetNumActiveOutputs(const NameT &item_property_name) const=0;
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
//virtual NameT FindFreeInputName(void)=0;

// Возвращает подключенный к этому коннектору объект по индексу
const UCItem& GetCItem(const NameT &connector_property_name, const UEPtr<UItem> &item, int &index) const;
const UCItem& GetCItem(const NameT &connector_property_name, int index) const;
void GetCItem(const NameT &connector_property_name, std::vector<UCItem> &buffer) const;

// Возвращает информацию об индексах связей с этим item или -1, -1
// если такая связь отсутствует
virtual void GetCLink(const UEPtr<UItem> &item, std::vector<UCLink> &buffer) const;
virtual void GetCLink(const UItem* const item, std::vector<UCLink> &buffer) const;
// --------------------------

// --------------------------
// Методы доступа к описанию входов и выходов
// --------------------------
/// Ищет свойство-вход по заданному индексу
virtual void FindInputProperty(const NameT &connector_property_name, UIProperty* &property) const;

/// Возвращает указатель на свойство подключенного входа компонента-приемника
virtual void FindConnectedProperty(const NameT &item_property_name, int index, UIProperty* &property) const=0;
// --------------------------

// ----------------------
// Коммуникационные методы
// ----------------------
protected:
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Возвращает false если na уже подключен к этому входу.
// При успешном подключении c_index содержит реальный индекс подключенного входа
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

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
// Проверяет, допустимо ли подключение заданного item к этому коннектору
//bool CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name);

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
explicit EInvalidInputIndex(int index) : EInvalidIndex(index) {};
};

// Такой вход не существует
struct EInputIndexNotExist: public EInvalidIndex
{
explicit EInputIndexNotExist(int index) : EInvalidIndex(index) {};
};
};

class RDK_LIB_TYPE UIPropertyInput
{
protected: // Данные
/// Указатель на компонент-источник данных
UItem* Item;

/// Имя выхода компнента-источника данных
std::string ItemOutputName;

public:
/// Конструкторы и деструкторы
UIPropertyInput(void);
virtual ~UIPropertyInput(void);

/// Возвращает указатель на компонент-источник
virtual UItem* GetItem(void);

/// Возвращает имя подключенного компонента
virtual std::string GetItemName(void) const;

/// Возвращает полное имя подключенного компонента
virtual std::string GetItemFullName(void) const;

/// Возвращает имя подключенного выхода
virtual std::string GetItemOutputName(void) const;

/// Инициализирует данные
virtual void Init(UItem* item, const std::string &output_name);

/// Деинициализирует данные
virtual void UnInit(void);
};

class RDK_LIB_TYPE UIPropertyOutput
{
protected: // Данные
/// Указатели на компоненты-приемники данных
std::vector<UItem*> Connectors;

/// Имена входов компнентов-приемников данных
std::vector<std::string> ConnectorInputNames;

public:
/// Конструкторы и деструкторы
UIPropertyOutput(void);
virtual ~UIPropertyOutput(void);

/// Возвращает число подключенных входов
virtual size_t GetNumConnectors(void) const;

/// Возвращает указатель на компонент-приемник
virtual UConnector* GetConnector(int index);

/// Возвращает имя подключенного входа компонента-приемника
virtual std::string GetConnectorInputName(int index) const;

/// Возвращает указатель на свойство подключенного входа компонента-приемника
virtual UIProperty* GetConnectorProperty(int index);
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


 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(I->second[i].Item)
   {
	if(exclude_internals)
	{
	 if(reinterpret_cast<UContainer*>(I->second[i].Item)->CheckOwner(internal_level))
	  continue;
	}
   reinterpret_cast<UContainer*>(I->second[i].Item)->GetLongId(netlevel,item.Id);
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

#include "UPropertyIO.h"
#endif
