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
protected: // ���������
/// �������� �����
int SourceMode;

/// ������ ������ � ���������� ���������, ����������� ������
int ChannelIndex;

/// ����� ������������� � ��������
/// 0 - ��� ������������� � ��������
/// 1 - ������������� � �������� ��������
int SyncMode;

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

// ������ ����� ��������� �� ����� ���� ����������
int FrameIndex;

// ����� ������
// 0 - Bmp
// 1 - Avi
// 2 - Camera
// 3 - IP Camera
// 4 - Image sequence
// 5 - Http Server
// 6 - Shared memory
int Mode;

//Graphics::TBitmap* ConvertBitmap;

// ����� ���������� ������� �����
TVideoOutputToolsForm* MyVideoOutputToolsForm;

// ����� ���������� �������������� �����
TVideoGrabberControlForm* MyVideoGrabberControlForm;

// ��������� �� ����� ������ ����������-���������
TUComponentsListForm *MyComponentsListForm;

// ����� ������������
TVideoCaptureThread* CaptureThread;

// �������� �����������
RDK::UBitmap BmpSource,ReflectedBmpSource;

// ����� ���������
RDK::UBitmap BmpCanvas;

// ������ �������
RDK::UGraphics Graph;

// ��������� ���������
RDK::MGraphics<double,2> GeometryGraphics;

// ������� ���������
RDK::MGraphics<double,2> CurrentGeometryGraphics;

// ������ ����������� ��� ������ ������������������ ��������
//std::vector<std::string> BmpSequenceNames;

// ���� �� ������������������ �����������
//std::string BmpSequencePathName;

// ������� ���� � ������������������ ��������
//int CurrentBmpSequenceIndex;

protected: // �������� ��������� ����� � �����-������ ����������
// ��� ����������, � �������� �������� ��������
std::string LinkedComponentName;

// ����� ��������
// 0 - �� �����
// 1 - � ������
// 2 - � ��������
int LinkedMode;

// �����, � �������� �������� ��������
std::string LinkedIndex;

// ��������, � �������� �������� ��������
std::string LinkedComponentPropertyName;


public:
// ��������� ����� �������
//long long ServerTimeStamp;

// ������� ����������� ������
//RDK::MGeometry<double,2> Figure;

// ������ ������� ������������� ������
int FigureIndex;

// ������ ������� ������������� �����
int PointIndex;

// ���� �������� ������
//bool FigureFlag;

// ���� �������� �����
// 0 - ���������
// 1 - �������� ����� �����
// 2 - �������������� ������������
int PointFlag;

// ���� ���������� ��������� ����
bool ZoneSelectEnable;

// �������, ��������������� ��� ��������� ��������� ����
HANDLE ZoneSelectEvent;

//int PipeIndex;
//std::string PipeName;
//int SharedMemoryPipeSize;

// ============================================================
// ��������� ����������
// ============================================================
// ���������� ������ ��������� ���� ��������
int x1b, y1b, x2b, y2b;

// ������������� ���������� ������ ��������� ���� ��������
int corrx1b, corry1b, corrx2b, corry2b;

// �������� ���������� ������ ��������� ���� ��������
double left,top,width,height;

// ���� ���������
TColor SelColor;

bool CorrSelectFlag;

// ���� ���������� ���������
bool UpdateFlag;

// ������� �������������� ����
//long CurrentFrameNumber;

// ������� ��������-�������� ������ � ���������
std::string SelectedComponentPName;
std::string SelectedComponentParameterName;

// ������� ���������� ���������-�������� ������ � ���������
std::string SelectedComponentSName;
std::string SelectedComponentStateName;

// ������� �������� �������-�������� ������ � ���������
std::string SelectedComponentPropertyMatrixName;
std::string SelectedComponentMatrixName;

//std::vector<char> PipeBuffer;

//int LastReadSequenceIndex;

// ����, ������������ ��� ��������� ������������
bool IsStarted;
// ============================================================



/// ���������� ��������� �����
bool DestroyCaptureThread(void);

// ������������� ������ avi-������
void InitByAvi(const String &filename);

// ������������� ������ bmp-������
void InitByBmp(const String &filename);

// ������������� ��������� �����������
bool InitByBmp(const RDK::UBitmap &bmp);

// ������������� ������ �������
void InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index);

// ������������� ������ IP-�������
void InitByIPCamera(const String camera_url, const String user_name, const String user_password);

// ������������� ������������������� �����������
bool InitByImageSequence(const String &pathname);

// ������������� http-�������
bool InitByHttpServer(int listen_port);

// ������������� ����� ������
bool InitBySharedMemory(int pipe_index, const std::string &pipe_name);

// ��������� ��������� �������� �� ������ � ������� ����
//bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

// ������������� �������� ����
bool SetTitle(String title);

// ������������� ���������� ������������ ����
// ���� ��� ���������� == -1 �� ���� �� ������������
bool SetFrameRect(int x,int y, int x_width, int y_height, TColor color=(TColor)0x00FF0000);

// ��������� ��������� ����
bool UpdateVideo(void);

// ������������ ������� ��������� �����������
void DrawCapture(Graphics::TBitmap *bmp);

// ��������� ������ �����
void UpdateGeometryList(TCheckListBox *GeometryCheckListBox, TCheckListBox *PointsCheckListBox);

// ����� ��������� ������������� ����
void __fastcall TVideoOutputFrame::DrawFrameRect(TImage *image, int x1, int y1, int x2,
												int y2, int framewidth, TColor color);

// ��������� ��������� ������� ������
void AddFigureRect(double l,double t,double w,double h);

// ������������� ������� �������
void SetSampleGeometryGraphics(RDK::MGraphics<double,2>& samplegraphics);

// -------------------------
// ������ ����� ������ ����� ��������� �� ���������� � ���������� ��������� ���������
// -------------------------
// ���������� ����� ����� � �������� ����������
void SendToComponentPropertyMatrix(const std::string &stringid, const std::string &parameter_name, int figure_index);

// ���������� ����� ����� � �������� ����������
void SendToComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index);

// ���������� ����� ����� � ���������� ��������� ����������
void SendToComponentState(const std::string &stringid, const std::string &state_name, int figure_index);

// ��������� ����� ����� �� ��������� ����������
void ReceiveFromComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index);

// ��������� ����� ����� �� ���������� ��������� ����������
void ReceiveFromComponentState(const std::string &stringid, const std::string &state_name, int figure_index);
// -------------------------

// -------------------------
// ������ ������ ����������� �� �����-������ ���������
// -------------------------
// ���������� ����������� � ��������� ���������
void SendToComponentIO(void);
// -------------------------

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� �������
void ABeforeReset(void);

// �����, ���������� ����� ����� �������
virtual void ABeforeCalculate(void);

// �����, ���������� ����� ���� �������
virtual void AAfterCalculate(void);

// ���������� ����������
virtual void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputFrame *VideoOutputFrame;
//---------------------------------------------------------------------------
#endif
