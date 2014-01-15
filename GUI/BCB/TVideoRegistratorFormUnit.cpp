//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoRegistratorFormUnit.h"
#include "VideoOutputToolsFormUnit.h"
#include "TVideoGrabberControlFormUnit.h"
#include "rdk_initdll.h"
#include "UGEngineControlFormUnit.h"
#include "UShowProgressBarUnit.h"
//#include "USharedMemoryLoader.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFormUnit"
#pragma resource "*.dfm"
TTVideoRegistratorForm *TVideoRegistratorForm;
//---------------------------------------------------------------------------
__fastcall TTVideoRegistratorForm::TTVideoRegistratorForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

__fastcall TTVideoRegistratorForm::~TTVideoRegistratorForm(void)
{

}


// �����, ���������� ����� ����� �������
void TTVideoRegistratorForm::ABeforeCalculate(void)
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
void TTVideoRegistratorForm::AUpdateInterface(void)
{
 for(int i=0;i<GetNumSources();i++)
 {
  Sources[i]->UpdateInterface();
 }
}

// ������� ���������� � �������� ���������
void TTVideoRegistratorForm::AClearInterface(void)
{
 ClearSources();
}

// ��������� ��������� ���������� � xml
void TTVideoRegistratorForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("NumSources",GetNumSources());
}

// ��������� ��������� ���������� �� xml
void TTVideoRegistratorForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 int num=xml.ReadInteger("NumSources",1);
 ClearSources();
 for(int i=0;i<num;i++)
  AddSource();
 UpdateInterface();
}

// �������� ����� ����� ����������
TTVideoRegistratorForm* TTVideoRegistratorForm::New(TComponent *owner)
{
 return new TTVideoRegistratorForm(owner);
}

// ����� ���������� �����
int TTVideoRegistratorForm::GetNumSources(void) const
{
 return PageControl->PageCount;
}

// ��������� ����� �������� �����
void TTVideoRegistratorForm::AddSource(void)
{
 TTabSheet *sheet=new TTabSheet(PageControl);
 sheet->PageControl=PageControl;
 sheet->PageIndex=PageControl->PageCount-1;
 Sources.resize(PageControl->PageCount);

 size_t index=Sources.size()-1;

 Sources[index]=new TTVideoRegistratorFrame(0);
 //Sources[index]->LicenseString=UGEngineControlForm->VideoGrabberLicenseString;
 Sources[index]->FrameIndex=index;
 Sources[index]->Name=Sources[index]->Name+String("_")+PageControl->PageCount;
 //Sources[index]->MyVideoGrabberControlForm->Name=Sources[index]->Name+"_VideoGrabberControlForm";

 InsertComponent(Sources[index]);
 Sources[index]->Parent=sheet;
 Sources[index]->Align=alClient;
 PageControl->Pages[index]->Caption=IntToStr(int(index));
/* if(index == 0)
  Sources[index]->VideoGrabber->SynchronizationRole=sr_Master;
 else
  Sources[index]->VideoGrabber->SynchronizationRole=sr_Slave;
  */
}

// ������� �������� �����
void TTVideoRegistratorForm::DelSource(int index)
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
void TTVideoRegistratorForm::ClearSources(void)
{
 for(size_t i=0;i<Sources.size();i++)
 {
  delete Sources[i];
  delete PageControl->Pages[0];
 }

 Sources.clear();
}

// �������� ����� ��������� ���������
void TTVideoRegistratorForm::SetSourceType(int index, int mode)
{
 if(index<0 || index >=int(Sources.size()))
  return;

 //Sources[index]->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(mode);
}

// ���������� ������ �������� ��������� ��������� �����
int TTVideoRegistratorForm::GetActiveSource(void) const
{
 return PageControl->ActivePageIndex;
}

// ���������� ����� ��������� �����
TTVideoRegistratorFrame* TTVideoRegistratorForm::GetVideoOutputFrame(int index)
{
 if(index<0 || index >=int(Sources.size()))
  return 0;

 return Sources[index];
}

// ���������� ����� ��������� (����������) ��������� �����
TTVideoRegistratorFrame* TTVideoRegistratorForm::GetActiveVideoOutputFrame(void)
{
 if(GetActiveSource()>=0)
  return Sources[GetActiveSource()];

 return 0;
}


// ��������� ���������� �� ���������� ������ � �������� ini ����
void TTVideoRegistratorForm::SaveToIni(TMemIniFile *ini, const String &section)
{
// ini->WriteInteger(section,"NumSources",GetNumSources());
 for(int i=0;i<GetNumSources();i++)
 {
  //Sources[i]->MyVideoGrabberControlForm->VideoGrabberControlFrame->SaveToIni(ini,section+IntToStr(i));
 }
}

// ��������� ���������� �� ���������� ������ �� ��������� ini �����
void TTVideoRegistratorForm::LoadFromIni(TMemIniFile *ini, const String &section)
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
void TTVideoRegistratorForm::Start(int index)
{
 UShowProgressBarForm->SetBarHeader(1,"Starting video sources...");
 UShowProgressBarForm->ResetBarStatus(1, 1, GetNumSources());

 if(index>=0 && index<GetNumSources())
  Sources[index]->StartPreviewButtonClick(this);
 else
 {
//  if(GetNumSources()>0)
//   Sources[i]->
  for(int i=0;i<GetNumSources();i++)
  {
   Sources[i]->StartPreviewButtonClick(this);
   UShowProgressBarForm->IncBarStatus(1);
   UShowProgressBarForm->Update();
  }
 }
}

// ������������� ��������� �������� �����, ��� ��� ���� index == -1
void TTVideoRegistratorForm::Stop(int index)
{
 UShowProgressBarForm->SetBarHeader(1,"Stopping video sources...");
 UShowProgressBarForm->ResetBarStatus(1, 1, GetNumSources());

 if(index>=0 && index<GetNumSources())
  Sources[index]->StopButtonClick(this);
 else
  for(int i=0;i<GetNumSources();i++)
  {
   Sources[i]->StopButtonClick(this);
   UShowProgressBarForm->IncBarStatus(1);
   UShowProgressBarForm->Update();
  }
}

// ������������� ��������� �������� �����, ��� ��� ���� index == -1
// �� ��������� ��������� ������-���������� (������)
void TTVideoRegistratorForm::StopOffline(int index)
{
 UShowProgressBarForm->SetBarHeader(1,"Stopping video sources...");
 UShowProgressBarForm->ResetBarStatus(1, 1, GetNumSources());

 if(index>=0 && index<GetNumSources())
 {
  if(Sources[index]->Mode != 2 && Sources[index]->Mode != 3)
   Sources[index]->StopButtonClick(this);
 }
 else
  for(int i=0;i<GetNumSources();i++)
  {
   if(Sources[i]->Mode != 2 && Sources[i]->Mode != 3)
	Sources[i]->StopButtonClick(this);

   UShowProgressBarForm->IncBarStatus(1);
   UShowProgressBarForm->Update();
  }
}


//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorForm::PageControlChange(TObject *Sender)
{
 for(int i=0;i<PageControl->PageCount;i++)
  if(PageControl->Pages[i]->Visible == false)
  {
   //Sources[i]->MyVideoGrabberControlForm->Hide();
   //Sources[i]->MyVideoOutputToolsForm->Hide();
  }

}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorForm::AddSource1Click(TObject *Sender)
{
 AddSource();
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorForm::DelSource1Click(TObject *Sender)
{
 if(GetNumSources()<=0)
  return;

 DelSource(PageControl->ActivePageIndex);
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorForm::ClearSources1Click(TObject *Sender)
{
 ClearSources();
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorForm::AddSource2Click(TObject *Sender)
{
 AddSource1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorForm::DelSource2Click(TObject *Sender)
{
 DelSource1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorForm::ClearSource1Click(TObject *Sender)
{
 ClearSources1Click(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TTVideoRegistratorForm::FormCreate(TObject *Sender)
{
 UGEngineControlForm->SpecialForms["TVideoOutputForm"]=this;
 LoadUSharedMemoryLibrary("USharedMemory.dll");
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorForm::FormDestroy(TObject *Sender)
{
 UnLoadUSharedMemoryLibrary();
// UGEngineControlForm->SpecialForms.erase("TVideoOutputForm");
}
//---------------------------------------------------------------------------

