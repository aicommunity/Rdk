//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsLinksHintFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma link "UComponentIOFrameUnit"
#pragma resource "*.dfm"
TUComponentsLinksHintForm *UComponentsLinksHintForm;
//---------------------------------------------------------------------------
__fastcall TUComponentsLinksHintForm::TUComponentsLinksHintForm(TComponent* Owner)
	: TForm(Owner)
{
}

/// Отображает форму
void __fastcall  TUComponentsLinksHintForm::ShowLinks(const std::string &component_name,
								const std::string &owner_name,
								int x, int y)
{
 InputsFrame->ShowModifier=2;
 InputsFrame->Mode=5;
 OutputsFrame->ShowModifier=2;
 OutputsFrame->Mode=6;
 InputsFrame->ViewComponentOwnerLongId=owner_name;
 InputsFrame->ViewComponentLongId=component_name;

 OutputsFrame->ViewComponentOwnerLongId=owner_name;
 OutputsFrame->ViewComponentLongId=component_name;
 Left=x;
 Top=y;
 Caption=String("Links of ")+component_name.c_str();
 Show();
 InputsFrame->UpdateInterface();
 OutputsFrame->UpdateInterface();

 Width=CalcFormWidth();
 GroupBox2->Height=CalcOutputsHeight()+30;
 Height=CalcInputsHeight()+CalcOutputsHeight()+Splitter1->Height+GroupBox1->Height-GroupBox1->ClientHeight
 	+GroupBox2->Height-GroupBox2->ClientHeight+Height-ClientHeight+35;
}

/// Расчитывает ширину окна
int TUComponentsLinksHintForm::CalcFormWidth(void)
{
 int input_width=0;
 for(int i=0;i<InputsFrame->StringGrid->ColCount;i++)
  input_width+=InputsFrame->StringGrid->ColWidths[i];
 int output_width=0;
 for(int i=0;i<OutputsFrame->StringGrid->ColCount;i++)
  output_width+=OutputsFrame->StringGrid->ColWidths[i];

 return ((output_width>input_width)?output_width:input_width)+60;
}

/// Расчитывает высоту таблицы отображения входов
int TUComponentsLinksHintForm::CalcInputsHeight(void)
{
 int num_rows=InputsFrame->StringGrid->RowCount;
 if(num_rows>10)
  num_rows=10;
 int height=(num_rows>1?num_rows+1:2)*InputsFrame->StringGrid->RowHeights[0];
 return height;
}

/// Расчитывает высоту таблицы отображения выходов
int TUComponentsLinksHintForm::CalcOutputsHeight(void)
{
 int num_rows=OutputsFrame->StringGrid->RowCount;
 if(num_rows>10)
  num_rows=10;
 int height=(num_rows>1?num_rows:2)*InputsFrame->StringGrid->RowHeights[0];
 return height;
}

//---------------------------------------------------------------------------
