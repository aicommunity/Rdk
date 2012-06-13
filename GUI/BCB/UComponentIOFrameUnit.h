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

//#include "../../Kernel/NBios.h"

//using namespace NMSDK;
//---------------------------------------------------------------------------
class TUComponentIOFrame : public TFrame
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

// ���� ���������� ����������
bool UpdateInterfaceFlag;
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

// ������������ ������� ��������� ������
void __fastcall UpdateInterface(void);
// -----------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentIOFrame *UComponentIOFrame;
//---------------------------------------------------------------------------
#endif
