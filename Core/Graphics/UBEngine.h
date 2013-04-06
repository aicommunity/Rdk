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
#include "Libraries/Basic/UBAResize.h"


namespace RDK{

class UBEngine: public UEngine
{
protected: // Параметры инициализации

protected: // Переменные состояния

protected: // Данные

protected: // Временные переменные
UBitmap TempBmp,TempBmp2;

UBAResizeEdges BResizeEdges;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBEngine(void);
virtual ~UBEngine(void);
// --------------------------

// --------------------------
// Методы управления параметрами инициализации
// --------------------------
// --------------------------


// --------------------------
// Методы доступа к переменным состояния
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает указатель на среду
virtual UBAEnvironment* GetEnvironment(void);

virtual UNet* GetModel(void);
// --------------------------

// --------------------------
// Методы управления движком
// --------------------------
// Инициализирует данные движка
virtual void Init(void);
virtual bool Init(UEPtr<UStorage> storage, UEPtr<UEnvironment> env);

// Деинициализирует данные движка
// и сохраняет текущие настройки
virtual void UnInit(void);

// Запускает систему
virtual bool Start(void);

// Приостанавливает систему
virtual bool Pause(void);

// Останавливает систему
virtual bool Stop(void);
// --------------------------

// --------------------------
// Методы управления средой
// --------------------------
// Задает число входов среды
virtual void Env_SetNumInputImages(int number);

// Задает число выходов среды
virtual void Env_SetNumOutputImages(int number);

// Возвращает число входов среды
virtual int Env_GetNumInputImages(void);

// Возвращает число выходов среды
virtual int Env_GetNumOutputImages(void);

// Задает разрешение по умолчанию (рабочее разрешение)
virtual void Env_SetInputRes(int number, int width, int height);

// Задает флаг отражения входного изображения вокруг горизонтальной оси
virtual void Env_SetReflectionXFlag(bool value);

// Возвращает разрешение по умолчанию (рабочее разрешение)
virtual int Env_GetInputImageWidth(int number);
virtual int Env_GetInputImageHeight(int number);
virtual int Env_GetInputImageColorModel(int number);

// Возвращает текущее выходное разрешение
virtual int Env_GetOutputImageWidth(int number);
virtual int Env_GetOutputImageHeight(int number);
virtual int Env_GetOutputImageColorModel(int number);

virtual void Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

virtual unsigned char* Env_GetInputImage(int index);

virtual unsigned char* Env_GetOutputImage(int index);

virtual unsigned char* Env_GetOutputImageY8(int index);

// --------------------------
// Методы управления моделью
// --------------------------
// Возвращает указатель на выход с индексом 'index' компонента 'id'
virtual const RDK::UBitmap* Model_GetComponentOutput(const char *stringid, int index);

// Возвращает указатель на выход с индексом 'index' компонента 'id'
virtual const RDK::UBitmap* Model_GetComponentBitmapOutput(const char *stringid, int index);

// Возвращает указатель на вход с индексом 'index' компонента 'id'
virtual const RDK::UBitmap* Model_GetComponentBitmapInput(const char *stringid, int index);

// Замещает изображение выхода с индексом 'index' компонента 'id'
virtual void Model_SetComponentBitmapOutput(const char *stringid, int index, const RDK::UBitmap* const bmp);

// Замещает изображение входа с индексом 'index' компонента 'id'
virtual void Model_SetComponentBitmapInput(const char *stringid, int index, const RDK::UBitmap* const bmp);
// --------------------------

// --------------------------
// Методы внутреннего управления движком
// --------------------------
protected:
// Загружает набор предустановленных библиотек
virtual int LoadPredefinedLibraries(void);
// --------------------------

// --------------------------
// Методы внутреннего управления консолью
// --------------------------
protected:
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
// --------------------------
};


}
#endif
