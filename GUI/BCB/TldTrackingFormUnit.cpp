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
}

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������� ������
void TTldTrackingForm::ABeforeReset(void)
{

}

// �����, ���������� ����� ����� �������
void TTldTrackingForm::ABeforeCalculate(void)
{

}

// ���������� ����������
void TTldTrackingForm::AUpdateInterface(void)
{
 if(ComponentControlName.empty())
  return;
 int num_inputs=1;//RDK::ReadParameterValue<int>(ComponentControlName, "NumInputs");
 LoadVideoInputs(num_inputs, VideoSourceComboBox);

 const RDK::MDMatrix<double>* points=(const RDK::MDMatrix<double>*)Model_GetComponentPropertyData("MatrixSource", "DoubleMatrix");

 num_inputs=points->GetRows();//RDK::ReadParameterValue<int>(ComponentControlName, "NumTrackers");
 LoadVideoInputs(num_inputs, ObjectReceiverComboBox);
// LoadVideoInputs(num_inputs, ObjectReceiver2ComboBox);
}

// �����, ���������� ����� ���� �������
void TTldTrackingForm::AAfterCalculate(void)
{
 if(ComponentControlName.empty())
  return;

 int video_index=0;

 if(VideoSourceComboBox->ItemIndex >= 0)
  video_index=VideoSourceComboBox->ItemIndex;

 RDK::UBitmap* bmp=0;
 if(VideoOutputForm->GetVideoOutputFrame(VideoSourceComboBox->ItemIndex))
 {
  VideoOutputFrame1->ZoneSelectEnable=true;
  ResultBmp=VideoOutputForm->GetVideoOutputFrame(VideoSourceComboBox->ItemIndex)->BmpSource;
 }
 else
  ResultBmp.Fill(0);

 VideoOutputFrame1->InitByBmp(ResultBmp);

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

// ��������� ��������� ���������� � xml
void TTldTrackingForm::ASaveParameters(RDK::USerStorageXML &xml)
{
}

// ��������� ��������� ���������� �� xml
void TTldTrackingForm::ALoadParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ComboBox ������� ����� ������ Tld
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

 if(VideoOutputForm->GetVideoOutputFrame(VideoSourceComboBox->ItemIndex))
 {
  VideoOutputFrame1->ZoneSelectEnable=true;
  VideoOutputFrame1->InitByBmp(VideoOutputForm->GetVideoOutputFrame(VideoSourceComboBox->ItemIndex)->BmpSource);
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

void __fastcall TTldTrackingForm::GotoFirstFrameButtonClick(TObject *Sender)
{
 if(VideoOutputForm->GetVideoOutputFrame(VideoSourceComboBox->ItemIndex))
 {
  VideoOutputForm->GetVideoOutputFrame(VideoSourceComboBox->ItemIndex)->TrackBar->Position=0;
  VideoOutputForm->GetVideoOutputFrame(VideoSourceComboBox->ItemIndex)->UpdateVideo();
 }
}
//---------------------------------------------------------------------------

void __fastcall TTldTrackingForm::SendObjectToButtonClick(TObject *Sender)
{
 if(ObjectReceiverComboBox->ItemIndex < 0)
  return;

 const RDK::MDMatrix<double>* old_points=(const RDK::MDMatrix<double>*)Model_GetComponentPropertyData("MatrixSource", "DoubleMatrix");
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
 Model_SetComponentBitmapInput(ComponentControlName.c_str(), ObjectReceiverComboBox->ItemIndex, &ResultBmp);

 Model_SetComponentPropertyData(ComponentControlName.c_str(), "InitialFlags", &initial_flags);
 Model_SetComponentPropertyData("MatrixSource", "DoubleMatrix", &points);
 // RDK::WriteStateValue(ComponentControlName, "InitialFlags", initial_flags);
// RDK::WriteStateValue("MatrixSource", "DoubleMatrix", points);
 Env_Calculate(ComponentControlName.c_str());
 AAfterCalculate();
 VideoOutputFrame1->UpdateVideo();
 //RDK::UIVisualControllerStorage::AfterCalculate();
 //RDK::UIVisualControllerStorage::UpdateInterface();
 // UEngineMonitorForm->EngineMonitorFrame->Step1Click(Sender);
// if(started)
//  StartTrackingButtonClick(Sender);
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
 filename+="TrackerData";
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
 filename+="TrackerData";
 filename+=RDK::sntoa(ObjectReceiverComboBox->ItemIndex);
 filename+=".tld";
 Model_SetComponentPropertyData(ComponentControlName.c_str(), "TrackerDataFileName", &filename);
 int item_index=ObjectReceiverComboBox->ItemIndex;
 Model_SetComponentPropertyData(ComponentControlName.c_str(), "LoadTrackerDataIndex", &item_index);
 //UEngineMonitorForm->EngineMonitorFrame->Step1Click(Sender);
}
//---------------------------------------------------------------------------

