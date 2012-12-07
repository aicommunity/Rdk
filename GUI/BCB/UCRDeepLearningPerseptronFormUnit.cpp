//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UCRDeepLearningPerseptronFormUnit.h"
#include "UEngineMonitorFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUCRDeepLearningPerseptronForm *UCRDeepLearningPerseptronForm;
//---------------------------------------------------------------------------
__fastcall TUCRDeepLearningPerseptronForm::TUCRDeepLearningPerseptronForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 ComponentControlName="CRPerseptronDeepLearning";
}


// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
void TUCRDeepLearningPerseptronForm::AAfterReset(void)
{
}

// Обновление интерфейса
void TUCRDeepLearningPerseptronForm::AUpdateInterface(void)
{

  try{
   Learning=RDK::dynamic_pointer_cast<RDK::UCRPerseptronDeepLearning>(GetModel()->GetComponentL(ComponentControlName));
  }
  catch(...){}
 if(!Learning)
  return;
/*// RDK::ReadParameterValue<std::vector<std::vector<std::vector<double> > > >(ComponentName, "Weights", Weights);
 const std::vector<std::pair<RDK::UItemData,int> > &sample_data=Sample->GetSampleData();

 SampleStringGrid->RowCount=sample_data.size()+1;
 int col_count=2;
 for(size_t i=0;i<sample_data.size();i++)
 {
  if(col_count<sample_data[i].first.GetSize()+2)
  {
   SampleStringGrid->ColCount=sample_data[i].first.GetSize()+2;
   col_count=SampleStringGrid->ColCount;
  }
  for(size_t j=0;j<sample_data[i].first.GetSize();j++)
  {
   SampleStringGrid->Cells[0][i+1]=i;
   SampleStringGrid->Cells[1][i+1]=IntToStr(sample_data[i].second);
   SampleStringGrid->Cells[j+2][i+1]=FloatToStrF(sample_data[i].first.Double[j],ffFixed, 5,5);
  }
 }

 int index=SampleStringGrid->Row-1;
 if(index>=0 && index<sample_data.size())
  Bitmap=Sample->GetBitmapSampleData()[index].first;

 Bitmap>>Image->Picture->Bitmap;
 Image->Repaint();  */
}

// Сохраняет параметры интерфейса в xml
void TUCRDeepLearningPerseptronForm::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUCRDeepLearningPerseptronForm::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{

}

//---------------------------------------------------------------------------
void __fastcall TUCRDeepLearningPerseptronForm::ResetTrainingButtonClick(TObject *Sender)
{
 UEngineMonitorFrame->Reset1Click(this);
 Learning->ResetTraining();
}
//---------------------------------------------------------------------------





void __fastcall TUCRDeepLearningPerseptronForm::ModeComboBoxSelect(TObject *Sender)

{
 Learning->Mode=ModeComboBox->ItemIndex;
}
//---------------------------------------------------------------------------

