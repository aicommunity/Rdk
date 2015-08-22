#ifndef UIVisualController_H
#define UIVisualController_H

#include "../../Core/Graphics/UBitmap.h"
#include "../../Core/Graphics/UAGraphics.h"
#include "../../Core/Graphics/UGraphicsIO.h"
#include "../../Core/Graphics/UGraphics.h"
#include "../../Core/Graphics/UFont.h"
#include "../../Core/Serialize/USerStorageXML.h"
#include "../../Core/Engine/UELockVar.h"

namespace RDK {

// Глобальная коллекция шрифтов
extern RDK_LIB_TYPE RDK::UBitmapFontCollection GlobalFonts;
//extern RDK_LIB_TYPE RDK::UELockVar<RDK::UBitmapFontCollection> GlobalFonts;

// Очищает коллекцию глобальных шрифтов
RDK_LIB_TYPE void ClearClobalFonts(void);

// Загружает новый глобальный шрифт
RDK_LIB_TYPE bool AddGlobalFont(const std::string &font_file_name);

// Класс прототип-визуальных интерфейсов
class RDK_LIB_TYPE UIVisualController
{
public:
// Метод, вызываемый после загрузки проекта
virtual void AfterLoadProject(void)=0;

// Метод, вызываемый перед сбросом модели
virtual void BeforeReset(void)=0;

// Метод, вызываемый после сброса модели
virtual void AfterReset(void)=0;

// Метод, вызываемый перед шагом расчета
virtual void BeforeCalculate(void)=0;

// Метод, вызываемый после шага расчета
virtual void AfterCalculate(void)=0;

// Обновление интерфейса
virtual void UpdateInterface(bool force_update=false)=0;

// Возврат интерфейса в исходное состояние
virtual void ClearInterface(void)=0;

// Возвращает уникальное имя интерфейса
virtual std::string GetName(void)=0;

// Возвращает имя класса интерфейса
virtual std::string GetClassName(void)=0;

// Возвращает интервал обновления интерфейса
virtual long GetUpdateInterval(void)=0;

// Возвращает флаг разрешения обновления интерфейса даже если он не виден
virtual bool GetAlwaysUpdateFlag(void)=0;

// Сохраняет параметры интерфейса в xml
virtual void SaveParameters(RDK::USerStorageXML &xml)=0;

// Загружает параметры интерфейса из xml
virtual void LoadParameters(RDK::USerStorageXML &xml)=0;

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
virtual void ResetCalculationStepUpdatedFlag(void)=0;

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
virtual void SetCalculationStepUpdatedFlag(void)=0;

/// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
virtual bool GetCalculationStepUpdatedFlag(void)=0;

/// Возвращает время обновления интерфейса (мс)
virtual unsigned long long GetUpdateTime(void)=0;
};

// Класс хранилище-визуальных интерфейсов
class RDK_LIB_TYPE UIVisualControllerStorage
{
public:
// Список обработчиков, которые должны быть вызваны после расчета
static std::vector<RDK::UIVisualController*> InterfaceUpdaters;

/// Общее время обновления интерфейса
static unsigned long long UpdateTime;

public:
// Добавляет обработчик в список
static void AddInterface(RDK::UIVisualController *value);

// Удаляет обработчик из списка
static void DelInterface(RDK::UIVisualController *value);

// Метод, вызываемый после загрузки проекта
static void AfterLoadProject(void);

// Метод, вызываемый перед сбросом модели
static void BeforeReset(void);

// Метод, вызываемый после сброса модели
static void AfterReset(void);

// Метод, вызываемый перед шагом расчета
static void BeforeCalculate(void);

// Метод, вызываемый после шага расчета
static void AfterCalculate(void);

// Обновление интерфейса
static void UpdateInterface(bool force_update=false);

// Возврат интерфейса в исходное состояние
static void ClearInterface(void);

// Сохраняет параметры интерфейса в xml
static void SaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
static void LoadParameters(RDK::USerStorageXML &xml);

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
static void ResetCalculationStepUpdatedFlag(void);

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
static void SetCalculationStepUpdatedFlag(void);

/// Возвращает время обновления интерфейса (мс)
static unsigned long long GetUpdateTime(void);

};

}

#endif

