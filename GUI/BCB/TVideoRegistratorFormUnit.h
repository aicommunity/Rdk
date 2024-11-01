#ifndef RDK_VIDEO
#define RDK_VIDEO
#endif
//---------------------------------------------------------------------------

#ifndef VideoRegistratorFormUnitH
#define VideoRegistratorFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "TVideoRegistratorFrameUnit.h"
#include <Menus.hpp>
#include <IniFiles.hpp>
#include <Vcl.ComCtrls.hpp>
#include "TUVisualController.h"
#include "TUVisualControllerFormUnit.h"
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
//---------------------------------------------------------------------------
class TVideoRegistratorForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TMainMenu *MainMenu;
	TToolBar *ToolBar1;
	TImageList *ImageList;
	TMenuItem *Sources1;
	TMenuItem *AddSource1;
	TMenuItem *DelSource1;
	TMenuItem *ClearSources1;
	TPopupMenu *PopupMenu;
	TMenuItem *AddSource2;
	TMenuItem *DelSource2;
	TMenuItem *ClearSource1;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	void __fastcall PageControlChange(TObject *Sender);
	void __fastcall AddSource1Click(TObject *Sender);
	void __fastcall DelSource1Click(TObject *Sender);
	void __fastcall ClearSources1Click(TObject *Sender);
	void __fastcall AddSource2Click(TObject *Sender);
	void __fastcall DelSource2Click(TObject *Sender);
	void __fastcall ClearSource1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoRegistratorForm(TComponent* Owner);
	virtual __fastcall ~TVideoRegistratorForm(void);

// ��������� �����
std::vector<TTVideoRegistratorFrame*> Sources;

// �����, ���������� ����� ����� �������
void ABeforeCalculate(void);

// ��������� ���������
void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::USerStorageXML &xml);

// �������� ����� ����� ����������
virtual TVideoRegistratorForm* New(TComponent *owner=0);

// ����� ���������� �����
int GetNumSources(void) const;

// ��������� ����� �������� �����
void AddSource(void);

// ������� �������� �����
void DelSource(int index);

// ������� ��� ��������� �����
void ClearSources(void);

// �������� ����� ��������� ���������
void SetSourceType(int index, int mode);

// ���������� ������ �������� ��������� (����������) ��������� �����
int GetActiveSource(void) const;

// ���������� ����� ��������� �����
TTVideoRegistratorFrame* GetVideoOutputFrame(int index);

// ���������� ����� ��������� (����������) ��������� �����
TTVideoRegistratorFrame* GetActiveVideoOutputFrame(void);

// ��������� ���������� �� ���������� ������ � �������� ini ����
void SaveToIni(TMemIniFile *ini, const String &section);

// ��������� ���������� �� ���������� ������ �� ��������� ini �����
void LoadFromIni(TMemIniFile *ini, const String &section);

// ��������� ��������� �������� �����, ��� ��� ���� index == -1
void Start(int index=-1);

// ������������� ��������� �������� �����, ��� ��� ���� index == -1
void Stop(int index=-1);

// ������������� ��������� �������� �����, ��� ��� ���� index == -1
// �� ��������� ��������� ������-���������� (������)
void StopOffline(int index=-1);

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoRegistratorForm *VideoRegistratorForm;
//---------------------------------------------------------------------------
#endif
