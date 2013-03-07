/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBASTORAGE_H
#define UBASTORAGE_H

#include "UBAbstract.h"
#include "../Engine/UAStorage.h"

namespace RDK {

class UBAStorage: public UAStorage
{
public: // ���� ������

protected: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBAStorage(void);
virtual ~UBAStorage(void);
// --------------------------

// --------------------------
// ������ ���������� ��������� �������
// --------------------------
// ���������� �������� ������
/*UBAbstract* GetFilter(UId index);

// ��������� ����� ������
// ���������� Id �������, ��� ForbiddenId � ������ �������
virtual UId AddFilter(const UId &classid, UBAbstract *filter);
virtual UId AddFilter(UBAbstract *filter);

// ������� ������ �� �������
virtual void DelFilter(UId index);

// ������� ��� �������
virtual void ClearFilters(void); */
// --------------------------

// --------------------------
// ������ ���������� ������������ �������
// --------------------------
// --------------------------
};

}
#endif


