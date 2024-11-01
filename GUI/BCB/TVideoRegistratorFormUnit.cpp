//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoRegistratorFormUnit.h"
#include "VideoOutputToolsFormUnit.h"
//#include "TVideoGrabberControlFormUnit.h"
#include "UGEngineControlFormUnit.h"
#include "UShowProgressBarUnit.h"
//#include "USharedMemoryLoader.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFormUnit"
#pragma resource "*.dfm"
TVideoRegistratorForm *VideoRegistratorForm;
//---------------------------------------------------------------------------
__fastcall TVideoRegistratorForm::TVideoRegistratorForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

__fastcall TVideoRegistratorForm::~TVideoRegistratorForm(void)
{

}


// �����, ���������� ����� ����� �������
void TVideoRegistratorForm::ABeforeCalculate(void)
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
void TVideoRegistratorForm::AUpdateInterface(void)
{
 for(int i=0;i<GetNumSources();i++)
 {
  Sources[i]->UpdateInterface();
 }
}

// ������� ���������� � �������� ���������
void TVideoRegistratorForm::AClearInterface(void)
{
 ClearSources();
}

// ��������� ��������� ���������� � xml
void TVideoRegistratorForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("NumSources",GetNumSources());
}

// ��������� ��������� ���������� �� xml
void TVideoRegistratorForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 int num=xml.ReadInteger("NumSources",1);
 ClearSources();
 for(int i=0;i<num;i++)
  AddSource();
 UpdateInterface();
}

// �������� ����� ����� ����������
TVideoRegistratorForm* TVideoRegistratorForm::New(TComponent *owner)
{
 return new TVideoRegistratorForm(owner);
}

// ����� ���������� �����
int TVideoRegistratorForm::GetNumSources(void) const
{
 return PageControl->PageCount;
}

// ��������� ����� �������� �����
void TVideoRegistratorForm::AddSource(void)
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
void TVideoRegistratorForm::DelSource(int index)
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
void TVideoRegistratorForm::ClearSources(void)
{
 for(size_t i=0;i<Sources.size();i++)
 {
  delete Sources[i];
  delete PageControl->Pages[0];
 }

 Sources.clear();
}

// �������� ����� ��������� ���������
void TVideoRegistratorForm::SetSourceType(int index, int mode)
{
 if(index<0 || index >=int(Sources.size()))
  return;

 //Sources[index]->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(mode);
}

// ���������� ������ �������� ��������� ��������� �����
int TVideoRegistratorForm::GetActiveSource(void) const
{
 return PageControl->ActivePageIndex;
}

// ���������� ����� ��������� �����
TTVideoRegistratorFrame* TVideoRegistratorForm::GetVideoOutputFrame(int index)
{
 if(index<0 || index >=int(Sources.size()))
  return 0;

 return Sources[index];
}

// ���������� ����� ��������� (����������) ��������� �����
TTVideoRegistratorFrame* TVideoRegistratorForm::GetActiveVideoOutputFrame(void)
{
 if(GetActiveSource()>=0)
  return Sources[GetActiveSource()];

 return 0;
}


// ��������� ���������� �� ���������� ������ � �������� ini ����
void TVideoRegistratorForm::SaveToIni(TMemIniFile *ini, const String &section)
{
// ini->WriteInteger(section,"NumSources",GetNumSources());
 for(int i=0;i<GetNumSources();i++)
 {
  //Sources[i]->MyVideoGrabberControlForm->VideoGrabberControlFrame->SaveToIni(ini,section+IntToStr(i));
 }
}

// ��������� ���������� �� ���������� ������ �� ��������� ini �����
void TVideoRegistratorForm::LoadFromIni(TMemIniFile *ini, const String &section)
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
void TVideoRegistratorForm::Start(int index)
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
void TVideoRegistratorForm::Stop(int index)
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
void TVideoRegistratorForm::StopOffline(int index)
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
void __fastcall TVideoRegistratorForm::PageControlChange(TObject *Sender)
{
 for(int i=0;i<PageControl->PageCount;i++)
  if(PageControl->Pages[i]->Visible == false)
  {
   //Sources[i]->MyVideoGrabberControlForm->Hide();
   //Sources[i]->MyVideoOutputToolsForm->Hide();
  }

}
//---------------------------------------------------------------------------

void __fastcall TVideoRegistratorForm::AddSource1Click(TObject *Sender)
{
 AddSource();
}
//---------------------------------------------------------------------------

void __fastcall TVideoRegistratorForm::DelSource1Click(TObject *Sender)
{
 if(GetNumSources()<=0)
  return;

 DelSource(PageControl->ActivePageIndex);
}
//---------------------------------------------------------------------------

void __fastcall TVideoRegistratorForm::ClearSources1Click(TObject *Sender)
{
 ClearSources();
}
//---------------------------------------------------------------------------

void __fastcall TVideoRegistratorForm::AddSource2Click(TObject *Sender)
{
 AddSource1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoRegistratorForm::DelSource2Click(TObject *Sender)
{
 DelSource1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoRegistratorForm::ClearSource1Click(TObject *Sender)
{
 ClearSources1Click(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TVideoRegistratorForm::FormCreate(TObject *Sender)
{
 UGEngineControlForm->SpecialForms["TVideoRegistratorForm"]=this;
}
//---------------------------------------------------------------------------

void __fastcall TVideoRegistratorForm::FormDestroy(TObject *Sender)
{
 UnLoadUSharedMemoryLibrary();
// UGEngineControlForm->SpecialForms.erase("TVideoOutputForm");
}
//---------------------------------------------------------------------------

