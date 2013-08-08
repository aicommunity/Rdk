//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VideoOutputFormUnit.h"
#include "VideoOutputToolsFormUnit.h"
#include "TVideoGrabberControlFormUnit.h"
#include "rdk_initdll.h"
#include "UGEngineControlFormUnit.h"
//#include "USharedMemoryLoader.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFormUnit"
#pragma resource "*.dfm"
TVideoOutputForm *VideoOutputForm;
//---------------------------------------------------------------------------
__fastcall TVideoOutputForm::TVideoOutputForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

__fastcall TVideoOutputForm::~TVideoOutputForm(void)
{

}


// Метод, вызываемый перед шагом расчета
void TVideoOutputForm::ABeforeCalculate(void)
{
/* if(!Model_Check())
  return;

 int num_outputs=Model_GetComponentNumOutputs("");
 for(int i=0;i<num_outputs;i++)
 {
  if(!GetVideoOutputFrame(i))
   continue;
  RDK::UBitmap &source=GetVideoOutputFrame(i)->BmpSource;

  if(source.GetByteLength()>0)
  {
   Model_SetComponentBitmapOutput("", i, &source,true);

   //Env_SetInputRes(i, InputEnvImageWidth, InputEnvImageHeight);
   //Env_SetInputImage(i,(unsigned char*)source.GetData(),source.GetWidth(), source.GetHeight(),source.GetColorModel());
  }
 }    */
}

// Обновляет интерфейс
void TVideoOutputForm::AUpdateInterface(void)
{
}

// Сохраняет параметры интерфейса в xml
void TVideoOutputForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("NumSources",GetNumSources());
}

// Загружает параметры интерфейса из xml
void TVideoOutputForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 int num=xml.ReadInteger("NumSources",1);
 ClearSources();
 for(int i=0;i<num;i++)
  AddSource();
 UpdateInterface();
}

// Создание копии этого компонента
TVideoOutputForm* TVideoOutputForm::New(TComponent *owner)
{
 return new TVideoOutputForm(owner);
}

// Число источников видео
int TVideoOutputForm::GetNumSources(void) const
{
 return PageControl->PageCount;
}

// Добавляет новый источник видео
void TVideoOutputForm::AddSource(void)
{
 TTabSheet *sheet=new TTabSheet(PageControl);
 sheet->PageControl=PageControl;
 sheet->PageIndex=PageControl->PageCount-1;
 Sources.resize(PageControl->PageCount);

 size_t index=Sources.size()-1;

 Sources[index]=new TVideoOutputFrame(0);
 Sources[index]->FrameIndex=index;
 Sources[index]->Name=Sources[index]->Name+String("_")+PageControl->PageCount;
 Sources[index]->MyVideoGrabberControlForm->Name=Sources[index]->Name+"_VideoGrabberControlForm";

 InsertComponent(Sources[index]);
 Sources[index]->Parent=sheet;
 Sources[index]->Align=alClient;
 PageControl->Pages[index]->Caption=IntToStr(int(index));
 if(index == 0)
  Sources[index]->VideoGrabber->SynchronizationRole=sr_Master;
 else
  Sources[index]->VideoGrabber->SynchronizationRole=sr_Slave;
}

// Удаляет источник видео
void TVideoOutputForm::DelSource(int index)
{
 if(index<0 || index >=int(Sources.size()))
  return;

 delete Sources[index];
 Sources.erase(Sources.begin()+index);
 delete PageControl->Pages[index];
// for(int i=0;i<PageControl->PageCount;i++)
//  Sources[i]->Parent=PageControl->Pages[i];
}

// Удаляет все источники видео
void TVideoOutputForm::ClearSources(void)
{
 for(size_t i=0;i<Sources.size();i++)
 {
  delete Sources[i];
  delete PageControl->Pages[0];
 }

 Sources.clear();
}

// Выбирает режим заданного источника
void TVideoOutputForm::SetSourceType(int index, int mode)
{
 if(index<0 || index >=int(Sources.size()))
  return;

 Sources[index]->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(mode);
}

// Возвращает индекс текущего активного источника видео
int TVideoOutputForm::GetActiveSource(void) const
{
 return PageControl->ActivePageIndex;
}

// Возвращает фрейм источника видео
TVideoOutputFrame* TVideoOutputForm::GetVideoOutputFrame(int index)
{
 if(index<0 || index >=int(Sources.size()))
  return 0;

 return Sources[index];
}

// Возвращает фрейм активного (выбранного) источника видео
TVideoOutputFrame* TVideoOutputForm::GetActiveVideoOutputFrame(void)
{
 if(GetActiveSource()>=0)
  return Sources[GetActiveSource()];

 return 0;
}


// Сохраняет информацию об источниках данных в заданный ini файл
void TVideoOutputForm::SaveToIni(TMemIniFile *ini, const String &section)
{
// ini->WriteInteger(section,"NumSources",GetNumSources());
 for(int i=0;i<GetNumSources();i++)
 {
  //Sources[i]->MyVideoGrabberControlForm->VideoGrabberControlFrame->SaveToIni(ini,section+IntToStr(i));
 }
}

// Загружает информацию об источниках данных из заданного ini файла
void TVideoOutputForm::LoadFromIni(TMemIniFile *ini, const String &section)
{
// int numsources=ini->ReadInteger(section,"NumSources",0);
// ClearSources();
 for(int i=0;i<GetNumSources();i++)
 {
//  AddSource();
 // Sources[i]->MyVideoGrabberControlForm->VideoGrabberControlFrame->LoadFromIni(ini,section+IntToStr(i));
 }

}

// Запускает выбранный источник видео, или все если index == -1
void TVideoOutputForm::Start(int index)
{
 if(index>=0 && index<GetNumSources())
  Sources[index]->StartButtonClick(this);
 else
 {
//  if(GetNumSources()>0)
//   Sources[i]->
  for(int i=0;i<GetNumSources();i++)
   Sources[i]->StartButtonClick(this);
 }
}

// Останавливает выбранный источник видео, или все если index == -1
void TVideoOutputForm::Stop(int index)
{
 if(index>=0 && index<GetNumSources())
  Sources[index]->StopButtonClick(this);
 else
  for(int i=0;i<GetNumSources();i++)
   Sources[i]->StopButtonClick(this);
}

// Останавливает выбранный источник видео, или все если index == -1
// Не изменяяет состояние онлайн-источников (камеры)
void TVideoOutputForm::StopOffline(int index)
{
 if(index>=0 && index<GetNumSources())
 {
  if(Sources[index]->Mode != 2 && Sources[index]->Mode != 3)
   Sources[index]->StopButtonClick(this);
 }
 else
  for(int i=0;i<GetNumSources();i++)
   if(Sources[i]->Mode != 2 && Sources[i]->Mode != 3)
    Sources[i]->StopButtonClick(this);
}


//---------------------------------------------------------------------------
void __fastcall TVideoOutputForm::PageControlChange(TObject *Sender)
{
 for(int i=0;i<PageControl->PageCount;i++)
  if(PageControl->Pages[i]->Visible == false)
  {
   Sources[i]->MyVideoGrabberControlForm->Hide();
   Sources[i]->MyVideoOutputToolsForm->Hide();
  }

}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::AddSource1Click(TObject *Sender)
{
 AddSource();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::DelSource1Click(TObject *Sender)
{
 if(GetNumSources()<=0)
  return;

 DelSource(PageControl->ActivePageIndex);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::ClearSources1Click(TObject *Sender)
{
 ClearSources();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::AddSource2Click(TObject *Sender)
{
 AddSource1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::DelSource2Click(TObject *Sender)
{
 DelSource1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::ClearSource1Click(TObject *Sender)
{
 ClearSources1Click(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputForm::FormCreate(TObject *Sender)
{
 UGEngineControlForm->SpecialForms["TVideoOutputForm"]=this;
 int res=LoadUSharedMemoryLibrary("USharedMemory.dll");
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::FormDestroy(TObject *Sender)
{
 UnLoadUSharedMemoryLibrary();
// UGEngineControlForm->SpecialForms.erase("TVideoOutputForm");
}
//---------------------------------------------------------------------------

