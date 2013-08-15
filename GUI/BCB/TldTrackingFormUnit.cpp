//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsListFrameUnit.h"
#include "TldTrackingFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "UGEngineControlFormUnit.h"
#include "VideoOutputFormUnit.h"
#include "rdk_initdll.h"
//#include "RTVSupport.h"
//#include "RTVXMLSerialize.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma link "VideoOutputFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma resource "*.dfm"
TTldTrackingForm *TldTrackingForm;

//---------------------------------------------------------------------------
__fastcall TTldTrackingForm::TTldTrackingForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 ComponentControlName="TldTracking";
 VideoSourceType=1;
}

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед сбросом модели
void TTldTrackingForm::ABeforeReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void TTldTrackingForm::ABeforeCalculate(void)
{

}

// Обновление интерфейса
void TTldTrackingForm::AUpdateInterface(void)
{
 if(ComponentControlName.empty())
  return;
 int num_inputs=1;//RDK::ReadParameterValue<int>(ComponentControlName, "NumInputs");
// LoadVideoInputs(num_inputs, VideoSourceComboBox);

 size_t pos=ComponentControlName.find_last_of(".");
 std::string source_name;
 if(pos != std::string::npos)
 {
  source_name=ComponentControlName.substr(0,ComponentControlName.find_last_of(".")+1)+"MatrixSource";
 }
 else
  source_name="MatrixSource";

 const RDK::MDMatrix<double>* points=(const RDK::MDMatrix<double>*)Model_GetComponentPropertyData(source_name.c_str(), "DoubleMatrix");

 if(points)
  num_inputs=points->GetRows();//RDK::ReadParameterValue<int>(ComponentControlName, "NumTrackers");
 else
  num_inputs=0;
 LoadVideoInputs(num_inputs, ObjectReceiverComboBox);
// LoadVideoInputs(num_inputs, ObjectReceiver2ComboBox);
}

// Метод, вызываемый после шага расчета
void TTldTrackingForm::AAfterCalculate(void)
{
 if(ComponentControlName.empty())
  return;

// int video_index=0;

// if(VideoSourceComboBox->ItemIndex >= 0)
//  video_index=VideoSourceComboBox->ItemIndex;

 if(VideoSourceType == 0)
 {
  const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentBitmapInput(ComponentControlName.c_str(), 0);
  if(bmp)
  {
   const_cast<RDK::UBitmap*>(bmp)->ReflectionX(&ResultBmp);
//  ResultBmp=*bmp;

   VideoOutputFrame1->ZoneSelectEnable=true;
   VideoOutputFrame1->InitByBmp(ResultBmp);
  }
  else
   ResultBmp.Fill(0);
 }

 const RDK::MDMatrix<double> *results;
 results=(const RDK::MDMatrix<double>*)(Model_GetComponentOutputAsMatrix(ComponentControlName.c_str(), 0));
 if(!results)
  return;

// RDK::ReadStateValue(ComponentControlName, "Result",results);
 if(results->GetRows()>0)
 {
  VideoOutputFrame1->left=(*results)(0,0);
  VideoOutputFrame1->top=(*results)(0,1);
  VideoOutputFrame1->width=(*results)(0,2);
  VideoOutputFrame1->height=(*results)(0,3);
 }
}

// Сохраняет параметры интерфейса в xml
void TTldTrackingForm::ASaveParameters(RDK::USerStorageXML &xml)
{
}

// Загружает параметры интерфейса из xml
void TTldTrackingForm::ALoadParameters(RDK::USerStorageXML &xml)
{

}


// Создание копии этого компонента
TTldTrackingForm* TTldTrackingForm::New(TComponent *owner)
{
 return new TTldTrackingForm(owner);
}

// Заполняет ComboBox списком видео входов Tld
void TTldTrackingForm::LoadVideoInputs(int num_inputs, TComboBox *box)
{
 if(!box)
  return;

 int index=box->ItemIndex;
 box->Clear();
 for(int i=0;i<num_inputs;i++)
  box->Items->Add(IntToStr(i));

 if(index >=0 && index < box->Items->Count)
  box->ItemIndex=index;
 else
  box->ItemIndex=0;
}
// -----------------------------
//---------------------------------------------------------------------------
void __fastcall TTldTrackingForm::StartTrackingButtonClick(TObject *Sender)
{
 VideoOutputFrame1->GeometryGraphics.Clear();
 VideoOutputFrame1->MyVideoOutputToolsForm->AddFigureButtonClick(Sender);
 VideoOutputFrame1->left=VideoOutputFrame1->top=VideoOutputFrame1->width=VideoOutputFrame1->height=-1;
 VideoOutputFrame1->UpdateVideo();

 UEngineMonitorForm->EngineMonitorFrame->Start1Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TTldTrackingForm::StopTrackingButtonClick(TObject *Sender)
{
 UEngineMonitorForm->EngineMonitorFrame->Pause1Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TTldTrackingForm::GetFrameButtonClick(TObject *Sender)
{
 bool started=UEngineMonitorForm->EngineMonitorFrame->Timer->Enabled;
 if(started)
  StopTrackingButtonClick(Sender);

// if(VideoOutputForm->GetVideoOutputFrame(VideoSourceComboBox->ItemIndex))
 {
  VideoOutputFrame1->ZoneSelectEnable=true;
  const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentBitmapInput(ComponentControlName.c_str(), 0);
  if(bmp)
  {
   const_cast<RDK::UBitmap*>(bmp)->ReflectionX(&ResultBmp);
   VideoOutputFrame1->InitByBmp(ResultBmp);
   ResultBmp.ReflectionX();
  }
 }
}
//---------------------------------------------------------------------------
void __fastcall TTldTrackingForm::FormCreate(TObject *Sender)
{
 TUComponentsListFrame::ComponentControllers["TDllTldTracking"]=TldTrackingForm;
}
//---------------------------------------------------------------------------
void __fastcall TTldTrackingForm::EnableLogCheckBoxClick(TObject *Sender)
{
/* if(EnableLogCheckBox->Checked)
  RDK::WriteParameterValue("BStatisticSimple", "Activity", true);
 else
  RDK::WriteParameterValue("BStatisticSimple", "Activity", false);
  */
}
//---------------------------------------------------------------------------


void __fastcall TTldTrackingForm::SendObjectToButtonClick(TObject *Sender)
{
 if(ObjectReceiverComboBox->ItemIndex < 0)
  return;

 size_t pos=ComponentControlName.find_last_of(".");
 std::string source_name;
 if(pos != std::string::npos)
 {
  source_name=ComponentControlName.substr(0,ComponentControlName.find_last_of(".")+1)+"MatrixSource";
 }
 else
  source_name="MatrixSource";

 const RDK::MDMatrix<double>* old_points=(const RDK::MDMatrix<double>*)Model_GetComponentPropertyData(source_name.c_str(), "DoubleMatrix");
 RDK::MDMatrix<double> points(*old_points);//(ObjectReceiverComboBox->Items->Count,4);
 points.Resize(points.GetRows(),4);
 points(ObjectReceiverComboBox->ItemIndex,0)=VideoOutputFrame1->left;
 points(ObjectReceiverComboBox->ItemIndex,1)=VideoOutputFrame1->top;
 points(ObjectReceiverComboBox->ItemIndex,2)=VideoOutputFrame1->width;
 points(ObjectReceiverComboBox->ItemIndex,3)=VideoOutputFrame1->height;

 RDK::MDVector<int> initial_flags(points.GetRows());
 initial_flags[ObjectReceiverComboBox->ItemIndex]=1;

 bool started=UEngineMonitorForm->EngineMonitorFrame->Timer->Enabled;
 if(started)
  StopTrackingButtonClick(Sender);
 UEngineMonitorForm->EngineMonitorFrame->Step1Click(Sender);

 ResultBmp.SetColorModel(RDK::ubmY8);
 VideoOutputFrame1->BmpSource.ConvertTo(ResultBmp);
 ResultBmp.ReflectionX();
 Env_Calculate(0);
 // Заглушка
// Model_SetComponentBitmapInput(ComponentControlName.c_str(), ObjectReceiverComboBox->ItemIndex, &ResultBmp);

 Model_SetComponentPropertyData(ComponentControlName.c_str(), "InitialFlags", &initial_flags);
 Model_SetComponentPropertyData(source_name.c_str(), "DoubleMatrix", &points);

 Env_Calculate(ComponentControlName.c_str());
 AAfterCalculate();
 VideoOutputFrame1->MyVideoOutputToolsForm->DelAllFiguresButtonClick(Sender);
 VideoOutputFrame1->MyVideoOutputToolsForm->AddFigureButtonClick(Sender);
 VideoOutputFrame1->UpdateVideo();
}
//---------------------------------------------------------------------------

void __fastcall TTldTrackingForm::CopyTrackerDataButtonClick(TObject *Sender)
{
/* RDK::WriteStateValue(ComponentControlName, "CopyTrackerDataFlag", 1);
 RDK::WriteStateValue(ComponentControlName, "CopyTrackerSourceIndex", ObjectReceiverComboBox->ItemIndex);
 RDK::WriteStateValue(ComponentControlName, "CopyTrackerDestinationIndex", ObjectReceiver2ComboBox->ItemIndex);
 Env_Calculate(ComponentControlName.c_str());
 */
}
//---------------------------------------------------------------------------

void __fastcall TTldTrackingForm::SaveTrackerDataButtonClick(TObject *Sender)
{
 std::string filename=AnsiString(UGEngineControlForm->ProjectPath).c_str();
 filename+=ComponentControlName.c_str();
 filename+=RDK::sntoa(ObjectReceiverComboBox->ItemIndex);
 filename+=".tld";
 Model_SetComponentPropertyData(ComponentControlName.c_str(), "TrackerDataFileName", &filename);
 int item_index=ObjectReceiverComboBox->ItemIndex;
 Model_SetComponentPropertyData(ComponentControlName.c_str(), "SaveTrackerDataIndex", &item_index);
 Env_Calculate(ComponentControlName.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TTldTrackingForm::LoadTrackerDataButtonClick(TObject *Sender)
{
 std::string filename=AnsiString(UGEngineControlForm->ProjectPath).c_str();
 filename+=ComponentControlName.c_str();
 filename+=RDK::sntoa(ObjectReceiverComboBox->ItemIndex);
 filename+=".tld";

 if(!OpenDialog1->Execute())
  return;

 filename=AnsiString(OpenDialog1->FileName).c_str();
 filename=filename.substr(0,filename.size()-5);
 Model_SetComponentPropertyData(ComponentControlName.c_str(), "TrackerDataFileName", &filename);
 int item_index=ObjectReceiverComboBox->ItemIndex;
 Model_SetComponentPropertyData(ComponentControlName.c_str(), "LoadTrackerDataIndex", &item_index);
 Env_Calculate(ComponentControlName.c_str());
}
//---------------------------------------------------------------------------


void __fastcall TTldTrackingForm::PointsTabSheetShow(TObject *Sender)
{
 VideoOutputFrame1->MyVideoOutputToolsForm->Parent=PointsTabSheet;
 VideoOutputFrame1->MyVideoOutputToolsForm->BorderStyle=bsNone;
 VideoOutputFrame1->MyVideoOutputToolsForm->FigurePanel->Visible=false;
 VideoOutputFrame1->MyVideoOutputToolsForm->Align=alClient;
 VideoOutputFrame1->MyVideoOutputToolsForm->Show();
 PointsTabSheet->Repaint();
}
//---------------------------------------------------------------------------




void __fastcall TTldTrackingForm::SendPointsButtonClick(TObject *Sender)
{
 size_t pos=ComponentControlName.find_last_of(".");
 std::string source_name;
 if(pos != std::string::npos)
 {
  source_name=ComponentControlName.substr(0,ComponentControlName.find_last_of(".")+1)+"MatrixSource";
 }
 else
  source_name="MatrixSource";

 const std::vector<RDK::MVector<double,2> > &new_points=VideoOutputFrame1->GeometryGraphics.Geometry(0).GetVertices();

 const RDK::MDMatrix<double>* old_points=(const RDK::MDMatrix<double>*)Model_GetComponentPropertyData(source_name.c_str(), "DoubleMatrix");
 RDK::MDMatrix<double> points(*old_points);//(ObjectReceiverComboBox->Items->Count,4);
 points.Resize(points.GetRows(),4);

 int area_width=StrToInt(RectWidthLabeledEdit->Text);
 int area_height=StrToInt(RectHeightLabeledEdit->Text);
 for(int i=0;i<new_points.size();i++)
 {
  if(i>=points.GetRows())
   break;
  points(i,0)=new_points[i](0)-area_width/2;
  points(i,1)=new_points[i](1)-area_height/2;
  points(i,2)=area_width;
  points(i,3)=area_height;
 }

 RDK::MDMatrix<int> initial_flags;
 RDK::MDMatrix<double> initial_reliability;
 initial_flags.Resize(points.GetRows(),1);
 initial_reliability.Resize(points.GetRows(),1);
 for(int i=0;i<initial_flags.GetRows();i++)
  if(VideoOutputFrame1->MyVideoOutputToolsForm->PointsCheckListBox->Count>i && VideoOutputFrame1->MyVideoOutputToolsForm->PointsCheckListBox->Checked[i] == true)
  {
   initial_flags(i,0)=1;
   initial_reliability(i,0)=1.0;
  }
  else
  {
   initial_flags(i,0)=0;
   initial_reliability(i,0)=0.0;
  }
// initial_flags.Assign(points.GetRows(),1);

 bool started=UEngineMonitorForm->EngineMonitorFrame->Timer->Enabled;
 if(started)
  StopTrackingButtonClick(Sender);
 if(CheckBox1->Checked)
 {
  UEngineMonitorForm->EngineMonitorFrame->Step1Click(Sender);

  ResultBmp.SetColorModel(RDK::ubmY8);
  VideoOutputFrame1->BmpSource.ConvertTo(ResultBmp);
  ResultBmp.ReflectionX();
  Env_Calculate(0);
  Model_SetComponentBitmapInput(ComponentControlName.c_str(), 0, &ResultBmp);
 }
 Model_SetComponentPropertyData(ComponentControlName.c_str(), "InitialFlags", &initial_flags);
 Model_SetComponentPropertyData(source_name.c_str(), "DoubleMatrix", &points);
 Model_SetComponentPropertyData("MatrixSource", "DoubleMatrix", &initial_reliability);

 if(CheckBox1->Checked)
 {
  Env_Calculate(ComponentControlName.c_str());
 }
 UEngineMonitorForm->EngineMonitorFrame->Step1Click(Sender);

 VideoOutputFrame1->MyVideoOutputToolsForm->DelAllFiguresButtonClick(Sender);
 VideoOutputFrame1->MyVideoOutputToolsForm->AddFigureButtonClick(Sender);
// VideoOutputFrame1->UpdateVideo();
// RDK::UIVisualControllerStorage::UpdateInterface();
// Hide();
}
//---------------------------------------------------------------------------

void __fastcall TTldTrackingForm::FormShow(TObject *Sender)
{
 VideoOutputFrame1->MyVideoOutputToolsForm->DelAllFiguresButtonClick(Sender);
 VideoOutputFrame1->MyVideoOutputToolsForm->AddFigureButtonClick(Sender);

 PointsTabSheetShow(Sender);
 PageControl1->ActivePageIndex=0;
 PageControl1->ActivePageIndex=1;
}
//---------------------------------------------------------------------------

void __fastcall TTldTrackingForm::Timer1Timer(TObject *Sender)
{
 if(VideoSourceType == 1)
 {
  if(VideoOutputForm && VideoOutputForm->GetActiveVideoOutputFrame())
  {
   VideoOutputFrame1->ZoneSelectEnable=true;
   VideoOutputFrame1->InitByBmp(VideoOutputForm->GetActiveVideoOutputFrame()->BmpSource);
  }
 }
}
//---------------------------------------------------------------------------

