//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsFormUnit.h"
#include "VideoOutputFrameUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsForm *VideoCaptureOptionsForm;

/// Список настройщиков источников видео
std::map<int, TVideoCaptureOptionsDesciption> TVideoCaptureOptionsForm::VideoSourceOptionsFrames;

/// Список поддерживаемых источников видео
std::map<int, RDK::UEPtr<TVideoCaptureThread> > TVideoCaptureOptionsForm::VideoSourcePrototypes;

//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsForm::TVideoCaptureOptionsForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
// VideoCaptureOptionsInterface=0;

 VideoOutputFrame=0;
}
//---------------------------------------------------------------------------
// ---------------------------
// Методы управления поддерживаемыми источниками видео
// ---------------------------
/// Возвращает список поддерживаемых источников видео
const std::map<int, RDK::UEPtr<TVideoCaptureThread> >& TVideoCaptureOptionsForm::GetVideoSourcePrototypes(void)
{
 return VideoSourcePrototypes;
}

/// Возвращает список поддерживаемых источников видео
bool TVideoCaptureOptionsForm::AddVideoSourcePrototypes(int mode, RDK::UEPtr<TVideoCaptureThread> thread)
{
 const std::map<int, RDK::UEPtr<TVideoCaptureThread> >::iterator I=VideoSourcePrototypes.find(mode);
 if(I != VideoSourcePrototypes.end())
 {
  delete thread;
  return false;
 }
 VideoSourcePrototypes[mode]=thread;
 return true;
}

/// Проверяет, существует ли такой видеоисточник
bool TVideoCaptureOptionsForm::CheckVideoSourcePrototypes(int mode)
{
 const std::map<int, RDK::UEPtr<TVideoCaptureThread> >::iterator I=VideoSourcePrototypes.find(mode);
 if(I != VideoSourcePrototypes.end())
  return true;
 return false;
}

/// Очищает список поддерживаемых источников видео
void TVideoCaptureOptionsForm::ClearAllVideoSourcePrototypes(void)
{
 std::map<int, RDK::UEPtr<TVideoCaptureThread> >::iterator I=VideoSourcePrototypes.begin();
 while(I != VideoSourcePrototypes.end())
 {
  delete I->second.operator ->();
 }
 VideoSourcePrototypes.clear();
}

/// Возвращает список настройщиков источников видео
std::map<int, TVideoCaptureOptionsDesciption> TVideoCaptureOptionsForm::GetVideoSourceOptionsFrames(void)
{
 return VideoSourceOptionsFrames;
}

/// Добавляет новый настройщик источников видео
bool TVideoCaptureOptionsForm::AddVideoSourceOptionsFrame(int mode, TVideoCaptureOptionsDesciption &frame)
{
 const std::map<int, TVideoCaptureOptionsDesciption>::iterator I=VideoSourceOptionsFrames.find(mode);
 if(I != VideoSourceOptionsFrames.end())
  return false;
 VideoSourceOptionsFrames[mode]=frame;
 return true;
}

/// Проверяет, существует ли такой настройщик
bool TVideoCaptureOptionsForm::CheckVideoSourceOptionsFrame(int mode)
{
 const std::map<int, TVideoCaptureOptionsDesciption>::iterator I=VideoSourceOptionsFrames.find(mode);
 if(I != VideoSourceOptionsFrames.end())
  return false;
 return true;
}

/// Очищает список поддерживаемых настройщиков видео
void TVideoCaptureOptionsForm::ClearAllVideoSourceOptionsFrames(void)
{
 std::map<int, TVideoCaptureOptionsDesciption>::iterator I=VideoSourceOptionsFrames.begin();
 while(I != VideoSourceOptionsFrames.end())
 {
  delete I->second.Form;
 }
 VideoSourceOptionsFrames.clear();
}

/// Возвращает активный (выбранный) источник видео
int TVideoCaptureOptionsForm::GetActiveVideoSourceId(void)
{
 TTabSheet *tab=PageControl->ActivePage;

 std::map<int, TVideoCaptureOptionsDesciption>::iterator I=CurrentVideoSourceOptionsFrames.begin();
 for(;I != CurrentVideoSourceOptionsFrames.end();++I)
 {
  if(I->second.Form->Parent == tab)
   return I->first;
 }
 return -1;
}

/// Возвращает настройщик видеоисточника, соответствующий режиму
TVideoCaptureOptionsDesciption* TVideoCaptureOptionsForm::GetCurrentVideoSourceOptionsFrame(int mode)
{
 std::map<int, TVideoCaptureOptionsDesciption>::iterator I=CurrentVideoSourceOptionsFrames.begin();
 for(;I != CurrentVideoSourceOptionsFrames.end();++I)
 {
  if(I->second.Form->GetVideoSourceType() == mode)
   return &(I->second);
 }
 return 0;
}

/// Устанавливает активной вкладку видеоисточника по индексу
bool TVideoCaptureOptionsForm::SelectVideoSourcePage(int mode)
{
 UpdateInterface(true);
 TVideoCaptureOptionsDesciption *frame=GetCurrentVideoSourceOptionsFrame(mode);
 if(!frame)
  return false;

 for(int i=0;i<PageControl->PageCount;i++)
 {
  if(PageControl->Pages[i] == frame->Form->Parent)
  {
   PageControl->ActivePageIndex=i;
//   PageControl->Update();
   PageControl->Repaint();
   break;
  }
 }
 return true;
}

// ---------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Если force_update == true, то интерфейс обновляется
// вне зависимости от UpdateInterval
void TVideoCaptureOptionsForm::AUpdateInterface(void)
{
 if(!VideoOutputFrame)
  return;

 const std::map<int, RDK::UEPtr<TVideoCaptureThread> > &prototypes=
	GetVideoSourcePrototypes();


 while(PageControl->PageCount < prototypes.size())
 {
  TTabSheet *tab=new TTabSheet(PageControl);
  tab->PageControl=PageControl;
 }

 std::map<int, TVideoCaptureOptionsDesciption>::const_iterator I=VideoSourceOptionsFrames.begin();
// int i=0;
 for(;I != VideoSourceOptionsFrames.end();++I)
 {
  if(!GetCurrentVideoSourceOptionsFrame(I->first))
  {
   TTabSheet *tab=PageControl->Pages[I->second.Position];

   CurrentVideoSourceOptionsFrames[I->first]=I->second;
   TVideoCaptureOptionsInterface *form=CurrentVideoSourceOptionsFrames[I->first].Form=(TVideoCaptureOptionsInterface *)I->second.Form->New(this);
   form->Parent=tab;
   form->Align=alClient;
   form->BorderStyle=bsNone;
   tab->Caption=I->second.Name.c_str();
   form->Show();
  }
 }

}

// Возврат интерфейса в исходное состояние
void TVideoCaptureOptionsForm::AClearInterface(void)
{

}

// Сохраняет параметры интерфейса в xml
void TVideoCaptureOptionsForm::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TVideoCaptureOptionsForm::ALoadParameters(RDK::USerStorageXML &xml)
{

}

// Создание копии этого компонента
TUVisualControllerForm* TVideoCaptureOptionsForm::New(TComponent *owner)
{
 return new TVideoCaptureOptionsForm(owner);
}
// -----------------------------
void __fastcall TVideoCaptureOptionsForm::ApplyButtonClick(TObject *Sender)
{
 if(!VideoOutputFrame)
  return;

 // Сохраняем параметры
 std::map<int, TVideoCaptureOptionsDesciption>::iterator I=CurrentVideoSourceOptionsFrames.begin();
 for(;I!=CurrentVideoSourceOptionsFrames.end();++I)
 {
  I->second.Form->Repaint();
  I->second.Form->WriteParametersToXml(VideoOutputFrame->VideoSourceOptions[I->first]);
 }

 int mode=GetActiveVideoSourceId();
 if(mode >=0)
  VideoOutputFrame->Init(mode);
 Hide();
}
//---------------------------------------------------------------------------

void __fastcall TVideoCaptureOptionsForm::FormShow(TObject *Sender)
{
 if(!VideoOutputFrame)
  return;

 // Грузим параметры
 UpdateInterface();
 std::map<int, TVideoCaptureOptionsDesciption>::iterator I=CurrentVideoSourceOptionsFrames.begin();
 for(;I!=CurrentVideoSourceOptionsFrames.end();++I)
 {
  I->second.Form->ReadParametersToGui(VideoOutputFrame->VideoSourceOptions[I->first]);
 }
}
//---------------------------------------------------------------------------

void __fastcall TVideoCaptureOptionsForm::RestoreButtonClick(TObject *Sender)
{
 if(VideoOutputFrame && VideoOutputFrame->CaptureThread)
  SelectVideoSourcePage(VideoOutputFrame->CaptureThread->GetSourceMode());
 FormShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoCaptureOptionsForm::CancelClick(TObject *Sender)
{
 Hide();
}
//---------------------------------------------------------------------------

