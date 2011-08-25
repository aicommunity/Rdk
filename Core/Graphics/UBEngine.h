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
protected: // Параметры инициализации

protected: // Переменные состояния

protected: // Данные

protected: // Временные переменные
RDK::UBitmap TempBmp,TempBmp2;


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
// --------------------------

// --------------------------
// Методы управления движком
// --------------------------
// Инициализирует данные движка
virtual bool Init(void);

// Деинициализирует данные движка
// и сохраняет текущие настройки
virtual bool UnInit(void);

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
virtual void Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

virtual unsigned char* Env_GetInputImage(int index);

virtual unsigned char* Env_GetOutputImage(int index);

virtual unsigned char* Env_GetOutputImageY8(int index);

// Возвращает указатель на выход с индексом 'index' компонента 'id'
virtual const RDK::UBitmap* const Env_GetComponentOutput(const char *stringid, int index);
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
