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
#include <Vcl.Grids.hpp>
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
	TRadioGroup *ProjectCalculationModeRadioGroup;
	TCheckBox *ProjectAutoSaveFlagCheckBox;
	TOpenTextFileDialog *OpenTextFileDialog;
	TGroupBox *VideoAnalysisGroupBox;
	TLabeledEdit *ImageWidthLabeledEdit;
	TLabeledEdit *ImageHeightLabeledEdit;
	TLabeledEdit *NumInputsLabeledEdit;
	TLabeledEdit *NumOutputsLabeledEdit;
	TCheckBox *UpendInputImageCheckBox;
	TRadioGroup *CalculationSourceTimeModeRadioGroup;
	TCheckBox *ProjectAutoSaveStatesFlagCheckBox;
	TRadioGroup *ProjectModeRadioGroup;
	TLabeledEdit *ChannelsNumberLabeledEdit;
	TUpDown *UpDown1;
	TGroupBox *GroupBox4;
	TStringGrid *StringGrid1;
	TPanel *Panel2;
	TPanel *Panel3;
	TCheckBox *ShowChannelsStateCheckBox;
	TGroupBox *ProjectTimeStepGroupBox;
	TLabel *Label2;
	TEdit *ProjectTimeStepEdit;
	TEdit *Edit1;
	TLabel *Label3;
	TCheckBox *EventsLogFlagCheckBox;
	TRadioGroup *CalculationModeRadioGroup;
	TCheckBox *InitAfterLoadCheckBox;
	TCheckBox *ResetAfterLoadCheckBox;
	TCheckBox *DebugModeCheckBox;
	TRadioGroup *MultiThreadingModeRadioGroup;
	TGroupBox *GroupBox5;
	TEdit *MinInterstepsIntervalEdit;
	TPageControl *ModelPageControl;
	TTabSheet *PredefinedModelTabSheet;
	TTabSheet *ModelFromFileTabSheet;
	TTabSheet *ModelFromComponentTabSheet;
	TComboBox *PredefinedModelComboBox;
	TLabeledEdit *ProjectModelFileNameLabeledEdit;
	TButton *OpenModelButton;
	TGroupBox *GroupBox2;
	TUClassesListFrame *UClassesListFrame1;
	TPanel *Panel4;
	TTabSheet *DontChangeTabSheet;
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
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUCreateProjectWizardForm(TComponent* Owner);

bool UpdateInterfaceFlag;

// Число шагов визарда
int NumSteps;

// Текущий шаг визарда
int CurrentStep;

// Описание шагов визарда
std::vector<String> StepDescriptions;

// Список вариантов предустановленных моделей
std::map<std::string, int> PredefinedModels;

public: // Выходные данные
int PredefinedStructure;

// ----------------------
// Методы
// ----------------------
// Обновление интерфейса
void UpdateInterface(void);

// Очистка визарда
void ClearWizard(void);

// Очистить список вариантов предустановленных моделей
void ClearPredefinedModels(void);

// Добавить вариант в список предустановленных моделей
void AddPredefinedModel(const std::string &name, int index);

// Отображает визард для создания проекта
int ShowCreateProject(void);

// Отображает визард для модификации проекта
int ShowProjectOptions(void);
// ----------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TUCreateProjectWizardForm *UCreateProjectWizardForm;
//---------------------------------------------------------------------------
#endif
