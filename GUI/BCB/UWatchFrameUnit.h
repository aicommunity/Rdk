//---------------------------------------------------------------------------


#ifndef UWatchFrameUnitH
#define UWatchFrameUnitH
#include <Classes.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ExtDlgs.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <VCLTee.Series.hpp>

#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>

#include <IniFiles.hpp>
#include <jpeg.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <Menus.hpp>
#include "TUVisualControllerFrameUnit.h"


#include <string>
#include <map>
#include <vector>
#include "myrdk.h"
#include "TUVisualController.h"

using namespace std;

//typedef float real;
//---------------------------------------------------------------------------
/*
   ��������������� ����� - ��������� ������, ��������������� ����������� �����
�����.
*/
//---------------------------------------------------------------------------
class TUWatchInfo
{
public: // �����
// ��� �����
// 0 - ������� (����� �� �������)
// 0x100 - ����������� X(Y)
// 0x200 - MDMatrix
int Type;

// ������ ������� � ������� �����
int SeriesIndex;

// ��� ����������-��������� ������ �� ��� X
string XDataSourceName;

// ������ ������ ��������� ������ �� ��� X
string XOutputIndex;
int XOutputIndexOld;

// ������ �������� ������ ��������� ������ �� ��� X
int XOutputElementIndex;

// ��� ����������-��������� ������ �� ��� X
string YDataSourceName;

// ������ ������ ��������� ������ �� ��� X
string YOutputIndex;
int YOutputIndexOld;

// ������ �������� ������ ��������� ������ �� ��� X
int YOutputElementIndex;

// ���������� ������, ��������� ������ �� ��� Y ��� ������ MDMatrix
int MRow, MCol;

public: // ������ �������
const double *X;
const double *Y;
int XYSize;

// ��� ��������� � �������� �� �������� �������� �������
//double XCurrentMax,YCurrentMax;
//double XCurrentMin,YCurrentMin;

// ��� ����������
// true - � �������� X,Y �������� ����� ���� �������� �������
// � ��� ����� ����������� ������� �� ������ ����
// false -
// �� ������ ���� ����� ������� ����������� � �����
bool FullUpdate;

// �������� ����������, ������ ������ ������������� ���������
// ������������ ���� <0, ��� ���� FullUpdate == true
double WatchInterval;

// ���� ���������
// ���� true - ����� ������������, ����� - ���
bool Visible;

public: // ���������� �������
// �������� �������� ����� ���� ������������ ����� �� Y
double YShift;

// ���� �������
TColor Color;

// ����� ����� �������
TPenStyle Style;

// ������� �������
string Legend;

// ������� �����
int LineWidth;

public: // ������
TUWatchInfo(void);
~TUWatchInfo(void);
TUWatchInfo(const TUWatchInfo &wd);
TUWatchInfo& operator = (const TUWatchInfo& wd);
};

//---------------------------------------------------------------------------
class TUWatchFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
        TChart *Chart1;
	TSavePictureDialog *SavePictureDialog;
	TPopupMenu *PopupMenu;
	TMenuItem *N1;
	TMenuItem *bmp1;
	TMenuItem *AddWatch1;
	TMenuItem *N2;
	TMenuItem *AddXPulseWatch1;
	TMenuItem *AddXYWatch1;
	TMenuItem *DeleteAll1;
	TMenuItem *N3;
	TMenuItem *AddTimeMatrixWatch1;
	TMenuItem *AddTimeYWatchold1;
	void __fastcall N1Click(TObject *Sender);
	void __fastcall bmp1Click(TObject *Sender);
	void __fastcall AddWatch1Click(TObject *Sender);
	void __fastcall AddXPulseWatch1Click(TObject *Sender);
	void __fastcall DeleteAll1Click(TObject *Sender);
	void __fastcall AddTimeMatrixWatch1Click(TObject *Sender);
	void __fastcall AddTimeYWatchold1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TUWatchFrame(TComponent* Owner);
        virtual __fastcall ~TUWatchFrame(void);

protected:	// ���������
// ������ ���� ������������ ������
int CacheSize;

protected:	// ������
        // -----------------------------------------
        // ������ �� ����������� ������ ����� ������
        // -----------------------------------------
        // ������ ��� ����������� ����� �����, � ��������
        // �������� ����� ���� ������������ ����� �� ��� Y.
		vector<TUWatchInfo> NameList;

		// ������� ������ ���������� � ����
		int CacheIndex;

		// �������� ����������
		double WatchInterval;
		// -----------------------------------------

		// -----------------------------------------
		// ������ � ���������
		// -----------------------------------------
		// �������� 'true' ���� ������ ����� ���� ��������
		// ��� ���� ���������/������� ����� �����
		bool ModifyState;

		// -----------------------------------------

public:	// ������
		// ------------------------------
		// ������ ��������� �����������
		// ------------------------------
		// ��������/��������� ������������������� �� ��� X
		void __fastcall SetXAutoScale(bool value);

		// ���������� ��������� ������������������� �� ��� X
		bool __fastcall GetXAutoScale(void);

		// ��������/��������� ������������������� �� ��� Y
		void __fastcall SetYAutoScale(bool value);

		// ���������� ��������� ������������������� �� ��� Y
		bool __fastcall GetYAutoScale(void);

		// ������������� ������� ��������� �� ��� X
		void __fastcall SetXMin(double value);
		void __fastcall SetXMax(double value);

		// ������������� ������� ��������� �� ��� Y
		void __fastcall SetYMin(double value);
		void __fastcall SetYMax(double value);

		// ���������� ������� ��������� �� ��� X
		double __fastcall GetXMin(void);
		double __fastcall GetXMax(void);

		// ���������� ������� ��������� �� ��� Y
		double __fastcall GetYMin(void);
		double __fastcall GetYMax(void);

		// ������������� ��������� �������
		void __fastcall SetGraphTitle(AnsiString title);

		// ������������� ��������� �������� �� ����
		void __fastcall SetXLabelVisible(bool value);
		void __fastcall SetYLabelVisible(bool value);

		// ���������� ��������� �������� �� ����
		bool __fastcall GetXLabelVisible(void);
		bool __fastcall GetYLabelVisible(void);

		// ������������� ������� �� ����
		void __fastcall SetXLabelTitle(AnsiString value);
		void __fastcall SetYLabelTitle(AnsiString value);

		// ���������� ������� �� ����
		AnsiString __fastcall GetXLabelTitle(void);
		AnsiString __fastcall GetYLabelTitle(void);

		// ������������� ��������� �������
		void __fastcall SetLegendVisible(bool value);

		// ���������� ��������� �������
		bool __fastcall GetLegendVisible(void);

		// ������������� ��������� �������
		// 0 - �����
		// 1 - ����� (�� �������)
		// 2 - ������
		// 3 - ������ (�� �������)
		// 4 - ������
		// 5 - �����
		void __fastcall SetLegendPosition(int value);

		// ���������� ��������� �������
		int __fastcall GetLegendPosition(void);

		// ���������� �������� ����������
		double __fastcall GetWatchInterval(void);

		// ������������� �������� ����������
		void __fastcall SetWatchInterval(double value);

		// ������ ���� ������������ ������
		int GetCacheSize(void) const;
		bool SetCacheSize(int value);
		// ------------------------------

		// ------------------------------
        // ������ ���������� ����������
        // ------------------------------
        // ���������� 'true', ���� ������ � ������ ���� ��������,
        // ��� ���� ����� ���� ���������/�������
		bool __fastcall GetModifyState(void);
        // ------------------------------

        // ------------------------------
        // ������ ���������� ������������
		// ------------------------------
		// ���������� ������ ����������
		TUWatchInfo* __fastcall Get(int seriesindex);

		// ���������� ����� ����� ������ ����������
		int __fastcall GetNumWatches(void);

		// ���������� ������ ����������
		// ���������� ������ �����
		int __fastcall Add(TUWatchInfo& wd);

		// ���������� ������ ���������� �� ����� ���������� � ������� ������
		// ���������� ������ �����
		int __fastcall Add(int type, const string &xname, const string &yname, const string &xoutput, int xoutindex, const string &youtput, int youtindex, int mrow, int mcol, double yshift=0, TPenStyle style=psSolid, TColor color=TColor(0));
		int __fastcall Add(int type, const string &xname, const string &yname, int xoutput, int xoutindex, int youtput, int youtindex, int mrow, int mcol, double yshift=0, TPenStyle style=psSolid, TColor color=TColor(0));

        // �������� ����������
        void __fastcall Del(int seriesindex);

        // ������� ��� ����������
		void __fastcall Clear(void);

		// ��������� ����������� �����
		// ������������ ��� ���������� ������
		// ���� seriesindex < 0 �� ��������� ��� �����
		void __fastcall SeriesDisable(int seriesindex=-1);

		// �������� ����������� �����
		// ������������ ��� ���������� ������
		// ���� seriesindex < 0 �� �������� ��� �����
		void __fastcall SeriesEnable(int seriesindex=-1);

		// ���������� ���������� �� 'stepcount' ��������� ����� ��������������
		void __fastcall StepUpdate(void);

        // ������� ������ ���� ����������
        void __fastcall Reset(void);

        // ���������� ����� ������ ���� ����������� �����
		void __fastcall GetWatchList(map<int, TUWatchInfo> &buffer);

		// ���������� ����� ������ ���� ������� �����
		void __fastcall GetVisibleList(map<int, TUWatchInfo> &buffer);

		// ���������� ����� ������ ���� ��������� �����
		void __fastcall GetInvisibleList(map<int, TUWatchInfo> &buffer);

		// �������� ������� ��� ��������� ������
		void __fastcall ChangeLegend(int seriesindex, string legend);

        // �������� ���� ��������� �����
		void __fastcall ChangeColor(int seriesindex, TColor color);

		// �������� ��� ����� ��������� �����
		void __fastcall ChangeLineStyle(int seriesindex, TPenStyle style);

		// �������� ���������� � ��������� �����
		void __fastcall ChangeVisible(int seriesindex, bool visible);

		// �������� �������� �� ��� Y
		void __fastcall ChangeYShift(int seriesindex, double yshift);

		// �������� ������� �����
		void __fastcall ChangeLineWidth(int seriesindex, int width);
		// ------------------------------

		// -----------------------------
		// ������ ����������� ����������
		// -----------------------------
		// ��������� ������ �����
		void __fastcall TBSeriesModify(TObject *Sender);

		// ���������� ������� � ����
		// ��������� ����������� ������� � bmp
		bool SaveToBitmap(const AnsiString &filename);

		// ��������� ����������� � ��������
		bool SaveToMetafile(const AnsiString &filename);

		// ��������� ����������� ������� � jpeg
		bool SaveToJpeg(const AnsiString &filename);

		// ��������� ����������� � ������� ���� �� �������
		bool Save(void);
		// -----------------------------

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
virtual void AAfterReset(void);

// ���������� ����������
virtual void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUWatchFrame *UWatchFrame;
//---------------------------------------------------------------------------
#endif
