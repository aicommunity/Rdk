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

// ����� �����������
typedef enum {ubmUnknown=0, ubmY32=1, ubmF32=2, ubmRGB24=3, ubmRGB32=4, ubmRGB555=5,
              ubmRGB565=6, ubmRGB8=7, ubmRGB4=8, ubmRGB1=9, ubmRGB96=10, ubmYUY2=100,
              ubmYUYV=101, ubmYVU9=103, ubmYVYU=105,
              ubmUYVY=106, ubmY211=107, ubmY8=400,
              ubmYCrCb411=411, ubmYCrCb422=422, ubmYCrCb444=444,
              ubmHSI=500, ubmHSV=501} UBMColorModel;

// ����������� ������
typedef enum {ubmUp=1, ubmDown=2, ubmLeft=3, ubmRight=4} UBMShiftDirection;

// ��� �������
typedef enum {ubmFillNone=0, ubmFillImage=1, ubmFillColor=2} UBMFillType;

// �������� ������
typedef enum {ubmY=0, ubmCr=1, ubmCb=2} UBMColorChannel;

// ��� ��������� �������� ������
typedef unsigned char UBColor;

// RGB ���
struct RDK_LIB_TYPE UBRgb { UBColor r,g,b,d; }; // 32,24 ��� RGB

// YCrCb ���
struct RDK_LIB_TYPE UBYCrCb { UBColor y,cr,cb; }; // YCrCb

// HSV ���
struct RDK_LIB_TYPE UBHsv { double h,s,b; }; // HSV

union RDK_LIB_TYPE UColorT
{
unsigned int c; // �����, �/� ��� ���������� ������� 000C
float f; // ������������ ��� ����������
UBRgb rgb; // 32,24 ��� RGB
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

// ���������� ������� �����������
// (�������� � ����� �����)
struct RDK_LIB_TYPE UBPoint;

// ���������� � ���� ������� �����������
// (�������� � ����� �����)
struct RDK_LIB_TYPE UBColorPoint;

// ������������� ������� �����������
// (�������� � ����� �����)
struct RDK_LIB_TYPE UBRect;

// ������� �����������
// (�������� � ����� �����)
struct RDK_LIB_TYPE UBHistogramElement;

// �����������
// (�������� � ����� �����)
class RDK_LIB_TYPE UBHistogram;


class RDK_LIB_TYPE UBitmapParam
{
public:
// ������� ����������� �� ����
int Width, Height;

// ������ �����������
UBMColorModel ColorModel;

// --------------------------
// ������������ � �����������
// --------------------------
UBitmapParam(void);
UBitmapParam(const UBitmapParam &bitmap);
UBitmapParam(UBMColorModel cmodel);
~UBitmapParam(void);
// --------------------------
};

// �����, ����������� �����������
class RDK_LIB_TYPE UBitmap: protected UBitmapParam
{
protected: // ������
UBColor *Data;

// ������ �����������
//UBMColorModel ColorModel;

// ������� ����������� �� ����
//int Width, Height;

// ����� �������� �����������
int Length;

// ����� ��������� UBColor �����������
int ByteLength;

// ����� ���� ���������� ������
int MemoryLength;

// �������� �������� �������
int ChannelOffset[4];

// ����� ������ ����������� � ������
int LineByteLength;

// ����� ������� ����������� � ������
int PixelByteLength;

// �������� ������ ������� (������) � ����� ��� ���������������
// � 1 ���� �� ������� (�����).
// ����������� ��� �������� ubmY32, ubmRGB96
int Shift;

protected: // ������ ������ �����������
// ��������� �� ������� ������� �����������
UBColor* PData;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBitmap(void);
UBitmap(const UBitmap &bitmap);
UBitmap(UBMColorModel cmodel);
UBitmap(int width, int height, UColorT color=0, UBMColorModel cmodel=ubmRGB24);
UBitmap(int width, int height, const UBColor* data, UBMColorModel cmodel);
~UBitmap(void);
// --------------------------

// -------------------------
// ������ ���������� �������
// -------------------------
// ���������� ��������� �� ���������� �����
inline UBColor* GetData(void) const
{ return Data; };

// ���������� ������ ����������� �� ��� X
inline int GetWidth(void) const
{ return Width; };

// ���������� ������ ����������� �� ��� Y
inline int GetHeight(void) const
{ return Height; };

inline int GetLength(void) const
{ return Length; };

// ����� ���� �����������
inline int GetByteLength(void) const
{ return ByteLength; };

// �������� ������ ������� (������) � ����� ��� ���������������
int GetShift(void) const;
bool SetShift(int value);

// ����� ����� ����������� � ������
int GetLineByteLength(void) const;

// ����� ������� � ������
int GetPixelByteLength(void) const;

inline UBMColorModel GetColorModel(void) const
{ return ColorModel; };

// �������� �������� �������
inline int GetChannelOffset(UBMColorChannel channel) const
{ return ChannelOffset[channel]; };

// ������ ������ �����������
// ���� isupdate == true �� ������������ ����������� �������������
void SetColorModel(UBMColorModel cmodel, bool isupdate=true);

// ������� ���������� �����
void SetRes(int width, int height, UBMColorModel cmodel=ubmUnknown);

// �������� ����� ����������� �� ������ data
// � �������� ���������
void SetImage(const UBColor* data);

// �������� ����� ����������� �� ������ data
// � ������ ���������
void SetImage(int width, int height, const UBColor* data, UBMColorModel cmodel);

// ������������� ���������� ��������� �� ����� data
// �������� ������� ���������� � ��������
void AttachBuffer(UBColor* data);

// ������������� ���������� ��������� �� ����� data
// �������� ���������� � ��������
void AttachBuffer(int width, int height, UBColor* data, UBMColorModel cmodel);

// ������ ����� ����������� �� ������� ����������
// ����� ���������� ������ ����� ���������� ������
// ���������� ������������� �����
UBColor* DetachBuffer(void);

// ������� ������ ����������� � �������� ��� �������
void Clear(void);

// ��������� ����������� ������ color
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void Fill(UColorT color);
void Fill(UColorT color, const UBRect &rect);

// ����������� ��� ����������� � ������ ��������� 'target'
// � ���������� ��������� � ��������.
void ConvertTo(UBitmap &target) const;

// �������� ����������� � 'target' � �������,
// ������������ ��� x,y
// ���� ����������� �� ��������� �������, �� ��� ���������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void CopyTo(int x, int y, UBitmap &target);

// �������� ����������� � 'target' � �������,
// ������������ ��� x,y
// ���� ����������� �� ��������� �������, �� ��� ���������
// ����������� ������ ������������� � �������� ������ ���� 'target'
// �������� ����������� ��������� � ������ 'transp' �� �����������
// (������ ������������)
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void CopyTo(int x, int y, UBitmap &target, UColorT transp);

// �������� ����������� � 'target' � �������,
// ������������ ��� x,y
// ���� ����������� �� ��������� �������, �� ��� ���������
// ����������� ������ ������������� � �������� ������ ���� 'target'
// �������� ����������� ��������� � ������ 'transp' �� �����������
// (������ ������������)
// ��� ��������� �������� ���������� � �������� ����������� �� ���� 'color'
// ��� ����������� ��� ����
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void MaskTo(int x, int y, UBitmap &target, UColorT transp, UColorT color);
void MaskTo(int x, int y, UBitmap &target, UColorT transp);

// ���������� ������� ����������� � ������������
// ������ �������� ���� x,y � ������� � ������
// ��������������� �������� 'target'
// ���������� false ���� ��� ����������� ��������� ��������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
bool GetRect(int x, int y, UBitmap &target) const;

// ��������� ������� ����������� �� �������� ������
// ������ ����������� � �������� ������ 'Y8' ��� ��������� 'RGB24' �
// � 'Y32' ��� ��������� 'RGB96'
// ������������ ������ ubmRGB24, ubmRGB96
void Separate(UBitmap* channels);

// ��������� � ���� ������ � ���� ������� �����������
// ������������ ������ ubmRGB24, ubmRGB96
void Merge(UBitmap* channels);
// -------------------------

// -------------------------
// ������ ����� ����������
// -------------------------
// ���������� ����������� � ������������ �������� ��������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void FindColorRange(UColorT &minval, UColorT &maxval) const;

// ���������� ����������� � ������������ �������� �������� � �� ����������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void FindColorRange(UBColorPoint &minval, UBColorPoint &maxval) const;

// ���������� ����������� � ������������ �������� �������� ������� �����������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void FindColorRange(UBRect rect, UColorT &minval, UColorT &maxval) const;

// ��������� ���������������� ����� �������� ���� ������� ������� �����������
// ���� 'width' ��� 'height' < 0 ��� ��������� ��������
// �� ������� ������� ����������� �� ����� �����������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
UColorT FindAverageColor(int x=0, int y=0, int width=-1,int height=-1) const;

// ��������� ��������� ������� �� ������ ����������� � ������� y
// ������������ ������ ubmY8, ubmRGB24, ubmY32
unsigned int CalcBrightnessByRow(int y) const;

// ��������� ��������� ������� �� ������� ����������� � ������� x
// ������������ ������ ubmY8, ubmRGB24, ubmY32
unsigned int CalcBrightnessByCol(int x) const;

// ��������� ��������� ������� ��������� �� �������� � �������� ����������� �� 
// ������� x1 �� x2, � �� ������ y1 �� y2. 
// � x_result �������� ��������, � y_result - �����, ������ ������ ���� ��������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void CalcBrightness(unsigned *x_result, unsigned *y_result,
                                   int x1=-1, int x2=-1, int y1=-1, int y2=-1) const;

// ��������� ������������� ��������� ������������� ��������� �� �������� �
// �������� ����������� �� ������� x1 �� x2, � �� ������ y1 �� y2.
// ������������� ��������� ��������� �� ������� � ����������� �� �������� ������
// � x_result �������� ��������, � y_result - �����, ������ ������ ���� ��������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void CalcBrightnessAverage(UColorT *x_result, UColorT *y_result,
                                   int x1=-1, int x2=-1, int y1=-1, int y2=-1) const;

// ������������ ����� �������� ������� ������������� ���� ������ UColorT
int CalcNumPixels(UColorT threshold) const;

// ��������� ��������� ������ �����������, ���� ��� ��������
// � �������� ������ �� ����������� ����� ���������
// result - ��������� �� ������������ ������ ������ �����������
// ������������ ������ ubmY8, ubmRGB24
//void PrepareHistogram(UBHistogram &result,
//                    int x=0, int y=0, int width=-1, int height=-1) const;

// ��������� ����������� �������� �� ����������� � �������� �������
// result - ��������� �� ������������ ������ ������ �����������
// ������ ������ ���� �������� �� �������� ����� ���������
// ����������� ����������� ��� ������ N �������� �������� �������� � ������� result
// ��� ������ ubmY8, ��� �������� �������� ��� ������ ubmY32
// ������������ ������ ubmY8, ubmY32
//void CalcHistogram(UBHistogram &result,
//                    int x=0, int y=0, int width=-1, int height=-1) const;

// ������������ ������ ubmRGB24
//void CalcHistogram(UBHistogram &red, UBHistogram &green, UBHistogram &blue,
//                    int x=0, int y=0, int width=-1, int height=-1) const;
// -------------------------

// -------------------------
// ������ ��������� �����������
// -------------------------
// ���������� �����������
// ��������� ������ n-�� ������� ����������� �� �����������
// � ������ m-� ������ �� ���������
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
void Reduce(int n, int m, UBitmap *target=0);

// ��������� �� ���������
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// � �������� ������ 'target' ���������� ������� ���������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void ReflectionX(UBitmap *target=0);

// ��������� �� �����������
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// � �������� ������ 'target' ���������� ������� ���������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void ReflectionY(UBitmap *target=0);

// �������� ����������� � ��������� ������� �� 'pixels' ��������
// ����������� ������������ 'direction'
// 1 - Up
// 2 - Down
// 3 - Left
// 4 - Right
// ��� ���� ��������� 'direction' ��� ������������� ��������� pixels
// ����� �� ������ ������
// ������ ����� ����������� ������ 'color'
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// � �������� ������ 'target' ���������� ������� ���������
void Move(int pixels, int direction, UBMFillType filltype=ubmFillImage, UColorT color=0, UBitmap *target=0);

// �������� ����������� �� 'x' �������� �� ��� ������� � �� 'y'
// �������� �� ��� �������
// ������������� �������� �������� ����� ������ � ����
// ������������� - ����� � �����
// ������ ����� ����������� ������ 'color'
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// � �������� ������ 'target' ���������� ������� ���������
void MoveXY(int x, int y, UBMFillType filltype=ubmFillImage, UColorT color=0, UBitmap *target=0);

// �������� ������ �����, �������� ���������� �����������
// ���� ����������� �� ���������� ������� � ����� �����,
// �� ��� ���������
// ��������� ����� ����������� ������ 'color'
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// � �������� ������ 'target' ���������� ������� ���������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void ResizeCanvas(int top, int left, int right, int bottom, UColorT color=0, UBitmap *target=0);

// ��������� �������������� ������ �������� 'thickness'
// ������� � ������� � ������� y-�����������
// ������ ����������� ������ 'color'
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void InsertHorLine(int y, int thickness, UColorT color=0, UBitmap *target=0);

// ������� �������������� ������ �������� 'thickness'
// ������� � ������� � ������� y-�����������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void RemoveHorLine(int y, int thickness, UBitmap *target=0);

// ��������� ������������ ������ �������� 'thickness'
// ������� � ������� � ����� x-�����������
// ������ ����������� ������ 'color'
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void InsertVertLine(int x, int thickness, UColorT color=0, UBitmap *target=0);

// ������� ������������ ������ �������� 'thickness'
// ������� � ������� � ����� x-�����������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void RemoveVertLine(int x, int thickness, UBitmap *target=0);

// ����������������. ���������� �������� �������� ������� � ������� 0 - 255.
// ������ ���������� ������������ � target
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void Contrasting(UBitmap *target=0);

// ����������������. ���������� �������� �������� ������� � ������� minb - maxb.
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void Contrasting(UColorT minb, UColorT maxb, UBitmap *target=0);

// ���������������� � �������� �������. ���������� �������� �������� ������� � ������� minb - maxb.
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void Contrasting(UBRect rect, UColorT minb, UColorT maxb, UColorT threshold, UBitmap *target=0);

// ���������� ����������� ����������� � ������� threshold
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// ������� � ��������� ���� ������ ��������������� ������� 'minval'
// ������� � ��������� ���� ������ ��������������� ������� 'maxval'
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void Binarization(UColorT threshold, UColorT minval, UColorT maxval, UBitmap *target=0);

// ���������� �������������� ����������� ����������� ��
// ������, ��������������� ����� ���������������� ������� �����������
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// ������� � ��������� ���� ������ ��������������� ������� 'minval'
// ������� � ��������� ���� ������ ��������������� ������� 'maxval'
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void Binarization(UColorT minval, UColorT maxval, UBitmap *target=0);

// ����������� �����������
// ���� 'target' != 0 �� ��������� �������� ����������� � ����
// ��� ���� �������� ������� ���� ��������������� == 255
// �.�. ������ ubmY32 ��������� ����������� � [0:255] ����� ���������
// ������������ ������ ubmY8, ubmRGB24, ubmY32
void Inverse(UBitmap *target=0);

/// ������ ������� RGB ������ �� RGB � BGR � ��������.
/// ������������ ������ ubmY8, ubmRGB24, ubmY32
void SwapRGBChannels(UBitmap *target=0);
// -------------------------

// -----------------------
// ��������� �������
// -----------------------
// �������� ������������
// �������� ������ ��������� ������ ���������� ������� ���������
UBitmap& operator = (const UBitmap &bitmap);

// ���������� �������� �������� ����������� ������ � �������
inline UBColor& operator [] (int index) const
{ return *(Data+index); };

// ���������� ������ �������� ������� � �������� �����������
// ����� ����� ��� RGB, Y32 ��������
inline UColorT& operator () (int index) const
{ return reinterpret_cast<UColorT&>(*(Data+index*PixelByteLength)); };

// ���������� ������ �� ������ �������� ������� � 2D �����������
// ����� ����� ��� RGB, Y8, Y32 � ������ ��������, ��� �������
// �������� ������ ������� ��������� � ������ ������� ������
inline UBColor& operator () (int ix, int iy) const
{ return *(Data+iy*LineByteLength+ix*PixelByteLength); };

inline UBColor& operator () (int ix, int iy, int ch) const
{ return *(Data+iy*LineByteLength+ix*PixelByteLength+ch); };

// ���������� �������� �������� ������������ ������� � �������� �����������
// � �������� ������
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
// ��������� ������ �����������
// -----------------------
// ���������� ������� ������� �����������
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


protected: // ������� ������
// -----------------------
// ������ ������� ���������� �����������
// -----------------------
// ��������� ����� ������ ����������� � ������
int CalcLineByteLength(int width, UBMColorModel cmodel) const;

// ���������� ����� ������ ����������� � ������, �� �������� �������� � �������� ������
int CalcByteLength(int width, int height, UBMColorModel cmodel) const;

// ����������� �������� �������
void CalcChannelOffset(int width, int height, UBMColorModel cmodel, int *offset);

// ������� ����� ����� � ��������� ����������� � �������������� ��� ���������� ����������
void CreateData(int width, int height, UBMColorModel cmodel);
// -----------------------

// -----------------------
// ������ �������������� �������� �������
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


// ��� - ��������� �� �����������
typedef UBitmap* PUBitmap;

// ���������� ������� �����������
struct RDK_LIB_TYPE UBPoint
{
int X,Y;

// --------------------------
// ������������ � �����������
// --------------------------
UBPoint(void);
UBPoint(int x, int y);
~UBPoint(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
// ��������� ���������
bool operator == (const UBPoint &value) const;
bool operator != (const UBPoint &value) const;
// --------------------------
};

// ���������� � ���� ������� �����������
// (�������� � ����� �����)
struct RDK_LIB_TYPE UBColorPoint: public UBPoint
{
// ���� �����
UColorT Color;

// --------------------------
// ������������ � �����������
// --------------------------
UBColorPoint(void);
~UBColorPoint(void);
// --------------------------
};

// ������������� ������� �����������
struct RDK_LIB_TYPE UBRect: public UBPoint
{
// ���������� ������� ������� ���� ��������������
//int X2,Y2;

// ������� ��������������
int Width,Height;

// --------------------------
// ������������ � �����������
// --------------------------
UBRect(void);
UBRect(int x, int y, int width, int height);
~UBRect(void);
// --------------------------

// ---------------------
// ������ ������� � ������
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
// ���������
// ---------------------
bool operator == (const UBRect &value) const;
bool operator != (const UBRect &value) const;
// ---------------------
};

// ������� �����������
struct RDK_LIB_TYPE UBHistogramElement
{
// ���� �����������
UColorT Color;

union {
// ����� ��������� ����������� ��������� �����
unsigned Int;
float Float;
} Number;

// ������
// ---------------------
// ���������
// ---------------------
bool operator < (const UBHistogramElement &value) const;
bool operator > (const UBHistogramElement &value) const;
// ---------------------
};

extern "C" {
// ������� ��������� ���� ��������� �����������
int UBHistogramElementCompare(const void *e1, const void *e2);
}

// �����������
class RDK_LIB_TYPE UBHistogram
{
friend class UBitmap;
protected: // ���������
// ������ �����������
int Size;

// ���� ���������� �����������
// ���� true - �� ����������� ������� ������������� ��� ������������� � ����������� ������
bool NormalizeFlag;

protected: // ������
// ������ �����������
mutable UBHistogramElement* Data;

// ������� � �������� �����������
UBHistogramElement Min,Max;

// ����� ��������� �� ������� ��������� �����������
// ����������� ������������ �������� = 1
int NumPixels;

// �������� ������ �������
int RealSize;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBHistogram(void);
~UBHistogram(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ������ �����������
int GetSize(void) const;
bool Resize(int value);

// ���������� ��������� ���������� �����������
// ���� true - �� ����������� ������� ������������� ��� ������������� � ����������� ������
bool IsNormalized(void) const;
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ������ �����������
UBHistogramElement* GetData(void) const;

// �������� ������ �������
int GetRealSize(void) const;

// ������� � �������� �����������
UBHistogramElement GetMin(void) const;
bool SetMin(UBHistogramElement value);
UBHistogramElement GetMax(void) const;
bool SetMax(UBHistogramElement value);

// ����� ��������� �� ������� ��������� �����������
// ����������� ������������ �������� = 1
int GetNumPixels(void) const;
bool SetNumPixels(int value);
// --------------------------

// --------------------------
// ������ �����
// --------------------------
// ������� ������ ����������� � ������������ � �������� �������
// ������������ ������ ubmY8, ubmRGB24, ubmRGB32
bool Prepare(UBMColorModel model);
bool Prepare(const UBitmap &bmp);

// ��������� ����������� �������� ���������
void Fill(unsigned value);
void Fill(float value);

// ���������� ����������� �� ������� ��������� ������
// ������������ ������ ubmY8, ubmRGB24, ubmRGB32
void Calc(const UBitmap &bmp, int x, int y, int width, int height, int channel=0);
void Calc(const UBitmap &bmp, const UBitmap &mask, int x, int y, int width, int height, int channel=0);
void Calc(const UBitmap &bmp, int channel=0);
void Calc(const UBitmap &bmp, const UBitmap &mask, int channel=0);

// ���������� ������ ������������� ������������� �� ������������ �����������
// ������������� ��������� �������� �������� �������������
// ���� ������� ����������� - �������������, �� ������������� ����������� �������������
void CalcCumulativeDistribution(UBHistogram &output);

// ������ ���� ���������� � ��������� �������� ����������� ���� 'process' == true
bool Normalize(bool value, bool process=false);

// ���������� ����� � ����������� ������������������ �������
// � �������� ���������
void CalcZeroSmoothHistogram(UBHistogram &result, UBColor min=0, UBColor max=255);

// ���������� ��������� �������� �����������
void CalcHistogramRange(void);

// ���������� ����������� �� ����������� �������� Y
// ���� output != 0, �� ���������� ��������� � ����
void Sort(UBHistogram *output);
// --------------------------

// --------------------------
// ���������
// --------------------------
UBHistogramElement& operator [] (int index) const;
UBHistogram& operator = (const UBHistogram &value);
friend UBHistogram operator - (const UBHistogram &value1, const UBHistogram &value2);
// --------------------------
};

}
#endif

