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
bool UIPropertyInputBase::DisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
{
 if(!output_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property pointer is null"));
  return false;
 }

 return output_property->DisconnectFromInput(this,c_index);
}

/// Разрывает связь по индексу с_index
bool UIPropertyInputBase::DisconnectFromOutput(int c_index)
{
 if(c_index<0 || c_index >=ConnectedProperties.size())
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property index not found: ")+sntoa(c_index));
  return false;
 }
 return ConnectedProperties[c_index]->DisconnectFromInput(this,c_index);
}


/// Разрывает все связи со свойством
bool UIPropertyInputBase::DisconnectAllOutputs(void)
{
 bool res(true);
 std::vector<UEPtr<UIPropertyOutput> >::iterator I=ConnectedProperties.begin(),J;

 while(I != ConnectedProperties.end())
 {
  J=I; ++J;
  res &=(*I)->DisconnectFromInput(this);
  I=J;
 }
 return res;
}
/// Финальные действия по связыванию входа со свойством output_property
bool UIPropertyInputBase::FinalizeConnectToOutput(UIPropertyOutput *output_property)
{
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

 ConnectedProperties.push_back(output_property);
 return AFinalizeConnectToOutput(output_property);
}

/// Финальные действия по уничтожению связи со свойством output_property
bool UIPropertyInputBase::FinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
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

 ConnectedProperties.erase(ConnectedProperties.begin()+c_index);
 return AFinalizeDisconnectToOutput(output_property,c_index);
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
bool UIPropertyOutputBase::ConnectToInput(UIPropertyInput *input_property)
{
 if(!input_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connection failed. Connected property pointer is null"));
  return false;
 }

 if(!input_property->FinalizeConnectToOutput(this))
 {
  return false;
 }
 SubscribedProperties.push_back(input_property);
 return true;
}

/// Разрывает связь этого выхода со входом input_property
/// Если c_index == -1 то отключает все вхождения этого выхода
bool UIPropertyOutputBase::DisconnectFromInput(UIPropertyInput *input_property, int c_index)
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
	res&=input_property->FinalizeDisconnectFromOutput(this,i);
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
bool UIPropertyOutputBase::DisconnectAllInputs(void)
{
 bool res(true);
 std::vector<UEPtr<UIPropertyInput> >::iterator I=SubscribedProperties.begin(),J;
 while(I != SubscribedProperties.end())
 {
  J=I; ++J;
  res &= DisconnectFromInput(*I);
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

