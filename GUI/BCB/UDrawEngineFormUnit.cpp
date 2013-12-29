//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsControlFormUnit.h"
#include "UDrawEngineFormUnit.h"
#include "../../Core/Graphics/UDrawEngine.cpp"
#include "TUBitmap.h"
//#include "ULinkSelectionFormUnit.h"
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
 LongLinkFlag=false;
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
 if(!NetXml.GetNumNodes())
 {
  const char *xml=Model_SaveComponentDrawInfo(ComponentName.c_str());
  if(xml)
   NetXml.Load(xml,"");

   // RichEdit1->Text=xml;
  DrawEngine.SetNetXml(NetXml);
 }

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

 FontTypeComboBox->Clear();
 std::vector<std::string> buffer;
 RDK::GlobalFonts.GetFontNames(buffer);
 for(size_t i=0;i<buffer.size();i++)
 {
  FontTypeComboBox->Items->Add(buffer[i].c_str());
  if(buffer[i] == FontType)
   FontTypeComboBox->ItemIndex=i;
 }

 FontSizeComboBox->Clear();
 std::vector<int> size_buffer;
 RDK::GlobalFonts.GetFontSizes(FontType, size_buffer);
 for(size_t i=0;i<size_buffer.size();i++)
 {
  FontSizeComboBox->Items->Add(IntToStr(size_buffer[i]));
  if(size_buffer[i] == FontSize)
  {
   FontSizeComboBox->ItemIndex=i;
  }
 }
 if(FontSizeComboBox->ItemIndex < 0 && FontSizeComboBox->Items->Count>0)
  FontSizeComboBox->ItemIndex=0;

 RectWidthLabeledEdit->Text=IntToStr(DrawEngine.GetRectWidth());
 RectHeightLabeledEdit->Text=IntToStr(DrawEngine.GetRectHeight());
}


// Возврат интерфейса в исходное состояние
void TUDrawEngineForm::AClearInterface(void)
{
 NetXml.Destroy();
}

// Сохраняет параметры интерфейса в xml
void TUDrawEngineForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("FontFileName",FontFileName);
 xml.WriteInteger("CanvasWidth",GraphCanvas.GetWidth());
 xml.WriteInteger("CanvasHeight",GraphCanvas.GetHeight());

 xml.WriteString("FontType",FontType);
 xml.WriteInteger("FontSize",FontSize);
 xml.WriteInteger("RectWidth",DrawEngine.GetRectWidth());
 xml.WriteInteger("RectHeight",DrawEngine.GetRectHeight());
}

// Загружает параметры интерфейса из xml
void TUDrawEngineForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 // Имя компонента, содержимое которого будет отображено
 ComponentName.clear();

 // Xml описание сети
 NetXml.Destroy();

 StartName.clear();
 StopName.clear();

 MoveComponentName.clear();
 BreakLinkComponentName.clear();
 FontFileName.clear();

 FontType=xml.ReadString("FontType","Times New Roman");
 FontSize=xml.ReadInteger("FontSize",15);
 DrawEngine.SetRectWidth(xml.ReadInteger("RectWidth",80));
 DrawEngine.SetRectHeight(xml.ReadInteger("RectHeight",25));

 RDK::UBitmapFont* font=dynamic_cast<RDK::UBitmapFont*>(RDK::GlobalFonts.GetFont(FontType,FontSize));
 if(font)
  Font=*font;
 GraphCanvas.SetRes(xml.ReadInteger("CanvasWidth",640),xml.ReadInteger("CanvasHeight",480));
 SetNet(ComponentName);
}

// Создание копии этого компонента
TUDrawEngineForm* TUDrawEngineForm::New(TComponent *owner)
{
 return new TUDrawEngineForm(owner);
}
// -----------------------------

// -----------------------------
// Методы управления данными
// -----------------------------
void TUDrawEngineForm::SetNet(const std::string &comp_name)
{
 if(ComponentName == comp_name)
  return;

 if(!Model_Check())
  return;

 ComponentName=comp_name;
 NetXml.Destroy();

 if(Visible)
  UpdateInterface(false);
}

void TUDrawEngineForm::ReloadNet(void)
{

 if(!Model_Check())
  return;

 NetXml.Destroy();

 if(Visible)
  UpdateInterface(false);
}


void TUDrawEngineForm::SelectComponent(const std::string &comp_name)
{
 DrawEngine.SelectSingleComponent(comp_name);

 if(Visible)
  UpdateInterface(false);
}


// Сохраняет положение компонента в заданных координатах
void TUDrawEngineForm::SaveComponentPosition(const std::string &name)
{
 if(name == "")
  return;
 const RDK::UGEDescription &descr=DrawEngine.GetDescription(name);
 RDK::USerStorageXML xml;
 xml.Create("Coord");
 std::string buffer;
 RDK::MVector<double,3> pos=(descr.Position-DrawEngine.GetOrigin())/DrawEngine.GetZoomCoeff();
 RDK::operator << (xml,pos);
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
// if(DownShift.Contains(ssShift))
 if(DownShift.Contains(ssRight))
 {
  std::string name=DrawEngine.FindComponent(X,Y);
  TPoint pos=Mouse->CursorPos;
  PopupX=X; PopupY=Y;
  if(!name.empty())
   PopupMenu->Popup(pos.X,pos.Y);
  return;
 }

 if(!LongLinkFlag)
 {
  MoveComponentName=DrawEngine.FindComponent(X,Y);
  if(MoveComponentName != "")
  {
   StartX=X; StartY=Y;
  }
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
 /*
 if(LongLinkFlag)//DownShift.Contains(ssRight) && !StartName.empty())
 {
  ShowCanvas>>Image->Picture->Bitmap;
  Image->Canvas->Pen->Color=clGreen;
  Image->Canvas->Pen->Width=1;
  Image->Canvas->Pen->Style=psDot;
  Image->Canvas->PenPos=TPoint(StartX,StartY);
  Image->Canvas->LineTo(StopX,StopY);
 }
*/
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
/* else
 if(DownShift.Contains(ssRight) && !DownShift.Contains(ssDouble))
 {
  if(!LongLinkFlag)
  {
   PopupX=X; PopupY=Y;
   Finishlonglink1Click(Sender);
  }
 }*/

 StartX=StartY=StopX=StopY=-1;
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

 std::string classname=AnsiString(UClassesListFrame->GetSelectedName()).c_str();
 const char* pname=Model_AddComponent(ComponentName.c_str(), classname.c_str());
 if(pname)
 {
  std::string name=pname;
  UpdateInterfaceFlag=true;
  ReloadNet();
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
 std::string name=DrawEngine.FindComponent(PopupX,PopupY);
 PopupX=-1; PopupY=-1;
  if(name.empty())
   return;
  if(!ComponentName.empty())
  {
   name=ComponentName+std::string(".")+name;
  }

  UComponentLinksForm->UComponentLinksFrame->Init(2, name,ComponentName);
  if(UComponentLinksForm->ShowModal() == mrOk)
  {
   UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
//   SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
   ReloadNet();
   UpdateInterface();
  }
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::ApplyButtonClick(TObject *Sender)
{
 FontType=AnsiString(FontTypeComboBox->Text).c_str();
 FontSize=StrToInt(FontSizeComboBox->Text);

 RDK::UBitmapFont* font=dynamic_cast<RDK::UBitmapFont*>(RDK::GlobalFonts.GetFont(FontType,FontSize));
 if(font)
  Font=*font;

 DrawEngine.SetRectWidth(StrToInt(RectWidthLabeledEdit->Text));
 DrawEngine.SetRectHeight(StrToInt(RectHeightLabeledEdit->Text));

 DrawEngine.UpdateAllElementsSize();

 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::RestoreButtonClick(TObject *Sender)
{
 for(int i=0;i<FontTypeComboBox->Items->Count;i++)
 {
  if(Font.GetName() == AnsiString(FontSizeComboBox->Items->Strings[i]).c_str())
   FontTypeComboBox->ItemIndex=i;
 }

 for(int i=0;i<FontSizeComboBox->Items->Count;i++)
 {
  if(Font.GetHeight() == StrToInt(FontSizeComboBox->Items->Strings[i]))
   FontTypeComboBox->ItemIndex=i;
 }

 RectWidthLabeledEdit->Text=IntToStr(DrawEngine.GetRectWidth());
 RectHeightLabeledEdit->Text=IntToStr(DrawEngine.GetRectHeight());
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::FontTypeComboBoxSelect(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 FontType=AnsiString(FontTypeComboBox->Text).c_str();
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::Createlonglink1Click(TObject *Sender)
{
 LongLinkFlag=true;
 Image->Cursor=crCross;
 Createlonglink1->Enabled=false;
 Finishlonglink1->Enabled=true;
 Cancellonglink1->Enabled=true;
 Breakinputlink1->Enabled=false;

 if(ComponentName.empty())
 {
  StartName=DrawEngine.FindComponent(PopupX,PopupY);
 }
 else
 {
  StartName=ComponentName+std::string(".")+DrawEngine.FindComponent(PopupX,PopupY);
 }

 //StartX=PopupX; StartY=PopupY;
 PopupX=-1;
 PopupY=-1;
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::Finishlonglink1Click(TObject *Sender)
{
 if(ComponentName.empty())
 {
  StopName=DrawEngine.FindComponent(PopupX,PopupY);
 }
 else
 {
  StopName=ComponentName+std::string(".")+DrawEngine.FindComponent(PopupX,PopupY);
 }
 PopupX=-1;
 PopupY=-1;

 if(StartName.empty() || StopName.empty())
  return;

 // Отображаем окно установки связи
/* std::string full_start_name;
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

 UComponentLinksForm->UComponentLinksFrame->Init(1, full_start_name,ComponentName,full_stop_name);
  */
 UComponentLinksForm->UComponentLinksFrame->Init(1, StartName,"",StopName);
 if(UComponentLinksForm->Visible)
  UComponentLinksForm->Hide();
 if(UComponentLinksForm->ShowModal() == mrOk)
 {
  UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
//   SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
  ReloadNet();
  UpdateInterface();
 }

 Cancellonglink1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineForm::Cancellonglink1Click(TObject *Sender)
{
 StartX=StartY=StopX=StopY=-1;
 StartName.clear();
 StopName.clear();

 Createlonglink1->Enabled=true;
 Finishlonglink1->Enabled=false;
 Cancellonglink1->Enabled=false;
 Breakinputlink1->Enabled=true;
 Image->Cursor=crDefault;
 LongLinkFlag=false;
}
//---------------------------------------------------------------------------

