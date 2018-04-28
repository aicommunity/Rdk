//---------------------------------------------------------------------------

#ifndef UComponentLinksFrameUnitH
#define UComponentLinksFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "UComponentIOFrameUnit.h"
#include <Vcl.ComCtrls.hpp>
#include "TUVisualController.h"
#include "TUVisualControllerFrameUnit.h"
//---------------------------------------------------------------------------
class TUComponentLinksFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TUComponentIOFrame *NANetFrameInputs;
	TGroupBox *GroupBox2;
	TUComponentIOFrame *NANetFrameOutputs;
	TGroupBox *GroupBox3;
	TUComponentIOFrame *NANetFrameLinks;
	TPanel *Panel1;
	TSplitter *Splitter1;
	THeaderControl *HeaderControl1;
	TPanel *ButtonPanel;
	TButton *OkButton;
	TButton *CancelButton;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall HeaderControl1SectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall OkButtonClick(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TUComponentLinksFrame(TComponent* Owner);

protected:		// User declarations
// Длинный id наблюдаемого компонента модели
//std::string ViewComponentLongId;

// Режим работы фрейма
// 0 - Обычный режим с панелью
// 1 - Модальный режим создания связи с кнопками
// 2 - Модальный режим разрыва связи с кнопками
// 3 - Модальный режим перемещения связей с кнопками
int Mode;


public:
// Доступ к id наблюдаемого компонента
//const std::string& GetViewComponentLongId(void) const;
//void SetViewComponentLongId(const std::string& value);

// Инициализация интерфейса перед отображением
void __fastcall Init(int mode, const std::string &component_name1, const std::string &owner_component_name, const std::string &component_name2="");

// Выбор режима отображения
void SetMode(int mode);

// Обновление интерфейса
void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Создает выбранную связь
void __fastcall CreateLink(void);

// Разрывает выбранную связь
void __fastcall BreakLink(void);

// Разрывает все связи
void __fastcall BreakAll(void);

// Перемещает все связи с выхода первого компонента на выход второго
void __fastcall SwitchLink(void);

// Связывает все объекты по цепочке в порядке возрастания id
// Предварительно разрывает все существующие связи
void __fastcall ChainLinking(void);

// Связывает все объекты параллельно с выходами модели
// Предварительно разрывает все существующие связи
void __fastcall ParallelLinking(void);


};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentLinksFrame *UComponentLinksFrame;
//---------------------------------------------------------------------------
#endif
