//---------------------------------------------------------------------------

#ifndef TVideoSourceThreadH
#define TVideoSourceThreadH
#include "VidGrab.hpp"
#include <jpeg.hpp>
#include "TUHttpServerUnit.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

class TVideoGrabberFrame;
class TGrabberThread;
class TImageFrame;

enum TVideoCaptureThreadCommands { tvcNone=0, tvcStart=1, tvcStop=2, tvcTerminate=3, tvcRecreate=4, tvcHalt=5 };

/// Описание команды
struct TVideoCaptureThreadCmdDescr
{
/// Идентификатор команды
TVideoCaptureThreadCommands Id;

/// Ожидаемое время исполнения команды
double ExecTime;

TVideoCaptureThreadCmdDescr(void);
TVideoCaptureThreadCmdDescr(TVideoCaptureThreadCommands id, double exec_time);
};

class TSourceStarterBase
{
	virtual void NextSource()=0;
};

//---------------------------------------------------------------------------
class TVideoGrabberControlForm;
class TVideoOutputFrame;

class TVideoCaptureThread: public TThread
{
private:
/// Очередь команд управления тредом
/// <временная метка команды, ID команды>
std::list<std::pair<double,TVideoCaptureThreadCmdDescr> > CommandQueue;

/// Мьютекс для разделения доступа к командам
//TMutex* CommandMutex;

protected: // Параметры
/// Желаемый FPS
/// Если 0, то FPS источника
RDK::UELockVar<double> Fps;

/// Источник видео
/// 0 - bmp
/// 1 - avi
/// 2 - camera
/// 3 - ip camera
/// 4 - bmp sequence
/// 5 - http server
/// 6 - shared memory
RDK::UELockVar<int> SourceMode;

/// Индекс канала в библиотеке аналитики, управляемый тредом
RDK::UELockVar<int> ChannelIndex;

/// Режим синхронизации с расчетом
/// 0 - Нет сихнронизации с расчетом
/// 1 - Синхронизация с расчетом включена
RDK::UELockVar<int> SyncMode;

/// Флаг повтора воспроизведения сначала после завершения
RDK::UELockVar<bool> RepeatFlag;

/// Режим восстановления после сбоя захвата
/// 0 - не делать ничего
/// 1 - попытаться восстановить захват
/// 2 - вызвать метод останова захвата
RDK::UELockVar<int> RestartMode;

/// Интервал между последним стартом и рестартом, мс
RDK::UELockVar<int> RestartInterval;

/// Максимально допустимый интервал между захватами кадра,
/// после которого мы считаем, что произошла потеря соединения, мс
RDK::UELockVar<int> MaxInterstepInterval;

/// Желаемое разрешение захвата
RDK::UELockVar<int> DesiredWidth;
RDK::UELockVar<int> DesiredHeight;

/// Флаг включения выбора желаемого разрешения захвата
RDK::UELockVar<bool> DesiredResolutionFlag;

protected: // Данные
/// Флаг состояния треда
/// 0 - остановлен
/// 1 - Запущен
//RDK::UELockVar<int> ThreadState;

/// Реальное состояние соединения с источником видео
/// 0 - состояние неизвестно
/// 1 - источник отключен
/// 2 - источник запущен
/// 10 - источник в процессе подключения
RDK::UELockVar<int> ConnectionState;

/// Последняя попытка запуска захвата
RDK::UELockVar<double> LastStartTime;

/// Временная метка последнего кадра в данных реального времени
RDK::UELockVar<double> RealLastTimeStamp;


private:
/// Временная метка последнего кадра
double LastTimeStamp;

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
HANDLE SourceWriteUnlock;

/// Выставляется на время работы видеозахвата
HANDLE CaptureEnabled;

/// Сбрасывается на время ожидания расчета
HANDLE CalcCompleteEvent;

/// Событие блокировки очереди
HANDLE CommandUnlockMutex;

public:
/// Глобальное событие блокировки запуска видеозахвата
static HANDLE GlobalStartUnlockMutex;

/// Локальное событие информаирования о запуске видеозахвата
HANDLE StartInProgressEvent;

public:

/// Выставляется как PulseEvent по завершении источника
HANDLE SourceStoppedEvent;

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThread(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThread(void);
// --------------------------

// --------------------------
// Управление командами
// --------------------------
protected:
/// Добавляет команду в очередь
void AddCommand(TVideoCaptureThreadCmdDescr value);

/// Очищает очередь
void ClearCommandQueue(void);

/// Осуществляет обработку очередной команды из очереди
void ProcessCommandQueue(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
public:
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

/// Флаг повтора воспроизведения сначала после завершения
virtual bool GetRepeatFlag(void) const;
virtual bool SetRepeatFlag(bool value);

/// Устанавливает значение FPS
virtual double GetFps(void) const;
virtual bool SetFps(double fps);

/// Интервал между последним стартом и рестартом, мс
virtual int GetRestartInterval(void) const;
virtual bool SetRestartInterval(int value);

/// Желаемое разрешение захвата
virtual int GetDesiredWidth(void) const;
virtual bool SetDesiredWidth(int value);
virtual int GetDesiredHeight(void) const;
virtual bool SetDesiredHeight(int value);

/// Флаг включения выбора желаемого разрешения захвата
virtual bool GetDesiredResolutionFlag(void) const;
virtual bool SetDesiredResolutionFlag(bool value);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Флаг состояния треда
/// 0 - остановлен
/// 1 - Запущен
//int GetThreadState(void) const;

/// Указатель на владельца
TVideoOutputFrame *GetFrame(void) const;
bool SetFrame(TVideoOutputFrame * frame);

/// Возвращает разрешение потока
virtual int GetWidth(void);
virtual int GetHeight(void);

/// Создает копию этого потока
virtual RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended)=0;

/// Сохранение настроек в xml
virtual bool SaveParameters(RDK::USerStorageXML &xml);
virtual bool SaveParametersEx(RDK::USerStorageXML &xml);
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool LoadParameters(RDK::USerStorageXML &xml);
virtual bool LoadParametersEx(RDK::USerStorageXML &xml);
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление событиями
// --------------------------
/// Выставляется по завершении захвата нового кадра
HANDLE GetFrameNotInProgress(void) const;

/// Выставлено всегда. Сбрасывается на время доступа к изображению
//HANDLE GetSourceUnlock(void) const;

/// Выставляется на время работы видеозахвата
HANDLE GetCaptureEnabled(void) const;

/// Сбрасывается на время ожидания расчета
HANDLE GetCalcCompleteEvent(void) const;
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall Start(double time);

virtual void __fastcall Stop(double time);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void)=0;

virtual void __fastcall Execute(void);
virtual void __fastcall ExecuteCaptureInit(void);
virtual void __fastcall ExecuteCaptureUnInit(void);

/// Возвращает копию изображения с блокировкой
bool ReadSourceSafe(RDK::UBitmap& dest, double &time_stamp, bool reflect);

/// Записывает изображение в тред с блокировкой
bool WriteSourceSafe(const RDK::UBitmap& src, double time_stamp, bool reflect);
bool WriteSourceSafe(Graphics::TBitmap *src, double time_stamp, bool reflect);

// Меняет временную метку с блокировкой
virtual bool SetLastTimeStampSafe(double time_stamp);

// Считывает временную метку с блокировкой
virtual double GetLastTimeStampSafe(void) const;

/// Возвращает 0 если если состояние не определено
/// Возвращает 1 если если нет подключения к источнику
/// Возвращает 2 если если есть подключение к источнику
virtual int CheckConnection(void) const;

/// Возвращает 0 если захват не работает и 1 если активен
virtual int CheckCaptureThreadState(void) const;
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
protected:
virtual bool __fastcall RunCapture(void);
virtual void __fastcall ARunCapture(void)=0;

virtual bool __fastcall StopCapture(void);
virtual void __fastcall AStopCapture(void)=0;

/// Останавливает фактический захват не меняя статуса треда
virtual bool __fastcall HaltCapture(void);

virtual bool __fastcall RecreateCapture(void);
virtual void __fastcall ARecreateCapture(void);
// --------------------------


};

class TVideoCaptureThreadBmp: public TVideoCaptureThread
{
protected: // Параметры
/// Имя файла изображения
RDK::UELockVar<std::string> FileName;

//double Fps;

protected: // Временные изображения
RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

RDK::UELockVar<double> CurrentTimeStamp;


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
std::string GetFileName(void) const;
bool SetFileName(const std::string& value);

/// Возвращает число изображений в последовательности
virtual long long GetNumBitmaps(void) const;

/// Устанавливает текущую позицию в последовательности
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);

/// Устанавливает значение FPS
double GetFps(void) const;
bool SetFps(double fps);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall AStopCapture(void);

virtual void __fastcall ARecreateCapture(void);
// --------------------------
};

class TVideoCaptureThreadBmpSequence: public TVideoCaptureThread
{
protected: // Параметры
/// Имя пути до файлов изображения
RDK::UELockVar<std::string> PathName;

protected: // Временные изображения
// Массив изображений для режима последовательности картинок
std::vector<std::string> BmpSequenceNames;

// Текущий кадр в последовательности картинок
RDK::UELockVar<int> CurrentBmpSequenceIndex;

RDK::UELockVar<int> LastReadSequenceIndex;

//double Fps;



RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

RDK::UELockVar<double> CurrentTimeStamp;

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
std::string GetPathName(void) const;
bool SetPathName(const std::string& value);

/// Возвращает число изображений в последовательности
long long GetNumBitmaps(void) const;

/// Устанавливает текущую позицию в последовательности
long long GetPosition(void) const;
bool SetPosition(long long index);

/// Устанавливает значение FPS
double GetFps(void) const;
bool SetFps(double fps);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

// Загружает выбранную картинку по индеку в массиве имен
bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

// Меняет временную метку с блокировкой
//virtual bool SetLastTimeStampSafe(double time_stamp);
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall AStopCapture(void);

virtual void __fastcall ARecreateCapture(void);
// --------------------------
};

class TVideoCaptureThreadHttpServer: public TVideoCaptureThread
{
protected: // Параметры
/// Порт сервера
RDK::UELockVar<int> ListenPort;

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
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

void __fastcall IdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
		  TIdHTTPResponseInfo *AResponseInfo);
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall AStopCapture(void);

virtual void __fastcall ARecreateCapture(void);
// --------------------------

};

class TVideoCaptureThreadVideoGrabber: public TVideoCaptureThread
{
protected: // Параметры
/// Таймаут соединения с камерой
RDK::UELockVar<int> ConnectionTimeout;

/// Таймаут захвата
RDK::UELockVar<int> CaptureTimeout;

/// Режим масштабирования отображения на экране
/// 0 - Изображение выводится оригинального размера
/// 1 - Изображение масштабируется пропорционально, по границам окна
/// 2 - Изображение масштабиуется с растяжением по всем сторонам
RDK::UELockVar<int> AutoScaleMode;

protected: // Данные
TVideoGrabber* VideoGrabber;

Graphics::TBitmap* ConvertBitmap;

/// Маска для OSD
Graphics::TBitmap* OverlayMaskBitmap;

RDK::UBitmap ConvertUBitmap,ConvertResult;

RDK::UELockVar<double> ConvertTimeStamp;

/// Хендл окна в которое необходимо выводить данные захвата
TWinControl* OverlayHandle;

protected: // События
/// Выставляется при получении очередного кадра
HANDLE VideoGrabberCompleted;

/// Событие блокировки изображения для конвертации
HANDLE ConvertMutex;

/// Событие блокировки OSD изображения
HANDLE OSDMutex;


protected: // Временные переменные


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabber(void);
// --------------------------
// --------------------------
// Управление параметрами
// --------------------------
/// Устанавливает значение FPS
double GetFps(void) const;
bool SetFps(double fps);

/// Таймаут соединения с камерой
int GetConnectionTimeout(void) const;
bool SetConnectionTimeout(int value);

/// Таймаут захвата
int GetCaptureTimeout(void) const;
bool SetCaptureTimeout(int value);
// --------------------------
// Управление данными
// --------------------------
/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);

/// Хендл окна в которое необходимо выводить данные захвата
virtual TWinControl* GetOverlayHandle(void) const;
virtual bool SetOverlayHandle(TWinControl* value, bool forced=false);

/// Управление маской для OSD
virtual Graphics::TBitmap* GetOverlayMaskBitmap(void);
virtual bool SetOverlayMaskBitmap(Graphics::TBitmap* value);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall TvgExecuteCaptureInit(void);
virtual void __fastcall TvgExecuteCaptureUnInit(void);
TVideoGrabber* GetVideoGrabber(void);
void __fastcall OnFrameCaptureCompleted(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId);

void __fastcall VideoGrabberLog(TObject *Sender,
	  TLogType LogType, String Severity, String InfoMsg);
void __fastcall VideoGrabberDeviceLost(TObject *Sender);

void __fastcall VideoGrabberFrameBitmap(TObject *Sender,
	  pFrameInfo FrameInfo, pFrameBitmapInfo BitmapInfo);

void __fastcall VideoGrabberPlayerEndOfStream(TObject *Sender);

void __fastcall VideoGrabberOnPlayerOpened(System::TObject* Sender);

void __fastcall VideoGrabberOnThreadSync(System::TObject* Sender, TThreadSyncPoint ThreadSyncPoint);

void __fastcall VideoGrabberOnPreviewStarted(TObject *Sender);

void __fastcall VideoGrabberOnVideoMouseUp(System::TObject* Sender, int VideoWindow, System::Uitypes::TMouseButton Button, System::Classes::TShiftState Shift, int X, int Y);

void __fastcall VideoGrabberOnVideoMouseDown(System::TObject* Sender, int VideoWindow, System::Uitypes::TMouseButton Button, System::Classes::TShiftState Shift, int X, int Y);

void __fastcall VideoGrabberOnVideoMouseMove(System::TObject* Sender, int VideoWindow, System::Classes::TShiftState Shift, int X, int Y);

virtual void __fastcall Calculate(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

/// Возвращает число изображений в последовательности
virtual long long GetNumBitmaps(void) const;

/// Устанавливает текущую позицию в последовательности
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);

/// Возвращает 0 если если состояние не определено
/// Возвращает 1 если если нет подключения к источнику
/// Возвращает 2 если если есть подключение к источнику
virtual int CheckConnection(void) const;

/// Режим масштабирования отображения на экране
/// 0 - Изображение выводится оригинального размера
/// 1 - Изображение масштабируется пропорционально, по границам окна
/// 2 - Изображение масштабиуется с растяжением по всем сторонам
virtual int GetAutoScaleMode(void) const;
virtual bool SetAutoScaleMode(int value, bool forced=true);
// --------------------------
};

class TVideoCaptureThreadVideoGrabberAvi: public TVideoCaptureThreadVideoGrabber
{
protected: // Параметры
/// Имя файла
RDK::UELockVar<std::string> FileName;

RDK::UELockVar<bool> ProcessAllFramesFlag;

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
std::string GetFileName(void) const;
bool SetFileName(const std::string& value);

bool GetProcessAllFramesFlag(void) const;
bool SetProcessAllFramesFlag(bool value);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall TvgExecuteCaptureInit(void);


// Меняет временную метку с блокировкой
virtual bool SetLastTimeStampSafe(double time_stamp);

void __fastcall AfterCalculate(void);
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
protected:
virtual bool __fastcall RecreateCapture(void);

virtual void __fastcall ARunCapture(void);

virtual void __fastcall AStopCapture(void);
// --------------------------

};


class TVideoCaptureThreadVideoGrabberCamera: public TVideoCaptureThreadVideoGrabber
{
protected: // Параметры
RDK::UELockVar<int> CameraIndex;
RDK::UELockVar<int> InputIndex;
RDK::UELockVar<int> SizeIndex;
RDK::UELockVar<int> SubtypeIndex;
RDK::UELockVar<int> AnalogIndex;
RDK::UELockVar<int> TVTunerChannel;
RDK::UELockVar<int> TVTunerCountryCode;
RDK::UELockVar<int> TVTunerMode;
RDK::UELockVar<int> TVTunerInputType;

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
int GetTVTunerChannel(void) const;
int GetTVTunerCountryCode(void) const;
int GetTVTunerMode(void) const;
int GetTVTunerInputType(void) const;
bool Init(int camera_index, int input_index, int size_index, int subtype_index, int analog_index,
			int tv_tuner_channel, int tv_tuner_country_code, int tv_tuner_mode, int tv_tuner_input_type);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall TvgExecuteCaptureInit(void);
// --------------------------


// --------------------------
// Скрытые методы управления потоком
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall AStopCapture(void);
// --------------------------

};

class TVideoCaptureThreadVideoGrabberIpCamera: public TVideoCaptureThreadVideoGrabber
{
protected: // Параметры
/// Имя камеры
RDK::UELockVar<String> Url;

/// Имя пользователя
RDK::UELockVar<String> UserName;

/// Пароль
RDK::UELockVar<String> Password;

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
String GetUrl(void) const;

/// Имя пользователя
String GetUserName(void) const;

/// Пароль
String GetPassword(void) const;

bool Init(const String camera_url, const String user_name, const String user_password);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall TvgExecuteCaptureInit(void);
// --------------------------


// --------------------------
// Скрытые методы управления потоком
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall AStopCapture(void);
// --------------------------

};


class TVideoCaptureThreadSharedMemory: public TVideoCaptureThread
{
protected: // Параметры
/// Индекс канала общей памяти
RDK::UELockVar<int> PipeIndex;

/// Имя канала общей памяти
RDK::UELockVar<std::string> PipeName;

/// Размер канала общей памяти
RDK::UELockVar<int> SharedMemoryPipeSize;

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
std::string GetPipeName(void) const;
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
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

virtual void __fastcall UnsafeInit(void);
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall AStopCapture(void);
// --------------------------

};

class TVideoCaptureThreadNewVideoGrabber: public TVideoCaptureThread
{
protected: // Параметры

protected: // Временные изображения
RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

//TVideoGrabberFrame *VideoGrabberFrame;
TGrabberThread *GrabberThread;
//TImageFrame* ImageFrame;

String UserName, Password;

RDK::UELockVar<double> CurrentTimeStamp;

Graphics::TBitmap* ConvertBitmap;

/// Маска для OSD
Graphics::TBitmap* OverlayMaskBitmap;

RDK::UBitmap ConvertUBitmap,ConvertResult;

RDK::UELockVar<double> ConvertTimeStamp;

/// Хендл окна в которое необходимо выводить данные захвата
TWinControl* OverlayHandle;

protected: // События
/// Выставляется при получении очередного кадра
HANDLE VideoGrabberCompleted;

/// Событие блокировки изображения для конвертации
HANDLE ConvertMutex;

/// Событие блокировки OSD изображения
HANDLE OSDMutex;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadNewVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadNewVideoGrabber(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
virtual bool SetSourceMode(int mode);
/// Имя файла изображения
//std::string GetFileName(void) const;
//bool SetFileName(const std::string& value);

/// Возвращает число изображений в последовательности
virtual long long GetNumBitmaps(void) const;

/// Устанавливает текущую позицию в последовательности
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);

/// Устанавливает значение FPS
double GetFps(void) const;
bool SetFps(double fps);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall AStopCapture(void);

virtual void __fastcall ARecreateCapture(void);
// --------------------------

public:
void __fastcall OnFrameCaptureCompleted(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId);

void __fastcall VideoGrabberLog(TObject *Sender,
	  TLogType LogType, String Severity, String InfoMsg);
void __fastcall VideoGrabberDeviceLost(TObject *Sender);

void __fastcall VideoGrabberFrameBitmap(TObject *Sender,
	  pFrameInfo FrameInfo, pFrameBitmapInfo BitmapInfo);

void __fastcall VideoGrabberPlayerEndOfStream(TObject *Sender);

void __fastcall VideoGrabberOnPlayerOpened(System::TObject* Sender);

void __fastcall VideoGrabberOnThreadSync(System::TObject* Sender, TThreadSyncPoint ThreadSyncPoint);

void __fastcall VideoGrabberOnPreviewStarted(TObject *Sender);

void __fastcall VideoGrabberOnVideoMouseUp(System::TObject* Sender, int VideoWindow, System::Uitypes::TMouseButton Button, System::Classes::TShiftState Shift, int X, int Y);

void __fastcall VideoGrabberOnVideoMouseDown(System::TObject* Sender, int VideoWindow, System::Uitypes::TMouseButton Button, System::Classes::TShiftState Shift, int X, int Y);

void __fastcall VideoGrabberOnVideoMouseMove(System::TObject* Sender, int VideoWindow, System::Classes::TShiftState Shift, int X, int Y);

};


#endif
