//---------------------------------------------------------------------------

#ifndef UFavoriteComponentInfoFrameUnitH
#define UFavoriteComponentInfoFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.Grids.hpp>
#include <Vcl.Menus.hpp>

struct TUFavoriteData
{
public: // ������
// ��� ������
// 0 - �� ���������
// 1 - �������� ���������
// 2 - �������� ��������� ���������
// 3 - �������� ����� ����������
// 4 - �������� ������ ����������
int Type;

// ����� ������ ��������
// 0 - ��������� ��������
// 1 - ������� �� �������� Time
// 2 - ������������ �� �������� Time
// 3 - ����������� �� �������� Time
// 4 - ������������ �� ������ �� �������� Time
// 5 - ����������� �� ������ �� �������� Time
// 6 - ��������� �� �������� Time
int Mode;

// �������� ������� � �������� ��� �������
double Time;

// ������� ��� ����������
std::string ComponentName;

// ��� ���������/��������� � �.�.
std::string DataName;

// ����������� ��������
std::string Data;

// �������� ���
std::string Header;

protected: // ��������� ����������
// ������ �������� �� �������� �������� �������
std::vector<std::pair<double,double> > History;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
TUFavoriteData(void);
TUFavoriteData(const TUFavoriteData &copy);
~TUFavoriteData(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ��������� �������� � ������������ � ����� � ������� � ��������� ��� � ���������� Data
void ReadData(void);
// --------------------------

};

//---------------------------------------------------------------------------
class TUFavoriteComponentInfoFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	TPopupMenu *PopupMenu;
	TMenuItem *Add1;
	TMenuItem *Delete1;
	TMenuItem *Moveup1;
	TMenuItem *MoveDown1;
	TMenuItem *Rename1;
	void __fastcall Add1Click(TObject *Sender);
	void __fastcall Delete1Click(TObject *Sender);
	void __fastcall MoveDown1Click(TObject *Sender);
	void __fastcall Moveup1Click(TObject *Sender);
	void __fastcall Rename1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUFavoriteComponentInfoFrame(TComponent* Owner);

// ������ ������
std::vector<TUFavoriteData> Info;

// ��������� ���������
void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::Serialize::USerStorageXML &xml);

};
//---------------------------------------------------------------------------
extern PACKAGE TUFavoriteComponentInfoFrame *UFavoriteComponentInfoFrame;
//---------------------------------------------------------------------------
#endif
