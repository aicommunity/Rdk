#ifndef RDK_VIDEO
#define RDK_VIDEO
#endif
//---------------------------------------------------------------------------

#ifndef VideoOutputFormUnitH
#define VideoOutputFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "VideoOutputFrameUnit.h"
#include <Menus.hpp>
#include <IniFiles.hpp>
#include <Vcl.ComCtrls.hpp>
#include "TUVisualController.h"
#include "TUVisualControllerFormUnit.h"
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
//---------------------------------------------------------------------------
class TVideoOutputForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TMainMenu *MainMenu;
	TToolBar *ToolBar1;
	TImageList *ImageList;
	TMenuItem *Sources1;
	TMenuItem *AddSource1;
	TMenuItem *DelSource1;
	TMenuItem *ClearSources1;
	TPopupMenu *PopupMenu;
	TMenuItem *AddSource2;
	TMenuItem *DelSource2;
	TMenuItem *ClearSource1;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	void __fastcall PageControlChange(TObject *Sender);
	void __fastcall AddSource1Click(TObject *Sender);
	void __fastcall DelSource1Click(TObject *Sender);
	void __fastcall ClearSources1Click(TObject *Sender);
	void __fastcall AddSource2Click(TObject *Sender);
	void __fastcall DelSource2Click(TObject *Sender);
	void __fastcall ClearSource1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoOutputForm(TComponent* Owner);
	virtual __fastcall ~TVideoOutputForm(void);

// Источники видео
std::vector<TVideoOutputFrame*> Sources;

// Метод, вызываемый перед шагом расчета
void ABeforeCalculate(void);

// Обновляет интерфейс
void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::USerStorageXML &xml);

// Создание копии этого компонента
virtual TVideoOutputForm* New(TComponent *owner=0);

// Число источников видео
int GetNumSources(void) const;

// Добавляет новый источник видео
void AddSource(void);

// Удаляет источник видео
void DelSource(int index);

// Удаляет все источники видео
void ClearSources(void);

// Выбирает режим заданного источника
void SetSourceType(int index, int mode);

// Возвращает индекс текущего активного (выбранного) источника видео
int GetActiveSource(void) const;

// Возвращает фрейм источника видео
TVideoOutputFrame* GetVideoOutputFrame(int index);

// Возвращает фрейм активного (выбранного) источника видео
TVideoOutputFrame* GetActiveVideoOutputFrame(void);

// Сохраняет информацию об источниках данных в заданный ini файл
void SaveToIni(TMemIniFile *ini, const String &section);

// Загружает информацию об источниках данных из заданного ini файла
void LoadFromIni(TMemIniFile *ini, const String &section);

// Запускает выбранный источник видео, или все если index == -1
void Start(int index=-1);

// Останавливает выбранный источник видео, или все если index == -1
void Stop(int index=-1);

// Останавливает выбранный источник видео, или все если index == -1
// Не изменяяет состояние онлайн-источников (камеры)
void StopOffline(int index=-1);

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputForm *VideoOutputForm;
//---------------------------------------------------------------------------
#endif
