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
#include <vector>
#include "myrdk.h"
//---------------------------------------------------------------------------
class TUImagesFrame : public TFrame
{
__published:    // IDE-managed Components
    TDrawGrid *DrawGrid;
    void __fastcall DrawGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);

private:    // User declarations
public:        // User declarations
    __fastcall TUImagesFrame(TComponent* Owner);

// Массив изображений
std::vector<std::vector<TImage*> > Images;

// Массив длинных id компонент, связанных с изображениями
std::vector<std::vector<std::string> > StringIds;

// Массив индексов выходов компонент, связанных с изображениями
std::vector<std::vector<int> > ComponentIndexes;

// Флаг отражения вокруг оси X изображений при выводе
bool ReflectionXFlag;

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
void UpdateInterface(void);

// Сохраняет информацию об источниках данных в заданный ini файл
void SaveToIni(TMemIniFile *ini, const String &section);

// Загружает информацию об источниках данных из заданного ini файла
void LoadFromIni(TMemIniFile *ini, const String &section);
// --------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TUImagesFrame *UImagesFrame;
//---------------------------------------------------------------------------
#endif
