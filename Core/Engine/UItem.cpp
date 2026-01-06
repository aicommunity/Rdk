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
void UItem::FindOutputProperty(const NameT &item_property_name, UIPropertyOutput* &property) const
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
  property=dynamic_cast<UIPropertyOutput*>(I->second.Property.Get());
 }
}


/// Возвращает указатель на свойство подключенного входа компонента-приемника
void UItem::FindConnectedProperty(const NameT &item_property_name, int index, UIPropertyInput* &property) const
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
          property=dynamic_pointer_cast<UIPropertyInput>(vec[index]->FindProperty(link.InputName));
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
 UIPropertyOutput* output_property=dynamic_pointer_cast<UIPropertyOutput>(na->FindProperty(item_property_name));

 // Ищем указатель на входные данные (они гарантированно существуют, мы это проверили выше)
 UIPropertyInput* input_property=dynamic_pointer_cast<UIPropertyInput>(FindProperty(connector_property_name));


 int size=int(ConnectedItemList[connector_property_name].size());
 if(size>c_index)
  ConnectedItemList[connector_property_name][c_index].Name=item_property_name;
 else
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Fatal: c_index incorrect: ")+sntoa(c_index));

  if(!input_property->SetPointer(c_index,output_property))
  {
   LogMessageEx(RDK_EX_FATAL, __FUNCTION__, std::string("SetPointer fail"));
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



