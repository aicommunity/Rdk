/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBPIPELINE_H
#define UBPIPELINE_H

#include "UBAModel.h"

namespace RDK {

// Класс конвеера обработки  изображений
class UBPipeline: public UBAModel
{
protected: // Типы данных
typedef UBAbstract* PUBAbstract;
protected: // Данные

protected: // Временные переменные
// Массив промежуточных изображений
UBitmap *InputBuffer,*OutputBuffer;

// Размер массива промежуточных изображений
int InputBufferSize;
int OutputBufferSize;

// Реальный размер массива
int InputBufferRealSize;
int OutputBufferRealSize;

// Промежуточные массивы входов и выходов
PUBitmap *InputArray;
PUBitmap *OutputArray;

// Размеры промежуточных массивов входов и выходов
int InputArraySize, OutputArraySize;

// Реальные размеры промежуточных массивов входов и выходов
int InputArrayRealSize, OutputArrayRealSize;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBPipeline(void);
virtual ~UBPipeline(void);
// --------------------------

// ---------------------
// Методы управления конвеером
// ---------------------
// ---------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
virtual bool CheckComponentType(UEPtr<UAContainer> comp) const;
// --------------------------

// ---------------------
// Методы счета
// ---------------------
// Создание новой копии этого объекта
virtual UBPipeline* New(void);

//virtual bool Calculate(void);

virtual bool PLACalculate(UBitmap **input, UBitmap **output, int num_inputs=1, int num_outputs=1);
// ---------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
protected:
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
// Может быть передан указатель на локальную переменную
virtual bool AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual bool ADelComponent(UEPtr<UAContainer> comp);
// --------------------------

// ---------------------
// Скрытые методы счета
// ---------------------
protected:
// Расширяет размер буфера изображений на заданную величину
void ExpandInputBuffer(int size);
void ExpandOutputBuffer(int size);
void ExpandImageBuffer(UBitmap* &buffer, int &currentsize, int &realsize, int newsize);

// Расширяет размер промежуточного массива входов
void ExpandInputArray(int size);
void ExpandArray(PUBitmap* &array, int &currentsize, int &realsize, int newsize);

// Расширяет размер промежуточного массива выходов
void ExpandOutputArray(int size);

// Расчитывает и устанавливается для конвеера минимально необходимое числов входов и выходов
// если размер входа и выхода конвеера недостаточен
void CalcNumIOs(void);
// ---------------------

// ---------------------
// Операторы
// ---------------------
public:
UBAbstract* operator [] (int index);
// ---------------------

};


// Класс параллельного конвеера
class UBParallelPipeline: public UBPipeline
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBParallelPipeline(void);
virtual ~UBParallelPipeline(void);
// --------------------------

// ---------------------
// Методы счета
// ---------------------
// Создание новой копии этого объекта
virtual UBParallelPipeline* New(void);

virtual bool PLACalculate(UBitmap **input, UBitmap **output, int num_inputs=1, int num_outputs=1);
// ---------------------
};

}
#endif
