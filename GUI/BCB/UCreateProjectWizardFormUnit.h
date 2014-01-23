//---------------------------------------------------------------------------

#ifndef UCreateProjectWizardFormUnitH
#define UCreateProjectWizardFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UClassesListFrameUnit.h"
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
//---------------------------------------------------------------------------
class TUCreateProjectWizardForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPageControl *PageControl;
	TButton *PrevButton;
	TButton *NextButton;
	TButton *FinishButton;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TPanel *TitlePanel2;
	TPanel *TitlePanel1;
	TPanel *TitlePanel3;
	TLabeledEdit *ProjectNameLabeledEdit;
	TLabeledEdit *ProjectDirectoryLabeledEdit;
	TButton *Button1;
	TLabel *Label1;
	TRichEdit *ProjectDescriptionRichEdit;
	TRadioGroup *ProjectTypeRadioGroup;
	TGroupBox *ProjectTimeStepGroupBox;
	TRadioGroup *ProjectCalculationModeRadioGroup;
	TLabel *Label2;
	TEdit *ProjectTimeStepEdit;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TUClassesListFrame *UClassesListFrame1;
	TRadioButton *RootModelComponentNameRadioButton;
	TRadioButton *PredefinedModelRadioButton;
	TCheckBox *ProjectAutoSaveFlagCheckBox;
	TComboBox *PredefinedModelComboBox;
	TGroupBox *GroupBox1;
	TRadioButton *ModelFileNameRadioButton;
	TLabeledEdit *ProjectModelFileNameLabeledEdit;
	TButton *OpenModelButton;
	TOpenTextFileDialog *OpenTextFileDialog;
	TGroupBox *VideoAnalysisGroupBox;
	TLabeledEdit *ImageWidthLabeledEdit;
	TLabeledEdit *ImageHeightLabeledEdit;
	TLabeledEdit *NumInputsLabeledEdit;
	TLabeledEdit *NumOutputsLabeledEdit;
	TCheckBox *UpendInputImageCheckBox;
	TRadioGroup *CalculationSourceTimeModeRadioGroup;
	void __fastcall FinishButtonClick(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall ProjectTypeRadioGroupClick(TObject *Sender);
	void __fastcall PredefinedModelRadioButtonClick(TObject *Sender);
	void __fastcall RootModelComponentNameRadioButtonClick(TObject *Sender);
	void __fastcall PageControlChange(TObject *Sender);
	void __fastcall PrevButtonClick(TObject *Sender);
	void __fastcall NextButtonClick(TObject *Sender);
	void __fastcall OpenModelButtonClick(TObject *Sender);
	void __fastcall ModelFileNameRadioButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUCreateProjectWizardForm(TComponent* Owner);

bool UpdateInterfaceFlag;

// ����� ����� �������
int NumSteps;

// ������� ��� �������
int CurrentStep;

// �������� ����� �������
std::vector<String> StepDescriptions;

// ������ ��������� ����������������� �������
std::map<std::string, int> PredefinedModels;

public: // �������� ������
int PredefinedStructure;

// ----------------------
// ������
// ----------------------
// ���������� ����������
void UpdateInterface(void);

// ������� �������
void ClearWizard(void);

// �������� ������ ��������� ����������������� �������
void ClearPredefinedModels(void);

// �������� ������� � ������ ����������������� �������
void AddPredefinedModel(const std::string &name, int index);

// ���������� ������ ��� �������� �������
int ShowCreateProject(void);

// ���������� ������ ��� ����������� �������
int ShowProjectOptions(void);
// ----------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TUCreateProjectWizardForm *UCreateProjectWizardForm;
//---------------------------------------------------------------------------
#endif
