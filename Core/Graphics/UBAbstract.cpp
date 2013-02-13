/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBABSTRACT_CPP
#define UBABSTRACT_CPP

#include "UBAbstract.h"

namespace RDK {

// ---------------------
// ������������ � �����������
// ---------------------
UBAbstract::UBAbstract(void)
{
 // ����� ������ �������
 NumInputs=0;

 // ����� ������� �������
 NumOutputs=0;
}

UBAbstract::~UBAbstract(void)
{
}
// ---------------------

// ---------------------
// ������ ���������� �����������
// ---------------------
// ����, ����������� ������������ ������ ���������� ������� ������
bool UBAbstract::GetInternalInputsFlag(void) const
{
 return InternalInputsFlag;
}

bool UBAbstract::SetInternalInputsFlag(bool value)
{
 if(InternalInputsFlag == value)
  return true;

 InternalInputsFlag=value;
 Ready=false;
 return true;
}

// ����, ����������� ������������ ������ ���������� �������� ������
bool UBAbstract::GetInternalOutputsFlag(void) const
{
 return InternalOutputsFlag;
}

bool UBAbstract::SetInternalOutputsFlag(bool value)
{
 if(InternalOutputsFlag == value)
  return true;

 InternalOutputsFlag=value;
 Ready=false;
 return true;
}

// �������� ������ ����� � ������
UBMColorModel UBAbstract::GetInputColorModel(int index) const
{
 return InputColorModel[index];
}

bool UBAbstract::SetInputColorModel(int index, UBMColorModel cmodel)
{
 if(InputColorModel[index] == cmodel)
  return true;

 InputColorModel[index] = cmodel;
 return true;
}

UBMColorModel UBAbstract::GetOutputColorModel(int index) const
{
 return OutputColorModel[index];
}

bool UBAbstract::SetOutputColorModel(int index, UBMColorModel cmodel)
{
 if(OutputColorModel[index] == cmodel)
  return true;

 OutputColorModel[index] = cmodel;
 return true;
}
// ---------------------

// ---------------------
// ������ ���������� �������
// ---------------------
// ---------------------

// ---------------------
// ������ �����
// ---------------------
bool UBAbstract::PLACalculate(UBitmap **input, UBitmap **output, int num_inputs, int num_outputs)
{
 return true;
}

bool UBAbstract::PLCalculate(UBitmap **input, UBitmap **output, int num_inputs, int num_outputs)
{
 if(!Build())
  return false;

 if(!input || !output || num_inputs<NumInputs || num_outputs<NumOutputs)
  return true;
 /*
 for(int i=0;i<NumInputs;i++)
 {
  if(!input[i])
   return true;

  if(InputColorModel[i] != ubmUnknown)
  {
   if(InternalInputsFlag)
	input[i]->SetColorModel(InputColorModel[i]);
   else
	if(InputColorModel[i] != input[i]->GetColorModel())
   	 return true;
  }
 }
         */
 for(int i=0;i<NumOutputs;i++)
 {
  if(!output[i])
   return true;
  if(OutputColorModel[i] != ubmUnknown)
   output[i]->SetColorModel(OutputColorModel[i]);
 }

 if(!PLACalculate(input, output, num_inputs, num_outputs))
  return false;

 return true;
}

// ���������� ������ ������� �����������
UPBitmapVector& UBAbstract::GetInputs(void)
{
 return Inputs;
}

// ���������� ������ �������� �����������
UPBitmapVector& UBAbstract::GetOutputs(void)
{
 return Outputs;
}

// ������������� ������ ������� �����������
bool UBAbstract::SetInputs(PUBitmap* bitmaps)
{
 if(!Build())
  return false;

 for(int i=0;i<Inputs.GetSize();i++)
  Inputs[i]=bitmaps[i];

 return true;
}

bool UBAbstract::SetInput(int index, PUBitmap bitmap)
{
 if(index <0 || index>=NumInputs)
  return false;

 if(!Build())
  return false;

 Inputs[index]=bitmap;

 return true;
}

// ������������� ������ �������� �����������
bool UBAbstract::SetOutputs(PUBitmap* bitmaps)
{
 if(InternalOutputsFlag)
  return false;

 if(!Build())
  return false;

 for(int i=0;i<Outputs.GetSize();i++)
  Outputs[i]=bitmaps[i];

 return true;
}

bool UBAbstract::SetOutput(int index, PUBitmap bitmap)
{
 if(InternalOutputsFlag)
  return false;

 if(index <0 || index>=NumOutputs)
  return false;

 if(!Build())
  return false;

 Outputs[index]=bitmap;

 return true;
}
// ---------------------

// ----------------------
// ���������������� ������
// ----------------------
// ��������� �������� ����� ��������� ������������ �����
bool UBAbstract::AConnectToItem(UEPtr<UAItem> na, int i_index, int c_index)
{
 UEPtr<UBAbstract> filter=dynamic_pointer_cast<UBAbstract>(na);

 if(!InternalInputsFlag)
  Inputs[c_index]=filter->Outputs[i_index];

 return AFConnectToItem(filter,i_index,c_index);
}

// ��������� �������� ����� ��������� ���������� �����
void UBAbstract::ADisconnectFromItem(UEPtr<UAItem> na, int i_index, int c_index)
{
 UEPtr<UBAbstract> filter=dynamic_pointer_cast<UBAbstract>(na);

 AFDisconnectFromItem(filter,i_index,c_index);
}
// ----------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UBAbstract::ADefault(void)
{
 SetNumInputs(1);
 SetNumOutputs(1);
 SetInternalOutputsFlag(true);
 SetInternalInputsFlag(false);
 return AFDefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UBAbstract::ABuild(void)
{
 Outputs.Resize(NumOutputs);

 Inputs.Resize(NumInputs);

 if(InternalInputsFlag)
 {
  InternalInputs.Resize(NumInputs);
  for(int i=0;i<Inputs.GetSize();i++)
   Inputs[i]=InternalInputs[i];
 }
 else
  InternalInputs.Clear();

 if(InternalOutputsFlag)
 {
  InternalOutputs.Resize(NumOutputs);
  for(int i=0;i<Outputs.GetSize();i++)
   Outputs[i]=InternalOutputs[i];
 }
 else
  InternalOutputs.Clear();

 InputColorModel.assign(NumInputs,ubmUnknown);
 OutputColorModel.assign(NumOutputs,ubmUnknown);

 if(!AFBuild())
  return false;

 if(InternalInputsFlag)
 {
  for(int i=0;i<Inputs.GetSize();i++)
   Inputs[i]->SetColorModel(InputColorModel[i],false);
 }

 if(InternalOutputsFlag)
 {
  for(int i=0;i<Outputs.GetSize();i++)
   if(Outputs[i])
	Outputs[i]->SetColorModel(OutputColorModel[i],false);
 }
 return true;
}

// ����� �������� ����� ��� ������ ��������
bool UBAbstract::AReset(void)
{
 return AFReset();
}

// ��������� ������ ����� �������
bool UBAbstract::ACalculate(void)
{
 if(InternalInputsFlag)
 {
  throw int(1);
 /*
  for(int i=0;i<NumInputs;i++)
  {
   const UCItem &citem=GetCItem(i);
   UBAbstract* input=static_cast<UBAbstract*>(citem.Item);
   if(!input)
	return true;
   int index=citem.Index;

   if(input->GetNumOutputs()<=index)
	return true;

   UBitmap *bmp=input->GetOutputs()[index];
   if(!bmp)
	return true;
   UBMColorModel cmodel=InputColorModel[i];
   if(cmodel == ubmUnknown)
   {
    *Inputs[i]=*bmp;
   }
   else
   {
    Inputs[i]->SetRes(bmp->GetWidth(),bmp->GetHeight(),InputColorModel[i]);
    bmp->ConvertTo(*Inputs[i]);
   }
  }   */
 }
 else
 {
  for(int i=0;i<NumInputs;i++)
  {
   const UCItem &citem=GetCItem(i);
   UBAbstract* input=static_cast<UBAbstract*>(citem.Item);
   if(input)
   {
	int index=citem.Index;

	if(input->GetNumOutputs()<=index)
	 return true;

	Inputs[i]=input->GetOutputs()[index];
   }
  }

 }

 if(!PLCalculate(Inputs.GetBuffer(), Outputs.GetBuffer(), NumInputs, NumOutputs))
  return false;

 return AFCalculate();
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������ ��������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UBAbstract::AFDefault(void)
{
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UBAbstract::AFBuild(void)
{
 return true;
}

// ����� �������� ����� ��� ������ ��������
bool UBAbstract::AFReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool UBAbstract::AFCalculate(void)
{
 return true;
}
// --------------------------


// ----------------------
// ���������������� ������ ��������
// ----------------------
// ��������� �������� ����� ��������� ������������ �����
bool UBAbstract::AFConnectToItem(UEPtr<UBAbstract> na, int i_index, int c_index)
{
 return true;
}

// ��������� �������� ����� ��������� ���������� �����
void UBAbstract::AFDisconnectFromItem(UEPtr<UBAbstract> na, int i_index, int c_index)
{
 return;
}
// ----------------------

}
#endif



