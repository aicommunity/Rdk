//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsBmpSequenceFrameUnitH
#define TVideoCaptureOptionsBmpSequenceFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TVideoCaptureOptionsInterface.h"

//---------------------------------------------------------------------------
class TVideoCaptureOptionsBmpSequenceFrame : public TFrame, public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TLabel *Label7;
	TEdit *ImageSequencePathEdit;
	TCheckBox *ImageSequencePathCheckBox;
	TButton *ImageSequencePathBrowseButton;
	TCheckBox *RepeatSequenceCheckBox;
	TLabeledEdit *ImageSequenceFpsLabeledEdit;
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsBmpSequenceFrame(TComponent* Owner);


/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
virtual bool LoadParamters(RDK::USerStorageXML &xml);

/// ���������� ��������� �� ����� ���������� � xml
virtual bool SaveParamters(RDK::USerStorageXML &xml);
/// -------------------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsBmpSequenceFrame *VideoCaptureOptionsBmpSequenceFrame;
//---------------------------------------------------------------------------
#endif
