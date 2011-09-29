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
protected: // Параметры
// Цветовая модель расчетов по умолчанию
UBMColorModel DefaultColorModel;

// Число входных изображений
int NumInputImages;

// Число выходных изображений
int NumOutputImages;

protected: // Состояния

protected: // Данные
// Входной массив изображений
mutable UBitmapVector InputImages;

// Выходной массив изображений
mutable UBitmapVector OutputImages;

// Идентификатор источника данных в модели, связанного с входным массивом изображений
UId ModelInputId;

// Идентификатор приемника данных в модели, связанного с выходным массивом изображений
UId ModelOutputId;

protected: // Временные переменные
// Временное изображения для операция преобразования
UBitmap TempConvertBitmap;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBAEnvironment(void);
virtual ~UBAEnvironment(void);
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Возвращает указатель на хранилище
virtual UBAStorage* GetStorage(void);

// Возвращает указатель на модель
virtual UBAModel* GetModel(void);

// Создает новую модель из хранилища по id класса
virtual bool CreateModel(const UId& classid);

// Создает новую модель из хранилища по имени класса
virtual bool CreateModel(const NameT& classname);

// Уничтожает текущую модель
virtual bool DestroyModel(void);

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
virtual bool BuildStorage(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
public:
// Цветовая модель расчетов по умолчанию
UBMColorModel GetDefaultColorModel(void) const;
bool SetDefaultColorModel(UBMColorModel model);

// Идентификатор источника данных в модели, связанного с входным массивом изображений
UId GetModelInputId(void) const;
bool SetModelInputId(UId value);

// Идентификатор приемника данных в модели, связанного с выходным массивом изображений
UId GetModelOutputId(void) const;
bool SetModelOutputId(UId value);
// --------------------------

// --------------------------
// Методы управления состояниями
// --------------------------
public:
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Число входных изображений
int GetNumInputImages(void) const;
bool SetNumInputImages(int value);

// Число выходных изображений
int GetNumOutputImages(void) const;
bool SetNumOutputImages(int value);

// Разрешение входного изображения
int GetInputImageWidth(int i) const;
int GetInputImageHeight(int i) const;
UBMColorModel GetInputImageColorModel(int i) const;
bool SetInputImageRes(int i, int width, int height);

// Разрешение выходного изображения
int GetOutputImageWidth(int i) const;
int GetOutputImageHeight(int i) const;
UBMColorModel GetOutputImageColorModel(int i) const;

// Задает входное изображение
bool SetInputImage(int i, const UBitmap &bmp);
bool SetInputImage(int i, UBColor *data, UBMColorModel colormodel);

// Возвращает указатель на входные изображения
PUBitmap* GetInputImages(void) const;

// Возвращает входное изображение по индексу
UBitmap& GetInputImage(int i);

// Возвращает данные входного изображения по индексу
UBColor* GetInputImageData(int i);

// Возвращает указатель на выходные изображения
PUBitmap* GetOutputImages(void) const;

// Возвращает выходное изображение по индексу
UBitmap& GetOutputImage(int i);

// Возвращает данные выходного изображения по индексу
UBColor* GetOutputImageData(int i);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
public:
// --------------------------

// --------------------------
// Скрытые методы управления
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
// --------------------------

// --------------------------
// Скрытые методы формирования моделей обработки
// --------------------------
// --------------------------


};

}

#endif


