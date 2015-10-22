#ifndef UEnginePerformanceH
#define UEnginePerformanceH

#include "../../Deploy/Include/rdk.h"

namespace RDK {

/// Содержит мгновенные данные о временах расчета
struct RDK_LIB_TYPE UEngineTimeData
{
/// Время модели, дни
double ModelTime;

/// Реальное время, дни
double RealTime;

/// Реальное время расчета в режиме RT (Real Time)
double RtCalcDuration;

/// Модельное время расчета в режиме RT
double RtModelDuration;

/// Оценка производительности режима RT
double RtPerformance;
};

/// Результаты расчетов производительности для компонента или элемента интерфейса
struct RDK_LIB_TYPE UPerformanceResults
{
/// Среднее
long long AvgCalcDuration;

/// Процент от общего потребления времени аналогичными элементами
double CalcPrecent;
};

/// История производительности компонента или элемента интерфейса
struct RDK_LIB_TYPE UPerformanceData: public UPerformanceResults
{
/// Данные производительности
std::vector<long long> CalcDurationHistory;

};

/// Обеспечивает хранение данных производительности и подсчет выходных метрик
class RDK_LIB_TYPE UEnginePrerformanceCalc
{
protected: // Параметры
/// Число усреднений
int AverageIterations;

/// Массив длинных имен наблюдаемых компонент
std::vector<std::string> ComponentsName;

/// Массив имен наблюдаемых интерфейсов
std::vector<std::string> GuiNames;

protected: // Данные
/// Производительность компонент
std::vector<UPerformanceData> ComponentsPerformance;

/// Производительность интерфейсов
std::vector<UPerformanceData> GuiPerformance;

/// Производительность всей модели
UPerformanceData ModelPerformance;

/// Время работы
UEngineTimeData EngineTimeData;

public: // Методы
/// --------------------------
/// Конструкторы и деструкторы
/// --------------------------
UEnginePrerformanceCalc(void);
virtual ~UEnginePrerformanceCalc(void);
/// --------------------------

/// --------------------------
/// Методы управления параметрами
/// --------------------------
/// Число усреднений
int GetAverageIterations(void) const;
bool SetAverageIterations(int num);

/// Массив длинных имен наблюдаемых компонент
const std::vector<std::string>& GetComponentsName(void) const;
bool SetComponentsName(const std::vector<std::string>& value);

/// Массив имен наблюдаемых интерфейсов
const std::vector<std::string>& GetGuiNames(void) const;
bool SetGuiNames(const std::vector<std::string>& value);

/// Возвращает имя компнента по индексу
std::string GetComponentName(int index) const;

/// Ищет индекс по имени компонента
std::string FindComponentIndex(const std::string &name) const;

/// Возвращает имя gui по индексу
std::string GetGuiName(int index) const;
/// --------------------------

/// --------------------------
/// Методы управления данными
/// --------------------------
/// Удаляет все накопленные данные
void ClearPerformanceData(void);

/// Добавляет данные для выбранного компонента
void AddComponentPerformanceData(int index, long long value);
void AddComponentPerformanceData(const std::string &name, long long value);

/// Добавляет данные для выбранного gui
void AddGuiPerformanceData(int index, long long value);

/// Производит расчет оценки производительности
void Calculate(void);
/// --------------------------

/// --------------------------
/// Методы чтения статистики
/// --------------------------
/// Возавращает оценку производительности для компонент
UPerformanceResults GetComponentPerformance(const std::string &name) const;
UPerformanceResults GetComponentPerformance(int index) const;

/// Возавращает оценку производительности для gui
UPerformanceResults GetGuiPerformance(int index) const;

/// Производительность всей модели
UPerformanceData GetModelPerformance(void) const;

/// Время работы
UEngineTimeData GetEngineTimeData(void) const;
/// --------------------------
};

}

#endif
