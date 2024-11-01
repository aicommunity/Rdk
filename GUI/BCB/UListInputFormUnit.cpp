/* ***********************************************************
   @Copyright Alexander V. Bakhshiev, 2005-2009.
   E-mail:     alexab@ailab.ru
   url:     www.ailab.ru

   This file - part of the RDK


   version 2.0.0 25.06.2009
*********************************************************** */

#include <vcl.h>
#pragma hdrstop

#include "UListInputFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUListInputForm *UListInputForm;
//---------------------------------------------------------------------------
__fastcall TUListInputForm::TUListInputForm(TComponent* Owner)
        : TForm(Owner)
{
 List->ColWidths[0]=List->Width;
 List->DefaultRowHeight=List->Font->Size
                                  * List->Font->PixelsPerInch / 72+5;
 PresentSelect=0;
 MustInput=true;
 Sort=true;
 top=left=width=height=0;
}

//---------------------------------------------------------------------------
// ����� ����� ��������� ����, ������ ���������
// � ��������� �������� ���� �����
void __fastcall TUListInputForm::Init(string caption,vector<string> &listvals,string deftext)
{
 Caption=caption.c_str();

 if(Sort)
  {
   sort(listvals.begin(),listvals.end());
  }

 List->RowCount=1;
 List->Cells[0][0]="";
 for(unsigned int i=0; i<listvals.size();i++)
  {
   List->Cells[0][List->RowCount-1]=listvals[i].c_str();
   List->RowCount=List->RowCount + 1;
  } 
 if(List->RowCount > 1)
  List->RowCount=List->RowCount - 1;

 Edit->Text=deftext.c_str();
}


// ���������� ��������� �� ������
void __fastcall TUListInputForm::ShowError(string cap, string msg)
{
 Panel1->Visible=false;
 List->Visible=false;
 Panel2->Visible=true;
 Panel2->Caption=msg.c_str();
 Button1->Default=true;

 // ��������� ���� �� ���������
 top=Top;
 left=Left;
 width=Width;
 height=Height;
 caption=Caption;

 ClientWidth=Panel1->Width;
 ClientHeight=Panel1->Height;
 Caption=cap.c_str();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TUListInputForm::FormHide(TObject *Sender)
{
 //
}
//---------------------------------------------------------------------------
void __fastcall TUListInputForm::FormShow(TObject *Sender)
{
 if(List->Cells[0][0] == "" && List->RowCount == 1)
  List->Enabled=false;
 else
  List->Enabled=true;
 Edit->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TUListInputForm::BitBtn1Click(TObject *Sender)
{
 string s=AnsiString(Edit->Text).c_str();
 if(s.find_first_not_of(' ') == string::npos || s == "")
  if(MustInput)
   {
    ShowError("Error", "Input string must be not empty.");
    return;
   }

 if(PresentSelect == 0)
  {
   if(Edit->Text == List->Cells[List->Col][List->Row])
    {
     ShowError("Error", "Name alredy exist. Please input another.");
     return;
    }
  }
 else
 if(PresentSelect == 1)
  {
   if(Edit->Text != List->Cells[List->Col][List->Row])
    {
     ShowError("Error", "Please select one item from list!");
     return;
    }
  }

 ModalResult=mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TUListInputForm::ListDblClick(TObject *Sender)
{
 if(List->Row < 0)
  return;
 Edit->Text=List->Cells[List->Col][List->Row];

 if(PresentSelect == 1)
  BitBtn1Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TUListInputForm::EditChange(TObject *Sender)
{
 int i;
 AnsiString temp;
 //   ��� ������ ������ ���������� ����������� ����� ������� ������
 // � ������ 

 for(i=0;i<List->RowCount;i++)
  {
   temp=List->Cells[0][i];
   if(temp.SubString(1,Edit->Text.Length()) == Edit->Text)
    {
     List->Row=i;
     return;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TUListInputForm::EditKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 // ���� ������ ������� Enter
 if(Key == VK_INSERT)
  {
   List->SetFocus();
   Edit->Text=List->Cells[List->Col][List->Row];
   Edit->SetFocus();
  }
 // ���� ������ ������� �����
 if( Key == VK_UP )
  {
   if(List->Row > 0)
    List->Row--;
  }
 else // �����...
 // ���� ������ ������� ����
 if( Key == VK_DOWN )
  {
   if(List->Row < List->RowCount)
    List->Row++;
  }
}
//---------------------------------------------------------------------------
void __fastcall TUListInputForm::FormResize(TObject *Sender)
{
 List->ColWidths[0]=List->Width;
}
//---------------------------------------------------------------------------
void __fastcall TUListInputForm::Button1Click(TObject *Sender)
{
 Top=top;
 Left=left;
 Width=width;
 Height=height;
 Caption=caption;

 Panel2->Visible=false;
 Panel1->Visible=true;
 List->Visible=true;
 BitBtn1->Default=true;
}
//---------------------------------------------------------------------------

void __fastcall TUListInputForm::ListClick(TObject *Sender)
{
 ListDblClick(Sender);
}
//---------------------------------------------------------------------------

