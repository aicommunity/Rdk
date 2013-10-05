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
#include "myrdk.h"
#include <CheckLst.hpp>
#include "VideoOutputToolsFormUnit.h"
#include "UComponentsListFormUnit.h"
#include <Menus.hpp>
#include "VidGrab.hpp"
#include "TUVisualController.h"
#include "TUVisualControllerFrameUnit.h"
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdCustomHTTPServer.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdHTTPServer.hpp>
#include "TUHttpServerUnit.h"
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>

class TVideoGrabberControlForm;
class TVideoOutputFrame;

class TVideoCaptureThread: public TThread
{
protected: // Параметры
/// Источник видео
int SourceMode;

/// Индекс канала в библиотеке аналитики, управляемый тредом
int ChannelIndex;

/// Режим синхронизации с расчетом
/// 0 - Нет сихнронизации с расчетом
/// 1 - Синхронизация с расчетом включена
int SyncMode;

protected: // Данные
/// Временная метка последнего кадра
long long LastTimeStamp;

/// Данные изображения
RDK::UBitmap Source[2];

/// Указатель на текущее изображение для чтения
RDK::UBitmap* ReadSource;

/// Указатель на текущее изображение для записи
RDK::UBitmap* WriteSource;

/// Указатель на владельца
TVideoOutputFrame *Frame;

protected: // События
/// Снимается на время захвата кадра
HANDLE FrameNotInProgress;

/// Выставлено всегда. Сбрасывается на время доступа к изображению
HANDLE SourceUnlock;

/// Выставляется на время работы видеозахвата
HANDLE CaptureEnabled;

/// Сбрасывается на время ожидания расчета
HANDLE CalcCompleteEvent;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThread(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThread(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Источник видео
int GetSourceMode(void) const;

/// Индекс канала в библиотеке аналитики, управляемый тредом
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);

/// Режим синхронизации с расчетом
int GetSyncMode(void) const;
bool SetSyncMode(int mode);

/// Возвращает число изображений в последовательности
virtual long long GetNumBitmaps(void) const=0;

/// Устанавливает текущую позицию в последовательности
virtual long long GetPosition(void) const=0;
virtual bool SetPosition(long long index)=0;
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Указатель на владельца
TVideoOutputFrame *GetFrame(void) const;
bool SetFrame(TVideoOutputFrame * frame);
// --------------------------

// --------------------------
// Управление событиями
// --------------------------
/// Выставляется по завершении захвата нового кадра
HANDLE GetFrameNotInProgress(void) const;

/// Выставлено всегда. Сбрасывается на время доступа к изображению
HANDLE GetSourceUnlock(void) const;

/// Выставляется на время работы видеозахвата
HANDLE GetCaptureEnabled(void) const;

/// Сбрасывается на время ожидания расчета
HANDLE GetCalcCompleteEvent(void) const;
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void)=0;

virtual void __fastcall Execute(void);

/// Возвращает копию изображения с блокировкой
bool ReadSourceSafe(RDK::UBitmap& dest, long long &time_stamp, bool reflect);

/// Записывает изображение в тред с блокировкой
bool WriteSourceSafe(const RDK::UBitmap& src, long long time_stamp, bool reflect);
bool WriteSourceSafe(Graphics::TBitmap *src, long long time_stamp, bool reflect);

// Меняет временную метку с блокировкой
virtual bool SetLastTimeStampSafe(long long time_stamp);
// --------------------------
};

class TVideoCaptureThreadBmp: public TVideoCaptureThread
{
protected: // Параметры
/// Имя файла изображения
std::string FileName;

protected: // Временные изображения
RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadBmp(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadBmp(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Имя файла изображения
const std::string& GetFileName(void) const;
bool SetFileName(const std::string& value);

/// Возвращает число изображений в последовательности
virtual long long GetNumBitmaps(void) const;

/// Устанавливает текущую позицию в последовательности
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);
// --------------------------
};

class TVideoCaptureThreadBmpSequence: public TVideoCaptureThread
{
protected: // Параметры
/// Имя пути до файлов изображения
std::string PathName;

protected: // Временные изображения
// Массив изображений для режима последовательности картинок
std::vector<std::string> BmpSequenceNames;

// Текущий кадр в последовательности картинок
int CurrentBmpSequenceIndex;

int LastReadSequenceIndex;



RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadBmpSequence(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadBmpSequence(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Имя пути до файлов изображения
const std::string& GetPathName(void) const;
bool SetPathName(const std::string& value);

/// Возвращает число изображений в последовательности
long long GetNumBitmaps(void) const;

/// Устанавливает текущую позицию в последовательности
long long GetPosition(void) const;
bool SetPosition(long long index);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

// Загружает выбранную картинку по индеку в массиве имен
bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

// Меняет временную метку с блокировкой
virtual bool SetLastTimeStampSafe(long long time_stamp);
// --------------------------
};

class TVideoCaptureThreadHttpServer: public TVideoCaptureThread
{
protected: // Параметры
/// Порт сервера
int ListenPort;

protected: // Временные переменные
TUHttpServerFrame *UHttpServerFrame;

RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

protected: // События
/// Выставляется при получении очередного кадра
HANDLE HttpServerCompleted;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadHttpServer(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadHttpServer(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Имя файла изображения
int GetListenPort(void) const;
bool SetListenPort(int value);

/// Возвращает число изображений в последовательности
virtual long long GetNumBitmaps(void) const;

/// Устанавливает текущую позицию в последовательности
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

void __fastcall IdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
		  TIdHTTPResponseInfo *AResponseInfo);
// --------------------------
};

class TVideoCaptureThreadVideoGrabber: public TVideoCaptureThread
{
protected: // Параметры

protected: // Данные
TVideoGrabber* VideoGrabber;

Graphics::TBitmap* ConvertBitmap;

protected: // События
/// Выставляется при получении очередного кадра
HANDLE VideoGrabberCompleted;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabber(void);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
TVideoGrabber* GetVideoGrabber(void);

void __fastcall OnFrameCaptureCompleted(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId);

virtual void __fastcall Calculate(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

/// Возвращает число изображений в последовательности
virtual long long GetNumBitmaps(void) const;

/// Устанавливает текущую позицию в последовательности
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);
// --------------------------
};

class TVideoCaptureThreadVideoGrabberAvi: public TVideoCaptureThreadVideoGrabber
{
protected: // Параметры
/// Имя файла
std::string FileName;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberAvi(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabberAvi(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Имя канала общей памяти
const std::string& GetFileName(void) const;
bool SetFileName(const std::string& value);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);


// Меняет временную метку с блокировкой
virtual bool SetLastTimeStampSafe(long long time_stamp);
// --------------------------
};


class TVideoCaptureThreadVideoGrabberCamera: public TVideoCaptureThreadVideoGrabber
{
protected: // Параметры
int CameraIndex;
int InputIndex;
int SizeIndex;
int SubtypeIndex;
int AnalogIndex;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberCamera(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabberCamera(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
int GetCameraIndex(void) const;
int GetInputIndex(void) const;
int GetSizeIndex(void) const;
int GetSubtypeIndex(void) const;
int GetAnalogIndex(void) const;

bool Init(int camera_index, int input_index, int size_index, int subtype_index, int analog_index);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);
// --------------------------
};

class TVideoCaptureThreadVideoGrabberIpCamera: public TVideoCaptureThreadVideoGrabber
{
protected: // Параметры
/// Имя камеры
String Url;

/// Имя пользователя
String UserName;

/// Пароль
String Password;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberIpCamera(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabberIpCamera(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Имя камеры
const String& GetUrl(void) const;

/// Имя пользователя
const String& GetUserName(void) const;

/// Пароль
const String& GetPassword(void) const;

bool Init(const String camera_url, const String user_name, const String user_password);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);
// --------------------------
};


class TVideoCaptureThreadSharedMemory: public TVideoCaptureThread
{
protected: // Параметры
/// Индекс канала общей памяти
int PipeIndex;

/// Имя канала общей памяти
std::string PipeName;

/// Размер канала общей памяти
int SharedMemoryPipeSize;

protected: // Данные
/// Буфер приема данных из канала
std::vector<char> PipeBuffer;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadSharedMemory(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadSharedMemory(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Индекс канала общей памяти
int GetPipeIndex(void) const;
bool SetPipeIndex(int value);

/// Имя канала общей памяти
const std::string& GetPipeName(void) const;
bool SetPipeName(const std::string& value);

/// Размер канала общей памяти
int GetSharedMemoryPipeSize(void) const;

/// Возвращает число изображений в последовательности
virtual long long GetNumBitmaps(void) const;

/// Устанавливает текущую позицию в последовательности
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

virtual void __fastcall UnsafeInit(void);
// --------------------------
};

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
//	void __fastcall UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
//          TIdHTTPResponseInfo *AResponseInfo);


private:    // User declarations
public:        // User declarations
    __fastcall TVideoOutputFrame(TComponent* Owner);
    __fastcall ~TVideoOutputFrame(void);

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
TVideoGrabberControlForm* MyVideoGrabberControlForm;

// Указатель на форму выбора компоненты-источника
TUComponentsListForm *MyComponentsListForm;

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
// ============================================================



/// Уничтожает созданный поток
bool DestroyCaptureThread(void);

// Инициализация фрейма avi-файлом
void InitByAvi(const String &filename);

// Инициализация фрейма bmp-файлом
void InitByBmp(const String &filename);

// Устанавливает отдельное изображение
bool InitByBmp(const RDK::UBitmap &bmp);

// Инициализация фрейма камерой
void InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index);

// Инициализация фрейма IP-камерой
void InitByIPCamera(const String camera_url, const String user_name, const String user_password);

// Инициализация последовательностью изображений
bool InitByImageSequence(const String &pathname);

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

// Устанавливает образец графики
void SetSampleGeometryGraphics(RDK::MGraphics<double,2>& samplegraphics);

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

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputFrame *VideoOutputFrame;
//---------------------------------------------------------------------------
#endif
