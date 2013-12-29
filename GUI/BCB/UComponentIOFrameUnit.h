//---------------------------------------------------------------------------

#ifndef UComponentIOFrameUnitH
#define UComponentIOFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include "TUVisualControllerFrameUnit.h"
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

// ���� �� �������� ������������ ����������
std::string ViewComponentOwnerLongId;

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

// ���������� ������ �������-������/������� � map
void DecodePropertiesIOList(const std::string &source, std::map<int, std::string> &result);
// -----------------

// -----------------
// ��������������� ������ ����������
// -----------------
protected:
// ���������� ������ ������� � �������
void __fastcall ShowOutputs(TStringGrid *string_grid, RDK::ULongIdVector &linkslist);

// ���������� ������ ������ � �������
void __fastcall ShowInputs(TStringGrid *string_grid, RDK::ULongIdVector &linkslist);
// -----------------


// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
public:
// �����, ���������� ����� ����� �������
void ABeforeCalculate(void);

// �����, ���������� ����� ���� �������
void AAfterCalculate(void);

// ���������� ����������
void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentIOFrame *UComponentIOFrame;
//---------------------------------------------------------------------------
#endif
