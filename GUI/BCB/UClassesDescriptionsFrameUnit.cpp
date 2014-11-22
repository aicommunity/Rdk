//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UClassesDescriptionsFrameUnit.h"
#include "rtv_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UClassesListFrameUnit"
#pragma resource "*.dfm"
TUClassesDescriptionsFrame *UClassesDescriptionsFrame;
//---------------------------------------------------------------------------
__fastcall TUClassesDescriptionsFrame::TUClassesDescriptionsFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 IsDescrModified=false;
}

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
void TUClassesDescriptionsFrame::AAfterReset(void)
{

}

// ���������� ����������
void TUClassesDescriptionsFrame::AUpdateInterface(void)
{
 if(CurrentClassName.Length() == 0)
  return;

 const char * str=Storage_GetClassDescription(AnsiString(CurrentClassName).c_str());
 if(!str)
 {

  return;
 }

 PropertyDescriptionRichEdit->Lines->Clear();
 PropertyDescriptionRichEdit->Lines->Add(str);

 Engine_FreeBufString(str);
}

// ��������� ��������� ���������� � xml
void TUClassesDescriptionsFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
void TUClassesDescriptionsFrame::ALoadParameters(RDK::USerStorageXML &xml)
{

}

// �������� ����� ����� ����������
TUClassesDescriptionsFrame* TUClassesDescriptionsFrame::New(TComponent *owner)
{
 return new TUClassesDescriptionsFrame(owner);
}
// -----------------------------

//---------------------------------------------------------------------------
void __fastcall TUClassesDescriptionsFrame::ClassesListFrameTreeViewClick(TObject *Sender)

{
 String str=ClassesListFrame->GetSelectedName();
// if(IsDescrModified)

 CurrentClassName=str;
 UpdateInterface();
}
//---------------------------------------------------------------------------

