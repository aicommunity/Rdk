/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UACOMPONENT_H
#define UACOMPONENT_H

#include "UModule.h"
#include "UAEnvSupport.h"

namespace RDK {

class UAStorage;

//typedef long int UId;
//extern UId ForbiddenId;

class UAComponent: public UModule
{
friend UAStorage;
protected: // �������� ��������
// ��������� �� ��������� ���� ��������
UAComponent *Owner;

// ��������� �� �������� ��������� ���� ��������
// ������������� ��������������� ��� ���� �������� ��������
UAComponent *MainOwner;

// ��������� �� ��������� ��������� ����� �������
UAStorage *Storage;

// ��������� �������������� Id ���������
UId LastId;

protected: // ������
// ������������� ������
UId Class;

// ������������� ���������� �������
//UId Id;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAComponent(void);
virtual ~UAComponent(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������
UAComponent* const GetOwner(void) const;
virtual bool SetOwner(UAComponent* owner);

// ���������� ��������� �� �������� ��������� ���� ��������
UAComponent* const GetMainOwner(void) const;
virtual bool SetMainOwner(UAComponent* const mainowner);

// ���������� ��������� ��������� ����� �������
UAStorage* const GetStorage(void) const;
virtual bool SetStorage(UAStorage* const storage);

// ���������� ��������� �������������� Id ���������
UId GetLastId(void) const;
// --------------------------


// --------------------------
// ������ ���������� �������
// --------------------------
// ������������� ������
UId GetClass(void) const;
bool SetClass(UId value);

// ������������� ����������
/*UId GetId(void) const;
bool SetId(UId value);          */
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ������� ��������� ����� ������
virtual UAComponent* New(void)=0;

// ����������� ����� �������
void Free(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// --------------------------
};

}

#endif

