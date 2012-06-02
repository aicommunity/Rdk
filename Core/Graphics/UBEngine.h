/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBENGINE_H
#define UBENGINE_H

#include "../Engine/UEngine.h"
#include "UBAEnvironment.h"

namespace RDK{

class UBEngine: public UEngine
{
protected: // ��������� �������������

protected: // ���������� ���������

protected: // ������

protected: // ��������� ����������
RDK::UBitmap TempBmp,TempBmp2;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBEngine(void);
virtual ~UBEngine(void);
// --------------------------

// --------------------------
// ������ ���������� ����������� �������������
// --------------------------
// --------------------------


// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ��������� �� �����
virtual UBAEnvironment* GetEnvironment(void);

virtual UBAbstract* GetModel(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// �������������� ������ ������
virtual bool Init(void);

// ���������������� ������ ������
// � ��������� ������� ���������
virtual bool UnInit(void);

// ��������� �������
virtual bool Start(void);

// ���������������� �������
virtual bool Pause(void);

// ������������� �������
virtual bool Stop(void);
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ����� ������ �����
virtual void Env_SetNumInputImages(int number);

// ������ ����� ������� �����
virtual void Env_SetNumOutputImages(int number);

// ���������� ����� ������ �����
virtual int Env_GetNumInputImages(void);

// ���������� ����� ������� �����
virtual int Env_GetNumOutputImages(void);

// ������ ���������� �� ��������� (������� ����������)
virtual void Env_SetInputRes(int number, int width, int height);

// ������ ���� ��������� �������� ����������� ������ �������������� ���
virtual void Env_SetReflectionXFlag(bool value);

// ���������� ���������� �� ��������� (������� ����������)
virtual int Env_GetInputImageWidth(int number);
virtual int Env_GetInputImageHeight(int number);
virtual int Env_GetInputImageColorModel(int number);

// ���������� ������� �������� ����������
virtual int Env_GetOutputImageWidth(int number);
virtual int Env_GetOutputImageHeight(int number);
virtual int Env_GetOutputImageColorModel(int number);

virtual void Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

virtual unsigned char* Env_GetInputImage(int index);

virtual unsigned char* Env_GetOutputImage(int index);

virtual unsigned char* Env_GetOutputImageY8(int index);

// --------------------------
// ������ ���������� �������
// --------------------------
// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
virtual const RDK::UBitmap* const Model_GetComponentOutput(const char *stringid, int index);
// --------------------------

// --------------------------
// ������ ����������� ���������� �������
// --------------------------
protected:
// ��������� ����� ����������������� ���������
virtual int LoadPredefinedLibraries(void);
// --------------------------

// --------------------------
// ������ ����������� ���������� ��������
// --------------------------
protected:
// --------------------------

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

// ����� �������� �����.
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};


}
#endif
