//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VideoOutputFormUnit.h"
#include "VideoOutputToolsFormUnit.h"
//#include "TVideoGrabberControlFormUnit.h"
#include "rdk_initdll.h"
#include "UGEngineControlFormUnit.h"
#include "UShowProgressBarUnit.h"
#include "TVideoCaptureOptionsFormUnit.h"
//#include "USharedMemoryLoader.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFormUnit"
#pragma resource "*.dfm"
TVideoOutputForm *VideoOutputForm;
//---------------------------------------------------------------------------
__fastcall TVideoOutputForm::TVideoOutputForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 MassiveStartChannelsDelay=5000;
 if(!TVideoCaptureThread::GlobalStartUnlockMutex)
  TVideoCaptureThread::GlobalStartUnlockMutex=CreateMutex(NULL,FALSE,NULL);//CreateEvent(0,TRUE,TRUE,0);
}

__fastcall TVideoOutputForm::~TVideoOutputForm(void)
{
 if(TVideoCaptureThread::GlobalStartUnlockMutex)
 {
  CloseHandle(TVideoCaptureThread::GlobalStartUnlockMutex);
  TVideoCaptureThread::GlobalStartUnlockMutex=0;
 }
}


// �����, ���������� ����� ����� �������
void TVideoOutputForm::ABeforeCalculate(void)
{
/* if(!Model_Check())
  return;

 int num_outputs=Model_GetComponentNumOutputs("");
 for(int i=0;i<num_outputs;i++)
 {
  if(!GetVideoOutputFrame(i))
   continue;
  RDK::UBitmap &source=GetVideoOutputFrame(i)->BmpSource;

  if(source.GetByteLength()>0)
  {
   Model_SetComponentBitmapOutput("", i, &source,true);

   //Env_SetInputRes(i, InputEnvImageWidth, InputEnvImageHeight);
   //Env_SetInputImage(i,(unsigned char*)source.GetData(),source.GetWidth(), source.GetHeight(),source.GetColorModel());
  }
 }    */
}

// ��������� ���������
void TVideoOutputForm::AUpdateInterface(void)
{
 for(int i=0;i<GetNumSources();i++)
 {
  Sources[i]->UpdateInterface();
 }
}

// ������� ���������� � �������� ���������
void TVideoOutputForm::AClearInterface(void)
{
 ClearSources();
}

// ��������� ��������� ���������� � xml
void TVideoOutputForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("NumSources",GetNumSources());
 xml.WriteInteger("MassiveStartChannelsDelay",MassiveStartChannelsDelay);
}

// ��������� ��������� ���������� �� xml
void TVideoOutputForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 int num=xml.ReadInteger("NumSources",1);
 MassiveStartChannelsDelay=xml.ReadInteger("MassiveStartChannelsDelay",5000);
 ClearSources();
 for(int i=0;i<num;i++)
  AddSource();
 UpdateInterface();
}

// �������� ����� ����� ����������
TVideoOutputForm* TVideoOutputForm::New(TComponent *owner)
{
 return new TVideoOutputForm(owner);
}

// ����� ���������� �����
int TVideoOutputForm::GetNumSources(void) const
{
 return PageControl->PageCount;
}

// ��������� ����� �������� �����
void TVideoOutputForm::AddSource(void)
{
 TTabSheet *sheet=new TTabSheet(PageControl);
 sheet->PageControl=PageControl;
 sheet->PageIndex=PageControl->PageCount-1;
 Sources.resize(PageControl->PageCount);

 size_t index=Sources.size()-1;

 Sources[index]=new TVideoOutputFrame(0);
 Sources[index]->LicenseString=UGEngineControlForm->VideoGrabberLicenseString;
 Sources[index]->FrameIndex=index;
 Sources[index]->Name=Sources[index]->Name+String("_")+PageControl->PageCount;
// Sources[index]->MyVideoGrabberControlForm->Name=Sources[index]->Name+"_VideoGrabberControlForm";

 InsertComponent(Sources[index]);
 Sources[index]->Parent=sheet;
 Sources[index]->Align=alClient;
 PageControl->Pages[index]->Caption=IntToStr(int(index));

 Sources[index]->InitPrimarySettings();
/* if(index == 0)
  Sources[index]->VideoGrabber->SynchronizationRole=sr_Master;
 else
  Sources[index]->VideoGrabber->SynchronizationRole=sr_Slave;
  */
}

// ������� �������� �����
void TVideoOutputForm::DelSource(int index)
{
 if(index<0 || index >=int(Sources.size()))
  return;

 delete Sources[index];
 Sources.erase(Sources.begin()+index);
 delete PageControl->Pages[index];
// for(int i=0;i<PageControl->PageCount;i++)
//  Sources[i]->Parent=PageControl->Pages[i];
}

// ������� ��� ��������� �����
void TVideoOutputForm::ClearSources(void)
{
 for(size_t i=0;i<Sources.size();i++)
 {
  delete Sources[i];
  delete PageControl->Pages[0];
 }

 Sources.clear();
}

// �������� ����� ��������� ���������
void TVideoOutputForm::SetSourceType(int index, int mode)
{
 if(index<0 || index >=int(Sources.size()))
  return;

 Sources[index]->Init(mode);
}

// ���������� ������ �������� ��������� ��������� �����
int TVideoOutputForm::GetActiveSource(void) const
{
 return PageControl->ActivePageIndex;
}

// ���������� ����� ��������� �����
TVideoOutputFrame* TVideoOutputForm::GetVideoOutputFrame(int index)
{
 if(index<0 || index >=int(Sources.size()))
  return 0;

 return Sources[index];
}

// ���������� ����� ��������� (����������) ��������� �����
TVideoOutputFrame* TVideoOutputForm::GetActiveVideoOutputFrame(void)
{
 if(GetActiveSource()>=0)
  return Sources[GetActiveSource()];

 return 0;
}


// ��������� ���������� �� ���������� ������ � �������� ini ����
void TVideoOutputForm::SaveToIni(TMemIniFile *ini, const String &section)
{
// ini->WriteInteger(section,"NumSources",GetNumSources());
 for(int i=0;i<GetNumSources();i++)
 {
  //Sources[i]->MyVideoGrabberControlForm->VideoGrabberControlFrame->SaveToIni(ini,section+IntToStr(i));
 }
}

// ��������� ���������� �� ���������� ������ �� ��������� ini �����
void TVideoOutputForm::LoadFromIni(TMemIniFile *ini, const String &section)
{
// int numsources=ini->ReadInteger(section,"NumSources",0);
// ClearSources();
 for(int i=0;i<GetNumSources();i++)
 {
//  AddSource();
 // Sources[i]->MyVideoGrabberControlForm->VideoGrabberControlFrame->LoadFromIni(ini,section+IntToStr(i));
 }

}

// ��������� ��������� �������� �����, ��� ��� ���� index == -1
void TVideoOutputForm::Start(int index)
{
 UShowProgressBarForm->SetBarHeader(1,"Starting video sources...");
 UShowProgressBarForm->ResetBarStatus(1, 1, GetNumSources());

 if(index>=0 && index<GetNumSources())
  Sources[index]->Start();
 else
 {
//  if(GetNumSources()>0)
//   Sources[i]->
  double curr_time=TDateTime::CurrentDateTime().operator double();
  for(int i=0;i<GetNumSources();i++)
  {
   Sources[i]->Start(curr_time+double(i*MassiveStartChannelsDelay)/(86400*1000.0));
//   Sources[i]->Start(curr_time);
   UShowProgressBarForm->IncBarStatus(1);
   UShowProgressBarForm->Update();
  }
 }
}

// ������������� ��������� �������� �����, ��� ��� ���� index == -1
void TVideoOutputForm::Stop(int index)
{
 UShowProgressBarForm->SetBarHeader(1,"Stopping video sources...");
 UShowProgressBarForm->ResetBarStatus(1, 1, GetNumSources());

 if(index>=0 && index<GetNumSources())
  Sources[index]->Pause(0);
 else
 {
  double curr_time=TDateTime::CurrentDateTime().operator double();
  for(int i=0;i<GetNumSources();i++)
  {
   Sources[i]->Pause(curr_time);
   UShowProgressBarForm->IncBarStatus(1);
   UShowProgressBarForm->Update();
  }
 }
}

// ������������� ��������� �������� �����, ��� ��� ���� index == -1
// �� ��������� ��������� ������-���������� (������)
void TVideoOutputForm::StopOffline(int index)
{
 UShowProgressBarForm->SetBarHeader(1,"Stopping video sources...");
 UShowProgressBarForm->ResetBarStatus(1, 1, GetNumSources());

 if(index>=0 && index<GetNumSources())
 {
  if(Sources[index]->Mode != 2 && Sources[index]->Mode != 3)
   Sources[index]->Pause();
 }
 else
  for(int i=0;i<GetNumSources();i++)
  {
   if(Sources[i]->Mode != 2 && Sources[i]->Mode != 3)
	Sources[i]->Pause();

   UShowProgressBarForm->IncBarStatus(1);
   UShowProgressBarForm->Update();
  }
}


//---------------------------------------------------------------------------
void __fastcall TVideoOutputForm::PageControlChange(TObject *Sender)
{
 for(int i=0;i<PageControl->PageCount;i++)
  if(PageControl->Pages[i]->Visible == false)
  {
//   Sources[i]->MyVideoGrabberControlForm->Hide();
   Sources[i]->VideoCaptureOptionsForm->Hide();
   Sources[i]->MyVideoOutputToolsForm->Hide();
  }

}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::AddSource1Click(TObject *Sender)
{
 AddSource();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::DelSource1Click(TObject *Sender)
{
 if(GetNumSources()<=0)
  return;

 DelSource(PageControl->ActivePageIndex);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::ClearSources1Click(TObject *Sender)
{
 ClearSources();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::AddSource2Click(TObject *Sender)
{
 AddSource1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::DelSource2Click(TObject *Sender)
{
 DelSource1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::ClearSource1Click(TObject *Sender)
{
 ClearSources1Click(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputForm::FormCreate(TObject *Sender)
{
 UGEngineControlForm->SpecialForms["TVideoOutputForm"]=this;
 LoadUSharedMemoryLibrary("USharedMemory.dll");
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::FormDestroy(TObject *Sender)
{
 UnLoadUSharedMemoryLibrary();
// UGEngineControlForm->SpecialForms.erase("TVideoOutputForm");
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::StartAll1Click(TObject *Sender)
{
 Start();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::StopAll1Click(TObject *Sender)
{
 Stop();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::ToolButton4Click(TObject *Sender)
{
 StartAll1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::ToolButton6Click(TObject *Sender)
{
 StopAll1Click(Sender);
}
//---------------------------------------------------------------------------

