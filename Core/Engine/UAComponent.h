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
#include "UEPtr.h"

namespace RDK {

class UAStorage;
class UComponentDescription;

//typedef long int UId;
//extern UId ForbiddenId;

class UAComponent: public UModule
{
friend class UAStorage;
public: // ������ �������� ����������
class IException: public UException {};

protected: // �������� ��������
// ��������� �� ��������� ���� ��������
UEPtr<UAComponent> Owner;

// ��������� �� �������� ��������� ���� ��������
// ������������� ��������������� ��� ���� �������� ��������
UEPtr<UAComponent> MainOwner;

// ��������� �� ��������� ��������� ����� �������
UEPtr<UAStorage> Storage;

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
UEPtr<UAComponent> const GetOwner(void) const;
virtual bool SetOwner(UEPtr<UAComponent> owner);

// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UAComponent> const GetMainOwner(void) const;
virtual void SetMainOwner(UEPtr<UAComponent> mainowner);

// ���������� ��������� ��������� ����� �������
UEPtr<UAStorage> const GetStorage(void) const;
virtual bool SetStorage(UEPtr<UAStorage> storage);
// --------------------------


// --------------------------
// ������ ���������� �������
// --------------------------
// ������������� ������
UId GetClass(void) const;
bool SetClass(UId value);
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ������� ��������� ����� ������
virtual UAComponent* New(void)=0;

// ������� ��������� �������� ������
virtual UComponentDescription* NewDescription(void);

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

