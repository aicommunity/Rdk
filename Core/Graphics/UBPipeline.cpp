/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBPIPELINE_CPP
#define UBPIPELINE_CPP

#include <string.h>
#include "UBPipeline.h"


namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UBPipeline::UBPipeline(void)
{
 // ������ ������������� �����������
 InputBuffer=0;
 OutputBuffer=0;

 // ������ ������� ������������� �����������
 InputBufferSize=0;
 OutputBufferSize=0;

 // �������� ������ �������
 InputBufferRealSize=0;
 OutputBufferRealSize=0;

 // ������������� ������� ������ � �������
 InputArray=0;
 OutputArray=0;

 // ������� ������������� �������� ������ � �������
 InputArraySize=0;
 OutputArraySize=0;

 // �������� ������� ������������� �������� ������ � �������
 InputArrayRealSize=0;
 OutputArrayRealSize=0;
}

UBPipeline::~UBPipeline(void)
{
 if(InputBuffer)
 {
  delete []InputBuffer;
  InputBuffer=0;
 }
 InputBufferSize=InputBufferRealSize=0;

 if(OutputBuffer)
 {
  delete []OutputBuffer;
  OutputBuffer=0;
 }
 OutputBufferSize=OutputBufferRealSize=0;

 // ������������� ������� ������ � �������
 if(InputArray)
 {
  delete []InputArray;
  InputArray=0;
  InputArraySize=0;
  InputArrayRealSize=0;
 }

 if(OutputArray)
 {
  delete []OutputArray;
  OutputArray=0;
  OutputArraySize=0;
  OutputArrayRealSize=0;
 }

}
// --------------------------

// ---------------------
// ������ ���������� ���������
// ---------------------
// ---------------------


// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool UBPipeline::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<UBAbstract>(comp))
  return true;
 return false;
}
// --------------------------

// ---------------------
// ������ �����
// ---------------------
// �������� ����� ����� ����� �������
UBPipeline* UBPipeline::New(void)
{
 return new UBPipeline;
}
// ---------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
// ����� ���� ������� ��������� �� ��������� ����������
bool UBPipeline::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 CalcNumIOs();
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool UBPipeline::ADelComponent(UEPtr<UContainer> comp)
{
 CalcNumIOs();
 return true;
}
// --------------------------

// ---------------------
// ������� ������ �����
// ---------------------
// ��������� ������ ������ ����������� �� �������� ��������
void UBPipeline::ExpandInputBuffer(int size)
{
 ExpandImageBuffer(InputBuffer,InputBufferSize,InputBufferRealSize,size);
}

void UBPipeline::ExpandOutputBuffer(int size)
{
 ExpandImageBuffer(OutputBuffer,OutputBufferSize,OutputBufferRealSize,size);
}

void UBPipeline::ExpandImageBuffer(UBitmap* &buffer, int &currentsize, int &realsize, int newsize)
{
 if(realsize<newsize || !buffer)
 {
  UBitmap *newbuffer=new UBitmap[newsize];
  for(int i=0;i<currentsize;i++)
   newbuffer[i]=buffer[i];

  if(buffer)
   delete []buffer;
  buffer=newbuffer;
  realsize=currentsize=newsize;
 }
 else
 {
  currentsize=newsize;
 }
}


// ��������� ������ �������������� ������� ������
void UBPipeline::ExpandInputArray(int size)
{
 ExpandArray(InputArray,InputArraySize,InputArrayRealSize,size);
}

// ��������� ������ �������������� ������� �������
void UBPipeline::ExpandOutputArray(int size)
{
 ExpandArray(OutputArray,OutputArraySize,OutputArrayRealSize,size);
}

void UBPipeline::ExpandArray(PUBitmap* &array, int &currentsize, int &realsize, int newsize)
{
 if(realsize<newsize || !array)
 {
  PUBitmap *buffer=new PUBitmap[newsize];
  for(int i=0;i<currentsize;i++)
   buffer[i]=array[i];

  if(array)
  {
   delete []array;
   array=0;
  }
  array=buffer;
  realsize=currentsize=newsize;
 }
 else
 {
  currentsize=newsize;
 }
}


// ����������� � ��������������� ��� �������� ���������� ����������� ������ ������ � �������
// ���� ������ ����� � ������ �������� ������������
void UBPipeline::CalcNumIOs(void)
{
 // ������� ����������� ����������� ����� ������� ��� �������
 int max_output_size=0, max_input_size=0;
 for(int i=0;i<NumComponents;i++)
 {
  if(static_pointer_cast<UBAbstract>(PComponents[i]))
  {
   if(max_output_size<static_pointer_cast<UBAbstract>(PComponents[i])->GetNumOutputs())
    max_output_size=static_pointer_cast<UBAbstract>(PComponents[i])->GetNumOutputs();
   if(max_input_size<static_pointer_cast<UBAbstract>(PComponents[i])->GetNumInputs())
    max_input_size=static_pointer_cast<UBAbstract>(PComponents[i])->GetNumInputs();
  }
 }

 if(NumOutputs<max_output_size)
  SetNumOutputs(max_output_size);

 if(NumInputs<max_input_size)
  SetNumInputs(max_input_size);
}
// ---------------------


// ---------------------
// ���������
// ---------------------
UBAbstract* UBPipeline::operator [] (int index)
{
 return static_pointer_cast<UBAbstract>(PComponents[index]);
}
// ---------------------




// class UBParallelPipeline
// --------------------------
// ������������ � �����������
// --------------------------
UBParallelPipeline::UBParallelPipeline(void)
{
}

UBParallelPipeline::~UBParallelPipeline(void)
{
}
// --------------------------

// ---------------------
// ������ �����
// ---------------------
// �������� ����� ����� ����� �������
UBParallelPipeline* UBParallelPipeline::New(void)
{
 return new UBParallelPipeline;
}
// ---------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UBParallelPipeline::AFDefault(void)
{
 return true;
}

// ����� �������� �����.
bool UBParallelPipeline::AFReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool UBParallelPipeline::AFCalculate(void)
{
 int min_inputs=(NumInputs<Inputs.GetSize())?NumInputs:Inputs.GetSize();
 int min_outputs=(NumOutputs<Outputs.GetSize())?NumOutputs:Outputs.GetSize();

 ExpandInputArray(NumInputs);
 ExpandOutputArray(NumOutputs);
 for(int i=0;i<min_inputs;i++)
  InputArray[i]=(UBitmap *)Inputs[i];

 for(int i=0;i<min_outputs;i++)
  OutputArray[i]=Outputs[i];

 for(int i=0;i<NumComponents;i++)
 {
  if(static_pointer_cast<UBAbstract>(PComponents[i]))
  {
   if(!static_pointer_cast<UBAbstract>(static_pointer_cast<UBAbstract>(PComponents[i]))->SetInputs(InputArray))
    return false;
   if(!static_pointer_cast<UBAbstract>(static_pointer_cast<UBAbstract>(PComponents[i]))->SetOutputs(OutputArray))
    return false;
   if(!static_pointer_cast<UBAbstract>(static_pointer_cast<UBAbstract>(PComponents[i]))->Calculate())
    return false;
  }
 }

 return true;
}
// --------------------------
}

#endif


