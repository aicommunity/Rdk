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
#include "myrdk.h"

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
	TCheckBox *ProjectAutoSaveFlagCheckBox;
	TOpenTextFileDialog *OpenTextFileDialog;
	TRadioGroup *CalculationSourceTimeModeRadioGroup;
	TCheckBox *ProjectAutoSaveStatesFlagCheckBox;
	TRadioGroup *ProjectModeRadioGroup;
	TLabeledEdit *ChannelsNumberLabeledEdit;
	TUpDown *UpDown1;
	TGroupBox *GroupBox4;
	TStringGrid *ChannelsStringGrid;
	TPanel *Panel2;
	TPanel *Panel3;
	TCheckBox *ShowChannelsStateCheckBox;
	TGroupBox *ProjectTimeStepGroupBox;
	TLabel *Label2;
	TEdit *ProjectTimeStepEdit;
	TEdit *GlobalTimeStepEdit;
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
	TRichEdit *RichEdit1;
	TRichEdit *ModelInfoRichEdit;
	TCheckBox *DisableStopVideoSourcesCheckBox;
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
	void __fastcall ChannelsNumberLabeledEditChange(TObject *Sender);
	void __fastcall ModelPageControlChange(TObject *Sender);
	void __fastcall ChannelsStringGridClick(TObject *Sender);
	void __fastcall PredefinedModelComboBoxChange(TObject *Sender);
	void __fastcall UClassesListFrame1StringGridClick(TObject *Sender);
	void __fastcall UClassesListFrame1LibComponentListStringGridClick(TObject *Sender);
	void __fastcall ProjectTimeStepEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall GlobalTimeStepEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall CalculationModeRadioGroupClick(TObject *Sender);
	void __fastcall MinInterstepsIntervalEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall InitAfterLoadCheckBoxClick(TObject *Sender);
	void __fastcall ResetAfterLoadCheckBoxClick(TObject *Sender);
	void __fastcall DebugModeCheckBoxClick(TObject *Sender);
	void __fastcall ProjectDirectoryLabeledEditChange(TObject *Sender);
	void __fastcall ProjectNameLabeledEditChange(TObject *Sender);
	void __fastcall ProjectDescriptionRichEditChange(TObject *Sender);
	void __fastcall ProjectAutoSaveFlagCheckBoxClick(TObject *Sender);
	void __fastcall ProjectAutoSaveStatesFlagCheckBoxClick(TObject *Sender);
	void __fastcall EventsLogFlagCheckBoxClick(TObject *Sender);
	void __fastcall ProjectModeRadioGroupClick(TObject *Sender);
	void __fastcall MultiThreadingModeRadioGroupClick(TObject *Sender);
	void __fastcall CalculationSourceTimeModeRadioGroupClick(TObject *Sender);
	void __fastcall DisableStopVideoSourcesCheckBoxClick(TObject *Sender);




private:	// User declarations
public:		// User declarations
	__fastcall TUCreateProjectWizardForm(TComponent* Owner);

bool UpdateInterfaceFlag;

/// Режим визарда по умолчанию
/// 0 - общий
/// 1 - Видеоаналитика
/// 2 - Управление поворотками
/// 3 - Нейронные сети
int WizardMode;

// Число шагов визарда
int NumSteps;

// Текущий шаг визарда
int CurrentStep;

// Описание шагов визарда
std::vector<String> StepDescriptions;

// Список вариантов предустановленных моделей
std::map<std::string, int> PredefinedModels;

public: // Выходные данные

/// Конфигурация сервера
RDK::TProjectConfig ProjectConfig;

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
int ShowCreateProject(int wizard_mode);

// Отображает визард для модификации проекта
int ShowProjectOptions(void);

/// Переключает параметры проекта в состояние wizard_mode
void ApplyWizardMode(int wizard_mode);
// ----------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TUCreateProjectWizardForm *UCreateProjectWizardForm;
//---------------------------------------------------------------------------
#endif
