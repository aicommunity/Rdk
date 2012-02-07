/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UACOMPONENT_CPP
#define UACOMPONENT_CPP

#include "UAComponent.h"
#include "UAStorage.h"
#include "UComponentDescription.h"

namespace RDK {

//class UAComponent
// --------------------------
// ������������ � �����������
// --------------------------
UAComponent::UAComponent(void)
{
 Owner=0;

 MainOwner=0;

 Storage=0;

 Class=ForbiddenId;
}

UAComponent::~UAComponent(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������.
UEPtr<UAComponent> const UAComponent::GetOwner(void) const
{
 return Owner;
}

bool UAComponent::SetOwner(UEPtr<UAComponent> owner)
{
 if(Owner == owner)
  return true;

 Owner=owner;
 return true;
}


// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UAComponent> const UAComponent::GetMainOwner(void) const
{
 return MainOwner;
}

bool UAComponent::SetMainOwner(UEPtr<UAComponent> mainowner)
{
 if(mainowner == MainOwner)
  return true;

 MainOwner=mainowner;
 return true;
}

// ���������� ��������� ��������� ����� �������
UAStorage* const UAComponent::GetStorage(void) const
{
 return Storage;
}

bool UAComponent::SetStorage(UAStorage* const storage)
{
 if(Storage == storage)
  return true;

 Storage=storage;
 return true;
}
// --------------------------


// --------------------------
// ������ ���������� �������
// --------------------------
// ������������� ������
UId UAComponent::GetClass(void) const
{
 return Class;
}

bool UAComponent::SetClass(UId value)
{
 if(Class == value)
  return true;

 Class=value;
 return true;
}
// --------------------------



// --------------------------
// ������ ���������� ������
// --------------------------
// ������� ��������� �������� ������
UComponentDescription* UAComponent::NewDescription(void)
{
 UComponentDescription* result=new UComponentDescription;

 result->SetClassId(sntoa(Class));

 return result;
}

// ����������� ����� �������
void UAComponent::Free(void)
{
 delete this;
}
// --------------------------

}

#endif


