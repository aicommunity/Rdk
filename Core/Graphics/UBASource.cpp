/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UBASOURCE_CPP
#define UBASOURCE_CPP

#include "UBASource.h"

namespace RDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
UBASource::UBASource(void)
{
}

UBASource::~UBASource(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ��������������� ������ �������� �����������
bool UBASource::SetOutputData(int index, const UBitmap &bitmap)
{
 if(index<0 || index>=NumOutputs)
  return false;

 if(!Build())
  return false;

 Outputs(index)=bitmap;

 return true;
}
// --------------------------



// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
UBASource* UBASource::New(void)
{
 return new UBASource;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
bool UBASource::PLACalculate(UBitmap **input, UBitmap **output, int num_inputs, int num_outputs)
{
 return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool UBASource::AFDefault(void)
{
 return true;
}

// ����� �������� �����.
bool UBASource::AFReset(void)
{
 return true;
}

// ��������� ������ ����� �������
/*bool UBASource::ACalculate(void)
{
 if(ActionCounter.v>0)
 {
  ActionCounter-=TimeStep.v;
  if(ActionCounter.v <= 0) // ��������� ������� � �������� ��������
  {
   ActionCounter=0;
   Activity=false;
  }
 }

 return true;
}      */
// --------------------------
}
#endif


