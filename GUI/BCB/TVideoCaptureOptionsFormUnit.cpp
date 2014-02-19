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
std::map<int, TFrame* > TVideoCaptureOptionsForm::VideoSourceOptionsFrames;


//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsForm::TVideoCaptureOptionsForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 VideoCaptureOptionsInterface=0;

 VideoOutputFrame=0;
}
//---------------------------------------------------------------------------
// ---------------------------
// Методы управления поддерживаемыми источниками видео
// ---------------------------
/// Возвращает список настройщиков источников видео
std::map<int, TFrame* > TVideoCaptureOptionsForm::GetVideoSourceOptionsFrames(void)
{
 return VideoSourceOptionsFrames;
}

/// Добавляет новый настройщик источников видео
bool TVideoCaptureOptionsForm::AddVideoSourceOptionsFrame(int mode, TFrame *frame)
{
 const std::map<int, TFrame*>::iterator I=VideoSourceOptionsFrames.find(mode);
 if(I != VideoSourceOptionsFrames.end())
  return false;
 VideoSourceOptionsFrames[mode]=frame;
 return true;
}

/// Проверяет, существует ли такой настройщик
bool TVideoCaptureOptionsForm::CheckVideoSourceOptionsFrame(int mode)
{
 const std::map<int, TFrame*>::iterator I=VideoSourceOptionsFrames.find(mode);
 if(I != VideoSourceOptionsFrames.end())
  return false;
 return true;
}

/// Очищает список поддерживаемых настройщиков видео
void TVideoCaptureOptionsForm::ClearAllVideoSourceOptionsFrames(void)
{
 std::map<int, TFrame*>::iterator I=VideoSourceOptionsFrames.begin();
 while(I != VideoSourceOptionsFrames.end())
 {
  delete I->second;
 }
 VideoSourceOptionsFrames.clear();
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
	TVideoOutputFrame::GetVideoSourcePrototypes();


 while(PageControl->PageCount < prototypes.size())
 {
  TTabSheet *tab=new TTabSheet(PageControl);
  tab->PageControl=PageControl;
 }

 std::map<int, TFrame*>::const_iterator I=VideoSourceOptionsFrames.begin();
 int i=0;
 for(;I != VideoSourceOptionsFrames.end();++I, i++)
 {
  TTabSheet *tab=PageControl->Pages[i];
//  frame->Name=frame->Name+String("_")+count;
//  InsertComponent(frame);
  I->second->Parent=tab;
  I->second->Align=alClient;

  tab->Caption=IntToStr(I->first);//I->second->Caption;
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
