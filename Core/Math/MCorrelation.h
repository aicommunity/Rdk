/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef MCorrelationH
#define MCorrelationH

#include "../Engine/UModule.h"
#include <cstdlib>

//---------------------------------------------------------------------------
namespace RDK {

//using namespace std;
//using namespace Utilities;

// ������������� �����-����������
class RDK_LIB_TYPE NCC2D: public UModule
{
protected: // ���������
// ������� �������� �����������
int IWidth,IHeight;

// ������� �������
int TWidth,THeight;

// ��� ����������
int CStep;

// ���� ���������� ��������� ��������
bool SubAverageFlag;

// ����� �������� ��������� ����������
int NumCalcPasses;

// ���� ��������������� ���������� ���������� �������� ��������� ����������
bool AutoPassParametersFlag;

// ����� ���������� ����������
// 0 - ������� ������
// 1 - ��������������� ������
int CalcMode;

protected: // ������
// ���������� ������� ����������
int CWidth,CHeight;

// �������� ������� ������� �� ������� �����������
int WorkIX, WorkIY, WorkIWidth, WorkIHeight;

// ������������� ������
// (��� ��������)
int* Template;

// ��������� ����� �������
long long TSumNorm;

// ����� �������� �������
long long TSumAverage;

// ������� ����������� � ����� �������� ������
unsigned char *Input;

// ����� �������� �������� �����������
// ��� ������� �������� ������� ����������
long long* ISumAverage;

// ��������� ������� ����������
double* Result;

// �������� � ������� ����������
double MaxResult,MinResult;

// ���������� ����������� �������
// x,y � �������� �����������
int MaxResX,MaxResY,MinResX,MinResY;

// ������ �������� ���� ���������� ���������� � �������� ���������
int* PassCStep;

protected: // ��������� ����������
// ����� ������� Template
int TemplateSize;

// ����� ������� ISumAverage
int ISumAverageSize;

// ����� ������� Result
int ResultSize;

// ����� ������� PassCStep;
int PassCStepSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NCC2D(void);
~NCC2D(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ������� �������� �����������
int GetIWidth(void) const;
int GetIHeight(void) const;
bool SetIRes(int width, int height);

// �������� ������� ������� �� ������� �����������
int GetWorkIX(void) const;
bool SetWorkIX(int value);

int GetWorkIY(void) const;
bool SetWorkIY(int value);

int GetWorkIWidth(void) const;
bool SetWorkIWidth(int value);

int GetWorkIHeight(void) const;
bool SetWorkIHeight(int value);

// ������� �������
int GetTWidth(void) const;
int GetTHeight(void) const;
bool SetTRes(int width, int height);

// ��� ����������
int GetCStep(void) const;
bool SetCStep(int step);

// ���� ���������� ��������� ��������
bool GetSubAverageFlag(void) const;
bool SetSubAverageFlag(bool value);

// ����� �������� ��������� ����������
int GetNumCalcPasses(void) const;
bool SetNumCalcPasses(int value);

// ���� ��������������� ���������� ���������� �������� ��������� ����������
bool GetAutoPassParametersFlag(void) const;
bool SetAutoPassParametersFlag(bool value);

// ����� ���������� ����������
// 0 - ������� ������
// 1 - ��������������� ������
int GetCalcMode(void) const;
bool SetCalcMode(int value);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������� ����������
int GetCWidth(void) const;
int GetCHeight(void) const;

// ������������� ������
// (��� ��������)
const int* GetTemplate(void) const;
bool SetTemplate(const int* buffer, int size);

// ��������� ����� �������
long long GetTSumNorm(void) const;
bool SetTSumNorm(long long value);

// ����� �������� �������
// ������� ����������� ��� �����/(TWidth*THeight)
long long GetTSumAverage(void) const;
bool SetTSumAverage(long long value);

// ������� ����������� � ����� �������� ������
const unsigned char *GetInputData(void) const;

// ����� �������� �������� �����������
// ��� ������� �������� ������� ����������
long long GetISumAverage(int x, int y) const;

// ��������� ������� ����������
double GetResult(int x, int y) const;

// �������� � ������� ����������
double GetMaxResult(void) const;
double GetMinResult(void) const;

// ���������� ����������� �������
// x,y � �������� �����������
int GetMaxResX(void) const;
int GetMaxResY(void) const;
int GetMinResX(void) const;
int GetMinResY(void) const;
// --------------------------

// --------------------------
// ������ �����
// --------------------------
// ������ ����� ������ �������
void SetTemplate(const unsigned char *buffer);

// ������� ����������� � ����� �������� ������
bool SetInput(unsigned char *input);
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

// ��������� ������ ������� ��� ������ �������� ���������� ����������
bool SimpleCalculate(void);

// ��������� ������ ������� ��� ������ ���������������� ���������� ����������
bool PassCalculate(void);
// --------------------------
};

}
//---------------------------------------------------------------------------
#endif

