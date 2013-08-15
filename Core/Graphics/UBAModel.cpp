/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBAMODEL_CPP
#define UBAMODEL_CPP

#include "UBAModel.h"

namespace RDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
UBAModel::UBAModel(void)
 : Output("Output",this,0)
{
}

UBAModel::~UBAModel(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
UBAModel* UBAModel::New(void)
{
 return new UBAModel;
}
// --------------------------



}
#endif


