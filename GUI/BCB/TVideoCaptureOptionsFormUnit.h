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

class TVideoOutputFrame;
//---------------------------------------------------------------------------
class TVideoCaptureOptionsForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TButton *ApplyButton;
	TButton *Cancel;
	TButton *RestoreButton;
	TPageControl *PageControl;
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsForm(TComponent* Owner);

/// ������ ������������ ���������� �����
static std::map<int, TFrame* > VideoSourceOptionsFrames;

/// ��������� �� ������� ��������� �����
TVideoCaptureOptionsInterface *VideoCaptureOptionsInterface;

/// ��������� �� ������� ����� ����������� ��������� �����
TVideoOutputFrame* VideoOutputFrame;

// ---------------------------
// ������ ���������� ��������������� ����������� �����
// ---------------------------
/// ���������� ������ ������������ ���������� �����
static std::map<int, TFrame* > GetVideoSourceOptionsFrames(void);

/// ��������� ����� ���������� ���������� �����
static bool AddVideoSourceOptionsFrame(int mode, TFrame *frame);

/// ���������, ���������� �� ����� ����������
static bool CheckVideoSourceOptionsFrame(int mode);

/// ������� ������ �������������� ������������ �����
static void ClearAllVideoSourceOptionsFrames(void);

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
