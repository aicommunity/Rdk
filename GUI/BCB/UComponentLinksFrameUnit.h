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
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall HeaderControl1SectionClick(THeaderControl *HeaderControl, THeaderSection *Section);

private:	// User declarations
public:		// User declarations
	__fastcall TUComponentLinksFrame(TComponent* Owner);

protected:		// User declarations
// Длинный id наблюдаемого компонента модели
std::string ViewComponentLongId;

public:
// Доступ к id наблюдаемого компонента
const std::string& GetViewComponentLongId(void) const;
void SetViewComponentLongId(const std::string& value);

// Инициализация интерфейса перед первым отображением
void __fastcall Init(void);

// Обновление интерфейса
void UpdateInterface(void);

// Создает выбранную связь
void __fastcall CreateLink(void);

// Разрывает выбранную связь
void __fastcall BreakLink(void);

// Разрывает все связи
void __fastcall BreakAll(void);

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
