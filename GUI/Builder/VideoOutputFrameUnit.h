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
#include "rdk_builder.h"
#include <CheckLst.hpp>
#include "VideoOutputToolsFormUnit.h"
#include <Menus.hpp>
#include "VidGrab.hpp"

class TVideoGrabberControlForm;

//using namespace RDK::VCapture;
//---------------------------------------------------------------------------
class TVideoOutputFrame : public TFrame
{
__published:	// IDE-managed Components
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

private:	// User declarations
public:		// User declarations
	__fastcall TVideoOutputFrame(TComponent* Owner);
	__fastcall ~TVideoOutputFrame(void);

// ����� ������
// 0 - Bmp
// 1 - Avi
// 2 - Camera
int Mode;

Graphics::TBitmap* ConvertBitmap;

// ����� ���������� ������� �����
TVideoOutputToolsForm* MyVideoOutputToolsForm;

// ����� ���������� �������������� �����
TVideoGrabberControlForm* MyVideoGrabberControlForm;

// �������� �����������
RDK::UBitmap BmpSource;

// ����� ���������
RDK::UBitmap BmpCanvas;

// ������ �������
RDK::UGraphics Graph;

// ��������� ���������
RDK::MGraphics<double> GeometryGraphics;

protected:
// ������� ����������� ���������
RDK::MGraphics<double> SampleGeometryGraphics;

public:
// ������� ����������� ������
RDK::MGeometry<double> Figure;

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
// ============================================================
// ������������� ������ avi-������
void InitByAvi(const String &filename);

// ������������� ������ bmp-������
void InitByBmp(const String &filename);

// ������������� ��������� �����������
bool InitByBmp(const RDK::UBitmap &bmp);

// ������������� ������ �������
void InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index);

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
void SetSampleGeometryGraphics(RDK::MGraphics<double>& samplegraphics);

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputFrame *VideoOutputFrame;
//---------------------------------------------------------------------------
#endif
