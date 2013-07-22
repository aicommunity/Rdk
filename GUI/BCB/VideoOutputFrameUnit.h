#ifndef RDK_VIDEO
#define RDK_VIDEO
#endif
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
#include "myrdk.h"
#include <CheckLst.hpp>
#include "VideoOutputToolsFormUnit.h"
#include "UComponentsListFormUnit.h"
#include <Menus.hpp>
#include "VidGrab.hpp"
#include "TUVisualController.h"
#include "TUVisualControllerFrameUnit.h"
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdCustomHTTPServer.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdHTTPServer.hpp>
#include "TUHttpServerUnit.h"

class TVideoGrabberControlForm;

//using namespace RDK::VCapture;
//---------------------------------------------------------------------------
class TVideoOutputFrame : public TUVisualControllerFrame
{
__published:    // IDE-managed Components
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
    TMenuItem *N3;
    TMenuItem *SendToComponent;
    TMenuItem *SendToComponentState1;
	TMenuItem *N4;
	TMenuItem *SendImageToComponentInput1;
	TMenuItem *SendImageToComponentOutput1;
	TPanel *Panel2;
	TEdit *SendToEdit;
	TButton *Button1;
	TButton *SelectSendToButton;
	TPopupMenu *SelectPopupMenu;
	TMenuItem *Parameter1;
	TMenuItem *State1;
	TMenuItem *SendImageToComponentProperty1;
	TPanel *Panel3;
	TCheckBox *ShowCentralPointCheckBox;
	TEdit *PointXEdit;
	TEdit *PointYEdit;
	TButton *SendAsMatrixButton;
	TMenuItem *PropertyMatrix1;
	TCheckBox *SendPointsByStepCheckBox;
	TCheckBox *DeletePointsAfterSendCheckBox;
	TUHttpServerFrame *UHttpServerFrame;
	TGroupBox *GroupBox1;
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
    void __fastcall SendToComponentClick(TObject *Sender);
    void __fastcall SendToComponentState1Click(TObject *Sender);
	void __fastcall SendImageToComponentOutput1Click(TObject *Sender);
	void __fastcall SendImageToComponentInput1Click(TObject *Sender);
	void __fastcall Parameter1Click(TObject *Sender);
	void __fastcall State1Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall SendImageToComponentProperty1Click(TObject *Sender);
	void __fastcall SendAsMatrixButtonClick(TObject *Sender);
	void __fastcall PropertyMatrix1Click(TObject *Sender);
	void __fastcall UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
          TIdHTTPResponseInfo *AResponseInfo);


private:    // User declarations
public:        // User declarations
    __fastcall TVideoOutputFrame(TComponent* Owner);
    __fastcall ~TVideoOutputFrame(void);

// Режим работы
// 0 - Bmp
// 1 - Avi
// 2 - Camera
// 3 - IP Camera
// 4 - Image sequence
// 5 - Http Server
int Mode;

Graphics::TBitmap* ConvertBitmap;

// Форма управления выводом видео
TVideoOutputToolsForm* MyVideoOutputToolsForm;

// Форма управления инициализацией видео
TVideoGrabberControlForm* MyVideoGrabberControlForm;

// Указатель на форму выбора компоненты-источника
TUComponentsListForm *MyComponentsListForm;

// Источник изображения
RDK::UBitmap BmpSource,ReflectedBmpSource;

// Канва рисования
RDK::UBitmap BmpCanvas;

// Модуль графики
RDK::UGraphics Graph;

// Отрисовка геометрии
RDK::MGraphics<double,2> GeometryGraphics;

// Текущая геометрия
RDK::MGraphics<double,2> CurrentGeometryGraphics;

// Массив изображений для режима последовательности картинок
std::vector<std::string> BmpSequenceNames;

// Путь до последовательности изображений
std::string BmpSequencePathName;

// Текущий кадр в последовательности картинок
int CurrentBmpSequenceIndex;

protected: // Привязка источника видео к входу-выходу компонента
// Имя компонента, к которому привязан источник
std::string LinkedComponentName;

// Режим привязки
// 0 - ко входу
// 1 - к выходу
// 2 - к свойству
int LinkedMode;

// Выход, к которому привязан источник
int LinkedIndex;

// Свойство, к которому привязан источник
std::string LinkedComponentPropertyName;


public:
// Временная метка сервера
long long ServerTimeStamp;

// Текущая создаваемая фигура
//RDK::MGeometry<double,2> Figure;

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

// Целевой параметр-приемник данных о геометрии
std::string SelectedComponentPName;
std::string SelectedComponentParameterName;

// Целевая переменная состояния-приемник данных о геометрии
std::string SelectedComponentSName;
std::string SelectedComponentStateName;

// Целевое свойство матрица-приемник данных о геометрии
std::string SelectedComponentPropertyMatrixName;
std::string SelectedComponentMatrixName;

int LastReadSequenceIndex;
// ============================================================


// Инициализация фрейма avi-файлом
void InitByAvi(const String &filename);

// Инициализация фрейма bmp-файлом
void InitByBmp(const String &filename);

// Устанавливает отдельное изображение
bool InitByBmp(const RDK::UBitmap &bmp);

// Инициализация фрейма камерой
void InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index);

// Инициализация фрейма IP-камерой
void InitByIPCamera(const String camera_url, const String user_name, const String user_password);

// Инициализация последовательностью изображений
bool InitByImageSequence(const String &pathname);

// Инициализация http-сервера
bool InitByHttpServer(int listen_port);

// Загружает выбранную картинку по индеку в массиве имен
bool LoadImageFromSequence(int index, RDK::UBitmap &bmp);

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
void SetSampleGeometryGraphics(RDK::MGraphics<double,2>& samplegraphics);

// -------------------------
// Методы ввода вывода точек геометрии из параметров и переменных состояния компонент
// -------------------------
// Отправляет набор точек в свойство компонента
void SendToComponentPropertyMatrix(const std::string &stringid, const std::string &parameter_name, int figure_index);

// Отправляет набор точек в параметр компонента
void SendToComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index);

// Отправляет набор точек в переменную состояния компонента
void SendToComponentState(const std::string &stringid, const std::string &state_name, int figure_index);

// Считывает набор точек из параметра компонента
void ReceiveFromComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index);

// Считывает набор точек из переменной состояния компонента
void ReceiveFromComponentState(const std::string &stringid, const std::string &state_name, int figure_index);
// -------------------------

// -------------------------
// Методы вывода изображений во входы-выходы компонент
// -------------------------
// Отправляет изображение в выбранный компонент
void SendToComponentIO(void);
// -------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
virtual void ABeforeCalculate(void);

// Обновление интерфейса
virtual void AUpdateInterface(void);

// Сохраняет параметры интерфейса в xml
virtual void ASaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void ALoadParameters(RDK::USerStorageXML &xml);
// -----------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputFrame *VideoOutputFrame;
//---------------------------------------------------------------------------
#endif
