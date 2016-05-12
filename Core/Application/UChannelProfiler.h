#ifndef UChannelProfilerH
#define UChannelProfilerH

#include "../../Deploy/Include/rdk.h"

namespace RDK {

/// Содержит интегральные данные о производительности движка
struct RDK_LIB_TYPE UIntegralPerfomanceResults
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
double RtPerfomance;

/// Полный FPS (включая Gui)
double FullFps;

/// FPS модели (какой был бы FPS при отсутствии накладных расходов на Gui и прочее)
double ModelFps;

UIntegralPerfomanceResults(void);
};

/// Результаты расчетов производительности для компонента или элемента интерфейса
struct RDK_LIB_TYPE UPerfomanceResults
{
/// Среднее время расчета, с
double AvgDuration;

/// Процент от общего потребления времени аналогичными элементами
double Percentage;

UPerfomanceResults(void);
};

/// История производительности компонента или элемента интерфейса
struct RDK_LIB_TYPE UPerfomanceData: public UPerfomanceResults
{
/// Данные производительности, мс
std::vector<long long> CalcDurationHistory;

/// Добавляет данные в историю
void AddHistory(long long value, int max_values);

/// Расчет среднего
void CalcAverage(void);

/// Расчет процента от общего времений
void CalcPercentage(double full_time);
};

/// Обеспечивает хранение данных производительности и подсчет выходных метрик
class RDK_LIB_TYPE UChannelProfiler
{
protected: // Параметры
/// Индекс канала
int ChannelIndex;

/// Число усреднений
int AverageIterations;

/// Массив длинных имен наблюдаемых компонент
std::vector<std::string> ComponentsName;

/// Массив имен наблюдаемых интерфейсов
std::vector<std::string> GuiNames;

protected: // Данные
/// Производительность компонент
std::vector<UPerfomanceData> ComponentsPerfomance;

/// Производительность интерфейсов
std::vector<UPerfomanceData> GuiPerfomance;

/// Производительность всей модели
UPerfomanceData ModelPerfomance;

/// Производительность всего кроме модели
UPerfomanceData OtherPerfomance;

/// Суммарная производительность Gui
UPerfomanceData SummaryGuiPerfomance;

/// Интегральные данные о производительности
UIntegralPerfomanceResults IntegralPerfomanceResults;

protected: // Выходные данные расчета
/// Список легенда-время в секундах для компонент и модели в целом
/// Элементы расположены подряд в соответствии с ComponentsName
/// Если ComponentsName.size()>0 то имеет размер ComponentsName.size()+5
/// где 3 последние элемента это время, затрачиваемое:
/// 1: остальными компонентами модели
/// 2: всей моделью
/// 3: обработкой Gui
/// 4: остаток от разности между полным временем цикла-модели-обработка gui
/// 5: полное время цикла
/// Если ComponentsName.size()==0 то имеет размер 4
/// где элементы - это время, затрачиваемое:
/// 1: всей моделью
/// 2: обработкой Gui
/// 3: остаток от разности между полным временем цикла-модели-обработка gui
/// 4: полное время цикла
/// В обоих случаях разность между 5-(2+3+4), или 4-(1+2+3) может быть не равна
/// нулю, т.к. используются усредненные данные по каждой величине
std::list<pair<std::string, UPerfomanceResults> > ComponentsProfilerOutputData;

/// Список легенда-время в секундах обновления Gui
/// элементы расположены подряд в соответствии с GuiNames
std::list<pair<std::string, UPerfomanceResults> > GuiProfilerOutputData;

protected: // Временные переменные
/// Мьютекс блокировки доступа к данным
UGenericMutex* Mutex;

public: // Методы
/// --------------------------
/// Конструкторы и деструкторы
/// --------------------------
UChannelProfiler(void);
virtual ~UChannelProfiler(void);
/// --------------------------

/// --------------------------
/// Методы управления параметрами
/// --------------------------
/// Индекс канала
int GetChannelIndex(void) const;
bool SetChannelIndex(int index);

/// Число усреднений
int GetAverageIterations(void) const;
bool SetAverageIterations(int num);

/// Массив длинных имен наблюдаемых компонент
std::vector<std::string> GetComponentsName(void) const;
bool SetComponentsName(const std::vector<std::string>& value);

/// Добавляет новый компонент в список
bool AddComponent(const std::string &name);

/// Добавляет все дочерние компоненты данного компонента в список
bool AddAllComponents(const std::string &name);

/// Удаляет компонент
bool DelComponent(const std::string &name);
bool DelComponent(int index);

/// Удаляет все компоненты
void DelAllComponents(void);

/// Массив имен наблюдаемых интерфейсов
std::vector<std::string> GetGuiNames(void) const;
bool SetGuiNames(const std::vector<std::string>& value);

/// Добавляет все интерфейсы в список
bool AddAllGui(void);

/// Возвращает имя компнента по индексу
std::string GetComponentName(int index) const;

/// Ищет индекс по имени компонента
int FindComponentIndex(const std::string &name) const;

/// Возвращает имя gui по индексу
std::string GetGuiName(int index) const;
/// --------------------------

/// --------------------------
/// Методы управления данными
/// --------------------------
/// Удаляет все накопленные данные
void ClearPerfomanceData(void);

/// Добавляет данные для выбранного компонента
void AddComponentPerfomanceData(int index, long long value);
void AddComponentPerfomanceData(const std::string &name, long long value);

/// Добавляет данные для выбранного gui
void AddGuiPerfomanceData(int index, long long value);

/// Выполняет считывание сырых данных ядра о производительности
void LoadCorePerfomanceData(void);

/// Выполняет считывание сырых данных gui о производительности
void LoadGuiPerfomanceData(void);

/// Производит расчет оценкок производительности ядра из сырых данных
void CalcCorePerfomance(void);

/// Производит расчет оценкок производительности gui из сырых данных
void CalcGuiPerfomance(void);

/// Производит расчет выходных данных профайлера
void CalcProfilerOutputData(void);

/// Производит полный расчет данных профайлера
void Calculate(void);
void CalculateCore(void);
void CalculateGui(void);
/// --------------------------

/// --------------------------
/// Методы чтения статистики
/// --------------------------
/// Возавращает оценку производительности для компонент
UPerfomanceResults GetComponentPerfomance(const std::string &name) const;
UPerfomanceResults GetComponentPerfomance(int index) const;

/// Возвращает оценку производительности для gui
UPerfomanceResults GetGuiPerfomance(int index) const;

/// Производительность всей модели
UPerfomanceData GetModelPerfomance(void) const;

/// Производительность всего кроме модели
UPerfomanceData GetOtherPerfomance(void) const;

/// Суммарная производительность Gui
UPerfomanceData GetSummaryGuiPerfomance(void) const;

/// Возвращает интегральные данные о производительности
UIntegralPerfomanceResults GetIntegralPerfomanceResults(void) const;
/// --------------------------

/// --------------------------
/// Методы чтения обработанной статистики
/// --------------------------
/// Возвращает строку с текущим временем для режима Rt
std::string CalcRtTimeText(void) const;

/// Возвращает строку с текущим временем для обычного режима
std::string CalcNormalTimeText(void) const;

/// Возвращает строку с производительностью для режима Rt
std::string CalcRtPerfomanceText(void) const;

/// Возвращает строку с текущей длительностью шага расчета для нормального режима
std::string CalcNormalStepDurationText(void) const;

/// Возвращает строку с текущим FPS для нормального режима
std::string CalcNormalFpsText(void) const;

/// Список легенда-время в секундах для компонент и модели в целом
/// (см описание соответствующего параметра)
std::list<pair<std::string, UPerfomanceResults> > GetComponentsProfilerOutputData(void) const;

/// Список легенда-время в секундах обновления Gui
/// элементы расположены подряд в соответствии с GuiNames
std::list<pair<std::string, UPerfomanceResults> > GetGuiProfilerOutputData(void) const;
/// --------------------------


// --------------------------
// Общие методы управления контроллером
// --------------------------
// Сохраняет параметры интерфейса в xml
virtual void SaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void LoadParameters(RDK::USerStorageXML &xml);
// --------------------------
};

}

#endif
