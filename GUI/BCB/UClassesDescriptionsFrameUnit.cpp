//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UClassesDescriptionsFrameUnit.h"
#include "rdk_initdll.h"
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
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
void TUClassesDescriptionsFrame::AAfterReset(void)
{

}

// Обновление интерфейса
void TUClassesDescriptionsFrame::AUpdateInterface(void)
{
 if(CurrentClassName.Length() == 0)
  return;

 const char * str=Storage_GetClassDescription(AnsiString(CurrentClassName).c_str());
 if(!str)
 {
  return;
 }

 RDK::USerStorageXML xml;
 xml.Load(str,AnsiString(CurrentClassName).c_str());

 PropertyDescriptionRichEdit->Lines->Clear();
 PropertyDescriptionRichEdit->Lines->Add(str);
 Engine_FreeBufString(str);

 ClassNameLabeledEdit->Text=xml.ReadString("ClassName","").c_str();
 ClassFullNameLabeledEdit->Text=xml.ReadString("Header","").c_str();
 ClassDescriptionRichEdit->Text=xml.ReadString("Description","").c_str();

 if(xml.SelectNode("Properties"))
 {
  int num_nodes=xml.GetNumNodes();
  PropertiesListStringGrid->RowCount=num_nodes+1;
  PropertiesListStringGrid->ColCount=4;
  PropertiesListStringGrid->Cells[0][0]="#";
  PropertiesListStringGrid->Cells[1][0]="Name";
  PropertiesListStringGrid->Cells[2][0]="Header";
  PropertiesListStringGrid->Cells[3][0]="Type";

  PropertiesListStringGrid->ColWidths[0]=25;
  PropertiesListStringGrid->ColWidths[1]=150;
  PropertiesListStringGrid->ColWidths[2]=PropertiesListStringGrid->ClientWidth-
										(PropertiesListStringGrid->ColWidths[0]+
										PropertiesListStringGrid->ColWidths[1]+
										PropertiesListStringGrid->ColWidths[3]+30);
  PropertiesListStringGrid->ColWidths[3]=100;
  for(int i=0;i<num_nodes;i++)
  {
   xml.SelectNode(i);
   PropertiesListStringGrid->Cells[0][i+1]=IntToStr(i);
   PropertiesListStringGrid->Cells[1][i+1]=xml.GetNodeName().c_str();
   PropertiesListStringGrid->Cells[2][i+1]=xml.ReadString("Header","").c_str();
   PropertiesListStringGrid->Cells[3][i+1]=xml.ReadString("Type","").c_str();

   xml.SelectUp();
  }
  xml.SelectUp();
 }

}

// Сохраняет параметры интерфейса в xml
void TUClassesDescriptionsFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUClassesDescriptionsFrame::ALoadParameters(RDK::USerStorageXML &xml)
{

}

// Создание копии этого компонента
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

