/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UCONTROLLER_CPP
#define UCONTROLLER_CPP

#include <algorithm>
#include "UController.h"
#include "UContainer.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UController::UController(void)
{
 Enabled=false;
}

UController::~UController(void)
{
 UnLinkAll();
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���� ���������� ���������� ����������
bool UController::IsEnabled(void) const
{
 return Enabled;
}

bool UController::IsEnabled(bool value)
{
 if(Enabled == value)
  return true;

 Enabled=value;
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���������� ����� ��������� � ������� ��������� ����������
size_t UController::GetNumComponents(void) const
{
 return Components.size();
}

// ���������� ��������� � �������� ��������� ���������� �� �������
UContainer* UController::GetComponents(size_t index)
{
 return Components[index];
}

// ���������� ������ ���������� �� ���������
int UController::FindComponent(UContainer* component) const
{
 vector<UContainer*>::const_iterator I=find(Components.begin(),Components.end(),component);

 if(I != Components.end())
  return I-Components.begin();

 return -1;
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ��������� ��������� � �����������
bool UController::Link(UContainer* component, bool forchilds)
{
 if(find(Components.begin(),Components.end(),component) != Components.end())
  return true;

 Components.push_back(component);
 component->AddController(this,forchilds);

 if(!ALink(component))
 {
  component->DelController(this,forchilds);
  Components.erase(Components.begin()+(Components.size()-1));
  return false;
 }

 return true;
}

// ���������� ��������� �� ����������
bool UController::UnLink(int index, bool forchilds)
{
 if(index<0 || index> int(Components.size()))
  return false;

 if(!AUnLink(index,forchilds))
  return false;

 Components[index]->DelController(this,forchilds);
 Components.erase(Components.begin()+index);
 return true;
}

bool UController::UnLink(UContainer* component, bool forchilds)
{
 int index=FindComponent(component);

 if(index < 0)
  return true;

 return UnLink(index, forchilds);
}

bool UController::UnLinkAll(bool forchilds)
{
 bool res=true;
 while(Components.begin() != Components.end())
 {
  res &= UnLink(Components.size()-1,forchilds);
 }

 return res;
}


// ��������� ���������
bool UController::Update(void)
{
 if(!Enabled)
  return true;

 return AUpdate();
}
// --------------------------

}


#endif

