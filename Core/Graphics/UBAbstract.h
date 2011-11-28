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

// Структура пар индексов
struct UBIOPair
{
int Input;
int Output;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBIOPair(void);
UBIOPair(int input, int output);
// --------------------------
};

// Базовый класс для всех классов обработки изображений
class UBAbstract: public UANet//UAComponent
{
protected: // Параметры
// Флаг, разрешающий использовать массив внутренних входных данных
bool InternalInputsFlag;

// Флаг, разрешающий использовать массив внутренних выходных данных
bool InternalOutputsFlag;

// Число входов фильтра
//int NumInputs;

// Число выходов фильтра
//int NumOutputs;

// Число транзитных входов фильтра
int NumTransitInputs;

protected: // Данные
// Используемый массив входных изображений
UPBitmapVector Inputs;

// Используемый массив выходных изображений
UPBitmapVector Outputs;

// Физически существующий массив выходных изображений
// Используется по умолчанию, но может быть отключен
UBitmapVector InternalOutputs;

// Физически существующий массив входных изображений
// Не используется по умолчанию, но может быть включен
UBitmapVector InternalInputs;

// Цветовая модель входа и выхода
std::vector<UBMColorModel> InputColorModel, OutputColorModel;

// Таблица соответствий входов
int *InputTable;

// Таблица соответствий выходов
int *OutputTable;

// Таблица соответствий транзитных входов
// указывает какой номер входа нужно транзитом отправить в выход c индексом
UBIOPair *TransitTable;

protected: // Сериализация
// Индекс начального параметра этого класса
// (исключая родительские параметры)
//int FirstParamIndex;

// Индекс конечного параметра этого класса
// (исключая родительские параметры)
//int LastParamIndex;

public: // Методы
// ---------------------
// Конструкторы и деструкторы
// ---------------------
UBAbstract(void);
virtual ~UBAbstract(void);
// ---------------------

// ---------------------
// Методы управления параметрами
// ---------------------
// Флаг, разрешающий использовать массив внутренних входных данных
bool GetInternalInputsFlag(void) const;
bool SetInternalInputsFlag(bool value);

// Флаг, разрешающий использовать массив внутренних выходных данных
bool GetInternalOutputsFlag(void) const;
bool SetInternalOutputsFlag(bool value);

// Число входов фильтра
//int GetNumInputs(void) const;
//bool SetNumInputs(int value);

// Число выходов фильтра
//int GetNumOutputs(void) const;
//bool SetNumOutputs(int value);

// Число транзитных входов
int GetNumTransitInputs(void) const;
bool SetNumTransitInputs(int value);

// Цветовая модель входа и выхода
UBMColorModel GetInputColorModel(int index) const;
bool SetInputColorModel(int index, UBMColorModel cmodel);

UBMColorModel GetOutputColorModel(int index) const;
bool SetOutputColorModel(int index, UBMColorModel cmodel);
// ---------------------

// ---------------------
// Методы управления данными
// ---------------------
// Возвращает указатель на таблицу входов
const int* GetInputTable(void) const;

// Возвращает указатель на таблицу выходов
const int* GetOutputTable(void) const;

// Возвращает указатель на таблицу транзитных входов
const UBIOPair* GetTransitTable(void) const;

// Предоставлет доступ к элементу таблицы соответствий входов
int& InputTableValue(int index);

// Предоставлет доступ к элементу таблицы соответствий выходов
int& OutputTableValue(int index);

// Предоставлет доступ к элементу таблицы транзитных входов
UBIOPair& TransitTableValue(int index);

// Устаналивает таблицу соответствий входов по умолчанию
void SetDefaultInputTable(void);

// Устаналивает таблицу соответствий выходов по умолчанию
void SetDefaultOutputTable(void);

// Возвращает массив входных изображений
UPBitmapVector& GetInputs(void);

// Возвращает массив выходных изображений
UPBitmapVector& GetOutputs(void);

// Устанавливает массив входных изображений
bool SetInputs(PUBitmap* bitmaps);
bool SetInput(int index, PUBitmap bitmap);

// Устанавливает массив выходных изображений
bool SetOutputs(PUBitmap* bitmaps);
bool SetOutput(int index, PUBitmap bitmap);
// ---------------------

// ---------------------
// Методы счета
// ---------------------
// Создание новой копии этого объекта
virtual UBAbstract* New(void)=0;

// Методы счета в составе конвеера
// Pipeline calculation methods
virtual bool PLACalculate(UBitmap **input, UBitmap **output, int num_inputs=1, int num_outputs=1)=0;
bool PLCalculate(UBitmap **input, UBitmap **output, int num_inputs=1, int num_outputs=1);
// ---------------------

// ----------------------
// Коммуникационные методы
// ----------------------
protected:
// Выполняет действия после физически установленой связи
virtual bool AConnectToItem(UEPtr<UAItem> na, int i_index, int c_index);

// Выполняет действия после физически разорваной связи
virtual void ADisconnectFromItem(UEPtr<UAItem> na, int i_index, int c_index);
// ----------------------

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

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом фильтров
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool AFDefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool AFBuild(void);

// Сброс процесса счета без потери настроек
virtual bool AFReset(void);

// Выполняет расчет этого объекта
virtual bool AFCalculate(void);
// --------------------------

// ----------------------
// Коммуникационные методы фильтров
// ----------------------
protected:
// Выполняет действия после физически установленой связи
virtual bool AFConnectToItem(UEPtr<UBAbstract> na, int i_index, int c_index);

// Выполняет действия после физически разорваной связи
virtual void AFDisconnectFromItem(UEPtr<UBAbstract> na, int i_index, int c_index);
// ----------------------

};


}

#endif
