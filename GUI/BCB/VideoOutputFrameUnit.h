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

class TVideoGrabberControlForm;

//using namespace RDK::VCapture;
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
    TVideoGrabber *VideoGrabber;
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
	TUHttpServerFrame *UHttpServerFrame;
	TGroupBox *GroupBox1;
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
    void __fastcall VideoGrabberFrameCaptureCompleted(TObject *Sender, Pointer FrameBitmap,
          int BitmapWidth, int BitmapHeight, DWORD FrameNumber, __int64 FrameTime,
          TFrameCaptureDest DestType, UnicodeString FileName, bool Success,
          int FrameId);
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
	void __fastcall UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
          TIdHTTPResponseInfo *AResponseInfo);


private:    // User declarations
public:        // User declarations
    __fastcall TVideoOutputFrame(TComponent* Owner);
    __fastcall ~TVideoOutputFrame(void);

// ����� ������
// 0 - Bmp
// 1 - Avi
// 2 - Camera
// 3 - IP Camera
// 4 - Image sequence
// 5 - Http Server
int Mode;

Graphics::TBitmap* ConvertBitmap;

// ����� ���������� ������� �����
TVideoOutputToolsForm* MyVideoOutputToolsForm;

// ����� ���������� �������������� �����
TVideoGrabberControlForm* MyVideoGrabberControlForm;

// ��������� �� ����� ������ ����������-���������
TUComponentsListForm *MyComponentsListForm;

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
std::vector<std::string> BmpSequenceNames;

// ���� �� ������������������ �����������
std::string BmpSequencePathName;

// ������� ���� � ������������������ ��������
int CurrentBmpSequenceIndex;

protected: // �������� ��������� ����� � �����-������ ����������
// ��� ����������, � �������� �������� ��������
std::string LinkedComponentName;

// ����� ��������
// 0 - �� �����
// 1 - � ������
// 2 - � ��������
int LinkedMode;

// �����, � �������� �������� ��������
int LinkedIndex;

// ��������, � �������� �������� ��������
std::string LinkedComponentPropertyName;


public:
// ��������� ����� �������
long long ServerTimeStamp;

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
long CurrentFrameNumber;

// ������� ��������-�������� ������ � ���������
std::string SelectedComponentPName;
std::string SelectedComponentParameterName;

// ������� ���������� ���������-�������� ������ � ���������
std::string SelectedComponentSName;
std::string SelectedComponentStateName;

// ������� �������� �������-�������� ������ � ���������
std::string SelectedComponentPropertyMatrixName;
std::string SelectedComponentMatrixName;

int LastReadSequenceIndex;
// ============================================================


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

// ��������� ��������� �������� �� ������ � ������� ����
bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

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
// �����, ���������� ����� ����� �������
virtual void ABeforeCalculate(void);

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
