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

namespace RDK {

// ������� ����� ��� ���� ������� ��������� �����������
class UBAbstract: public UANet
{
protected: // ���������
// ����, ����������� ������������ ������ ���������� ������� ������
bool InternalInputsFlag;

// ����, ����������� ������������ ������ ���������� �������� ������
bool InternalOutputsFlag;

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
