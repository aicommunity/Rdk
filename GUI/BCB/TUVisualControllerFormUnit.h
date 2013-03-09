//---------------------------------------------------------------------------

#ifndef TUVisualControllerFormUnitH
#define TUVisualControllerFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "myrdk.h"

//---------------------------------------------------------------------------
#pragma warn -8130
class TUVisualControllerForm : public TForm, public RDK::UIVisualController
{
__published:	// IDE-managed Components
private:	// User declarations
public:		// User declarations
	__fastcall TUVisualControllerForm(TComponent* Owner);
	__fastcall ~TUVisualControllerForm(void);

protected:
bool UpdateInterfaceFlag;

// ����, ����������� ���������� ����������, ���� ���� ������ �� �����
bool AlwaysUpdateFlag;

// ������� ��� ������������ ���������� ������ (�����������)
std::string ComponentControlName;

// ��������� ���� ��� �������� ������������ ����������
std::string PureFormCaption;

// �������� ���������� ���������� � ������������� ��� �����
// (���� =0) �� ��������� �� ����������� ��� �����
long UpdateInterval;

// ����� ���������� ����������
DWORD LastUpdateTime;

public:
// ����, ���������� ��� ���� ������
static bool CalculationModeFlag;

public:
// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������� ������
virtual void BeforeReset(void);
virtual void ABeforeReset(void);

// �����, ���������� ����� ������ ������
virtual void AfterReset(void);
virtual void AAfterReset(void);

// �����, ���������� ����� ����� �������
virtual void BeforeCalculate(void);
virtual void ABeforeCalculate(void);

// �����, ���������� ����� ���� �������
virtual void AfterCalculate(void);
virtual void AAfterCalculate(void);

// ���� force_update == true, �� ��������� �����������
// ��� ����������� �� UpdateInterval
virtual void UpdateInterface(bool force_update=true);
virtual void AUpdateInterface(void);

// ���������� ���������� ��� ����������
virtual std::string GetName(void);

// ��������� ��������� ���������� � xml
virtual void SaveParameters(RDK::USerStorageXML &xml);
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void LoadParameters(RDK::USerStorageXML &xml);
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// ���������� ������� ������ ������������ ����������
// ������� ��� ������������ ���������� ������ (�����������)
const std::string& GetComponentControlName(void) const;
bool SetComponentControlName(const std::string& name);
// -----------------------------
};

// --------------------------
// ��������������� ������� ������������
// --------------------------
// ��������� ������ ��������� ����� � xml
void SaveFormPosition(RDK::USerStorageXML &xml, TForm *form);

// ��������� ������ ��������� ����� �� xml
void LoadFormPosition(RDK::USerStorageXML &xml, TForm *form);
// --------------------------

//---------------------------------------------------------------------------
extern PACKAGE TUVisualControllerForm *UVisualControllerForm;
//---------------------------------------------------------------------------
#endif
