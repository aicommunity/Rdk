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
#include <Vcl.ExtCtrls.hpp>
#include <vector>
#include "UComponentsListFormUnit.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
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
	TImage *FullImage;
	TScrollBox *ScrollBox1;
	TPanel *Panel1;
	TCheckBox *ShowLegendCheckBox;
	TCheckBox *ShowHistogramCheckBox;
	TRadioButton *TiledSizeRadioButton;
	TRadioButton *OriginalSizeRadioButton;
	TCheckBox *ShowInfoCheckBox;
	TCheckBox *IndChannelsCheckBox;
	TRadioButton *ProportionalSizeRadioButton;
	TCheckBox *CenterImageCheckBox;
	TMenuItem *N4;
	TMenuItem *Update1;
	TLabel *Labelshow;
	TLabel *Labelsize;
	TPopupMenu *PopupMenu1;
	TMenuItem *SetPointsMode2;
	TMenuItem *DeleteLastPoint2;
	TMenuItem *DeleteAllPoints2;
	TMenuItem *ShowPoints2;
    void __fastcall DrawGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall SaveToBmpClick(TObject *Sender);
	void __fastcall SelectSourceClick(TObject *Sender);
	void __fastcall AddColumnClick(TObject *Sender);
	void __fastcall AddRowClick(TObject *Sender);
	void __fastcall DeleteColumnClick(TObject *Sender);
	void __fastcall DeleteRowClick(TObject *Sender);
	void __fastcall DrawGridDblClick(TObject *Sender);
	void __fastcall FullImageDblClick(TObject *Sender);
	void __fastcall FullImageMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall FullImageMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall FullImageMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall DrawGridClick(TObject *Sender);
	void __fastcall OriginalSizeRadioButtonClick(TObject *Sender);
	void __fastcall TiledSizeRadioButtonClick(TObject *Sender);
	void __fastcall ShowLegendCheckBoxClick(TObject *Sender);
	void __fastcall ShowHistogramCheckBoxClick(TObject *Sender);
	void __fastcall ShowInfoCheckBoxClick(TObject *Sender);
	void __fastcall ProportionalSizeRadioButtonClick(TObject *Sender);
	void __fastcall SaveToJpegClick(TObject *Sender);
	void __fastcall Update1Click(TObject *Sender);
	void __fastcall SetPointsMode2Click(TObject *Sender);
	void __fastcall DeleteLastPoint2Click(TObject *Sender);
	void __fastcall DeleteAllPoints2Click(TObject *Sender);
	void __fastcall ShowPoints2Click(TObject *Sender);




private:    // User declarations
public:        // User declarations
	__fastcall TUImagesFrame(TComponent* Owner);
	virtual __fastcall ~TUImagesFrame(void);

bool presIm;
int x2, y2;

RDK::UBitmap TempBmp;

// ������ �����������
std::vector<std::vector<TImage*> > Images;

// ������ ������� id ���������, ��������� � �������������
std::vector<std::vector<std::string> > StringIds;

// ������ �������� ������� ���������, ��������� � �������������
std::vector<std::vector<std::string> > ComponentIndexes;

// ������ �������� ������� ���������, ��������� � �������������
std::vector<std::vector<int> > ComponentIndexesOld;

// ������ �������� �������, ������ ����������� ����������� ��������
std::vector<std::vector<int> > ComponentChannelIndexes;

// ������ ���� ��������� � �� �������, ��������� � ������ ����
std::vector<std::vector<std::pair<std::string,std::string> > > MouseClickComponents;

// ������ ������
std::vector<std::vector<std::string> > Legends;

// ������ �����, ������������ �� ������
std::vector<std::vector<std::vector<RDK::UBPoint> > > OnScreenPoints;
std::vector<std::vector<std::vector<RDK::UColorT> > > OnScreenPointsColors;

// ������ �����, ������������ �� ������
std::vector<std::vector<std::vector<RDK::UBPoint> > > ExternalPoints;
std::vector<std::vector<std::vector<RDK::UColorT> > > ExternalPointsColors;

//��������� ����� ����������
bool CyclicPointUpdate;

//������������ ����� ����� ��� ����������� ����������
int MaxPointCount;

//��������� ����������� �����
std::vector<std::vector<int> > LastUpdatedPoint;

/// ���� ���� ��� ��������� ��������
TColor SingleBackgroundColor;

// ���� ��������� ������ ��� X ����������� ��� ������
bool ReflectionXFlag;

// ���� ������������� ����������� ������ �����������
bool IsShowCapturedFrames;

// ����� ������� ����� �� �����������
bool IsSetPointsMode;

// ������������ ����������� ����� �� �����������
bool IsShowPoints;

// ������� ����� �� �����������
int PointSize;

// ���� ����� �� �����������
RDK::UColorT PointColor;

// ��������� �� ����� ������ ����������-���������
TUComponentsListForm *MyComponentsListForm;

/// ����� ���������������
/// 0 - ������������ ������
/// 1 - ��������������� ����� ��� ����� ������� � �����
/// 2 - ��������������� � ����������� ��������� ������� �����������
int SizeMode;

/// ����� �����������
/// 0 - ����� �� �����������
/// 1 - ���� ���������� �����������
int ShowMode;

int NumCols,NumRows;

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

// ���� ������������� ����������� ������ �����������
void SetIsShowCapturedFrames(bool value);
bool GetIsShowCapturedFrames(void);

// ������� ����� �� �����������
int SetPointSize(int value);
int GetPointSize(void);

// ���� ����� �� �����������
void SetPointColor(RDK::UColorT value);
RDK::UColorT GetPointColor(void);

void SetCyclicPointUpdate(bool value) {CyclicPointUpdate=value;};
bool GetCyclicPointUpdate() {return CyclicPointUpdate;};

void SetMaxPointCount(int value) {MaxPointCount=value;};
int GetMaxPointCount(){return MaxPointCount;};

// �������� �� ����������� �����
std::vector<RDK::UBPoint> &GetOnScreenPoints(int col, int row);
// ����� ����� �������� �� �����������
std::vector<RDK::UColorT> &GetOnScreenPointsColors(int col, int row);

// �������� �� ����������� �����
std::vector<RDK::UBPoint> &GetExternalPoints(int col, int row);
// ����� ����� �������� �� �����������
std::vector<RDK::UColorT> &GetExternalPointsColors(int col, int row);

int GetLastUpdatedPoint(int col, int row);

bool AddExternalPoint(int col, int row, int X, int Y, const RDK::UColorT& color);
// --------------------------

// --------------------------
// ������ ���������� �������������
// --------------------------
// ��������� ������ � ��������������� ����������
void LinkToComponent(int i, int j, const std::string &stringid, std::string index, int channel_id);

// ������������� �������� ����������� � ������ � ������������ i,j
bool SetBitmap(int i, int j, const RDK::UBitmap &bitmap);
bool SetImage(int i, int j, const TImage *image);
bool SetImage(int i, int j, int width, int height, int colormodel, unsigned char *buffer);

// ������������� �������� ����������� � ������� ������
//bool SetBitmap(RDK::UBitmap &bitmap);
bool SetImage(const TImage *image);
//bool SetImage(int width, int height, int colormodel, const unsigned char *buffer);
void SetSource(int channel_index, const std::string &component, const std::string &output, int col, int row);

// ���������� ���������� ������� ������
int GetX(void);
int GetY(void);

// ���������� ����������� � ��������� ������
Graphics::TBitmap* GetImage(int i, int j);

// ���������� ����������� � ������� ������
Graphics::TBitmap* GetImage(void);

// ��������� ����� �� �����������
void DrawPoints(TBitmap *bitmap, std::vector<RDK::UBPoint> &points, std::vector<RDK::UColorT> &colors);

// ��������� ����������� � ����
bool SaveToBitmap(int i, int j);
bool SaveToJpg(int i, int j);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
void ABeforeCalculate(void);
void AAfterCalculate(void);

void AUpdateInterface(void);

// ������ ���������� �����
void ManualUpdate(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUImagesFrame *UImagesFrame;
//---------------------------------------------------------------------------
#endif
