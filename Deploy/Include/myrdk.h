#ifndef MYRDK_H
#define MYRDK_H

#include "../../Core/System/rdk_system.h"
#include "../../ThirdParty/ThirdParty.h"
#include "../../Core/Math/UMath.h"
#include "../../Core/Utilities/Utilities.h"
#include "../../Core/Serialize/Serialize.h"

#include "../../Core/Engine/UAEnvSupport.h"
#include "../../Core/Engine/UXMLEnvSerialize.h"
#include "../../Core/Engine/UBinaryEnvSerialize.h"
#include "../../Core/Graphics/UBitmap.h"
#include "../../Core/Graphics/UAGraphics.h"
#include "../../Core/Graphics/UGraphicsIO.h"
#include "../../Core/Graphics/UGraphics.h"
#include "../../Core/Graphics/UFont.h"
#include "../../Core/Graphics/UDrawEngine.h"
#include "../../Core/Graphics/UGraphicsXMLSerialize.h"
#include "../../Core/Graphics/UGraphicsBinarySerialize.h"

namespace RDK {

// Класс прототип-визуальных интерфейсов
class UIVisualController
{
public:
// Метод, вызываемый перед сбросом модели
virtual void BeforeReset(void)=0;

// Метод, вызываемый после сброса модели
virtual void AfterReset(void)=0;

// Метод, вызываемый перед шагом расчета
virtual void BeforeCalculate(void)=0;

// Метод, вызываемый после шага расчета
virtual void AfterCalculate(void)=0;

// Обновление интерфейса
virtual void UpdateInterface(void)=0;

// Возвращает уникальное имя интерфейса
virtual std::string GetName(void)=0;

// Сохраняет параметры интерфейса в xml
virtual void SaveParameters(RDK::Serialize::USerStorageXML &xml)=0;

// Загружает параметры интерфейса из xml
virtual void LoadParameters(RDK::Serialize::USerStorageXML &xml)=0;
};

// Класс хранилище-визуальных интерфейсов
class UIVisualControllerStorage
{
public:
// Список обработчиков, которые должны быть вызваны после расчета
static std::vector<RDK::UIVisualController*> InterfaceUpdaters;

public:
// Добавляет обработчик в список
static void AddInterface(RDK::UIVisualController *value);

// Удаляет обработчик из списка
static void DelInterface(RDK::UIVisualController *value);

// Метод, вызываемый перед сбросом модели
static void BeforeReset(void);

// Метод, вызываемый после сброса модели
static void AfterReset(void);

// Метод, вызываемый перед шагом расчета
static void BeforeCalculate(void);

// Метод, вызываемый после шага расчета
static void AfterCalculate(void);

// Обновление интерфейса
static void UpdateInterface(void);

// Сохраняет параметры интерфейса в xml
static void SaveParameters(RDK::Serialize::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
static void LoadParameters(RDK::Serialize::USerStorageXML &xml);
};

}
#endif

