//---------------------------------------------------------------------------


#ifndef WatchFrameUnitH
#define WatchFrameUnitH
#include <Classes.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ExtDlgs.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <series.hpp>

#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>

#include <IniFiles.hpp>
#include <jpeg.hpp>
#include <Chart.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Menus.hpp>


#include <string>
#include <map>
#include <vector>

using namespace std;

//typedef float real;
//---------------------------------------------------------------------------
/*
   ��������������� ����� - ��������� ������, ��������������� ����������� �����
�����.
*/
//---------------------------------------------------------------------------
class OutGateWatchData
{
public: // �����
// ������ ������� � ������� �����
int SeriesIndex;

// ��������� �� �������� ������
void* DataSource;

public: // ������ �������
const double *X;
const double *Y;
int XYSize;

// ��� ��������� � �������� �� �������� �������� �������
double XCurrentMax,YCurrentMax;
double XCurrentMin,YCurrentMin;

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
OutGateWatchData(void);
~OutGateWatchData(void);
OutGateWatchData(const OutGateWatchData &wd);
OutGateWatchData& operator = (const OutGateWatchData& wd);
};
        
//---------------------------------------------------------------------------
class TWatchFrame : public TFrame
{
__published:	// IDE-managed Components
        TChart *Chart1;
	TSavePictureDialog *SavePictureDialog;
	TPopupMenu *PopupMenu;
	TMenuItem *N1;
	TMenuItem *bmp1;
	void __fastcall N1Click(TObject *Sender);
	void __fastcall bmp1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TWatchFrame(TComponent* Owner);
        virtual __fastcall ~TWatchFrame(void);

protected:	// ���������
// ������ ���� ������������ ������
int CacheSize;

protected:	// ������
        // -----------------------------------------
        // ������ �� ����������� ������ ����� ������
        // -----------------------------------------
        // ������ ��� ����������� ����� �����, � ��������
        // �������� ����� ���� ������������ ����� �� ��� Y.
		vector<OutGateWatchData> NameList;

		// ������� ������ ���������� � ����
		int CacheIndex;
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

        // �������� ���������� �� �������� ������ � ���� 'watchname'
        // ���� 'collectstate' == 'true', �� ��������� ����� ����������� ����������
        bool __fastcall CollectInfo(string watchname, bool collectstate);

        // ��������������� ����� �� ���������� �� ����� 'watchname'
        // ���� 'collectstate' == 'true', �� ��������������� ����� �����������
        // ����������
        // (��� �������� ���������� ����� �������������� �������)
        bool __fastcall RestoreInfo(string watchname, bool collectstate);
        // ------------------------------

        // ------------------------------
        // ������ ���������� ������������
		// ------------------------------
		// ���������� ������ ����������
		OutGateWatchData* __fastcall Get(int seriesindex);
		OutGateWatchData* __fastcall Get(void *datasource);

		// ���������� ����� ����� ������ ����������
		int __fastcall GetNumWatches(void);

		// ���������� ������ ����������
		// ���������� ������ �����
		int __fastcall Add(OutGateWatchData& wd);

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

		// ���������� ������ �����
		// ���������� ����� ����� � �����
		void __fastcall SeriesUpdate(int seriesindex, double x, double y);
		void __fastcall SeriesUpdate(void* datasource, double x, double y);

		// ���������� ������ �����
		// ���������� ������� ����� � �����
		void __fastcall SeriesUpdate(int seriesindex, double* x, double* y, int size);
		void __fastcall SeriesUpdate(void* datasource, double* x, double* y, int size);

		// ���������� ���������� �� 'stepcount' ��������� ����� ��������������
		void __fastcall StepUpdate(bool speedup=true);

        // ������� ������ ���� ����������
        void __fastcall Reset(void);

        // ���������� ����� ������ ���� ����������� �����
		void __fastcall GetWatchList(map<int, OutGateWatchData> &buffer);

		// ���������� ����� ������ ���� ������� �����
		void __fastcall GetVisibleList(map<int, OutGateWatchData> &buffer);

		// ���������� ����� ������ ���� ��������� �����
		void __fastcall GetInvisibleList(map<int, OutGateWatchData> &buffer);

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
};
//---------------------------------------------------------------------------
extern PACKAGE TWatchFrame *WatchFrame;
//---------------------------------------------------------------------------
#endif
