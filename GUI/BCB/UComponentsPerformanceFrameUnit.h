//---------------------------------------------------------------------------

#ifndef UComponentsPerformanceFrameUnitH
#define UComponentsPerformanceFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Series.hpp>
#include <Vcl.Menus.hpp>
#include <IniFiles.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.Samples.Spin.hpp>
#include <vector>
#include <string>
#include "UComponentsListFormUnit.h"
#include "myrdk.h"
#include "TUVisualController.h"
//---------------------------------------------------------------------------
#pragma warn -8130
class TUComponentsPerformanceFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TChart *Chart;
	TBarSeries *Series1;
	TBarSeries *Series2;
	TPopupMenu *PopupMenu;
	TMenuItem *SelectSource1;
	TMenuItem *N1;
	TMenuItem *ClearAll1;
	TPanel *Panel1;
	TRadioGroup *ShowModeRadioGroup;
	TGroupBox *GroupBox1;
	TSpinEdit *AverageIntervalSpinEdit;
	void __fastcall SelectSource1Click(TObject *Sender);
	void __fastcall ClearAll1Click(TObject *Sender);
	void __fastcall AverageIntervalSpinEditChange(TObject *Sender);
	void __fastcall ShowModeRadioGroupClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentsPerformanceFrame(TComponent* Owner);
	__fastcall ~TUComponentsPerformanceFrame(void);

// ������ ������� ���� ����������� ���������
std::vector<std::string> ComponentNames;

// ���������� ���������
std::vector<std::vector<long long> > ComponentData;

// ��������� �� ����� ������ ����������-���������
TUComponentsListForm *MyComponentsListForm;

// ����� �������� ����������
int AverageIterations;

// --------------------------
// ������ ���������� �������
// --------------------------
void ABeforeCalculate(void);
void AAfterCalculate(void);

// ��������� ���������
void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

// ���������� ����� ����������� ���������
std::size_t GetNumComponents(void) const;

// ��������� ��������� ��� �����������
void AddComponent(const std::string &componentname);

// ��������� ��� ����������, ������������ ��������������� � ��������� ����������
void AddAllComponents(const std::string &componentname);

// ������� ����������� ���������
void DelComponent(const std::string &componentname);
void DelComponent(std::size_t index);

// ������� ��� ����������� ����������
void ClearComponents(void);
};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsPerformanceFrame *UComponentsPerformanceFrame;
//---------------------------------------------------------------------------
#endif
