//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UCRConvolutionNetworkFormUnit.h"
#include "UComponentsListFrameUnit.h"
#include "UShowProgressBarUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUCRConvolutionNetworkForm *UCRConvolutionNetworkForm;
//---------------------------------------------------------------------------
__fastcall TUCRConvolutionNetworkForm::TUCRConvolutionNetworkForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 ComponentControlName="CRConvolutionNetwork";
 ShowWeightFlag=false;
}
//---------------------------------------------------------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
void TUCRConvolutionNetworkForm::AAfterReset(void)
{
}

// Обновление интерфейса
void TUCRConvolutionNetworkForm::AUpdateInterface(void)
{
 try{
   if(!GetModel())
    return;
   Network=RDK::static_pointer_cast<RDK::UCRConvolutionNetwork>(GetModel()->GetComponentL(ComponentControlName));
  }
  catch(/*RDK::EUsingZeroPtr &*/...){}

 if(!Network)
  return;
  /*
  Weights=Network->Weights;

  int index=WeightsComboBox->ItemIndex;
  WeightsComboBox->Clear();
  for(size_t i=0;i<Weights.size();i++)
   WeightsComboBox->Items->Add(IntToStr(int(i)));
  if(index >=0 && index <WeightsComboBox->Items->Count)
   WeightsComboBox->ItemIndex=index;
  else
   WeightsComboBox->ItemIndex=0;


 if(ShowWeightFlag)
 {
  ShowWeightFlag=false;
  if(int(Weights.size())<WeightsComboBox->ItemIndex)
  {
   WeightsStringGrid->RowCount=1;
   return;
  }

  UShowProgressBarForm->ResetBarStatus(1, 0, Weights[WeightsComboBox->ItemIndex].size());
  UShowProgressBarForm->ResetBarStatus(2, 0, Weights[WeightsComboBox->ItemIndex].size());
  UShowProgressBarForm->Show();

  WeightsStringGrid->RowCount=Weights[WeightsComboBox->ItemIndex].size()+1;
  int col_count=1;
  int rows_size=Weights[WeightsComboBox->ItemIndex].size();
  if(rows_size>100)
   rows_size=100;
  for(int i=0;i<rows_size;i++)
  {
   if(col_count<int(Weights[WeightsComboBox->ItemIndex][i].size())+1)
   {
	WeightsStringGrid->ColCount=Weights[WeightsComboBox->ItemIndex][i].size()+1;
	col_count=WeightsStringGrid->ColCount;
   }
   for(size_t j=0;j<Weights[WeightsComboBox->ItemIndex][i].size();j++)
	WeightsStringGrid->Cells[j+1][i+1]=FloatToStrF(Weights[WeightsComboBox->ItemIndex][i][j],ffFixed, 5,5);
   UShowProgressBarForm->IncBarStatus(1);
   UShowProgressBarForm->IncBarStatus(2);
  }
  UShowProgressBarForm->Hide();
 } */


 Result=*(Network->Result);

 OutputsStringGrid->RowCount=Result.GetRows()+1;
 size_t col_count=0;
 for(size_t i=0;i<Result.GetRows();i++)
 {
  if(col_count<Result.GetCols()+1)
  {
   OutputsStringGrid->ColCount=Result.GetCols()+1;
   col_count=OutputsStringGrid->ColCount;
  }
  for(size_t j=0;j<Result.GetCols();j++)
   OutputsStringGrid->Cells[j+1][i+1]=FloatToStrF(Result(i, j),ffFixed, 5,5);
  for(size_t j=Result.GetCols();j<col_count;j++)
   OutputsStringGrid->Cells[j+1][i+1]="";
 }
}

// Сохраняет параметры интерфейса в xml
void TUCRConvolutionNetworkForm::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUCRConvolutionNetworkForm::ALoadParameters(RDK::USerStorageXML &xml)
{

}

// Создание копии этого компонента
TUCRConvolutionNetworkForm* TUCRConvolutionNetworkForm::New(TComponent *owner)
{
 return new TUCRConvolutionNetworkForm(owner);
}
// -----------------------------
void __fastcall TUCRConvolutionNetworkForm::WeightsComboBoxSelect(TObject *Sender)
{
 ShowWeightFlag=true;
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUCRConvolutionNetworkForm::FormShow(TObject *Sender)
{
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUCRConvolutionNetworkForm::Button1Click(TObject *Sender)
{
 ShowWeightFlag=true;
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUCRConvolutionNetworkForm::FormCreate(TObject *Sender)
{
 TUComponentsListFrame::ComponentControllers["UCRConvolutionNetwork"]=UCRConvolutionNetworkForm;
}
//---------------------------------------------------------------------------

