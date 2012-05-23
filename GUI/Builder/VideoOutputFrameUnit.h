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
#include "VidGrab.hpp"

class TVideoGrabberControlForm;

//using namespace RDK::VCapture;
//---------------------------------------------------------------------------
class TVideoOutputFrame : public TFrame
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox;
	TImage *Image;
	TPanel *Panel1;
	TButton *StopButton;
	TButton *StartButton;
	TTimer *Timer;
	TMaskEdit *TimeEdit;
	TPopupMenu *PopupMenu;
	TMenuItem *N1;
	TVideoGrabber *VideoGrabber;
	TTrackBar *TrackBar;
	TMenuItem *SourceControl1;
	TMenuItem *N2;
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
	void __fastcall VideoGrabberFrameCaptureCompleted(TObject *Sender, Pointer FrameBitmap,
          int BitmapWidth, int BitmapHeight, DWORD FrameNumber, __int64 FrameTime,
          TFrameCaptureDest DestType, UnicodeString FileName, bool Success,
          int FrameId);
	void __fastcall TrackBarChange(TObject *Sender);
	void __fastcall SourceControl1Click(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TVideoOutputFrame(TComponent* Owner);
	__fastcall ~TVideoOutputFrame(void);

// Режим работы
// 0 - Bmp
// 1 - Avi
// 2 - Camera
int Mode;

Graphics::TBitmap* ConvertBitmap;

// Форма управления выводом видео
TVideoOutputToolsForm* MyVideoOutputToolsForm;

// Форма управления инициализацией видео
TVideoGrabberControlForm* MyVideoGrabberControlForm;

// Источник изображения
RDK::UBitmap BmpSource;

// Канва рисования
RDK::UBitmap BmpCanvas;

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
int FigureIndex;

// Индекс текущей редактируемой точки
int PointIndex;

// Флаг создания фигуры
//bool FigureFlag;

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

// Текущий обрабатываемый кадр
long CurrentFrameNumber;
// ============================================================
// Инициализация фрейма avi-файлом
void InitByAvi(const String &filename);

// Инициализация фрейма bmp-файлом
void InitByBmp(const String &filename);

// Устанавливает отдельное изображение
bool InitByBmp(const RDK::UBitmap &bmp);

// Инициализация фрейма камерой
void InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index);

// Устанавливает название окна
bool SetTitle(String title);

// Устанавливает координаты отображаемой зоны
// Если все координаты == -1 то зона не отображается
bool SetFrameRect(int x,int y, int x_width, int y_height, TColor color=(TColor)0x00FF0000);

// Обновляет отрисовку окна
bool UpdateVideo(void);

// Отрисовываем текущее состояние видеопотока
void DrawCapture(Graphics::TBitmap *bmp);

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
