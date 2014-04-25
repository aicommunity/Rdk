#ifndef RDK_VIDEO
#define RDK_VIDEO
#endif
//---------------------------------------------------------------------------

#ifndef VideoOutputFrameUnitH
#define VideoOutputFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <CheckLst.hpp>
#include <Menus.hpp>
#include "VidGrab.hpp"
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdCustomHTTPServer.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdHTTPServer.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include "TUVisualController.h"
#include "TUVisualControllerFrameUnit.h"
#include "VideoOutputToolsFormUnit.h"
#include "UComponentsListFormUnit.h"
#include "TVideoSourceThread.h"
#include "TUHttpServerUnit.h"
#include "myrdk.h"
#include "TVideoRegistratorFrameUnit.h"
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.ActnList.hpp>

class TVideoCaptureOptionsForm;

//---------------------------------------------------------------------------
class TVideoOutputFrame : public TUVisualControllerFrame
{
__published:    // IDE-managed Components
    TGroupBox *GroupBox;
    TImage *Image;
    TPanel *Panel1;
    TButton *StopButton;
    TButton *StartButton;
    TTimer *Timer;
    TMaskEdit *TimeEdit;
    TPopupMenu *PopupMenu;
    TMenuItem *N1;
    TTrackBar *TrackBar;
    TMenuItem *SourceControl1;
    TMenuItem *N2;
    TMenuItem *N3;
    TMenuItem *SendToComponent;
    TMenuItem *SendToComponentState1;
	TMenuItem *N4;
	TMenuItem *SendImageToComponentInput1;
	TMenuItem *SendImageToComponentOutput1;
	TPanel *Panel2;
	TEdit *SendToEdit;
	TButton *Button1;
	TButton *SelectSendToButton;
	TPopupMenu *SelectPopupMenu;
	TMenuItem *Parameter1;
	TMenuItem *State1;
	TMenuItem *SendImageToComponentProperty1;
	TPanel *Panel3;
	TCheckBox *ShowCentralPointCheckBox;
	TEdit *PointXEdit;
	TEdit *PointYEdit;
	TButton *SendAsMatrixButton;
	TMenuItem *PropertyMatrix1;
	TCheckBox *SendPointsByStepCheckBox;
	TCheckBox *DeletePointsAfterSendCheckBox;
	TGroupBox *GroupBox1;
	TMenuItem *N5;
	TMenuItem *SaveImage1;
	TSavePictureDialog *SavePictureDialog;
	TPageControl *PageControl;
	TTabSheet *VideoSourceTabSheet;
	TTabSheet *NetworkStreamingTabSheet;
	TTVideoRegistratorFrame *NetworkStreamingFrame;
	TTabSheet *RecordingTabSheet;
	TTVideoRegistratorFrame *RecordingFrame;
	TMenuItem *SourceControl21;
	TToolBar *ToolBar;
	TToolButton *CaptureStartToolButton;
	TToolButton *CaptureStopToolButton;
	TToolButton *StartStreamingToolButton;
	TToolButton *StopStreamingToolButton;
	TToolButton *StartRecordingToolButton;
	TToolButton *StopRecordingToolButton;
	TImageList *ImageList;
	TPopupMenu *RecordingPopupMenu;
	TMenuItem *SelectFile1;
	TToolButton *ToolButton1;
	TToolButton *ToolButton3;
	TToolButton *SavePictureToolButton;
	TToolButton *SelectSourceToolButton;
	TActionList *ActionList;
	TAction *SavePictureAction;
	TMenuItem *VideoCodec;
	TMenuItem *RecordingMethod;
    void __fastcall TimerTimer(TObject *Sender);
	void __fastcall StartButtonClick(TObject *Sender);
    void __fastcall StopButtonClick(TObject *Sender);
    void __fastcall ImageMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall ImageMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
    void __fastcall ImageMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall TimeEditChange(TObject *Sender);
    void __fastcall N1Click(TObject *Sender);
//    void __fastcall VideoGrabberFrameCaptureCompleted(TObject *Sender, Pointer FrameBitmap,
//          int BitmapWidth, int BitmapHeight, DWORD FrameNumber, __int64 FrameTime,
//          TFrameCaptureDest DestType, UnicodeString FileName, bool Success,
//          int FrameId);
    void __fastcall TrackBarChange(TObject *Sender);
    void __fastcall SourceControl1Click(TObject *Sender);
    void __fastcall SendToComponentClick(TObject *Sender);
    void __fastcall SendToComponentState1Click(TObject *Sender);
	void __fastcall SendImageToComponentOutput1Click(TObject *Sender);
	void __fastcall SendImageToComponentInput1Click(TObject *Sender);
	void __fastcall Parameter1Click(TObject *Sender);
	void __fastcall State1Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall SendImageToComponentProperty1Click(TObject *Sender);
	void __fastcall SendAsMatrixButtonClick(TObject *Sender);
	void __fastcall PropertyMatrix1Click(TObject *Sender);
	void __fastcall SaveImage1Click(TObject *Sender);
	void __fastcall SourceControl21Click(TObject *Sender);
	void __fastcall StartRecordingToolButtonClick(TObject *Sender);
	void __fastcall SelectFile1Click(TObject *Sender);
	void __fastcall CaptureStartToolButtonClick(TObject *Sender);
	void __fastcall CaptureStopToolButtonClick(TObject *Sender);
	void __fastcall RecordingFrameStartRecordingButtonClick(TObject *Sender);
	void __fastcall StopRecordingToolButtonClick(TObject *Sender);
	void __fastcall NetworkStreamingFrameNetworkStreamingButtonClick(TObject *Sender);
	void __fastcall StopStreamingToolButtonClick(TObject *Sender);
	void __fastcall SelectSourceToolButtonClick(TObject *Sender);
	void __fastcall SavePictureActionExecute(TObject *Sender);

//	void __fastcall UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
//          TIdHTTPResponseInfo *AResponseInfo);
	// Реакция на клик динамического меню выбора видео кодека записи
	void __fastcall OnClickVideoCodec(TObject *Sender);

	// Реакция на клик динамического меню выбора видео кодека записи
	void __fastcall OnClickRecordingMethod(TObject *Sender);

	void __fastcall RecordingFrameVideoCompressorComboBoxChange(TObject *Sender);
	void __fastcall StartStreamingToolButtonClick(TObject *Sender);
	void __fastcall RecordingFrameRecordingMethodComboBoxChange(TObject *Sender);

private:    // User declarations
public:        // User declarations
    __fastcall TVideoOutputFrame(TComponent* Owner);
    __fastcall ~TVideoOutputFrame(void);

/// Список настроек поддерживаемых режимов
std::map<int, RDK::USerStorageXML> VideoSourceOptions;

// Индекс этого источника на форме всех источников
int FrameIndex;

// Режим работы
// 0 - Bmp
// 1 - Avi
// 2 - Camera
// 3 - IP Camera
// 4 - Image sequence
// 5 - Http Server
// 6 - Shared memory
int Mode;

//Graphics::TBitmap* ConvertBitmap;

// Форма управления выводом видео
TVideoOutputToolsForm* MyVideoOutputToolsForm;

// Форма управления инициализацией видео
//TVideoGrabberControlForm* MyVideoGrabberControlForm;

// Указатель на форму выбора компоненты-источника
TUComponentsListForm *MyComponentsListForm;

TVideoCaptureOptionsForm *VideoCaptureOptionsForm;

// Поток видеозахвата
TVideoCaptureThread* CaptureThread;

// Источник изображения
RDK::UBitmap BmpSource,ReflectedBmpSource;

// Канва рисования
RDK::UBitmap BmpCanvas;

// Модуль графики
RDK::UGraphics Graph;

// Отрисовка геометрии
RDK::MGraphics<double,2> GeometryGraphics;

// Текущая геометрия
RDK::MGraphics<double,2> CurrentGeometryGraphics;

// Массив изображений для режима последовательности картинок
//std::vector<std::string> BmpSequenceNames;

// Путь до последовательности изображений
//std::string BmpSequencePathName;

// Текущий кадр в последовательности картинок
//int CurrentBmpSequenceIndex;

protected: // Привязка источника видео к входу-выходу компонента
// Имя компонента, к которому привязан источник
std::string LinkedComponentName;

// Режим привязки
// 0 - ко входу
// 1 - к выходу
// 2 - к свойству
int LinkedMode;

// Выход, к которому привязан источник
std::string LinkedIndex;

// Свойство, к которому привязан источник
std::string LinkedComponentPropertyName;


public:
String LicenseString;
// Временная метка сервера
//long long ServerTimeStamp;

// Текущая создаваемая фигура
//RDK::MGeometry<double,2> Figure;

// Индекс текущей редактируемой фигуры
int FigureIndex;

// Индекс текущей редактируемой точки
int PointIndex;

// Флаг создания фигуры
//bool FigureFlag;

// Флаг создания точки
// 0 - выключено
// 1 - создание новой точки
// 2 - редактирование существующей
int PointFlag;

// Флаг разрешения выделения зоны
bool ZoneSelectEnable;

// Событие, устанавливаемое при окончании выделения зоны
HANDLE ZoneSelectEvent;

//int PipeIndex;
//std::string PipeName;
//int SharedMemoryPipeSize;

// ============================================================
// Временные переменные
// ============================================================
// Координаты окошка выделения зоны интереса
int x1b, y1b, x2b, y2b;

// Коррекционные координаты окошка выделения зоны интереса
int corrx1b, corry1b, corrx2b, corry2b;

// Конечные координаты окошка выделения зоны интереса
double left,top,width,height;

// Цвет выделения
TColor SelColor;

bool CorrSelectFlag;

// Флаг обновления отрисовки
bool UpdateFlag;

// Текущий обрабатываемый кадр
//long CurrentFrameNumber;

// Целевой параметр-приемник данных о геометрии
std::string SelectedComponentPName;
std::string SelectedComponentParameterName;

// Целевая переменная состояния-приемник данных о геометрии
std::string SelectedComponentSName;
std::string SelectedComponentStateName;

// Целевое свойство матрица-приемник данных о геометрии
std::string SelectedComponentPropertyMatrixName;
std::string SelectedComponentMatrixName;

//std::vector<char> PipeBuffer;

//int LastReadSequenceIndex;

// Флаг, выставляемый при включении видеозахвата
bool IsStarted;

protected:
bool RepeatSequenceFlag;
bool RepeatVideoFlag;
bool ProcessAllFramesFlag;

int SavePictureIndex;

public:
// ============================================================

// ---------------------------
// Методы управления поддерживаемыми источниками видео
// ---------------------------
/// Создает копию требуемого треда по индексу видеорежима
RDK::UEPtr<TVideoCaptureThread> TakeVideoCapureThread(int mode, TVideoOutputFrame *frame, bool create_suspended);

/// Уничтожает заданный тред
void ReturnVideoCapureThread(RDK::UEPtr<TVideoCaptureThread> thread);
// ---------------------------

// -----------------------------
// Методы управления видеозахватом
// -----------------------------
/// Инициализация захвата в заданном режиме
/// Если mode == -1 то осуществляет переиницализацию в текущем режиме
void Init(int mode=-1);

void Init(int mode, RDK::USerStorageXML &raw_xml_data);


/// Деинициализация захвата
void UnInit(void);

/// Запуск захвата
void Start(void);

/// Останов захвата
void Pause(void);

/// Чтение текущего изображения в bmp
void ReadSourceSafe(RDK::UBitmap &bmp, double &time_stamp, bool reflect);

/// Проверяет состояние завхата по id канала
/// 0 - не активен
/// 1 - активен
virtual int CheckCaptureState(void) const;
// -----------------------------

/// Инициализация первичных настроек
void InitPrimarySettings(void);
// Возвращает форму управления инициализацией видео
//TVideoGrabberControlForm* GetMyVideoGrabberControlForm(void);

// Уничтожает созданный поток
bool DestroyCaptureThread(void);

// Инициализация фрейма avi-файлом
void InitByAvi(const String &filename);

// Инициализация фрейма bmp-файлом
void InitByBmp(const String &filename, double fps);

// Устанавливает отдельное изображение
bool InitByBmp(const RDK::UBitmap &bmp, double fps);

// Инициализация фрейма камерой
void InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index);

// Инициализация фрейма IP-камерой
void InitByIPCamera(const String camera_url, const String user_name, const String user_password);

// Инициализация последовательностью изображений
bool InitByImageSequence(const String &pathname, double fps);

// Инициализация http-сервера
bool InitByHttpServer(int listen_port);

// Инициализация общей памяти
bool InitBySharedMemory(int pipe_index, const std::string &pipe_name);

// Загружает выбранную картинку по индеку в массиве имен
//bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

// Устанавливает название окна
bool SetTitle(String title);

// Устанавливает координаты отображаемой зоны
// Если все координаты == -1 то зона не отображается
bool SetFrameRect(int x,int y, int x_width, int y_height, TColor color=(TColor)0x00FF0000);

// Обновляет отрисовку окна
bool UpdateVideo(void);

// Отрисовываем текущее состояние видеопотока
void DrawCapture(Graphics::TBitmap *bmp);

// Обновляем список точек
void UpdateGeometryList(TCheckListBox *GeometryCheckListBox, TCheckListBox *PointsCheckListBox);

// Метод отрисовки прямоугольной зоны
void __fastcall TVideoOutputFrame::DrawFrameRect(TImage *image, int x1, int y1, int x2,
												int y2, int framewidth, TColor color);

// Добавляет очередной элемент фигуры
void AddFigureRect(double l,double t,double w,double h);

/// Добавляет новую фигуру
/// Функция возвращает индекс фигуры
int AddFigure(TColor color);

/// Возвращает число фигур
int GetNumFigures(void);

/// Удаляет фигуру
void DelFigure(int figure_index);

/// Удаляет все фигуры
void DelAllFigures(void);

/// Модифицирует имя фигуры
void EditFigureName(int figure_index, const std::string &figure_name);

/// Возвращает число точек в фигуре
int GetNumPoints(int figure_index);

/// Возвращает точку
const RDK::MVector<double,2> GetPoint(int figure_index, int point_index);

/// Добавляет новую точку
/// Функция возвращает индекс точки
int AddPoint(int figure_index, const RDK::MVector<double,2> &point_data);

/// Добавляет новую точку
/// Имя точки задается вручную
/// Функция возвращает индекс точки
int AddPoint(int figure_index, const std::string &point_name, const RDK::MVector<double,2> &point_data);

/// Удаляет точку
void DelPoint(int figure_index, int point_index);

/// Удаляет все точки
void DelAllPoints(int figure_index);

/// Модифицирует точку
void EditPoint(int figure_index, int point_index, const RDK::MVector<double,2> &point_data);

/// Модифицирует имя точки
void EditPointName(int figure_index, int point_index, const std::string &point_name);

// Устанавливает образец графики
void SetSampleGeometryGraphics(RDK::MGraphics<double,2>& samplegraphics);

/// Флаг повтора воспроизведения сначала после завершения
bool SetRepeatVideoFlag(bool value);
bool SetRepeatSequenceFlag(bool value);
bool SetProcessAllFramesFlag(bool value);

// -------------------------
// Методы ввода вывода точек геометрии из параметров и переменных состояния компонент
// -------------------------
// Отправляет набор точек в свойство компонента
void SendToComponentPropertyMatrix(const std::string &stringid, const std::string &parameter_name, int figure_index);

// Отправляет набор точек в параметр компонента
void SendToComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index);

// Отправляет набор точек в переменную состояния компонента
void SendToComponentState(const std::string &stringid, const std::string &state_name, int figure_index);

// Считывает набор точек из параметра компонента
void ReceiveFromComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index);

// Считывает набор точек из переменной состояния компонента
void ReceiveFromComponentState(const std::string &stringid, const std::string &state_name, int figure_index);
// -------------------------

// -------------------------
// Методы вывода изображений во входы-выходы компонент
// -------------------------
// Отправляет изображение в выбранный компонент
void SendToComponentIO(void);
// -------------------------
// Динамически наполняет меню
void DynamicMenuFilling(TMenuItem* target, std::vector<std::string> &itemsList);
// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед сбросом
void ABeforeReset(void);

// Метод, вызываемый перед шагом расчета
virtual void ABeforeCalculate(void);

// Метод, вызываемый после шага расчета
virtual void AAfterCalculate(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------

public:
void __fastcall SavePicture(String directory, String filename);

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputFrame *VideoOutputFrame;
//---------------------------------------------------------------------------
#endif
