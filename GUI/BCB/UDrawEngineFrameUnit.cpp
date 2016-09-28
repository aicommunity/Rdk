//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

//#include "UComponentsControlFormUnit.h"
#include "UDrawEngineFrameUnit.h"
#include "../../Core/Graphics/UDrawEngine.cpp"
#include "TUBitmap.h"
//#include "ULinkSelectionFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UComponentsListFrameUnit.h"
#include "UComponentsLinksHintFormUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma link "UClassesListFrameUnit"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TUDrawEngineFrame::TUDrawEngineFrame(TComponent* Owner)
    : TUVisualControllerFrame(Owner)
{
 ComponentsListFrame=0;
 Graph.SetCanvas(&GraphCanvas);
 Graph.SetFont(&Font);
 DrawEngine.SetEngine(&Graph);
 DrawEngine.SetFonts(GetCoreLock()->GetFonts());
 UpdateInterval=-1;
 DragDropFlag=false;
 LongLinkFlag=false;
 MoveFlag=false;
 FontSize=12;
 StartX=0;
 StartY=0;
 StopX=0;
 StopY=0;
 PopupX=0;
 PopupY=0;
 LinksHintVisibility=false;
}

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед сбросом модели
void TUDrawEngineFrame::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void TUDrawEngineFrame::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void TUDrawEngineFrame::ABeforeCalculate(void)
{

}

// Метод, вызываемый после шага расчета
void TUDrawEngineFrame::AAfterCalculate(void)
{

}

// Обновление интерфейса
void TUDrawEngineFrame::AUpdateInterface(void)
{
 if(!NetXml.GetNumNodes())
 {
  const char *xml=Model_SaveComponentDrawInfo(ComponentName.c_str());
  if(xml)
   NetXml.Load(xml,"");
  Engine_FreeBufString(xml);

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
 GetCoreLock()->GetFonts().GetFontNames(buffer);
 for(size_t i=0;i<buffer.size();i++)
 {
  FontTypeComboBox->Items->Add(buffer[i].c_str());
  if(buffer[i] == FontType)
   FontTypeComboBox->ItemIndex=i;
 }

 FontSizeComboBox->Clear();
 std::vector<int> size_buffer;
 GetCoreLock()->GetFonts().GetFontSizes(FontType, size_buffer);
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
 UClassesListFrame->UpdateInterface();
}

// Возврат интерфейса в исходное состояние
void TUDrawEngineFrame::AClearInterface(void)
{
 NetXml.Destroy();
 LinksHintVisibility=false;
// BalloonHint1->HideHint();
}

// Сохраняет параметры интерфейса в xml
void TUDrawEngineFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("FontFileName",FontFileName);
 xml.WriteInteger("CanvasWidth",GraphCanvas.GetWidth());
 xml.WriteInteger("CanvasHeight",GraphCanvas.GetHeight());

 xml.WriteString("FontType",FontType);
 xml.WriteInteger("FontSize",FontSize);
 xml.WriteInteger("RectWidth",DrawEngine.GetRectWidth());
 xml.WriteInteger("RectHeight",DrawEngine.GetRectHeight());
 xml.WriteBool("ShowLinksDetail",ShowLinkDetailCheckBox->Checked);
}

// Загружает параметры интерфейса из xml
void TUDrawEngineFrame::ALoadParameters(RDK::USerStorageXML &xml)
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

 FontType=xml.ReadString("FontType","Tahoma");
 FontSize=xml.ReadInteger("FontSize",16);
 DrawEngine.SetRectWidth(xml.ReadInteger("RectWidth",100));
 DrawEngine.SetRectHeight(xml.ReadInteger("RectHeight",25));

 RDK::UBitmapFont* font=dynamic_cast<RDK::UBitmapFont*>(GetCoreLock()->GetFonts().GetFont(FontType,FontSize));
 if(font)
  Font=*font;
 GraphCanvas.SetRes(xml.ReadInteger("CanvasWidth",640),xml.ReadInteger("CanvasHeight",480));
 SetNet(ComponentName);

 ShowLinkDetailCheckBox->Checked=xml.ReadBool("ShowLinksDetail",false);
}
// -----------------------------

// -----------------------------
// Методы управления данными
// -----------------------------
void TUDrawEngineFrame::SetNet(const std::string &comp_name)
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

void TUDrawEngineFrame::ReloadNet(void)
{

 if(!Model_Check())
  return;

 NetXml.Destroy();

 if(Visible)
  UpdateInterface(false);
}

void TUDrawEngineFrame::SelectComponent(const std::string &comp_name)
{
 DrawEngine.SelectSingleComponent(comp_name);

 if(Visible)
  UpdateInterface(false);
}


// Сохраняет положение компонента в заданных координатах
void TUDrawEngineFrame::SaveComponentPosition(const std::string &name)
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
void __fastcall TUDrawEngineFrame::ImageMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
 DownShift=Shift;
// if(DownShift.Contains(ssShift))
 if(DownShift.Contains(ssRight))
 {
  std::string name=DrawEngine.FindComponent(X,Y);
  TPoint pos=Mouse->CursorPos;
  PopupX=X; PopupY=Y;
//  if(!name.empty())
  if((X<10 || Y<10 || X>Image->Width-10 || Y>Image->Height-10) || !name.empty())
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
void __fastcall TUDrawEngineFrame::ImageMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
 if(ShowLinkDetailCheckBox->Checked || Shift.Contains(ssShift))
 {
  std::string name=DrawEngine.FindComponent(X,Y);
  if(name.empty())
  {
   if(UComponentsLinksHintForm->Visible)
   {
	UComponentsLinksHintForm->Hide();
   }
  }
  else
  {
   if(!UComponentsLinksHintForm->Visible)
   {
    TPoint p=Image->ClientToScreen(TPoint(X,Y));
	if(!ComponentName.empty())
	{
	 name=ComponentName+std::string(".")+name;
    }
	UComponentsLinksHintForm->ShowLinks(name,"",p.X,p.Y);
   }
  }
 }
 else
 if(UComponentsLinksHintForm->Visible && !Shift.Contains(ssShift))
  UComponentsLinksHintForm->Hide();

 if(StartX<0 || StartY <0)
  return;

 StopX=X; StopY=Y;
 if(DownShift.Contains(ssLeft))
 {
  DrawEngine.MoveComponent(MoveComponentName, X,Y);
  UpdateInterface();
 }

}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::ImageMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
 if(DownShift.Contains(ssRight) && DownShift.Contains(ssDouble))
 {
  if(ComponentsListFrame)
  {
  ComponentsListFrame->SelectUp();
  SetNet(ComponentsListFrame->GetCurrentComponentName());
  }
 }
 else
 if(DownShift.Contains(ssLeft) && !DownShift.Contains(ssDouble))
 {
  std::string name=DrawEngine.FindComponent(X,Y);

  if(name != "")
  {
   if(ComponentsListFrame)
    ComponentsListFrame->SetSelectedComponentName(name);
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
   if(ComponentsListFrame)
   {
	ComponentsListFrame->SelectComponentByName(name);
    SetNet(ComponentsListFrame->GetCurrentComponentName());
   }
  }
  else
  {
   ImageDragDrop(Sender, UClassesListFrame->StringGrid, X, Y);
  }
 }

 StartX=StartY=StopX=StopY=-1;
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::ScrollBoxResize(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------


void __fastcall TUDrawEngineFrame::UClassesListFrameStringGridMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y)
{
 UClassesListFrame->StringGridMouseMove(Sender, Shift, X, Y);
 if(Shift.Contains(ssLeft))
 {
  UClassesListFrame->StringGrid->BeginDrag(true);
  DragDropFlag=true;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::UClassesListFrameStringGridMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
 UClassesListFrame->StringGrid->DragMode=dmManual;
 DragDropFlag=false;
}
//---------------------------------------------------------------------------


void __fastcall TUDrawEngineFrame::ImageDragDrop(TObject *Sender, TObject *Source,
          int X, int Y)
{
 if(!DragDropFlag)
  return;
 DragDropFlag=false;

 std::string classname=AnsiString(UClassesListFrame->GetSelectedName()).c_str();
 if(classname.empty())
  return;

 const char* pname=Model_AddComponent(ComponentName.c_str(), classname.c_str());
 if(pname)
 {
  std::string name=pname;
  Engine_FreeBufString(pname);
  UpdateInterfaceFlag=true;
  ReloadNet();
  UpdateInterfaceFlag=false;
  DrawEngine.MoveComponent(name, X,Y);
  SaveComponentPosition(name);

  if(ComponentsListFrame)
   ComponentsListFrame->UpdateInterface();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::ImageDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept)
{
 if(Source == UClassesListFrame->StringGrid || Source == UClassesListFrame->LibComponentListStringGrid)
 {
	 Accept = true;
 }

}
//---------------------------------------------------------------------------


void __fastcall TUDrawEngineFrame::Breakinputlink1Click(TObject *Sender)
{
 std::string name=DrawEngine.FindComponent(PopupX,PopupY);
 PopupX=-1; PopupY=-1;
  if(name.empty())
   return;
  if(!ComponentName.empty())
  {
   name=ComponentName+std::string(".")+name;
  }

  UComponentLinksForm->UComponentLinksFrame->Init(2, name,"");
  if(UComponentLinksForm->ShowModal() == mrOk)
  {
   UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
//   SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
   ReloadNet();
   UpdateInterface();
  }
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::ApplyButtonClick(TObject *Sender)
{
 FontType=AnsiString(FontTypeComboBox->Text).c_str();

 try
 {
  FontSize=StrToInt(FontSizeComboBox->Text);
 }
 catch(EConvertError &err)
 {
  UpdateInterface();
  return;
 }

 RDK::UBitmapFont* font=dynamic_cast<RDK::UBitmapFont*>(GetCoreLock()->GetFonts().GetFont(FontType,FontSize));
 if(font)
  Font=*font;

 DrawEngine.SetRectWidth(StrToInt(RectWidthLabeledEdit->Text));
 DrawEngine.SetRectHeight(StrToInt(RectHeightLabeledEdit->Text));

 DrawEngine.UpdateAllElementsSize();

 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::RestoreButtonClick(TObject *Sender)
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

void __fastcall TUDrawEngineFrame::FontTypeComboBoxSelect(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 FontType=AnsiString(FontTypeComboBox->Text).c_str();
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::Createlonglink1Click(TObject *Sender)
{
 StartMoving1->Enabled=false;
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
  std::string found=DrawEngine.FindComponent(PopupX,PopupY);
  if(!found.empty())
   StartName=ComponentName+std::string(".")+found;
  else
   StartName=ComponentName;
 }

 PopupX=-1;
 PopupY=-1;
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::Finishlonglink1Click(TObject *Sender)
{
 if(ComponentName.empty())
 {
  StopName=DrawEngine.FindComponent(PopupX,PopupY);
 }
 else
 {
  std::string found=DrawEngine.FindComponent(PopupX,PopupY);
  if(!found.empty())
   StopName=ComponentName+std::string(".")+found;
  else
   StopName=ComponentName;
 }
 PopupX=-1;
 PopupY=-1;

// if(StartName.empty() || StopName.empty())
//  return;

 UComponentLinksForm->UComponentLinksFrame->Init(1, StartName,"",StopName);
 if(UComponentLinksForm->Visible)
  UComponentLinksForm->Hide();
 if(UComponentLinksForm->ShowModal() == mrOk)
 {
  UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
  ReloadNet();
  UpdateInterface();
 }

 Cancellonglink1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::Cancellonglink1Click(TObject *Sender)
{
 StartX=StartY=StopX=StopY=-1;
 StartName.clear();
 StopName.clear();

 Createlonglink1->Enabled=true;
 Finishlonglink1->Enabled=false;
 Cancellonglink1->Enabled=false;
 Breakinputlink1->Enabled=true;
 StartMoving1->Enabled=true;
 Image->Cursor=crDefault;
 LongLinkFlag=false;
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::FrameContextPopup(TObject *Sender, TPoint &MousePos,
		  bool &Handled)
{
 Handled=true;
}
//---------------------------------------------------------------------------


void __fastcall TUDrawEngineFrame::Rename1Click(TObject *Sender)
{
 if(ComponentsListFrame)
  ComponentsListFrame->Rename1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::Delete1Click(TObject *Sender)
{
 if(ComponentsListFrame)
  ComponentsListFrame->Delete1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::UClassesListFrameTreeViewMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y)
{
 UClassesListFrame->TreeViewMouseMove(Sender, Shift, X, Y);
 if(Shift.Contains(ssLeft))
 {
  UClassesListFrame->StringGrid->BeginDrag(true);
  DragDropFlag=true;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::UClassesListFrameTreeViewMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
 UClassesListFrame->StringGrid->DragMode=dmManual;
 DragDropFlag=false;
}
//---------------------------------------------------------------------------


void __fastcall TUDrawEngineFrame::UClassesListFrameLibComponentListStringGridMouseMove(TObject *Sender,
		  TShiftState Shift, int X, int Y)
{
 UClassesListFrame->LibComponentListStringGridMouseMove(Sender, Shift, X, Y);
 if(Shift.Contains(ssLeft))
 {
  UClassesListFrame->LibComponentListStringGrid->BeginDrag(true);
  DragDropFlag=true;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::UClassesListFrameLibComponentListStringGridMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
 UClassesListFrame->LibComponentListStringGrid->DragMode=dmManual;
 DragDropFlag=false;
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::UClassesListFrameAddClassButtonClick(TObject *Sender)

{
 UClassesListFrame->NewClassName="NewClass1";
 UClassesListFrame->NewComponentName=ComponentsListFrame->GetSelectedComponentName();
 UClassesListFrame->AddClassButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::StartMoving1Click(TObject *Sender)
{
 LongLinkFlag=true;
 Image->Cursor=crCross;
 Createlonglink1->Enabled=false;
 StartMoving1->Enabled=false;
 Finishmoving1->Enabled=true;
 Cancelmoving1->Enabled=true;

 if(ComponentName.empty())
 {
  StartName=DrawEngine.FindComponent(PopupX,PopupY);
 }
 else
 {
  std::string found=DrawEngine.FindComponent(PopupX,PopupY);
  if(!found.empty())
   StartName=ComponentName+std::string(".")+found;
  else
   StartName=ComponentName;
 }

 PopupX=-1;
 PopupY=-1;
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::Finishmoving1Click(TObject *Sender)
{
 if(ComponentName.empty())
 {
  StopName=DrawEngine.FindComponent(PopupX,PopupY);
 }
 else
 {
  std::string found=DrawEngine.FindComponent(PopupX,PopupY);
  if(!found.empty())
   StopName=ComponentName+std::string(".")+found;
  else
   StopName=ComponentName;
 }
 PopupX=-1;
 PopupY=-1;

// if(StartName.empty() || StopName.empty())
//  return;

 if(StartName.empty())
 {
  Application->MessageBox(L"You can't move a model",L"Error",MB_OK);
  Cancelmoving1Click(Sender);
  return;
 }

 if(Model_MoveComponent(StartName.c_str(),StopName.c_str()) != 0)
 {
  Application->MessageBox(L"Component move error",L"Error",MB_OK);
  Cancelmoving1Click(Sender);
  return;
 }

 UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
 ReloadNet();
 UpdateInterface();

 Cancelmoving1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::Cancelmoving1Click(TObject *Sender)
{
 StartX=StartY=StopX=StopY=-1;
 StartName.clear();
 StopName.clear();

 StartMoving1->Enabled=true;
 Finishmoving1->Enabled=false;
 Cancelmoving1->Enabled=false;
 Createlonglink1->Enabled=true;
 Image->Cursor=crDefault;
 LongLinkFlag=false;
}
//---------------------------------------------------------------------------


void __fastcall TUDrawEngineFrame::CopynametoClipboard1Click(TObject *Sender)
{
 if(ComponentsListFrame)
  ComponentsListFrame->CopynametoClipboard1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::CopylongnametoClipboard1Click(TObject *Sender)

{
 if(ComponentsListFrame)
  ComponentsListFrame->CopylongnametoClipboard1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUDrawEngineFrame::CopyclasstoClipboard1Click(TObject *Sender)
{
 if(ComponentsListFrame)
  ComponentsListFrame->CopyclasstoClipboard2Click(Sender);
}
//---------------------------------------------------------------------------



