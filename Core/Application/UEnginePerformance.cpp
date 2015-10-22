#include "UEnginePerformance.h"

namespace RDK {

/// --------------------------
/// Конструкторы и деструкторы
/// --------------------------
UEnginePrerformanceCalc::UEnginePrerformanceCalc(void)
{

}

UEnginePrerformanceCalc::~UEnginePrerformanceCalc(void)
{

}
/// --------------------------

/// --------------------------
/// Методы управления параметрами
/// --------------------------
/// Число усреднений
int UEnginePrerformanceCalc::GetAverageIterations(void) const
{
 return AverageIterations;
}

bool UEnginePrerformanceCalc::SetAverageIterations(int num)
{
 if(AverageIterations == num)
  return true;

 AverageIterations=num;
 return true;
}

/// Массив длинных имен наблюдаемых компонент
const std::vector<std::string>& UEnginePrerformanceCalc::GetComponentsName(void) const
{
 return ComponentsName;
}

bool UEnginePrerformanceCalc::SetComponentsName(const std::vector<std::string>& value)
{
 if(ComponentsName == value)
  return true;
 ComponentsName=value;
 ClearPerformanceData();
 return true;
}

/// Массив имен наблюдаемых интерфейсов
const std::vector<std::string>& UEnginePrerformanceCalc::GetGuiNames(void) const
{
 return GuiNames;
}

bool UEnginePrerformanceCalc::SetGuiNames(const std::vector<std::string>& value)
{
 if(GuiNames == value)
  return true;
 GuiNames=value;
 ClearPerformanceData();
 return true;
}

/// Возвращает имя компнента по индексу
std::string UEnginePrerformanceCalc::GetComponentName(int index) const
{
 if(index<0 || index>=int(ComponentsName.size()))
  return std::string("");
 return ComponentsName[index];
}

/// Ищет индекс по имени компонента
int UEnginePrerformanceCalc::FindComponentIndex(const std::string &name) const
{
 std::vector<std::string>::iterator I=find(ComponentsName.begin(),ComponentsName.end(),name);
 if(I == ComponentsName.end())
  return -1;
 return int(I-ComponentsName.begin());
}

/// Возвращает имя gui по индексу
std::string UEnginePrerformanceCalc::GetGuiName(int index) const
{
 if(index<0 || index>=int(GuiNames.size()))
  return std::string("");
 return GuiNames[index];
}
/// --------------------------

/// --------------------------
/// Методы управления данными
/// --------------------------
/// Удаляет все накопленные данные
void UEnginePrerformanceCalc::ClearPerformanceData(void)
{
 ComponentsPerformance.clear();
 GuiPerformance.clear();
 ModelPerformance=UPerformanceData();
 EngineTimeData=UEngineTimeData();
 ComponentsPerformance.resize(ComponentsName.size());
 GuiPerformance.resize(GuiNames.size());
}

/// Добавляет данные для выбранного компонента
void UEnginePrerformanceCalc::AddComponentPerformanceData(int index, long long value)
{
 if(index<0 || index>=ComponentsPerformance.size())
  return;

 ComponentsPerformance[index].CalcDurationHistory.push_back(value);
}

void UEnginePrerformanceCalc::AddComponentPerformanceData(const std::string &name, long long value)
{
 AddComponentPerformanceData(FindComponentIndex(name, value);
}

/// Добавляет данные для выбранного gui
void UEnginePrerformanceCalc::AddGuiPerformanceData(int index, long long value)
{
 if(index<0 || index>=GuiPerformance.size())
  return;

 GuiPerformance[index].CalcDurationHistory.push_back(value);
}

/// Производит расчет оценки производительности
void UEnginePrerformanceCalc::Calculate(void)
{
 ModelPerformance

 EngineTimeData

 for(size_t i=0;i<ComponentsPerformance.size();i++)
 {
 }

 for(size_t i=0;i<GuiPerformance.size();i++)
 {

 }
}
/// --------------------------

/// --------------------------
/// Методы чтения статистики
/// --------------------------
/// Возавращает оценку производительности для компонент
UPerformanceResults UEnginePrerformanceCalc::GetComponentPerformance(const std::string &name) const
{
 int i=FindComponentIndex(name);
 if(i<0)
  return UPerformanceResults();

 return ComponentsPerformance[i];
}

UPerformanceResults UEnginePrerformanceCalc::GetComponentPerformance(int index) const
{
 if(index<0 || index>=ComponentsPerformance.size())
  return UPerformanceResults();
 return ComponentsPerformance[i];
}

/// Возавращает оценку производительности для gui
UPerformanceResults UEnginePrerformanceCalc::GetGuiPerformance(int index) const
{
 if(index<0 || index>=GuiPerformance.size())
  return UPerformanceResults();
 return GuiPerformance[i];
}

/// Производительность всей модели
UPerformanceData UEnginePrerformanceCalc::GetModelPerformance(void) const
{
 return ModelPerformance;
}

/// Время работы
UEngineTimeData UEnginePrerformanceCalc::GetEngineTimeData(void) const
{
 return EngineTimeData;
}
/// --------------------------

}
