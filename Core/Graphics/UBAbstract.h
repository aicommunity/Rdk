/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBABSTRACT_H
#define UBABSTRACT_H

#include "UBitmap.h"
#include "UBitmapVector.h"
#include "../Engine/UANet.h"
//#include "../../UEnvironment/UEInterface.h"

namespace RDK {
        /*
class UBInterface: public UEInterface
{

};        */

// ��������� ��� ��������
struct UBIOPair
{
int Input;
int Output;

// --------------------------
// ������������ � �����������
// --------------------------
UBIOPair(void);
UBIOPair(int input, int output);
// --------------------------
};

// ������� ����� ��� ���� ������� ��������� �����������
class UBAbstract: public UANet//UAComponent
{
protected: // ���������
// ����, ����������� ������������ ������ ���������� ������� ������
bool InternalInputsFlag;

// ����, ����������� ������������ ������ ���������� �������� ������
bool InternalOutputsFlag;

// ����� ������ �������
//int NumInputs;

// ����� ������� �������
//int NumOutputs;

// ����� ���������� ������ �������
int NumTransitInputs;

protected: // ������
// ������������ ������ ������� �����������
UPBitmapVector Inputs;

// ������������ ������ �������� �����������
UPBitmapVector Outputs;

// ��������� ������������ ������ �������� �����������
// ������������ �� ���������, �� ����� ���� ��������
UBitmapVector InternalOutputs;

// ��������� ������������ ������ ������� �����������
// �� ������������ �� ���������, �� ����� ���� �������
UBitmapVector InternalInputs;

// �������� ������ ����� � ������
std::vector<UBMColorModel> InputColorModel, OutputColorModel;

// ������� ������������ ������
int *InputTable;

// ������� ������������ �������
int *OutputTable;

// ������� ������������ ���������� ������
// ��������� ����� ����� ����� ����� ��������� ��������� � ����� c ��������
UBIOPair *TransitTable;

protected: // ������������
// ������ ���������� ��������� ����� ������
// (�������� ������������ ���������)
//int FirstParamIndex;

// ������ ��������� ��������� ����� ������
// (�������� ������������ ���������)
//int LastParamIndex;

public: // ������
// ---------------------
// ������������ � �����������
// ---------------------
UBAbstract(void);
virtual ~UBAbstract(void);
// ---------------------

// ---------------------
// ������ ���������� �����������
// ---------------------
// ����, ����������� ������������ ������ ���������� ������� ������
bool GetInternalInputsFlag(void) const;
bool SetInternalInputsFlag(bool value);

// ����, ����������� ������������ ������ ���������� �������� ������
bool GetInternalOutputsFlag(void) const;
bool SetInternalOutputsFlag(bool value);

// ����� ������ �������
//int GetNumInputs(void) const;
//bool SetNumInputs(int value);

// ����� ������� �������
//int GetNumOutputs(void) const;
//bool SetNumOutputs(int value);

// ����� ���������� ������
int GetNumTransitInputs(void) const;
bool SetNumTransitInputs(int value);

// �������� ������ ����� � ������
UBMColorModel GetInputColorModel(int index) const;
bool SetInputColorModel(int index, UBMColorModel cmodel);

UBMColorModel GetOutputColorModel(int index) const;
bool SetOutputColorModel(int index, UBMColorModel cmodel);
// ---------------------

// ---------------------
// ������ ���������� �������
// ---------------------
// ���������� ��������� �� ������� ������
const int* GetInputTable(void) const;

// ���������� ��������� �� ������� �������
const int* GetOutputTable(void) const;

// ���������� ��������� �� ������� ���������� ������
const UBIOPair* GetTransitTable(void) const;

// ������������ ������ � �������� ������� ������������ ������
int& InputTableValue(int index);

// ������������ ������ � �������� ������� ������������ �������
int& OutputTableValue(int index);

// ������������ ������ � �������� ������� ���������� ������
UBIOPair& TransitTableValue(int index);

// ������������ ������� ������������ ������ �� ���������
void SetDefaultInputTable(void);

// ������������ ������� ������������ ������� �� ���������
void SetDefaultOutputTable(void);

// ���������� ������ ������� �����������
UPBitmapVector& GetInputs(void);

// ���������� ������ �������� �����������
UPBitmapVector& GetOutputs(void);

// ������������� ������ ������� �����������
bool SetInputs(PUBitmap* bitmaps);
bool SetInput(int index, PUBitmap bitmap);

// ������������� ������ �������� �����������
bool SetOutputs(PUBitmap* bitmaps);
bool SetOutput(int index, PUBitmap bitmap);
// ---------------------

// ---------------------
// ������ �����
// ---------------------
// �������� ����� ����� ����� �������
virtual UBAbstract* New(void)=0;

// ������ ����� � ������� ��������
// Pipeline calculation methods
virtual bool PLACalculate(UBitmap **input, UBitmap **output, int num_inputs=1, int num_outputs=1)=0;
bool PLCalculate(UBitmap **input, UBitmap **output, int num_inputs=1, int num_outputs=1);
// ---------------------

// ----------------------
// ���������������� ������
// ----------------------
protected:
// ��������� �������� ����� ��������� ������������ �����
virtual bool AConnectToItem(UEPtr<UAItem> na, int i_index, int c_index);

// ��������� �������� ����� ��������� ���������� �����
virtual void ADisconnectFromItem(UEPtr<UAItem> na, int i_index, int c_index);
// ----------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������ ��������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool AFDefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool AFBuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AFReset(void);

// ��������� ������ ����� �������
virtual bool AFCalculate(void);
// --------------------------

// ----------------------
// ���������������� ������ ��������
// ----------------------
protected:
// ��������� �������� ����� ��������� ������������ �����
virtual bool AFConnectToItem(UEPtr<UBAbstract> na, int i_index, int c_index);

// ��������� �������� ����� ��������� ���������� �����
virtual void AFDisconnectFromItem(UEPtr<UBAbstract> na, int i_index, int c_index);
// ----------------------

};


}

#endif
