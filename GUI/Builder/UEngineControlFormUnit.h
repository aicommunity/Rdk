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
class TUEngineControlForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TTimer *Timer;
	TMainMenu *MainMenu;
	TImageList *ImageList;
	TToolBar *ToolBar;
	TMenuItem *File1;
	TMenuItem *LoadProjectItem;
	TMenuItem *SaveProjectItem;
	TMenuItem *N1;
	TStatusBar *StatusBar;
	TMenuItem *Calculate1;
	TMenuItem *Start1;
	TMenuItem *Pause1;
	TMenuItem *Reset1;
	TMenuItem *Window1;
	TMenuItem *EngineMonitor1;
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
	TToolButton *ToolButton10;
	TToolButton *ToolButton12;
	TToolButton *ToolButton13;
	TMenuItem *N3;
	TMenuItem *Step1;
	TToolButton *ToolButton14;
	TToolButton *ToolButton15;
	TToolButton *ToolButton7;
	TToolButton *ToolButton6;
	TMenuItem *Performance1;
	TSaveDialog *SaveDialog;
	TOpenDialog *OpenDialog;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Start1Click(TObject *Sender);
	void __fastcall Pause1Click(TObject *Sender);
	void __fastcall EngineMonitor1Click(TObject *Sender);
	void __fastcall Step1Click(TObject *Sender);
	void __fastcall ComponentsControl1Click(TObject *Sender);
	void __fastcall ComponentsLinks1Click(TObject *Sender);
	void __fastcall LoadModel1Click(TObject *Sender);
	void __fastcall SaveModel1Click(TObject *Sender);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall Performance1Click(TObject *Sender);
	void __fastcall LoadProjectItemClick(TObject *Sender);
	void __fastcall SaveProjectItemClick(TObject *Sender);


private:	// User declarations
public:		// User declarations
	__fastcall TUEngineControlForm(TComponent* Owner);

// ������� ���������� ����������
bool UpdateInterfaceFlag;

// ���� �������� ���������
TMemIniFile *ProjectIni;

// ���� �� ����� �������
String ProjectPath;

// ��� ����� �������
String ProjectName;

// ���� ��������������� ���������� �������
bool ProjectAutoSaveFlag;

// ����� ������ �����
int NumEnvInputs;

// ����� ������� �����
int NumEnvOutputs;

// ���������� �����������
int InputEnvImageWidth, InputEnvImageHeight;

// ���������� ����������
void UpdateInterface(void);

// ������� ����� ������
void CreateProject(const String &FileName);

// ��������� ����������� ������
void CloseProject(void);

// ��������� ������
void OpenProject(const String &FileName);

// ��������� ������
void SaveProject(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUEngineControlForm *UEngineControlForm;
//---------------------------------------------------------------------------
#endif
