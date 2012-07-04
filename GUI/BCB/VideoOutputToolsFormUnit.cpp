//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VideoOutputToolsFormUnit.h"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoOutputToolsForm *VideoOutputToolsForm;

using namespace RDK;
//---------------------------------------------------------------------------
__fastcall TVideoOutputToolsForm::TVideoOutputToolsForm(TComponent* Owner,
	TVideoOutputFrame* _MyVideoOutputFrame,
	MGraphics<double,2>& _GeometryGraphics,
//	MGraphics<double,2>& _SampleGeometryGraphics,
	MGeometry<double,2>& _Figure,
	int& _FigureIndex,
//	bool& _FigureFlag,
	int& _PointIndex,
	int& _PointFlag)

	: TUVisualControllerForm(Owner),
	MyVideoOutputFrame(_MyVideoOutputFrame),
	GeometryGraphics(_GeometryGraphics),
//	SampleGeometryGraphics(_SampleGeometryGraphics),
	Figure(_Figure),
	FigureIndex(_FigureIndex),
//	FigureFlag(_FigureFlag),
	PointIndex(_PointIndex),
	PointFlag(_PointFlag)
{

}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::AddFigureButtonClick(TObject *Sender)
{
 Figure.Clear();
 FigureIndex=GeometryGraphics.AddGeometry(Figure);
 GeometryGraphics.GetDescription(FigureIndex).PenWidth=3;
 TColor col=ColorBox->Selected;
 GeometryGraphics.GetDescription(FigureIndex).Color.rgb.r=col & 0x000000FF;
 GeometryGraphics.GetDescription(FigureIndex).Color.rgb.g=(col & 0x0000FF00) >> 8;
 GeometryGraphics.GetDescription(FigureIndex).Color.rgb.b=(col & 0x00FF0000) >> 16;
 GeometryGraphics.GetDescription(FigureIndex).Name=string("Figure")+sntoa(FigureIndex+1,3);
 GeometryGraphics.GetDescription(FigureIndex).Description=string("��������� �")+sntoa(FigureIndex+1);
 MyVideoOutputFrame->UpdateVideo();
 GeometryCheckListBox->ItemIndex=FigureIndex;
// SampleGeometryGraphics.Clear();
 EditFigureButtonClick(Sender);
// AddPointButtonClick(Sender);
 return;
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::DelAllFiguresButtonClick(TObject *Sender)
{
 GeometryGraphics.DelAllGeometry();
// SampleGeometryGraphics.Clear();
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputToolsForm::DelFigureButtonClick(TObject *Sender)
{
 if(GeometryCheckListBox->ItemIndex<0)
  return;

 GeometryGraphics.DelGeometry(GeometryCheckListBox->ItemIndex);
// SampleGeometryGraphics.Clear();
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputToolsForm::GeometryCheckListBoxClick(TObject *Sender)
{
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputToolsForm::GeometryCheckListBoxClickCheck(TObject *Sender)

{
 if(GeometryCheckListBox->ItemIndex<0)
  return;

 GeometryGraphics.GetDescription(GeometryCheckListBox->ItemIndex).Visible=GeometryCheckListBox->Checked[GeometryCheckListBox->ItemIndex];
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::AddPointButtonClick(TObject *Sender)
{
 if(FigureIndex<0)
  AddFigureButtonClick(Sender);
// EditFigureButtonClick(Sender);
 Figure().SetNumVertex(Figure().GetNumVertex()+1);
 PointIndex=Figure().GetNumVertex()-1;
 Figure().UseName(PointIndex)=string(AnsiString(PointNameEdit->Text).c_str())+sntoa(PointIndex+1,3);
 GeometryGraphics.GetGeometry(FigureIndex)=Figure;
 MyVideoOutputFrame->UpdateVideo();
 PointsCheckListBox->ItemIndex=PointIndex;
// SampleGeometryGraphics.Clear();
// EditPointButtonClick(Sender);
 return;
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::EditPointButtonClick(TObject *Sender)
{
// EditFigureButtonClick(Sender);
/* if(PointFlag || PointsCheckListBox->ItemIndex<0)
 {
  PointFlag=0;
  EditPointButton->Caption="������� �����";
  MyVideoOutputFrame->UpdateVideo();
  if(PointsCheckListBox->ItemIndex <PointsCheckListBox->Items->Count-1)
   PointsCheckListBox->ItemIndex=PointsCheckListBox->ItemIndex+1;
  return;
 }  */

 PointFlag=2;
 FigureIndex=GeometryCheckListBox->ItemIndex;
 Figure=GeometryGraphics.GetGeometry(FigureIndex);
 MyVideoOutputFrame->UpdateVideo();
 PointIndex=PointsCheckListBox->ItemIndex;

// EditPointButton->Caption="��������������...";
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::EditFigureButtonClick(TObject *Sender)
{
/* if(FigureFlag)
 {
  EditFigureButton->Caption="������� ������";
  FigureFlag=false;
 }
 else
 {
  EditFigureButton->Caption="��������������";
  FigureFlag=true;
 }*/
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::PointsCheckListBoxClick(TObject *Sender)
{
 EditPointButtonClick(Sender);
// PointIndex=PointsCheckListBox->ItemIndex;
// MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputToolsForm::FormShow(TObject *Sender)
{
 MyVideoOutputFrame->ZoneSelectEnable=true;
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputToolsForm::DelAllPointsButtonClick(TObject *Sender)
{
 GeometryGraphics.GetGeometry(FigureIndex).Clear();
 PointIndex=-1;
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------


