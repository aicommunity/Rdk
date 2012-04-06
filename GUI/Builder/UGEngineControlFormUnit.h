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
//---------------------------------------------------------------------------
class TUGEngineControlForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TTimer *Timer;
	TMainMenu *MainMenu;
	TImageList *ImageList;
	TToolBar *ToolBar;
	TMenuItem *File1;
	TMenuItem *LoadProject;
	TMenuItem *SaveProject;
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
	void __fastcall FormShow(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
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


private:	// User declarations
public:		// User declarations
	__fastcall TUGEngineControlForm(TComponent* Owner);

// Признак обновления интерфейса
bool UpdateInterfaceFlag;

// Файл настроек программы
TMemIniFile *ProjectIni;

// Обновление интерфейса
void UpdateInterface(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUGEngineControlForm *UGEngineControlForm;
//---------------------------------------------------------------------------
#endif
