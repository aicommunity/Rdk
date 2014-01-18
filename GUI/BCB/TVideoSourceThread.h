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

protected: // ������
/// ��������� ����� ���������� �����
long long LastTimeStamp;

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
bool GetRepeatFlag(void) const;
bool SetRepeatFlag(bool value);
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ��������� �� ���������
TVideoOutputFrame *GetFrame(void) const;
bool SetFrame(TVideoOutputFrame * frame);
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
bool ReadSourceSafe(RDK::UBitmap& dest, long long &time_stamp, bool reflect);

/// ���������� ����������� � ���� � �����������
bool WriteSourceSafe(const RDK::UBitmap& src, long long time_stamp, bool reflect);
bool WriteSourceSafe(Graphics::TBitmap *src, long long time_stamp, bool reflect);

// ������ ��������� ����� � �����������
virtual bool SetLastTimeStampSafe(long long time_stamp);
// --------------------------
};

class TVideoCaptureThreadBmp: public TVideoCaptureThread
{
protected: // ���������
/// ��� ����� �����������
std::string FileName;

protected: // ��������� �����������
RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

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



RDK::UBitmap TempSource;
Graphics::TBitmap* TempBitmap;

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
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

// ��������� ��������� �������� �� ������ � ������� ����
bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

// ������ ��������� ����� � �����������
virtual bool SetLastTimeStampSafe(long long time_stamp);
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
TVideoGrabber* GetVideoGrabber(void);

void __fastcall OnFrameCaptureCompleted(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId);

virtual void __fastcall Calculate(void);

virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

/// ���������� ����� ����������� � ������������������
virtual long long GetNumBitmaps(void) const;

/// ������������� ������� ������� � ������������������
virtual long long GetPosition(void) const;
virtual bool SetPosition(long long index);
// --------------------------
};

class TVideoCaptureThreadVideoGrabberAvi: public TVideoCaptureThreadVideoGrabber
{
protected: // ���������
/// ��� �����
std::string FileName;

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
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
virtual void __fastcall Start(void);

virtual void __fastcall Stop(void);


// ������ ��������� ����� � �����������
virtual bool SetLastTimeStampSafe(long long time_stamp);

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
virtual void __fastcall BeforeCalculate(void);

virtual void __fastcall AfterCalculate(void);

virtual void __fastcall Calculate(void);

virtual void __fastcall UnsafeInit(void);
// --------------------------
};

#endif
