//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVCaptureOptionsFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVCaptureOptionsFrame *VCaptureOptionsFrame;
//---------------------------------------------------------------------------
__fastcall TVCaptureOptionsFrame::TVCaptureOptionsFrame(TComponent* Owner)
    : TFrame(Owner)
{
 UpdateCaptureInterfaceFlag=false;
 DCapture=0;
 FCapture=0;
 PauseInvisibleFlag=true;
}

// -----------------------------
// Методы управления параметрами
// -----------------------------
// Если флаг true - при смене активной вкладке - неактивные источники видео
// останавливаются командой Pause.
bool TVCaptureOptionsFrame::GetPauseInvisibleFlag(void) const
{
 return PauseInvisibleFlag;
}

bool TVCaptureOptionsFrame::SetPauseInvisibleFlag(bool value)
{
 if(PauseInvisibleFlag == value)
  return true;

 PauseInvisibleFlag=value;
 return true;
}
// -----------------------------


// -----------------------------
// Методы управления устройством ввода видео
// -----------------------------
// Устанавливает устройство видеоввода
bool TVCaptureOptionsFrame::SetDCapture(RDK::VCapture::VDCapture *capture)
{
 if(DCapture == capture)
  return true;

 DCapture=capture;
 return true;
}

bool TVCaptureOptionsFrame::SetFCapture(RDK::VCapture::VFCapture *capture)
{
 if(FCapture == capture)
  return true;

 FCapture=capture;
 return true;
}

// Возвращает устройство видеоввода
RDK::VCapture::VDCapture* TVCaptureOptionsFrame::GetDCapture(void) const
{
 return DCapture;
}

RDK::VCapture::VFCapture* TVCaptureOptionsFrame::GetFCapture(void) const
{
 return FCapture;
}

// Возвращает текущее устройство видеоввода
RDK::VCapture::VACapture* TVCaptureOptionsFrame::GetCapture(void) const
{
 if(VCapturePageControl->ActivePageIndex == 0)
  return DCapture;

 if(VCapturePageControl->ActivePageIndex == 1)
  return FCapture;

 return 0;
}


// Обновляет интерфейс фрейма
void TVCaptureOptionsFrame::UpdateInterface(void)
{
 UpdateCaptureInterfaceFlag=true;

 if(DCapture)
 {
  DeviceTabSheet->TabVisible=true;

  if(VCapturePageControl->ActivePage == DeviceTabSheet)
  {
   std::vector<std::wstring> &devices=DCapture->GetCaptureDevicesList();
   DeviceComboBox->Clear();
   for(size_t i=0;i<devices.size();i++)
    DeviceComboBox->Items->Add(devices[i].c_str());

   std::vector<long> &crossbar=DCapture->GetCrossbarInputsList();
   InputComboBox->Clear();
   InputComboBox->Items->Add("Default");
   for(size_t i=0;i<crossbar.size();i++)
    InputComboBox->Items->Add(RDK::VCapture::VideoCrossbarsNameList[crossbar[i]].c_str());

   if(DCapture->GetActiveCrossbarInput() >= 0 && DCapture->GetActiveCrossbarInput()< int(crossbar.size()))
    InputComboBox->ItemIndex=DCapture->GetActiveCrossbarInput()+1;
   else
    InputComboBox->ItemIndex=0;


   ModeComboBox->Clear();
   std::map<int,VIDEOINFOHEADER>& mode=DCapture->GetCaptureModesList();
   std::map<int,AM_MEDIA_TYPE>& fullmode=DCapture->GetCaptureFullModesList();

   String textmode;

   std::map<int,VIDEOINFOHEADER>::iterator I,J;
   I=mode.begin();
   J=mode.end();
   while(I != J)
   {
    textmode=IntToStr(int(I->first))+"> ";
    textmode+=IntToStr(int(I->second.bmiHeader.biWidth));
    textmode+="x";
    textmode+=IntToStr(int(I->second.bmiHeader.biHeight));
    textmode+="x";
    textmode+=IntToStr(int(I->second.bmiHeader.biBitCount));
    textmode+=": ";
    textmode+=DCapture->GetTextVideoModeType(I->first).c_str();
    ModeComboBox->Items->Add(textmode);
    ++I;
   }

   if(DCapture->GetActiveMode() >= 0 && DCapture->GetActiveMode()< int(mode.size()))
    ModeComboBox->ItemIndex=DCapture->GetActiveMode();
   else
    ModeComboBox->ItemIndex=-1;

   if(DCapture->GetActiveDevice() >= 0 && DCapture->GetActiveDevice()< int(devices.size()))
    DeviceComboBox->ItemIndex=DCapture->GetActiveDevice();
   else
    DeviceComboBox->ItemIndex=-1;
  }
 }
 else
 {
  DeviceTabSheet->TabVisible=false;
  DeviceComboBox->Clear();
  InputComboBox->Clear();
  ModeComboBox->Clear();
 }


 if(FCapture)
 {
  VideoFileTabSheet->TabVisible=true;

  if(VCapturePageControl->ActivePage == VideoFileTabSheet)
  {
   std::string sstamp;
   double cfiletime=FCapture->GetCurrentFileTime();
   RDK::VCapture::VTimeStamp stamp(cfiletime,FCapture->GetFPS());
   stamp>>sstamp;

   TimeTrackBar->Max=FCapture->GetLength();
   TimeTrackBar->Position=cfiletime;

   TimeEdit->Text=sstamp.c_str();
   TimeEdit->Repaint();

   VFNameEdit->Text=FCapture->GetFileName().c_str();
   VFNameEdit->Repaint();
  }
 }
 else
 {
  VideoFileTabSheet->TabVisible=false;
 }

 UpdateCaptureInterfaceFlag=false;
}
// -----------------------------
void __fastcall TVCaptureOptionsFrame::DeviceComboBoxSelect(TObject *Sender)
{
 if(UpdateCaptureInterfaceFlag)
  return;

 if(DCapture)
 {
  DCapture->SetActiveDevice(DeviceComboBox->ItemIndex);
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------

void __fastcall TVCaptureOptionsFrame::InputComboBoxSelect(TObject *Sender)
{
 if(UpdateCaptureInterfaceFlag)
  return;

 if(InputComboBox->ItemIndex == 0)
  return;

 if(DCapture)
 {
  DCapture->SetActiveCrossbarInput(InputComboBox->ItemIndex-1);
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------

void __fastcall TVCaptureOptionsFrame::ModeComboBoxSelect(TObject *Sender)
{
 if(UpdateCaptureInterfaceFlag)
  return;

 if(DCapture)
 {
  std::string mode=AnsiString(ModeComboBox->Text).c_str();
  int imode=0;
  int index=mode.find_first_of(">");
  imode=RDK::atoi(mode.substr(0,index));
  DCapture->SetActiveMode(imode);
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------
void __fastcall TVCaptureOptionsFrame::VFBrowseButtonClick(TObject *Sender)
{
 if(!VFOpenDialog->Execute())
  return;

 VFNameEdit->Text=VFOpenDialog->FileName;

 if(FCapture)
 {
  FCapture->SetFileName(VFNameEdit->Text.w_str());
  UpdateInterface();
 }

}
//---------------------------------------------------------------------------
void __fastcall TVCaptureOptionsFrame::TimeEditChange(TObject *Sender)
{
 if(UpdateCaptureInterfaceFlag || !FCapture)
  return;

 std::string sstamp;
 RDK::VCapture::VTimeStamp stamp(0.0,FCapture->GetFPS());
 sstamp=AnsiString(TimeEdit->Text).c_str();
 stamp<<sstamp;

 FCapture->SelectFrame(stamp());
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVCaptureOptionsFrame::TimeTrackBarChange(TObject *Sender)
{
 if(UpdateCaptureInterfaceFlag || !FCapture)
  return;

 std::string sstamp;
 RDK::VCapture::VTimeStamp stamp(double(TimeTrackBar->Position),double(FCapture->GetFPS()));
 FCapture->SelectFrame(stamp());
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVCaptureOptionsFrame::VCapturePageControlChange(TObject *Sender)

{
 if(PauseInvisibleFlag)
 {
  if(DeviceTabSheet->TabVisible == false && DCapture->GetCaptureState())
   DCapture->Pause();

  if(VideoFileTabSheet->TabVisible == false && FCapture->GetCaptureState())
   FCapture->Pause();
 }

 UpdateInterface();
}
//---------------------------------------------------------------------------


