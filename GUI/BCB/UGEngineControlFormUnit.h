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
#include "../../Deploy/Include/rdk_cpp_initdll.h"
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
// #include <Web.Win.Sockets.hpp>

#include <System.ImageList.hpp>

#define WM_SERVER_PING (WM_USER + 261)
#define WM_SERVER_PONG (WM_USER + 262)

extern String Lang_SaveProjectTitle;
extern String Lang_LoadProjectTitle;
extern String Lang_LoadingData;
extern String Lang_SavingData;
extern String Lang_Total;
extern String Lang_SaveInterface;
extern String Lang_UpdateInterface;
extern String Lang_Starting;
extern String Lang_Starting;
extern String Lang_ApplicationRun;
extern String Lang_ApplicationClose;
extern String Lang_StartupDelay;
extern String Lang_StartupDelaySuffix;


namespace RDK {
void CreateStatusWindow(const String &Text);
void RemoveStatusWindow(void);
}

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
	TMenuItem *N9;
	TMenuItem *SaveCommonDescriptions1;
	TMenuItem *SaveClassesDescriptions1;
	TMenuItem *Insert1;
	TMenuItem *DeleteSelected1;
	TMenuItem *InsertChannel1;
	TMenuItem *DeleteSelectedChannel1;
	TMenuItem *ApplicationOptions1;
	TMenuItem *ClassesDescription1;
	TMenuItem *N10;
	TMenuItem *StartChannel1;
	TMenuItem *PauseChannel1;
	TMenuItem *ResetChannel1;
	TApplicationEvents *ApplicationEvents;
	TPanel *Panel3;
	TCheckBox *ShowDebugMessagesCheckBox;
	TCheckBox *AutoupdatePropertiesCheckBox;
	TCheckBox *AutosaveStatesCheckBox;
	TCheckBox *AutosaveProjectCheckBox;
	TRadioButton *MultiThreadedRadioButton;
	TRadioButton *SingleThreadedRadioButton;
	TCheckBox *DetailedDebugLogCheckBox;
	TMenuItem *OpenProjectFolder1;
	TMenuItem *ClonetoNewChannel1;
	TMenuItem *VideoSources21;
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
	void __fastcall TrayIconDblClick(TObject *Sender);
	void __fastcall Open1Click(TObject *Sender);
	void __fastcall Hide1Click(TObject *Sender);
	void __fastcall Close1Click(TObject *Sender);
	void __fastcall Start2Click(TObject *Sender);
	void __fastcall Pause2Click(TObject *Sender);
	void __fastcall Reset2Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall AddChannel1Click(TObject *Sender);
	void __fastcall DelChannel1Click(TObject *Sender);
	void __fastcall VideoRegistration1Click(TObject *Sender);
	void __fastcall SaveCommonDescriptions1Click(TObject *Sender);
	void __fastcall SaveClassesDescriptions1Click(TObject *Sender);
	void __fastcall UComponentsListFrame1StringGridMouseEnter(TObject *Sender);
	void __fastcall UComponentsListFrame1NiceParamValRichEditMouseEnter(TObject *Sender);
	void __fastcall EnchancedSG1BasicStringGridMouseEnter(TObject *Sender);
	void __fastcall EnchancedSG2BasicStringGridMouseEnter(TObject *Sender);
	void __fastcall UComponentsListFrame1NiceStateValRichEditMouseEnter(TObject *Sender);
	void __fastcall Insert1Click(TObject *Sender);
	void __fastcall DeleteSelected1Click(TObject *Sender);
	void __fastcall InsertChannel1Click(TObject *Sender);
	void __fastcall DeleteSelectedChannel1Click(TObject *Sender);
	void __fastcall ApplicationOptions1Click(TObject *Sender);
	void __fastcall ClassesDescription1Click(TObject *Sender);
	void __fastcall StartChannel1Click(TObject *Sender);
	void __fastcall PauseChannel1Click(TObject *Sender);
	void __fastcall ResetChannel1Click(TObject *Sender);
	void __fastcall ToolButton15Click(TObject *Sender);
	void __fastcall UDrawEngineFrame1Reset1Click(TObject *Sender);
	void __fastcall UDrawEngineFrame1Calculate1Click(TObject *Sender);
	void __fastcall ApplicationEventsException(TObject *Sender, Exception *E);
	void __fastcall ShowDebugMessagesCheckBoxClick(TObject *Sender);
	void __fastcall AutoupdatePropertiesCheckBoxClick(TObject *Sender);
	void __fastcall AutosaveProjectCheckBoxClick(TObject *Sender);
	void __fastcall AutosaveStatesCheckBoxClick(TObject *Sender);
	void __fastcall MultiThreadedRadioButtonClick(TObject *Sender);
	void __fastcall SingleThreadedRadioButtonClick(TObject *Sender);
	void __fastcall DetailedDebugLogCheckBoxClick(TObject *Sender);
	void __fastcall OpenProjectFolder1Click(TObject *Sender);
	void __fastcall ClonetoNewChannel1Click(TObject *Sender);
	void __fastcall VideoSources21Click(TObject *Sender);
	//void __fastcall UComponentsListFrame1StringGridClick(TObject *Sender);



private:	// User declarations
public:		// User declarations
	__fastcall TUGEngineControlForm(TComponent* Owner);

void __fastcall WMSysCommand(TMessage &Msg);
void __fastcall WMServerPing(TMessage &Msg);
BEGIN_MESSAGE_MAP
MESSAGE_HANDLER(WM_SYSCOMMAND, TMessage, WMSysCommand)
MESSAGE_HANDLER(WM_SERVER_PING, TMessage, WMServerPing)
END_MESSAGE_MAP(TForm)

String MainFormName;
bool HideAdminFormFlag;
String AutoexecProjectFileName;
bool AutoStartProjectFlag;
bool MinimizeToTray;
bool StartMinimized;
String ProgramName;
String LogDir;
bool LogDebugMode;
int StartupDelay;

__int64 LastMaxMemory;
__int64 LastMaxBlock;

// ���� �������� �������
//RDK::USerStorageXML ProjectXml;

// ���� �������� ����������
//RDK::USerStorageXML InterfaceXml;

// ������� ������� ��������� �������
//bool ProjectOpenFlag;

// ���� �� ����� �������
//String ProjectPath;

// ��� ����� �������
//String ProjectFileName;

// ��� �������
//String ProjectName;

// �������� �������
//String ProjectDescription;

// ��� �������
// 0 - ������� ������
// 1 - ��������� ������
//int ProjectMode;

// ���� ��������������� ���������� �������
//bool ProjectAutoSaveFlag;

// ���� ��������������� ���������� ��������� ������
//bool ProjectAutoSaveStateFlag;

// ���� ����������� ��������� ������� ������ �� �� ��������
//bool ProjectShowChannelsStates;

// ����� ������ �����
//int NumEnvInputs;

// ����� ������� �����
//int NumEnvOutputs;

/// ���� ���������� ������������ ������� �������
//bool EventsLogEnabled;

// ���������� �����������
//int InputEnvImageWidth, InputEnvImageHeight;

// ������ �������������� �������� ������
//std::vector<int> PredefinedStructure;

// ��� ����� �� ���������
//std::vector<int> DefaultTimeStep;

// ���������� ��� ����� ������
//std::vector<int> GlobalTimeStep;

// ���� ������������� ������ ������������� ����� ��������
//std::vector<int> InitAfterLoadFlag;

// ���� ������������� ������ ������ ����� ��������
//std::vector<int> ResetAfterLoadFlag;

// ���� ��������� ����������� ������
//std::vector<bool> DebugModeFlag;

// ���� ������������� �������������� ������� �����������
//bool ReflectionFlag;

// ����� �����
//std::vector<int> CalculationMode;

/// ����������� �������� ����� ���������� �����, ��
//std::vector<long long> MinInterstepsInterval;

/// ����� ���������� ���������� ��������
//std::string ServerInterfaceAddress;

/// ���� ���������� ���������� ��������
//int ServerInterfacePort;

/// ���� ������� �� ������ ������� ����� �� Ctrl+F1
bool DisableAdminForm;

/// ����, ����������� ��������� ��������������� ��� ��������� �������
bool DisableStopVideoSources;

// ������ ����������� ���� (�� ������� ���������� ���������� � ������)
std::map<std::string, TUVisualControllerForm*> SpecialForms;

/// �������� TVideoGrabber
String VideoGrabberLicenseString;

/// ������ ��
String VersionString;

/// ������ ��������� �������� ��������
//std::list<std::string> LastProjectsList;

/// ������ ������� ��������� �������� ��������
//int LastProjectsListMaxSize;

Word Saved8087CW;

bool AppWinState;

TForm* CaptureControlForm;

void __fastcall AppMinimize(TObject *Sender);
void __fastcall AppRestore(TObject *Sender);

// ��������� ������ ��������� ����� �� xml
void LoadFormPosition(RDK::USerStorageXML &xml);

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
void CreateProject(const std::string &file_name, RDK::TProjectConfig &project_config);

// ��������� ����������� ������
void CloseProject(void);

// ��������� ������
void OpenProject(const String &FileName);

// ��������� ������ � �������� source_id, � ������ � �������� cloned_id
void CloneProject(int source_id, int cloned_id);

// ��������� ������
void SaveProject(void);

/// --------------------------
//��������� ����� ��������������� �����������

void RegisterVideoCaptureControlForm (TForm *form);

void UnRegisterVideoCaptureControlForm (TForm *form);

/// ������ ���������� ��������
/// --------------------------
/// ���������� ����� �������
int GetNumChannels(void) const;

/// ������������� ����� �������
/// ����� ���������� ����� ���������� �����
int SetNumChannels(int value);

/// ��������� ����� � ������� index
/// ���� ������� �� ����������, �� ��������� � �����
int AddChannel(int index);

/// ������� ����� �� ������� index
int DelChannel(int index);
/// --------------------------

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

/// ��������� ����� (����������) ����� � ���� Window
void AddGlobalWindowMenu(TMenuItem *item, TMenu *owner);

/// ������ ���������� ������
/// ���� channel_index == -1 �� ��������� ��� ������
virtual void StartChannel(int channel_index);

/// ������� ���������� ������
/// ���� channel_index == -1 �� ������������� ��� ������
virtual void PauseChannel(int channel_index);

/// ����� ���������� ������
/// ���� channel_index == -1 �� ���������� ��� ������
virtual void ResetChannel(int channel_index);

/// ������������ ������ � ���������� ����������� �� ������ ������������ ������
String DebugGenerateMemoryUsageString(void);

};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUGEngineControlForm *UGEngineControlForm;
//---------------------------------------------------------------------------
#endif
