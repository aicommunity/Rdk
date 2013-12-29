//---------------------------------------------------------------------------

#ifndef UImagesFrameUnitH
#define UImagesFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include <IniFiles.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include <vector>
#include "UComponentsListFormUnit.h"
#include "myrdk.h"
#include "TUVisualController.h"

#pragma warn -8130
class TUComponentsListForm;
//---------------------------------------------------------------------------
class TUImagesFrame : public TUVisualControllerFrame
{
__published:    // IDE-managed Components
    TDrawGrid *DrawGrid;
	TPopupMenu *PopupMenu;
	TMenuItem *SaveToBmp;
	TMenuItem *SaveToJpeg;
	TMenuItem *SaveAllToBmp;
	TMenuItem *SaveAllToJpeg;
	TMenuItem *N1;
	TMenuItem *SelectSource;
	TMenuItem *N2;
	TSavePictureDialog *SavePictureDialog;
	TMenuItem *N3;
	TMenuItem *AddColumn;
	TMenuItem *AddRow;
	TMenuItem *DeleteColumn;
	TMenuItem *DeleteRow;
	TImage *FullImage;
	TScrollBox *ScrollBox1;
	TPanel *Panel1;
	TCheckBox *ShowLegendCheckBox;
	TCheckBox *ShowHistogramCheckBox;
    void __fastcall DrawGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall SaveToBmpClick(TObject *Sender);
	void __fastcall SelectSourceClick(TObject *Sender);
	void __fastcall AddColumnClick(TObject *Sender);
	void __fastcall AddRowClick(TObject *Sender);
	void __fastcall DeleteColumnClick(TObject *Sender);
	void __fastcall DeleteRowClick(TObject *Sender);
	void __fastcall DrawGridDblClick(TObject *Sender);
	void __fastcall FullImageDblClick(TObject *Sender);
	void __fastcall FullImageMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall FullImageMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall FullImageMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall DrawGridClick(TObject *Sender);




private:    // User declarations
public:        // User declarations
	__fastcall TUImagesFrame(TComponent* Owner);
	virtual __fastcall ~TUImagesFrame(void);

bool presIm;
int x2, y2;

// Массив изображений
std::vector<std::vector<TImage*> > Images;

// Массив длинных id компонент, связанных с изображениями
std::vector<std::vector<std::string> > StringIds;

// Массив индексов выходов компонент, связанных с изображениями
std::vector<std::vector<std::string> > ComponentIndexes;

// Массив индексов выходов компонент, связанных с изображениями
std::vector<std::vector<int> > ComponentIndexesOld;

// Массив имен компонент и их свойств, связанных с кликом мыши
std::vector<std::vector<std::pair<std::string,std::string> > > MouseClickComponents;

// Флаг отражения вокруг оси X изображений при выводе
bool ReflectionXFlag;

// Указатель на форму выбора компоненты-источника
TUComponentsListForm *MyComponentsListForm;

// --------------------------
// Методы управления параметрами
// --------------------------
// Устанавливает размер ячейки
void SetCellRes(int width, int height);

// Устанавливает число ячеек
void SetNumCells(int width, int height);

// Возвращает число ячеек
int GetNumCellWidth(void);
int GetNumCellHeight(void);

// Флаг отражения вокруг оси X изображений при выводе
bool GetReflectionXFlag(void);
void SetReflectionXFlag(bool value);
// --------------------------

// --------------------------
// Методы управления изображениями
// --------------------------
// Связывает ячейку с идентификатором компонента
void LinkToComponent(int i, int j, const std::string &stringid, int index);

// Устанавливает заданное изображение в ячейку с координатами i,j
bool SetBitmap(int i, int j, const RDK::UBitmap &bitmap);
bool SetImage(int i, int j, const TImage *image);
bool SetImage(int i, int j, int width, int height, int colormodel, unsigned char *buffer);

// Устанавливает заданное изображение в текущую ячейку
//bool SetBitmap(RDK::UBitmap &bitmap);
bool SetImage(const TImage *image);
//bool SetImage(int width, int height, int colormodel, const unsigned char *buffer);

// Возвращает координаты текущей ячейки
int GetX(void);
int GetY(void);

// Возвращает изображение в выбранной ячейке
Graphics::TBitmap* GetImage(int i, int j);

// Возвращает изображение в текущей ячейке
Graphics::TBitmap* GetImage(void);
// --------------------------

// --------------------------
// Методы управления фреймом
// --------------------------
void ABeforeCalculate(void);
void AAfterCalculate(void);

void AUpdateInterface(void);

// Возврат интерфейса в исходное состояние
virtual void AClearInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// --------------------------

};
#pragma warn .8130
//---------------------------------------------------------------------------
extern PACKAGE TUImagesFrame *UImagesFrame;
//---------------------------------------------------------------------------
#endif
