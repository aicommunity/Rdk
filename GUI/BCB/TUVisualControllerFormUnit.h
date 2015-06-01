//---------------------------------------------------------------------------

#ifndef TUVisualControllerFormUnitH
#define TUVisualControllerFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "../../Deploy/Include/myrdk.h"

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
static RDK::UELockVar<bool> CalculationModeFlag;

bool CalculationStepUpdatedFlag;

/// ����, ����������� �������� �� ������������� ������ ����� ����������� ����������
bool CheckModelFlag;

/// �����, ����������� �� ���������� ����������
unsigned long long UpdateTime;


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

// ���� force_update == true, �� ��������� �����������
// ��� ����������� �� UpdateInterval
virtual void UpdateInterface(bool force_update=false);
virtual void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void ClearInterface(void);
virtual void AClearInterface(void);

// ���������� ���������� ��� ����������
virtual std::string GetName(void);

// ���������� ��� ������ ����������
virtual std::string GetClassName(void);

// ���������� �������� ���������� ����������
virtual long GetUpdateInterval(void);

// ���������� ���� ���������� ���������� ���������� ���� ���� �� �� �����
virtual bool GetAlwaysUpdateFlag(void);

// ��������� ��������� ���������� � xml
virtual void SaveParameters(RDK::USerStorageXML &xml);
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void LoadParameters(RDK::USerStorageXML &xml);
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// ���������� ������� ������ ������������ ����������
// ������� ��� ������������ ���������� ������ (�����������)
const std::string& GetComponentControlName(void) const;
virtual bool SetComponentControlName(const std::string& name);

// �������� ����� ����� ����������
virtual TUVisualControllerForm* New(TComponent *owner=0)=0;

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

// --------------------------
// ��������������� ������� ������������
// --------------------------
// ��������� ������ ��������� ����� � xml
virtual void SaveFormPosition(RDK::USerStorageXML &xml);

// ��������� ������ ��������� ����� �� xml
virtual void LoadFormPosition(RDK::USerStorageXML &xml);
// --------------------------
};

//---------------------------------------------------------------------------
extern PACKAGE TUVisualControllerForm *UVisualControllerForm;
//---------------------------------------------------------------------------
#endif
