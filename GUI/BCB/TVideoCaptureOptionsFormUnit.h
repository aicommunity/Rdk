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

/// Список настройщиков источников видео
static std::map<int, TFrame* > VideoSourceOptionsFrames;

/// Указатель на текущий интерфейс видео
TVideoCaptureOptionsInterface *VideoCaptureOptionsInterface;

/// Указатель на текущий фрейм отображения источника видео
TVideoOutputFrame* VideoOutputFrame;

// ---------------------------
// Методы управления поддерживаемыми источниками видео
// ---------------------------
/// Возвращает список настройщиков источников видео
static std::map<int, TFrame* > GetVideoSourceOptionsFrames(void);

/// Добавляет новый настройщик источников видео
static bool AddVideoSourceOptionsFrame(int mode, TFrame *frame);

/// Проверяет, существует ли такой настройщик
static bool CheckVideoSourceOptionsFrame(int mode);

/// Очищает список поддерживаемых настройщиков видео
static void ClearAllVideoSourceOptionsFrames(void);

// ---------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Если force_update == true, то интерфейс обновляется
// вне зависимости от UpdateInterval
virtual void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);

// Создание копии этого компонента
virtual TUVisualControllerForm* New(TComponent *owner=0);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TVideoCaptureOptionsForm *VideoCaptureOptionsForm;
//---------------------------------------------------------------------------
#endif
