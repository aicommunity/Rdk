//---------------------------------------------------------------------------

#ifndef UWatchFramePageUnitH
#define UWatchFramePageUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UWatchFrameUnit.h"
#include <Vcl.Menus.hpp>
//---------------------------------------------------------------------------
class TUWatchFramePage : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TPageControl *NTWPageControl;
	TPopupMenu *NTWPageControlPopupMenu;
	TMenuItem *AddPage1;
	TMenuItem *DeletePage1;
	TMenuItem *RanamePage1;
private:	// User declarations
public:		// User declarations

// ------------------------------
// ������ ���������� ����������
// ------------------------------
void AUpdateInterface(void);
void ABeforeCalculate(void);
void AAfterCalculate(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// �������� ����� ����� ����������
virtual TUWatchFramePage* New(TComponent *owner=0);
// ------------------------------

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
// ------------------------------


	__fastcall TUWatchFramePage(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUWatchFramePage *UWatchFramePage;
//---------------------------------------------------------------------------
#endif
