//---------------------------------------------------------------------------

#ifndef UFavoriteComponentInfoFrameUnitH
#define UFavoriteComponentInfoFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.Grids.hpp>
#include <Vcl.Menus.hpp>

struct TUFavoriteData
{
public: // Данные
// Тип данных
// 0 - не определен
// 1 - значение параметра
// 2 - значение перменной состояния
// 3 - значение входа компонента
// 4 - значение выхода компонента
int Type;

// Режим выбора значения
// 0 - последнее значение
// 1 - среднее за интервал Time
// 2 - максимальное за интервал Time
// 3 - минимальное за интервал Time
// 4 - максимальное по модулю за интервал Time
// 5 - минимальное по модулю за интервал Time
// 6 - амплитуда за интервал Time
int Mode;

// Интервал времени в секундах для расчета
double Time;

// Длинное имя компонента
std::string ComponentName;

// Имя параметра/состояния и т.п.
std::string DataName;

// Считываемое значение
std::string Data;

// Понятное имя
std::string Header;

protected: // Временные переменные
// Массив значений за заданный интервал времени
std::vector<std::pair<double,double> > History;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
TUFavoriteData(void);
TUFavoriteData(const TUFavoriteData &copy);
~TUFavoriteData(void);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Считывает значение в соответствии с типом и режимом и сохраняет его в переменной Data
void ReadData(void);
// --------------------------

};

//---------------------------------------------------------------------------
class TUFavoriteComponentInfoFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid;
	TPopupMenu *PopupMenu;
	TMenuItem *Add1;
	TMenuItem *Delete1;
	TMenuItem *Moveup1;
	TMenuItem *MoveDown1;
	TMenuItem *Rename1;
	void __fastcall Add1Click(TObject *Sender);
	void __fastcall Delete1Click(TObject *Sender);
	void __fastcall MoveDown1Click(TObject *Sender);
	void __fastcall Moveup1Click(TObject *Sender);
	void __fastcall Rename1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUFavoriteComponentInfoFrame(TComponent* Owner);

// Строки данных
std::vector<TUFavoriteData> Info;

// Обновляет интерфейс
void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
void ASaveParameters(RDK::Serialize::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
void ALoadParameters(RDK::Serialize::USerStorageXML &xml);

};
//---------------------------------------------------------------------------
extern PACKAGE TUFavoriteComponentInfoFrame *UFavoriteComponentInfoFrame;
//---------------------------------------------------------------------------
#endif
