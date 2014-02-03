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
//	MGeometry<double,2>& _Figure,
	int& _FigureIndex,
//	bool& _FigureFlag,
	int& _PointIndex,
	int& _PointFlag)

	: TUVisualControllerForm(Owner),
	MyVideoOutputFrame(_MyVideoOutputFrame),
	GeometryGraphics(_GeometryGraphics),
//	SampleGeometryGraphics(_SampleGeometryGraphics),
//	Figure(_Figure),
	FigureIndex(_FigureIndex),
//	FigureFlag(_FigureFlag),
	PointIndex(_PointIndex),
	PointFlag(_PointFlag)
{

}

// Создание копии этого компонента
TVideoOutputToolsForm* TVideoOutputToolsForm::New(TComponent *owner)
{
 return 0;
}

//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::AddFigureButtonClick(TObject *Sender)
{
 FigureIndex=MyVideoOutputFrame->AddFigure(ColorBox->Selected);

 GeometryCheckListBox->ItemIndex=FigureIndex;
 MyVideoOutputFrame->UpdateVideo();
 EditFigureButtonClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::DelAllFiguresButtonClick(TObject *Sender)
{
 MyVideoOutputFrame->DelAllFigures();
 FigureIndex=-1;
 PointIndex=-1;
// SampleGeometryGraphics.Clear();
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputToolsForm::DelFigureButtonClick(TObject *Sender)
{
 if(GeometryCheckListBox->ItemIndex<0)
  return;

 MyVideoOutputFrame->DelFigure(GeometryCheckListBox->ItemIndex);
 if(GeometryCheckListBox->ItemIndex>=GeometryCheckListBox->Count-1)
  --FigureIndex;
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputToolsForm::GeometryCheckListBoxClick(TObject *Sender)
{
 FigureIndex=GeometryCheckListBox->ItemIndex;
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
 PointFlag=1;
/*
 if(FigureIndex<0)
  AddFigureButtonClick(Sender);
// EditFigureButtonClick(Sender);
// GeometryGraphics.Geometry(FigureIndex).SetNumVertices(Figure.GetNumVertices()+1);
// PointIndex=GeometryGraphics.Geometry(FigureIndex).GetNumVertices()-1;
 GeometryGraphics.Geometry(FigureIndex).SetNumVertices(GeometryGraphics.Geometry(FigureIndex).GetNumVertices()+1);
 PointIndex=GeometryGraphics.Geometry(FigureIndex).GetNumVertices()-1;

 GeometryGraphics.Geometry(FigureIndex).VertexName(PointIndex)=string(AnsiString(PointNameEdit->Text).c_str())+sntoa(PointIndex+1,3);
// GeometryGraphics.Geometry(FigureIndex)=Figure;
 PointFlag=2;
 MyVideoOutputFrame->UpdateVideo();
 PointsCheckListBox->ItemIndex=PointIndex;
// SampleGeometryGraphics.Clear();
// EditPointButtonClick(Sender);
*/
 return;
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::EditPointButtonClick(TObject *Sender)
{
// EditFigureButtonClick(Sender);
/* if(PointFlag == 2 || PointsCheckListBox->ItemIndex<0)
 {
  PointFlag=0;
  EditPointButton->Caption="Править точку";
  MyVideoOutputFrame->UpdateVideo();
  if(PointsCheckListBox->ItemIndex <PointsCheckListBox->Items->Count-1)
   PointsCheckListBox->ItemIndex=PointsCheckListBox->ItemIndex+1;
  return;
 }*/

 PointFlag=2;
 PointIndex=PointsCheckListBox->ItemIndex;

// FigureIndex=GeometryCheckListBox->ItemIndex;
// if(FigureIndex>=0)
//  Figure=GeometryGraphics.Geometry(FigureIndex);
// MyVideoOutputFrame->UpdateVideo();
// PointIndex=PointsCheckListBox->ItemIndex;

// EditPointButton->Caption="Редактирование...";
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::EditFigureButtonClick(TObject *Sender)
{
/* if(FigureFlag)
 {
  EditFigureButton->Caption="Править фигуру";
  FigureFlag=false;
 }
 else
 {
  EditFigureButton->Caption="Редактирование";
  FigureFlag=true;
 }*/
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::PointsCheckListBoxClick(TObject *Sender)
{
 EditPointButtonClick(Sender);
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
 if(FigureIndex<0)
  return;

 MyVideoOutputFrame->DelAllPoints(FigureIndex);
 PointIndex=-1;
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputToolsForm::DelPointButtonClick(TObject *Sender)
{
 if(FigureIndex<0 || PointIndex<0)
  return;
 MyVideoOutputFrame->DelPoint(FigureIndex, PointIndex);
 if(PointIndex >= int(GeometryGraphics.Geometry(FigureIndex).GetNumVertices()))
  PointIndex=GeometryGraphics.Geometry(FigureIndex).GetNumVertices()-1;
 MyVideoOutputFrame->UpdateVideo();
}
//---------------------------------------------------------------------------

