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

// Базовый класс для всех классов обработки изображений
class UBAbstract: public UANet
{
protected: // Параметры
// Флаг, разрешающий использовать массив внутренних входных данных
bool InternalInputsFlag;

// Флаг, разрешающий использовать массив внутренних выходных данных
bool InternalOutputsFlag;

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
