/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBITMAP_H
#define UBITMAP_H

#include "../../Deploy/Include/initdll_defs.h"

namespace RDK {

// Режим изображения
typedef enum {ubmUnknown=0, ubmY32=1, ubmF32=2, ubmRGB24=3, ubmRGB32=4, ubmRGB555=5,
              ubmRGB565=6, ubmRGB8=7, ubmRGB4=8, ubmRGB1=9, ubmRGB96=10, ubmYUY2=100,
              ubmYUYV=101, ubmYVU9=103, ubmYVYU=105,
              ubmUYVY=106, ubmY211=107, ubmY8=400,
              ubmYCrCb411=411, ubmYCrCb422=422, ubmYCrCb444=444,
              ubmHSI=500, ubmHSV=501} UBMColorModel;

// Направления сдвига
typedef enum {ubmUp=1, ubmDown=2, ubmLeft=3, ubmRight=4} UBMShiftDirection;

// Тип заливки
typedef enum {ubmFillNone=0, ubmFillImage=1, ubmFillColor=2} UBMFillType;

// Цветовые каналы
typedef enum {ubmY=0, ubmCr=1, ubmCb=2} UBMColorChannel;

// Тип цветового элемента данных
typedef unsigned char UBColor;

// RGB тип
struct RDK_LIB_TYPE UBRgb { UBColor r,g,b,d; }; // 32,24 бит RGB

// YCrCb тип
struct RDK_LIB_TYPE UBYCrCb { UBColor y,cr,cb; }; // YCrCb

// HSV тип
struct RDK_LIB_TYPE UBHsv { double h,s,b; }; // HSV

union RDK_LIB_TYPE UColorT
{
unsigned int c; // Целое, ч/б для вычислений формата 000C
float f; // Вещественное для вычислений
UBRgb rgb; // 32,24 бит RGB
UBYCrCb ycrcb; // YCrCb

UColorT(void);

UColorT(unsigned color);

UColorT(UBColor bb, UBColor gg, UBColor rr, UBColor dd);

UColorT(UBColor yy, UBColor crr, UBColor cbb);

UColorT operator = (const UColorT &color);

UColorT operator = (const unsigned color);

bool operator == (const UColorT color) const;

bool operator != (const UColorT color) const;

UBColor& operator [] (int i);
};

// Координаты пикселя изображения
// (Описание в конце файла)
struct RDK_LIB_TYPE UBPoint;

// Координаты и цвет пикселя изображения
// (Описание в конце файла)
struct RDK_LIB_TYPE UBColorPoint;

// Прямоугольный элемент изображения
// (Описание в конце файла)
struct RDK_LIB_TYPE UBRect;

// Элемент гистограммы
// (Описание в конце файла)
struct RDK_LIB_TYPE UBHistogramElement;

// Гистограмма
// (Описание в конце файла)
class RDK_LIB_TYPE UBHistogram;


class RDK_LIB_TYPE UBitmapParam
{
public:
// размеры изображения по осям
int Width, Height;

// Формат изображения
UBMColorModel ColorModel;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBitmapParam(void);
UBitmapParam(const UBitmapParam &bitmap);
UBitmapParam(UBMColorModel cmodel);
~UBitmapParam(void);
// --------------------------
};

// Класс, описывающий изображение
class RDK_LIB_TYPE UBitmap: protected UBitmapParam
{
protected: // Данные
UBColor *Data;

// Формат изображения
//UBMColorModel ColorModel;

// размеры изображения по осям
//int Width, Height;

// Число пикселей изображения
int Length;

// Число элементов UBColor изображения
int ByteLength;

// Число байт выделенной памяти
int MemoryLength;

// Смещения цветовых каналов
int ChannelOffset[4];

// Длина строки изображения в байтах
int LineByteLength;

// Длина пикселя изображения в байтах
int PixelByteLength;

// Смещение данных пикселя (канала) в битах для масштабирования
// в 1 байт на пиксель (канал).
// Применяется для форматов ubmY32, ubmRGB96
int Shift;

protected: // Данные обхода изображения
// Указатель на текущий элемент изображения
UBColor* PData;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBitmap(void);
UBitmap(const UBitmap &bitmap);
UBitmap(UBMColorModel cmodel);
UBitmap(int width, int height, UColorT color=0, UBMColorModel cmodel=ubmRGB24);
UBitmap(int width, int height, const UBColor* data, UBMColorModel cmodel);
~UBitmap(void);
// --------------------------

// -------------------------
// Методы управления данными
// -------------------------
// Возвращает указатель на внутренний буфер
inline UBColor* GetData(void) const
{ return Data; };

// Возвращает размер изображения по оси X
inline int GetWidth(void) const
{ return Width; };

// Возвращает размер изображения по оси Y
inline int GetHeight(void) const
{ return Height; };

inline int GetLength(void) const
{ return Length; };

// Число байт изображения
inline int GetByteLength(void) const
{ return ByteLength; };

// Смещение данных пикселя (канала) в битах для масштабирования
int GetShift(void) const;
bool SetShift(int value);

// Длина линии изображения в байтах
int GetLineByteLength(void) const;

// Длина пикселя в байтах
int GetPixelByteLength(void) const;

inline UBMColorModel GetColorModel(void) const
{ return ColorModel; };

// Смещения цветовых каналов
inline int GetChannelOffset(UBMColorChannel channel) const
{ return ChannelOffset[channel]; };

// Задает формат изображения
// Если isupdate == true то существующее изображение преобразуется
void SetColorModel(UBMColorModel cmodel, bool isupdate=true);

// Создает внутренний буфер
void SetRes(int width, int height, UBMColorModel cmodel=ubmUnknown);

// Копирует новое изображение из буфера data
// с прежними размерами
void SetImage(const UBColor* data);

// Копирует новое изображение из буфера data
// с новыми размерами
void SetImage(int width, int height, const UBColor* data, UBMColorModel cmodel);

// Устанавливает внутренний указатель на буфер data
// сохраняя прежнюю информацию о размерах
void AttachBuffer(UBColor* data);

// Устанавливает внутренний указатель на буфер data
// обновляя информацию о размерах
void AttachBuffer(int width, int height, UBColor* data, UBMColorModel cmodel);

// Отдает буфер изображения на внешнее управление
// После выполнения метода класс становится пустым
// Возвращает освобожденный буфер
UBColor* DetachBuffer(void);

// Очищает память изображения и обнуляет его размеры
void Clear(void);

// Заполняет изображение цветом color
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void Fill(UColorT color);
void Fill(UColorT color, const UBRect &rect);

// Преобразует это изображение в формат приемника 'target'
// и записывает результат в приемник.
void ConvertTo(UBitmap &target) const;

// Копирует изображение в 'target' в позицию,
// начинающуюся как x,y
// Если изображение не вмещается целиком, то оно усекается
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void CopyTo(int x, int y, UBitmap &target);

// Копирует изображение в 'target' в позицию,
// начинающуюся как x,y
// Если изображение не вмещается целиком, то оно усекается
// Изображение всегда преобразуется в цветовую модель цели 'target'
// Элементы изображения источника с цветом 'transp' не переносятся
// (эффект прозрачности)
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void CopyTo(int x, int y, UBitmap &target, UColorT transp);

// Копирует изображение в 'target' в позицию,
// начинающуюся как x,y
// Если изображение не вмещается целиком, то оно усекается
// Изображение всегда преобразуется в цветовую модель цели 'target'
// Элементы изображения источника с цветом 'transp' не переносятся
// (эффект прозрачности)
// Все остальные элементы заменяются в итоговом изображении на цвет 'color'
// Или переносятся как есть
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void MaskTo(int x, int y, UBitmap &target, UColorT transp, UColorT color);
void MaskTo(int x, int y, UBitmap &target, UColorT transp);

// Возвращает участок изображения с координатами
// левого верхнего угла x,y и шириной и длиной
// соответствующей размерам 'target'
// Возвращает false если при копировании произошло усечение
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
bool GetRect(int x, int y, UBitmap &target) const;

// Разделяет текущее изображение на цветовые каналы
// Каналы формируются в цветовой модели 'Y8' для исходного 'RGB24' и
// в 'Y32' для исходного 'RGB96'
// Поддерживает режимы ubmRGB24, ubmRGB96
void Separate(UBitmap* channels);

// Совмещает в себе каналы в одно цветное изображение
// Поддерживает режимы ubmRGB24, ubmRGB96
void Merge(UBitmap* channels);
// -------------------------

// -------------------------
// Методы сбора статистики
// -------------------------
// Возвращает минимальное и максимальное значение яркостей
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void FindColorRange(UColorT &minval, UColorT &maxval) const;

// Возвращает минимальное и максимальное значение яркостей и их координаты
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void FindColorRange(UBColorPoint &minval, UBColorPoint &maxval) const;

// Возвращает минимальное и максимальное значение яркостей участка изображения
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void FindColorRange(UBRect rect, UColorT &minval, UColorT &maxval) const;

// Вычисляет средневзвешенную сумму яркостей всех пикслей участка изображения
// Если 'width' или 'height' < 0 или превышает максимум
// то размеры участка вычисляются до конца изображения
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
UColorT FindAverageColor(int x=0, int y=0, int width=-1,int height=-1) const;

// Вычисляет суммарную яркость по строке изображения с номером y
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
unsigned int CalcBrightnessByRow(int y) const;

// Вычисляет суммарную яркость по столбцу изображения с номером x
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
unsigned int CalcBrightnessByCol(int x) const;

// Вычисляет суммарную яркость раздельно по столбцам и строками изображения от 
// столбца x1 до x2, и от строки y1 до y2. 
// В x_result значения столбцов, в y_result - строк, память должна быть выделена
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void CalcBrightness(unsigned *x_result, unsigned *y_result,
                                   int x1=-1, int x2=-1, int y1=-1, int y2=-1) const;

// Вычисляет относительную суммарную интенсивность раздельно по столбцам и
// строками изображения от столбца x1 до x2, и от строки y1 до y2.
// Интенсивность считается раздельно по каналам в зависимости от цветовой модели
// В x_result значения столбцов, в y_result - строк, память должна быть выделена
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void CalcBrightnessAverage(UColorT *x_result, UColorT *y_result,
                                   int x1=-1, int x2=-1, int y1=-1, int y2=-1) const;

// Подсчитывает число пикселей имеющих интенсивность выше порога UColorT
int CalcNumPixels(UColorT threshold) const;

// Вычисляет ожидаемый размер гистограммы, если это возможно
// и выделяет память на необходимое число элементов
// result - указатель на возвращаемый массив данных гистограммы
// Поддерживает режимы ubmY8, ubmRGB24
//void PrepareHistogram(UBHistogram &result,
//                    int x=0, int y=0, int width=-1, int height=-1) const;

// Вычисляет гистограмму яркостей по изображению в заданной области
// result - указатель на возвращаемый массив данных гистограммы
// Память должна быть выделена на заданное число элементов
// Гистограмма вычисляется для первых N заданных яркостей пикселей в массиве result
// для случая ubmY8, для заданных яркостей для случая ubmY32
// Поддерживает режимы ubmY8, ubmY32
//void CalcHistogram(UBHistogram &result,
//                    int x=0, int y=0, int width=-1, int height=-1) const;

// Поддерживает режимы ubmRGB24
//void CalcHistogram(UBHistogram &red, UBHistogram &green, UBHistogram &blue,
//                    int x=0, int y=0, int width=-1, int height=-1) const;
// -------------------------

// -------------------------
// Методы обработки изображения
// -------------------------
// Прорежение изображения
// Прорежает каждую n-ый столбец изображения по горизонтали
// и каждую m-ю строку по вертикали
// Если 'target' != 0 то результат операции сохраняется в него
void Reduce(int n, int m, UBitmap *target=0);

// Отражение по вертикали
// Если 'target' != 0 то результат операции сохраняется в него
// и цветовая модель 'target' замещается моделью источника
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void ReflectionX(UBitmap *target=0);

// Отражение по горизонтали
// Если 'target' != 0 то результат операции сохраняется в него
// и цветовая модель 'target' замещается моделью источника
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void ReflectionY(UBitmap *target=0);

// Сдвигает изображение в требуемую сторону на 'pixels' пикселей
// Направление определяется 'direction'
// 1 - Up
// 2 - Down
// 3 - Left
// 4 - Right
// При иных значениях 'direction' или отрицательных значениях pixels
// метод не делает ничего
// Пустое место заполняется цветом 'color'
// Если 'target' != 0 то результат операции сохраняется в него
// и цветовая модель 'target' замещается моделью источника
void Move(int pixels, int direction, UBMFillType filltype=ubmFillImage, UColorT color=0, UBitmap *target=0);

// Сдвигает изображение на 'x' пикселей по оси абсцисс и на 'y'
// пикселей по оси ординат
// положительные значения вызывают сдвиг вправо и вниз
// отрицательные - влево и вверх
// Пустое место заполняется цветом 'color'
// Если 'target' != 0 то результат операции сохраняется в него
// и цветовая модель 'target' замещается моделью источника
void MoveXY(int x, int y, UBMFillType filltype=ubmFillImage, UColorT color=0, UBitmap *target=0);

// Изменяет размер канвы, сохраняя нетронутым изображение
// Если изображение не помещается целиком в новую канву,
// то оно усекается
// Свободное место заполняется цветом 'color'
// Если 'target' != 0 то результат операции сохраняется в него
// и цветовая модель 'target' замещается моделью источника
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void ResizeCanvas(int top, int left, int right, int bottom, UColorT color=0, UBitmap *target=0);

// Вставляет горизонтальную полосу толщиной 'thickness'
// начиная с позиции с верхней y-координатой
// Полоса заполняется цветом 'color'
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void InsertHorLine(int y, int thickness, UColorT color=0, UBitmap *target=0);

// Удаляет горизонтальную полосу толщиной 'thickness'
// начиная с позиции с верхней y-координатой
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void RemoveHorLine(int y, int thickness, UBitmap *target=0);

// Вставляет вертикальную полосу толщиной 'thickness'
// начиная с позиции с левой x-координатой
// Полоса заполняется цветом 'color'
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void InsertVertLine(int x, int thickness, UColorT color=0, UBitmap *target=0);

// Удаляет вертикальную полосу толщиной 'thickness'
// начиная с позиции с левой x-координатой
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void RemoveVertLine(int x, int thickness, UBitmap *target=0);

// Контрастирование. Производит пересчёт значений яркости в масштаб 0 - 255.
// Запись результата производится в target
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void Contrasting(UBitmap *target=0);

// Контрастирование. Производит пересчёт значений яркости в масштаб minb - maxb.
// Если 'target' != 0 то результат операции сохраняется в него
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void Contrasting(UColorT minb, UColorT maxb, UBitmap *target=0);

// Контрастирование в заданной области. Производит пересчёт значений яркости в масштаб minb - maxb.
// Если 'target' != 0 то результат операции сохраняется в него
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void Contrasting(UBRect rect, UColorT minb, UColorT maxb, UColorT threshold, UBitmap *target=0);

// Производит бинаризацию изображения с порогом threshold
// Если 'target' != 0 то результат операции сохраняется в него
// Пиксели с яркостями ниже порога устанавливаются равными 'minval'
// Пиксели с яркостями выше порога устанавливаются равными 'maxval'
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void Binarization(UColorT threshold, UColorT minval, UColorT maxval, UBitmap *target=0);

// Производит автоматическую бинаризацию изображения по
// порогу, представляющему собой средневзвешенную яркость изображения
// Если 'target' != 0 то результат операции сохраняется в него
// Пиксели с яркостями ниже порога устанавливаются равными 'minval'
// Пиксели с яркостями выше порога устанавливаются равными 'maxval'
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void Binarization(UColorT minval, UColorT maxval, UBitmap *target=0);

// Инвертирует изображение
// Если 'target' != 0 то результат операции сохраняется в него
// Для всех цветовых моделей база устанавливается == 255
// т.о. модель ubmY32 необходим нормировать в [0:255] перед инверсией
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void Inverse(UBitmap *target=0);

/// Меняет местами RGB каналы из RGB в BGR и наоборот.
/// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void SwapRGBChannels(UBitmap *target=0);
// -------------------------

// -----------------------
// Операторы доступа
// -----------------------
// Оператор присваивания
// Цветовая модель приемника всегда замещается моделью источника
UBitmap& operator = (const UBitmap &bitmap);

// Возвращает значение элемента изображения подряд в массиве
inline UBColor& operator [] (int index) const
{ return *(Data+index); };

// Возвращает полное значение пикселя с линейной координатой
// имеет смысл для RGB, Y32 форматов
inline UColorT& operator () (int index) const
{ return reinterpret_cast<UColorT&>(*(Data+index*PixelByteLength)); };

// Возвращает ссылку на начало значения пикселя с 2D координатой
// имеет смысл для RGB, Y8, Y32 и других форматов, для которых
// цветовые данные пикселя храняться в единой области памяти
inline UBColor& operator () (int ix, int iy) const
{ return *(Data+iy*LineByteLength+ix*PixelByteLength); };

inline UBColor& operator () (int ix, int iy, int ch) const
{ return *(Data+iy*LineByteLength+ix*PixelByteLength+ch); };

// Возвращают значение цветовой составляющей пикселя с линейной координатой
// в пределах канала
inline UBColor& operator () (int index, UBMColorChannel channel) const
{ return *(Data+ChannelOffset[channel]+index); };


inline bool operator == (const UBitmap &value) const
{
 return false;
}

bool operator != (const UBitmap &value) const
{
 return true;
}
// -----------------------

// -----------------------
// Операторы обхода изображения
// -----------------------
// Возвращает текущий элемент изображения
inline UBColor& operator () (void) const
{ return *PData; };

inline UBColor& operator ++ (void)
{ return *(PData++); };

inline UBColor& operator ++ (int)
{ return *(++PData); };

inline UBColor& operator -- (void)
{ return *(PData--); };

inline UBColor& operator -- (int)
{ return *(--PData); };

inline UBColor& operator += (int i)
{ return *(PData+=i); };

inline UBColor& operator -= (int i)
{ return *(PData-=i); };

inline UBColor& operator = (int i)
{ return *(PData=Data+i); };
// -----------------------


protected: // Скрытые методы
// -----------------------
// Методы расчета параметров изображения
// -----------------------
// Вычисляет длину строки изображения в байтах
int CalcLineByteLength(int width, UBMColorModel cmodel) const;

// Возвращает длину буфера изображения в байтах, по заданным размерам и цветовой модели
int CalcByteLength(int width, int height, UBMColorModel cmodel) const;

// Расчитывает смещения каналов
void CalcChannelOffset(int width, int height, UBMColorModel cmodel, int *offset);

// Создает новый буфер с заданными параметрами и инициализирует все внутренние переменные
void CreateData(int width, int height, UBMColorModel cmodel);
// -----------------------

// -----------------------
// Методы преобразования цветовых моделей
// -----------------------
void ColorConvert(UBColor *source, UBMColorModel sourcecmodel,
                                    UBColor *dest, UBMColorModel destcmodel) const;

void ColorConvertRGB24_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertRGB24_F32(UBColor *source, UBColor *dest) const;

void ColorConvertRGB32_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertRGB32_F32(UBColor *source, UBColor *dest) const;

void ColorConvertY32_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertY32_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertY32_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertY32_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertY32_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertY32_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertY32_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertY32_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertY32_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertY32_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertY32_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertY32_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertY32_F32(UBColor *source, UBColor *dest) const;

void ColorConvertY8_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertY8_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertY8_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertY8_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertY8_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertY8_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertY8_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertY8_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertY8_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertY8_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertY8_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertY8_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertY8_F32(UBColor *source, UBColor *dest) const;

void ColorConvertYUYV_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertYUYV_F32(UBColor *source, UBColor *dest) const;

void ColorConvertYVYU_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertYVYU_F32(UBColor *source, UBColor *dest) const;

void ColorConvertUYVY_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertUYVY_F32(UBColor *source, UBColor *dest) const;

void ColorConvertYUY2_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertYUY2_F32(UBColor *source, UBColor *dest) const;

void ColorConvertHSI_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertHSI_F32(UBColor *source, UBColor *dest) const;

void ColorConvertHSV_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertHSV_F32(UBColor *source, UBColor *dest) const;

void ColorConvertYCrCb411_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb411_F32(UBColor *source, UBColor *dest) const;

void ColorConvertYCrCb422_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb422_F32(UBColor *source, UBColor *dest) const;

void ColorConvertYCrCb444_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertYCrCb444_F32(UBColor *source, UBColor *dest) const;

void ColorConvertF32_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertF32_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertF32_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertF32_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertF32_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertF32_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertF32_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertF32_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertF32_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertF32_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertF32_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertF32_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertF32_YCrCb444(UBColor *source, UBColor *dest) const;

void ColorConvertRGB96_Y32(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_RGB24(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_RGB32(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_Y8(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_YUYV(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_YVYU(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_UYVY(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_YUY2(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_HSI(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_HSV(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_YCrCb411(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_YCrCb422(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_YCrCb444(UBColor *source, UBColor *dest) const;
void ColorConvertRGB96_F32(UBColor *source, UBColor *dest) const;
// -----------------------
};


// Тип - указатель на изображение
typedef UBitmap* PUBitmap;

// Координаты пикселя изображения
struct RDK_LIB_TYPE UBPoint
{
int X,Y;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBPoint(void);
UBPoint(int x, int y);
~UBPoint(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Операторы сравнения
bool operator == (const UBPoint &value) const;
bool operator != (const UBPoint &value) const;
// --------------------------
};

// Координаты и цвет пикселя изображения
// (Описание в конце файла)
struct RDK_LIB_TYPE UBColorPoint: public UBPoint
{
// Цвет точки
UColorT Color;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBColorPoint(void);
~UBColorPoint(void);
// --------------------------
};

// Прямоугольный элемент изображения
struct RDK_LIB_TYPE UBRect: public UBPoint
{
// Координаты правого нижнего угла прямоугольника
//int X2,Y2;

// Размеры прямоугольника
int Width,Height;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBRect(void);
UBRect(int x, int y, int width, int height);
~UBRect(void);
// --------------------------

// ---------------------
// Методы доступа к данным
// ---------------------
int X1(void) const;
int Y1(void) const;
int X2(void) const;
int Y2(void) const;
void X2(int value);
void Y2(int value);
/*int Width(void) const;
int Height(void) const;
void Width(int value);
void Height(int value);*/
// ---------------------


// ---------------------
// Операторы
// ---------------------
bool operator == (const UBRect &value) const;
bool operator != (const UBRect &value) const;
// ---------------------
};

// Элемент гистограммы
struct RDK_LIB_TYPE UBHistogramElement
{
// Цвет изображения
UColorT Color;

union {
// Число элементов изображения заданного цвета
unsigned Int;
float Float;
} Number;

// Методы
// ---------------------
// Операторы
// ---------------------
bool operator < (const UBHistogramElement &value) const;
bool operator > (const UBHistogramElement &value) const;
// ---------------------
};

extern "C" {
// Функция сравнения двух элементов гистограммы
int UBHistogramElementCompare(const void *e1, const void *e2);
}

// Гистограмма
class RDK_LIB_TYPE UBHistogram
{
friend class UBitmap;
protected: // Параметры
// Размер гистограммы
int Size;

// Флаг нормировки гистограммы
// если true - то гистограмму следует рассматривать как нормированную в веществнных числах
bool NormalizeFlag;

protected: // Данные
// Данные гистограммы
mutable UBHistogramElement* Data;

// Минимум и максимум гистограммы
UBHistogramElement Min,Max;

// Число элементов по которым строилась гистограмма
// Минимальное возвращаемое значение = 1
int NumPixels;

// Реальный размер массива
int RealSize;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBHistogram(void);
~UBHistogram(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Размер гистограммы
int GetSize(void) const;
bool Resize(int value);

// Возвращает состояние нормировки гистограммы
// если true - то гистограмму следует рассматривать как нормированную в веществнных числах
bool IsNormalized(void) const;
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Данные гистограммы
UBHistogramElement* GetData(void) const;

// Реальный размер массива
int GetRealSize(void) const;

// Минимум и максимум гистограммы
UBHistogramElement GetMin(void) const;
bool SetMin(UBHistogramElement value);
UBHistogramElement GetMax(void) const;
bool SetMax(UBHistogramElement value);

// Число элементов по которым строилась гистограмма
// Минимальное возвращаемое значение = 1
int GetNumPixels(void) const;
bool SetNumPixels(int value);
// --------------------------

// --------------------------
// Методы счета
// --------------------------
// Создает данные гистограммы в соответствии с цветовой моделью
// Поддерживает режимы ubmY8, ubmRGB24, ubmRGB32
bool Prepare(UBMColorModel model);
bool Prepare(const UBitmap &bmp);

// Заполняет гистограмму заданным значением
void Fill(unsigned value);
void Fill(float value);

// Вычисление гистограммы по индексу цветового канала
// Поддерживает режимы ubmY8, ubmRGB24, ubmRGB32
void Calc(const UBitmap &bmp, int x, int y, int width, int height, int channel=0);
void Calc(const UBitmap &bmp, const UBitmap &mask, int x, int y, int width, int height, int channel=0);
void Calc(const UBitmap &bmp, int channel=0);
void Calc(const UBitmap &bmp, const UBitmap &mask, int channel=0);

// Вычисление оценки интегрального распределения по существующей гистограмме
// Автоматически вычисляет диапазон значений распределения
// Если текущая гистограмма - нормированная, то распределение вычисляется нормированным
void CalcCumulativeDistribution(UBHistogram &output);

// Меняет флаг нормировки и вычисляет значения гистограммы если 'process' == true
bool Normalize(bool value, bool process=false);

// Заполнение дырок в гистограмме интерполированными данными
// в заданном диапазоне
void CalcZeroSmoothHistogram(UBHistogram &result, UBColor min=0, UBColor max=255);

// Вычисление диапазона значений гистограммы
void CalcHistogramRange(void);

// Сортировка гистограммы по возрастанию значений Y
// Если output != 0, то записывает результат в него
void Sort(UBHistogram *output);
// --------------------------

// --------------------------
// Операторы
// --------------------------
UBHistogramElement& operator [] (int index) const;
UBHistogram& operator = (const UBHistogram &value);
friend UBHistogram operator - (const UBHistogram &value1, const UBHistogram &value2);
// --------------------------
};

}
#endif

