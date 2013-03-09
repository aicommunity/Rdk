//---------------------------------------------------------------------------

#ifndef UDrawEngineFormUnitH
#define UDrawEngineFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFormUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include "rdk_initdll.h"
#include "myrdk.h"
#include <Vcl.ComCtrls.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UClassesListFrameUnit.h"
#include <Vcl.Menus.hpp>
//---------------------------------------------------------------------------
class TUDrawEngineForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
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

private:	// User declarations
public:		// User declarations
	__fastcall TUDrawEngineForm(TComponent* Owner);

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

int StartX,StartY,StopX,StopY;

std::string StartName,StopName;

std::string MoveComponentName;

std::string BreakLinkComponentName;

std::string FontFileName;

// ����, ������������ � �������� drag&drop
bool DragDropFlag;

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
//---------------------------------------------------------------------------
extern PACKAGE TUDrawEngineForm *UDrawEngineForm;
//---------------------------------------------------------------------------
#endif
