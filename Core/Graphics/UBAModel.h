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

#include "../Engine/UNet.h"

namespace RDK {

class UBAModel: public UNet
{
protected: // ����� � ������
UPropertyOutputData<UBitmap,UBAModel> Output;

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
// --------------------------
};

}

#endif



