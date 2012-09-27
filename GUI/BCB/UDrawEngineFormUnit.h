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
//---------------------------------------------------------------------------
class TUDrawEngineForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TScrollBox *ScrollBox;
	TImage *Image;
	void __fastcall FormResize(TObject *Sender);
	void __fastcall ImageMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
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

// ����� ���������
RDK::UBitmap GraphCanvas,ShowCanvas;

// Xml �������� ����
RDK::Serialize::USerStorageXML NetXml;

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
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUDrawEngineForm *UDrawEngineForm;
//---------------------------------------------------------------------------
#endif
