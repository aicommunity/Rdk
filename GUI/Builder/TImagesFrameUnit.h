//---------------------------------------------------------------------------

#ifndef TImagesFrameUnitH
#define TImagesFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include <vector>
//---------------------------------------------------------------------------
class TImagesFrame : public TFrame
{
__published:    // IDE-managed Components
    TDrawGrid *DrawGrid;
    void __fastcall DrawGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
private:    // User declarations
public:        // User declarations
    __fastcall TImagesFrame(TComponent* Owner);


// ћассив изображений
std::vector<std::vector<TImage*> > Images;

// ‘лаг отражени€ вокруг оси X изображений при выводе
bool ReflectionXFlag;

// --------------------------
// ћетоды управлени€ параметрами
// --------------------------
// ”станавливает размер €чейки
void SetCellRes(int width, int height);

// ”станавливает число €чеек
void SetNumCells(int width, int height);

// ¬озвращает число €чеек
int GetNumCellWidth(void);
int GetNumCellHeight(void);

// ‘лаг отражени€ вокруг оси X изображений при выводе
bool GetReflectionXFlag(void);
void SetReflectionXFlag(bool value);
// --------------------------

// --------------------------
// ћетоды управлени€ изображени€ми
// --------------------------
// ”станавливает заданное изображение в €чейку с координатами i,j
//bool SetBitmap(int i, int j, RDK::UBitmap &bitmap);
bool SetImage(int i, int j, const TImage *image);
bool SetImage(int i, int j, int width, int height, int colormodel, unsigned char *buffer);

// ”станавливает заданное изображение в текущую €чейку
//bool SetBitmap(RDK::UBitmap &bitmap);
bool SetImage(const TImage *image);
//bool SetImage(int width, int height, int colormodel, const unsigned char *buffer);

// ¬озвращает координаты текущей €чейки
int GetX(void);
int GetY(void);

// ¬озвращает изображение в выбранной €чейке
Graphics::TBitmap* GetImage(int i, int j);

// ¬озвращает изображение в текущей €чейке
Graphics::TBitmap* GetImage(void);
// --------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TImagesFrame *ImagesFrame;
//---------------------------------------------------------------------------
#endif
