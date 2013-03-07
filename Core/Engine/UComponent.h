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
#include "UEnvSupport.h"
#include "UEPtr.h"

namespace RDK {

class UStorage;
class UComponentDescription;

//typedef long int UId;
//extern UId ForbiddenId;

class UComponent: public UModule
{
friend class UStorage;
public: // ������ �������� ����������
class IException: public UException {};

protected: // �������� ��������
// ��������� �� ��������� ���� ��������
UEPtr<UComponent> Owner;

// ��������� �� �������� ��������� ���� ��������
// ������������� ��������������� ��� ���� �������� ��������
UEPtr<UComponent> MainOwner;

// ��������� �� ��������� ��������� ����� �������
UEPtr<UStorage> Storage;

protected: // ������
// ������������� ������
UId Class;

// ������������� ���������� �������
//UId Id;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UComponent(void);
virtual ~UComponent(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������
UEPtr<UComponent> const GetOwner(void) const;
virtual bool SetOwner(UEPtr<UComponent> owner);

// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UComponent> const GetMainOwner(void) const;
virtual void SetMainOwner(UEPtr<UComponent> mainowner);

// ���������� ��������� ��������� ����� �������
UEPtr<UStorage> const GetStorage(void) const;
virtual bool SetStorage(UEPtr<UStorage> storage);
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
virtual UComponent* New(void)=0;

// ������� ��������� �������� ������
virtual UComponentDescription* NewDescription(void);
virtual UComponentDescription* ANewDescription(UComponentDescription* description);

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

