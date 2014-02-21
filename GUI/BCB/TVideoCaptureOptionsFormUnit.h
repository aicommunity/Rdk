//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsFormUnitH
#define TVideoCaptureOptionsFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include "TUVisualControllerFormUnit.h"
#include "TVideoCaptureOptionsInterface.h"
//#include "TVideoOutputFrameUnit.h"
#include "TVideoSourceThread.h"

class TVideoOutputFrame;

struct TVideoCaptureOptionsDesciption
{
 TVideoCaptureOptionsInterface* Form;

 std::string Name;

 int Position;
};

//---------------------------------------------------------------------------
class TVideoCaptureOptionsForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TButton *ApplyButton;
	TButton *Cancel;
	TButton *RestoreButton;
	TPageControl *PageControl;
	void __fastcall ApplyButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall RestoreButtonClick(TObject *Sender);
	void __fastcall CancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsForm(TComponent* Owner);

/// ������ �������������� ���������� �����
static std::map<int, RDK::UEPtr<TVideoCaptureThread> > VideoSourcePrototypes;

/// ������ ������������ ���������� �����
static std::map<int, TVideoCaptureOptionsDesciption> VideoSourceOptionsFrames;

/// ������ ������������ ���������� ����� ���� �����
std::map<int, TVideoCaptureOptionsDesciption> CurrentVideoSourceOptionsFrames;

/// ��������� �� ������� ��������� �����
//TVideoCaptureOptionsDesciption* VideoCaptureOptionsInterface;

/// ��������� �� ������� ����� ����������� ��������� �����
TVideoOutputFrame* VideoOutputFrame;

// ---------------------------
// ������ ���������� ��������������� ����������� �����
// ---------------------------
/// ���������� ������ �������������� ���������� �����
static const std::map<int, RDK::UEPtr<TVideoCaptureThread> >& GetVideoSourcePrototypes(void);

/// ��������� ����� �������� ��������� �����
static bool AddVideoSourcePrototypes(int mode, RDK::UEPtr<TVideoCaptureThread> thread);

/// ���������, ���������� �� ����� �������������
static bool CheckVideoSourcePrototypes(int mode);

/// ������� ������ �������������� ���������� �����
static void ClearAllVideoSourcePrototypes(void);

/// ���������� ������ ������������ ���������� �����
static std::map<int, TVideoCaptureOptionsDesciption> GetVideoSourceOptionsFrames(void);

/// ��������� ����� ���������� ���������� �����
static bool AddVideoSourceOptionsFrame(int mode, TVideoCaptureOptionsDesciption &frame);

/// ���������, ���������� �� ����� ����������
static bool CheckVideoSourceOptionsFrame(int mode);

/// ������� ������ �������������� ������������ �����
static void ClearAllVideoSourceOptionsFrames(void);

/// ���������� �������� (���������) �������� �����
int GetActiveVideoSourceId(void);

/// ���������� ���������� ��������������, ��������������� ������
TVideoCaptureOptionsDesciption* GetCurrentVideoSourceOptionsFrame(int mode);

/// ������������� �������� ������� �������������� �� �������
bool SelectVideoSourcePage(int mode);
// ---------------------------

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// ���� force_update == true, �� ��������� �����������
// ��� ����������� �� UpdateInterval
virtual void AUpdateInterface(void);

// ������� ���������� � �������� ���������
virtual void AClearInterface(void);

// ��������� ��������� ���������� � xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// �������� ����� ����� ����������
virtual TUVisualControllerForm* New(TComponent *owner=0);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsForm *VideoCaptureOptionsForm;
//---------------------------------------------------------------------------
#endif
