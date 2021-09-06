#include "UChannelProfiler.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../Deploy/Include/rdk_exceptions.h"
#include "UIVisualController.h"

namespace RDK {

UPerformanceElement::UPerformanceElement(void)
 : Duration(0), RegTime(0), Interval(0)
{

}

UPerformanceElement::UPerformanceElement(const UPerformanceElement &copy)
 : Duration(copy.Duration), RegTime(copy.RegTime), Interval(copy.Interval)
{

}

UPerformanceElement::UPerformanceElement(long long duration, unsigned long long reg_time, long long interval)
    : Duration(duration), RegTime(reg_time), Interval(interval)
{

}

UIntegralPerfomanceResults::UIntegralPerfomanceResults(void)
{
 ModelTime=0.0;
 RealTime=0.0;
 RtCalcDuration=0.0;
 RtModelDuration=0.0;
 RtPerfomance=0.0;
 FullFps=0.0;
 ModelFps=0.0;
}


UPerfomanceResults::UPerfomanceResults(void)
 : MinDuration(0.0), MaxDuration(0.0),
   MinInterval(0.0), MaxInterval(0.0), AvgInterval(0.0), AvgDuration(0.0), Percentage(0.0)
{
}



/// Добавляет данные в историю
void UPerfomanceData::AddHistory(long long value, long long interval, int max_values)
{
 unsigned long long reg_time(0);
 if(!CalcDurationHistory.empty())
  reg_time=GetCurrentStartupTime()-CalcDurationHistory.back().RegTime;
 UPerformanceElement hist(value,reg_time,interval);
 CalcDurationHistory.push_back(hist);
 while(int(CalcDurationHistory.size())>max_values)
  CalcDurationHistory.erase(CalcDurationHistory.begin());
}

/// История производительности компонента или элемента интерфейса
//struct RDK_LIB_TYPE UPerfomanceData: public UPerfomanceResults
/// Расчет метрик
void UPerfomanceData::CalcMetrics(void)
{
 AvgDuration=0;
 MinDuration=10e6;
 MaxDuration=0;
 MinInterval=10e6;
 MaxInterval=0;

 std::list<UPerformanceElement>::iterator I=CalcDurationHistory.begin();
 for(;I!=CalcDurationHistory.end();I++)
 {
  UPerformanceElement &curr=*I;
  AvgDuration+=double(curr.Duration);

  AvgInterval+=double(curr.Interval);

  if(MinDuration>curr.Duration)
   MinDuration=double(curr.Duration);

  if(MaxDuration<curr.Duration)
   MaxDuration=double(curr.Duration);

  if(MinInterval>curr.Interval)
   MinInterval=double(curr.Interval);

  if(MaxInterval<curr.Interval)
   MaxInterval=double(curr.Interval);
 }
 if(!CalcDurationHistory.empty())
 {
  AvgDuration/=double(CalcDurationHistory.size())*1000.0;
  AvgInterval/=double(CalcDurationHistory.size())*1000.0;
 }
 MinInterval/=1000.0;
 MaxInterval/=1000.0;
 MaxDuration/=1000.0;
 MinDuration/=1000.0;
}

/// Расчет процента от общего времений
void UPerfomanceData::CalcPercentage(double full_time)
{
 if(full_time>0)
  Percentage=AvgDuration/full_time;
 else
  Percentage=0;
}


/// --------------------------
/// Конструкторы и деструкторы
/// --------------------------
UChannelProfiler::UChannelProfiler(void)
{
// Mutex=UCreateMutex();
 AverageIterations=10;
 ChannelIndex=0;
}

UChannelProfiler::~UChannelProfiler(void)
{
// if(Mutex)
//  UDestroyMutex(Mutex);
// Mutex=0;
}
/// --------------------------

/// --------------------------
/// Методы управления параметрами
/// --------------------------
/// Индекс канала
int UChannelProfiler::GetChannelIndex(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return ChannelIndex;
}

bool UChannelProfiler::SetChannelIndex(int index)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 if(ChannelIndex == index)
  return true;
 ChannelIndex=index;
 return true;
}

/// Число усреднений
int UChannelProfiler::GetAverageIterations(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return AverageIterations;
}

bool UChannelProfiler::SetAverageIterations(int num)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 if(AverageIterations == num)
  return true;

 AverageIterations=num;
 return true;
}

/// Массив длинных имен наблюдаемых компонент
std::vector<std::string> UChannelProfiler::GetComponentsName(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return ComponentsName;
}

bool UChannelProfiler::SetComponentsName(const std::vector<std::string>& value)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 if(ComponentsName == value)
  return true;
 ComponentsName=value;
 ClearPerfomanceData();
 return true;
}

/// Добавляет новый компонент в список
bool UChannelProfiler::AddComponent(const std::string &name)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> comp_names=ComponentsName;
 comp_names.push_back(name);
 ComponentsName=comp_names;
 ClearPerfomanceData();
 return true;
}

/// Добавляет новый компонент в список
bool UChannelProfiler::AddAllComponents(const std::string &name)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> comp_names=ComponentsName;

 const char* components_name_list=MModel_GetComponentsNameList(ChannelIndex,name.c_str());
 std::vector<std::string> names;
 RDK::separatestring<char>(components_name_list,names,',');
 MEngine_FreeBufString(ChannelIndex, components_name_list);

 for(size_t i=0;i<names.size();i++)
 {
  const char *pname=0;
  if(!name.empty())
   pname=MModel_GetComponentLongName(ChannelIndex, (name+std::string(".")+names[i]).c_str());
  else
   pname=MModel_GetComponentLongName(ChannelIndex, names[i].c_str());

  if(pname)
   names[i]=pname;
  MEngine_FreeBufString(ChannelIndex, pname);
 }

 for(size_t i=0;i<names.size();i++)
 {
  comp_names.push_back(names[i]);
 }

 ComponentsName=comp_names;

 ClearPerfomanceData();
 return true;
}

/// Удаляет компонент
bool UChannelProfiler::DelComponent(const std::string &name)
{
 std::vector<std::string> comp_names=ComponentsName;
 std::vector<std::string>::iterator I=find(comp_names.begin(),comp_names.end(),name);
 if(I != comp_names.end())
  comp_names.erase(I);
 ComponentsName=comp_names;
 ClearPerfomanceData();
 return true;
}

bool UChannelProfiler::DelComponent(int index)
{
 std::vector<std::string> comp_names=ComponentsName;
 if(index>=0 && index<int(comp_names.size()))
 {
  comp_names.erase(comp_names.begin()+index);
  ClearPerfomanceData();
 }
 ComponentsName=comp_names;
 return true;
}

/// Добавляет новый компонент в список
void UChannelProfiler::DelAllComponents(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> comp_names;
 ComponentsName=comp_names;
 ClearPerfomanceData();
}

/// Массив имен наблюдаемых интерфейсов
std::vector<std::string> UChannelProfiler::GetGuiNames(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return GuiNames;
}

bool UChannelProfiler::SetGuiNames(const std::vector<std::string>& value)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 if(GuiNames == value)
  return true;
 GuiNames=value;
 ClearPerfomanceData();
 return true;
}

/// Добавляет все интерфейсы в список
bool UChannelProfiler::AddAllGui(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<RDK::UIVisualController*> &interfaces=RDK::UIVisualControllerStorage::InterfaceUpdaters;
 std::vector<std::string> gui_names;
 for(size_t i=0;i<interfaces.size();i++)
 {
  gui_names.push_back(interfaces[i]->CalcFullName());
 }
 GuiNames=gui_names;
 ClearPerfomanceData();
 return true;
}

/// Возвращает имя компнента по индексу
std::string UChannelProfiler::GetComponentName(int index) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> comp_names=ComponentsName;
 if(index<0 || index>=int(comp_names.size()))
  return std::string("");
 return comp_names[index];
}

/// Ищет индекс по имени компонента
int UChannelProfiler::FindComponentIndex(const std::string &name) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> comp_names=ComponentsName;
 std::vector<std::string>::const_iterator I=find(comp_names.begin(),comp_names.end(),name);
 if(I == comp_names.end())
  return -1;
 return int(I-comp_names.begin());
}

/// Возвращает имя gui по индексу
std::string UChannelProfiler::GetGuiName(int index) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> gui_names=GuiNames;
 if(index<0 || index>=int(gui_names.size()))
  return std::string("");
 return gui_names[index];
}
/// --------------------------

/// --------------------------
/// Методы управления данными
/// --------------------------
/// Удаляет все накопленные данные
void UChannelProfiler::ClearPerfomanceData(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<UPerfomanceData> buf;
 ComponentsPerfomance=buf;
 GuiPerfomance=buf;
 SummaryGuiPerfomance=UPerfomanceData();
 ModelPerfomance=UPerfomanceData();
 OtherPerfomance=UPerfomanceData();
 IntegralPerfomanceResults=UIntegralPerfomanceResults();
 std::vector<std::string> comp_names=ComponentsName;
 std::vector<std::string> gui_names=GuiNames;

 std::vector<UPerfomanceData> comp_perf_buf=ComponentsPerfomance;
 std::vector<UPerfomanceData> gui_perf_buf=GuiPerfomance;
 comp_perf_buf.resize(comp_names.size());
 ComponentsPerfomance=comp_perf_buf;

 gui_perf_buf.resize(gui_names.size());
 GuiPerfomance=gui_perf_buf;
 std::list<pair<std::string, UPerfomanceResults> > profiler_bug;

 GuiProfilerOutputData=profiler_bug;
 ComponentsProfilerOutputData=profiler_bug;
}

/// Добавляет данные для выбранного компонента
void UChannelProfiler::AddComponentPerfomanceData(int index, long long value, long long interval)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<UPerfomanceData> comp_perf_buf=ComponentsPerfomance;
 if(index<0 || index>=int(comp_perf_buf.size()))
  return;

 comp_perf_buf[index].AddHistory(value,interval,AverageIterations);
 ComponentsPerfomance=comp_perf_buf;
}

void UChannelProfiler::AddComponentPerfomanceData(const std::string &name, long long value, long long interval)
{
 AddComponentPerfomanceData(FindComponentIndex(name), value,interval);
}

/// Добавляет данные для выбранного gui
void UChannelProfiler::AddGuiPerfomanceData(int index, long long value, long long interval)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<UPerfomanceData> gui_perf_buf=GuiPerfomance;
 if(index<0 || index>=int(gui_perf_buf.size()))
  return;

 gui_perf_buf[index].AddHistory(value,interval,AverageIterations);
 GuiPerfomance=gui_perf_buf;
}

/// Выполняет считывание сырых данных ядра о производительности
void UChannelProfiler::LoadCorePerfomanceData(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UELockPtr<UEngine> engine(GetEngineLock(ChannelIndex));
 UEPtr<UNet> model(GetModel<UNet>(ChannelIndex));
 if(!model)
  return;
 RDK_SYS_TRY
 {
  try
  {
   std::vector<std::string> comp_names=ComponentsName;
   for(size_t i=0;i<comp_names.size();i++)
   {
    UEPtr<UNet> component=model->GetComponentL<UNet>(comp_names[i],true);
	if(component)
     AddComponentPerfomanceData(int(i), component->GetFullStepDuration(), component->GetInterstepsInterval());
   }
   long long full_step=model->GetFullStepDuration();
   long long interstep_interval=model->GetInterstepsInterval();
   UPerfomanceData model_performance=ModelPerfomance;
   model_performance.AddHistory(full_step,0,AverageIterations);
   ModelPerfomance=model_performance;
   UPerfomanceData other_performance=OtherPerfomance;
   other_performance.AddHistory(interstep_interval,0,AverageIterations);
   OtherPerfomance=other_performance;

   UEPtr<UEnvironment> env=GetEnvironment(ChannelIndex);
   UIntegralPerfomanceResults integral_results=IntegralPerfomanceResults;
   integral_results.ModelTime=env->GetTime().GetDoubleTime();
   integral_results.RealTime=env->GetTime().GetDoubleRealTime();
   integral_results.RtCalcDuration=env->GetRTLastDuration();
   integral_results.RtModelDuration=env->GetRTModelCalcTime();
   integral_results.RtPerfomance=env->CalcRTPerformance();
   IntegralPerfomanceResults=integral_results;
  }
  catch (RDK::UException &exception)
  {
   engine->ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   engine->ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  engine->ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
}

/// Выполняет считывание сырых данных gui о производительности
void UChannelProfiler::LoadGuiPerfomanceData(void)
{
 std::vector<RDK::UIVisualController*> &interfaces=RDK::UIVisualControllerStorage::InterfaceUpdaters;
 std::vector<std::string> gui_names=GuiNames;

 if(interfaces.size() != gui_names.size())
 {
  AddAllGui();
 }

 std::vector<UPerfomanceData> gui_perf_buf=GuiPerfomance;
 for(size_t i=0;i<gui_perf_buf.size();i++)
 {
  if(interfaces[i]->GetUpdateInterval()>0)
   AddGuiPerfomanceData(int(i), interfaces[i]->GetUpdateTime(), interfaces[i]->GetUpdateInterval());
  else
   AddGuiPerfomanceData(int(i), interfaces[i]->GetUpdateTime(), 0);
 }
 // UGenericMutexExclusiveLocker locker(Mutex);
 UPerfomanceData summary_gui_perf=SummaryGuiPerfomance;
 summary_gui_perf.AddHistory(RDK::UIVisualControllerStorage::GetUpdateTime(),0,AverageIterations);
 SummaryGuiPerfomance=summary_gui_perf;
}

/// Производит расчет оценкок производительности ядра из сырых данных
void UChannelProfiler::CalcCorePerfomance(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UPerfomanceData model_perf=ModelPerfomance;
 UPerfomanceData other_perf=OtherPerfomance;
 UIntegralPerfomanceResults integral_perf=IntegralPerfomanceResults;
 model_perf.CalcMetrics();
 other_perf.CalcMetrics();
 double avg_full_step=model_perf.AvgDuration+other_perf.AvgDuration;
 model_perf.CalcPercentage(avg_full_step);
 other_perf.CalcPercentage(avg_full_step);

 if(model_perf.AvgDuration>0)
  integral_perf.ModelFps=1.0/model_perf.AvgDuration;
 else
  integral_perf.ModelFps=0.0;

 if(avg_full_step>0)
  integral_perf.FullFps=1.0/avg_full_step;
 else
  integral_perf.FullFps=0.0;

 ModelPerfomance=model_perf;
 OtherPerfomance=other_perf;
 IntegralPerfomanceResults=integral_perf;

 std::vector<UPerfomanceData> comp_perf_buf=ComponentsPerfomance;
 for(size_t i=0;i<comp_perf_buf.size();i++)
 {
  comp_perf_buf[i].CalcMetrics();
  comp_perf_buf[i].CalcPercentage(model_perf.AvgDuration);
 }
 ComponentsPerfomance=comp_perf_buf;
}

/// Производит расчет оценкок производительности gui из сырых данных
void UChannelProfiler::CalcGuiPerfomance(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);

 UPerfomanceData summary_perf=SummaryGuiPerfomance;
 summary_perf.CalcMetrics();
 summary_perf.CalcPercentage(summary_perf.AvgDuration);
 SummaryGuiPerfomance=summary_perf;

 std::vector<UPerfomanceData> gui_perf_buf=GuiPerfomance;
 for(size_t i=0;i<gui_perf_buf.size();i++)
 {
  gui_perf_buf[i].CalcMetrics();
  gui_perf_buf[i].CalcPercentage(summary_perf.AvgDuration);
 }
 GuiPerfomance=gui_perf_buf;
}

/// Производит расчет выходных данных профайлера
void UChannelProfiler::CalcProfilerOutputData(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);

 // рассчитываем выходные данные
 std::list<pair<std::string, UPerfomanceResults> > gui_prof_out_data;
 std::pair<std::string, UPerfomanceResults> perf;
 std::vector<UPerfomanceData> gui_perf_buf=GuiPerfomance;
 std::vector<std::string> gui_names=GuiNames;
 for(size_t i=0;i<gui_perf_buf.size();i++)
 {
  perf.first=gui_names[i];
  perf.second=gui_perf_buf[i];
  gui_prof_out_data.push_back(perf);
 }
 GuiProfilerOutputData=gui_prof_out_data;

 double sum(0.0), perc_sum(0.0);
 std::list<pair<std::string, UPerfomanceResults> > comp_prof_out_data;
 std::vector<std::string> comp_names=ComponentsName;
 std::vector<UPerfomanceData> comp_perf_buf=ComponentsPerfomance;
 UPerfomanceData model_perf=ModelPerfomance;
 UPerfomanceData other_perf=OtherPerfomance;
 //ComponentsProfilerOutputData=comp_prof_out_data; // очистка
 for(size_t i=0;i<comp_names.size();i++)
 {
  std::string legend=comp_names[i];

  if(legend.size()>10)
  {
   size_t n=legend.find_last_of('.');
   if(n != std::string::npos)
	legend=legend.substr(n+1);

   size_t j=0;
   for(;j<legend.size();j++)
	if(j>0 && !(j%8) && legend.size()-j>3)
	{
	 legend.insert(j,"\r\n"); j+=2;
	}
  }

  perf.first=legend;
  perf.second=comp_perf_buf[i];
  comp_prof_out_data.push_back(perf);
  sum+=comp_perf_buf[i].AvgDuration;
  perc_sum+=comp_perf_buf[i].Percentage;
 }
 if(!comp_names.empty())
 {
  perf.first="Others";
  perf.second.AvgDuration=model_perf.AvgDuration-sum;
  perf.second.Percentage=model_perf.Percentage-perc_sum;
  comp_prof_out_data.push_back(perf);
 }

 perf.first="Model";
 perf.second=ModelPerfomance;
 comp_prof_out_data.push_back(perf);

 perf.first="Overhead";
 perf.second.AvgDuration=other_perf.AvgDuration;
 perf.second.Percentage=other_perf.Percentage;
 comp_prof_out_data.push_back(perf);

 perf.first="Full Step";
 perf.second.AvgDuration=other_perf.AvgDuration+model_perf.AvgDuration;
 perf.second.Percentage=other_perf.Percentage+model_perf.Percentage;
 comp_prof_out_data.push_back(perf);

 perf.first="Gui";
 perf.second=SummaryGuiPerfomance;
 comp_prof_out_data.push_back(perf);
 ComponentsProfilerOutputData=comp_prof_out_data;
}

/// Производит полный расчет данных профайлера
void UChannelProfiler::Calculate(void)
{
 CalculateCore();
 CalculateGui();
 CalcProfilerOutputData();
}

void UChannelProfiler::CalculateCore(void)
{
 LoadCorePerfomanceData();
 CalcCorePerfomance();
}

void UChannelProfiler::CalculateGui(void)
{
 LoadGuiPerfomanceData();
 CalcGuiPerfomance();
}
/// --------------------------

/// --------------------------
/// Методы чтения статистики
/// --------------------------
/// Возавращает оценку производительности для компонент
UPerfomanceResults UChannelProfiler::GetComponentPerfomance(const std::string &name) const
{
 int i=FindComponentIndex(name);
 if(i<0)
  return UPerfomanceResults();

 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<UPerfomanceData> comp_perf_buf=ComponentsPerfomance;
 return comp_perf_buf[i];
}

UPerfomanceResults UChannelProfiler::GetComponentPerfomance(int index) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<UPerfomanceData> comp_perf_buf=ComponentsPerfomance;
 if(index<0 || index>=int(comp_perf_buf.size()))
  return UPerfomanceResults();
 return comp_perf_buf[index];
}

/// Возавращает оценку производительности для gui
UPerfomanceResults UChannelProfiler::GetGuiPerfomance(int index) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<UPerfomanceData> gui_perf_buf=GuiPerfomance;
 if(index<0 || index>=int(gui_perf_buf.size()))
  return UPerfomanceResults();
 return gui_perf_buf[index];
}

/// Производительность всей модели
UPerfomanceData UChannelProfiler::GetModelPerfomance(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return ModelPerfomance;
}

/// Производительность всего кроме модели
UPerfomanceData UChannelProfiler::GetOtherPerfomance(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return OtherPerfomance;
}

/// Суммарная производительность Gui
UPerfomanceData UChannelProfiler::GetSummaryGuiPerfomance(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return SummaryGuiPerfomance;
}

/// Возвращает интегральные данные о производительности
UIntegralPerfomanceResults UChannelProfiler::GetIntegralPerfomanceResults(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return IntegralPerfomanceResults;
}
/// --------------------------

/// --------------------------
/// Методы чтения обработанной статистики
/// --------------------------
/// Возвращает строку с текущим временем для режима RT
std::string UChannelProfiler::CalcRtTimeText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UIntegralPerfomanceResults integral_perf=IntegralPerfomanceResults;
 string result=string("MT: ")+RDK::get_text_time_from_seconds(integral_perf.ModelTime,'.', ':',true);
 result+=string(" RT: ")+RDK::get_text_time_from_seconds(integral_perf.RealTime,'.', ':',true);

 return result;
}

/// Возвращает строку с текущим временем для обычного режима
std::string UChannelProfiler::CalcNormalTimeText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UIntegralPerfomanceResults integral_perf=IntegralPerfomanceResults;
 string str_real_time=RDK::get_text_time_from_seconds(integral_perf.RealTime,'.', ':',true);
 return string("Time: ")+str_real_time;
}


/// Возвращает строку с производительностью для режима Rt
std::string UChannelProfiler::CalcRtPerfomanceText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UIntegralPerfomanceResults integral_perf=IntegralPerfomanceResults;
 return string("RT Coeff: ")+sntoa(integral_perf.RtPerfomance,3)+string("=")+sntoa(integral_perf.RtModelDuration,3)+string("/")+sntoa(integral_perf.RtCalcDuration,3);
}

/// Возвращает строку с текущей длительностью шага расчета для нормального режима
std::string UChannelProfiler::CalcNormalStepDurationText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UPerfomanceData model_perf=ModelPerfomance;
 UPerfomanceData other_perf=OtherPerfomance;
 return string("Step: ")+sntoa(model_perf.AvgDuration,3)+"/"+sntoa(model_perf.AvgDuration+other_perf.AvgDuration,3)+"s";
}

/// Возвращает строку с текущим FPS для нормального режима
std::string UChannelProfiler::CalcNormalFpsText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UIntegralPerfomanceResults integral_perf=IntegralPerfomanceResults;
 return string("Fps: ")+
                sntoa(integral_perf.FullFps,1)+string("/")+
                sntoa(integral_perf.ModelFps,1);
}


/// Список легенда-время в секундах для компонент и модели в целом
/// (см описание соответствующего параметра)
std::list<pair<std::string, UPerfomanceResults> > UChannelProfiler::GetComponentsProfilerOutputData(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return ComponentsProfilerOutputData;
}

/// Список легенда-время в секундах обновления Gui
/// элементы расположены подряд в соответствии с GuiNames
std::list<pair<std::string, UPerfomanceResults> > UChannelProfiler::GetGuiProfilerOutputData(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return GuiProfilerOutputData;
}

/// --------------------------


// --------------------------
// Общие методы управления контроллером
// --------------------------
// Сохраняет параметры интерфейса в xml
void UChannelProfiler::SaveParameters(RDK::USerStorageXML &xml)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 xml.WriteInteger("AverageIterations",AverageIterations);

 xml.SelectNodeForce("Components");
 xml.DelNodeInternalContent();
 std::vector<std::string> comp_names=ComponentsName;
 for(size_t i=0;i<comp_names.size();i++)
 {
  xml.WriteString(RDK::sntoa(i),comp_names[i]);
 }
 xml.SelectUp();

}

// Загружает параметры интерфейса из xml
void UChannelProfiler::LoadParameters(RDK::USerStorageXML &xml)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> components;
 xml.SelectNodeForce("Components");
 for(int i=0;i<xml.GetNumNodes();i++)
 {
  std::string name=xml.ReadString(i,"");
  components.push_back(name);
 }
 xml.SelectUp();
 SetComponentsName(components);
 SetAverageIterations(xml.ReadInteger("AverageIterations",10));
}
// --------------------------

}
