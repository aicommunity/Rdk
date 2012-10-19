//---------------------------------------------------------------------------

#ifndef UGEngineControlFormUnitH
#define UGEngineControlFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <ComCtrls.hpp>
#include <IniFiles.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>
#include "myrdk.h"
#include "TUVisualController.h"
#include "TUVisualControllerFormUnit.h"
//---------------------------------------------------------------------------
#pragma warn -8130
class TUGEngineControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TMainMenu *MainMenu;
	TImageList *ImageList;
	TToolBar *ToolBar;
	TMenuItem *File1;
	TMenuItem *LoadProjectItem;
	TMenuItem *SaveProjectItem;
	TMenuItem *N1;
	TMenuItem *OpenVideo1;
	TMenuItem *OpenImage1;
	TMenuItem *CaptureVideo1;
	TStatusBar *StatusBar;
	TMenuItem *Calculate1;
	TMenuItem *Start1;
	TMenuItem *Pause1;
	TMenuItem *Reset1;
	TMenuItem *Window1;
	TMenuItem *Images1;
	TMenuItem *EngineMonitor1;
	TMenuItem *VideoSource1;
	TMenuItem *ComponentsControl1;
	TMenuItem *ComponentsLinks1;
	TMenuItem *N2;
	TMenuItem *LoadModel1;
	TMenuItem *SaveModel1;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TToolButton *ToolButton4;
	TToolButton *ToolButton5;
	TToolButton *ToolButton6;
	TToolButton *ToolButton8;
	TToolButton *ToolButton9;
	TToolButton *ToolButton10;
	TToolButton *ToolButton11;
	TToolButton *ToolButton12;
	TToolButton *ToolButton13;
	TMenuItem *N3;
	TMenuItem *Step1;
	TToolButton *ToolButton14;
	TToolButton *ToolButton15;
	TToolButton *ToolButton7;
	TToolButton *ToolButton16;
	TMenuItem *Performance1;
	TOpenDialog *OpenDialog;
	TSaveDialog *SaveDialog;
	TMenuItem *CreateProjectItem;
	TMenuItem *CreateModel;
	TToolButton *ToolButton17;
	TToolButton *ToolButton18;
	TMenuItem *FavoriteInformation1;
	TMenuItem *DrawEngine1;
	TToolButton *ToolButton19;
	TToolButton *ToolButton21;
	TMenuItem *N4;
	TMenuItem *ReloadParameters1;
	TToolButton *ToolButton22;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Start1Click(TObject *Sender);
	void __fastcall Pause1Click(TObject *Sender);
	void __fastcall EngineMonitor1Click(TObject *Sender);
	void __fastcall Images1Click(TObject *Sender);
	void __fastcall VideoSource1Click(TObject *Sender);
	void __fastcall Step1Click(TObject *Sender);
	void __fastcall ComponentsControl1Click(TObject *Sender);
	void __fastcall ComponentsLinks1Click(TObject *Sender);
	void __fastcall LoadModel1Click(TObject *Sender);
	void __fastcall SaveModel1Click(TObject *Sender);
	void __fastcall OpenImage1Click(TObject *Sender);
	void __fastcall OpenVideo1Click(TObject *Sender);
	void __fastcall CaptureVideo1Click(TObject *Sender);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall Performance1Click(TObject *Sender);
	void __fastcall LoadProjectItemClick(TObject *Sender);
	void __fastcall SaveProjectItemClick(TObject *Sender);
	void __fastcall CreateProjectItemClick(TObject *Sender);
	void __fastcall CreateModelClick(TObject *Sender);
	void __fastcall FavoriteInformation1Click(TObject *Sender);
	void __fastcall DrawEngine1Click(TObject *Sender);
	void __fastcall ReloadParameters1Click(TObject *Sender);


private:	// User declarations
public:		// User declarations
	__fastcall TUGEngineControlForm(TComponent* Owner);

// Файл настроек проекта
RDK::Serialize::USerStorageXML ProjectXml;

// Файл настроек интефрейса
RDK::Serialize::USerStorageXML InterfaceXml;

// Признак наличия открытого проекта
bool ProjectOpenFlag;

// Путь до папки проекта
String ProjectPath;

// Имя файла проекта
String ProjectName;

// Флаг автоматического сохранения проекта
bool ProjectAutoSaveFlag;

// Флаг автоматического сохранения состояния модели
bool ProjectAutoSaveStateFlag;

// Число входов среды
int NumEnvInputs;

// Число выходов среды
int NumEnvOutputs;

// Разрешение изображений
int InputEnvImageWidth, InputEnvImageHeight;

// Индекс предварительно заданной модели
int PredefinedStructure;

// Шаг счета по умолчанию
int DefaultTimeStep;

// Глобальный шаг счета модели
int GlobalTimeStep;

// Обновление интерфейса
void AUpdateInterface(void);

// Метод, вызываемый перед шагом расчета
void ABeforeCalculate(void);

// Метод, вызываемый после шага расчета
void AAfterCalculate(void);

// Создает новый проект
void CreateProject(const String &FileName);

// Закрывает существущий проект
void CloseProject(void);

// Открывает проект
void OpenProject(const String &FileName);

// Сохраняет проект
void SaveProject(void);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUGEngineControlForm *UGEngineControlForm;
//---------------------------------------------------------------------------
#endif
