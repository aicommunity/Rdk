/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAGRAPHICS_H
#define UAGRAPHICS_H

#include <string>
#include "UBitmap.h"
#include "UFont.h"

namespace RDK {

using namespace std;

//class UAFont;

enum {alLeft=1, alRight=2, alCenter=3, alJustify=4};

class RDK_LIB_TYPE UAGraphics
{
protected: // Данные
// Шрифт
UAFont *Font;

protected: // Атрибуты инструментов рисования
// Цвет пера
UColorT PenColor;

// Толщина пера
int PenWidth;

// Координаты пера
int PenX, PenY;

protected: // Временные переменные
// Прямой указатель на данные канвы рисования
UBColor *CData;

// Разрешение канвы рисования
int CWidth, CHeight;

// Половина толщины пера
int HalfPenWidth;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAGraphics(void);
virtual ~UAGraphics(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Разрешение канвы рисования
int GetCWidth(void) const;
int GetCHeight(void) const;

// Устанавливает шрифт
bool SetFont(UAFont* font);

// Возвращает текущий шрифт
UAFont* GetFont(void);
// --------------------------

// --------------------------
// Методы доступа к атрибутам инструментов рисования
// --------------------------
// Возвращает цвет пера
const UColorT& GetPenColor(void) const;

// Возвращает толщину пера
int GetPenWidth(void) const;

// Возвращает X координату пера
int GetPenX(void) const;

// Возвращает Y координату пера
int GetPenY(void) const;

// Устанавливает цвет пера
void SetPenColor(const UColorT &color);

// Устанавливает толщину пера
bool SetPenWidth(int width);

// Позиционирует перо в точку x,y
void SetPenPos(int x, int y);
// --------------------------

// --------------------------
// Графические примитивы
// --------------------------
// Отображает пиксель
// если ispos == true позиционирует перо в заданную позицию
virtual void Pixel(int x, int y, bool ispos=true)=0;

// Отображает линию по координатам концов
virtual void Line(int x1, int y1, int x2, int y2)=0;

// Отображает линию относительно позиции пера
// Перемещает перо в позицию x,y
virtual void LineTo(int x, int y)=0;

// Отображает окружность с центром x,y и радиусом r
// Если fill == true - то рисуем с заливкой
virtual void Circle(int x, int y, int r, bool fill=false)=0;

// Отображает круговой сектор с центром x,y и радиусом r
// Раствор сектора fi, повернут на угол teta
// против часовой стрелки от горизонтальной оси
// углы задаются в градусах
virtual void Sector(int x, int y, int r, float fi, float teta, bool fill=false)=0;

// Отображает эллипс с центром x,y и радиусами hor, vert
// Если fill == true - то рисуем с заливкой
virtual void Ellipse(int x, int y, int hor, int vert, bool fill=false)=0;

// Отображает прямоугольник с координатами
// x1,y1 - верхнего левого угла
// x2,y2 - правого нижнего угла
// Если fill == true - то рисуем с заливкой
virtual void Rect(int x1, int y1, int x2, int y2, bool fill=false)=0;

// Отображает прямоугольник с координатами вершин
// с заливкой цветом color
// Если fill == true - то рисуем с заливкой
virtual void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill=false)=0;

// Простейшая заливка произвольной области
virtual void Fill(int x, int y, UColorT BorderColor)=0;

// Выводит изображение с началом в заданной позиции
// Если transparency == 0 копируется без прозрачности
// Если transparency == 1 копируется c эффектом прозрачности
// Если transparency == 2 копируется как маска, с цветом данных как текущего пера
virtual void Bitmap(int x, int y, UBitmap &bmp, int transparency=0, UColorT transp=UColorT(255,255,255))=0;
// --------------------------

// --------------------------
// Вывод текста
// --------------------------
// Вычисление длины и высоты строки текста
virtual void CalcTextSize(const wstring &str, int &width, int &height)=0;
virtual void CalcTextSize(const string &str, int &width, int &height)=0;

// Вычисление, сколько символов строки, начиная с символа index, войдет по
// ширине в заданное число пикселей
virtual int CalcTextLength(const string &str, int index, int width)=0;
virtual int CalcTextLength(const wstring &str, int index, int width)=0;

// Выводит текст str
virtual void Text(const wstring &str, int x, int y)=0;
virtual void Text(const string &str, int x, int y)=0;
virtual void TextRect(const wstring &str, const UBRect &rect, int align=alLeft)=0;
virtual void TextRect(const string &str, const UBRect &rect, int align=alLeft)=0;
// --------------------------

protected: // Вспомогательные методы
// --------------------------
// Вспомогательные графические примитивы
// --------------------------
// Отрисовывает пиксель в координатах x,y без проверок
virtual void DrawPixel(int x, int y)=0;
// --------------------------
};

}
#endif

