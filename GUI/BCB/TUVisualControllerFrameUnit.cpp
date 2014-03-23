//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUVisualControllerFrameUnit.h"
#include "rdk_cpp_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUVisualControllerFrame *UVisualControllerFrame;
// --------------------------
// ������������ � �����������
// --------------------------
// ����, ���������� ��� ���� ������
bool TUVisualControllerFrame::CalculationModeFlag=false;

__fastcall TUVisualControllerFrame::TUVisualControllerFrame(TComponent* Owner)
 : TFrame(Owner)
{
 UpdateInterfaceFlag=false;
 AlwaysUpdateFlag=false;
 UpdateInterval=1000;
 CalculationStepUpdatedFlag=false;

 RDK::UIVisualControllerStorage::AddInterface(this);
}

__fastcall TUVisualControllerFrame::~TUVisualControllerFrame(void)
{
 RDK::UIVisualControllerStorage::DelInterface(this);
}
// --------------------------

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������� ������
void TUVisualControllerFrame::BeforeReset(void)
{
 try
 {
  ABeforeReset();
 }
 catch (RDK::UException &exception)
 {
  Engine_LogMessage(exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
 }
 catch(Exception &exception)
 {
  Engine_LogMessage(RDK_EX_ERROR, (std::string("GUI-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::ABeforeReset(void)
{

}

// �����, ���������� ����� ������ ������
void TUVisualControllerFrame::AfterReset(void)
{
 try
 {
  LastUpdateTime=0;
  AAfterReset();
 }
 catch (RDK::UException &exception)
 {
  Engine_LogMessage(exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
 }
 catch(Exception &exception)
 {
  Engine_LogMessage(RDK_EX_ERROR, (std::string("GUI-AfterReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::AAfterReset(void)
{

}

// �����, ���������� ����� ����� �������
void TUVisualControllerFrame::BeforeCalculate(void)
{
 try
 {
  ResetCalculationStepUpdatedFlag();
  ABeforeCalculate();
 }
 catch (RDK::UException &exception)
 {
  Engine_LogMessage(exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
 }
 catch(Exception &exception)
 {
  Engine_LogMessage(RDK_EX_ERROR, (std::string("GUI-BeforeCalculate Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::ABeforeCalculate(void)
{
}

// �����, ���������� ����� ���� �������
void TUVisualControllerFrame::AfterCalculate(void)
{
 try
 {
  AAfterCalculate();
 }
 catch (RDK::UException &exception)
 {
  Engine_LogMessage(exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
 }
 catch(Exception &exception)
 {
  Engine_LogMessage(RDK_EX_ERROR, (std::string("GUI-BeforeReset Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::AAfterCalculate(void)
{
}

// ���������� ����������
void TUVisualControllerFrame::UpdateInterface(bool force_update)
{
 try
 {
 if(!force_update)
 {
  UpdateControlState();
  if(!Showing && !AlwaysUpdateFlag)
   return;
  if(!Parent || (!AlwaysUpdateFlag && !Parent->Visible) || (UpdateInterval<0 && CalculationModeFlag))
   return;
  if(UpdateInterval>0 && CalculationModeFlag)
  {
   DWORD curr_time=GetTickCount();
   if(curr_time-LastUpdateTime<DWORD(UpdateInterval))
	return;

   if(GetCalculationStepUpdatedFlag() == true)
	return;
   else
    SetCalculationStepUpdatedFlag();

   LastUpdateTime=curr_time;
  }
 }

 if(!IsEngineInit())
  return;
 if(!Model_Check())
  return;
 UpdateInterfaceFlag=true;
  AUpdateInterface();
 }
 catch (RDK::UException &exception)
 {
  UpdateInterfaceFlag=false;
  Engine_LogMessage(exception.GetType(), (std::string("Core-UpdateInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
 }
 catch(Exception &exception)
 {
  UpdateInterfaceFlag=false;
  Engine_LogMessage(RDK_EX_ERROR, (std::string("GUI-UpdateInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
 catch(...)
 {
  UpdateInterfaceFlag=false;
  throw;
 }
 UpdateInterfaceFlag=false;
}

void TUVisualControllerFrame::AUpdateInterface(void)
{
}

// ������� ���������� � �������� ���������
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
  Engine_LogMessage(exception.GetType(), (std::string("Core-ClearInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
 }
 catch(Exception &exception)
 {
  Engine_LogMessage(RDK_EX_ERROR, (std::string("GUI-ClearInterface Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::AClearInterface(void)
{

}

// ���������� ���������� ��� ����������
std::string TUVisualControllerFrame::GetName(void)
{
 return AnsiString(Name).c_str();
}

// ��������� ��������� ���������� � xml
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
  Engine_LogMessage(exception.GetType(), (std::string("Core-SaveParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
 }
 catch(Exception &exception)
 {
  Engine_LogMessage(RDK_EX_ERROR, (std::string("GUI-SaveParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
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
  Engine_LogMessage(exception.GetType(), (std::string("Core-LoadParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());
 }
 catch(Exception &exception)
 {
  Engine_LogMessage(RDK_EX_ERROR, (std::string("GUI-LoadParameters Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }
}

void TUVisualControllerFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// ���������� ������� ������ ������������ ����������
// ������� ��� ������������ ���������� ������ (�����������)
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

// ��������� ������ ���������� �����������
/// ���������� ���� ��������� ����������� � ���� �������� �����
void TUVisualControllerFrame::ResetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=false;
}

/// ���������� ���� ��������� ����������� � ���� �������� �����
void TUVisualControllerFrame::SetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=true;
}

/// ���������� ��������� ����� ��������� ����������� � ���� �������� �����
bool TUVisualControllerFrame::GetCalculationStepUpdatedFlag(void)
{
 return CalculationStepUpdatedFlag;
}
// -----------------------------


