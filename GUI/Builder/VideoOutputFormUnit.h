//---------------------------------------------------------------------------

#ifndef VideoOutputFormUnitH
#define VideoOutputFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "VideoOutputFrameUnit.h"
#include <Menus.hpp>
#include <IniFiles.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TVideoOutputForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	void __fastcall PageControlChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoOutputForm(TComponent* Owner);

// ��������� �����
std::vector<TVideoOutputFrame*> Sources;

bool UpdateInterfaceFlag;

// ��������� ���������
void UpdateInterface(void);

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

// ���������� ����� ��������� �����
TVideoOutputFrame* GetVideoOutputFrame(int index);

// ��������� ���������� �� ���������� ������ � �������� ini ����
void SaveToIni(TMemIniFile *ini, const String &section);

// ��������� ���������� �� ���������� ������ �� ��������� ini �����
void LoadFromIni(TMemIniFile *ini, const String &section);
};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputForm *VideoOutputForm;
//---------------------------------------------------------------------------
#endif
