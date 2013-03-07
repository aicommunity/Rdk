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

#include "UComponent.h"
#include "UStorage.h"
#include "UComponentDescription.h"

namespace RDK {

//class UComponent
// --------------------------
// ������������ � �����������
// --------------------------
UComponent::UComponent(void)
{
// Owner=0;

// MainOwner=0;

// Storage=0;

 Class=ForbiddenId;
}

UComponent::~UComponent(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������.
UEPtr<UComponent> const UComponent::GetOwner(void) const
{
 return Owner;
}

bool UComponent::SetOwner(UEPtr<UComponent> owner)
{
 if(Owner == owner)
  return true;

 Owner=owner;
 return true;
}


// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UComponent> const UComponent::GetMainOwner(void) const
{
 return MainOwner;
}

void UComponent::SetMainOwner(UEPtr<UComponent> mainowner)
{
 if(mainowner != MainOwner)
  MainOwner=mainowner;
}

// ���������� ��������� ��������� ����� �������
UEPtr<UStorage> const UComponent::GetStorage(void) const
{
 return Storage;
}

bool UComponent::SetStorage(UEPtr<UStorage> storage)
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
UId UComponent::GetClass(void) const
{
 return Class;
}

bool UComponent::SetClass(UId value)
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
UComponentDescription* UComponent::NewDescription(void)
{
 UComponentDescription* result=new UComponentDescription;

 return ANewDescription(result);
}

UComponentDescription* UComponent::ANewDescription(UComponentDescription* description)
{
 return description;
}

// ����������� ����� �������
void UComponent::Free(void)
{
 delete this;
}
// --------------------------

}

#endif


