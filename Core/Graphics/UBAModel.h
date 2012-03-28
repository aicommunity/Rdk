/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBAMODEL_H
#define UBAMODEL_H

#include "UBAbstract.h"
#include "UBASource.h"
#include "UBAReceiver.h"

namespace RDK {

//class UBASource;
//class UBAReceiver;

class UBAModel: public UBAbstract
{
protected: // �������� ��������


protected: // ��������� �� ��������� � ��������� �������� ������
// ������ ����������
//UCPointer<UBASource, UBAModel> Sources;

// ������ ����������
//UCPointer<UBAReceiver, UBAModel> Receivers;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBAModel(void);
virtual ~UBAModel(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UBAModel* New(void);

virtual bool PLACalculate(UBitmap **input, UBitmap **output, int num_inputs=1, int num_outputs=1);
// --------------------------
};

}

#endif



