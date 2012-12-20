//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VideoOutputToolsFormUnit.h"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFormUnit"
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
 GeometryGraphics.SetNumGeometries(GeometryGraphics.GetNumGeometries()+1);
 FigureIndex=GeometryGraphics.GetNumGeometries()-1;
 GeometryGraphics.Geometry(FigureIndex)=Figure;
 GeometryGraphics.Description(FigureIndex).PenWidth=3;
 TColor col=ColorBox->Selected;
 GeometryGraphics.Description(FigureIndex).Color.rgb.r=col & 0x000000FF;
 GeometryGraphics.Description(FigureIndex).Color.rgb.g=(col & 0x0000FF00) >> 8;
 GeometryGraphics.Description(FigureIndex).Color.rgb.b=(col & 0x00FF0000) >> 16;
 GeometryGraphics.Description(FigureIndex).Name=string("Figure")+sntoa(FigureIndex+1,3);
 GeometryGraphics.Description(FigureIndex).Description=string("��������� �")+sntoa(FigureIndex+1);
 GeometryGraphics.Description(FigureIndex).TargetPoints=true;
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
 GeometryGraphics.Clear();
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

 GeometryGraphics.Description(GeometryCheckListBox->ItemIndex).Visible=GeometryCheckListBox->Checked[GeometryCheckListBox->ItemIndex];
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::AddPointButtonClick(TObject *Sender)
{
 if(FigureIndex<0)
  AddFigureButtonClick(Sender);
// EditFigureButtonClick(Sender);
 Figure.SetNumVertices(Figure.GetNumVertices()+1);
 PointIndex=Figure.GetNumVertices()-1;
 Figure.VertexName(PointIndex)=string(AnsiString(PointNameEdit->Text).c_str())+sntoa(PointIndex+1,3);
 GeometryGraphics.Geometry(FigureIndex)=Figure;
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
 Figure=GeometryGraphics.Geometry(FigureIndex);
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
 GeometryGraphics.Geometry(FigureIndex).Clear();
 PointIndex=-1;
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------




