/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAMODEL_CPP
#define UAMODEL_CPP

#include "UAModel.h"

namespace RDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
UAModel::UAModel(void)
 : Sources("Sources",this),
   Receivers("Receivers",this)
{
}

UAModel::~UAModel(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
UAModel* UAModel::New(void)
{
 return new UAModel;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// --------------------------


}
#endif


