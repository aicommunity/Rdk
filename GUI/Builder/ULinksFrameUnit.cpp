//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ULinksFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "NANetFrameUnit"
#pragma link "UANetFrameUnit"
#pragma resource "*.dfm"
TNLinksFrame *NLinksFrame;

void __fastcall TNLinksFrame::Init(NANet *net)
{
 if(!net)
  return;

 Network=net;

 GroupBox1->DoubleBuffered=true;
 GroupBox2->DoubleBuffered=true;
 GroupBox3->DoubleBuffered=true;
 NANetFrameInputs->Net=Network;
 NANetFrameInputs->ShowModifier=2;
 NANetFrameInputs->Mode=1;

 NANetFrameOutputs->Net=Network;
 NANetFrameOutputs->ShowModifier=2;
 NANetFrameOutputs->Mode=2;

 NANetFrameLinks->Net=Network;
 NANetFrameLinks->ShowModifier=2;
 NANetFrameLinks->Mode=4;

 NANetFrameInputs->UpdateInterface();
 NANetFrameOutputs->UpdateInterface();
 NANetFrameLinks->UpdateInterface();
 Width=Width+1;
 Width=Width-1;
}
//---------------------------------------------------------------------------
__fastcall TNLinksFrame::TNLinksFrame(TComponent* Owner)
	: TFrame(Owner)
{
 Network=0;
}
//---------------------------------------------------------------------------
void __fastcall TNLinksFrame::CreateLinkButtonClick(TObject *Sender)
{
 string itemname=NANetFrameOutputs->StringGrid->Cells[2][NANetFrameOutputs->StringGrid->Row].t_str();
 int itemindex=StrToInt(NANetFrameLinks->StringGrid->Cells[1][NANetFrameLinks->StringGrid->Row]);
 string connname=NANetFrameInputs->StringGrid->Cells[2][NANetFrameInputs->StringGrid->Row].t_str();
 int connindex=StrToInt(NANetFrameLinks->StringGrid->Cells[1][NANetFrameLinks->StringGrid->Row]);

 Network->CreateLink(itemname,itemindex,connname,connindex);
 NANetFrameLinks->UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TNLinksFrame::BreakLinkButtonClick(TObject *Sender)
{
 string itemname=NANetFrameLinks->StringGrid->Cells[3][NANetFrameLinks->StringGrid->Row].t_str();
 int itemindex=StrToInt(NANetFrameLinks->StringGrid->Cells[1][NANetFrameLinks->StringGrid->Row]);
 string connname=NANetFrameLinks->StringGrid->Cells[4][NANetFrameLinks->StringGrid->Row].t_str();
 int connindex=StrToInt(NANetFrameLinks->StringGrid->Cells[2][NANetFrameLinks->StringGrid->Row]);

 Network->BreakLink(itemname,itemindex,connname,connindex);
 NANetFrameLinks->UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TNLinksFrame::FrameResize(TObject *Sender)
{
 GroupBox1->Width=ClientWidth/2;
 GroupBox2->Width=ClientWidth/2;
}
//---------------------------------------------------------------------------
