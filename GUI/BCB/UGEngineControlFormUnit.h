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

// Файл настроек проекта
//RDK::USerStorageXML ProjectXml;

// Файл настроек интефрейса
//RDK::USerStorageXML InterfaceXml;

// Признак наличия открытого проекта
//bool ProjectOpenFlag;

// Путь до папки проекта
//String ProjectPath;

// Имя файла проекта
//String ProjectFileName;

// Имя проекта
//String ProjectName;

// Описание проекта
//String ProjectDescription;

// Тип проекта
// 0 - Обычный проект
// 1 - Серверный проект
//int ProjectMode;

// Флаг автоматического сохранения проекта
//bool ProjectAutoSaveFlag;

// Флаг автоматического сохранения состояния модели
//bool ProjectAutoSaveStateFlag;

// Флаг отображения состояний каналов справа от их индексов
//bool ProjectShowChannelsStates;

// Число входов среды
//int NumEnvInputs;

// Число выходов среды
//int NumEnvOutputs;

/// Флаг разрешения логгирования событий сервера
//bool EventsLogEnabled;

// Разрешение изображений
//int InputEnvImageWidth, InputEnvImageHeight;

// Индекс предварительно заданной модели
//std::vector<int> PredefinedStructure;

// Шаг счета по умолчанию
//std::vector<int> DefaultTimeStep;

// Глобальный шаг счета модели
//std::vector<int> GlobalTimeStep;

// Флаг необходимости вызова инициализации после загрузки
//std::vector<int> InitAfterLoadFlag;

// Флаг необходимости вызова сброса после загрузки
//std::vector<int> ResetAfterLoadFlag;

// Флаг включения отладочного режима
//std::vector<bool> DebugModeFlag;

// Флаг необходимости переворачивать входные изображения
//bool ReflectionFlag;

// Режим счета
//std::vector<int> CalculationMode;

/// Минимальный интервал между итерациями счета, мс
//std::vector<long long> MinInterstepsInterval;

/// Адрес интерфейса управления сервером
//std::string ServerInterfaceAddress;

/// Порт интерфейса управления сервером
//int ServerInterfacePort;

/// Флаг запрета на запуск главной формы по Ctrl+F1
bool DisableAdminForm;

/// Флаг, запрещающий остановку видеоисточников при остановке расчета
bool DisableStopVideoSources;

// Список специальных форм (не имеющих компонента управления в модели)
std::map<std::string, TUVisualControllerForm*> SpecialForms;

/// Лицензия TVideoGrabber
String VideoGrabberLicenseString;

/// Версия ПО
String VersionString;

/// Список последних открытых проектов
//std::list<std::string> LastProjectsList;

/// Размер истории последних открытых проектов
//int LastProjectsListMaxSize;

Word Saved8087CW;

bool AppWinState;

TForm* CaptureControlForm;

void __fastcall AppMinimize(TObject *Sender);
void __fastcall AppRestore(TObject *Sender);

// Загружает данные положения формы из xml
void LoadFormPosition(RDK::USerStorageXML &xml);

// Обновление интерфейса
void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Метод, вызываемый перед шагом расчета
void ABeforeCalculate(void);

// Метод, вызываемый после шага расчета
void AAfterCalculate(void);

// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::USerStorageXML &xml);

// Создает новый проект
void CreateProject(const std::string &file_name, RDK::TProjectConfig &project_config);

// Закрывает существущий проект
void CloseProject(void);

// Открывает проект
void OpenProject(const String &FileName);

// Загружает проект с индексом source_id, в движок с индексом cloned_id
void CloneProject(int source_id, int cloned_id);

// Сохраняет проект
void SaveProject(void);

/// --------------------------
//открывает форму многоканального отображения

void RegisterVideoCaptureControlForm (TForm *form);

void UnRegisterVideoCaptureControlForm (TForm *form);

/// Методы управления каналами
/// --------------------------
/// Возвращает число каналов
int GetNumChannels(void) const;

/// Устанавливает число каналов
/// также выставляет число источников видео
int SetNumChannels(int value);

/// Добавляет канал в позицию index
/// Если позиция не существует, то добавляет в конец
int AddChannel(int index);

/// Удаляет канал из позиции index
int DelChannel(int index);
/// --------------------------

// Добавляет заданный фрейм типа TUImagesFrame, TUWatchFrame и т.п. на заданную вкладку
bool AddSpecialFrameToPage(TUVisualControllerFrame *frame, TTabSheet* tab, const String &caption);

// Добавляет заданную форму на заданную вкладку
bool AddSpecialFormToPage(TUVisualControllerForm *form, TTabSheet* tab, const String &caption);

// Создает новую вкладку с заданным именем для специальных фреймов
// type - строковое имя типа фрейма на вкладке
TTabSheet* AddSpecialFramePage(const String &type, const String &caption);

// Создает новую вкладку с заданным именем для специальных форм
TTabSheet* AddSpecialFormPage(const String &type);

// Ищет вкладку с заданным именем для заданной специальной форме
TTabSheet* FindSpecialFormPage(const String &type, TUVisualControllerForm* &form);

// Создает новую вкладку с заданным именем для формы управления компонентом
// Если этим копонентом уже управляют, то возвращает указатель на такую вкладку
TTabSheet* AddComponentControlFormPage(const string &component_name);

/// Ищет и возвращает указатель на форму или фрейм, соответствующий вкладке с заданным индексом
void FindVisualController(int index, TUVisualControllerFrame* &frame, TUVisualControllerForm* &form);

// Удаляет страницу
void DelPage(int index);

// Переименовывает заголовок страницы
void RenamePage(int index, String new_name);

// Ищет, существует ли уже вкладка, управляющая компонентом с заданным именем
// Возвращает индекс вкладки или -1, если вкладка не найдена
int FindComponentControlPage(const std::string &component_long_name);

// Удаляет все лишние вкладки (оставляет 2 начальные)
void ClearPages(void);

/// Добавляет новый пункт в подменю сетевого вещания событиями
void AddBroadcasterMenu(TMenuItem *item, TMenu *owner);

/// Добавляет новый (глобальный) пункт в меню Window
void AddGlobalWindowMenu(TMenuItem *item, TMenu *owner);

/// Запуск отдельного канала
/// если channel_index == -1 то запускает все каналы
virtual void StartChannel(int channel_index);

/// Останов отдельного канала
/// если channel_index == -1 то останавливает все каналы
virtual void PauseChannel(int channel_index);

/// Сброс отдельного канала
/// если channel_index == -1 то сбрасывает все каналы
virtual void ResetChannel(int channel_index);

/// Формирование строки с отладочной информацией об объеме используемой памяти
String DebugGenerateMemoryUsageString(void);

};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUGEngineControlForm *UGEngineControlForm;
//---------------------------------------------------------------------------
#endif
