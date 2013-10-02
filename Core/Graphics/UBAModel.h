/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBAMODEL_H
#define UBAMODEL_H

#include "../Engine/UNet.h"

namespace RDK {

class UBAModel: public UNet
{
protected: // Входы и выходы
UPropertyOutputData<UBitmap,UBAModel> Output;

/// Рабочее разрешение модели
ULProperty<int, UBAModel, ptPubState> WorkWidth;
ULProperty<int, UBAModel, ptPubState> WorkHeight;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBAModel(void);
virtual ~UBAModel(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual UBAModel* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);
virtual bool AMDefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);
virtual bool AMBuild(void);

// Сброс процесса счета.
virtual bool AReset(void);
virtual bool AMReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
virtual bool AMCalculate(void);
// --------------------------
};

}

#endif



