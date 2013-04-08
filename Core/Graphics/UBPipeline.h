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
class UBPipeline: public UBAbstract
{
protected: // ���� ������

protected: // ��������� ����������

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
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
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
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------

// ---------------------
// ������� ������ �����
// ---------------------
protected:
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
