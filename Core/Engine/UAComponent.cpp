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
// Id=ForbiddenId;
}

UAComponent::~UAComponent(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������.
UAComponent* const UAComponent::GetOwner(void) const
{
 return Owner;
}

bool UAComponent::SetOwner(UAComponent* owner)
{
 if(Owner == owner)
  return true;

 Owner=owner;
 return true;
}


// ���������� ��������� �� �������� ��������� ���� ��������
UAComponent* const UAComponent::GetMainOwner(void) const
{
 return MainOwner;
}

bool UAComponent::SetMainOwner(UAComponent* const mainowner)
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
/*
// ���������� ��������� �������������� Id ���������
UId UAComponent::GetLastId(void) const
{
 return LastId;
} */
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

/*
// ������������� ����������
UId UAComponent::GetId(void) const
{
 return Id;
}

bool UAComponent::SetId(UId value)
{
 if(Id == value)
  return true;

 Id=value;
 return true;
}            */
// --------------------------



// --------------------------
// ������ ���������� ������
// --------------------------
// ����������� ����� �������
void UAComponent::Free(void)
{
 delete this;
}
// --------------------------

}

#endif


