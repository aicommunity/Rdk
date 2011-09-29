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
// Конструкторы и деструкторы
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
// Конструкторы и деструкторы
// ---------------------
UBAbstract::UBAbstract(void)
{
 // Таблица соответствий входов
 InputTable=0;

 // Таблица соответствий выходов
 OutputTable=0;

 // Число входов фильтра
 NumInputs=0;
 SetNumInputs(1);

 // Число выходов фильтра
 NumOutputs=0;
 SetNumOutputs(1);

 NumTransitInputs=0;
 TransitTable=0;

 // Индекс начального параметра этого класса
 // (исключая родительские параметры)
// FirstParamIndex=-1;

 // Индекс конечного параметра этого класса
 // (исключая родительские параметры)
// LastParamIndex=-1;
}

UBAbstract::~UBAbstract(void)
{
 // Таблица соответствий входов
 if(InputTable)
 {
  delete []InputTable;
  InputTable=0;
 }

 // Таблица соответствий выходов
 if(OutputTable)
 {
  delete []OutputTable;
  OutputTable=0;
 }


 // Таблица соответствий выходов
 if(TransitTable)
 {
  delete []TransitTable;
  TransitTable=0;
 }

 // Число транзитных входов
 NumTransitInputs=0;

 // Число входов фильтра
 NumInputs=0;

 // Число выходов фильтра
 NumOutputs=0;
}
// ---------------------

// ---------------------
// Методы управления параметрами
// ---------------------
// Флаг, разрешающий использовать массив внутренних входных данных
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

// Флаг, разрешающий использовать массив внутренних выходных данных
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
// Число входов фильтра
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
// Число выходов фильтра
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

// Число транзитных входов
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

// Цветовая модель входа и выхода
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
// Методы управления данными
// ---------------------
// Возвращает указатель на таблицу входов
const int* UBAbstract::GetInputTable(void) const
{
 return InputTable;
}

// Возвращает указатель на таблицу выходов
const int* UBAbstract::GetOutputTable(void) const
{
 return OutputTable;
}

// Возвращает указатель на таблицу транзитных входов
const UBIOPair* UBAbstract::GetTransitTable(void) const
{
 return TransitTable;
}

// Предоставлет доступ к элементу таблицы соответствий входов
int& UBAbstract::InputTableValue(int index)
{
 Build();
 return InputTable[index];
}

// Предоставлет доступ к элементу таблицы соответствий выходов
int& UBAbstract::OutputTableValue(int index)
{
 Build();
 return OutputTable[index];
}

// Предоставлет доступ к элементу таблицы транзитных входов
UBIOPair& UBAbstract::TransitTableValue(int index)
{
 return TransitTable[index];
}

// Устаналивает таблицу соответствий входов по умолчанию
void UBAbstract::SetDefaultInputTable(void)
{
 for(int i=0;i<NumInputs;i++)
  InputTable[i]=i;
}

// Устаналивает таблицу соответствий выходов по умолчанию
void UBAbstract::SetDefaultOutputTable(void)
{
 for(int i=0;i<NumOutputs;i++)
  OutputTable[i]=i;
}
// ---------------------

// ---------------------
// Методы счета
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


 // Пробрасываем оставшиеся входы в выходы
 // Проходим по таблице выходов, и пробрасываем в ячейки с отрицательным
 // индексом соответствующий по модулю вход
 for(int i=0;i<NumTransitInputs;i++)
 {
  *output[TransitTable[i].Output]=*input[TransitTable[i].Input];
 }

 return true;
}

// Возвращает массив входных изображений
UPBitmapVector& UBAbstract::GetInputs(void)
{
 return Inputs;
}

// Возвращает массив выходных изображений
UPBitmapVector& UBAbstract::GetOutputs(void)
{
 return Outputs;
}

// Устанавливает массив входных изображений
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

// Устанавливает массив выходных изображений
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
// Коммуникационные методы
// ----------------------
// Выполняет действия после физически установленой связи
bool UBAbstract::AConnectToItem(UAItem *na, int i_index, int c_index)
{
 UBAbstract* filter=dynamic_cast<UBAbstract*>(na);

 if(!InternalInputsFlag)
  Inputs[c_index]=filter->Outputs[i_index];

 return AFConnectToItem(filter,i_index,c_index);
}

// Выполняет действия после физически разорваной связи
void UBAbstract::ADisconnectFromItem(UAItem *na, int i_index, int c_index)
{
 UBAbstract* filter=dynamic_cast<UBAbstract*>(na);

 AFDisconnectFromItem(filter,i_index,c_index);
}
// ----------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UBAbstract::ADefault(void)
{
 SetInternalOutputsFlag(true);
 SetInternalInputsFlag(true);
 return AFDefault();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
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

// Сброс процесса счета без потери настроек
bool UBAbstract::AReset(void)
{
 return AFReset();
}

// Выполняет расчет этого объекта
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
// Скрытые методы управления счетом фильтров
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UBAbstract::AFDefault(void)
{
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UBAbstract::AFBuild(void)
{
 return true;
}

// Сброс процесса счета без потери настроек
bool UBAbstract::AFReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool UBAbstract::AFCalculate(void)
{
 return true;
}
// --------------------------


// ----------------------
// Коммуникационные методы фильтров
// ----------------------
// Выполняет действия после физически установленой связи
bool UBAbstract::AFConnectToItem(UBAbstract *na, int i_index, int c_index)
{
 return true;
}

// Выполняет действия после физически разорваной связи
void UBAbstract::AFDisconnectFromItem(UBAbstract *na, int i_index, int c_index)
{
 return;
}
// ----------------------

}
#endif



