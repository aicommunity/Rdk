//---------------------------------------------------------------------------

#ifndef TVideoGrabberControlFrameUnitH
#define TVideoGrabberControlFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "rdk.bcb.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Mask.hpp>
#include <IniFiles.hpp>
#include "VidGrab.hpp"
#include "VideoOutputFrameUnit.h"
#include "TUVisualController.h"
//---------------------------------------------------------------------------
class TVideoGrabberControlFrame : public TUVisualControllerFrame
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
    TOpenDialog *VideoOpenDialog;
    TOpenDialog *PicturesOpenDialog;
    TCheckBox *VideoTruncPathCheckBox;
    TCheckBox *PictureTruncPathCheckBox;
    void __fastcall DeviceComboBoxSelect(TObject *Sender);
    void __fastcall InputComboBoxSelect(TObject *Sender);
    void __fastcall VideoSizeComboBoxSelect(TObject *Sender);
    void __fastcall VFBrowseButtonClick(TObject *Sender);
    void __fastcall VCapturePageControlChange(TObject *Sender);
    void __fastcall VideoSubTypeComboBoxSelect(TObject *Sender);
    void __fastcall AnalogVideoStandardComboBoxSelect(TObject *Sender);
    void __fastcall OpenImageFileButtonClick(TObject *Sender);
private:    // User declarations
public:        // User declarations
    __fastcall TVideoGrabberControlFrame(TComponent* Owner);

protected:

TVideoGrabber* VideoGrabber;
TVideoOutputFrame *VideoOutputFrame;

// -----------------------------
// Методы управления устройством ввода видео
// -----------------------------
public:
// Выбор управляемой компоненты видеозахвата
TVideoGrabber* GetVideoGrabber(void);
TVideoOutputFrame* GetVideoOutputFrame(void);
void Init(TVideoOutputFrame* video_output_frame, TVideoGrabber* grabber);

// Выбор активного режима видеоввода
int GetMode(void) const;
void SelectMode(int mode);

void __fastcall AssignListToComboBox (TComboBox* ComboBox, String List, int Index);
// -----------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
void ABeforeCalculate(void);

// Метод, вызываемый после шага расчета
void AAfterCalculate(void);

// Обновление интерфейса
void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::Serialize::USerStorageXML &xml);
// -----------------------------


};
//---------------------------------------------------------------------------
extern PACKAGE TVideoGrabberControlFrame *VideoGrabberControlFrame;
//---------------------------------------------------------------------------
#endif
