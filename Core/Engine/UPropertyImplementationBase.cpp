#ifndef UPropertyImplementationBase_CPP
#define UPropertyImplementationBase_CPP

#include "UPropertyImplementationBase.h"
#include "UNet.h"
//#include "UPropertyOutput.cpp"
//#include "UPropertyInput.cpp"

namespace RDK {

//class UIPropertyInputBase: public UIPropertyIO
/// Конструкторы и деструкторы
UIPropertyInputBase::UIPropertyInputBase(void)
{
 SetNumConnectionsLimit(1);
}

UIPropertyInputBase::~UIPropertyInputBase(void)
{
}


/// Возвращает тип свойства входа
  /*
int UIPropertyInputBase::GetInputType(void) const
{
 return InputType;
}          */

/// Возвращает лимит на число подключений ко входу
/// если -1, то нет ограничений
int UIPropertyInputBase::GetNumConnectionsLimit(void) const
{
 return NumConnectionsLimit;
}

/// Возвращает число подключений ко входу
int UIPropertyInputBase::GetNumConnections(void) const
{
 return int(ConnectedProperties.size());
}

/// Возвращает указатели на свойства-источники данных
const std::vector<UEPtr<UIPropertyOutput> > UIPropertyInputBase::GetConnectedProperties(void) const
{
 return ConnectedProperties;
}

/// Возвращает указатели на свойства-источники данных
const UEPtr<UIPropertyOutput> UIPropertyInputBase::GetConnectedProperty(int c_index) const
{
 return ConnectedProperties[c_index];
}

UEPtr<UIPropertyOutput> UIPropertyInputBase::GetConnectedProperty(int c_index)
{
 return ConnectedProperties[c_index];
}
/*
/// Возвращает полное имя подключенного компонента
std::string UIPropertyInputBase::GetItemFullName(int c_index) const
{
 if(ConnectedProperties.empty())
  return "";

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
  return "";

 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=ConnectedProperties.begin();
 int i=0;
 for(;I != ConnectedProperties.end();++I,++i)
 {
  if(i == c_index)
   return (*I)->GetName();
 }

 return "";
} */

/// Возвращает true если вход имеет подключение
bool UIPropertyInputBase::IsConnected(void) const
{
 return (ConnectedProperties.empty())?false:true;
}

// Проверяет, существует ли связь с заданным коннектором
bool UIPropertyInputBase::IsConnectedTo(const UIPropertyOutput *output_property) const
{
 std::vector<UEPtr<UIPropertyOutput> >::const_iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),output_property);
 return I == ConnectedProperties.end();
}

/// Разрывает связь со свойством output_property
bool UIPropertyInputBase::Disconnect(UIPropertyOutput *output_property, int c_index)
{
 if(!output_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property pointer is null"));
  return false;
 }

 return output_property->Disconnect(this,c_index);
}

/// Разрывает связь по индексу с_index
bool UIPropertyInputBase::Disconnect(int c_index)
{
 if(c_index<0 || c_index >=ConnectedProperties.size())
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property index not found: ")+sntoa(c_index));
  return false;
 }
 return ConnectedProperties[c_index]->Disconnect(this,c_index);
}


/// Разрывает все связи со свойством
bool UIPropertyInputBase::DisconnectAll(void)
{
 bool res(true);
 std::vector<UEPtr<UIPropertyOutput> >::iterator I=ConnectedProperties.begin(),J;

 while(I != ConnectedProperties.end())
 {
  J=I; ++J;
  res &=(*I)->Disconnect(this);
  I=J;
 }
 return res;
}
/// Финальные действия по связыванию входа со свойством output_property
bool UIPropertyInputBase::ConnectToOutput(UIPropertyOutput *output_property)
{

 // TODO: тут проверка, не занят ли вход
 if(NumConnectionsLimit == 0)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connection failed. NumConnectionLimit set to zero."));
  return false;
 }

 if(NumConnectionsLimit>0 && ConnectedProperties.size() == NumConnectionsLimit)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connection failed. Connector already in use."));
  return false;
 }

 if(!CompareLanguageType(*output_property))
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connection failed. Language types are incompatible. ")+output_property->GetLanguageType().name()+std::string(" != ")+GetLanguageType().name());
  return false;
 }

 // TODO: тут код связывания
 // TODO: тут код физического подключения данных
 ConnectedProperties.push_back(output_property);
 return true;
}

/// Финальные действия по уничтожению связи со свойством output_property
bool UIPropertyInputBase::DisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
{
 if(c_index<0 || c_index>=int(ConnectedProperties.size()))
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property index not found: ")+sntoa(c_index));
  return false;
 }

 if(ConnectedProperties[c_index] != output_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found by expected connected index: ")+output_property->GetName()+std::string(" c_index=")+sntoa(c_index));
  return false;
 }
 /*
 std::vector<UEPtr<UIPropertyOutput> >::iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),output_property);

 if(I == ConnectedProperties.end())
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found in ConnectedProperties list: ")+output_property->GetName());
  return false;
 }   */

 // TODO: тут код физического отключения данных
 ConnectedProperties.erase(ConnectedProperties.begin()+c_index);
 return true;
}

/// Задает лимит на число подключений ко входу
/// если -1, то нет ограничений
void UIPropertyInputBase::SetNumConnectionsLimit(int value)
{
 NumConnectionsLimit=value;
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
 SubscribedProperties.clear();
}

// Возвращает число подключенных входов
size_t UIPropertyOutputBase::GetNumSubscribers(void) const
{
 return SubscribedProperties.size();
}

/// Возвращает указатели на свойства-приемники данных
const std::vector<UEPtr<UIPropertyInput> > UIPropertyOutputBase::GetSubscribedProperties(void) const
{
 return SubscribedProperties;
}

/// Возвращает указатели на свойства-источники данных
const UEPtr<UIPropertyInput> UIPropertyOutputBase::GetSubscribedProperty(int c_index) const
{
 return SubscribedProperties[c_index];
}

UEPtr<UIPropertyInput> UIPropertyOutputBase::GetSubscribedProperty(int c_index)
{
 return SubscribedProperties[c_index];
}

/// Устанавливает связь этого выхода со входом input_property
bool UIPropertyOutputBase::Connect(UIPropertyInput *input_property)
{
 if(!input_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connection failed. Connected property pointer is null"));
  return false;
 }

 if(!input_property->ConnectToOutput(this))
 {
  // откатываем действия
  return false;
 }
 SubscribedProperties.push_back(input_property);
 return true;
}

/// Разрывает связь этого выхода со входом input_property
/// Если c_index == -1 то отключает все вхождения этого выхода
bool UIPropertyOutputBase::Disconnect(UIPropertyInput *input_property, int c_index)
{
 if(!input_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property pointer is null"));
  return false;
 }

 bool res(true);
 size_t found_counter(0);

 size_t i(0);
 while(i<SubscribedProperties.size())
 {
  if(SubscribedProperties[i] == input_property)
  {
   if(c_index == -1 || i == c_index)
   {
	// TODO: тут код физического отключения данных
	SubscribedProperties.erase(SubscribedProperties.begin()+i);
	res&=input_property->DisconnectFromOutput(this,i);
    ++found_counter;
   }
   else
   {
    ++i;
   }
  }
  else
   ++i;
 }

 if(found_counter == 0)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found in ConnectedProperties list: ")+input_property->GetName());
  return false;
 }
 return res;
}

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
bool UIPropertyOutputBase::DisconnectAll(void)
{
 bool res(true);
 std::vector<UEPtr<UIPropertyInput> >::iterator I=SubscribedProperties.begin(),J;
 while(I != SubscribedProperties.end())
 {
  J=I; ++J;
  res &= Disconnect(*I);
  I=J;
 }
 return res;
}

/// Возвращает true если выход подключен к выбранному входу
bool UIPropertyOutputBase::IsConnectedToInput(UIPropertyInput *input_property)
{
 std::vector<UEPtr<UIPropertyInput> >::iterator I=find(SubscribedProperties.begin(),SubscribedProperties.end(),input_property);
 if(I == SubscribedProperties.end())
  return false;

 return true;
}

/// Возвращает true если выход подключен к одному из входов выбранного компонента
bool UIPropertyOutputBase::IsConnectedToComponent(UNet *component)
{
 std::vector<UEPtr<UIPropertyInput> >::iterator I=SubscribedProperties.begin();
 for(;I != SubscribedProperties.end();I++)
  if((*I)->GetOwner() == component)
   return true;

 return false;
}



}

#endif

