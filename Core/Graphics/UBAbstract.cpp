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


// --------------------------
// ������������ � �����������
// --------------------------
UBIOPair::UBIOPair(void)
{
 Input=0;
 Output=0;
}

UBIOPair::UBIOPair(int input, int output)
{
 Input=input;
 Output=output;
}
// --------------------------

// ---------------------
// ������������ � �����������
// ---------------------
UBAbstract::UBAbstract(void)
{
 // ������� ������������ ������
 InputTable=0;

 // ������� ������������ �������
 OutputTable=0;

 // ����� ������ �������
 NumInputs=0;
 SetNumInputs(1);

 // ����� ������� �������
 NumOutputs=0;
 SetNumOutputs(1);

 NumTransitInputs=0;
 TransitTable=0;

 // ������ ���������� ��������� ����� ������
 // (�������� ������������ ���������)
// FirstParamIndex=-1;

 // ������ ��������� ��������� ����� ������
 // (�������� ������������ ���������)
// LastParamIndex=-1;
}

UBAbstract::~UBAbstract(void)
{
 // ������� ������������ ������
 if(InputTable)
 {
  delete []InputTable;
  InputTable=0;
 }

 // ������� ������������ �������
 if(OutputTable)
 {
  delete []OutputTable;
  OutputTable=0;
 }


 // ������� ������������ �������
 if(TransitTable)
 {
  delete []TransitTable;
  TransitTable=0;
 }

 // ����� ���������� ������
 NumTransitInputs=0;

 // ����� ������ �������
 NumInputs=0;

 // ����� ������� �������
 NumOutputs=0;
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

/*
// ����� ������ �������
int UBAbstract::GetNumInputs(void) const
{
 return NumInputs;
}

bool UBAbstract::SetNumInputs(int value)
{
 if(NumInputs == value)
  return true;

 NumInputs=value;
 if(InputTable)
 {
  delete []InputTable;
  InputTable=0;
 }
 InputTable=new int[NumInputs];

 SetDefaultInputTable();
 InputBitmaps.Resize(NumInputs);
 return true;
}
*/
/*
// ����� ������� �������
int UBAbstract::GetNumOutputs(void) const
{
 return NumOutputs;
}

bool UBAbstract::SetNumOutputs(int value)
{
 if(NumOutputs == value)
  return true;

 NumOutputs=value;

 if(OutputTable)
 {
  delete []OutputTable;
  OutputTable=0;
 }
 OutputTable=new int[NumOutputs];

 SetDefaultOutputTable();
 OutputBitmaps.Resize(NumOutputs);
 return true;
}    */

// ����� ���������� ������
int UBAbstract::GetNumTransitInputs(void) const
{
 return NumTransitInputs;
}

bool UBAbstract::SetNumTransitInputs(int value)
{
 if(NumTransitInputs == value)
  return true;

 NumTransitInputs=value;

 if(TransitTable)
 {
  delete []TransitTable;
  TransitTable=0;
 }
 TransitTable=new UBIOPair[NumTransitInputs];

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
// ���������� ��������� �� ������� ������
const int* UBAbstract::GetInputTable(void) const
{
 return InputTable;
}

// ���������� ��������� �� ������� �������
const int* UBAbstract::GetOutputTable(void) const
{
 return OutputTable;
}

// ���������� ��������� �� ������� ���������� ������
const UBIOPair* UBAbstract::GetTransitTable(void) const
{
 return TransitTable;
}

// ������������ ������ � �������� ������� ������������ ������
int& UBAbstract::InputTableValue(int index)
{
 Build();
 return InputTable[index];
}

// ������������ ������ � �������� ������� ������������ �������
int& UBAbstract::OutputTableValue(int index)
{
 Build();
 return OutputTable[index];
}

// ������������ ������ � �������� ������� ���������� ������
UBIOPair& UBAbstract::TransitTableValue(int index)
{
 return TransitTable[index];
}

// ������������ ������� ������������ ������ �� ���������
void UBAbstract::SetDefaultInputTable(void)
{
 for(int i=0;i<NumInputs;i++)
  InputTable[i]=i;
}

// ������������ ������� ������������ ������� �� ���������
void UBAbstract::SetDefaultOutputTable(void)
{
 for(int i=0;i<NumOutputs;i++)
  OutputTable[i]=i;
}
// ---------------------

// ---------------------
// ������ �����
// ---------------------
bool UBAbstract::PLCalculate(UBitmap **input, UBitmap **output, int num_inputs, int num_outputs)
{
 if(!Build())
  return false;

 if(!input || !output || num_inputs<NumInputs || num_outputs<NumOutputs)
  return true;

 for(int i=0;i<NumInputs;i++)
 {
  if(!input[InputTable[i]])
   return true;

  if(InputColorModel[InputTable[i]] != ubmUnknown)
   input[InputTable[i]]->SetColorModel(InputColorModel[InputTable[i]]);
 }

 for(int i=0;i<NumOutputs;i++)
 {
  if(!output[OutputTable[i]])
   return true;
  if(OutputColorModel[OutputTable[i]] != ubmUnknown)
   output[OutputTable[i]]->SetColorModel(OutputColorModel[OutputTable[i]]);
 }

 if(!PLACalculate(input, output, num_inputs, num_outputs))
  return false;


 // ������������ ���������� ����� � ������
 // �������� �� ������� �������, � ������������ � ������ � �������������
 // �������� ��������������� �� ������ ����
 for(int i=0;i<NumTransitInputs;i++)
 {
  *output[TransitTable[i].Output]=*input[TransitTable[i].Input];
 }

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
bool UBAbstract::AConnectToItem(UAItem *na, int i_index, int c_index)
{
 UBAbstract* filter=dynamic_cast<UBAbstract*>(na);

 if(!InternalInputsFlag)
  Inputs[c_index]=filter->Outputs[i_index];

 return AFConnectToItem(filter,i_index,c_index);
}

// ��������� �������� ����� ��������� ���������� �����
void UBAbstract::ADisconnectFromItem(UAItem *na, int i_index, int c_index)
{
 UBAbstract* filter=dynamic_cast<UBAbstract*>(na);

 AFDisconnectFromItem(filter,i_index,c_index);
}
// ----------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UBAbstract::ADefault(void)
{
 SetInternalOutputsFlag(true);
 SetInternalInputsFlag(true);
 return AFDefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UBAbstract::ABuild(void)
{
 if(OutputTable)
 {
  delete []OutputTable;
  OutputTable=0;
 }
 OutputTable=new int[NumOutputs];

 SetDefaultOutputTable();
 Outputs.Resize(NumOutputs);

 if(InputTable)
 {
  delete []InputTable;
  InputTable=0;
 }
 InputTable=new int[NumInputs];

 SetDefaultInputTable();
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

 return AFBuild();
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
  for(int i=0;i<NumInputs;i++)
  {
   const UCItem &citem=GetCItem(InputTable[i]);
   UBAbstract *input=static_cast<UBAbstract*>(citem.Item);
   if(!input)
    return true;
   int index=citem.Index;

   if(input->GetNumOutputs()<=index)
    return true;

   UBitmap *bmp=input->GetOutputs()[index];
   if(!bmp)
    return true;
   UBMColorModel cmodel=InputColorModel[InputTable[i]];
   if(cmodel == ubmUnknown)
   {
    *Inputs[i]=*bmp;
   }
   else
   {
    Inputs[i]->SetRes(bmp->GetWidth(),bmp->GetHeight(),InputColorModel[InputTable[i]]);
    bmp->ConvertTo(*Inputs[i]);
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
bool UBAbstract::AFConnectToItem(UBAbstract *na, int i_index, int c_index)
{
 return true;
}

// ��������� �������� ����� ��������� ���������� �����
void UBAbstract::AFDisconnectFromItem(UBAbstract *na, int i_index, int c_index)
{
 return;
}
// ----------------------

}
#endif



