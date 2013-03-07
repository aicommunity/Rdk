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
#include "UAGraphics.h"

namespace RDK {

/*
    Структура, описывающая объект UContainer как визуальный элемент
*/
struct UGEDescription
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

// Тип элемента
// 0 - не определено
// 1 - элемент без дочерних объектов
// 2 - элемент - сеть с дочерними объектами
int Type;

// Число входов и выходов
int NumInputs, NumOutputs;
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

// Цвет исходящих связей
UColorT LinkColor;

// Толщина контура связи
int LinkWidth;

// Флаг выделения компонента
bool Highlight;
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
class UDrawEngine
{
public: // Типы данных
typedef map<string,UGEDescription> DescriptionsTableT;
typedef map<string,UGEDescription>::iterator DescriptionsTableIteratorT;

typedef map<string,vector<DescriptionsTableIteratorT> > DescriptionsLinksTableT;
typedef map<string,vector<DescriptionsTableIteratorT> >::iterator DescriptionsLinksTableIteratorT;

protected: // Данные
// Указатель на сеть
//UEPtr<UNet> Net;
Serialize::USerStorageXML NetXml;

// Таблица соответствий между нейронами сети и описаний визуальных элементов
DescriptionsTableT Descriptions;


// Связи
DescriptionsLinksTableT Links;


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

// Коэффициент зуммирования
// Пиксельная координата X,Y=ZoomCoeff*Coord.X,Y
double ZoomCoeff;

// Начало координат в канве
MVector<double,3> Origin;
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
// ---------------------------

// ---------------------------
// Методы доступа к данным
// ---------------------------
// Возвращает указатель на НС
//UEPtr<UNet> GetNet(void);
const Serialize::USerStorageXML& GetNetXml(void) const;

// Возвращает элемент таблицы соответствий
UGEDescription& GetDescription(const string &name);

// Возвращает всю таблицу соответсвий
const UDrawEngine::DescriptionsTableT& GetDescriptions(void);

// Возвращает указатель на движок отображения
UEPtr<UAGraphics> GetGEngine(void);

// Связывает класс с новой НС
// Если net == 0 то отключает класс от текущей НС и возвращает true
//bool SetNet(UEPtr<UNet> net);
bool SetNetXml(Serialize::USerStorageXML &net_xml);

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


// Отрисовывает индикатор с заданными параметрами
// Направление direction задается следующим образом
// 1 - индикатор заполняется слева направо
// 2 - индикатор заполняется справа налево
// 3 - индикатор заполняется сверху вниз
// 4 - индикатор заполняется снизу вверх
void PaintIndicator(int x,int y, int width, int height,
                   double precent, int direction, UColorT full, UColorT empty);

// Отрисовывает окружность
// ---------------------------
};

}
#endif


