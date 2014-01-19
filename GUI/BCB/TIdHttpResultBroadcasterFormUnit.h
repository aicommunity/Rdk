//---------------------------------------------------------------------------

#ifndef TIdHttpResultBroadcasterFormUnitH
#define TIdHttpResultBroadcasterFormUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
#include "TUVisualControllerFormUnit.h"
#include "TIdHttpResultBroadcasterFrameUnit.h"
#include "TServerBroadcasterCommonUnit.h"
#include <vector>
//---------------------------------------------------------------------------
class TIdHttpResultBroadcasterForm : public TBroadcasterForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TMainMenu *MainMenu;
	TMenuItem *Connect1;
	TMenuItem *ConnectAll1;
	TMenuItem *DisconnectAll1;
	TMenuItem *Servers1;
	TMenuItem *Add1;
	TMenuItem *Del1;
	TPopupMenu *PopupMenu1;
	TMenuItem *HttpBroadcaster1;
	TCheckBox *BroadcastEnabledCheckBox;
	void __fastcall Add1Click(TObject *Sender);
	void __fastcall Del1Click(TObject *Sender);
	void __fastcall ConnectAll1Click(TObject *Sender);
	void __fastcall DisconnectAll1Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall HttpBroadcaster1Click(TObject *Sender);
	void __fastcall BroadcastEnabledCheckBoxClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TIdHttpResultBroadcasterForm(TComponent* Owner);


/// Функция добавления метаданных в очередь на отправку в соответствии с настройками
bool AddMetadata(int channel_index, long long time_stamp);

/// Инициирует процедуру отправки метаданных
bool SendMetadata(void);

// Вещатели
std::vector<TIdHttpResultBroadcasterFrame*> Broadcasters;

// Метод, вызываемый перед шагом расчета
void ABeforeCalculate(void);

// Обновляет интерфейс
void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Сохраняет параметры интерфейса в xml
void AASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
void AALoadParameters(RDK::USerStorageXML &xml);

// Создание копии этого компонента
TIdHttpResultBroadcasterForm* New(TComponent *owner);

// Число вещателей
int GetNumBroadcasters(void) const;

// Добавляет новый вещатель
void AddBroadcaster(void);

// Удаляет вещатель
void DelBroadcaster(int index);

// Удаляет все вещатели
void ClearBroadcasters(void);

// Возвращает индекс текущего активного (выбранного) вещателя
int GetActiveBroadcaster(void) const;

// Возвращает фрейм вещателя
TIdHttpResultBroadcasterFrame* GetBroadcasterFrame(int index);

// Возвращает фрейм активного (выбранного) вещателя
TIdHttpResultBroadcasterFrame* GetActiveBroadcasterFrame(void);

// Подключает вещатель, или все, если index == -1
void Connect(int index=-1);

// Отключает вещатель, или все, если index == -1
void Disconnect(int index=-1);

};
//---------------------------------------------------------------------------
extern PACKAGE TIdHttpResultBroadcasterForm *IdHttpResultBroadcasterForm;
//---------------------------------------------------------------------------
#endif
