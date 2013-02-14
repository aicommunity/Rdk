/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBPIPELINE_H
#define UBPIPELINE_H

#include "UBAModel.h"

namespace RDK {

// ����� �������� ���������  �����������
class UBPipeline: public UBAModel
{
protected: // ���� ������
typedef UBAbstract* PUBAbstract;
protected: // ������

protected: // ��������� ����������
// ������ ������������� �����������
UBitmap *InputBuffer,*OutputBuffer;

// ������ ������� ������������� �����������
int InputBufferSize;
int OutputBufferSize;

// �������� ������ �������
int InputBufferRealSize;
int OutputBufferRealSize;

// ������������� ������� ������ � �������
PUBitmap *InputArray;
PUBitmap *OutputArray;

// ������� ������������� �������� ������ � �������
int InputArraySize, OutputArraySize;

// �������� ������� ������������� �������� ������ � �������
int InputArrayRealSize, OutputArrayRealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBPipeline(void);
virtual ~UBPipeline(void);
// --------------------------

// ---------------------
// ������ ���������� ���������
// ---------------------
// ---------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<UAContainer> comp) const;
// --------------------------

// ---------------------
// ������ �����
// ---------------------
// �������� ����� ����� ����� �������
virtual UBPipeline* New(void);
// ---------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
// ����� ���� ������� ��������� �� ��������� ����������
virtual bool AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UAContainer> comp);
// --------------------------

// ---------------------
// ������� ������ �����
// ---------------------
protected:
// ��������� ������ ������ ����������� �� �������� ��������
void ExpandInputBuffer(int size);
void ExpandOutputBuffer(int size);
void ExpandImageBuffer(UBitmap* &buffer, int &currentsize, int &realsize, int newsize);

// ��������� ������ �������������� ������� ������
void ExpandInputArray(int size);
void ExpandArray(PUBitmap* &array, int &currentsize, int &realsize, int newsize);

// ��������� ������ �������������� ������� �������
void ExpandOutputArray(int size);

// ����������� � ��������������� ��� �������� ���������� ����������� ������ ������ � �������
// ���� ������ ����� � ������ �������� ������������
void CalcNumIOs(void);
// ---------------------

// ---------------------
// ���������
// ---------------------
public:
UBAbstract* operator [] (int index);
// ---------------------

};


// ����� ������������� ��������
class UBParallelPipeline: public UBPipeline
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBParallelPipeline(void);
virtual ~UBParallelPipeline(void);
// --------------------------

// ---------------------
// ������ �����
// ---------------------
// �������� ����� ����� ����� �������
virtual UBParallelPipeline* New(void);
// ---------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
/// �������������� �������� �� ��������� � ����� �������� �����
virtual bool AFDefault(void);

/// ����� �������� �����.
virtual bool AFReset(void);

/// ��������� ������ ����� �������
virtual bool AFCalculate(void);
// --------------------------
};

}
#endif
