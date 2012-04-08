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
	: TForm(Owner)
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
 if(index<0 || index >=Sources.size())
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
 if(index<0 || index >=Sources.size())
  return;
}

// ���������� ����� ��������� �����
TVideoOutputFrame* TVideoOutputForm::GetVideoOutputFrame(int index)
{
 if(index<0 || index >=Sources.size())
  return 0;

 return Sources[index];
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

