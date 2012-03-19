//---------------------------------------------------------------------------

#ifndef TVideoGrabberControlFrameUnitH
#define TVideoGrabberControlFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "rdk_builder.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Mask.hpp>
#include "VidGrab.hpp"
#include "VideoOutputFrameUnit.h"
//---------------------------------------------------------------------------
class TVideoGrabberControlFrame : public TFrame
{
__published:    // IDE-managed Components
    TPageControl *VCapturePageControl;
    TTabSheet *DeviceTabSheet;
    TPanel *VDCapturePanel;
    TLabel *ModeLabel;
    TLabel *InputLabel;
    TLabel *DeviceLabel;
	TComboBox *VideoSizeComboBox;
    TComboBox *InputComboBox;
    TComboBox *DeviceComboBox;
    TTabSheet *VideoFileTabSheet;
    TPanel *VFCapturePanel;
    TLabel *VideoFileNameLabel;
    TEdit *VFNameEdit;
    TButton *VFBrowseButton;
    TTabSheet *PictureFileTabSheet;
	TComboBox *VideoSubTypeComboBox;
	TLabel *Label1;
	TComboBox *AnalogVideoStandardComboBox;
	TLabel *Label2;
	TEdit *ImageFileNameEdit;
	TLabel *Label3;
	TButton *OpenImageFileButton;
	TOpenDialog *OpenDialog;
    void __fastcall DeviceComboBoxSelect(TObject *Sender);
    void __fastcall InputComboBoxSelect(TObject *Sender);
    void __fastcall VideoSizeComboBoxSelect(TObject *Sender);
    void __fastcall VFBrowseButtonClick(TObject *Sender);
    void __fastcall TimeEditChange(TObject *Sender);
    void __fastcall TimeTrackBarChange(TObject *Sender);
    void __fastcall VCapturePageControlChange(TObject *Sender);
	void __fastcall VideoSubTypeComboBoxSelect(TObject *Sender);
	void __fastcall AnalogVideoStandardComboBoxSelect(TObject *Sender);
	void __fastcall OpenImageFileButtonClick(TObject *Sender);
private:    // User declarations
public:        // User declarations
    __fastcall TVideoGrabberControlFrame(TComponent* Owner);

protected:
bool UpdateCaptureInterfaceFlag;

TVideoGrabber* VideoGrabber;
TVideoOutputFrame *VideoOutputFrame;

// ���� ���� true - ��� ����� �������� ������� - ���������� ��������� �����
// ��������������� �������� Pause.
bool PauseInvisibleFlag;

// -----------------------------
// ������ ���������� �����������
// -----------------------------
public:
// ���� ���� true - ��� ����� �������� ������� - ���������� ��������� �����
// ��������������� �������� Pause.
bool GetPauseInvisibleFlag(void) const;
bool SetPauseInvisibleFlag(bool value);
// -----------------------------

// -----------------------------
// ������ ���������� ����������� ����� �����
// -----------------------------
// ����� ����������� ���������� ������������
TVideoGrabber* GetVideoGrabber(void);
TVideoOutputFrame* GetVideoOutputFrame(void);
void Init(TVideoOutputFrame* video_output_frame, TVideoGrabber* grabber);

void __fastcall AssignListToComboBox (TComboBox* ComboBox, String List, int Index);

// ������������� ���������� ����������
//bool SetDCapture(RDK::VCapture::VDCapture *capture);
//bool SetFCapture(RDK::VCapture::VFCapture *capture);

// ���������� ���������� ����������
//RDK::VCapture::VDCapture* GetDCapture(void) const;
//RDK::VCapture::VFCapture* GetFCapture(void) const;

// ���������� ������� ���������� ����������
//RDK::VCapture::VACapture* GetCapture(void) const;

// ��������� ��������� ������
void UpdateInterface(void);
// -----------------------------


};
//---------------------------------------------------------------------------
extern PACKAGE TVideoGrabberControlFrame *VideoGrabberControlFrame;
//---------------------------------------------------------------------------
#endif
