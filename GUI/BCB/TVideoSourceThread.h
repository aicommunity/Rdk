//---------------------------------------------------------------------------

#ifndef TVideoSourceThreadH
#define TVideoSourceThreadH
#include "VidGrab.hpp"
#include "TUHttpServerUnit.h"
#include "myrdk.h"

enum TVideoCaptureThreadCommands { tvcNone=0, tvcStart=1, tvcStop=2, tvcTerminate=3, tvcRecreate=4 };

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
/// ������� ������ ���������� ������
/// <��������� ����� �������, ID �������>
std::map<double,TVideoCaptureThreadCommands> CommandQueue;

/// ������� ��� ���������� ������� � ��������
TMutex* CommandMutex;

protected: // ���������
/// �������� FPS
/// ���� 0, �� FPS ���������
RDK::UELockVar<double> Fps;

/// �������� �����
/// 0 - bmp
/// 1 - avi
/// 2 - camera
/// 3 - ip camera
/// 4 - bmp sequence
/// 5 - http server
/// 6 - shared memory
RDK::UELockVar<int> SourceMode;

/// ������ ������ � ���������� ���������, ����������� ������
RDK::UELockVar<int> ChannelIndex;

/// ����� ������������� � ��������
/// 0 - ��� ������������� � ��������
/// 1 - ������������� � �������� ��������
RDK::UELockVar<int> SyncMode;

/// ���� ������� ��������������� ������� ����� ����������
RDK::UELockVar<bool> RepeatFlag;

/// ����� �������������� ����� ���� �������
/// 0 - �� ������ ������
/// 1 - ���������� ������������ ������
/// 2 - ������� ����� �������� �������
RDK::UELockVar<int> RestartMode;

/// �������� ����� ��������� ������� � ���������, ��
RDK::UELockVar<int> RestartInterval;

/// ����������� ���������� �������� ����� ��������� �����,
/// ����� �������� �� �������, ��� ��������� ������ ����������, ��
RDK::UELockVar<int> MaxInterstepInterval;

protected: // ������
/// ���� ��������� �����
/// 0 - ����������
/// 1 - �������
RDK::UELockVar<int> ThreadState;

/// �������� ��������� ���������� � ���������� �����
/// 0 - ��������� ����������
/// 1 - �������� ��������
/// 2 - �������� �������
/// 10 - �������� � �������� �����������
RDK::UELockVar<int> ConnectionState;

/// ��������� ������� ������� �������
RDK::UELockVar<double> LastStartTime;

/// ��������� ����� ���������� ����� � ������ ��������� �������
RDK::UELockVar<double> RealLastTimeStamp;


private:
/// ��������� ����� ���������� �����
double LastTimeStamp;


/// ������ �����������
RDK::UBitmap Source[2];

/// ��������� �� ������� ����������� ��� ������
RDK::UBitmap* ReadSource;

/// ��������� �� ������� ����������� ��� ������
RDK::UBitmap* WriteSource;

/// ��������� �� ���������
TVideoOutputFrame *Frame;

protected: // �������
/// ��������� �� ����� ������� �����
HANDLE FrameNotInProgress;

/// ���������� ������. ������������ �� ����� ������� � �����������
HANDLE SourceUnlock;
HANDLE SourceWriteUnlock;

/// ������������ �� ����� ������ ������������
HANDLE CaptureEnabled;

/// ������������ �� ����� �������� �������
HANDLE CalcCompleteEvent;


public:

/// ������������ ��� PulseEvent �� ���������� ���������
HANDLE SourceStoppedEvent;

// ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThread(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThread(void);
// --------------------------

// --------------------------
// ���������� ���������
// --------------------------
protected:
/// ��������� ������� � �������
void AddCommand(TVideoCaptureThreadCommands value);

/// ������� �������
void ClearCommandQueue(void);

/// ������������ ��������� ��������� ������� �� �������
void ProcessCommandQueue(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
public:
/// �������� �����
int GetSourceMode(void) const;

/// ������ ������ � ���������� ���������, ����������� ������
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);

/// ����� ������������� � ��������
int GetSyncMode(void) const;
bool SetSyncMode(int mode);

/// ���������� ����� ����������� � ������������������
virtual long long GetNumBitmaps(void) const=0;

/// ������������� ������� ������� � ������������������
virtual long long GetPosition(void) const=0;
virtual bool SetPosition(long long index)=0;

/// ���� ������� ��������������� ������� ����� ����������
virtual bool GetRepeatFlag(void) const;
virtual bool SetRepeatFlag(bool value);

/// ������������� �������� FPS
virtual double GetFps(void) const;
virtual bool SetFps(double fps);

/// �������� ����� ��������� ������� � ���������, ��
virtual int GetRestartInterval(void) const;
virtual bool SetRestartInterval(int value);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ���� ��������� �����
/// 0 - ����������
/// 1 - �������
int GetThreadState(void) const;

/// ��������� �� ���������
TVideoOutputFrame *GetFrame(void) const;
bool SetFrame(TVideoOutputFrame * frame);

/// ���������� ���������� ������
virtual int GetWidth(void);
virtual int GetHeight(void);

/// ������� ����� ����� ������
virtual RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended)=0;

/// ���������� �������� � xml
virtual bool SaveParameters(RDK::USerStorageXML &xml);
virtual bool SaveParametersEx(RDK::USerStorageXML &xml);
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// �������� � ���������� �������� �� xml
virtual bool LoadParameters(RDK::USerStorageXML &xml);
virtual bool LoadParametersEx(RDK::USerStorageXML &xml);
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// ���������� ���������
// --------------------------
/// ������������ �� ���������� ������� ������ �����
HANDLE GetFrameNotInProgress(void) const;

/// ���������� ������. ������������ �� ����� ������� � �����������
HANDLE GetSourceUnlock(void) const;

/// ������������ �� ����� ������ ������������
HANDLE GetCaptureEnabled(void) const;

/// ������������ �� ����� �������� �������
HANDLE GetCalcCompleteEvent(void) const;
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall Start(void);
virtual void __fastcall AStart(void)=0;

virtual void __fastcall Stop(void);
virtual void __fastcall AStop(void)=0;

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void)=0;

virtual void __fastcall Execute(void);

/// ���������� ����� ����������� � �����������
bool ReadSourceSafe(RDK::UBitmap& dest, double &time_stamp, bool reflect);

/// ���������� ����������� � ���� � �����������
bool WriteSourceSafe(const RDK::UBitmap& src, double time_stamp, bool reflect);
bool WriteSourceSafe(Graphics::TBitmap *src, double time_stamp, bool reflect);

// ������ ��������� ����� � �����������
virtual bool SetLastTimeStampSafe(double time_stamp);

// ��������� ��������� ����� � �����������
virtual double GetLastTimeStampSafe(void) const;

/// ���������� 0 ���� ���� ��������� �� ����������
/// ���������� 1 ���� ���� ��� ����������� � ���������
/// ���������� 2 ���� ���� ���� ����������� � ���������
virtual int CheckConnection(void) const;

/// ���������� 0 ���� ������ �� �������� � 1 ���� �������
virtual int CheckCaptureThreadState(void) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
virtual bool __fastcall RunCapture(void);
virtual void __fastcall ARunCapture(void)=0;

virtual bool __fastcall PauseCapture(void);
virtual void __fastcall APauseCapture(void)=0;

virtual bool __fastcall RecreateCapture(void);
virtual void __fastcall ARecreateCapture(void);

bool SetThreadState(int value);
// --------------------------


};

class TVideoCaptureThreadBmp: public TVideoCaptureThread
{
protected: // ���������
/// ��� ����� �����������
RDK::UELockVar<std::string> FileName;

//double Fps;

protected: // ��������� �����������
RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

RDK::UELockVar<double> CurrentTimeStamp;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadBmp(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadBmp(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// ��� ����� �����������
std::string GetFileName(void) const;
bool SetFileName(const std::string& value);

/// ���������� ����� ����������� � ������������������
virtual long long GetNumBitmaps(void) const;

/// ������������� ������� ������� � ������������������
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);

/// ������������� �������� FPS
double GetFps(void) const;
bool SetFps(double fps);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// ���������� �������� � xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// �������� � ���������� �������� �� xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall AStart(void);

virtual void __fastcall AStop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall APauseCapture(void);

virtual void __fastcall ARecreateCapture(void);
// --------------------------
};

class TVideoCaptureThreadBmpSequence: public TVideoCaptureThread
{
protected: // ���������
/// ��� ���� �� ������ �����������
RDK::UELockVar<std::string> PathName;

protected: // ��������� �����������
// ������ ����������� ��� ������ ������������������ ��������
std::vector<std::string> BmpSequenceNames;

// ������� ���� � ������������������ ��������
RDK::UELockVar<int> CurrentBmpSequenceIndex;

RDK::UELockVar<int> LastReadSequenceIndex;

//double Fps;



RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

RDK::UELockVar<double> CurrentTimeStamp;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadBmpSequence(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadBmpSequence(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// ��� ���� �� ������ �����������
std::string GetPathName(void) const;
bool SetPathName(const std::string& value);

/// ���������� ����� ����������� � ������������������
long long GetNumBitmaps(void) const;

/// ������������� ������� ������� � ������������������
long long GetPosition(void) const;
bool SetPosition(long long index);

/// ������������� �������� FPS
double GetFps(void) const;
bool SetFps(double fps);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// ���������� �������� � xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// �������� � ���������� �������� �� xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall AStart(void);

virtual void __fastcall AStop(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

// ��������� ��������� �������� �� ������ � ������� ����
bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

// ������ ��������� ����� � �����������
virtual bool SetLastTimeStampSafe(double time_stamp);
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall APauseCapture(void);

virtual void __fastcall ARecreateCapture(void);
// --------------------------
};

class TVideoCaptureThreadHttpServer: public TVideoCaptureThread
{
protected: // ���������
/// ���� �������
RDK::UELockVar<int> ListenPort;

protected: // ��������� ����������
TUHttpServerFrame *UHttpServerFrame;

RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

protected: // �������
/// ������������ ��� ��������� ���������� �����
HANDLE HttpServerCompleted;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadHttpServer(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadHttpServer(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// ��� ����� �����������
int GetListenPort(void) const;
bool SetListenPort(int value);

/// ���������� ����� ����������� � ������������������
virtual long long GetNumBitmaps(void) const;

/// ������������� ������� ������� � ������������������
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// ���������� �������� � xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// �������� � ���������� �������� �� xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall AStart(void);

virtual void __fastcall AStop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

void __fastcall IdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
		  TIdHTTPResponseInfo *AResponseInfo);
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall APauseCapture(void);

virtual void __fastcall ARecreateCapture(void);
// --------------------------

};

class TVideoCaptureThreadVideoGrabber: public TVideoCaptureThread
{
protected: // ���������
//double Fps;

protected: // ������
TVideoGrabber* VideoGrabber;

Graphics::TBitmap* ConvertBitmap;

RDK::UBitmap ConvertUBitmap;

protected: // �������
/// ������������ ��� ��������� ���������� �����
HANDLE VideoGrabberCompleted;

protected: // ��������� ����������


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabber(void);
// --------------------------
// --------------------------
// ���������� �����������
// --------------------------
/// ������������� �������� FPS
double GetFps(void) const;
bool SetFps(double fps);
// --------------------------
// ���������� �������
// --------------------------
/// ���������� �������� � xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// �������� � ���������� �������� �� xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
TVideoGrabber* GetVideoGrabber(void);

void __fastcall OnFrameCaptureCompleted(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId);

void __fastcall VideoGrabberLog(TObject *Sender,
	  TLogType LogType, String Severity, String InfoMsg);
void __fastcall VideoGrabberDeviceLost(TObject *Sender);

void __fastcall VideoGrabberFrameBitmap(TObject *Sender,
	  pFrameInfo FrameInfo, pFrameBitmapInfo BitmapInfo);

void __fastcall VideoGrabberPlayerEndOfStream(TObject *Sender);

virtual void __fastcall Calculate(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

/// ���������� ����� ����������� � ������������������
virtual long long GetNumBitmaps(void) const;

/// ������������� ������� ������� � ������������������
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);

/// ���������� 0 ���� ���� ��������� �� ����������
/// ���������� 1 ���� ���� ��� ����������� � ���������
/// ���������� 2 ���� ���� ���� ����������� � ���������
virtual int CheckConnection(void) const;

virtual void __fastcall ARecreateCapture(void);
// --------------------------
};

class TVideoCaptureThreadVideoGrabberAvi: public TVideoCaptureThreadVideoGrabber
{
protected: // ���������
/// ��� �����
RDK::UELockVar<std::string> FileName;

RDK::UELockVar<bool> ProcessAllFramesFlag;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberAvi(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabberAvi(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// ��� ������ ����� ������
std::string GetFileName(void) const;
bool SetFileName(const std::string& value);

bool GetProcessAllFramesFlag(void) const;
bool SetProcessAllFramesFlag(bool value);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// ���������� �������� � xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// �������� � ���������� �������� �� xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall AStart(void);

virtual void __fastcall AStop(void);


// ������ ��������� ����� � �����������
virtual bool SetLastTimeStampSafe(double time_stamp);

void __fastcall AfterCalculate(void);
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall APauseCapture(void);
// --------------------------

};


class TVideoCaptureThreadVideoGrabberCamera: public TVideoCaptureThreadVideoGrabber
{
protected: // ���������
RDK::UELockVar<int> CameraIndex;
RDK::UELockVar<int> InputIndex;
RDK::UELockVar<int> SizeIndex;
RDK::UELockVar<int> SubtypeIndex;
RDK::UELockVar<int> AnalogIndex;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberCamera(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabberCamera(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
int GetCameraIndex(void) const;
int GetInputIndex(void) const;
int GetSizeIndex(void) const;
int GetSubtypeIndex(void) const;
int GetAnalogIndex(void) const;

bool Init(int camera_index, int input_index, int size_index, int subtype_index, int analog_index);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// ���������� �������� � xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// �������� � ���������� �������� �� xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall AStart(void);

virtual void __fastcall AStop(void);
// --------------------------


// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall APauseCapture(void);
// --------------------------

};

class TVideoCaptureThreadVideoGrabberIpCamera: public TVideoCaptureThreadVideoGrabber
{
protected: // ���������
/// ��� ������
RDK::UELockVar<String> Url;

/// ��� ������������
RDK::UELockVar<String> UserName;

/// ������
RDK::UELockVar<String> Password;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberIpCamera(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabberIpCamera(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// ��� ������
String GetUrl(void) const;

/// ��� ������������
String GetUserName(void) const;

/// ������
String GetPassword(void) const;

bool Init(const String camera_url, const String user_name, const String user_password);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// ���������� �������� � xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// �������� � ���������� �������� �� xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall AStart(void);

virtual void __fastcall AStop(void);
// --------------------------


// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall APauseCapture(void);
// --------------------------

};


class TVideoCaptureThreadSharedMemory: public TVideoCaptureThread
{
protected: // ���������
/// ������ ������ ����� ������
RDK::UELockVar<int> PipeIndex;

/// ��� ������ ����� ������
RDK::UELockVar<std::string> PipeName;

/// ������ ������ ����� ������
RDK::UELockVar<int> SharedMemoryPipeSize;

protected: // ������
/// ����� ������ ������ �� ������
std::vector<char> PipeBuffer;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadSharedMemory(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadSharedMemory(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// ������ ������ ����� ������
int GetPipeIndex(void) const;
bool SetPipeIndex(int value);

/// ��� ������ ����� ������
std::string GetPipeName(void) const;
bool SetPipeName(const std::string& value);

/// ������ ������ ����� ������
int GetSharedMemoryPipeSize(void) const;

/// ���������� ����� ����������� � ������������������
virtual long long GetNumBitmaps(void) const;

/// ������������� ������� ������� � ������������������
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> New(TVideoOutputFrame *frame, bool create_suspended);

/// ���������� �������� � xml
virtual bool ASaveParameters(RDK::USerStorageXML &xml);

/// �������� � ���������� �������� �� xml
virtual bool ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall AStart(void);

virtual void __fastcall AStop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

virtual void __fastcall UnsafeInit(void);
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
virtual void __fastcall ARunCapture(void);

virtual void __fastcall APauseCapture(void);
// --------------------------

};

#endif
