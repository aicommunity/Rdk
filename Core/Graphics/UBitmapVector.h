/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBITMAP_VECTOR_H
#define UBITMAP_VECTOR_H

#include "UBitmap.h"

namespace RDK {

// ���������� ������ ���������� �� �����������
class UPBitmapVector
{
protected: // ���������
// ������ ������� �����������
int Size;

protected: // ������
// ������ ���������� �� �����������
mutable PUBitmap* Buffer;

// �������� ������ �������
int RealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UPBitmapVector(void);
UPBitmapVector(const UPBitmapVector &copy);
virtual ~UPBitmapVector(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���������� ������ �������
int GetSize(void) const;

// �������� ������ ������� � ����������� ������� ������
virtual void Resize(int newsize);

// ������� ������
virtual void Clear(void);

// ���������� ��������� �� ������ ������
PUBitmap* GetBuffer(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UPBitmapVector& operator = (const UPBitmapVector &copy);

// �������� �������
PUBitmap& operator [] (int index);
const PUBitmap& operator [] (int index) const;
UBitmap& operator () (int index);
const UBitmap& operator () (int index) const;
// --------------------------
};

// ���������� ������ �����������
class UBitmapVector: public UPBitmapVector
{
protected: // ���������

protected: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBitmapVector(void);
UBitmapVector(const UBitmapVector &copy);
virtual ~UBitmapVector(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// �������� ������ ������� � ����������� ������� ������
virtual void Resize(int newsize);

// ������� ������
virtual void Clear(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UBitmapVector& operator = (const UBitmapVector &copy);

// �������� �������
PUBitmap operator [] (int index);
const PUBitmap operator [] (int index) const;
UBitmap& operator () (int index);
const UBitmap& operator () (int index) const;
// --------------------------
};


}
#endif


