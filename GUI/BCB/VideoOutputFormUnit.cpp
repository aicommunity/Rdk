//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VideoOutputFormUnit.h"
#include "VideoOutputToolsFormUnit.h"
#include "TVideoGrabberControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoOutputForm *VideoOutputForm;
//---------------------------------------------------------------------------
__fastcall TVideoOutputForm::TVideoOutputForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 UpdateInterfaceFlag=false;
}

// ��������� ���������
void TVideoOutputForm::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 UpdateInterfaceFlag=false;
}

// ����� ���������� �����
int TVideoOutputForm::GetNumSources(void) const
{
 return PageControl->PageCount;
}

// ��������� ����� �������� �����
void TVideoOutputForm::AddSource(void)
{
// PageControl->PageCount=PageControl->PageCount+1;

 TTabSheet *sheet=new TTabSheet(PageControl);
 sheet->PageControl=PageControl;
 sheet->PageIndex=PageControl->PageCount-1;
 Sources.resize(PageControl->PageCount);

 size_t index=Sources.size()-1;
 Sources[index]=new TVideoOutputFrame(sheet);
 Sources[index]->Parent=sheet;
 Sources[index]->Align=alClient;
 PageControl->Pages[index]->Caption=IntToStr(int(index));
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

 Sources[index]->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(mode);
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
 if(GetActiveSource()>0)
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
 if(index>=0 && index<GetNumSources())
  Sources[index]->StartButtonClick(this);
 else
  for(int i=0;i<GetNumSources();i++)
   Sources[i]->StartButtonClick(this);
}

// ������������� ��������� �������� �����, ��� ��� ���� index == -1
void TVideoOutputForm::Stop(int index)
{
 if(index>=0 && index<GetNumSources())
  Sources[index]->StartButtonClick(this);
 else
  for(int i=0;i<GetNumSources();i++)
   Sources[i]->StopButtonClick(this);
}
//---------------------------------------------------------------------------





void __fastcall TVideoOutputForm::PageControlChange(TObject *Sender)
{
 for(int i=0;i<PageControl->PageCount;i++)
  if(PageControl->Pages[i]->Visible == false)
  {
   Sources[i]->MyVideoGrabberControlForm->Hide();
   Sources[i]->MyVideoOutputToolsForm->Hide();
  }

}
//---------------------------------------------------------------------------

