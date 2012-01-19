//---------------------------------------------------------------------------

#ifndef VideoOutputFrameUnitH
#define VideoOutputFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include "rdk_builder.h"
#include <CheckLst.hpp>
#include "VideoOutputToolsFormUnit.h"
#include <Menus.hpp>

//using namespace RDK::VCapture;
using namespace RDK;
//---------------------------------------------------------------------------
class TVideoOutputFrame : public TFrame
{
__published:    // IDE-managed Components
    TGroupBox *GroupBox;
    TImage *Image;
    TPanel *Panel1;
    TTrackBar *ImageTrackBar;
    TButton *StopButton;
    TButton *StartButton;
    TTimer *Timer;
    TMaskEdit *TimeEdit;
    TPopupMenu *PopupMenu;
    TMenuItem *N1;
    void __fastcall ImageTrackBarChange(TObject *Sender);
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall StartButtonClick(TObject *Sender);
    void __fastcall StopButtonClick(TObject *Sender);
    void __fastcall ImageMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall ImageMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
    void __fastcall ImageMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall TimeEditChange(TObject *Sender);
    void __fastcall N1Click(TObject *Sender);

private:    // User declarations
public:        // User declarations
    __fastcall TVideoOutputFrame(TComponent* Owner);
    __fastcall ~TVideoOutputFrame(void);

// Форма управления выводом видео
TVideoOutputToolsForm* MyVideoOutputToolsForm;

// Источник видео
//VACapture *Capture;

// Источник изображения
UBitmap BmpSource;

// Канва рисования
UBitmap BmpCanvas;

// Модуль графики
RDK::UGraphics Graph;

// Отрисовка геометрии
RDK::MGraphics<double> GeometryGraphics;

protected:
// Образец формируемой геометрии
RDK::MGraphics<double> SampleGeometryGraphics;

public:
// Текущая создаваемая фигура
RDK::MGeometry<double> Figure;

// Индекс текущей редактируемой фигуры
size_t FigureIndex;

// Индекс текущей редактируемой точки
size_t PointIndex;

// Флаг создания фигуры
bool FigureFlag;

// Флаг создания точки
// 0 - выключено
// 1 - создание новой точки
// 2 - редактирование существующей
int PointFlag;

// Флаг разрешения выделения зоны
bool ZoneSelectEnable;

// Событие, устанавливаемое при окончании выделения зоны
HANDLE ZoneSelectEvent;

// ============================================================
// Временные переменные
// ============================================================
// Координаты окошка выделения зоны интереса
int x1b, y1b, x2b, y2b;

// Коррекционные координаты окошка выделения зоны интереса
int corrx1b, corry1b, corrx2b, corry2b;

// Конечные координаты окошка выделения зоны интереса
double left,top,width,height;

// Цвет выделения
TColor SelColor;

bool CorrSelectFlag;

// Флаг обновления отрисовки
bool UpdateFlag;
// ============================================================


// Устанавливает новый источник видео
// Если capture == 0, то отменяет текущий источник
bool SetCapture(VACapture *capture);

// Устанавливает отдельное изображение
bool SetBitmap(const UBitmap &bmp);

// Устанавливает название окна
bool SetTitle(String title);

// Устанавливает координаты отображаемой зоны
// Если все координаты == -1 то зона не отображается
bool SetFrameRect(int x,int y, int x_width, int y_height, TColor color=(TColor)0x00FF0000);

// Обновляет отрисовку окна
bool UpdateVideo(void);

// Отрисовываем текущее состояние видеопотока
void DrawCapture(VACapture *capture, Graphics::TBitmap *bmp);

// Обновляем список точек
void UpdateGeometryList(TCheckListBox *GeometryCheckListBox, TCheckListBox *PointsCheckListBox);

// Метод отрисовки прямоугольной зоны
void __fastcall TVideoOutputFrame::DrawFrameRect(TImage *image, int x1, int y1, int x2,
                                                int y2, int framewidth, TColor color);

// Добавляет очередной элемент фигуры
void AddFigureRect(double l,double t,double w,double h);

// Устанавливает образец графики
void SetSampleGeometryGraphics(RDK::MGraphics<double>& samplegraphics);

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputFrame *VideoOutputFrame;
//---------------------------------------------------------------------------
#endif
