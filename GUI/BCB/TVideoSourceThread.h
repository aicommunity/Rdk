//---------------------------------------------------------------------------

#ifndef TVideoSourceThreadH
#define TVideoSourceThreadH
#include "VidGrab.hpp"
#include "TUHttpServerUnit.h"
#include "myrdk.h"

//---------------------------------------------------------------------------
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

/// Флаг повтора воспроизведения сначала после завершения
bool RepeatFlag;

/// Режим восстановления после сбоя захвата
/// 0 - не делать ничего
/// 1 - попытаться восстановить захват
/// 2 - вызвать метод останова захвата
int RestartMode;

protected: // Данные
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

/// Флаг повтора воспроизведения сначала после завершения
virtual bool GetRepeatFlag(void) const;
virtual bool SetRepeatFlag(bool value);

/// Устанавливает значение FPS
virtual double GetFps(void) const;
virtual bool SetFps(double fps);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
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
virtual bool ASaveParameters(RDK::USerStorageXML &xml)=0;

/// Загрузка и применение настроек из xml
virtual bool LoadParameters(RDK::USerStorageXML &xml);
virtual bool LoadParametersEx(RDK::USerStorageXML &xml);
virtual bool ALoadParameters(RDK::USerStorageXML &xml)=0;
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
bool ReadSourceSafe(RDK::UBitmap& dest, double &time_stamp, bool reflect);

/// Записывает изображение в тред с блокировкой
bool WriteSourceSafe(const RDK::UBitmap& src, double time_stamp, bool reflect);
bool WriteSourceSafe(Graphics::TBitmap *src, double time_stamp, bool reflect);

// Меняет временную метку с блокировкой
virtual bool SetLastTimeStampSafe(double time_stamp);

/// Возвращает 0 если если состояние не определено
/// Возвращает 1 если если нет подключения к источнику
/// Возвращает 2 если если есть подключение к источнику
virtual int CheckConnection(void) const;

/// Возвращает 0 если захват не работает и 1 если активен
virtual int CheckCaptureThreadState(void) const;
// --------------------------
};

class TVideoCaptureThreadBmp: public TVideoCaptureThread
{
protected: // Параметры
/// Имя файла изображения
std::string FileName;

double Fps;

protected: // Временные изображения
RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

double CurrentTimeStamp;


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

double Fps;



RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

double CurrentTimeStamp;

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
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

// Загружает выбранную картинку по индеку в массиве имен
bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

// Меняет временную метку с блокировкой
virtual bool SetLastTimeStampSafe(double time_stamp);
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

protected: // Временные переменные
int ConnectionState;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabber(void);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Сохранение настроек в xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// Загрузка и применение настроек из xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
TVideoGrabber* GetVideoGrabber(void);

void __fastcall OnFrameCaptureCompleted(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId);

void __fastcall VideoGrabberLog(TObject *Sender,
	  TLogType LogType, String Severity, String InfoMsg);
void __fastcall VideoGrabberDeviceLost(TObject *Sender);

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
// --------------------------
};

class TVideoCaptureThreadVideoGrabberAvi: public TVideoCaptureThreadVideoGrabber
{
protected: // Параметры
/// Имя файла
std::string FileName;

bool ProcessAllFramesFlag;

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
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);


// Меняет временную метку с блокировкой
virtual bool SetLastTimeStampSafe(double time_stamp);

void __fastcall AfterCalculate(void);
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
};

#endif
