//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UCRDeepLearningPerseptronFormUnit.h"
#include "UEngineMonitorFrameUnit.h"
#include "TUBitmap.h"
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

 try{
   Perseptron=Learning->DeepPerseptron;
   }
  catch(/*RDK::EUsingZeroPtr &*/...){}

 if(!Perseptron)
  return;

  int index=LayersComboBox->ItemIndex;
  if(LayersComboBox->Items->Count !=Perseptron->Weights.size())
  {
   LayersComboBox->Clear();
   for(size_t i=0;i<Perseptron->Weights.size();i++)
	LayersComboBox->Items->Add(IntToStr(int(i)));
   if(index >=0 && index <LayersComboBox->Items->Count)
	LayersComboBox->ItemIndex=index;
   else
    LayersComboBox->ItemIndex=0;
  }

  DrawImageFromPerseptronData(Perseptron->Outputs.v[LayersComboBox->ItemIndex], Learning->TrainSample->SampleImageWidth, Learning->TrainSample->SampleImageHeight, Perseptron->MinOutputValue,
								 Perseptron->MaxOutputValue, Bmp);

  RDK::operator >>(Bmp,Image->Picture->Bitmap);
  Image->Repaint();
}

// Сохраняет параметры интерфейса в xml
void TUCRDeepLearningPerseptronForm::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUCRDeepLearningPerseptronForm::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{

}

void TUCRDeepLearningPerseptronForm::DrawImageFromPerseptronData(const std::vector<double> &data,
					int image_width, int image_height, double min_output, double max_output, RDK::UBitmap &result)
{
 result.SetRes(image_width, image_height, RDK::ubmY8);
 unsigned char * image_data=result.GetData();
 for(size_t i=0;i<data.size();i++)
 {
  *image_data++=255-int((data[i]-min_output)*(255)/(max_output-min_output));
 }
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

void __fastcall TUCRDeepLearningPerseptronForm::Button1Click(TObject *Sender)
{
 Learning->Train1CreateLayer();
}
//---------------------------------------------------------------------------

void __fastcall TUCRDeepLearningPerseptronForm::FormShow(TObject *Sender)
{
  try{
   Learning=RDK::dynamic_pointer_cast<RDK::UCRPerseptronDeepLearning>(GetModel()->GetComponentL(ComponentControlName));
  }
  catch(...){}
}
//---------------------------------------------------------------------------

