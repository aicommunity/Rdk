/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBITMAP_VECTOR_CPP
#define UBITMAP_VECTOR_CPP

#include "UBitmapVector.h"

namespace RDK {


// ������
// --------------------------
// ������������ � �����������
// --------------------------
UPBitmapVector::UPBitmapVector(void)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;
}

UPBitmapVector::UPBitmapVector(const UPBitmapVector &copy)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;

 *this=copy;
}

UPBitmapVector::~UPBitmapVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���������� ������ �������
int UPBitmapVector::GetSize(void) const
{
 return Size;
}

// �������� ������ ������� � ����������� ������� ������
void UPBitmapVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  PUBitmap *newbuffer=new PUBitmap[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  for(int i=Size;i<newsize;i++)
   newbuffer[i]=0;

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ������� ������
void UPBitmapVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// ���������� ��������� �� ������ ������
PUBitmap* UPBitmapVector::GetBuffer(void)
{
 return Buffer;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UPBitmapVector& UPBitmapVector::operator = (const UPBitmapVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  *Buffer[i]=*copy.Buffer[i];

 return *this;
}

// �������� �������
PUBitmap& UPBitmapVector::operator [] (int index)
{
 return Buffer[index];
}

const PUBitmap& UPBitmapVector::operator [] (int index) const
{
 return Buffer[index];
}

UBitmap& UPBitmapVector::operator () (int index)
{
 return *Buffer[index];
}

const UBitmap& UPBitmapVector::operator () (int index) const
{
 return *Buffer[index];
}
// --------------------------


// ������
// --------------------------
// ������������ � �����������
// --------------------------
UBitmapVector::UBitmapVector(void)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;
}

UBitmapVector::UBitmapVector(const UBitmapVector &copy)
    : UPBitmapVector(copy)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;

 *this=copy;
}

UBitmapVector::~UBitmapVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// �������� ������ ������� � ����������� ������� ������
void UBitmapVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  PUBitmap *newbuffer=new PUBitmap[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  for(int i=Size;i<newsize;i++)
   newbuffer[i]=new UBitmap;

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ������� ������
void UBitmapVector::Clear(void)
{
 if(Buffer)
 {
  for(int i=0;i<RealSize;i++)
   delete Buffer[i];

  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UBitmapVector& UBitmapVector::operator = (const UBitmapVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  *Buffer[i]=*copy.Buffer[i];

 return *this;
}

// �������� �������
PUBitmap UBitmapVector::operator [] (int index)
{
 return Buffer[index];
}

PUBitmap UBitmapVector::operator [] (int index) const
{
 return Buffer[index];
}

UBitmap& UBitmapVector::operator () (int index)
{
 return *Buffer[index];
}

const UBitmap& UBitmapVector::operator () (int index) const
{
 return *Buffer[index];
}
// --------------------------


}

#endif


