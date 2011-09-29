/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MCORRELATION_CPP
#define MCORRELATION_CPP

#include <math.h>
#include <cstring>
#include "MCorrelation.h"

namespace RDK {


using namespace std;
//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NCC2D::NCC2D(void)
{
 Template=0;
 Input=0;
 ISumAverage=0;
 Result=0;

 // Длина массива Template
 TemplateSize=0;

 // Длина массива ISumAverage
 ISumAverageSize=0;

 // Длина массива Result
 ResultSize=0;

 // Массив значений шага вычисления корреляции в проходах уточнения
 PassCStep=0;

 // Длина массива PassCStep;
 PassCStepSize=0;

 CStep=1;
}

NCC2D::~NCC2D(void)
{
 if(Template)
  delete []Template;
 Template=0;

 if(ISumAverage)
  delete []ISumAverage;
 ISumAverage=0;

 if(Result)
  delete []Result;
 Result=0;

 if(PassCStep)
  delete []PassCStep;
 PassCStep=0;
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Размеры входного изображения
int NCC2D::GetIWidth(void) const
{
 return IWidth;
}

int NCC2D::GetIHeight(void) const
{
 return IHeight;
}

bool NCC2D::SetIRes(int width, int height)
{
 if(width == IWidth && height == IHeight)
  return true;

 Ready=false;

 IWidth=width;
 IHeight=height;

// CWidth=(IWidth-TWidth)/CStep+1;
// CHeight=(IHeight-THeight)/CStep+1;

 return true;
}

// Габариты рабочей области во входном изображении
int NCC2D::GetWorkIX(void) const
{
 return WorkIX;
}

bool NCC2D::SetWorkIX(int value)
{
 if(WorkIX == value)
  return true;

 WorkIX=value;

 return true;
}

int NCC2D::GetWorkIY(void) const
{
 return WorkIY;
}

bool NCC2D::SetWorkIY(int value)
{
 if(WorkIY == value)
  return true;

 WorkIY=value;

 return true;
}

int NCC2D::GetWorkIWidth(void) const
{
 return WorkIWidth;
}

bool NCC2D::SetWorkIWidth(int value)
{
 if(WorkIWidth == value)
  return true;

 WorkIWidth=value;
 CWidth=(WorkIWidth-TWidth)/CStep+1;
 CHeight=(WorkIHeight-THeight)/CStep+1;

 return true;
}

int NCC2D::GetWorkIHeight(void) const
{
 return WorkIHeight;
}

bool NCC2D::SetWorkIHeight(int value)
{
 if(WorkIHeight == value)
  return true;

 WorkIHeight=value;
 CWidth=(WorkIWidth-TWidth)/CStep+1;
 CHeight=(WorkIHeight-THeight)/CStep+1;

 return true;
}

// Размеры шаблона
int NCC2D::GetTWidth(void) const
{
 return TWidth;
}

int NCC2D::GetTHeight(void) const
{
 return THeight;
}

bool NCC2D::SetTRes(int width, int height)
{
 if(width == TWidth && height == THeight)
  return true;

 Ready=false;

 TWidth=width;
 THeight=height;
 CWidth=(WorkIWidth-TWidth)/CStep+1;
 CHeight=(WorkIHeight-THeight)/CStep+1;
// CWidth=(IWidth-TWidth)/CStep+1;
// CHeight=(IHeight-THeight)/CStep+1;

 return true;
}

// Шаг корреляции
int NCC2D::GetCStep(void) const
{
 return CStep;
}

bool NCC2D::SetCStep(int step)
{
 if(step == CStep)
  return true;

// Ready=false;

 CStep=step;
 CWidth=(WorkIWidth-TWidth)/CStep+1;
 CHeight=(WorkIHeight-THeight)/CStep+1;
// CWidth=(IWidth-TWidth)/CStep+1;
// CHeight=(IHeight-THeight)/CStep+1;
 return true;
}

// Флаг разрешения вычитания среднего
bool NCC2D::GetSubAverageFlag(void) const
{
 return SubAverageFlag;
}

bool NCC2D::SetSubAverageFlag(bool value)
{
 if(value == SubAverageFlag)
  return true;

 Ready=false;

 SubAverageFlag=value;

 return true;
}

// Число проходов уточнения корреляции
int NCC2D::GetNumCalcPasses(void) const
{
 return NumCalcPasses;
}

bool NCC2D::SetNumCalcPasses(int value)
{
 if(NumCalcPasses == value)
  return true;

 if(value < 1)
  return false;

 NumCalcPasses=value;
 Ready=false;
 return true;
}

// Флаг автоматического вычисления параметров проходов уточнения корреляции
bool NCC2D::GetAutoPassParametersFlag(void) const
{
 return AutoPassParametersFlag;
}

bool NCC2D::SetAutoPassParametersFlag(bool value)
{
 if(AutoPassParametersFlag == value)
  return true;

 AutoPassParametersFlag=value;
 Ready=false;
 return true;
}

// Режим вычисления корреляции
// 0 - простой расчет
// 1 - многопроходовый расчет
int NCC2D::GetCalcMode(void) const
{
 return CalcMode;
}

bool NCC2D::SetCalcMode(int value)
{
 if(CalcMode == value)
  return true;

 if(value <0 || value > 1)
  return false;

 CalcMode=value;
 return true;
}


// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Разрешение матрицы корреляций
int NCC2D::GetCWidth(void) const
{
 return CWidth;
}

int NCC2D::GetCHeight(void) const
{
 return CHeight;
}

// Нормированный шаблон
// (без среднего)
const int* NCC2D::GetTemplate(void) const
{
 return &Template[0];
}

bool NCC2D::SetTemplate(const int* buffer, int size)
{
 if(size != static_cast<int>(TWidth*THeight))
  return false;

 if(!Build())
  return false;

 memcpy(&Template[0],buffer,size*sizeof(int));
 return true;
}


// Суммарная норма шаблона
long long NCC2D::GetTSumNorm(void) const
{
 return TSumNorm;
}

bool NCC2D::SetTSumNorm(long long value)
{
 if(value <= 0 || !Build())
  return false;

 TSumNorm=value;
 return true;
}


// Сумма среднего шаблона
// Среднее вычисляется как сумма/(TWidth*THeight)
long long NCC2D::GetTSumAverage(void) const
{
 return TSumAverage;
}

bool NCC2D::SetTSumAverage(long long value)
{
 if(value <= 0 || !Build())
  return false;

 TSumAverage=value;
 return true;
}

// Входное изображение в форме градаций серого
const unsigned char* NCC2D::GetInputData(void) const
{
 return Input;
}

// Сумма среднего входного изображения
// для каждого элемента матрицы корреляций
long long NCC2D::GetISumAverage(int x, int y) const
{
 return ISumAverage[y*CWidth+x];
}

// Результат расчета корреляций
double NCC2D::GetResult(int x, int y) const
{
 return Result[y*CWidth+x];
}

// Максимум и минимум корреляции
double NCC2D::GetMaxResult(void) const
{
 return MaxResult;
}

double NCC2D::GetMinResult(void) const
{
 return MinResult;
}

// Координаты результатов расчета
// x,y в исходном изображении
int NCC2D::GetMaxResX(void) const
{
 return MaxResX;
}

int NCC2D::GetMaxResY(void) const
{
 return MaxResY;
}

int NCC2D::GetMinResX(void) const
{
 return MinResX;
}

int NCC2D::GetMinResY(void) const
{
 return MinResY;
}
// --------------------------

// --------------------------
// Методы счета
// --------------------------
// Задает новые данные шаблона
void NCC2D::SetTemplate(const unsigned char *buffer)
{
 if(!Build())
  return;

 // Считаем параметры шаблона

 // Считаем среднее
 TSumAverage=0;
 TSumNorm=0;
 int size=TWidth*THeight;
 const unsigned char *pbuf=buffer;

 if(SubAverageFlag)
 {
  for(int i=0;i<size;i++,pbuf++)
  {
   TSumAverage+=*pbuf;
  }
  pbuf=buffer;
  for(int i=0;i<size;i++,pbuf++)
  {
   long long sub=*pbuf*size-TSumAverage;
   Template[i]=static_cast<int>(sub);
   TSumNorm+=sub*sub;
  }
 }
 else
 {
  pbuf=buffer;
  for(int i=0;i<size;i++,pbuf++)
  {
   int sub=*pbuf;
   TSumNorm+=sub*sub;
   Template[i]=sub;
  }
 }
}

// Входное изображение в форме градаций серого
bool NCC2D::SetInput(unsigned char *input)
{
 if(Input == input)
  return true;

 if(!input)
  return false;

 Input=input;

 return true;
}
// --------------------------




// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NCC2D::ADefault(void)
{
 SetCStep(1);
 SetNumCalcPasses(1);
 SetAutoPassParametersFlag(true);
 SetCalcMode(0);
 SetIRes(1,1);
 SetTRes(1,1);
 SetSubAverageFlag(true);
 SetWorkIX(0);
 SetWorkIY(0);
 SetWorkIWidth(IWidth);
 SetWorkIHeight(IHeight);

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NCC2D::ABuild(void)
{
 if(IWidth<TWidth || IHeight<THeight || CStep <=0 ||
    IWidth <= 0 || IHeight <= 0 || TWidth <= 0 || THeight <= 0)
  return false;

 // Разрешение матрицы корреляций
 int fullcwidth=(IWidth-TWidth)+1;
 int fullcheight=(IHeight-THeight)+1;

 // Габариты рабочей области во входном изображении
 //WorkIX, WorkIY, WorkIWidth, WorkIHeight;
 Input=0;

 // Нормированный шаблон
 // (без среднего)
 if(!Template || TemplateSize<TWidth*THeight)
 {
  delete []Template;
  Template=new int[TWidth*THeight];
  TemplateSize=TWidth*THeight;
 }

 // Суммарная норма шаблона
 TSumNorm=0;

 // Сумма среднего шаблона
 // Среднее вычисляется как сумма/(TWidth*THeight)
 TSumAverage=0;

 // Сумма среднего входного изображения
 // для каждого элемента матрицы корреляций
 if(!ISumAverage || ISumAverageSize<fullcwidth*fullcheight)
  delete []ISumAverage;
 if(SubAverageFlag)
 {
  if(ISumAverageSize<fullcwidth*fullcheight)
  {
   ISumAverage=new long long[fullcwidth*fullcheight];
//   memset(ISumAverage,0,CWidth*CHeight*sizeof(long long));//.assign(CWidth*CHeight,0);
   ISumAverageSize=fullcwidth*fullcheight;
  }
 }
 else
  ISumAverage=0;

 // Результат расчета корреляций
 if(!Result || ResultSize<fullcwidth*fullcheight)
 {
  delete []Result;
  Result=new double[fullcwidth*fullcheight];
  ResultSize=fullcwidth*fullcheight;
 }
// memset(Result,0,CWidth*CHeight*sizeof(double));
// Result.assign(CWidth*CHeight,0);

 MaxResult=0;

// for(size_t i=0;i<ISumAverage.size();i++)
//  ISumAverage[i].assign(IWidth*IHeight,0);

 if(!PassCStep || PassCStepSize<NumCalcPasses)
 {
  delete []PassCStep;
  PassCStep=new int[NumCalcPasses];
  PassCStepSize=NumCalcPasses;
 }

 if(AutoPassParametersFlag)
 {
  for(int i=0;i<NumCalcPasses;i++)
   PassCStep[i]=1<<i;
 }
 else
  memset(PassCStep,0,NumCalcPasses*sizeof(int));

 return true;
}

// Сброс процесса счета.
bool NCC2D::AReset(void)
{

 return true;
}

// Выполняет расчет этого объекта
bool NCC2D::ACalculate(void)
{
 if(!Input)
  return false;

 switch(CalcMode)
 {
 case 0:
  return SimpleCalculate();
 case 1:
  return PassCalculate();
 }
 return false;
}

// Выполняет расчет объекта для случая простого вычисления корреляции
bool NCC2D::SimpleCalculate(void)
{
 int k;

 int size=TWidth*THeight;

 // Считаем среднее входного изображения для всех u,v
 k=0;
 int sum=0;
 long long res=0;


 unsigned char *pInput=Input;

 if(SubAverageFlag)
 {
  ISumAverage[0]=0;
  for(int n=0;n<THeight;n++)
  {
   pInput=Input+n*IWidth;
   for(int m=0;m<TWidth;m++,pInput++)
    sum+=*pInput;
  }

  ISumAverage[0]=sum;

  ++k;
  for(int j=0;j<CHeight;j++)
  {
   if(j%2 == 0) // Идем вправо
   {
    for(int i=1;i<CWidth;i++)
    {
     pInput=Input+j*CStep*IWidth+i*CStep+TWidth-1;

     for(int n=0;n<THeight;n++)
     {
      sum+=int(*pInput);
      pInput+=IWidth;
     }

     pInput=Input+j*CStep*IWidth+(i-1)*CStep;
     for(int n=0;n<THeight;n++)
     {
      sum-=int(*pInput);
      pInput+=IWidth;
     }

     ISumAverage[k]=sum;
     ++k;
    }
   }
   else // Идем влево
   {
    for(int i=CWidth-2;i>=0;i--)
    {
     pInput=Input+j*CStep*IWidth+i*CStep;
     for(int n=0;n<THeight;n++)
     {
      sum+=int(*pInput);
      pInput+=IWidth;
     }

     pInput=Input+j*CStep*IWidth+(i+1)*CStep+TWidth-1;
     for(int n=0;n<THeight;n++)
     {
      sum-=int(*pInput);
      pInput+=IWidth;
     }

     ISumAverage[k]=sum;
     --k;
    }
   }

   // Здесь смещаемся на 1 строку
   if(j < CHeight-1)
   {
    if(j%2 == 0)// Если шли вправо
    {
     pInput=Input+((j+1)*CStep+THeight-1)*IWidth+(TWidth-1)*CStep;
     for(int m=0;m<TWidth;m++,pInput++)
      sum+=int(*pInput);

     pInput=Input+j*CStep*IWidth+(TWidth-1)*CStep;
     for(int m=0;m<TWidth;m++,pInput++)
      sum-=int(*pInput);
     k+=CWidth-1;
     ISumAverage[k]=sum;
     --k;
    }
    else // Если шли влево
    {
     pInput=Input+((j+1)*CStep+THeight-1)*IWidth;
     for(int m=0;m<TWidth;m++,pInput++)
      sum+=int(*pInput);

     pInput=Input+j*CStep*IWidth;
     for(int m=0;m<TWidth;m++,pInput++)
      sum-=int(*pInput);

     k+=CWidth+1;
     ISumAverage[k]=sum;
     ++k;
    }
   }
  }

 // Считаем корреляцию
 MaxResult=-1;
 MinResult=1;
 MaxResX=MaxResY=MinResX=MinResY-1;

 k=0;
 for(int j=0;j<CHeight;j++)
  for(int i=0;i<CWidth;i++)
  {
   Result[k]=-1;
   long long iSumNorm=0;
   long long res=0;
   long long isumaverage;
   if(SubAverageFlag)
    isumaverage=ISumAverage[k];
   else
    isumaverage=0;

   int* ttemplate=&Template[0];


   unsigned char *pInput=Input+j*CStep*IWidth+i*CStep;
   for(int n=j*CStep;n<THeight+j*CStep;n++)
   {
    for(int m=i*CStep;m<TWidth+i*CStep;m++,pInput++)
    {
     long long sub=(*pInput)*size-isumaverage;
     res+=sub*(*ttemplate);
     iSumNorm+=sub*sub;

     ++ttemplate;
    }
    pInput+=IWidth-TWidth;
   }

   double sq1=double(iSumNorm),sq2=double(TSumNorm);
   if(sq1 != 0 && sq2 != 0)
    Result[k]=double(res)/sqrtl(sq1*sq2);
   else
    Result[k]=1;
   if(MaxResult<Result[k])
   {
    MaxResult=Result[k];
    MaxResX=i*CStep;
    MaxResY=j*CStep;
   }
   if(MinResult>Result[k])
   {
    MinResult=Result[k];
    MinResX=i*CStep;
    MinResY=j*CStep;
   }
   ++k;
  }

  return true;
 }

 // Считаем корреляцию
 MaxResult=-1;
 MinResult=1;
 MaxResX=MaxResY=MinResX=MinResY-1;

 k=0;
 for(int j=0;j<CHeight;j++)
  for(int i=0;i<CWidth;i++)
  {
   Result[k]=-1;
   int iSumNorm=0;
//   int res=0;
   int diffwidth=IWidth-TWidth;

   int* ttemplate=&Template[0];


   unsigned char *pInput=Input+WorkIY*IWidth+j*CStep*IWidth+i*CStep+WorkIX;
   for(int n=0;n<THeight;++n)
   {
    for(int m=0;m<TWidth;++m,++pInput,++ttemplate)
    {
     iSumNorm+=int(*pInput)*int(*pInput);
     res+=int(*pInput)*(*ttemplate);
    }
    pInput+=diffwidth;
   }

   double sq1=double(iSumNorm),sq2=double(TSumNorm);
   if(sq1 != 0 && sq2 != 0)
   {
    Result[k]=double(res)/sqrtl(fabs(sq1*sq2));
   }
   else
    Result[k]=1;
   if(MaxResult<Result[k])
   {
    MaxResult=Result[k];
    MaxResX=i*CStep+WorkIX;
    MaxResY=j*CStep+WorkIY;
   }
   if(MinResult>Result[k])
   {
    MinResult=Result[k];
    MinResX=i*CStep+WorkIX;
    MinResY=j*CStep+WorkIY;
   }
   ++k;
  }

 return true;
}

// Выполняет расчет объекта для случая многопроходового вычисления корреляции
bool NCC2D::PassCalculate(void)
{
 int tx=0,ty=0; // Временные координаты максимума на данном шаге
 double res=-1;

 SetWorkIX(0);
 SetWorkIY(0);
 SetWorkIWidth(IWidth);
 SetWorkIHeight(IHeight);

 for(int n=NumCalcPasses-1;n>=0;n--)
 {
  if(!SetCStep(PassCStep[n]))
   return false;

  if(!SimpleCalculate())
   return false;

  tx=MaxResX;
  ty=MaxResY;
  res=MaxResult;

  int coord=tx-PassCStep[n]+1;
  if(coord<0)
   coord=0;
  SetWorkIX(coord);

  coord=ty-PassCStep[n]+1;
  if(coord<0)
   coord=0;
  SetWorkIY(coord);

  coord=TWidth+PassCStep[n]-1;
  if(coord>=IWidth)
   coord=IWidth-WorkIX;
  SetWorkIWidth(coord);

  coord=THeight+PassCStep[n]-1;
  if(coord>=IHeight)
   coord=IHeight-WorkIY;
  SetWorkIHeight(coord);
 }

 MaxResult=res;
 MaxResX=tx;
 MaxResY=ty;
 return true;
}
// --------------------------


//---------------------------------------------------------------------------

}

#endif
