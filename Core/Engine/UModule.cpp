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
: Ready(false), InitFlag(false)
{
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
// ���� ���������� ������� � ��������� �������������
bool UModule::IsInit(void) const
{
 return InitFlag;
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UModule::BeforeDefault(void)
{
 ABeforeDefault();
 return true;
}

bool UModule::AfterDefault(void)
{
 AAfterDefault();
 Build();
 return true;
}

bool UModule::Default(void)
{
 BeforeDefault();
 Ready=false;
 ADefault();
 AfterDefault();
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UModule::BeforeBuild(void)
{
 ABeforeBuild();
 return true;
}

bool UModule::AfterBuild(void)
{
 AAfterBuild();
 Reset();
 return true;
}

bool UModule::Build(void)
{
 if(IsReady())
  return true;
 BeforeBuild();
 ABuild();
 Ready=true;
 AfterBuild();
 return true;
}

// ����� �������� �����.
bool UModule::BeforeReset(void)
{
 ABeforeReset();
 return true;
}

bool UModule::AfterReset(void)
{
 AAfterReset();
 return true;
}

bool UModule::Reset(void)
{
 if(!IsReady())
 {
  Build();
  return true;
 }
 BeforeReset();
 AReset();
 AfterReset();
 return true;
}

// ��������� ������ ����� �������
bool UModule::BeforeCalculate(void)
{
 ABeforeCalculate();
 return true;
}

bool UModule::AfterCalculate(void)
{
 AAfterCalculate();
 return true;
}

bool UModule::Calculate(void)
{
 if(!IsReady())
  Build();

 BeforeCalculate();
 ACalculate();
 AfterCalculate();
 return true;
}

// ��������� ������������� ����� �������
void UModule::BeforeInit(void)
{
 ABeforeInit();
}

void UModule::AfterInit(void)
{
 AAfterInit();
}

void UModule::Init(void)
{
 if(IsInit())
  return;

 BeforeInit();
 AInit();
 InitFlag=true;
 AfterInit();
}

// ��������� ��������������� ����� �������
void UModule::BeforeUnInit(void)
{
 ABeforeUnInit();
}

void UModule::AfterUnInit(void)
{
 AAfterUnInit();
}

void UModule::UnInit(void)
{
 if(!IsInit())
  return;


 BeforeUnInit();
 AUnInit();
 AfterUnInit();
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UModule::ABeforeDefault(void)
{
 return true;
}

bool UModule::AAfterDefault(void)
{
 return true;
}

bool UModule::ADefault(void)
{
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UModule::ABeforeBuild(void)
{
 return true;
}

bool UModule::AAfterBuild(void)
{
 return true;
}

bool UModule::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool UModule::ABeforeReset(void)
{
 return true;
}

bool UModule::AAfterReset(void)
{
 return true;
}

bool UModule::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool UModule::ABeforeCalculate(void)
{
 return true;
}

bool UModule::AAfterCalculate(void)
{
 return true;
}

bool UModule::ACalculate(void)
{
 return true;
}

// ��������� ������������� ����� �������
void UModule::ABeforeInit(void)
{

}

void UModule::AAfterInit(void)
{

}

void UModule::AInit(void)
{
}

// ��������� ��������������� ����� �������
void UModule::ABeforeUnInit(void)
{

}

void UModule::AAfterUnInit(void)
{

}

void UModule::AUnInit(void)
{
}
// --------------------------

}

#endif

