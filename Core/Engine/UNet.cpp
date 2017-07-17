/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UANET_CPP
#define UANET_CPP

#include "UNet.h"
#include "UXMLEnvSerialize.h"

namespace RDK {

//class UIPropertyInputBase: public UIPropertyIO
/// Конструкторы и деструкторы
UIPropertyInputBase::UIPropertyInputBase(void)
{
}

UIPropertyInputBase::~UIPropertyInputBase(void)
{
}


/// Возвращает тип свойства входа

int UIPropertyInputBase::GetInputType(void) const
{
 return InputType;
}


/// Возвращает число подключений ко входу
int UIPropertyInputBase::GetNumConnections(void) const
{
 return int(ConnectedProperties.size());
}

/// Возвращает указатели на свойства-источники данных
const std::list<UEPtr<UIPropertyOutput> > UIPropertyInputBase::GetConnectedProperties(void) const
{
 return ConnectedProperties;
}

/// Возвращает указатель на компонент-источник
/*
const std::vector<UCItem>& UIPropertyInputBase::GetItemsList(void) const
{
 return ItemsList;
}         */

							/*
// Возвращает указатель на компонент-источник
const UNet* UIPropertyInputBase::GetConnectedItem(int c_index) const
{
 if(ConnectedProperties.empty())
  return 0;

 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=ConnectedProperties.begin();
 int i=0;
 for(;I != ConnectedProperties.end();++I,++i)
 {
  if(i == c_index)
   return (*I)->GetOwner();
 }

 return 0;
}

UNet* UIPropertyInputBase::GetConnectedItem(int c_index)
{
 if(ConnectedProperties.empty())
  return 0;

 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=ConnectedProperties.begin();
 int i=0;
 for(;I != ConnectedProperties.end();++I,++i)
 {
  if(i == c_index)
   return (*I)->GetOwner();
 }

 return 0;
}

/// Возвращает имя подключенного компонента
std::string UIPropertyInputBase::GetItemName(int c_index) const
{
 const UNet* item=GetConnectedItem(c_index);
 if(!item)
  return "";

 return item->GetName();
}
				  */
/// Возвращает полное имя подключенного компонента
std::string UIPropertyInputBase::GetItemFullName(int c_index) const
{
 if(ConnectedProperties.empty())
  return 0;

 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=ConnectedProperties.begin();
 int i=0;
 for(;I != ConnectedProperties.end();++I,++i)
 {
  if(i == c_index)
   return (*I)->GetOwner()->GetFullName();
 }

 return "";
}

// Возвращает имя подключенного выхода
std::string UIPropertyInputBase::GetItemOutputName(int c_index) const
{
 if(ConnectedProperties.empty())
  return 0;

 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=ConnectedProperties.begin();
 int i=0;
 for(;I != ConnectedProperties.end();++I,++i)
 {
  if(i == c_index)
   return (*I)->GetName();
 }

 return "";
}

/// Возвращает true если вход имеет подключение
bool UIPropertyInputBase::IsConnected(void) const
{
 return (ConnectedProperties.empty())?false:true;
}

// Проверяет, существует ли связь с заданным коннектором
bool UIPropertyInputBase::IsConnectedTo(const UIPropertyOutput *output_property) const
{
 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),output_property);
 return I == ConnectedProperties.end();
}
	   /*
// Проверяет, существует ли связь с заданным коннектором
bool UIPropertyInputBase::IsConnectedTo(const UEPtr<UNet> &item) const
{
 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=ConnectedProperties.begin();
 for(;I != ConnectedProperties.end();++I)
 {
  if((*I)->GetOwner() == item)
   return true;
 }
 return false;
}                */

/// Разрывает связь со свойством output_property
bool UIPropertyInputBase::Disconnect(UIPropertyOutput *output_property)
{
 if(!output_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property pointer is null"));
  return false;
 }

 std::list<UEPtr<UIPropertyOutput> >::iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),output_property);

 if(I == ConnectedProperties.end())
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found in ConnectedProperties list: ")+output_property->GetName());
  return false;
 }

 return DisconnectFinalAction(I);
}

/// Разрывает все связи со свойством
bool UIPropertyInputBase::DisconnectAll(void)
{
 bool res(true);
 std::list<UEPtr<UIPropertyOutput> >::iterator I=ConnectedProperties.begin(),J;

 while(I != ConnectedProperties.end())
 {
  J=I; ++J;
  bool local_res=DisconnectFinalAction(I);
  res &=local_res;
  I=J;
 }
 return res;
}
									 /*
// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UIPropertyInputBase::IsConnectedTo(const UEPtr<UNet> &item, const NameT &item_property_name, int c_index) const
{
 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=ConnectedProperties.begin();
 for(;I != ConnectedProperties.end();++I)
 {
  if((*I)->GetOwner() == item)
  {
   if((*I)->GetName() == item_property_name)
    return true;
  }
 }
 return false;
}
                                   */
/// Завершающие действия по отключению
bool UIPropertyInputBase::DisconnectFinalAction(std::list<UEPtr<UIPropertyOutput> >::iterator &it)
{
 // TODO: тут код физического отключения данных
 ConnectedProperties.erase(it);
 return true;
}

/* *************************************************************************** */
//class UIPropertyOutputBase: public UIPropertyIO
/// Конструкторы и деструкторы
UIPropertyOutputBase::UIPropertyOutputBase(void)
{
}

UIPropertyOutputBase::~UIPropertyOutputBase(void)
{
// Connectors.clear();
// ConnectorInputNames.clear();
 ConnectedProperties.clear();
}

// Возвращает число подключенных входов
size_t UIPropertyOutputBase::GetNumConnectors(void) const
{
 return ConnectedProperties.size();
}
					/*
// Возвращает указатель на компонент-приемник
const UNet* UIPropertyOutputBase::GetConnector(int index) const
{
 return Connectors[index];
}

UNet* UIPropertyOutputBase::GetConnector(int index)
{
 return Connectors[index];
}                    */

/// Возвращает указатели на свойства-приемники данных
const std::list<UEPtr<UIPropertyInput> > UIPropertyOutputBase::GetConnectedProperties(void) const
{
 return ConnectedProperties;
}
	   /*
// Возвращает имя подключенного входа компонента-приемника
std::string UIPropertyOutputBase::GetConnectorInputName(int index) const
{
 return ConnectorInputNames[index];
}             */
  /*
// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
bool UIPropertyOutputBase::Disconnect(const UId &id)
{
}   */

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
bool UIPropertyOutputBase::DisconnectAll(void)
{
 /*
 std::map<std::string, std::vector<PUAConnector> >::iterator I=RelatedConnectors.begin();

 for(;I!= RelatedConnectors.end();++I)
 {
  int i=int(I->second.size())-1;
  while(i>=0)
  {
   Disconnect(I->second[i]);
   i=int(I->second.size())-1;
  }
 }   */
}

/// Возвращает true если выход подключен к выбранному входу
bool UIPropertyOutputBase::IsConnectedTo(UIPropertyInput *input_property)
{
 std::list<UEPtr<UIPropertyInput> >::iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),input_property);
 if(I == ConnectedProperties.end())
  return false;

 return true;
}

/// Возвращает true если выход подключен к одному из входов выбранного компонента
bool UIPropertyOutputBase::IsConnectedTo(UNet *component)
{
 std::list<UEPtr<UIPropertyInput> >::iterator I=ConnectedProperties.begin();
 for(;I != ConnectedProperties.end();I++)
  if((*I)->GetOwner() == component)
   return true;

 return false;
}

// Устанавливает связь с коннектором 'c'
/*
bool UIPropertyOutputBase::Connect(UEPtr<UNet> c, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item)
{

 std::vector<PUAConnector> &vec=RelatedConnectors[item_property_name];
 for(size_t i=0;i<vec.size();i++)
  if(vec[i] == c)
   return true;

 vec.push_back(c);
 return true;


 return false;
}   */

/// Разрывает все связи выхода этого объекта с коннектором 'c'.
/*bool UIPropertyOutputBase::Disconnect(UEPtr<UNet> c)
{

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
 */
// Разрывает связь выхода этого объекта с коннектором 'c' по индексу
/*
bool UIPropertyOutputBase::Disconnect(UEPtr<UNet> c, const NameT &connector_property_name, int c_index)
{

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
}    */
							 /*
// Возвращает  коннектор из списка подключений.
UEPtr<UConnector> UIPropertyOutputBase::GetAConnectorByIndex(int c_index) const
{
}
							   */
							   /*
// Проверяет, существует ли связь с заданным коннектором
bool UIPropertyOutputBase::CheckLink(const UEPtr<UNet> &connector, int c_index) const
{
	 return false;

}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UIPropertyOutputBase::CheckLink(const UEPtr<UNet> &connector, const NameT &connector_property_name, int c_index) const
{
	 return false;

}
                                 */    /*
// Переустанавливает все связи этого выхода со всеми connectors
void UIPropertyOutputBase::BuildLinks(void)
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
     */
/* *************************************************************************** */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UNet::UNet(void)
{
}

UNet::~UNet(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool UNet::CheckComponentType(UEPtr<UContainer> comp) const
{
 return (dynamic_pointer_cast<UNet>(comp))?true:false;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool UNet::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool UNet::ADelComponent(UEPtr<UContainer> comp)
{
 if(!comp->IsMoving())
 {
  if(dynamic_pointer_cast<UNet>(comp))
   static_pointer_cast<UNet>(comp)->BreakLinks(this);
 }
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
// Если 'stor' == 0, то создание объектов осуществляется
// в том же хранилище где располагается этот объект
bool UNet::Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor, bool copystate) const
{
 ULinksList linkslist;
 ULinksList oldlinkslist;

 if(!dynamic_pointer_cast<UNet>(target))
  return false;

 if(UContainer::Copy(target,stor,copystate))
  {
   static_pointer_cast<UNet>(target)->BreakLinks();

   if(static_pointer_cast<UNet>(target)->CreateLinks(GetLinks(linkslist,const_cast<UNet*>(this))))
    return true;
  }

 return false;
}

// Осуществляет освобождение этого объекта в его хранилище
// или вызов деструктора, если Storage == 0
void UNet::Free(void)
{
 UContainer::Free();
}
// --------------------------

// ----------------------
// Методы управления связями
// ----------------------
/*
bool UNet::CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index)
{
 return CreateLink(ULinkSide(item_id,item_index),ULinkSide(conn_id,conn_index));
} */

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
/*bool UNet::CreateLink(const NameT &item, int item_index,
						const NameT &connector, int connector_index, bool forced_connect_same_item)
{
 return CreateLink(UStringLinkSide(item,item_index),UStringLinkSide(connector,connector_index), forced_connect_same_item);
} */

// Устанавливает новую связь 'link'
bool UNet::CreateLink(const ULink &link, bool forced_connect_same_item)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
 {
  res &=CreateLink(link.Item, link.Connector[i], forced_connect_same_item);
 }

 return res;
}

// Устанавливает новую связь между выходом элемента сети
// 'item' и коннектором 'connector'
bool UNet::CreateLink(const ULinkSide &item, const ULinkSide &connector, bool forced_connect_same_item)
{
 UEPtr<UNet> pitem;
 if(item.ComponentName.empty())
  pitem=this;
 else
  pitem=GetComponentL<UNet>(item.ComponentName,true);

 UEPtr<UNet> pconnector=0;
 if(connector.ComponentName.empty())
  pconnector=this;
 else
  pconnector=GetComponentL<UNet>(connector.ComponentName,true);

 if(!pitem)
 {
  std::ostringstream stream;
  stream<<"Item not found when connect "<<item.ComponentName<<":"<<item.PropertyName<<" to "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
  return false;
 }

 if(!pconnector)
 {
  std::ostringstream stream;
  stream<<"Connector not found when connect "<<item.ComponentName<<":"<<item.PropertyName<<" to "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());

  return false;
 }

 UIPropertyInput *input_property=pconnector->FindInputProperty(connector.PropertyName);
 UIPropertyOutput *output_property=pitem->FindOutputProperty(item.PropertyName);

 if(!output_property)
 {
  std::ostringstream stream;
  stream<<"Item`s output property not found when connect "<<item.ComponentName<<":"<<item.PropertyName<<" to "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
  return false;
 }

 if(!input_property)
 {
  std::ostringstream stream;
  stream<<"Connector`s input property not found when connect "<<item.ComponentName<<":"<<item.PropertyName<<" to "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());

  return false;
 }

 return output_property->Connect(input_property);
}

bool UNet::CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index, int connector_c_index, bool forced_connect_same_item)
{
 return CreateLink(ULinkSide(item,item_index),ULinkSide(connector,connector_index, connector_c_index), forced_connect_same_item);
}

// Устанавливает все связи из массива 'linkslist'.
bool UNet::CreateLinks(const ULinksList &linkslist, UEPtr<UNet> owner_level)
{
 bool res=true;

 int i=0;
 for(i=0;i<linkslist.GetSize();i++)
 {
  if(owner_level)
   res&=owner_level->CreateLink(linkslist[i]);
  else
   res&=CreateLink(linkslist[i]);
 }

 if(!res)
  return false;

 return res;
}

// Разрывает связь 'link'
bool UNet::BreakLink(const ULink &link)
{
 bool res=true;
 for(size_t i=0;i<link.Connector.size();i++)
  res&=BreakLink(link.Item,link.Connector[i]);

 return res;
}

// Разрывает связь между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
bool UNet::BreakLink(const ULinkSide &item, const ULinkSide &connector)
{
 UEPtr<UNet> pitem=0;
 if(item.ComponentName.empty())
  pitem=this;
 else
  pitem=GetComponentL<UNet>(item.ComponentName,true);

 UEPtr<UNet> pconnector=0;
 if(connector.ComponentName.empty())
  pconnector=this;
 else
  pconnector=GetComponentL<UNet>(connector.ComponentName,true);

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.PropertyName);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.PropertyName);
  return false;
 }

 UIPropertyInput *input_property=pconnector->FindInputProperty(connector.PropertyName);
 UIPropertyOutput *output_property=pitem->FindOutputProperty(item.PropertyName);

 if(!output_property)
 {
  std::ostringstream stream;
  stream<<"Item`s output property not found when disconnect "<<item.ComponentName<<":"<<item.PropertyName<<" from "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
  return false;
 }

 if(!input_property)
 {
  std::ostringstream stream;
  stream<<"Connector`s input property not found when disconnect "<<item.ComponentName<<":"<<item.PropertyName<<" from "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());

  return false;
 }

 return output_property->Disconnect(input_property);
}

bool UNet::BreakLink(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index)
{
 return BreakLink(ULinkSide(itemname,item_property_name), ULinkSide(connectorname,connector_property_name, connector_c_index));
}

// Разрывает все связи между выходом элемента сети, 'itemid'
// и коннектором 'connectorid'
bool UNet::BreakLink(const NameT &itemname, const NameT &connectorname)
{
 UEPtr<UNet> pitem=0;
 if(itemname.empty())
  pitem=this;
 else
  pitem=GetComponentL<UNet>(itemname,true);

 UEPtr<UNet> pconnector=0;
 if(connectorname.empty())
  pconnector=this;
 else
  pconnector=GetComponentL<UNet>(connectorname,true);

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+itemname);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return false;
 }

 std::vector<UEPtr<UIPropertyInput> > input_properties;
 std::vector<UEPtr<UIPropertyOutput> > output_properties;
 pconnector->FindPropertiesByType<UIPropertyInput>(input_properties);
 pitem->FindPropertiesByType<UIPropertyOutput>(output_properties);
 bool res(true);
 for(size_t i=0;i<output_properties.size();i++)
 {
  for(size_t j=0;j<input_properties.size();j++)
  {
   res&=output_properties[i]->Disconnect(input_properties[j]);
  }
 }

 return res;
}



// Разрывает все связи между выходом элемента сети и любыми коннекторами
bool UNet::BreakOutputLinks(void)
{
 std::vector<UEPtr<UIPropertyOutput> > output_properties;
 FindPropertiesByType<UIPropertyOutput>(output_properties);
 bool res(true);
 for(size_t i=0;i<output_properties.size();i++)
 {
  res&=output_properties[i]->DisconnectAll();
 }

 return res;
}

bool UNet::BreakOutputLinks(const NameT &itemname)
{
 UEPtr<UNet> pitem=0;
 if(!itemname.empty())
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UNet>(GetComponentL(itemname,true));

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+itemname);
  return false;
 }

 return BreakOutputLinks();
}

bool UNet::BreakOutputLinks(const NameT &itemname, const NameT &item_property_name)
{
 UEPtr<UNet> pitem=0;
 if(!itemname.empty())
  pitem=this;
 else
  pitem=GetComponentL<UNet>(itemname,true);

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+itemname);
  return false;
 }

 UIPropertyOutput *output_property=pitem->FindOutputProperty(item_property_name);

 if(!output_property)
 {
  std::ostringstream stream;
  stream<<"Item`s output property not found when disconnect "<<itemname<<":"<<item_property_name;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
  return false;
 }


 return output_property->DisconnectAll();
}

bool UNet::BreakInputLinks(void)
{
 std::vector<UEPtr<UIPropertyInput> > input_properties;
 FindPropertiesByType<UIPropertyInput>(input_properties);
 bool res(true);
 for(size_t i=0;i<input_properties.size();i++)
 {
  res&=input_properties[i]->DisconnectAll();
 }

 return res;
}

bool UNet::BreakInputLinks(const NameT &connectorname, const NameT &connector_property_name, int connector_c_index)
{
 UEPtr<UNet> pconnector=0;
 if(!connectorname.empty())
  pconnector=this;
 else
  pconnector=GetComponentL<UNet>(connectorname,true);

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return false;
 }

 UIPropertyInput *input_property=pconnector->FindInputProperty(connector_property_name);

 if(!input_property)
 {
  std::ostringstream stream;
  stream<<"Connector`s input property not found when disconnect all from"<<connectorname<<":"<<connector_property_name;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());

  return false;
 }
 return input_property->DisconnectAll();
}


// Разрывает все связи сети
// исключая ее внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
bool UNet::BreakLinks(UEPtr<UContainer> brklevel)
{
 bool res(true);
 for(int i=0;i<NumComponents;i++)
  {
   res&=GetComponentByIndex<UNet>(i)->DisconnectBy(brklevel);
  }
 return res;
}

// Разрывает все связи объекта
// исключая его внутренние связи и обратные связи
// brklevel - объект, относительно которого связи считаются внутренними
bool UNet::DisconnectBy(UEPtr<UContainer> brklevel)
{
 Build();

 std::vector<UEPtr<UIPropertyOutput> > buffer;
 FindPropertiesByType<UIPropertyOutput>(buffer);
 for(size_t j=0;j<buffer.size();j++)
 {
  size_t i=0;
  std::list<UEPtr<UIPropertyInput> >::const_iterator I=buffer[j]->GetConnectedProperties().begin(),J;

  while(I != buffer[j]->GetConnectedProperties().end())
  {
   UEPtr<UIPropertyInput> input=*I;
   if(!input->GetOwner()->CheckOwner(brklevel))
   {
	J=++I;
	buffer[j]->Disconnect(input);
	I=J;
   }
   else
	++I;
  }
 }

}

// Разрывает заданные связи сети
bool UNet::BreakLinks(const ULinksList &linkslist)
{
 bool res=true;

 for(int i=0;i<linkslist.GetSize();i++)
  res&=BreakLink(linkslist[i]);

 return res;
}

// Разрывает все внутренние связи сети.
void UNet::BreakLinks(void)
{
 for(int i=0;i<NumComponents;i++)
  if(dynamic_pointer_cast<UNet>(PComponents[i]))
   static_pointer_cast<UNet>(PComponents[i])->BreakLinks();

 BreakOutputLinks();
 BreakInputLinks();
}

// Проверяет, существует ли заданная связь
bool UNet::IsLinkExists(const ULinkSide &item, const ULinkSide &connector)
{
 UEPtr<UNet> pitem=GetComponentL<UNet>(item.ComponentName,true);
 UEPtr<UNet> pconnector=GetComponentL<UNet>(connector.ComponentName,true);
 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.ComponentName);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.ComponentName);
  return false;
 }


 UIPropertyInput *input_property=pconnector->FindInputProperty(connector.PropertyName);
 UIPropertyOutput *output_property=pitem->FindOutputProperty(item.PropertyName);

 if(!output_property)
 {
  std::ostringstream stream;
  stream<<"Item`s output property not found when link check "<<item.ComponentName<<":"<<item.PropertyName<<" from "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());
  return false;
 }

 if(!input_property)
 {
  std::ostringstream stream;
  stream<<"Connector`s input property not found when link check "<<item.ComponentName<<":"<<item.PropertyName<<" from "<<connector.ComponentName<<":"<<connector.PropertyName;
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, stream.str());

  return false;
 }

 return output_property->IsConnectedTo(input_property);
}

bool UNet::IsLinkExists(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index)
{
 return IsLinkExists(ULinkSide(itemname,item_property_name),ULinkSide(connectorname,connector_property_name, connector_c_index));
}

bool UNet::IsLinkExists(const NameT &itemname,const NameT &connectorname)
{
 UEPtr<UNet> pitem=GetComponentL<UNet>(itemname,true);
 UEPtr<UNet> pconnector=GetComponentL<UNet>(connectorname,true);
 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+itemname);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connectorname);
  return false;
 }

 std::vector<UEPtr<UIPropertyOutput> > output_properties;
 pitem->FindPropertiesByType<UIPropertyOutput>(output_properties);
 bool res(true);
 for(size_t i=0;i<output_properties.size();i++)
 {
  if(output_properties[i]->IsConnectedTo(pconnector))
	return true;
 }
 return false;
}
// --------------------------

// --------------------------
// Скрытые методы доступа к свойствам
// --------------------------
// Возращает все связи компонента в виде xml в буфер buffer
ULinksList& UNet::GetLinks(ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals, UEPtr<UNet> internal_level) const
{
 return GetLinks(const_cast<UNet*>(this), linkslist, netlevel, exclude_internals, internal_level);
}

// Возращает все связи компонента в виде xml в буфер buffer
// включая связи этого компонента
// если 'sublevel' == -1, то возвращает также все связи между объектом и любым дочерним компонентом
// второго объекта. Работает симметрично в обе стороны.
// если 'sublevel' == 0, то возвращает связи только между этими объектами
ULinksList& UNet::GetPersonalLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, int sublevel)
{
 return GetPersonalLinks(const_cast<UNet*>(this), cont, linkslist, netlevel);
}

ULinksList& UNet::GetLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals, UEPtr<UNet> internal_level) const
{
 cont->GetOutputLinks(linkslist,netlevel, exclude_internals,internal_level);
 cont->GetInputLinks(linkslist,netlevel, exclude_internals,internal_level);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetLinks(cont->GetComponentByIndex<UNet>(i), linkslist, netlevel, exclude_internals,internal_level);

 return linkslist;
}

ULinksList& UNet::GetPersonalLinks(UEPtr<UNet> cont, UEPtr<UNet> cont2, ULinksList &linkslist, UEPtr<UNet> netlevel) const
{
 cont->GetOutputPersonalLinks(cont2,linkslist,netlevel);
 cont->GetInputPersonalLinks(cont2,linkslist,netlevel);

 for(int i=0;i<cont->GetNumComponents();i++)
  GetPersonalLinks(cont->GetComponentByIndex<UNet>(i), cont2, linkslist, netlevel);

 return linkslist;
}

// Возращает все связи компонента в виде xml в буфер buffer
ULinksList& UNet::GetOutputLinks(ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals, UEPtr<UNet> internal_level) const
{
 ULink link;
 ULinkSide item;
 ULinkSide connector;

 GetLongName(netlevel,item.ComponentName);
 if(item.ComponentName.empty())
  return linkslist;
 link.Item=item;

 std::vector<UEPtr<UIPropertyOutput> > output_properties;
 this->FindPropertiesByType<UIPropertyOutput>(output_properties);
 for(size_t i=0;i<output_properties.size();i++)
 {
  link.Connector.clear();
  const std::list<UEPtr<UIPropertyInput> >& connected_properties=output_properties[i]->GetConnectedProperties();
  std::list<UEPtr<UIPropertyInput> >::const_iterator I=connected_properties.begin();
  for(;I != connected_properties.end();I++)
  {
   const UEPtr<UIPropertyInput> input_property=*I;
   UNet* curr_conn(input_property->GetOwner());
   if(exclude_internals)
   {
	if(curr_conn->CheckOwner(internal_level))
	 continue;
   }
   curr_conn->GetLongName(netlevel,connector.ComponentName);
   if(!connector.ComponentName.empty())
   {
	  link.Item.Index=-1;
	  link.Item.PropertyName=output_properties[i]->GetName();
	  connector.Index=-1;
	  connector.PropertyName=input_property->GetName();

	  link.Connector.push_back(connector);
	  linkslist.Set(link);
   }
  }

 }
 return linkslist;
}

ULinksList& UNet::GetInputLinks(ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals, UEPtr<UNet> internal_level) const
{
 ULink link;
 ULinkSide connector;
 ULinkSide item;
 GetLongName(netlevel,connector.ComponentName);
 if(connector.ComponentName.empty())
  return linkslist;

 std::vector<UEPtr<UIPropertyInput> > input_properties;
 FindPropertiesByType<UIPropertyInput>(input_properties);

 for(size_t j=0;j<input_properties.size();j++)
 {
  const std::list<UEPtr<UIPropertyOutput> >& connected_properties=input_properties[j]->GetConnectedProperties();
  std::list<UEPtr<UIPropertyOutput> >::const_iterator I=connected_properties.begin();
  for(;I != connected_properties.end();I++)
  {
   const UIPropertyOutput* output_property=*I;
	if(exclude_internals)
	{
	 if(output_property->GetOwner()->CheckOwner(internal_level))
	  continue;
	}
	output_property->GetOwner()->GetLongName(netlevel,item.ComponentName);
    connector.Index=-1;
	connector.PropertyName=input_properties[j]->GetName();

	item.Index=-1;
	item.PropertyName=output_property->GetName();
	if(!connector.ComponentName.empty())
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

// Возращает все связи компонента в виде xml в буфер buffer
// включая связи этого компонента
// если 'sublevel' == -1, то возвращает также все связи между объектом и любым дочерним компонентом
// второго объекта. Работает симметрично в обе стороны.
// если 'sublevel' == 0, то возвращает связи только между этими объектами
ULinksList& UNet::GetOutputPersonalLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, int sublevel)
{
 ULink link;
 ULinkSide item;
 ULinkSide connector;

 GetLongName(netlevel,item.ComponentName);
 if(item.ComponentName.empty())
  return linkslist;
 link.Item=item;

 std::vector<UEPtr<UIPropertyOutput> > output_properties;
 this->FindPropertiesByType<UIPropertyOutput>(output_properties);
 bool res(true);
 for(size_t i=0;i<output_properties.size();i++)
 {
  link.Connector.clear();
  const std::list<UEPtr<UIPropertyInput> >& connected_properties=output_properties[i]->GetConnectedProperties();
  std::list<UEPtr<UIPropertyInput> >::const_iterator I=connected_properties.begin();
  for(;I != connected_properties.end();I++)
  {
   const UEPtr<UIPropertyInput> input_property=*I;
   UNet* curr_conn(input_property->GetOwner());
   if(curr_conn != cont)
	continue;
   curr_conn->GetLongName(netlevel,connector.ComponentName);
   if(!connector.ComponentName.empty())
   {
	  link.Item.Index=-1;
	  link.Item.PropertyName=output_properties[i]->GetName();
	  connector.Index=-1;
	  connector.PropertyName=input_property->GetName();

	  link.Connector.push_back(connector);
	  linkslist.Set(link);
   }
  }

 }
 return linkslist;
}

ULinksList& UNet::GetInputPersonalLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, int sublevel)
{
 ULink link;
 ULinkSide connector;
 ULinkSide item;
 GetLongName(netlevel,connector.ComponentName);
 if(connector.ComponentName.empty())
  return linkslist;

 std::vector<UEPtr<UIPropertyInput> > input_properties;
 FindPropertiesByType<UIPropertyInput>(input_properties);

 for(size_t j=0;j<input_properties.size();j++)
 {
  const std::list<UEPtr<UIPropertyOutput> >& connected_properties=input_properties[j]->GetConnectedProperties();
  std::list<UEPtr<UIPropertyOutput> >::const_iterator I=connected_properties.begin();
  for(;I != connected_properties.end();I++)
  {
   const UIPropertyOutput* output_property=*I;
   if(output_property->GetOwner() != cont)
    continue;

	output_property->GetOwner()->GetLongName(netlevel,item.ComponentName);
    connector.Index=-1;
	connector.PropertyName=input_properties[j]->GetName();

	item.Index=-1;
	item.PropertyName=output_property->GetName();
	if(!connector.ComponentName.empty())
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
// --------------------------


/// Ищет свойство-выход по заданному индексу
UIPropertyOutput* UNet::FindOutputProperty(const NameT &property_name)
{
 return FindPropertyByType<UIPropertyOutput>(property_name);
}

/// Ищет свойство-вход по заданному индексу
UIPropertyInput* UNet::FindInputProperty(const NameT &property_name)
{
 return FindPropertyByType<UIPropertyInput>(property_name);
}

/// Возвращает число входов
int UNet::GetNumInputs(void)
{
 std::vector<UEPtr<UIPropertyInput> > input_properties;
 FindPropertiesByType<UIPropertyInput>(input_properties);
 return int(input_properties.size());
}

/// Возвращает число входов
int UNet::GetNumOutputs(void)
{
 std::vector<UEPtr<UIPropertyOutput> > output_properties;
 FindPropertiesByType<UIPropertyOutput>(output_properties);
 return int(output_properties.size());
}
// ----------------------



// --------------------------
// Методы сериализации компонент
// --------------------------
// Возвращает свойства компонента по идентификатору
bool UNet::GetComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
  if(!serstorage)
   return false;

  const RDK::UContainer::VariableMapT &props=GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
	try
	{
	 GetProperty(I->first,serstorage);
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 ProcessException(exception);
	}

	std::string paramname=I->first;//I->second.Property->GetName();
    if(serstorage->SelectNode(paramname))
    {
     serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
     serstorage->SelectUp();
	}
   }
   ++I;
  }

 return true;
}

// Возвращает выборочные свойства компонента по идентификатору
// Память для buffer должна быть выделена!
bool UNet::GetComponentSelectedProperties(RDK::USerStorageXML *serstorage)
{
  if(!serstorage)
   return false;

 return true;
}

// Возвращает свойства компонента по идентификатору с описаниями
// Память для buffer должна быть выделена!
bool UNet::GetComponentPropertiesEx(RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
  if(!serstorage)
   return false;

  const RDK::UContainer::VariableMapT &props=GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  UEPtr<UContainerDescription> descr=dynamic_pointer_cast<UContainerDescription>(Storage->GetClassDescription(Storage->FindClassName(GetClass())));

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
	try
	{
	 GetProperty(I->first,serstorage);
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 ProcessException(exception);
	}

	std::string paramname=I->first;//I->second.Property->GetName();
	if(serstorage->SelectNode(paramname))
	{
	 serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
	 if(descr)
	 {
	  serstorage->SetNodeAttribute("Header",descr->GetPropertyDescription(paramname).Header);
	 }
	 serstorage->SelectUp();
	}
   }
   ++I;
  }

 return true;
}


// устанавливает свойства компонента по идентификатору
int UNet::SetComponentProperties(RDK::USerStorageXML *serstorage)
{
  if(!serstorage)
   return 1;

  const RDK::UContainer::VariableMapT &props=GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   try
   {
	SetProperty(I->first,serstorage);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	std::string value("<ZeroPtr>");
	if(serstorage)
	{
	 value=serstorage->GetNodeText();
	 if(value.empty())
	  serstorage->SaveFromNode(value);
	}

	LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Error set property '")+GetFullName()+std::string(":")+I->first+std::string("' to ")+value+std::string(". Reason: ")+exception.what());
   }
   catch(UException &exception)
   {
	std::string value("<ZeroPtr>");
	if(serstorage)
	{
	 value=serstorage->GetNodeText();
	 if(value.empty())
	  serstorage->SaveFromNode(value);
	}

	LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Error set property '")+GetFullName()+std::string(":")+I->first+std::string(" to ")+value+std::string(". Reason: ")+exception.what());
   }
   catch(std::exception &exception)
   {
	std::string value("<ZeroPtr>");
	if(serstorage)
	{
	 value=serstorage->GetNodeText();
	 if(value.empty())
	  serstorage->SaveFromNode(value);
	}

	LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Error set property '")+GetFullName()+std::string(":")+I->first+std::string("' to ")+value+std::string(". Reason: ")+exception.what());
   }
   catch(...)
   {
	std::string value("<ZeroPtr>");
	if(serstorage)
	{
	 value=serstorage->GetNodeText();
	 if(value.empty())
	  serstorage->SaveFromNode(value);
	}

	LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Error set property '")+GetFullName()+std::string(":")+I->first+std::string("' to ")+value+std::string(". Reason: Undefined exception"));
   }
   ++I;
  }
 return 0;
}


// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
bool UNet::SaveComponent(RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask)
{
  if(!serstorage)
   return false;

  serstorage->AddNode(GetName());
  serstorage->SetNodeAttribute("Class",/*RDK::sntoa(cont->GetClass())*/Storage->FindClassName(GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(ptParameter));
  if(!GetComponentProperties(serstorage,params_type_mask))
   return false;
  serstorage->SelectUp();

  if(links)
  {
   serstorage->AddNode("Links");
   if(GetComponentInternalLinks(serstorage,0))
    return false;
   serstorage->SelectUp();
  }

  serstorage->AddNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
   if(!dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->SaveComponent(serstorage,false,params_type_mask))
   {
	std::string name;
	LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Sub component not found: ")+GetFullName(name));
//	return false;
   }
  }
  serstorage->SelectUp();

  serstorage->SelectUp();

 return true;
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
bool UNet::LoadComponent(RDK::USerStorageXML *serstorage, bool links)
{
  if(!serstorage)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);

  if(GetClass() != id)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Wrong class id: expected ")+sntoa(GetClass())+std::string(" found ")+sntoa(id));
   return false;
  }

  SetName(serstorage->GetNodeName());

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	try
	{
	 if(SetComponentProperties(serstorage))
	 {
	  std::string name;
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("SetComponentProperties failed: ")+GetFullName(name));
//	  return false;
	 }
	}
	catch(UException &exception)
	{
	 ProcessException(exception);
    }
	serstorage->SelectUp();
   }
  }

  DelAllComponents();

  if(!serstorage->SelectNode("Components"))
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Components section not found"));
   return false;
  }
  UStorage* storage=GetStorage();
  for(int i=0;i<serstorage->GetNumNodes();i++)
  {
   serstorage->SelectNode(i);
   std::string nodename=serstorage->GetNodeName();
   name=serstorage->GetNodeAttribute("Class");
   try
   {
    id=Storage->FindClassId(name);
	UEPtr<UNet> newcont=dynamic_pointer_cast<UNet>(storage->TakeObject(id));
	if(!newcont)
	 continue;
	if(AddComponent(static_pointer_cast<UContainer>(newcont)) == ForbiddenId)
	 continue;

	if(!newcont->LoadComponent(serstorage,false))
	{
	 std::string name;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("LoadComponent failed: ")+newcont->GetFullName(name));
//	 return false;
    }
   }
   catch(UException &exception)
   {
	ProcessException(exception);
   }
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  if(links)
  {
   serstorage->SelectNode("Links");
   if(!SetComponentInternalLinks(serstorage,0))
	return false;
   serstorage->SelectUp();
  }

 return true;
}


// Сохраняет все свойства компонента и его дочерних компонент в xml
bool UNet::SaveComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
  if(!serstorage)
   return false;

  serstorage->AddNode(GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(type_mask));
  if(!GetComponentProperties(serstorage,type_mask))
   return false;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
   try
   {
	if(!dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->SaveComponentProperties(serstorage,type_mask))
	{
	 std::string name;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("SaveComponentProperties failed: ")+GetFullName(name));
//	 return false;
	}
   }
   catch (UException &exception)
   {
	ProcessException(exception);
   }
  }
  serstorage->SelectUp();

  serstorage->SelectUp();

 return true;
}

// Загружает все свойства компонента и его дочерних компонент из xml
bool UNet::LoadComponentProperties(RDK::USerStorageXML *serstorage)
{
  if(!serstorage)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);
  if(GetClass() != id)
  {
   LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Wrong class id: expected ")+sntoa(GetClass())+std::string(" found ")+sntoa(id));
   return false;
  }

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	try
	{
	 if(SetComponentProperties(serstorage))
	 {
	  std::string name;
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("SetComponentProperties failed: ")+GetFullName(name));
//	  return false;
	 }
	}
	catch (UException &exception)
	{
	 ProcessException(exception);
    }
    serstorage->SelectUp();
   }
  }

  serstorage->SelectNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
   if(!serstorage->SelectNode(GetComponentByIndex(i)->GetName()))
	continue;
   std::string nodename=serstorage->GetNodeName();
   try
   {
	if(!dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->LoadComponentProperties(serstorage))
	{
	 std::string name;
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("LoadComponentProperties failed: ")+GetFullName(name));
//	 return false;
	}
   }
   catch (UException &exception)
   {
	ProcessException(exception);
   }
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

 return true;
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
void UNet::SetGlobalComponentPropertyValue(UId classid, const char *paramname, const char *buffer)
{
  if(classid == ForbiddenId)
   return;

  if(GetClass() == classid)
  {
   try
   {
	SetPropertyValue(paramname,buffer);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	ProcessException(exception);
   }
  }

  for(int i=0;i<GetNumComponents();i++)
  {
   dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->SetGlobalComponentPropertyValue(classid, paramname, buffer);
  }
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
void UNet::SetGlobalOwnerComponentPropertyValue(UId classid, UId owner_classid, const char *paramname, const char *buffer)
{
  if(classid == ForbiddenId)
   return;

  if(GetClass() == classid && GetOwner() && GetOwner()->GetClass() == owner_classid)
  {
   try
   {
	SetPropertyValue(paramname,buffer);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	ProcessException(exception);
   }
  }

  for(int i=0;i<GetNumComponents();i++)
  {
   dynamic_pointer_cast<RDK::UNet>(GetComponentByIndex(i))->SetGlobalOwnerComponentPropertyValue(classid, owner_classid, paramname, buffer);
  }
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UNet::GetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
  if(!serstorage)
   return 1;

  ULinksList linkslist;
  if(owner_level)
   GetLinks(linkslist, owner_level);
  else
   GetLinks(linkslist, this);


  *serstorage<<linkslist;
 return 0;
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются до уровня компонента owner_level
// Если owner_level не задан, то имена применяются до уровня текущего компонента
int UNet::SetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
  if(!serstorage)
   return 1;

  ULinksList linkslist;
  *serstorage>>linkslist;

  BreakLinks();
  CreateLinks(linkslist, owner_level);

 return true;
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UNet::GetComponentInputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel)
{
  if(!serstorage)
   return 1;

  ULinksList linkslist;
//  cont->GetInputLinks(linkslist, cont);

  *serstorage<<linkslist;
 return 0;
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UNet::GetComponentOutputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel)
{
  if(!serstorage)
   return 1;

  ULinksList linkslist;
//  cont->GetOutputLinks(linkslist, cont);

  *serstorage<<linkslist;
 return 0;
}

// Возращает все внешние связи c компонентом и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UNet::GetComponentPersonalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
  if(!serstorage)
   return 1;

  ULinksList linkslist;
  if(owner_level)
   GetLinks(linkslist, owner_level, true, this);
  else
   GetLinks(linkslist, dynamic_pointer_cast<UNet>(GetOwner()), true, this);

  *serstorage<<linkslist;
 return 0;
}

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
bool UNet::SaveComponentDrawInfo(RDK::USerStorageXML *serstorage)
{
  if(!serstorage)
   return false;

  serstorage->AddNode(GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(GetClass()));

  serstorage->AddNode("Links");

  ULinksList linkslist;

  for(int i=0;i<GetNumComponents();i++)
  {
   UEPtr<UNet> sub_cont=static_pointer_cast<UNet>(GetComponentByIndex(i));
   sub_cont->GetLinks(linkslist, this, true, sub_cont);
  }
  *serstorage<<linkslist;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<GetNumComponents();i++)
  {
   UEPtr<UNet> sub_cont=static_pointer_cast<UNet>(GetComponentByIndex(i));
   serstorage->AddNode(sub_cont->GetName());
   serstorage->SetNodeAttribute("Class",Storage->FindClassName(sub_cont->GetClass()));
   serstorage->AddNode("Parameters");
   try
   {
	if(!sub_cont->GetComponentProperties(serstorage,ptPubParameter))
	 return false;
   }
   catch (UException &exception)
   {
	ProcessException(exception);
   }
   serstorage->SelectUp();
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  serstorage->SelectUp();

 return true;
}
// --------------------------

// --------------------------
// Методы доступа к свойствам
// --------------------------

			  /*
bool UNet::IsLinkExists(const ULinkSide &item, const ULinkSide &connector)
{
 UEPtr<UItem> pitem;
 if(!CheckLongId(item.Id))
  pitem=this;
 else
  pitem=dynamic_pointer_cast<UItem>(GetComponentL(item.Id,true));

 UEPtr<UConnector> pconnector=0;
 if(!CheckLongId(connector.Id))
  pconnector=this;
 else
  pconnector=dynamic_pointer_cast<UConnector>(GetComponentL(connector.Id,true));

 if(!pitem)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item not found: ")+item.Name);
  return false;
 }

 if(!pconnector)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector not found: ")+connector.Name);
  return false;
 }
  if(!item.Name.empty() || !connector.Name.empty())
  {
   if(pitem->IsLinkExists(pconnector,item.Name, connector.Name, connector.Index))
	return true;
  }
  else
  {
   LogMessageEx(RDK_EX_ERROR, __FUNCTION__, "Check links by index unsupported now!");
//   if(pitem->CheckLink(pconnector,item.Index, connector.Index))
//	return true;
  }

 return false;
}               */
// --------------------------



}
#endif

