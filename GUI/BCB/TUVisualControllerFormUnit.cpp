//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUVisualControllerFormUnit.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUVisualControllerForm *UVisualControllerForm;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Флаг, сообщающий что идет расчет
RDK::UELockVar<bool> TUVisualControllerForm::CalculationModeFlag=false;

extern TUVisualControllerForm *RdkMainForm;

__fastcall TUVisualControllerForm::TUVisualControllerForm(TComponent* Owner)
 : TForm(Owner)
{
 UpdateInterfaceFlag=false;
 AlwaysUpdateFlag=false;
 UpdateInterval=1000;
 PureFormCaption=AnsiString(Caption).c_str();

 CalculationStepUpdatedFlag=0;

 CheckModelFlag=true;

 ComponentControlChannel=0;

 ShowTabbedFlag = true;

 RDK::UIVisualControllerStorage::AddInterface(this);
}

__fastcall TUVisualControllerForm::~TUVisualControllerForm(void)
{
 RDK::UIVisualControllerStorage::DelInterface(this);
}
// --------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после загрузки проекта
void TUVisualControllerForm::AfterLoadProject(void)
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
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("Core-AfterLoadProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-AfterLoadProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerForm::AAfterLoadProject(void)
{

}


// Метод, вызываемый перед сбросом модели
void TUVisualControllerForm::BeforeReset(void)
{
 try
 {
  ResetCalculationStepUpdatedFlag();
  ABeforeReset();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerForm::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void TUVisualControllerForm::AfterReset(void)
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
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-AfterReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerForm::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void TUVisualControllerForm::BeforeCalculate(void)
{
 try
 {
  ABeforeCalculate();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-BeforeCalculate Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerForm::ABeforeCalculate(void)
{
}

// Метод, вызываемый после шага расчета
void TUVisualControllerForm::AfterCalculate(void)
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
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerForm::AAfterCalculate(void)
{
}

// Обновление интерфейса
void TUVisualControllerForm::UpdateInterface(bool force_update)
{
 unsigned long long current_time=0;
 try
 {
 UpdateTime=0;
//  UpdateTime=RDK::GetCurrentStartupTime();
 if(!force_update)
 {
  if((!AlwaysUpdateFlag && (!Visible || (Parent && !Parent->Visible))) || (UpdateInterval<0 && CalculationModeFlag))
  {
//   UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
   return;
  }

  UpdateControlState();
  if(!Showing && !AlwaysUpdateFlag)
  {
//   UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
   return;
  }

  if(UpdateInterval>0 && CalculationModeFlag)
  {
   DWORD curr_time=GetTickCount();
   if(curr_time-LastUpdateTime<DWORD(UpdateInterval))
   {
//	UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
	return;
   }

   if(GetCalculationStepUpdatedFlag() == true)
   {
//	UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
	return;
   }
   else
	SetCalculationStepUpdatedFlag();

   LastUpdateTime=curr_time;
  }
 }

 if(this != RdkMainForm)
 {
  if(ComponentControlName.size()>0)
   Caption=(PureFormCaption+std::string(" [")+ComponentControlName+std::string("]")).c_str();
  else
   Caption=PureFormCaption.c_str();
 }

 if(!Core_IsChannelInit())
 {
//   UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
   return;
 }

 if(CheckModelFlag && !Model_Check())
 {
//   UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
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
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-UpdateInterface Unhandled Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(")").c_str()).c_str());
  throw;
 }
 UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
 UpdateInterfaceFlag=false;
}

void TUVisualControllerForm::AUpdateInterface(void)
{
}


// Возврат интерфейса в исходное состояние
void TUVisualControllerForm::ClearInterface(void)
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
}

void TUVisualControllerForm::AClearInterface(void)
{

}

// Возвращает уникальное имя интерфейса
std::string TUVisualControllerForm::GetName(void)
{
 return AnsiString(Name).c_str();
}

// Возвращает имя класса интерфейса
std::string TUVisualControllerForm::GetClassName(void)
{
 return AnsiString(ClassName()).c_str();
}

// Возвращает интервал обновления интерфейса
long TUVisualControllerForm::GetUpdateInterval(void)
{
 return UpdateInterval;
}

// Задает интервал обновления интерфейса
bool TUVisualControllerForm::SetUpdateInterval(long value)
{
 if(value<0)
  return false;

 UpdateInterval=value;
 return true;
}

// Возвращает флаг разрешения обновления интерфейса даже если он не виден
bool TUVisualControllerForm::GetAlwaysUpdateFlag(void)
{
 return AlwaysUpdateFlag;
}

// Сохраняет параметры интерфейса в xml
void TUVisualControllerForm::SaveParameters(RDK::USerStorageXML &xml)
{
 try
 {
 TTabSheet *tab=dynamic_cast<TTabSheet*>(Parent);
 if(tab)
  xml.SelectNodeForce(AnsiString(tab->PageControl->Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ASaveParameters(xml);
 SaveFormPosition(xml);
 xml.WriteString("ComponentControlName",ComponentControlName);
 xml.WriteInteger("UpdateInterval",UpdateInterval);
 xml.WriteBool("AlwaysUpdateFlag",AlwaysUpdateFlag);
 if(tab)
  xml.SelectUp();

 xml.SelectUp();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-SaveParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("Core-SaveParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-SaveParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }

}

void TUVisualControllerForm::ASaveParameters(RDK::USerStorageXML &xml)
{

}


// Загружает параметры интерфейса из xml
void TUVisualControllerForm::LoadParameters(RDK::USerStorageXML &xml)
{
 try
 {
 TTabSheet *tab=dynamic_cast<TTabSheet*>(Parent);
 if(tab)
  xml.SelectNodeForce(AnsiString(tab->PageControl->Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ComponentControlName=xml.ReadString("ComponentControlName","");
 UpdateInterval=xml.ReadInteger("UpdateInterval",UpdateInterval);
 AlwaysUpdateFlag=xml.ReadBool("AlwaysUpdateFlag",false);
 LoadFormPosition(xml);
 ALoadParameters(xml);

 if(tab)
  xml.SelectUp();
 xml.SelectUp();
 UpdateInterface();
 }
 catch (RDK::UException &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, exception.GetType(), (std::string("Core-LoadParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("Core-LoadParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.what()).c_str());
 }
 catch(Exception &exception)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("GUI-LoadParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerForm::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// Управление длинным именем управляемого компонента
// Длинное имя управляемого компонента модели (опционально)
const std::string& TUVisualControllerForm::GetComponentControlName(void) const
{
 return ComponentControlName;
}

bool TUVisualControllerForm::SetComponentControlName(const std::string& name)
{
 if(ComponentControlName == name)
  return true;

 ComponentControlName=name;
 UpdateInterface();
 return true;
}

const int TUVisualControllerForm::GetComponentControlChannel(void) const
{
 return ComponentControlChannel;
}
bool TUVisualControllerForm::SetComponentControlChannel(const int index)
{
 ComponentControlChannel = index;
 return true;
}

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
void TUVisualControllerForm::ResetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=false;
}

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
void TUVisualControllerForm::SetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=true;
}

/// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
bool TUVisualControllerForm::GetCalculationStepUpdatedFlag(void)
{
 return CalculationStepUpdatedFlag;
}

/// Возвращает время обновления интерфейса (мс)
unsigned long long TUVisualControllerForm::GetUpdateTime(void)
{
 return UpdateTime;
}

// Вызывается при попытке показать форму нетрадиционным способом, когда не вызывается обычный OnFormShow
// (пример - открытие GUI компонента, когда уже открыт GUI того же компонента, но с другого канала)
void TUVisualControllerForm::ComponentFormShowManually(const std::string& component_name, int ChannelIndex)
{

}

// -----------------------------

// --------------------------
// Вспомогательные функции сериализации
// --------------------------
// Сохраняет данные положения формы в xml
void TUVisualControllerForm::SaveFormPosition(RDK::USerStorageXML &xml)
{
 xml.SelectNodeForce("FormPosition");
 xml.WriteInteger("Left",Left);
 xml.WriteInteger("Top",Top);
 xml.WriteInteger("Width",Width);
 xml.WriteInteger("Height",Height);
 xml.WriteBool("Visible",Visible);
 xml.WriteInteger("WindowState",(int)WindowState);
 xml.SelectUp();
}

// Загружает данные положения формы из xml
void TUVisualControllerForm::LoadFormPosition(RDK::USerStorageXML &xml)
{
 xml.SelectNodeForce("FormPosition");
 int value=0;
 value=xml.ReadInteger("Left",Left);
 if(value<Screen->DesktopLeft || value>= Screen->DesktopLeft+Screen->DesktopWidth)
  value=Screen->DesktopLeft;
 Left=value;

 value=xml.ReadInteger("Top",Top);
 if(value<Screen->DesktopTop || value>= Screen->DesktopTop+Screen->DesktopHeight)
  value=Screen->DesktopTop;
 Top=value;

 Width=xml.ReadInteger("Width",Width);
 Height=xml.ReadInteger("Height",Height);

 if(!RdkMainForm || RdkMainForm != this)
 {
  Visible=xml.ReadBool("Visible",Visible);
  WindowState=(TWindowState)xml.ReadInteger("WindowState",(int)WindowState);
 }

 xml.SelectUp();
}
// --------------------------


