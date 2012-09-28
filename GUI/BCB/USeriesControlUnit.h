//---------------------------------------------------------------------------

#ifndef USeriesControlUnitH
#define USeriesControlUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>
#include "UWatchFrameUnit.h"
//---------------------------------------------------------------------------
class TUSeriesControlForm : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel2;
        TPanel *Panel4;
        TPanel *Panel5;
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        TButton *Button4;
        TColorDialog *ColorDialog1;
        TPanel *Panel3;
        TPanel *Panel6;
        TBitBtn *BitBtn1;
        TPanel *Panel1;
        TLabel *Label1;
        TGroupBox *GroupBox1;
        TGroupBox *GroupBox2;
        TCheckBox *CheckBox2;
        TEdit *Edit2;
        TCheckBox *CheckBox4;
        TEdit *Edit1;
        TCheckBox *CheckBox1;
        TCheckBox *CheckBox3;
        TLabel *Label2;
        TEdit *Edit3;
        TLabel *Label3;
        TLabel *Label4;
        TEdit *Edit4;
        TBitBtn *BitBtn2;
        TPanel *OutterCPanel;
        TPanel *InnerCPanel;
        TCheckBox *CheckBox5;
	TGroupBox *GroupBox3;
	TStringGrid *StringGrid1;
	TGroupBox *GroupBox4;
	TStringGrid *StringGrid2;
	TLabel *Label5;
	TComboBox *ComboBox1;
	TLabel *Label6;
	TComboBox *ComboBox2;
	TLabel *Label7;
	TComboBox *ComboBox3;
	TButton *DelPointButton;
	TLabel *Label8;
	TEdit *WatchIntervalEdit;
        void __fastcall OutterCPanelDblClick(TObject *Sender);
        void __fastcall InnerCPanelDblClick(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall CheckBox1Click(TObject *Sender);
        void __fastcall CheckBox2Click(TObject *Sender);
        void __fastcall StringGrid2Click(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall StringGrid2DblClick(TObject *Sender);
        void __fastcall Edit3DblClick(TObject *Sender);
        void __fastcall BitBtn1Click(TObject *Sender);
        void __fastcall CheckBox3Click(TObject *Sender);
        void __fastcall CheckBox4Click(TObject *Sender);
        void __fastcall StringGrid1DblClick(TObject *Sender);
        void __fastcall Edit4KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall Edit4Exit(TObject *Sender);
        void __fastcall Edit1Change(TObject *Sender);
        void __fastcall Edit2Change(TObject *Sender);
        void __fastcall BitBtn2Click(TObject *Sender);
        void __fastcall CheckBox5Click(TObject *Sender);
	void __fastcall ComboBox1Select(TObject *Sender);
	void __fastcall ComboBox2Select(TObject *Sender);
	void __fastcall ComboBox3Select(TObject *Sender);
	void __fastcall DelPointButtonClick(TObject *Sender);
	void __fastcall WatchIntervalEditChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TUSeriesControlForm(TComponent* Owner);

protected: // ������
        // ������������� ���� ����������
        TUWatchFrame* GrSender;

        // -----------------------------------------
        // ������ �� ����������� ������ ����� ������
        // -----------------------------------------
        // ������ ��� ����������� ����� �����, � ��������
        // �������� ����� ���� ������������ ����� �� ��� Y.
		map<int,TUWatchInfo> WatchList;

		// ������ ��� ��������� ��� ���������� ����� �����
		map<int,TUWatchInfo> PossibleWatchList;

		// ������ ��� ���������� ��� �������� ����� �����
		map<int,TUWatchInfo> DelWatchList;
		// -----------------------------------------

        // -----------------------------------------
        // ��������, ����������� ��������� ����������� �����
        // -----------------------------------------
        // ��������� ��������� ����������� ���� � ������� � ���
        bool GraphChanged;

        // ��������� ��������� �������� ����� �����
        bool ColorsChanged;

        // ��������� ��������� ������� �����
        bool LegendsChanged;

        // ��������� ���������� ����� �/��� �� ��������
		bool SeriesChanged;

		// ��������� ��������� ��� �����
		bool LineStyleChanged;

		// ��������� ��������� ������� �����
		bool LineWidthChanged;
		// -----------------------------------------

protected: // ��������������� ������
        // ------------------------
        // ������ ���������� ���������� ����������/�������� ����� ����������
        // ------------------------
        // ���������� ������� ������
        void __fastcall UpdateDataLists(void);

        // ��������� ���������� � ����� � ��������� ���� ��������� ����������
        void __fastcall UpdateInfo(void);

        // ��������� ���������� �� ��������� ����������� ����� �����
        void __fastcall UpdateSelectedWatch(void);
        // ------------------------

public: // ������
        // ------------------------
        // ������ ���������� ������
        // ------------------------
        // �����, ������� ������ ���� ������ ��� �������� �������
        void __fastcall Execute(TUWatchFrame* grsender);

        // ------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUSeriesControlForm *USeriesControlForm;
//---------------------------------------------------------------------------
#endif
