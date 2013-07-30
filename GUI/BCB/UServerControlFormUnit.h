//---------------------------------------------------------------------------

#ifndef UServerControlFormUnitH
#define UServerControlFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include "TUVisualControllerFormUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include "TUHttpServerUnit.h"
#include "TUVisualControllerFrameUnit.h"

#include "myrdk.h"

/// ����������� �������, �������������� ������������� ���������� �������
int StandardCommandRequestDecoder(std::map<std::string,std::vector<char> > &source, std::map<std::string,std::vector<char> > &dest);

/// ����������� �������, �������������� ����������� ���������� ������
int StandardCommandResponseEncoder(const std::string &response_type, std::vector<char> &source, std::vector<char> &dest);

//---------------------------------------------------------------------------
class TUServerControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TTabSheet *ControlTabSheet;
	TTabSheet *OptionsTabSheet;
	TPanel *Panel1;
	TGroupBox *GroupBox1;
	TLabeledEdit *ServerControlPortLabeledEdit;
	TLabeledEdit *NumberOfChannelsLabeledEdit;
	TGroupBox *GroupBox2;
	TValueListEditor *ChannelNamesValueListEditor;
	TButton *ApplyChannelsNumberButton;
	TGroupBox *GroupBox3;
	TChart *PerformanceChart;
	TBarSeries *Series1;
	TPanel *Panel;
	TRadioButton *FpsRadioButton;
	TRadioButton *RadioButton2;
	TPanel *Panel2;
	TUHttpServerFrame *UHttpServerFrame;
	void __fastcall UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
          TIdHTTPResponseInfo *AResponseInfo);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUServerControlForm(TComponent* Owner);

/// ��������� �� �������, �������������� ������������� ���������� �������
int (*CommandRequestDecoder)(std::map<std::string,std::vector<char> > &source, std::map<std::string,std::vector<char> > &dest);

/// ��������� �� �������, �������������� ����������� ���������� ������
int (*CommandResponseEncoder)(const std::string &response_type, std::vector<char> &source, std::vector<char> &dest);

/// ����� (���) �������
/// 0 - ��������� � ������� ���������� ��������� (Control)
/// 1 - ��������� ����� �������� ������ ��������� ������
/// 1 - ��������� � ������ (Engine)
/// 2 - ��������� � ������ (Model)
/// 2 - ��������� � ����� ���������� (Env)
/// 3 - ��������� � ��������� ��������� (Storage)
/// <0 - ����������� ��� �������
int Mode;

/// ������������ ������ ������� �������
std::map<std::string,std::vector<char> > DecodedRequest;

/// �����
std::vector<char> Response;

/// ����������� ��� �������� �����
std::vector<char> EncodedResponse;

/// ��� ������
std::string ResponseType;

/// ������� �������
std::string Command;

/// ������ ������ �������
std::string ChannelIndex;

TMemoryStream* MemStream;
	TBitmap *Bitmap;

	RDK::UBitmap TempUBitmap;

// �������, �������������� ������� ���������� ��������
void ProcessControlCommand(const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data);

// �������, �������������� ������� ���������� ������ ��������
void ProcessRPCCommand(int channel, const std::string &cmd_name, std::map<std::string,std::vector<char> > &args, std::string &response_type, std::vector<char> &response_data);

/// �������� ������ � ������
void ConvertStringToVector(const std::string &source, std::vector<char> &dest);


// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
virtual void AAfterReset(void);

// ���������� ����������
virtual void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUServerControlForm *UServerControlForm;
//---------------------------------------------------------------------------
#endif
