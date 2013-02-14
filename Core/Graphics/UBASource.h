/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBASOURCE_H
#define UBASOURCE_H

#include "UBAbstract.h"

namespace RDK {

/// ������� �����-�������� �����������
class UBASource: public UBAbstract
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBASource(void);
virtual ~UBASource(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������
// --------------------------
/// ��������������� ������ �������� �����������
bool SetOutputData(int index, const UBitmap &bitmap);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
/// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UBASource* New(void);
// --------------------------

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



