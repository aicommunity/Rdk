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

/// Список поддерживаемых источников видео
static std::map<int, RDK::UEPtr<TVideoCaptureThread> > VideoSourcePrototypes;

/// Список настройщиков источников видео
static std::map<int, TVideoCaptureOptionsDesciption> VideoSourceOptionsFrames;

/// Список настройщиков источников видео этой формы
std::map<int, TVideoCaptureOptionsDesciption> CurrentVideoSourceOptionsFrames;

/// Указатель на текущий интерфейс видео
//TVideoCaptureOptionsDesciption* VideoCaptureOptionsInterface;

/// Указатель на текущий фрейм отображения источника видео
TVideoOutputFrame* VideoOutputFrame;

// ---------------------------
// Методы управления поддерживаемыми источниками видео
// ---------------------------
/// Возвращает список поддерживаемых источников видео
static const std::map<int, RDK::UEPtr<TVideoCaptureThread> >& GetVideoSourcePrototypes(void);

/// Добавляет новый прототип источника видео
static bool AddVideoSourcePrototypes(int mode, RDK::UEPtr<TVideoCaptureThread> thread);

/// Проверяет, существует ли такой видеоисточник
static bool CheckVideoSourcePrototypes(int mode);

/// Очищает список поддерживаемых источников видео
static void ClearAllVideoSourcePrototypes(void);

/// Возвращает список настройщиков источников видео
static std::map<int, TVideoCaptureOptionsDesciption> GetVideoSourceOptionsFrames(void);

/// Добавляет новый настройщик источников видео
static bool AddVideoSourceOptionsFrame(int mode, TVideoCaptureOptionsDesciption &frame);

/// Проверяет, существует ли такой настройщик
static bool CheckVideoSourceOptionsFrame(int mode);

/// Очищает список поддерживаемых настройщиков видео
static void ClearAllVideoSourceOptionsFrames(void);

/// Возвращает активный (выбранный) источник видео
int GetActiveVideoSourceId(void);

/// Возвращает настройщик видеоисточника, соответствующий режиму
TVideoCaptureOptionsDesciption* GetCurrentVideoSourceOptionsFrame(int mode);

/// Устанавливает активной вкладку видеоисточника по индексу
bool SelectVideoSourcePage(int mode);
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
