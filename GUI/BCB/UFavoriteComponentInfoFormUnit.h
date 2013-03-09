//---------------------------------------------------------------------------

#ifndef UFavoriteComponentInfoFormUnitH
#define UFavoriteComponentInfoFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFormUnit.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
//---------------------------------------------------------------------------
class TUFavoriteComponentInfoForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TPopupMenu *PopupMenu;
	TMenuItem *AddPage1;
	TMenuItem *DeletePage1;
	TMenuItem *RenamePage1;
	void __fastcall AddPage1Click(TObject *Sender);
	void __fastcall DeletePage1Click(TObject *Sender);
	void __fastcall RenamePage1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUFavoriteComponentInfoForm(TComponent* Owner);

// ��������� ���������
void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::USerStorageXML &xml);

// ------------------------------
// ������ ���������� ����������
// ------------------------------
// ������� ��� ������ ������� (��������� 1 ���������)
void ClearPages(void);

// ��������� ��������
void AddPage(void);

// ������� ��������
void DelPage(int index);

// ��������������� ��������� ��������
void RenamePage(int index, String new_name);
};
//---------------------------------------------------------------------------
extern PACKAGE TUFavoriteComponentInfoForm *UFavoriteComponentInfoForm;
//---------------------------------------------------------------------------
#endif
