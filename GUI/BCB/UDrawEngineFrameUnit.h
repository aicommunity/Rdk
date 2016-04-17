//---------------------------------------------------------------------------

#ifndef UDrawEngineFrameUnitH
#define UDrawEngineFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFormUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include "../../Deploy/Include/rdk_cpp_initdll.h"
//#include "myrdk.h"
#include <Vcl.ComCtrls.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UClassesListFrameUnit.h"
#include <Vcl.Menus.hpp>
#include <Vcl.CustomizeDlg.hpp>
#include "UComponentsLinksHintFormUnit.h"

class TUComponentsListFrame;
//---------------------------------------------------------------------------
class TUDrawEngineFrame : public TUVisualControllerFrame
{
__published:    // IDE-managed Components
    TPanel *Panel1;
    TPanel *Panel2;
    TScrollBox *ScrollBox;
    TImage *Image;
    TSplitter *Splitter1;
    TUClassesListFrame *UClassesListFrame;
    TPopupMenu *PopupMenu;
    TMenuItem *Breakinputlink1;
    TPanel *Panel3;
    TLabeledEdit *RectWidthLabeledEdit;
    TLabel *Label1;
    TComboBox *FontTypeComboBox;
    TLabel *Label2;
    TComboBox *FontSizeComboBox;
    TLabeledEdit *RectHeightLabeledEdit;
    TButton *ApplyButton;
    TButton *RestoreButton;
    TMenuItem *Createlonglink1;
    TMenuItem *Finishlonglink1;
    TMenuItem *Cancellonglink1;
    TMenuItem *N2;
	TMenuItem *N1;
	TMenuItem *GUI1;
	TMenuItem *N3;
	TMenuItem *Rename1;
	TMenuItem *N4;
	TMenuItem *Delete1;
	TMenuItem *StartMoving1;
	TMenuItem *Finishmoving1;
	TMenuItem *Cancelmoving1;
	TMenuItem *N5;
	TMenuItem *Reset1;
	TMenuItem *Calculate1;
	TMenuItem *N6;
	TMenuItem *CopynametoClipboard1;
	TMenuItem *CopylongnametoClipboard1;
	TMenuItem *CopyclasstoClipboard1;
	TCheckBox *ShowLinkDetailCheckBox;
    void __fastcall ImageMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall ImageMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall ImageMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall ScrollBoxResize(TObject *Sender);
    void __fastcall UClassesListFrameStringGridMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall UClassesListFrameStringGridMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall ImageDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall ImageDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State,
          bool &Accept);
    void __fastcall Breakinputlink1Click(TObject *Sender);
    void __fastcall ApplyButtonClick(TObject *Sender);
    void __fastcall RestoreButtonClick(TObject *Sender);
    void __fastcall FontTypeComboBoxSelect(TObject *Sender);
    void __fastcall Createlonglink1Click(TObject *Sender);
    void __fastcall Finishlonglink1Click(TObject *Sender);
    void __fastcall Cancellonglink1Click(TObject *Sender);
	void __fastcall FrameContextPopup(TObject *Sender, TPoint &MousePos, bool &Handled);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall Delete1Click(TObject *Sender);
	void __fastcall UClassesListFrameTreeViewMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall UClassesListFrameTreeViewMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall UClassesListFrameLibComponentListStringGridMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
	void __fastcall UClassesListFrameLibComponentListStringGridMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall UClassesListFrameAddClassButtonClick(TObject *Sender);
	void __fastcall StartMoving1Click(TObject *Sender);
	void __fastcall Finishmoving1Click(TObject *Sender);
	void __fastcall Cancelmoving1Click(TObject *Sender);
	void __fastcall CopynametoClipboard1Click(TObject *Sender);
	void __fastcall CopylongnametoClipboard1Click(TObject *Sender);
	void __fastcall CopyclasstoClipboard1Click(TObject *Sender);


private:    // User declarations
public:        // User declarations
    __fastcall TUDrawEngineFrame(TComponent* Owner);

// ��������� �� ����� ���������� ������� ���������
TUComponentsListFrame * ComponentsListFrame;

// ��� ����������, ���������� �������� ����� ����������
std::string ComponentName;

// ����� ��������� ��������� ����
RDK::UDrawEngine DrawEngine;

// ����������� ������
RDK::UGraphics Graph;

// ��� ������
std::string FontType;

// ������ ������
int FontSize;

// �����
RDK::UBitmapFont Font;

// ����� ���������
RDK::UBitmap GraphCanvas,ShowCanvas;

// Xml �������� ����
RDK::USerStorageXML NetXml;

TShiftState DownShift;

int StartX,StartY,StopX,StopY,PopupX,PopupY;

std::string StartName,StopName;

std::string MoveComponentName;

std::string BreakLinkComponentName;

std::string FontFileName;

// ����, ������������ � �������� drag&drop
bool DragDropFlag;

// ����, ������������ � �������� �������� "�������" �����
bool LongLinkFlag;

// ����, ������������ � �������� ����������� �������
bool MoveFlag;

/// ����, ������������ �� ����� ������ ��������� � ������
bool LinksHintVisibility;


// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������� ������
virtual void ABeforeReset(void);

// �����, ���������� ����� ������ ������
virtual void AAfterReset(void);

// �����, ���������� ����� ����� �������
virtual void ABeforeCalculate(void);

// �����, ���������� ����� ���� �������
virtual void AAfterCalculate(void);

// ���������� ����������
virtual void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------

// -----------------------------
// ������ ���������� �������
// -----------------------------
void SetNet(const std::string &comp_name);

void ReloadNet(void);

void SelectComponent(const std::string &comp_name);

// ��������� ��������� ���������� � �������� �����������
void SaveComponentPosition(const std::string &name);
// -----------------------------
};

#endif
