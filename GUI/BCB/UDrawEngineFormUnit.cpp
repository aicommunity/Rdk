//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsControlFormUnit.h"
#include "UDrawEngineFormUnit.h"
#include "../../Core/Graphics/UDrawEngine.cpp"
#include "TUBitmap.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUDrawEngineForm *UDrawEngineForm;
//---------------------------------------------------------------------------
__fastcall TUDrawEngineForm::TUDrawEngineForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 Graph.SetCanvas(&GraphCanvas);
 DrawEngine.SetEngine(&Graph);
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

}

// Загружает параметры интерфейса из xml
void TUDrawEngineForm::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{

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
// -----------------------------


//---------------------------------------------------------------------------
void __fastcall TUDrawEngineForm::FormResize(TObject *Sender)
{
 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUDrawEngineForm::ImageMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
 if(Shift.Contains(ssRight) && Shift.Contains(ssDouble))
 {
  UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->SelectUp();
  SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
 }
 else
 if(Shift.Contains(ssLeft) && !Shift.Contains(ssDouble))
 {
  std::string name=DrawEngine.FindComponent(X,Y);

  if(name != "")
  {
   UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->SetSelectedComponentName(name);
  }
 }
 else
 if(Shift.Contains(ssLeft) && Shift.Contains(ssDouble))
 {
  TPoint pos=Image->ScreenToClient(Mouse->CursorPos);

  std::string name=DrawEngine.FindComponent(pos.X,pos.Y);

  if(name != "")
  {
   UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->SelectComponentByName(name);
   SetNet(UComponentsControlForm->ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
  }
 }


}
//---------------------------------------------------------------------------
