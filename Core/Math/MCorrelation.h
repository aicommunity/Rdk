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

// Нормированная кросс-корреляция
class RDK_LIB_TYPE NCC2D: public UModule
{
protected: // Параметры
// Размеры входного изображения
int IWidth,IHeight;

// Размеры шаблона
int TWidth,THeight;

// Шаг корреляции
int CStep;

// Флаг разрешения вычитания среднего
bool SubAverageFlag;

// Число проходов уточнения корреляции
int NumCalcPasses;

// Флаг автоматического вычисления параметров проходов уточнения корреляции
bool AutoPassParametersFlag;

// Режим вычисления корреляции
// 0 - простой расчет
// 1 - многопроходовый расчет
int CalcMode;

protected: // Данные
// Разрешение матрицы корреляций
int CWidth,CHeight;

// Габариты рабочей области во входном изображении
int WorkIX, WorkIY, WorkIWidth, WorkIHeight;

// Нормированный шаблон
// (без среднего)
int* Template;

// Суммарная норма шаблона
long long TSumNorm;

// Сумма среднего шаблона
long long TSumAverage;

// Входное изображение в форме градаций серого
unsigned char *Input;

// Сумма среднего входного изображения
// для каждого элемента матрицы корреляций
long long* ISumAverage;

// Результат расчета корреляций
double* Result;

// Максимум и минимум корреляции
double MaxResult,MinResult;

// Координаты результатов расчета
// x,y в исходном изображении
int MaxResX,MaxResY,MinResX,MinResY;

// Массив значений шага вычисления корреляции в проходах уточнения
int* PassCStep;

protected: // Временные переменные
// Длина массива Template
int TemplateSize;

// Длина массива ISumAverage
int ISumAverageSize;

// Длина массива Result
int ResultSize;

// Длина массива PassCStep;
int PassCStepSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NCC2D(void);
~NCC2D(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Размеры входного изображения
int GetIWidth(void) const;
int GetIHeight(void) const;
bool SetIRes(int width, int height);

// Габариты рабочей области во входном изображении
int GetWorkIX(void) const;
bool SetWorkIX(int value);

int GetWorkIY(void) const;
bool SetWorkIY(int value);

int GetWorkIWidth(void) const;
bool SetWorkIWidth(int value);

int GetWorkIHeight(void) const;
bool SetWorkIHeight(int value);

// Размеры шаблона
int GetTWidth(void) const;
int GetTHeight(void) const;
bool SetTRes(int width, int height);

// Шаг корреляции
int GetCStep(void) const;
bool SetCStep(int step);

// Флаг разрешения вычитания среднего
bool GetSubAverageFlag(void) const;
bool SetSubAverageFlag(bool value);

// Число проходов уточнения корреляции
int GetNumCalcPasses(void) const;
bool SetNumCalcPasses(int value);

// Флаг автоматического вычисления параметров проходов уточнения корреляции
bool GetAutoPassParametersFlag(void) const;
bool SetAutoPassParametersFlag(bool value);

// Режим вычисления корреляции
// 0 - простой расчет
// 1 - многопроходовый расчет
int GetCalcMode(void) const;
bool SetCalcMode(int value);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Разрешение матрицы корреляций
int GetCWidth(void) const;
int GetCHeight(void) const;

// Нормированный шаблон
// (без среднего)
const int* GetTemplate(void) const;
bool SetTemplate(const int* buffer, int size);

// Суммарная норма шаблона
long long GetTSumNorm(void) const;
bool SetTSumNorm(long long value);

// Сумма среднего шаблона
// Среднее вычисляется как сумма/(TWidth*THeight)
long long GetTSumAverage(void) const;
bool SetTSumAverage(long long value);

// Входное изображение в форме градаций серого
const unsigned char *GetInputData(void) const;

// Сумма среднего входного изображения
// для каждого элемента матрицы корреляций
long long GetISumAverage(int x, int y) const;

// Результат расчета корреляций
double GetResult(int x, int y) const;

// Максимум и минимум корреляции
double GetMaxResult(void) const;
double GetMinResult(void) const;

// Координаты результатов расчета
// x,y в исходном изображении
int GetMaxResX(void) const;
int GetMaxResY(void) const;
int GetMinResX(void) const;
int GetMinResY(void) const;
// --------------------------

// --------------------------
// Методы счета
// --------------------------
// Задает новые данные шаблона
void SetTemplate(const unsigned char *buffer);

// Входное изображение в форме градаций серого
bool SetInput(unsigned char *input);
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета.
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);

// Выполняет расчет объекта для случая простого вычисления корреляции
bool SimpleCalculate(void);

// Выполняет расчет объекта для случая многопроходового вычисления корреляции
bool PassCalculate(void);
// --------------------------
};

}
//---------------------------------------------------------------------------
#endif

