/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAMODEL_H
#define UAMODEL_H

#include "UANet.h"

namespace RDK {
/*
class UASource: public UASimpleNet
{

};

class UAReceiver: public UASimpleNet
{

};*/

class UAModel: public UANet
{
protected: // �������� ��������

protected: // ��������� �� ��������� � ��������� �������� ������
// ������ ����������
UCPointer<UANet, UAModel> Sources;

// ������ ����������
UCPointer<UANet, UAModel> Receivers;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAModel(void);
virtual ~UAModel(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UAModel* New(void);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// --------------------------
};


}

#endif



