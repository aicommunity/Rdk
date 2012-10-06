//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsControlFormUnit.h"
#include "UDrawEngineFormUnit.h"
#include "../../Core/Graphics/UDrawEngine.cpp"
#include "TUBitmap.h"
#include "ULinkSelectionFormUnit.h"
#include "UComponentLinksFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma link "UClassesListFrameUnit"
#pragma resource "*.dfm"
TUDrawEngineForm *UDrawEngineForm;
//---------------------------------------------------------------------------
__fastcall TUDrawEngineForm::TUDrawEngineForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 Graph.SetCanvas(&GraphCanvas);
 Graph.SetFont(&Font);
 DrawEngine.SetEngine(&Graph);
 UpdateInterval=-1;
 DragDropFlag=false;
}

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед сбросом модели
void TUDrawEngineForm::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void TUDrawEngineForm::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void TUDrawEngineForm::ABeforeCalculate(void)
{

}

// Метод, вызываемый после шага расчета
void TUDrawEngineForm::AAfterCalculate(void)
{

}

// Обновление интерфейса
void TUDrawEngineForm::AUpdateInterface(void)
{
 int new_img_width, new_img_height, new_bmp_width, new_bmp_height;
 int vert_sb_size=VertScrollBar->Size;
 int horz_sb_size=HorzScrollBar->Size;
 if(ScrollBox->ClientWidth<GraphCanvas.GetWidth())
 {
  new_bmp_width=new_img_width=GraphCanvas.GetWidth();
 }
 else
 {
  new_bmp_width=new_img_width=ScrollBox->ClientWidth-vert_sb_size;
 }

 if(ScrollBox->ClientHeight<GraphCanvas.GetHeight())
 {
  new_bmp_height=new_img_height=GraphCanvas.GetHeight();
 }
 else
 {
  new_bmp_height=new_img_height=ScrollBox->ClientHeight-horz_sb_size;
 }

 GraphCanvas.SetRes(new_bmp_width,new_bmp_height,RDK::ubmRGB24);
 Image->Width=new_img_width;
 Image->Height=new_img_height;
 Graph.SetCanvas(&GraphCanvas);
 DrawEngine.Draw();
 GraphCanvas.ReflectionX(&ShowCanvas);
 ShowCanvas>>Image->Picture->Bitmap;
 Image->Repaint();
}

// Сохраняет параметры интерфейса в xml
void TUDrawEngineForm::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{
 xml.WriteString("FontFileName",FontFileName);
 xml.WriteInteger("CanvasWidth",GraphCanvas.GetWidth());
 xml.WriteInteger("CanvasHeight",GraphCanvas.GetHeight());
}

// Загружает параметры интерфейса из xml
void TUDrawEngineForm::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
 FontFileName=xml.ReadString("FontFileName","Font/");//"Font_16x8_EnRu.bmp");

 for(int i=0;i<256;i++)
  Font.Load(i,FontFileName+RDK::sntoa(i,4)+".bmp");
// Font.Load(FontFileName);

 GraphCanvas.SetRes(xml.ReadInteger("CanvasWidth",640),xml.ReadInteger("CanvasHeight",480));
 SetNet(ComponentName);
}
// -----------------------------

// -----------------------------
// Методы управления данными
// -----------------------------
void TUDrawEngineForm::SetNet(const std::string &comp_name)
{
 NetXml.Destroy();
 if(!Model_Check())
  return;

 ComponentName=comp_name;
 const char *xml=Model_SaveComponentDrawInfo(ComponentName.c_str());
 if(xml)
  NetXml.Load(xml,"");

// RichEdit1->Text=xml;
 DrawEngine.SetNetXml(NetXml);

 if(Visible)
  UpdateInterface();
}

void TUDrawEngineForm::SelectComponent(const std::string &comp_name)
{
 DrawEngine.SelectSingleComponent(comp_name);

 if(Visible)
  UpdateInterface();
}


// Сохраняет положение компонента в заданных координатах
void TUDrawEngineForm::SaveComponentPosition(const std::string &name)
{
 if(name == "")
  return;
 const RDK::UGEDescription &descr=DrawEngine.GetDescription(name);
 RDK::Serialize::USerStorageXML xml;
 xml.Create("Coord");
 std::string buffer;
 RDK::MVector<double,3> pos=(descr.Position-DrawEngine.GetOrigin())/DrawEngine.GetZoomCoeff();
 RDK::Serialize::operator << (xml,pos);
 xml.Save(buffer);

 if(!ComponentName.empty())
  Model_SetComponentParameterValue((ComponentName+std::string(".")+name).c_str(), "Coord", buffer.c_str());
 else
  Model_SetComponentParameterValue(name.c_str(), "Coord", buffer.c_str());
}

// -----------------------------


//---------------------------------------------------------------------------
void __fastcall TUDrawEngineForm::ImageMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
 DownShift=Shift;
 if(DownShift.Contains(ssShift))
 {
  BreakLinkComponentName=DrawEngine.FindComponent(X,Y);
  TPoint pos=Mouse->CursorPos;
  if(!BreakLinkComponentName.empty())
   PopupMenu->Popup(pos.X,pos.Y);
  return;
 }
 MoveComponentName=StartName=DrawEngine.FindComponent(X,Y);
 if(MoveComponentName != "")
 {
  StartX=X; StartY=Y;
 }
}
//---------------------------------------------------------------------------
void __fastcall TUDrawEngineForm::ImageMouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y)
{
 if(StartX<0 || StartY <0)
  return;

 StopX=X; StopY=Y;
 if(DownShift.Contains(ssLeft))
 {
  DrawEngine.MoveComponent(MoveComponentName, X,Y);
  UpdateInterface();
 }
 else
 if(DownShift.Contains(ssRight) && !StartName.empty())
 {
  ShowCanvas>>Image->Picture->Bitmap;
  Image->Canvas->Pen->Color=clGreen;
  Image->Canvas->Pen->Width=1;
  Image->Canvas->Pen->Style=psDot;
  Image->Canvas->PenPos=TPoint(StartX,StartY);
  Image->Canvas->LineTo(StopX,StopY);
 }

}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::ImageMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
 if(DownShift.Contains(ssRight) && DownShift.Contains(ssDouble))
 {
  UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->SelectUp();
  SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
 }
 else
 if(DownShift.Contains(ssLeft) && !DownShift.Contains(ssDouble))
 {
  std::string name=DrawEngine.FindComponent(X,Y);

  if(name != "")
  {
   UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->SetSelectedComponentName(name);
   SaveComponentPosition(name);
   UpdateInterface();
  }
 }
 else
 if(DownShift.Contains(ssLeft) && DownShift.Contains(ssDouble))
 {
  TPoint pos=Image->ScreenToClient(Mouse->CursorPos);

  std::string name=DrawEngine.FindComponent(pos.X,pos.Y);

  if(name != "")
  {
   UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->SelectComponentByName(name);
   SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
  }
  else
  {
   ImageDragDrop(Sender, UClassesListFrame->StringGrid, X, Y);
  }
 }
 else
 if(DownShift.Contains(ssRight) && !DownShift.Contains(ssDouble))
 {
  StopName=DrawEngine.FindComponent(StopX,StopY);
  if(StartName.empty() || StopName.empty())
   return;

  // Отображаем окно установки связи
  std::string full_start_name;
  std::string full_stop_name;
  if(ComponentName.empty())
  {
   full_start_name=StartName;
   full_stop_name=StopName;
  }
  else
  {
   full_start_name=ComponentName+std::string(".")+StartName;
   full_stop_name=ComponentName+std::string(".")+StopName;
  }
/*
  ULinkSelectionForm->StartName=StartName;
  ULinkSelectionForm->StopName=StopName;
  ULinkSelectionForm->SetMode(0);
  if(ULinkSelectionForm->ShowModal() != mrOk)
   return;
*/
  UComponentLinksForm->UComponentLinksFrame->Init(1, full_start_name,ComponentName,full_stop_name);
/*   NANetFrameInputs->ViewComponentOwnerLongId=ComponentName;
  UComponentLinksForm->UComponentLinksFrame->NANetFrameInputs->ViewComponentLongId=full_stop_name;
  UComponentLinksForm->UComponentLinksFrame->NANetFrameOutputs->ViewComponentOwnerLongId=ComponentName;
  UComponentLinksForm->UComponentLinksFrame->NANetFrameOutputs->ViewComponentLongId=full_start_name;
  UComponentLinksForm->UComponentLinksFrame->NANetFrameLinks->ViewComponentOwnerLongId=ComponentName;
  UComponentLinksForm->UComponentLinksFrame->NANetFrameLinks->ViewComponentLongId=full_start_name;
  UComponentLinksForm->UComponentLinksFrame->NANetFrameLinks->ViewComponentLongId2=full_stop_name;
  UComponentLinksForm->UComponentLinksFrame->SetMode(1); */
  if(UComponentLinksForm->ShowModal() == mrOk)
  {
   UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
   SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
   UpdateInterface();
  }
 }

 StartX=StartY=StopX=StopY=-1;
 StartName.clear();
 StopName.clear();
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::ScrollBoxResize(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------


void __fastcall TUDrawEngineForm::UClassesListFrameStringGridMouseMove(TObject *Sender,
		  TShiftState Shift, int X, int Y)
{
 if(Shift.Contains(ssLeft))
 {
  UClassesListFrame->StringGrid->BeginDrag(true);
  DragDropFlag=true;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::UClassesListFrameStringGridMouseUp(TObject *Sender,
		  TMouseButton Button, TShiftState Shift, int X, int Y)
{
 UClassesListFrame->StringGrid->DragMode=dmManual;
 DragDropFlag=false;
}
//---------------------------------------------------------------------------


void __fastcall TUDrawEngineForm::ImageDragDrop(TObject *Sender, TObject *Source,
		  int X, int Y)
{
 if(!DragDropFlag)
  return;
 DragDropFlag=false;

 int classid=UClassesListFrame->GetSelectedId();
 const char* pname=Model_AddComponent(ComponentName.c_str(), classid);
 if(pname)
 {
  std::string name=pname;
  UpdateInterfaceFlag=true;
  SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
  UpdateInterfaceFlag=false;
  DrawEngine.MoveComponent(name, X,Y);
  SaveComponentPosition(name);

  UComponentsControlForm->ComponentsControlFrame->UpdateInterface();
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::ImageDragOver(TObject *Sender, TObject *Source,
		  int X, int Y, TDragState State, bool &Accept)
{
 if(Source == UClassesListFrame->StringGrid)
 {
	 Accept = true;
	// установка изображения курсора
//	if(State == dsDragLeave)
//	  Source->DragCursor = crDrag;
//	if((State == dsDragEnter)  && UClassesListFrame->StringGrid->Row >= 1))
//	  Source->DragCursor = crMultiDrag;
 }

}
//---------------------------------------------------------------------------


void __fastcall TUDrawEngineForm::Breakinputlink1Click(TObject *Sender)
{
  if(BreakLinkComponentName.empty())
   return;
  if(!ComponentName.empty())
  {
   BreakLinkComponentName=ComponentName+std::string(".")+BreakLinkComponentName;
  }

//  UComponentLinksForm->UComponentLinksFrame->NANetFrameLinks->ViewComponentOwnerLongId=ComponentName;
//  UComponentLinksForm->UComponentLinksFrame->NANetFrameLinks->ViewComponentLongId=BreakLinkComponentName;
//  UComponentLinksForm->UComponentLinksFrame->SetMode(2);
  UComponentLinksForm->UComponentLinksFrame->Init(2, BreakLinkComponentName,ComponentName);
  if(UComponentLinksForm->ShowModal() == mrOk)
  {
   UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
   SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
   UpdateInterface();
  }
}
//---------------------------------------------------------------------------

