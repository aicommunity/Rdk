/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBAENVIRONMENT_CPP
#define UBAENVIRONMENT_CPP

#include "UBAEnvironment.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UBAEnvironment::UBAEnvironment(void)
{
}

UBAEnvironment::~UBAEnvironment(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ���������� ��������� �� ���������
UBAStorage* UBAEnvironment::GetStorage(void)
{
 return dynamic_cast<UBAStorage*>(UAEnvironment::GetStorage());
}

// ���������� ��������� �� ������
UEPtr<UBAModel> UBAEnvironment::GetModel(void)
{
 return dynamic_pointer_cast<UBAModel>(UAEnvironment::GetModel());
}

// ������� ����� ������ �� ��������� �� id ������
bool UBAEnvironment::CreateModel(const UId& classid)
{
 if(!GetStorage())
  return false;

 UEPtr<UAComponent> component=GetStorage()->GetClass(classid);
 if(!component)
  return false;

 if(!UAEnvironment::CreateModel(classid))
  return false;

 return true;
}

// ������� ����� ������ �� ��������� �� ����� ������
bool UBAEnvironment::CreateModel(const NameT& classname)
{
 return UAContainerEnvironment::CreateModel(classname);
}



// ���������� ������� ������
bool UBAEnvironment::DestroyModel(void)
{
 return UAEnvironment::DestroyModel();
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool UBAEnvironment::BuildStorage(void)
{
 return UAEnvironment::BuildStorage();
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// �������� ������ �������� �� ���������
UBMColorModel UBAEnvironment::GetDefaultColorModel(void) const
{
 return DefaultColorModel;
}

bool UBAEnvironment::SetDefaultColorModel(UBMColorModel model)
{
 if(DefaultColorModel == model)
  return true;

 DefaultColorModel=model;
 Ready=false;
 return true;
}

// ������������� ��������� ������ � ������, ���������� � ������� �������� �����������
UId UBAEnvironment::GetModelInputId(void) const
{
 return ModelInputId;
}

bool UBAEnvironment::SetModelInputId(UId value)
{
 if(ModelInputId == value)
  return true;

 ModelInputId=value;
 return true;
}

// ������������� ��������� ������ � ������, ���������� � �������� �������� �����������
UId UBAEnvironment::GetModelOutputId(void) const
{
 return ModelOutputId;
}

bool UBAEnvironment::SetModelOutputId(UId value)
{
 if(ModelOutputId == value)
  return true;

 ModelOutputId=value;
 return true;
}

// ���� ���������� ����������� ������ �������������� ��� ��� �����
bool UBAEnvironment::GetReflectionXFlag(void) const
{
 return ReflectionXFlag;
}

void UBAEnvironment::SetReflectionXFlag(bool value)
{
 if(ReflectionXFlag == value)
  return;

 ReflectionXFlag=value;
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ����� ������� �����������
int UBAEnvironment::GetNumInputImages(void) const
{
 return NumInputImages;
}

bool UBAEnvironment::SetNumInputImages(int value)
{
 if(NumInputImages == value)
  return true;

 NumInputImages = value;

 Ready=false;
 return true;
}

// ����� �������� �����������
int UBAEnvironment::GetNumOutputImages(void) const
{
 return NumOutputImages;
}

bool UBAEnvironment::SetNumOutputImages(int value)
{
 if(NumOutputImages == value)
  return true;

 NumOutputImages = value;

 Ready=false;
 return true;
}

// ���������� �������� �����������
int UBAEnvironment::GetInputImageWidth(int i) const
{
 return InputImages[i]->GetWidth();
}

int UBAEnvironment::GetInputImageHeight(int i) const
{
 return InputImages[i]->GetHeight();
}

UBMColorModel UBAEnvironment::GetInputImageColorModel(int i) const
{
 return InputImages[i]->GetColorModel();
}

bool UBAEnvironment::SetInputImageRes(int i, int width, int height)
{
 if(!Build())
  return false;

 InputImages[i]->SetRes(width,height,DefaultColorModel);
 return true;
}

// ���������� ��������� �����������
int UBAEnvironment::GetOutputImageWidth(int i) const
{
 return OutputImages[i]->GetWidth();
}

int UBAEnvironment::GetOutputImageHeight(int i) const
{
 return OutputImages[i]->GetHeight();
}

UBMColorModel UBAEnvironment::GetOutputImageColorModel(int i) const
{
 return OutputImages[i]->GetColorModel();
}

// ������ ������� �����������
bool UBAEnvironment::SetInputImage(int i, const UBitmap &bmp)
{
 if(!Build())
  return false;

 if(bmp.GetWidth() != InputImages[i]->GetWidth() ||
    bmp.GetHeight() != InputImages[i]->GetHeight())
  return false;

 bmp.ConvertTo(*InputImages[i]);
 if(ReflectionXFlag)
  InputImages[i]->ReflectionX();
 return true;
}

bool UBAEnvironment::SetInputImage(int i, UBColor *data, UBMColorModel colormodel)
{
 if(!Build())
  return false;

 if(!data)
  return false;

 TempConvertBitmap.AttachBuffer(InputImages[i]->GetWidth(), InputImages[i]->GetHeight(), data, colormodel);
 SetInputImage(i, TempConvertBitmap);
 TempConvertBitmap.DetachBuffer();
 return true;
}

// ���������� ��������� �� ������� �����������
PUBitmap* UBAEnvironment::GetInputImages(void) const
{
 return InputImages.GetBuffer();
}

// ���������� ������� ����������� �� �������
UBitmap& UBAEnvironment::GetInputImage(int i)
{
 return InputImages(i);
}

// ���������� ������ �������� ����������� �� �������
UBColor* UBAEnvironment::GetInputImageData(int i)
{
 return InputImages[i]->GetData();
}

// ���������� ��������� �� �������� �����������
PUBitmap* UBAEnvironment::GetOutputImages(void) const
{
 return OutputImages.GetBuffer();
}

// ���������� �������� ����������� �� �������
UBitmap& UBAEnvironment::GetOutputImage(int i)
{
 return OutputImages(i);
}

// ���������� ������ ��������� ����������� �� �������
UBColor* UBAEnvironment::GetOutputImageData(int i)
{
 return OutputImages[i]->GetData();
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// --------------------------

// --------------------------
// ������� ������ ����������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UBAEnvironment::ADefault(void)
{
 if(!UAContainerEnvironment::ADefault())
  return false;

 DefaultColorModel=ubmY8;
 SetNumInputImages(1);
 SetNumOutputImages(1);
 SetReflectionXFlag(true);

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UBAEnvironment::ABuild(void)
{
 InputImages.Resize(NumInputImages);
 OutputImages.Resize(NumOutputImages);

 UEPtr<UBAbstract> model=dynamic_pointer_cast<UBAbstract>(GetModel());
 if(model)
 {
  model->SetInternalInputsFlag(false);
  model->SetInternalOutputsFlag(false);
  model->SetNumInputs(GetNumOutputImages());
  model->SetNumOutputs(GetNumInputImages());
  model->SetInputs(OutputImages.GetBuffer());
  model->SetOutputs(InputImages.GetBuffer());
 }

 return UAContainerEnvironment::ABuild();
}

// ����� �������� �����.
bool UBAEnvironment::AReset(void)
{
 return UAContainerEnvironment::AReset();
}

// ��������� ������ ����� �������
bool UBAEnvironment::ACalculate(void)
{
 if(!UAContainerEnvironment::ACalculate())
  return false;

 UEPtr<UBAbstract> model=dynamic_pointer_cast<UBAbstract>(GetModel());
 if(model)
 {
  for(int i=0;i<GetNumOutputImages();i++)
   *OutputImages[i]=*model->GetInputs()[i];
 }

 // ��� ������ ������� ������
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ������������ ������� ���������
// --------------------------
// --------------------------

}
#endif


