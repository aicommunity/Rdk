//---------------------------------------------------------------------------

#ifndef UComponentIOFrameUnitH
#define UComponentIOFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <string>
#include "TUVisualController.h"
//---------------------------------------------------------------------------
class TUComponentIOFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	void __fastcall FrameResize(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUComponentIOFrame(TComponent* Owner);

// -----------------
// Данные
// -----------------
// Указатель на рассматриваемую сеть
//NANet* Net;

// Путь до родителя наблюдаемого компонента
std::string ViewComponentOwnerLongId;

// Путь до наблюдаемого компонента
std::string ViewComponentLongId;

// Режим работы:
// 1 - показывать входы
// 2 - показывать выходы
// 3 - показывать и входы и выходы
// 4 - показывать существующие связи
int Mode;

// Модификатор режима показа
// 1 - показывать только входы (выходы, связи) своего уровня
// 2 - показывать входы (выходы, связи) своего уровня, и всех вложенных сетей
int ShowModifier;
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

// Заполняет таблицу входами и выходами сети
void __fastcall ShowInputsOutputs(void);

// Заполняет таблицу установленными связями
void __fastcall ShowLinks(void);

// Декодирует список свойств-входов/выходов в map
void DecodePropertiesIOList(const std::string &source, std::map<int, std::string> &result);
// -----------------

// -----------------
// Вспомогательные методы управления
// -----------------
protected:
// Отображает данные выходов в таблицу
void __fastcall ShowOutputs(TStringGrid *string_grid, RDK::ULongIdVector &linkslist);

// Отображает данные входов в таблицу
void __fastcall ShowInputs(TStringGrid *string_grid, RDK::ULongIdVector &linkslist);
// -----------------


// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
public:
// Метод, вызываемый перед шагом расчета
void ABeforeCalculate(void);

// Метод, вызываемый после шага расчета
void AAfterCalculate(void);

// Обновление интерфейса
void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentIOFrame *UComponentIOFrame;
//---------------------------------------------------------------------------
#endif
