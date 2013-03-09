/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef USerStorageBinaryBINARY_H
#define USerStorageBinaryBINARY_H

#include "USerStorage.h"

namespace RDK {

class USerStorageBinary: public USerStorage
{
private:
// ��������� ������������ ������ �������
enum { INIT_SIZE = 1024 };

protected: // ������
// ������ �������
unsigned char* m_pData;

// ����� ������ � ������
int Size;

// ����������� ��������� ������ �������
int MaxSize;

// ������ ������� ��������
int Front;

// ������ ���������� ��������
int Back;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
// ������� ������ �������
USerStorageBinary(void);

// ������� ������� � �������������� ������������ ��������
USerStorageBinary(int size);

// ������� ������� ����������� ������� �� ������� vec
USerStorageBinary(const unsigned char* vec, int nsize);

virtual ~USerStorageBinary(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������ �������
unsigned char&    front(void);

// ���������� ��������� �������
unsigned char&    back(void);

// �������� ������� � ��������� �� �������
// ������ ��������� �� ������ �������
unsigned char& operator [] (int i);

// ���������� true ���� ������� �����
bool empty(void);

// ���������� ����� ��������� ������ � �������
int size(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ��������� ������� � �����
void push(const unsigned char& obj);

// ��������� ������� �� ������
void pop(void);

// ������� �������
void clear(void);


// ����������� ����� ��� size_ ���������
void reserve(int size_);

// ��������� ������� ������� �� ������� vec.
// 0-� ������� ������� ���������� ������� �������
void FromVec(const unsigned char* vec, int nsize);
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
protected:
// ����� ��������������� �������
void InternalResize(int size);
// --------------------------
};

}
#endif
