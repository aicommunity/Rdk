/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UGRAPHICS_H
#define UGRAPHICS_H

#include "UAGraphics.h"

namespace RDK {

class UGraphics: public UAGraphics
{
protected: // Данные
// Канва рисования
UBitmap* Canvas;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UGraphics(void);
UGraphics(UBitmap *canvas);
virtual ~UGraphics(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает текущую канву рисования
UBitmap* GetCanvas(void);

// Задает канву рисования
bool SetCanvas(UBitmap *canvas);
// --------------------------

// --------------------------
// Перегруженные операторы
// --------------------------
// Оператор присваивания
UGraphics& operator = (UGraphics &tool);
// --------------------------

// --------------------------
// Графические примитивы
// --------------------------
// Отображает пиксель
// если ispos == true позиционирует перо в заданную позицию
void Pixel(int x, int y, bool ispos=true);

// Отображает линию по координатам концов
void Line(int x1, int y1, int x2, int y2);

// Отображает линию относительно позиции пера
// Перемещает перо в позицию x,y
void LineTo(int x, int y);

// Отображает окружность с центром x,y и радиусом r
// Если fill == true - то рисуем с заливкой
void Circle(int x, int y, int r, bool fill=false);

// Отображает круговой сектор с центром x,y и радиусом r
// Раствор сектора fi, повернут на угол teta 
// против часовой стрелки от горизонтальной оси
// углы задаются в градусах
void Sector(int x, int y, int r, float fi, float teta, bool fill=false);

// Отображает эллипс с центром x,y и радиусами hor, vert
// Если fill == true - то рисуем с заливкой
void Ellipse(int x, int y, int hor, int vert, bool fill=false);

// Отображает прямоугольник с координатами
// x1,y1 - верхнего левого угла
// x2,y2 - правого нижнего угла
// Если fill == true - то рисуем с заливкой
void Rect(int x1, int y1, int x2, int y2, bool fill=false);

// Отображает прямоугольник с координатами вершин
// с заливкой цветом color
// Если fill == true - то рисуем с заливкой
void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill=false);

// Простейшая заливка произвольной области
void Fill(int x, int y, UColorT BorderColor);

// Выводит изображение с началом в заданной позиции
// Если transparency == 0 копируется без прозрачности
// Если transparency == 1 копируется c эффектом прозрачности прозрачности
// Если transparency == 2 копируется как маска, с цветом данных как текущего пера
void Bitmap(int x, int y, UBitmap &bmp, int transparency=0, UColorT transp=UColorT(255,255,255));
// --------------------------

protected: // Вспомогательные методы
// --------------------------
// Вспомогательные графические примитивы
// --------------------------
// Отрисовывает пиксель в координатах x,y без проверок
void DrawPixel(int x, int y);
// --------------------------
};

}
#endif

