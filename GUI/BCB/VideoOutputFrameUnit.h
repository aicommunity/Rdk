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
	// ������� �� ���� ������������� ���� ������ ����� ������ ������
	void __fastcall OnClickVideoCodec(TObject *Sender);

	// ������� �� ���� ������������� ���� ������ ����� ������ ������
	void __fastcall OnClickRecordingMethod(TObject *Sender);

	void __fastcall RecordingFrameVideoCompressorComboBoxChange(TObject *Sender);
	void __fastcall StartStreamingToolButtonClick(TObject *Sender);
	void __fastcall RecordingFrameRecordingMethodComboBoxChange(TObject *Sender);

private:    // User declarations
public:        // User declarations
    __fastcall TVideoOutputFrame(TComponent* Owner);
    __fastcall ~TVideoOutputFrame(void);

/// ������ �������� �������������� �������
std::map<int, RDK::USerStorageXML> VideoSourceOptions;

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
//TVideoGrabberControlForm* MyVideoGrabberControlForm;

// ��������� �� ����� ������ ����������-���������
TUComponentsListForm *MyComponentsListForm;

TVideoCaptureOptionsForm *VideoCaptureOptionsForm;

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
String LicenseString;
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

protected:
bool RepeatSequenceFlag;
bool RepeatVideoFlag;
bool ProcessAllFramesFlag;

int SavePictureIndex;

public:
// ============================================================

// ---------------------------
// ������ ���������� ��������������� ����������� �����
// ---------------------------
/// ������� ����� ���������� ����� �� ������� �����������
RDK::UEPtr<TVideoCaptureThread> TakeVideoCapureThread(int mode, TVideoOutputFrame *frame, bool create_suspended);

/// ���������� �������� ����
void ReturnVideoCapureThread(RDK::UEPtr<TVideoCaptureThread> thread);
// ---------------------------

// -----------------------------
// ������ ���������� �������������
// -----------------------------
/// ������������� ������� � �������� ������
/// ���� mode == -1 �� ������������ ���������������� � ������� ������
void Init(int mode=-1);

void Init(int mode, RDK::USerStorageXML &raw_xml_data);


/// ��������������� �������
void UnInit(void);

/// ������ �������
void Start(void);

/// ������� �������
void Pause(void);

/// ������ �������� ����������� � bmp
void ReadSourceSafe(RDK::UBitmap &bmp, double &time_stamp, bool reflect);

/// ��������� ��������� ������� �� id ������
/// 0 - �� �������
/// 1 - �������
virtual int CheckCaptureState(void) const;
// -----------------------------

/// ������������� ��������� ��������
void InitPrimarySettings(void);
// ���������� ����� ���������� �������������� �����
//TVideoGrabberControlForm* GetMyVideoGrabberControlForm(void);

// ���������� ��������� �����
bool DestroyCaptureThread(void);

// ������������� ������ avi-������
void InitByAvi(const String &filename);

// ������������� ������ bmp-������
void InitByBmp(const String &filename, double fps);

// ������������� ��������� �����������
bool InitByBmp(const RDK::UBitmap &bmp, double fps);

// ������������� ������ �������
void InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index);

// ������������� ������ IP-�������
void InitByIPCamera(const String camera_url, const String user_name, const String user_password);

// ������������� ������������������� �����������
bool InitByImageSequence(const String &pathname, double fps);

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

/// ��������� ����� ������
/// ������� ���������� ������ ������
int AddFigure(TColor color);

/// ���������� ����� �����
int GetNumFigures(void);

/// ������� ������
void DelFigure(int figure_index);

/// ������� ��� ������
void DelAllFigures(void);

/// ������������ ��� ������
void EditFigureName(int figure_index, const std::string &figure_name);

/// ���������� ����� ����� � ������
int GetNumPoints(int figure_index);

/// ���������� �����
const RDK::MVector<double,2> GetPoint(int figure_index, int point_index);

/// ��������� ����� �����
/// ������� ���������� ������ �����
int AddPoint(int figure_index, const RDK::MVector<double,2> &point_data);

/// ��������� ����� �����
/// ��� ����� �������� �������
/// ������� ���������� ������ �����
int AddPoint(int figure_index, const std::string &point_name, const RDK::MVector<double,2> &point_data);

/// ������� �����
void DelPoint(int figure_index, int point_index);

/// ������� ��� �����
void DelAllPoints(int figure_index);

/// ������������ �����
void EditPoint(int figure_index, int point_index, const RDK::MVector<double,2> &point_data);

/// ������������ ��� �����
void EditPointName(int figure_index, int point_index, const std::string &point_name);

// ������������� ������� �������
void SetSampleGeometryGraphics(RDK::MGraphics<double,2>& samplegraphics);

/// ���� ������� ��������������� ������� ����� ����������
bool SetRepeatVideoFlag(bool value);
bool SetRepeatSequenceFlag(bool value);
bool SetProcessAllFramesFlag(bool value);

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
// ����������� ��������� ����
void DynamicMenuFilling(TMenuItem* target, std::vector<std::string> &itemsList);
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

public:
void __fastcall SavePicture(String directory, String filename);

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputFrame *VideoOutputFrame;
//---------------------------------------------------------------------------
#endif
