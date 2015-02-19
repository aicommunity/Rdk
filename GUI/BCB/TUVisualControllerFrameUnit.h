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
RDK::UELockVar<long> UpdateInterval;

// ����� ���������� ����������
DWORD LastUpdateTime;

bool CalculationStepUpdatedFlag;

/// ����, ����������� �������� �� ������������� ������ ����� ����������� ����������
bool CheckModelFlag;

/// �����, ����������� �� ���������� ����������
unsigned long long UpdateTime;

public:
// ����, ���������� ��� ���� ������
static RDK::UELockVar<bool> CalculationModeFlag;

public:
// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� �������� �������
virtual void AfterLoadProject(void);
virtual void AAfterLoadProject(void);

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
virtual void UpdateInterface(bool force_update=false);
virtual void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void ClearInterface(void);
virtual void AClearInterface(void);

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

// ��������� ������ ���������� �����������
/// ���������� ���� ��������� ����������� � ���� �������� �����
virtual void ResetCalculationStepUpdatedFlag(void);

/// ���������� ���� ��������� ����������� � ���� �������� �����
virtual void SetCalculationStepUpdatedFlag(void);

/// ���������� ��������� ����� ��������� ����������� � ���� �������� �����
virtual bool GetCalculationStepUpdatedFlag(void);

/// ���������� ����� ���������� ���������� (��)
virtual unsigned long long GetUpdateTime(void);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUVisualControllerFrame *UVisualControllerFrame;
//---------------------------------------------------------------------------
#pragma warn .8130
#endif
