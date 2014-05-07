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
protected: // ���������
/// �������� �����
int SourceMode;

/// ������ ������ � ���������� ���������, ����������� ������
int ChannelIndex;

/// ����� ������������� � ��������
/// 0 - ��� ������������� � ��������
/// 1 - ������������� � �������� ��������
int SyncMode;

/// ���� ������� ��������������� ������� ����� ����������
bool RepeatFlag;

/// ����� �������������� ����� ���� �������
/// 0 - �� ������ ������
/// 1 - ���������� ������������ ������
/// 2 - ������� ����� �������� �������
int RestartMode;

protected: // ������
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


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThread(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThread(void);
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
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
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
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
virtual bool ASaveParameters(RDK::USerStorageXML &xml)=0;

/// �������� � ���������� �������� �� xml
virtual bool LoadParameters(RDK::USerStorageXML &xml);
virtual bool LoadParametersEx(RDK::USerStorageXML &xml);
virtual bool ALoadParameters(RDK::USerStorageXML &xml)=0;
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

virtual void __fastcall Stop(void);

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

/// ���������� 0 ���� ���� ��������� �� ����������
/// ���������� 1 ���� ���� ��� ����������� � ���������
/// ���������� 2 ���� ���� ���� ����������� � ���������
virtual int CheckConnection(void) const;

/// ���������� 0 ���� ������ �� �������� � 1 ���� �������
virtual int CheckCaptureThreadState(void) const;
// --------------------------
};

class TVideoCaptureThreadBmp: public TVideoCaptureThread
{
protected: // ���������
/// ��� ����� �����������
std::string FileName;

double Fps;

protected: // ��������� �����������
RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

double CurrentTimeStamp;


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
const std::string& GetFileName(void) const;
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
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);
// --------------------------
};

class TVideoCaptureThreadBmpSequence: public TVideoCaptureThread
{
protected: // ���������
/// ��� ���� �� ������ �����������
std::string PathName;

protected: // ��������� �����������
// ������ ����������� ��� ������ ������������������ ��������
std::vector<std::string> BmpSequenceNames;

// ������� ���� � ������������������ ��������
int CurrentBmpSequenceIndex;

int LastReadSequenceIndex;

double Fps;



RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

double CurrentTimeStamp;

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
const std::string& GetPathName(void) const;
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
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

// ��������� ��������� �������� �� ������ � ������� ����
bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

// ������ ��������� ����� � �����������
virtual bool SetLastTimeStampSafe(double time_stamp);
// --------------------------
};

class TVideoCaptureThreadHttpServer: public TVideoCaptureThread
{
protected: // ���������
/// ���� �������
int ListenPort;

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
protected: // ���������

protected: // ������
TVideoGrabber* VideoGrabber;

Graphics::TBitmap* ConvertBitmap;

protected: // �������
/// ������������ ��� ��������� ���������� �����
HANDLE VideoGrabberCompleted;

protected: // ��������� ����������
int ConnectionState;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended);
virtual __fastcall ~TVideoCaptureThreadVideoGrabber(void);
// --------------------------

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
// --------------------------
};

class TVideoCaptureThreadVideoGrabberAvi: public TVideoCaptureThreadVideoGrabber
{
protected: // ���������
/// ��� �����
std::string FileName;

bool ProcessAllFramesFlag;

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
const std::string& GetFileName(void) const;
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
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);


// ������ ��������� ����� � �����������
virtual bool SetLastTimeStampSafe(double time_stamp);

void __fastcall AfterCalculate(void);
// --------------------------
};


class TVideoCaptureThreadVideoGrabberCamera: public TVideoCaptureThreadVideoGrabber
{
protected: // ���������
int CameraIndex;
int InputIndex;
int SizeIndex;
int SubtypeIndex;
int AnalogIndex;

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
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);
// --------------------------
};

class TVideoCaptureThreadVideoGrabberIpCamera: public TVideoCaptureThreadVideoGrabber
{
protected: // ���������
/// ��� ������
String Url;

/// ��� ������������
String UserName;

/// ������
String Password;

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
const String& GetUrl(void) const;

/// ��� ������������
const String& GetUserName(void) const;

/// ������
const String& GetPassword(void) const;

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
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);
// --------------------------
};


class TVideoCaptureThreadSharedMemory: public TVideoCaptureThread
{
protected: // ���������
/// ������ ������ ����� ������
int PipeIndex;

/// ��� ������ ����� ������
std::string PipeName;

/// ������ ������ ����� ������
int SharedMemoryPipeSize;

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
const std::string& GetPipeName(void) const;
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
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

virtual void __fastcall UnsafeInit(void);
// --------------------------
};

#endif
