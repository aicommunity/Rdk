/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2008.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UITEM_DATA_H
#define UITEM_DATA_H


// ���������� �������������� � VS
//#pragma warning( disable : 4355)
//#pragma warning( disable : 4800)

//#pragma warning( disable: 4786 )

namespace RDK {

// ������
class RDK_LIB_TYPE UItemData
{
friend class UADItem;
public: // ������ ���������� �������� ������ �� ���������
static int DefaultDataSize;
public: // ������
// ��������� �� ������
union
{
  void *Void;
  void **PVoid;
  char *Char;
  unsigned char *UChar;
  short *Short;
  unsigned short *UShort;
  int *Int;
  unsigned int *UInt;
  long long *Long;
  unsigned long long *ULong;
  float *Float;
  double *Double;
  long double *LDouble;
};

protected: // ���������
// ������ ������� ������
int Size;

// ������ ������ � ������
int ByteSize;

// ������ ���������� ������� � ������
int DataSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UItemData(void);
UItemData(const UItemData &copy);
~UItemData(void);
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
// ������ ������� ������
virtual int GetSize(void) const;

// ������ ������ � ������
int GetByteSize(void) const;

// ������ ���������� ������� � ������
int GetDataSize(void) const;
// --------------------------

// --------------------------
// ������
// --------------------------
// ��������� ������
void Resize(int size);

// ��������� ������ � ���������� �������� ������
void Assign(int size, char ch);

// ��������� ������ � ���������� �������� �������������������
// ������ ByteSize
void Assign(int size, const void *data);

// ��������� ������ � ���������� �������� �������������������
// ������ size � ���������� ������ datasize
// ������ ������������ ������� � ������� shift
// ���������� ������ ������� datasize*size+shift*datasize*shift
void Assign(int size, int datasize, const void *data, int shift=0);

// ���������� �������� �������������������
// � ������������� ��� ������ ��� ��������
// ������ ������������ ������� � ������� shift
void Assign(int size, const void *data, int shift=0) const;

// ������������� ������ ���������� ������� � ������
bool SetDataSize(int size);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UItemData& operator = (const UItemData &data);

// �������� ���������� �������
// ������������, ��� ����������� ������ ����� ��� �� ������
// ���������� ���������� ������ ������������ ������� ������
UItemData& operator += (const UItemData &data);

// �������� �������
//inline char operator [] (int i);
// --------------------------
};



}

#endif

