//---------------------------------------------------------------------------

#ifndef TVideoCaptureOptionsVideoFileFrameUnitH
#define TVideoCaptureOptionsVideoFileFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "TVideoCaptureOptionsInterface.h"

//---------------------------------------------------------------------------
class TVideoCaptureOptionsVideoFileFrame : public TFrame, public TVideoCaptureOptionsInterface
{
__published:	// IDE-managed Components
	TPanel *VFCapturePanel;
	TLabel *VideoFileNameLabel;
	TEdit *VFNameEdit;
	TButton *VFBrowseButton;
	TCheckBox *VideoTruncPathCheckBox;
	TCheckBox *RepeatVideoCheckBox;
	TCheckBox *ProcessAllFramesVideoCheckBox;
private:	// User declarations
public:		// User declarations
	__fastcall TVideoCaptureOptionsVideoFileFrame(TComponent* Owner);


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
extern PACKAGE TVideoCaptureOptionsVideoFileFrame *VideoCaptureOptionsVideoFileFrame;
//---------------------------------------------------------------------------
#endif
