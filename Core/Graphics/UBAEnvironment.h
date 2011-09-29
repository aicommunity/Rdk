/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBAENVIRONMENT_H
#define UBAENVIRONMENT_H

#include "../Engine/UAContainerEnvironment.h"
#include "UBAStorage.h"
#include "UBitmapVector.h"
#include "UBPipeline.h"

namespace RDK {

class UBAEnvironment: public UAContainerEnvironment//, public UBPipeline
{
protected: // ���������
// �������� ������ �������� �� ���������
UBMColorModel DefaultColorModel;

// ����� ������� �����������
int NumInputImages;

// ����� �������� �����������
int NumOutputImages;

protected: // ���������

protected: // ������
// ������� ������ �����������
mutable UBitmapVector InputImages;

// �������� ������ �����������
mutable UBitmapVector OutputImages;

// ������������� ��������� ������ � ������, ���������� � ������� �������� �����������
UId ModelInputId;

// ������������� ��������� ������ � ������, ���������� � �������� �������� �����������
UId ModelOutputId;

protected: // ��������� ����������
// ��������� ����������� ��� �������� ��������������
UBitmap TempConvertBitmap;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBAEnvironment(void);
virtual ~UBAEnvironment(void);
// --------------------------

// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ���������� ��������� �� ���������
virtual UBAStorage* GetStorage(void);

// ���������� ��������� �� ������
virtual UBAModel* GetModel(void);

// ������� ����� ������ �� ��������� �� id ������
virtual bool CreateModel(const UId& classid);

// ������� ����� ������ �� ��������� �� ����� ������
virtual bool CreateModel(const NameT& classname);

// ���������� ������� ������
virtual bool DestroyModel(void);

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
virtual bool BuildStorage(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// �������� ������ �������� �� ���������
UBMColorModel GetDefaultColorModel(void) const;
bool SetDefaultColorModel(UBMColorModel model);

// ������������� ��������� ������ � ������, ���������� � ������� �������� �����������
UId GetModelInputId(void) const;
bool SetModelInputId(UId value);

// ������������� ��������� ������ � ������, ���������� � �������� �������� �����������
UId GetModelOutputId(void) const;
bool SetModelOutputId(UId value);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ����� ������� �����������
int GetNumInputImages(void) const;
bool SetNumInputImages(int value);

// ����� �������� �����������
int GetNumOutputImages(void) const;
bool SetNumOutputImages(int value);

// ���������� �������� �����������
int GetInputImageWidth(int i) const;
int GetInputImageHeight(int i) const;
UBMColorModel GetInputImageColorModel(int i) const;
bool SetInputImageRes(int i, int width, int height);

// ���������� ��������� �����������
int GetOutputImageWidth(int i) const;
int GetOutputImageHeight(int i) const;
UBMColorModel GetOutputImageColorModel(int i) const;

// ������ ������� �����������
bool SetInputImage(int i, const UBitmap &bmp);
bool SetInputImage(int i, UBColor *data, UBMColorModel colormodel);

// ���������� ��������� �� ������� �����������
PUBitmap* GetInputImages(void) const;

// ���������� ������� ����������� �� �������
UBitmap& GetInputImage(int i);

// ���������� ������ �������� ����������� �� �������
UBColor* GetInputImageData(int i);

// ���������� ��������� �� �������� �����������
PUBitmap* GetOutputImages(void) const;

// ���������� �������� ����������� �� �������
UBitmap& GetOutputImage(int i);

// ���������� ������ ��������� ����������� �� �������
UBColor* GetOutputImageData(int i);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
public:
// --------------------------

// --------------------------
// ������� ������ ����������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool ABuild(void);

// ����� �������� �����.
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------

// --------------------------
// ������� ������ ������������ ������� ���������
// --------------------------
// --------------------------


};

}

#endif


