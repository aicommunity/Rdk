/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UMODULE_CPP
#define UMODULE_CPP

#include "UModule.h"

namespace RDK {


// --------------------------
// ������������ � �����������
// --------------------------
UModule::UModule(void)
{
 Ready=false;
}

UModule::~UModule(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���� ���������� ������� � �����
bool UModule::IsReady(void) const
{
 return Ready;
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UModule::Default(void)
{
 Ready=false;
 if(!ADefault())
  return false;

 return Build();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UModule::Build(void)
{
 if(IsReady())
  return true;

 if(ABuild())
 {
  Ready=true;
  return Reset();
 }

 Ready=false;
 return false;
}

// ����� �������� �����.
bool UModule::Reset(void)
{
 if(!IsReady())
  return Build();

 if(!AReset())
  return false;

 return true;
}

// ��������� ������ ����� �������
bool UModule::Calculate(void)
{
 if(!IsReady())
  if(!Build())
   return false;

 return ACalculate();
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UModule::ADefault(void)
{
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UModule::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool UModule::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool UModule::ACalculate(void)
{
 return true;
}
// --------------------------

}

#endif

