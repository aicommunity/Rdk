//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUVisualControllerFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUVisualControllerFrame *UVisualControllerFrame;
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Флаг, сообщающий что идет расчет
RDK::UELockVar<bool> TUVisualControllerFrame::CalculationModeFlag(false);

__fastcall TUVisualControllerFrame::TUVisualControllerFrame(TComponent* Owner)
 : TFrame(Owner)
{
 UpdateInterfaceFlag=false;
 AlwaysUpdateFlag=false;
 UpdateInterval=1000;
 CalculationStepUpdatedFlag=false;
 CheckModelFlag=true;

 RDK::UIVisualControllerStorage::AddInterface(this);
}

__fastcall TUVisualControllerFrame::~TUVisualControllerFrame(void)
{
 RDK::UIVisualControllerStorage::DelInterface(this);
}
// --------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после загрузки проекта
void TUVisualControllerFrame::AfterLoadProject(void)
{
 try
 {
  AAfterLoadProject();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-AfterLoadProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("Core-AfterLoadProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("GUI-AfterLoadProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::AAfterLoadProject(void)
{

}

/// Метод, вызываемый перед закрытием проекта
/// \details Вызывает метод ABeforeCloseProject() в блоке обработки исключений
void TUVisualControllerFrame::BeforeCloseProject(void)
{
 try
 {
     ABeforeCloseProject();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-BeforeCloseProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("Core-BeforeCloseProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("GUI-BeforeCloseProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::ABeforeCloseProject(void)
{

}

// Метод, вызываемый перед сбросом модели
void TUVisualControllerFrame::BeforeReset(void)
{
 try
 {
  ABeforeReset();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("GUI-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void TUVisualControllerFrame::AfterReset(void)
{
 try
 {
  LastUpdateTime=0;
  UpdateTime=0;
  AAfterReset();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("GUI-AfterReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void TUVisualControllerFrame::BeforeCalculate(void)
{
 try
 {
  ResetCalculationStepUpdatedFlag();
  ABeforeCalculate();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("GUI-BeforeCalculate Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::ABeforeCalculate(void)
{
}

// Метод, вызываемый после шага расчета
void TUVisualControllerFrame::AfterCalculate(void)
{
 try
 {
  AAfterCalculate();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("GUI-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::AAfterCalculate(void)
{
}

// Обновление интерфейса
void TUVisualControllerFrame::UpdateInterface(bool force_update)
{
 unsigned long long current_time=0;
 try
 {
//  UpdateTime=RDK::GetCurrentStartupTime();
 if(!force_update)
 {
  UpdateTime=0;
  UpdateControlState();
  if(!Showing && !AlwaysUpdateFlag)
  {
//   UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
   return;
  }
  if(!Parent || (!AlwaysUpdateFlag && !Parent->Visible) || (UpdateInterval<0 && CalculationModeFlag))
  {
//   UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
   return;
  }
  if(UpdateInterval>0 && CalculationModeFlag)
  {
   DWORD curr_time=GetTickCount();
   if(curr_time-LastUpdateTime<DWORD(UpdateInterval))
   {
	return;
   }

   if(GetCalculationStepUpdatedFlag() == true)
   {
	return;
   }
   else
	SetCalculationStepUpdatedFlag();

   LastUpdateTime=curr_time;
  }
 }

 if(!Core_IsChannelInit()) // TODO: PossibleUnsafe!
 {
   return;
 }

 if(CheckModelFlag && !RDK::GetModel())  // TODO: PossibleUnsafe!
 {
   return;
 }
  UpdateInterfaceFlag=true;
  current_time=RDK::GetCurrentStartupTime();
  AUpdateInterface();
 }
 catch (RDK::UException &exception)
 {
  UpdateInterfaceFlag=false;
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-UpdateInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  UpdateInterfaceFlag=false;
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("Core-UpdateInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  UpdateInterfaceFlag=false;
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-UpdateInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
 catch(...)
 {
  UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
  UpdateInterfaceFlag=false;
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-UpdateInterface unhandled exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(")")).c_str());
  throw;
 }
 UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
 UpdateInterfaceFlag=false;
}

void TUVisualControllerFrame::AUpdateInterface(void)
{
}

// Возврат интерфейса в исходное состояние
void TUVisualControllerFrame::ClearInterface(void)
{
 try
 {
  AClearInterface();
  ComponentControlName.clear();
  UpdateInterface(true);
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-ClearInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("Core-ClearInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-ClearInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
 catch(...)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-ClearInterface unhandled exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(")")).c_str());
  throw;
 }

}

void TUVisualControllerFrame::AClearInterface(void)
{
}

// Возвращает уникальное имя интерфейса
std::string TUVisualControllerFrame::GetName(void)
{
 return AnsiString(Name).c_str();
}

// Возвращает полное уникальное имя интерфейса
std::string TUVisualControllerFrame::CalcFullName(void)
{
 std::string full_name;

 if(Parent)
  full_name=AnsiString(Parent->Name+String(".")+Name).c_str();
 else
  full_name=AnsiString(Name).c_str();
 return full_name;
}

// Возвращает имя класса интерфейса
std::string TUVisualControllerFrame::GetClassName(void)
{
 return AnsiString(ClassName()).c_str();
}

// Возвращает интервал обновления интерфейса
long TUVisualControllerFrame::GetUpdateInterval(void)
{
 return UpdateInterval;
}

// Задает интервал обновления интерфейса
bool TUVisualControllerFrame::SetUpdateInterval(long value)
{
 if(value<-1)
  return false;

 UpdateInterval=value;
 return true;
}


// Возвращает флаг разрешения обновления интерфейса даже если он не виден
bool TUVisualControllerFrame::GetAlwaysUpdateFlag(void)
{
 return AlwaysUpdateFlag;
}

// Сохраняет параметры интерфейса в xml
void TUVisualControllerFrame::SaveParameters(RDK::USerStorageXML &xml)
{
 try
 {
  if(!Owner)
   return;
 xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
// xml.WriteString("FrameTypeName",AnsiString(ClassName()).c_str());
 ASaveParameters(xml);
 xml.WriteInteger("UpdateInterval",UpdateInterval);
 xml.WriteString("ComponentControlName",ComponentControlName);
 xml.WriteBool("AlwaysUpdateFlag",AlwaysUpdateFlag);

 xml.SelectUp();
 xml.SelectUp();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-SaveParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("Core-SaveParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("GUI-SaveParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUVisualControllerFrame::LoadParameters(RDK::USerStorageXML &xml)
{
 try
 {
  if(!Owner)
   return;
 xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ComponentControlName=xml.ReadString("ComponentControlName","");
 UpdateInterval=xml.ReadInteger("UpdateInterval",UpdateInterval);
 AlwaysUpdateFlag=xml.ReadBool("AlwaysUpdateFlag",false);
 ALoadParameters(xml);
 xml.SelectUp();
 xml.SelectUp();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-LoadParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("Core-LoadParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_ERROR, (std::string("GUI-LoadParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// Управление длинным именем управляемого компонента
// Длинное имя управляемого компонента модели (опционально)
const std::string& TUVisualControllerFrame::GetComponentControlName(void) const
{
 return ComponentControlName;
}

bool TUVisualControllerFrame::SetComponentControlName(const std::string& name)
{
 if(ComponentControlName == name)
  return true;

 ComponentControlName=name;
 UpdateInterface();
 return true;
}

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
void TUVisualControllerFrame::ResetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=false;
}

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
void TUVisualControllerFrame::SetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=true;
}

/// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
bool TUVisualControllerFrame::GetCalculationStepUpdatedFlag(void)
{
 return CalculationStepUpdatedFlag;
}

/// Возвращает время обновления интерфейса (мс)
unsigned long long TUVisualControllerFrame::GetUpdateTime(void)
{
 return UpdateTime;
}
// -----------------------------


