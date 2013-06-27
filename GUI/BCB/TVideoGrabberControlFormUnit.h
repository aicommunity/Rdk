#ifndef RDK_VIDEO
#define RDK_VIDEO
#endif
//---------------------------------------------------------------------------

#ifndef TVideoGrabberControlFormUnitH
#define TVideoGrabberControlFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVideoGrabberControlFrameUnit.h"
#include "TUVisualController.h"
#include "TUVisualControllerFormUnit.h"
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TVideoGrabberControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TVideoGrabberControlFrame *VideoGrabberControlFrame;
	TPanel *Panel1;
	TButton *CloseButton;
	void __fastcall CloseButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoGrabberControlForm(TComponent* Owner);


// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ����� �������
void ABeforeCalculate(void);

// �����, ���������� ����� ���� �������
void AAfterCalculate(void);

// ���������� ����������
void AUpdateInterface(void);

// ��������� ��������� ���������� � xml
void ASaveParameters(RDK::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::USerStorageXML &xml);

// �������� ����� ����� ����������
virtual TVideoGrabberControlForm* New(TComponent *owner=0);
// -----------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoGrabberControlForm *VideoGrabberControlForm;
//---------------------------------------------------------------------------
#endif
