//---------------------------------------------------------------------------

#ifndef TUVisualControllerFrameUnitH
#define TUVisualControllerFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <string>
#include "myrdk.h"
//---------------------------------------------------------------------------
#pragma warn -8130
class TUVisualControllerFrame : public TFrame, public RDK::UIVisualController
{
__published:	// IDE-managed Components
private:	// User declarations
public:		// User declarations
	__fastcall TUVisualControllerFrame(TComponent* Owner);
	__fastcall ~TUVisualControllerFrame(void);

bool UpdateInterfaceFlag;

// ����, ����������� ���������� ����������, ���� ���� ������ �� �����
bool AlwaysUpdateFlag;

// ������� ��� ������������ ���������� ������ (�����������)
std::string ComponentControlName;

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

// ���������� ����������
// ���� force_update == true, �� ��������� �����������
// ��� ����������� �� UpdateInterval
virtual void UpdateInterface(bool force_update=true);
virtual void AUpdateInterface(void);

// ���������� ���������� ��� ����������
virtual std::string GetName(void);

// ��������� ��������� ���������� � xml
virtual void SaveParameters(RDK::Serialize::USerStorageXML &xml);
virtual void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void LoadParameters(RDK::Serialize::USerStorageXML &xml);
virtual void ALoadParameters(RDK::Serialize::USerStorageXML &xml);

// ���������� ������� ������ ������������ ����������
// ������� ��� ������������ ���������� ������ (�����������)
const std::string& GetComponentControlName(void) const;
bool SetComponentControlName(const std::string& name);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUVisualControllerFrame *UVisualControllerFrame;
//---------------------------------------------------------------------------
#pragma warn .8130
#endif
