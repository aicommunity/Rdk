//---------------------------------------------------------------------------

#ifndef UImagesFrameUnitH
#define UImagesFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include <IniFiles.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <vector>
#include "UComponentsListFormUnit.h"
#include "myrdk.h"
#include "TUVisualController.h"

#pragma warn -8130
class TUComponentsListForm;
//---------------------------------------------------------------------------
class TUImagesFrame : public TUVisualControllerFrame
{
__published:    // IDE-managed Components
    TDrawGrid *DrawGrid;
	TPopupMenu *PopupMenu;
	TMenuItem *SaveToBmp;
	TMenuItem *SaveToJpeg;
	TMenuItem *SaveAllToBmp;
	TMenuItem *SaveAllToJpeg;
	TMenuItem *N1;
	TMenuItem *SelectSource;
	TMenuItem *N2;
	TSavePictureDialog *SavePictureDialog;
	TMenuItem *N3;
	TMenuItem *AddColumn;
	TMenuItem *AddRow;
	TMenuItem *DeleteColumn;
	TMenuItem *DeleteRow;
    void __fastcall DrawGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall SaveToBmpClick(TObject *Sender);
	void __fastcall SelectSourceClick(TObject *Sender);
	void __fastcall AddColumnClick(TObject *Sender);
	void __fastcall AddRowClick(TObject *Sender);
	void __fastcall DeleteColumnClick(TObject *Sender);
	void __fastcall DeleteRowClick(TObject *Sender);

private:    // User declarations
public:        // User declarations
	__fastcall TUImagesFrame(TComponent* Owner);
	virtual __fastcall ~TUImagesFrame(void);

// ������ �����������
std::vector<std::vector<TImage*> > Images;

// ������ ������� id ���������, ��������� � �������������
std::vector<std::vector<std::string> > StringIds;

// ������ �������� ������� ���������, ��������� � �������������
std::vector<std::vector<int> > ComponentIndexes;

// ���� ��������� ������ ��� X ����������� ��� ������
bool ReflectionXFlag;

// ��������� �� ����� ������ ����������-���������
TUComponentsListForm *MyComponentsListForm;

// --------------------------
// ������ ���������� �����������
// --------------------------
// ������������� ������ ������
void SetCellRes(int width, int height);

// ������������� ����� �����
void SetNumCells(int width, int height);

// ���������� ����� �����
int GetNumCellWidth(void);
int GetNumCellHeight(void);

// ���� ��������� ������ ��� X ����������� ��� ������
bool GetReflectionXFlag(void);
void SetReflectionXFlag(bool value);
// --------------------------

// --------------------------
// ������ ���������� �������������
// --------------------------
// ��������� ������ � ��������������� ����������
void LinkToComponent(int i, int j, const std::string &stringid, int index);

// ������������� �������� ����������� � ������ � ������������ i,j
bool SetBitmap(int i, int j, const RDK::UBitmap &bitmap);
bool SetImage(int i, int j, const TImage *image);
bool SetImage(int i, int j, int width, int height, int colormodel, unsigned char *buffer);

// ������������� �������� ����������� � ������� ������
//bool SetBitmap(RDK::UBitmap &bitmap);
bool SetImage(const TImage *image);
//bool SetImage(int width, int height, int colormodel, const unsigned char *buffer);

// ���������� ���������� ������� ������
int GetX(void);
int GetY(void);

// ���������� ����������� � ��������� ������
Graphics::TBitmap* GetImage(int i, int j);

// ���������� ����������� � ������� ������
Graphics::TBitmap* GetImage(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
void ABeforeCalculate(void);
void AAfterCalculate(void);

void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::Serialize::USerStorageXML &xml);
// --------------------------

};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUImagesFrame *UImagesFrame;
//---------------------------------------------------------------------------
#endif
