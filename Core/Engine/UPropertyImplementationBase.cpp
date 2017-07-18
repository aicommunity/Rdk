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

/// Разрывает связь со свойством output_property
bool UIPropertyInputBase::Disconnect(UIPropertyOutput *output_property)
{
 if(!output_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property pointer is null"));
  return false;
 }

 return output_property->Disconnect(this);
}

/// Разрывает все связи со свойством
bool UIPropertyInputBase::DisconnectAll(void)
{
 bool res(true);
 std::list<UEPtr<UIPropertyOutput> >::iterator I=ConnectedProperties.begin(),J;

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
 // TODO: тут код физического подключения данных
 ConnectedProperties.push_back(output_property);
 return true;
}

/// Финальные действия по уничтожению связи со свойством output_property
bool UIPropertyInputBase::DisconnectFromOutput(UIPropertyOutput *output_property)
{
 std::list<UEPtr<UIPropertyOutput> >::iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),output_property);

 if(I == ConnectedProperties.end())
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found in ConnectedProperties list: ")+output_property->GetName());
  return false;
 }

 // TODO: тут код физического отключения данных
 ConnectedProperties.erase(I);
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

/// Возвращает указатели на свойства-приемники данных
const std::list<UEPtr<UIPropertyInput> > UIPropertyOutputBase::GetConnectedProperties(void) const
{
 return ConnectedProperties;
}

/// Устанавливает связь этого выхода со входом input_property
bool UIPropertyOutputBase::Connect(UIPropertyInput *input_property)
{
 if(!input_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connected property pointer is null"));
  return false;
 }

 // TODO: тут код связывания

 if(!input_property->ConnectToOutput(this))
 {
  // откатываем действия
  return false;
 }
 ConnectedProperties.push_back(input_property);
 return true;
}

/// Разрывает связь этого выхода со входом input_property
bool UIPropertyOutputBase::Disconnect(UIPropertyInput *input_property)
{
 if(!input_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property pointer is null"));
  return false;
 }

 std::list<UEPtr<UIPropertyInput> >::iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),input_property);

 if(I == ConnectedProperties.end())
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found in ConnectedProperties list: ")+input_property->GetName());
  return false;
 }

 // TODO: тут код физического отключения данных
 ConnectedProperties.erase(I);
 return input_property->DisconnectFromOutput(this);
}

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
bool UIPropertyOutputBase::DisconnectAll(void)
{
 bool res(true);
 std::list<UEPtr<UIPropertyInput> >::iterator I=ConnectedProperties.begin(),J;
 while(I != ConnectedProperties.end())
 {
  J=I; ++J;
  res &= Disconnect(*I);
  I=J;
 }
 return res;
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



}

#endif

