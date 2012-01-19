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

//using namespace RDK::VCapture;
using namespace RDK;
//---------------------------------------------------------------------------
class TVideoOutputFrame : public TFrame
{
__published:    // IDE-managed Components
    TGroupBox *GroupBox;
    TImage *Image;
    TPanel *Panel1;
    TTrackBar *ImageTrackBar;
    TButton *StopButton;
    TButton *StartButton;
    TTimer *Timer;
    TMaskEdit *TimeEdit;
    TPopupMenu *PopupMenu;
    TMenuItem *N1;
    void __fastcall ImageTrackBarChange(TObject *Sender);
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

private:    // User declarations
public:        // User declarations
    __fastcall TVideoOutputFrame(TComponent* Owner);
    __fastcall ~TVideoOutputFrame(void);

// ����� ���������� ������� �����
TVideoOutputToolsForm* MyVideoOutputToolsForm;

// �������� �����
//VACapture *Capture;

// �������� �����������
UBitmap BmpSource;

// ����� ���������
UBitmap BmpCanvas;

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
size_t FigureIndex;

// ������ ������� ������������� �����
size_t PointIndex;

// ���� �������� ������
bool FigureFlag;

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
// ============================================================


// ������������� ����� �������� �����
// ���� capture == 0, �� �������� ������� ��������
bool SetCapture(VACapture *capture);

// ������������� ��������� �����������
bool SetBitmap(const UBitmap &bmp);

// ������������� �������� ����
bool SetTitle(String title);

// ������������� ���������� ������������ ����
// ���� ��� ���������� == -1 �� ���� �� ������������
bool SetFrameRect(int x,int y, int x_width, int y_height, TColor color=(TColor)0x00FF0000);

// ��������� ��������� ����
bool UpdateVideo(void);

// ������������ ������� ��������� �����������
void DrawCapture(VACapture *capture, Graphics::TBitmap *bmp);

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
