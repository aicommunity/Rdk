//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "UEngineMonitorFrameUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUEngineMonitorFrame *UEngineMonitorFrame;

//---------------------------------------------------------------------------
__fastcall TUEngineMonitorFrame::TUEngineMonitorFrame(TComponent* Owner)
	: TFrame(Owner)
{
 UpdateInterfaceFlag=false;
 CalculateMode=0;
}

// Управление режимом расчетов
int TUEngineMonitorFrame::GetCalculateMode(void) const
{
 return CalculateMode;
}

void TUEngineMonitorFrame::SetCalculateMode(int value)
{
 CalculateMode=value;
}

// Добавляет обработчик в список
void TUEngineMonitorFrame::AddInterface(RDK::UIVisualController* value)
{
 std::vector<RDK::UIVisualController*>::iterator I=find(InterfaceUpdaters.begin(),InterfaceUpdaters.end(),value);
 if(I == InterfaceUpdaters.end())
  InterfaceUpdaters.push_back(value);
}

// Удаляет обработчик из списка
void TUEngineMonitorFrame::DelInterface(RDK::UIVisualController* value)
{
 std::vector<RDK::UIVisualController*>::iterator I=find(InterfaceUpdaters.begin(),InterfaceUpdaters.end(),value);
 if(I != InterfaceUpdaters.end())
  InterfaceUpdaters.erase(I);
}

void TUEngineMonitorFrame::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;


 StatusBar->SimpleText=String("Model Time=")+FloatToStrF(Model_GetDoubleTime(),ffFixed,3,3)
				+String("; Real Time=")+FloatToStrF(Model_GetDoubleRealTime(),ffFixed,3,3)
				+String("; Model Duration Time=")+FloatToStrF(Model_GetFullStepDuration("")/1000.0,ffFixed,3,3)
				+String("; Model Performance=")+FloatToStrF(Model_GetInstantPerformance(""),ffFixed,3,3);
 StatusBar->Repaint();
 StatusBar->Update();

 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  InterfaceUpdaters[i]->UpdateInterface();
 UpdateInterfaceFlag=false;
}


// Сохраняет информацию в заданный ini файл
void TUEngineMonitorFrame::SaveToIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 ini->EraseSection(section);
 ini->WriteInteger(section,"CalculateMode",GetCalculateMode());
}

// Загружает информацию из заданного ini файла
void TUEngineMonitorFrame::LoadFromIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 SetCalculateMode(ini->ReadInteger(section,"CalculateMode",0));
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Start1Click(TObject *Sender)
{
 Timer->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Pause1Click(TObject *Sender)
{
 Timer->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Reset1Click(TObject *Sender)
{
 Env_Reset(0);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::TimerTimer(TObject *Sender)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  InterfaceUpdaters[i]->BeforeCalculate();

 switch(CalculateMode)
 {
 case 0:
  Env_Calculate(0);
 break;

 case 1:
  Env_RTCalculate();
 break;
 }

 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  InterfaceUpdaters[i]->AfterCalculate();
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Step1Click(TObject *Sender)
{
 TimerTimer(Sender);
}
//---------------------------------------------------------------------------

