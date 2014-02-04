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
#include "TUVisualControllerFrameUnit.h"
#include "UEngineMonitorFrameUnit.h"
#include "UComponentsListFrameUnit.h"
#include "UComponentsPerformanceFrameUnit.h"
#include "UImagesFrameUnit.h"
#include "UWatchFrameUnit.h"
#include "UDrawEngineFrameUnit.h"
#include <Vcl.AppEvnts.hpp>
//---------------------------------------------------------------------------
#pragma warn -8130
class TUGEngineControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TMainMenu *MainMenu;
	TImageList *ImageList;
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
	TMenuItem *N3;
	TMenuItem *Step1;
	TMenuItem *Performance1;
	TOpenDialog *OpenDialog;
	TSaveDialog *SaveDialog;
	TMenuItem *CreateProjectItem;
	TMenuItem *CreateModel;
	TMenuItem *FavoriteInformation1;
	TMenuItem *DrawEngine1;
	TMenuItem *N4;
	TMenuItem *ReloadParameters1;
	TMenuItem *CopyProject1;
	TMenuItem *N5;
	TMenuItem *ProjectOptions1;
	TMenuItem *WatchWindow1;
	TTimer *HideTimer;
	TPanel *Panel2;
	TPanel *Panel4;
	TSplitter *Splitter2;
	TSplitter *Splitter3;
	TUComponentsListFrame *UComponentsListFrame1;
	TMenuItem *New1;
	TMenuItem *Images2;
	TMenuItem *Watches1;
	TPopupMenu *PageControlPopupMenu;
	TMenuItem *AddPage1;
	TMenuItem *DeletePage1;
	TMenuItem *RenamePage1;
	TMenuItem *Images3;
	TMenuItem *Watches2;
	TPageControl *PageControl1;
	TTabSheet *Draw;
	TUComponentsPerformanceFrame *UComponentsPerformanceFrame1;
	TUDrawEngineFrame *UDrawEngineFrame1;
	TPageControl *PageControl2;
	TTabSheet *LogsTabSheet;
	TTabSheet *TabSheet2;
	TMenuItem *N6;
	TMenuItem *Broadcasters1;
	TMenuItem *Servercontrol1;
	TStringGrid *ChannelsStringGrid;
	TMenuItem *Channels1;
	TMenuItem *AddNew1;
	TMenuItem *DeleteLast1;
	TMenuItem *DeleteAll1;
	TToolBar *ToolBar;
	TToolButton *ToolButton17;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TToolButton *ToolButton19;
	TToolButton *ToolButton14;
	TToolButton *ToolButton10;
	TToolButton *ToolButton12;
	TToolButton *ToolButton13;
	TToolButton *ToolButton16;
	TToolButton *ToolButton21;
	TToolButton *ToolButton15;
	TTrayIcon *TrayIcon;
	TApplicationEvents *ApplicationEvents;
	TPopupMenu *TrayPopupMenu;
	TMenuItem *Open1;
	TMenuItem *Hide1;
	TMenuItem *N7;
	TMenuItem *Start2;
	TMenuItem *Pause2;
	TMenuItem *Reset2;
	TMenuItem *N8;
	TMenuItem *Close1;
	TPopupMenu *ChannelsPopupMenu;
	TMenuItem *AddChannel1;
	TMenuItem *DelChannel1;
	TMenuItem *VideoRegistration1;
	void __fastcall FormShow(TObject *Sender);
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
	void __fastcall CopyProject1Click(TObject *Sender);
	void __fastcall ProjectOptions1Click(TObject *Sender);
	void __fastcall WatchWindow1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall HideTimerTimer(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall Images2Click(TObject *Sender);
	void __fastcall Watches1Click(TObject *Sender);
	void __fastcall DeletePage1Click(TObject *Sender);
	void __fastcall Images3Click(TObject *Sender);
	void __fastcall Watches2Click(TObject *Sender);
	void __fastcall UComponentsListFrame1GUI1Click(TObject *Sender);
	void __fastcall UDrawEngineFrame1GUI1Click(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall DrawShow(TObject *Sender);
	void __fastcall Servercontrol1Click(TObject *Sender);
	void __fastcall ChannelsStringGridClick(TObject *Sender);
	void __fastcall ChannelsStringGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall AddNew1Click(TObject *Sender);
	void __fastcall DeleteLast1Click(TObject *Sender);
	void __fastcall DeleteAll1Click(TObject *Sender);
	void __fastcall ApplicationEventsMinimize(TObject *Sender);
	void __fastcall TrayIconDblClick(TObject *Sender);
	void __fastcall Open1Click(TObject *Sender);
	void __fastcall Hide1Click(TObject *Sender);
	void __fastcall Close1Click(TObject *Sender);
	void __fastcall Start2Click(TObject *Sender);
	void __fastcall Pause2Click(TObject *Sender);
	void __fastcall Reset2Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ApplicationEventsRestore(TObject *Sender);
	void __fastcall AddChannel1Click(TObject *Sender);
	void __fastcall DelChannel1Click(TObject *Sender);
	void __fastcall VideoRegistration1Click(TObject *Sender);






private:	// User declarations
public:		// User declarations
	__fastcall TUGEngineControlForm(TComponent* Owner);

String MainFormName;
bool HideAdminFormFlag;
String AutoexecProjectFileName;
bool AutoStartProjectFlag;
bool MinimizeToTray;
bool StartMinimized;
String ProgramName;

// ���� �������� �������
RDK::USerStorageXML ProjectXml;

// ���� �������� ����������
RDK::USerStorageXML InterfaceXml;

// ������� ������� ��������� �������
bool ProjectOpenFlag;

// ���� �� ����� �������
String ProjectPath;

// ��� ����� �������
String ProjectFileName;

// ��� �������
String ProjectName;

// �������� �������
String ProjectDescription;

// ��� �������
// 0 - ������� ������
// 1 - ��������� ������
int ProjectMode;

// ���� ��������������� ���������� �������
bool ProjectAutoSaveFlag;

// ���� ��������������� ���������� ��������� ������
bool ProjectAutoSaveStateFlag;

// ����� ������ �����
int NumEnvInputs;

// ����� ������� �����
int NumEnvOutputs;

// ���������� �����������
int InputEnvImageWidth, InputEnvImageHeight;

// ������ �������������� �������� ������
std::vector<int> PredefinedStructure;

// ��� ����� �� ���������
std::vector<int> DefaultTimeStep;

// ���������� ��� ����� ������
std::vector<int> GlobalTimeStep;

// ���� ������������� ������ ������������� ����� ��������
std::vector<int> InitAfterLoadFlag;

// ���� ������������� ������ ������ ����� ��������
std::vector<int> ResetAfterLoadFlag;

// ���� ��������� ����������� ������
std::vector<bool> DebugModeFlag;

// ���� ������������� �������������� ������� �����������
bool ReflectionFlag;

// ����� �����
std::vector<int> CalculationMode;

/// ����������� �������� ����� ���������� �����, ��
std::vector<long long> MinInterstepsInterval;

// ������ ����������� ���� (�� ������� ���������� ���������� � ������)
std::map<std::string, TUVisualControllerForm*> SpecialForms;

/// �������� TVideoGrabber
String VideoGrabberLicenseString;

/// ������ ��������� �������� ��������
std::list<std::string> LastProjectsList;

/// ������ ������� ��������� �������� ��������
int LastProjectsListMaxSize;

Word Saved8087CW;

bool AppWinState;

// ���������� ����������
void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// �����, ���������� ����� ����� �������
void ABeforeCalculate(void);

// �����, ���������� ����� ���� �������
void AAfterCalculate(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::USerStorageXML &xml);

// ������� ����� ������
void CreateProject(const String &FileName, const String &model_comp_name="", const String &model_file_name="");

// ��������� ����������� ������
void CloseProject(void);

// ��������� ������
void OpenProject(const String &FileName);

// ��������� ������ � �������� source_id, � ������ � �������� cloned_id
void CloneProject(int source_id, int cloned_id);

// ��������� ������
void SaveProject(void);

// ��������� �������� ����� ���� TUImagesFrame, TUWatchFrame � �.�. �� �������� �������
bool AddSpecialFrameToPage(TUVisualControllerFrame *frame, TTabSheet* tab, const String &caption);

// ��������� �������� ����� �� �������� �������
bool AddSpecialFormToPage(TUVisualControllerForm *form, TTabSheet* tab, const String &caption);

// ������� ����� ������� � �������� ������ ��� ����������� �������
// type - ��������� ��� ���� ������ �� �������
TTabSheet* AddSpecialFramePage(const String &type, const String &caption);

// ������� ����� ������� � �������� ������ ��� ����������� ����
TTabSheet* AddSpecialFormPage(const String &type);

// ���� ������� � �������� ������ ��� �������� ����������� �����
TTabSheet* FindSpecialFormPage(const String &type, TUVisualControllerForm* &form);

// ������� ����� ������� � �������� ������ ��� ����� ���������� �����������
// ���� ���� ���������� ��� ���������, �� ���������� ��������� �� ����� �������
TTabSheet* AddComponentControlFormPage(const string &component_name);

/// ���� � ���������� ��������� �� ����� ��� �����, ��������������� ������� � �������� ��������
void FindVisualController(int index, TUVisualControllerFrame* &frame, TUVisualControllerForm* &form);

// ������� ��������
void DelPage(int index);

// ��������������� ��������� ��������
void RenamePage(int index, String new_name);

// ����, ���������� �� ��� �������, ����������� ����������� � �������� ������
// ���������� ������ ������� ��� -1, ���� ������� �� �������
int FindComponentControlPage(const std::string &component_long_name);

// ������� ��� ������ ������� (��������� 2 ���������)
void ClearPages(void);

/// ��������� ����� ����� � ������� �������� ������� ���������
void AddBroadcasterMenu(TMenuItem *item, TMenu *owner);

/// ��������� ������� �������� �� �����
void LoadProjectsHistory(void);

/// ��������� ������� �������� � ����
void SaveProjectsHistory(void);

};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUGEngineControlForm *UGEngineControlForm;
//---------------------------------------------------------------------------
#endif
