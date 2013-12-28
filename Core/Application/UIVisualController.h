#ifndef UIVisualController_H
#define UIVisualController_H
  
#include "../../Core/Graphics/UBitmap.h"
#include "../../Core/Graphics/UAGraphics.h"
#include "../../Core/Graphics/UGraphicsIO.h"
#include "../../Core/Graphics/UGraphics.h"
#include "../../Core/Graphics/UFont.h"
  
namespace RDK {

// Глобальная коллекция шрифтов
extern RDK::UBitmapFontCollection GlobalFonts;

// Очищает коллекцию глобальных шрифтов
void ClearClobalFonts(void);

// Загружает новый глобальный шрифт
bool AddGlobalFont(const std::string &font_file_name);

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
virtual void UpdateInterface(bool force_update=true)=0;

// Возврат интерфейса в исходное состояние
virtual void ClearInterface(void)=0;

// Возвращает уникальное имя интерфейса
virtual std::string GetName(void)=0;

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
};

}

#endif

