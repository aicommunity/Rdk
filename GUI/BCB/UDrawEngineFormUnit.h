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
//---------------------------------------------------------------------------
class TUDrawEngineForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TScrollBox *ScrollBox;
	TImage *Image;
	TRichEdit *RichEdit1;
	TSplitter *Splitter1;
	void __fastcall FormResize(TObject *Sender);
	void __fastcall ImageMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall ImageMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall ImageMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);

private:	// User declarations
public:		// User declarations
	__fastcall TUDrawEngineForm(TComponent* Owner);

// ��� ����������, ���������� �������� ����� ����������
std::string ComponentName;

// ����� ��������� ��������� ����
RDK::UDrawEngine DrawEngine;

// ����������� ������
RDK::UGraphics Graph;

// �����
RDK::UBitmapFont Font;

// ����� ���������
RDK::UBitmap GraphCanvas,ShowCanvas;

// Xml �������� ����
RDK::Serialize::USerStorageXML NetXml;

TShiftState DownShift;

int StartX,StartY,StopX,StopY;

std::string MoveComponentName;

std::string FontFileName;

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
virtual void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::Serialize::USerStorageXML &xml);
// -----------------------------

// -----------------------------
// ������ ���������� �������
// -----------------------------
void SetNet(const std::string &comp_name);

void SelectComponent(const std::string &comp_name);

// ��������� ��������� ���������� � �������� �����������
void SaveComponentPosition(const std::string &name);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUDrawEngineForm *UDrawEngineForm;
//---------------------------------------------------------------------------
#endif
