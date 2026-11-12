// ===========================================================
// Version:        2.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: RDK

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UDRAW_ENGINE_GRAPHICS_H
#define UDRAW_ENGINE_GRAPHICS_H

//#include "../Engine/UNet.h"
#include "../Serialize/USerStorageXML.h"
#include "../Utilities/USupport.h"
#include "../Engine/UEPtr.h"
#include "../Math/MVector.h"
#include "UAGraphics.h"

namespace RDK {

/*
    Структура, описывающая порт компонента (вход или выход)
    Используется для визуализации точек подключения на компонентах
*/
struct RDK_LIB_TYPE UGEPort
{
    /// Имя порта (может быть алиас или имя свойства)
    std::string Name;

    /// Полный путь к свойству (ComponentPath.PropertyName)
    std::string FullPath;

    /// true = вход, false = выход
    bool IsInput;

    /// Есть ли активное подключение к этому порту
    bool IsConnected;

    /// Вертикальное смещение порта относительно верхней границы компонента
    int YOffset;

    /// Цвет порта (может зависеть от типа данных)
    UColorT Color;

    /// Тип свойства (ptPubInput, ptPubOutput и т.д.)
    unsigned int PropertyType;

    // Конструктор по умолчанию
    UGEPort()
        : IsInput(true), IsConnected(false), YOffset(0),
          Color(0, 0, 0), PropertyType(0)
    {}

    // Конструктор с параметрами
    UGEPort(const std::string& name, const std::string& fullPath, bool isInput,
            bool isConnected = false, unsigned int propType = 0)
        : Name(name), FullPath(fullPath), IsInput(isInput),
          IsConnected(isConnected), YOffset(0), Color(0, 0, 0),
          PropertyType(propType)
    {}
};

/*
    Структура, описывающая объект UContainer как визуальный элемент
*/
struct RDK_LIB_TYPE UGEDescription
{
// ------------------------
// Идентификационные параметры
// ------------------------
// Одномерный индекс i
int Index;

// 3D индекс
MVector<double, 3> Position;

// Понятное имя элемента
std::string Header;

// Класс элемента
std::string ClassName;

// Тип элемента
// 0 - не определено
// 1 - элемент без дочерних объектов
// 2 - элемент - сеть с дочерними объектами
int Type;

// Число входов и выходов
int NumInputs, NumOutputs;

// Состояние флага Activity
bool Activity;
// ------------------------

// ------------------------
// Геометрические параметры
// ------------------------
// Размеры элемента в пикселях
int Width, Height;

// Цвет контура элемента
UColorT ContourColor;

// Толщина контура элемента в пикселях
int ContourWidth;

// Цвет заливки элемента
UColorT FillColor;

// Цвет заливки неактивного элемента
UColorT InactiveFillColor;

// Цвет исходящих связей
UColorT LinkColor;

// Цвет текста неактивного элемента
UColorT InactiveFontColor;

// Толщина контура связи
int LinkWidth;

// Флаг выделения компонента
bool Highlight;
// ------------------------

// ------------------------
// Порты компонента
// ------------------------
// Входные порты (отображаются слева)
std::vector<UGEPort> InputPorts;

// Выходные порты (отображаются справа)
std::vector<UGEPort> OutputPorts;

// Флаг включения отображения портов
bool ShowPorts;

// Высота одного порта в пикселях
int PortHeight;

// Радиус круга порта в пикселях
int PortRadius;
// ------------------------

// ------------------------
// Индикаторы
// ------------------------
// Заполнение индикаторов в %
// (если -1, то отключен)
std::vector<double> Indicators;
// ------------------------

// ------------------------
// Конструкторы и деструкторы
// ------------------------
UGEDescription(void);
UGEDescription(const UGEDescription &copy);
~UGEDescription(void);
// ------------------------

UGEDescription& operator = (const UGEDescription &copy);
};

/*
    Класс графического отображения НС
*/
class RDK_LIB_TYPE UDrawEngine
{
public: // Типы данных
typedef map<string,UGEDescription> DescriptionsTableT;
typedef map<string,UGEDescription>::iterator DescriptionsTableIteratorT;

typedef map<string,vector<DescriptionsTableIteratorT> > DescriptionsLinksTableT;
typedef map<string,vector<DescriptionsTableIteratorT> >::iterator DescriptionsLinksTableIteratorT;

protected: // Данные
// Указатель на сеть
//UEPtr<UNet> Net;
USerStorageXML NetXml;

// Таблица соответствий между нейронами сети и описаний визуальных элементов
DescriptionsTableT Descriptions;


// Связи
DescriptionsLinksTableT Links;

/// Шрифты
RDK::UBitmapFontCollection Fonts;

// Движок для отображения сети
UEPtr<UAGraphics> GEngine;

protected: // Параметры
// ---------------------------
// Настройки отображения
// ---------------------------
// Реальная ширина и высота изображения
int CanvasWidth, CanvasHeight;

// Размеры элемента в пикселях по умолчанию
int RectWidth, RectHeight;

// Число элементов в строке
int ElementsXRes;

// Число строк элементов
int ElementsYRes;

// Цвет пустых индикаторов
std::vector<UColorT> EmptyIndicatorColors;

// Цвет полных индикаторов
std::vector<UColorT> FullIndicatorColors;

// Цвет фона
UColorT BackgroundColor;

// Цвет линий фона
UColorT BackgroundLineColor;

// Шаг сетки фона
int BackgroundLineStep;

// Цвет выделяемого элемента
UColorT SelectedColor;

// Цвет неактивного элемента
UColorT InactiveSelectedColor;

// Цвет входящих линий
UColorT InputLinksColor;

// Цвет исходящих линий
UColorT OutputLinksColor;

// Цвет линий по умолчанию
UColorT DefaultLinksColor;

// Коэффициент зуммирования
// Пиксельная координата X,Y=ZoomCoeff*Coord.X,Y
double ZoomCoeff;

// Начало координат в канве
MVector<double,3> Origin;

/// Флаг включения показа линий сетки
bool ShowBackgroundLines;
// ---------------------------


public: // Методы
// ---------------------------
// Конструкторы и деструкторы
// ---------------------------
UDrawEngine(void);
~UDrawEngine(void);
// ---------------------------

// ---------------------------
// Методы управления параметрами
// ---------------------------
// Коэффициент зуммирования
// Пиксельная координата X,Y=ZoomCoeff*Coord.X,Y
double GetZoomCoeff(void) const;

// Начало координат в канве
MVector<double,3> GetOrigin(void) const;

// Размеры элемента в пикселях по умолчанию
int GetRectWidth(void) const;
bool SetRectWidth(int value);
int GetRectHeight(void) const;
bool SetRectHeight(int value);

bool SetShowBackgroundLines(bool value);
bool GetShowBackgroundLines(void) const;
// ---------------------------

// ---------------------------
// Методы доступа к данным
// ---------------------------
// Возвращает указатель на НС
//UEPtr<UNet> GetNet(void);
const USerStorageXML& GetNetXml(void) const;

// Возвращает элемент таблицы соответствий
UGEDescription& GetDescription(const string &name);

// Возвращает всю таблицу соответсвий
const UDrawEngine::DescriptionsTableT& GetDescriptions(void);

// Возвращает указатель на движок отображения
UEPtr<UAGraphics> GetGEngine(void);

// Связывает класс с новой НС
// Если net == 0 то отключает класс от текущей НС и возвращает true
//bool SetNet(UEPtr<UNet> net);
bool SetNetXml(USerStorageXML &net_xml);

// Связывает класс с новой НС
// Если engine == 0 то возвращает false и не делеает ничего
bool SetEngine(UEPtr<UAGraphics> engine);

// Обновляет таблицу соответствий
void UpdateDescriptions(void);

// Выделяет заданный компонент, сбрасывая флаг выделения с остальных
void SelectSingleComponent(const string &name);

// Поиск объекта по заданным координатам в изображении
std::string FindComponent(int x, int y);

// Перемещает компонента в заданные координаты, с некоторой зоной нечуствительности
void MoveComponent(const string &name, int x, int y);

// Обновляет размеры всех элементов в соответствии с размерами по умолчанию
void UpdateAllElementsSize(void);

/// Возвращает рекомендуемый размер канвы
void CalcRecommendSize(int &width, int &height);

/// Шрифты
RDK::UBitmapFontCollection& GetFonts(void);
void SetFonts(RDK::UBitmapFontCollection& value);
// ---------------------------

// ---------------------------
// Методы размещения элементов по графическому полю
// ---------------------------
// Метод автоматического распределения существующих элементов по полю
void BuildDestinations(void);

// Метод автоматического распределения нераспределенных элементов
// (нераспределенные элементы имеют координаты -1,-1)
void UpdateDestinations(void);

// Устанавливает координаты размещения элемента id
// Возвращает false если элемент не найден или координаты некорректны
// Метод не проверяет свободна-ли занимаемая область
bool SetDestination(const string &name, int x, int y);

// Заполняет массивы связей компонент
void ParseLinks(void);
// ---------------------------

// ---------------------------
// Основные методы отображения
// ---------------------------
// Метод перерисовки всех элементов
void Draw(void);

// Метод обновления изображения отдельного элемента сети
// Возвращает false если элемент не найден
bool Draw(const string &name);

// Метод отображения связей сети
// links==true - отображает схему связей линиями
// highlight==true - подсвечивает все компоненты связанные с этим
// (сам нейрон подсвечивается в случае, если на него есть обратные связи)
// contour==true - окружает контуром компоненты связанные с этим
// (в контур могут попасть и не связанные компоненты)
bool DrawLinks(bool links=true, bool highlight=false, bool contour=false);

// Метод отображения фона
bool DrawBackground(void);
// ---------------------------

// ---------------------------
// Вспомогательные методы отображения
// ---------------------------
// Отрисовывает элемент в заданной позиции
void Paint(UGEDescription &ndescr);

// Отрисовывает конечный элемент (NAItem*) с центром в заданной позиции
void PaintItem(UGEDescription &ndescr);

// Отрисовывает элемент-сеть (UNet*) с центром в заданной позиции
void PaintNet(UGEDescription &ndescr);

// Отрисовывает связь
void PaintLink(UGEDescription &out, UGEDescription &in,
               bool links=true, bool highlight=true, bool contour=true);

// Отрисовывает связь между двумя портами
void PaintLinkBetweenPorts(int x1, int y1, int x2, int y2, int lineWidth);


// Отрисовывает индикатор с заданными параметрами
// Направление direction задается следующим образом
// 1 - индикатор заполняется слева направо
// 2 - индикатор заполняется справа налево
// 3 - индикатор заполняется сверху вниз
// 4 - индикатор заполняется снизу вверх
void PaintIndicator(int x,int y, int width, int height,
                   double precent, int direction, UColorT full, UColorT empty);

// Отрисовывает порты компонента
void PaintPorts(UGEDescription &ndescr);

// Отрисовывает один порт
void PaintPort(const UGEPort &port, int x, int y, int radius, bool isInput);

// Вычисляет позиции портов для компонента
void CalcPortPositions(UGEDescription &ndescr);

// Поиск порта по заданным координатам
// Возвращает указатель на порт или nullptr если не найден
// out_component_name заполняется именем компонента-владельца порта
UGEPort* FindPortAtPosition(int x, int y, std::string& out_component_name);

// Возвращает координаты центра порта
void GetPortCenter(const UGEDescription &ndescr, const UGEPort &port, int &x, int &y);
// ---------------------------
};

}
#endif


