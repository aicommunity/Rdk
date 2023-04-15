/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAItemCPP
#define UAItemCPP

#include <string.h>
#include "UItem.h"

namespace RDK {

/* *************************************************************************** */
// Контейнер - список указателей на подключенные элементы
// class UAConnectorVector
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAConnectorVector::UAConnectorVector(void)
{
 // Размер контейнера
 Size=0;

 // Массив элементов
 Data=0;

 // Реальный размер контейнера
 RealSize=0;
}

UAConnectorVector::UAConnectorVector(const UAConnectorVector &copy)
{
 *this=copy;
}


UAConnectorVector::~UAConnectorVector(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает контейнер
void UAConnectorVector::Clear(void)
{
 Size=0;
}

// Меняет размер контейнера с сохранением элементов
void UAConnectorVector::Resize(int newsize)
{
 if(Size == newsize)
  return;

 if(RealSize<newsize || !Data)
 {
  PUAConnector* newbuffer=new PUAConnector[newsize];
  memcpy(newbuffer,Data,sizeof(PUAConnector)*Size);

  if(Data)
   delete []Data;
  Data=newbuffer;
  RealSize=newsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Ищет в контейнере первый заданный элемент начиная с индекса index
// и возвращает его индекс или -1 если элемент не найден
int UAConnectorVector::Find(const PUAConnector &item, int index) const
{
 PUAConnector *pdata=Data;

 for(int i=index;i<Size;i++,pdata++)
  if(*pdata == item)
   return i;

 return -1;
}

// Добавляет в конец контейнера элемент
void UAConnectorVector::Add(const PUAConnector &item)
{
 Resize(Size+1);
 Data[Size-1]=item;
}

// Добавляет в конец контейнера элемент только если таких элементов еще нет
// Возвращает true в случае успешного добавления
bool UAConnectorVector::AddUnique(const PUAConnector &item)
{
 if(Find(item)>=0)
  return false;

 Add(item);
 return true;
}

// Удаляет элемент по индексу
void UAConnectorVector::Del(int index)
{
 if(index<0 || index >= Size)
  return;

 if(index < Size-1)
 {
  memcpy(Data+index,Data+index+1,(Size-index-1)*sizeof(PUAConnector));
 }

 Resize(Size-1);
}

// Удаляет элемент по указателю
void UAConnectorVector::Del(PUAConnector *item)
{
 int index=Find(*item);
 Del(index);
}
// --------------------------

// --------------------------
// Методы доступа
// --------------------------
// Оператор присваивания
UAConnectorVector& UAConnectorVector::operator = (const UAConnectorVector &copy)
{
 Resize(copy.Size);
 memcpy(Data,copy.Data,Size*sizeof(PUAConnector));

 return *this;
}

// Оператор доступа к элементу
PUAConnector& UAConnectorVector::operator [] (int index)
{
 return Data[index];
}

const PUAConnector& UAConnectorVector::operator [] (int index) const
{
 return Data[index];
}

// Оператор доступа к массиву
PUAConnector* UAConnectorVector::GetData(void)
{
 return Data;
}

// Оператор доступа к размеру
int UAConnectorVector::GetSize(void) const
{
 return Size;
}
// --------------------------
/* *************************************************************************** */

/* *************************************************************************** */
// Контейнер - список указателей на подключенные элементы
// class UAConnector2DVector
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAConnector2DVector::UAConnector2DVector(void)
{
 // Размер контейнера
 Size=0;

 // Массив элементов
 Data=0;

 // Реальный размер контейнера
 RealSize=0;
}

UAConnector2DVector::UAConnector2DVector(const UAConnector2DVector &copy)
{
 *this=copy;
}


UAConnector2DVector::~UAConnector2DVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает контейнер
void UAConnector2DVector::Clear(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
 }
 Size=RealSize=0;
}

// Меняет размер контейнера с сохранением элементов
void UAConnector2DVector::Resize(int newsize)
{
 if(Size == newsize)
  return;

 if(RealSize<newsize || !Data)
 {
  UAConnectorVector* newbuffer=new UAConnectorVector[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Data[i];
//  memcpy(newbuffer,Data,sizeof(UAConnectorVector)*Size);

  if(Data)
   delete []Data;
  Data=newbuffer;
  RealSize=newsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Добавляет в конец контейнера элемент
void UAConnector2DVector::Add(const UAConnectorVector &item)
{
 Resize(Size+1);
 Data[Size-1]=item;
}

// Удаляет элемент по индексу
void UAConnector2DVector::Del(int index)
{
 if(index<0 || index >= Size)
  return;

 for(int i=index;i<Size-1;i++)
  Data[i]=Data[i+1];
 Resize(Size-1);
}
// --------------------------

// --------------------------
// Методы доступа
// --------------------------
// Оператор присваивания
UAConnector2DVector& UAConnector2DVector::operator = (const UAConnector2DVector &copy)
{
 Resize(copy.Size);

 for(int i=0;i<Size;i++)
  Data[i]=copy.Data[i];
// memcpy(Data,copy.Data,Size*sizeof(UAConnectorVector));

 return *this;
}

// Оператор доступа к элементу
UAConnectorVector& UAConnector2DVector::operator [] (int index)
{
 return Data[index];
}

const UAConnectorVector& UAConnector2DVector::operator [] (int index) const
{
 return Data[index];
}

// Оператор доступа к массиву
UAConnectorVector* UAConnector2DVector::GetData(void)
{
 return Data;
}

// Оператор доступа к размеру
int UAConnector2DVector::GetSize(void) const
{
 return Size;
}
// --------------------------
/* *************************************************************************** */

/* *************************************************************************** */
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UItem::UItem(void)
{
}

UItem::~UItem(void)
{
 DisconnectAll();
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к описанию входов и выходов
// --------------------------
/// Ищет свойство-выход по заданному индексу
void UItem::FindOutputProperty(const NameT &item_property_name, UIProperty* &property) const
{
 // Ищем указатель на выходные данные
 property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.find(item_property_name);
// // TODO: Сначала проверяем алиасы
// VariableMapCIteratorT I=PropertiesLookupTable.end();
// if(CheckAlias(item_property_name))
//  I=PropertiesLookupTable.find(GetPropertyNameByAlias(item_property_name));
// else
//  I=PropertiesLookupTable.find(item_property_name);

 if(I == PropertiesLookupTable.end())
  return;

 if(I->second.Type & ptOutput)
 {
  property=I->second.Property.Get();
 }
}


/// Возвращает указатель на свойство подключенного входа компонента-приемника
void UItem::FindConnectedProperty(const NameT &item_property_name, int index, UIProperty* &property) const
{
 property=0;
 NameT connector_property_name;
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);

 if(I == RelatedConnectors.end())
  return;

 const std::vector<PUAConnector> &vec=I->second;

 if(index<0 || index>=int(vec.size()))
  return;

 if(!vec[index])
  return;

 std::vector<UCLink> buffer;
 vec[index]->GetCLink(this,buffer);
 for(size_t k=0;k<buffer.size();k++)
 {
  UCLink &link=buffer[k];
  if(!link.InputName.empty())
  {
   if(link.OutputName == item_property_name)
   {
    property=vec[index]->FindProperty(link.InputName);
    return;
   }
  }
 }

}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UItem::Free(void)
{
 DisconnectAll();
 UConnector::Free();
}
// --------------------------

// ----------------------
// Защищенные коммуникационные методы
// ----------------------

bool UItem::ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item)
{
 if(!UConnector::ConnectToItem(na, item_property_name, connector_property_name,c_index, forced_connect_same_item))
  return false;

 // Ищем указатель на выходные данные (они гарантированно существуют, мы это проверили выше)
 UIProperty* output_property=na->FindProperty(item_property_name);

 // Ищем указатель на входные данные (они гарантированно существуют, мы это проверили выше)
 UIProperty* input_property=FindProperty(connector_property_name);


 int size=int(ConnectedItemList[connector_property_name].size());
 if(size>c_index)
  ConnectedItemList[connector_property_name][c_index].Name=item_property_name;
 else
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Fatal: c_index incorrect: ")+sntoa(c_index));

  if(input_property->GetIoType() & ipData)
  {
   if(input_property->GetIoType() & ipSingle)
   {
	  if(!input_property->SetPointer(c_index,const_cast<void*>(output_property->GetPointer(0)),output_property))
	  {
       LogMessageEx(RDK_EX_FATAL, __FUNCTION__, std::string("SetPointer ipSingle fail"));
      }
   }
   else
   if(input_property->GetIoType() & ipRange)
   {
	if(output_property)
	{
	 if(!input_property->SetPointer(c_index,const_cast<void*>(output_property->GetPointer(0)),output_property))
	 {
      LogMessageEx(RDK_EX_FATAL, __FUNCTION__, std::string("SetPointer ipRange fail"));
     }
	}
   }
  }
  else
  if(input_property->GetIoType() & ipComp)
  {
   if(!input_property->SetPointer(c_index,na, 0))
    LogMessageEx(RDK_EX_FATAL, __FUNCTION__, std::string("SetPointer ipComp fail"));
  }

 return true;
}

// Устанавливает связь с коннектором 'c'.
bool UItem::Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item)
{
 if(!c)
  return false;

 if(!Build())
  return false;

 if(!c->ConnectToItem(this,item_property_name, connector_property_name, c_index, forced_connect_same_item))
  return false;

 std::vector<PUAConnector> &vec=RelatedConnectors[item_property_name];
 for(size_t i=0;i<vec.size();i++)
  if(vec[i] == c)
   return true;

 vec.push_back(c);

 return true;
}

/// Разрывает все связи выхода этого объекта с коннектором 'c'.
void UItem::Disconnect(UEPtr<UConnector> c)
{
 Build();

 if(c)
  c->DisconnectFromItem(this);

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i] == c)
	I->second.erase(I->second.begin()+i);
   else
    ++i;
  }
 }
}

// Разрывает связь выхода этого объекта с коннектором 'c' по индексу
void UItem::Disconnect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index)
{
 if(!c)
  return;

 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.find(item_property_name);

 if(I == RelatedConnectors.end())
  return;

 UCItem citem=c->GetCItem(connector_property_name,this,connected_c_index);
 int i=0;
 while(i<int(I->second.size()))
 {
  if(I->second[i] == c)
  {
   if(citem.Name == item_property_name && citem.Item == this)
   {
	I->second.erase(I->second.begin()+i);
    c->DisconnectFromItem(this, item_property_name, connector_property_name, connected_c_index);
   }
   else
    ++i;
  }
  else
   ++i;
 }
}
// ----------------------

// ----------------------
// Коммуникационные методы
// ----------------------
// Возвращает текущее число соединений.
int UItem::GetNumAConnectors(const NameT &item_property_name) const
{
 return GetNumActiveOutputs(item_property_name);
}

/// Возвращает число выходов к которым кто-то подключен
int UItem::GetNumActiveOutputs(void) const
{
 return int(RelatedConnectors.size());
}

/// Возвращает число коннекторов к которым подключено заданное свойство
int UItem::GetNumActiveOutputs(const NameT &item_property_name) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);
 if(I == RelatedConnectors.end())
  return 0;

 return int(I->second.size());
}

// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
bool UItem::Disconnect(const UId &id)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i]->Id == id)
	Disconnect(I->second[i]);
   else
    ++i;
  }
 }

 return true;
}

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
void UItem::DisconnectAll(void)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=int(I->second.size())-1;
  while(i>=0)
  {
   Disconnect(I->second[i]);
   i=int(I->second.size())-1;
  }
 }
}

void UItem::DisconnectAll(const NameT &item_property_name)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.find(item_property_name);

 if(I!= RelatedConnectors.end())
 {
  int i=int(I->second.size())-1;
  while(i>=0)
  {
   Disconnect(I->second[i]);
   i=int(I->second.size())-1;
  }
 }
}


// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
void UItem::DisconnectBy(UEPtr<UContainer> brklevel)
{
 Build();

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(!I->second[i]->CheckOwner(brklevel))
	Disconnect(I->second[i]);
   else
    ++i;
  }
 }


}

// Переустанавливает все связи этого item со всеми connectors которые получают
// данные от этого item
void UItem::BuildLinks(void)
{

 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i])
	continue;
   std::vector<UCLink> buffer;
   I->second[i]->GetCLink(this,buffer);
   for(size_t k=0;k<buffer.size();k++)
   {
	UCLink &indexes=buffer[k];
	if(I->first == indexes.OutputName)
	{
	 int c_index(-1);
	 I->second[i]->ConnectToItem(this,indexes.OutputName,indexes.InputName,c_index);
	}
   }
  }

}

// Возвращает указатель на коннектор из списка подключений
// по имени 'name'.
UEPtr<UConnector> UItem::GetAConnector(const UId &id, int index) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   if(!I->second[i])
	continue;

   if(I->second[i]->Id == id)
    return I->second[i];
  }

 return 0;
}

// Возвращает  коннектор из списка подключений.
UEPtr<UConnector> UItem::GetAConnectorByIndex(const NameT &item_property_name, int index) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);
 if(I == RelatedConnectors.end())
  return 0;

 if(index<0 || index>= int(I->second.size()))
  return 0;
 return I->second[index];
}


// Проверяет, существует ли связь с заданным коннектором
bool UItem::CheckLink(const UEPtr<UConnector> &connector, int connected_c_index) const
{
 std::vector<UCLink> buffer;
 connector->GetCLink(this,buffer);
 for(size_t k=0;k<buffer.size();k++)
 {
  UCLink &link=buffer[k];
  if(!link.InputName.empty() && !link.OutputName.empty()
    && (connected_c_index<0 || connected_c_index == link.Input) )
   return true;
  }

 return false;
}

bool UItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const
{
 std::vector<UCLink> buffer;
 connector->GetCLink(this,buffer);
 for(size_t k=0;k<buffer.size();k++)
 {
  UCLink &link=buffer[k];
  if(!link.InputName.empty())
  {
   if(link.OutputName == item_property_name)
	return true;
  }
 }

 return false;
}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index) const
{
 std::vector<UCLink> buffer;
 connector->GetCLink(this,buffer);
 for(size_t k=0;k<buffer.size();k++)
 {
  UCLink &link=buffer[k];
  if(!link.OutputName.empty())
  {
   if((connector_property_name.empty() || link.InputName == connector_property_name) && ((connected_c_index < 0 || connected_c_index == link.Input) || link.Input<0))
	return true;
  }
 }
 return false;
}

// ----------------------


// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UItem::Default(void)
{
 return UConnector::Default();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UItem::Build(void)
{
 if(IsReady())
  return true;

 BuildLinks();

 return UConnector::Build();
}
/* *************************************************************************** */


}

#endif



