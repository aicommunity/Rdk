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
bool UBPipeline::CheckComponentType(UEPtr<UAContainer> comp) const
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
	/*
bool UBPipeline::Calculate(void)
{
 if(!Build())
  return false;

 CalcNumIOs();

 return ACalculate();
} */

bool UBPipeline::PLACalculate(UBitmap **input, UBitmap **output, int num_inputs, int num_outputs)
{
/*
 if(!NumComponents)
  return true;

 int min_inputs=(NumInputs<num_inputs)?NumInputs:num_inputs;
 int min_outputs=(NumOutputs<num_outputs)?NumOutputs:num_outputs;

 // ������������ ������ 1 ������� � ��������
 if(NumComponents == 1)
 {
  ExpandInputArray(NumInputs);
  ExpandOutputArray(NumOutputs);
  for(int i=0;i<min_inputs;i++)
   InputArray[i]=(UBitmap *)input[i];

  for(int i=0;i<min_outputs;i++)
   OutputArray[i]=output[i];

  if(PComponents[0])
  {
   if(!static_pointer_cast<UBAbstract>(PComponents[0])->SetInputs(InputArray))
    return false;
   if(!static_pointer_cast<UBAbstract>(PComponents[0])->SetOutputs(OutputArray))
	return false;
   return static_pointer_cast<UBAbstract>(PComponents[0])->Calculate();
//   return Pipeline[0]->PLCalculate(InputArray,OutputArray,InputArraySize,OutputArraySize);
  }
  return false;
 }

 int max_buffer_size=0;
 // ������� ����������� ����������� ����� ������� ��� �������
 for(int i=0;i<NumComponents;i++)
 {
  if(static_pointer_cast<UBAbstract>(PComponents[i]))
  {
   if(max_buffer_size<static_pointer_cast<UBAbstract>(static_pointer_cast<UBAbstract>(PComponents[i]))->GetNumInputs())
	max_buffer_size=static_pointer_cast<UBAbstract>(static_pointer_cast<UBAbstract>(PComponents[i]))->GetNumInputs();
   if(max_buffer_size<static_pointer_cast<UBAbstract>(static_pointer_cast<UBAbstract>(PComponents[i]))->GetNumOutputs())
	max_buffer_size=static_pointer_cast<UBAbstract>(static_pointer_cast<UBAbstract>(PComponents[i]))->GetNumOutputs();
  }
 }
 ExpandInputBuffer(max_buffer_size);
 ExpandOutputBuffer(max_buffer_size);

 ExpandInputArray(max_buffer_size);
 ExpandOutputArray(max_buffer_size);
 for(int i=0;i<min_inputs;i++)
  InputArray[i]=(UBitmap *)input[i];

 for(int i=0;i<max_buffer_size;i++)
  OutputArray[i]=&OutputBuffer[i];

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

  // ������������ �������������� ����� ������ ������������ ������� �� ����� ��������
  if(i<NumComponents-1)
  for(int l=0;l<static_pointer_cast<UBAbstract>(PComponents[i])->GetNumOutputs();l++)
  {
   bool key=false;
   int output_table_index=l;//static_pointer_cast<UBAbstract>(static_pointer_cast<UBAbstract>(PComponents[i]))->OutputTableValue(l);

   for(int j=i+1;j<NumComponents;j++)
   {
	for(int k=0;k<static_pointer_cast<UBAbstract>(PComponents[j])->GetNumOutputs();k++)
	{
     int output_table_index2=k;//static_pointer_cast<UBAbstract>(PComponents[j])->OutputTableValue(k);
     if(output_table_index == output_table_index2)
     {
      key=true;
      break;
     }
    }

    if(key)
     break;
   }

   if(!key)
   {
    if(output_table_index<NumOutputs && output_table_index<NumOutputs)
     *output[output_table_index]=*OutputArray[output_table_index];
   }
  }

  // ���� ��� ��� ��������� ������, �������
  if(i == NumComponents-1)
   break;

  for(int j=0;j<min_inputs;j++)
   InputArray[j]=(UBitmap *)input[j];

  // ������� ������ ������ ��� ���������� �������
  for(int l=0;l<static_pointer_cast<UBAbstract>(PComponents[i])->GetNumOutputs();l++)
  {
   int output_table_index=l;//static_pointer_cast<UBAbstract>(static_pointer_cast<UBAbstract>(PComponents[i]))->OutputTableValue(l);
   // ��������� ������� ������� ������
   InputArray[output_table_index]=OutputArray[output_table_index];
  }

  // ������� ������ ������� ��� ���������� �������
  for(int l=0;l<static_pointer_cast<UBAbstract>(PComponents[i+1])->GetNumOutputs();l++)
  {
   int output_table_index=l;//static_pointer_cast<UBAbstract>(PComponents[i+1])->OutputTableValue(l);

   if(i<NumComponents-2)
   {
    // ������ ������� ������� �������
    if(OutputArray[output_table_index] == &OutputBuffer[output_table_index])
     OutputArray[output_table_index]=&InputBuffer[output_table_index];
    else
     OutputArray[output_table_index]=&OutputBuffer[output_table_index];
   }
   else
   {
    OutputArray[output_table_index]=output[output_table_index];
   }
  }

 }
   */
 return true;
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
bool UBPipeline::AAddComponent(UEPtr<UAContainer> comp, UIPointer* pointer)
{
 CalcNumIOs();
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool UBPipeline::ADelComponent(UEPtr<UAContainer> comp)
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

bool UBParallelPipeline::PLACalculate(UBitmap **input, UBitmap **output, int num_inputs, int num_outputs)
{
 int min_inputs=(NumInputs<num_inputs)?NumInputs:num_inputs;
 int min_outputs=(NumOutputs<num_outputs)?NumOutputs:num_outputs;

 ExpandInputArray(NumInputs);
 ExpandOutputArray(NumOutputs);
 for(int i=0;i<min_inputs;i++)
  InputArray[i]=(UBitmap *)input[i];

 for(int i=0;i<min_outputs;i++)
  OutputArray[i]=output[i];

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
//   if(!static_pointer_cast<UBAbstract>(PComponents)[i]->PLCalculate(InputArray,OutputArray,InputArraySize,OutputArraySize))
//    return false;
  }
 }

 return true;
}
// ---------------------
}

#endif


