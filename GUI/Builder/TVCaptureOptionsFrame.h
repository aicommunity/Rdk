//---------------------------------------------------------------------------

#ifndef TVCaptureOptionsFrameH
#define TVCaptureOptionsFrameH
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
//---------------------------------------------------------------------------
class TVCaptureOptionsFrame : public TFrame
{
__published:    // IDE-managed Components
    TPageControl *VCapturePageControl;
    TTabSheet *DeviceTabSheet;
    TPanel *VDCapturePanel;
    TLabel *ModeLabel;
    TLabel *InputLabel;
    TLabel *DeviceLabel;
    TComboBox *ModeComboBox;
    TComboBox *InputComboBox;
    TComboBox *DeviceComboBox;
    TTabSheet *VideoFileTabSheet;
    TPanel *VFCapturePanel;
    TLabel *VideoFileNameLabel;
    TEdit *VFNameEdit;
    TButton *VFBrowseButton;
    TTabSheet *PictureFileTabSheet;
    TOpenDialog *VFOpenDialog;
    TTrackBar *TimeTrackBar;
    TMaskEdit *TimeEdit;
    void __fastcall DeviceComboBoxSelect(TObject *Sender);
    void __fastcall InputComboBoxSelect(TObject *Sender);
    void __fastcall ModeComboBoxSelect(TObject *Sender);
    void __fastcall VFBrowseButtonClick(TObject *Sender);
    void __fastcall TimeEditChange(TObject *Sender);
    void __fastcall TimeTrackBarChange(TObject *Sender);
    void __fastcall VCapturePageControlChange(TObject *Sender);
private:    // User declarations
public:        // User declarations
    __fastcall TVCaptureOptionsFrame(TComponent* Owner);

//RDK::VCapture::VDCapture* DCapture;
//RDK::VCapture::VFCapture* FCapture;
bool UpdateCaptureInterfaceFlag;

// Если флаг true - при смене активной вкладке - неактивные источники видео
// останавливаются командой Pause.
bool PauseInvisibleFlag;

// -----------------------------
// Методы управления параметрами
// -----------------------------
// Если флаг true - при смене активной вкладке - неактивные источники видео
// останавливаются командой Pause.
bool GetPauseInvisibleFlag(void) const;
bool SetPauseInvisibleFlag(bool value);
// -----------------------------

// -----------------------------
// Методы управления устройством ввода видео
// -----------------------------
// Устанавливает устройство видеоввода
//bool SetDCapture(RDK::VCapture::VDCapture *capture);
//bool SetFCapture(RDK::VCapture::VFCapture *capture);

// Возвращает устройство видеоввода
//RDK::VCapture::VDCapture* GetDCapture(void) const;
//RDK::VCapture::VFCapture* GetFCapture(void) const;

// Возвращает текущее устройство видеоввода
//RDK::VCapture::VACapture* GetCapture(void) const;

// Обновляет интерфейс фрейма
void UpdateInterface(void);
// -----------------------------


};
//---------------------------------------------------------------------------
extern PACKAGE TVCaptureOptionsFrame *VCaptureOptionsFrame;
//---------------------------------------------------------------------------
#endif
