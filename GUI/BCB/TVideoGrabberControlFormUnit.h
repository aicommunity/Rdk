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
//---------------------------------------------------------------------------
class TVideoGrabberControlForm : public TUVisualControllerForm
{
__published:	// IDE-managed Components
	TVideoGrabberControlFrame *VideoGrabberControlFrame;
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
void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// ��������� ��������� ���������� �� xml
void ALoadParameters(RDK::Serialize::USerStorageXML &xml);
// -----------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoGrabberControlForm *VideoGrabberControlForm;
//---------------------------------------------------------------------------
#endif
