//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TApplicationOptionsFrameUnit.h"
#include "UGEngineControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TApplicationOptionsFrame *ApplicationOptionsFrame;

/// �������� ������ ����������
extern RDK::UApplication RdkApplication;

//---------------------------------------------------------------------------
__fastcall TApplicationOptionsFrame::TApplicationOptionsFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 UpdateInterval=-1;
}

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
void TApplicationOptionsFrame::AAfterReset(void)
{

}

// ���������� ����������
void TApplicationOptionsFrame::AUpdateInterface(void)
{
 if(!UGEngineControlForm)
  return;
 AutoStartProjectCheckBox->Checked=UGEngineControlForm->AutoStartProjectFlag;
 ApplicationNameLabeledEdit->Text=UGEngineControlForm->ProgramName;
 MainFormInternalNameLabeledEdit->Text=UGEngineControlForm->MainFormName;
 AutoStartProjectNameLabeledEdit->Text=UGEngineControlForm->AutoexecProjectFileName;
 HideAdminFormCheckBox->Checked=UGEngineControlForm->HideAdminFormFlag;
 DisableAdminFormCheckBox->Checked=UGEngineControlForm->DisableAdminForm;
 MinimizeToTrayCheckBox->Checked=UGEngineControlForm->MinimizeToTray;
 StartMinimizedCheckBox->Checked=UGEngineControlForm->StartMinimized;
 ProjectHistorySpinEdit->Value=RdkApplication.GetLastProjectsListMaxSize();
}

// ������� ���������� � �������� ���������
void TApplicationOptionsFrame::AClearInterface(void)
{

}

// ��������� ��������� ���������� � xml
void TApplicationOptionsFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
void TApplicationOptionsFrame::ALoadParameters(RDK::USerStorageXML &xml)
{

}

void TApplicationOptionsFrame::ApplyOptions(void)
{
 if(!UGEngineControlForm)
  return;
 UGEngineControlForm->AutoStartProjectFlag=AutoStartProjectCheckBox->Checked;
 UGEngineControlForm->ProgramName=ApplicationNameLabeledEdit->Text;
 UGEngineControlForm->MainFormName=MainFormInternalNameLabeledEdit->Text;
 UGEngineControlForm->AutoexecProjectFileName=AutoStartProjectNameLabeledEdit->Text;
 UGEngineControlForm->HideAdminFormFlag=HideAdminFormCheckBox->Checked;
 UGEngineControlForm->DisableAdminForm=DisableAdminFormCheckBox->Checked;
 UGEngineControlForm->MinimizeToTray=MinimizeToTrayCheckBox->Checked;
 UGEngineControlForm->StartMinimized=StartMinimizedCheckBox->Checked;
 RdkApplication.SetLastProjectsListMaxSize(ProjectHistorySpinEdit->Value);
}
// -----------------------------
//---------------------------------------------------------------------------
