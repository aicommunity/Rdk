//---------------------------------------------------------------------------

#ifndef UComponentIOFrameUnitH
#define UComponentIOFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <string>
#include "TUVisualController.h"
//---------------------------------------------------------------------------
class TUComponentIOFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	void __fastcall FrameResize(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentIOFrame(TComponent* Owner);

// -----------------
// ������
// -----------------
// ��������� �� ��������������� ����
//NANet* Net;

// ���� �� ������������ ����������
std::string ViewComponentLongId;

// ����� ������:
// 1 - ���������� �����
// 2 - ���������� ������
// 3 - ���������� � ����� � ������
// 4 - ���������� ������������ �����
int Mode;

// ����������� ������ ������
// 1 - ���������� ������ ����� (������, �����) ������ ������
// 2 - ���������� ����� (������, �����) ������ ������, � ���� ��������� �����
int ShowModifier;
// -----------------

// -----------------
// ������ ����������
// -----------------
// ������� �������
void __fastcall Clear(void);

// ��������� ������� ������� ����
void __fastcall ShowInputs(void);

// ��������� ������� �������� ����
void __fastcall ShowOutputs(void);

// ��������� ������� ������� � �������� ����
void __fastcall ShowInputsOutputs(void);

// ��������� ������� �������������� �������
void __fastcall ShowLinks(void);
// -----------------

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ����� �������
void ABeforeCalculate(void);

// �����, ���������� ����� ���� �������
void AAfterCalculate(void);

// ���������� ����������
void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::Serialize::USerStorageXML &xml);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentIOFrame *UComponentIOFrame;
//---------------------------------------------------------------------------
#endif
