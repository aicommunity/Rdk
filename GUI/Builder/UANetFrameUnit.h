//---------------------------------------------------------------------------

#ifndef UANetFrameUnitH
#define UANetFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>

#include "../../Kernel/NBios.h"

using namespace NMSDK;
//---------------------------------------------------------------------------
class TNANetFrame : public TFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	void __fastcall FrameResize(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TNANetFrame(TComponent* Owner);

// -----------------
// ������
// -----------------
// ��������� �� ��������������� ����
NANet* Net;

// ����� ������:
// 1 - ���������� �����
// 2 - ���������� ������
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

// ��������� ������� �������������� �������
void __fastcall ShowLinks(void);

// ������������ ������� ��������� ������
void __fastcall UpdateInterface(void);
// -----------------
};
//---------------------------------------------------------------------------
extern PACKAGE TNANetFrame *NANetFrame;
//---------------------------------------------------------------------------
#endif
