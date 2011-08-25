//---------------------------------------------------------------------------

#ifndef UANetFrameUnitH
#define UANetFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>

#include "../../Kernel/NBios.h"

using namespace NMSDK;
//---------------------------------------------------------------------------
class TNANetFrame : public TFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	void __fastcall FrameResize(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TNANetFrame(TComponent* Owner);

// -----------------
// Данные
// -----------------
// Указатель на рассматриваемую сеть
NANet* Net;

// Режим работы:
// 1 - показывать входы
// 2 - показывать выходы
// 4 - показывать существующие связи
int Mode;

// Модификатор режима показа
// 1 - показывать только входы (выходы, связи) своего уровня
// 2 - показывать входы (выходы, связи) своего уровня, и всех вложенных сетей
int ShowModifier;

// Флаг обновления интерфейса
bool UpdateInterfaceFlag;
// -----------------

// -----------------
// Методы управления
// -----------------
// Очищает таблицу
void __fastcall Clear(void);

// Заполняет таблицу входами сети
void __fastcall ShowInputs(void);

// Заполняет таблицу выходами сети
void __fastcall ShowOutputs(void);

// Заполняет таблицу установленными связями
void __fastcall ShowLinks(void);

// Отрисовывает текущее состояние фрейма
void __fastcall UpdateInterface(void);
// -----------------
};
//---------------------------------------------------------------------------
extern PACKAGE TNANetFrame *NANetFrame;
//---------------------------------------------------------------------------
#endif
