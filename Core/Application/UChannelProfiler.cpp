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



/// ��������� ������ � �������
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

/// ������� ������������������ ���������� ��� �������� ����������
//struct RDK_LIB_TYPE UPerfomanceData: public UPerfomanceResults
/// ������ ������
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

  if(MinDuration>static_cast<double>(curr.Duration))
   MinDuration=static_cast<double>(curr.Duration);

  if(MaxDuration<static_cast<double>(curr.Duration))
   MaxDuration=static_cast<double>(curr.Duration);

  if(MinInterval>static_cast<double>(curr.Interval))
   MinInterval=static_cast<double>(curr.Interval);

  if(MaxInterval<static_cast<double>(curr.Interval))
   MaxInterval=static_cast<double>(curr.Interval);
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

/// ������ �������� �� ������ ��������
void UPerfomanceData::CalcPercentage(double full_time)
{
 if(full_time>0)
  Percentage=AvgDuration/full_time;
 else
  Percentage=0;
}


/// --------------------------
/// ������������ � �����������
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
/// ������ ���������� �����������
/// --------------------------
/// ������ ������
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

/// ����� ����������
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

/// ������ ������� ���� ����������� ���������
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

/// ��������� ����� ��������� � ������
bool UChannelProfiler::AddComponent(const std::string &name)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> comp_names=ComponentsName;
 comp_names.push_back(name);
 ComponentsName=comp_names;
 ClearPerfomanceData();
 return true;
}

/// ��������� ����� ��������� � ������
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

/// ������� ���������
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

/// ��������� ����� ��������� � ������
void UChannelProfiler::DelAllComponents(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> comp_names;
 ComponentsName=comp_names;
 ClearPerfomanceData();
}

/// ������ ���� ����������� �����������
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

/// ��������� ��� ���������� � ������
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

/// ���������� ��� ��������� �� �������
std::string UChannelProfiler::GetComponentName(int index) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> comp_names=ComponentsName;
 if(index<0 || index>=int(comp_names.size()))
  return std::string("");
 return comp_names[index];
}

/// ���� ������ �� ����� ����������
int UChannelProfiler::FindComponentIndex(const std::string &name) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string> comp_names=ComponentsName;
 std::vector<std::string>::const_iterator I=find(comp_names.begin(),comp_names.end(),name);
 if(I == comp_names.end())
  return -1;
 return int(I-comp_names.begin());
}

/// ���������� ��� gui �� �������
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
/// ������ ���������� �������
/// --------------------------
/// ������� ��� ����������� ������
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

/// ��������� ������ ��� ���������� ����������
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

/// ��������� ������ ��� ���������� gui
void UChannelProfiler::AddGuiPerfomanceData(int index, long long value, long long interval)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<UPerfomanceData> gui_perf_buf=GuiPerfomance;
 if(index<0 || index>=int(gui_perf_buf.size()))
  return;

 gui_perf_buf[index].AddHistory(value,interval,AverageIterations);
 GuiPerfomance=gui_perf_buf;
}

/// ��������� ���������� ����� ������ ���� � ������������������
void UChannelProfiler::LoadCorePerfomanceData(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UELockPtr<UEngine> engine(GetEngineLock(ChannelIndex));
 std::shared_ptr<UNet> model(GetModel<UNet>(ChannelIndex));
 if(!model)
  return;
 RDK_SYS_TRY
 {
  try
  {
   std::vector<std::string> comp_names=ComponentsName;
   for(size_t i=0;i<comp_names.size();i++)
   {
    // CRITICAL: GetComponentL now returns weak_ptr, need to lock
    std::weak_ptr<UContainer> comp_weak = model->GetComponentL(comp_names[i],true);
    if(comp_weak.expired())
     continue;
    std::shared_ptr<UContainer> comp_cont = comp_weak.lock();
    if(!comp_cont)
     continue;
    std::shared_ptr<UNet> component = std::dynamic_pointer_cast<UNet>(comp_cont);
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

   std::shared_ptr<UEnvironment> env=GetEnvironment(ChannelIndex);
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

/// ��������� ���������� ����� ������ gui � ������������������
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

/// ���������� ������ ������� ������������������ ���� �� ����� ������
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

/// ���������� ������ ������� ������������������ gui �� ����� ������
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

/// ���������� ������ �������� ������ ����������
void UChannelProfiler::CalcProfilerOutputData(void)
{
 // UGenericMutexExclusiveLocker locker(Mutex);

 // ������������ �������� ������
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
 //ComponentsProfilerOutputData=comp_prof_out_data; // �������
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

/// ���������� ������ ������ ������ ����������
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
/// ������ ������ ����������
/// --------------------------
/// ����������� ������ ������������������ ��� ���������
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

/// ����������� ������ ������������������ ��� gui
UPerfomanceResults UChannelProfiler::GetGuiPerfomance(int index) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<UPerfomanceData> gui_perf_buf=GuiPerfomance;
 if(index<0 || index>=int(gui_perf_buf.size()))
  return UPerfomanceResults();
 return gui_perf_buf[index];
}

/// ������������������ ���� ������
UPerfomanceData UChannelProfiler::GetModelPerfomance(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return ModelPerfomance;
}

/// ������������������ ����� ����� ������
UPerfomanceData UChannelProfiler::GetOtherPerfomance(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return OtherPerfomance;
}

/// ��������� ������������������ Gui
UPerfomanceData UChannelProfiler::GetSummaryGuiPerfomance(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return SummaryGuiPerfomance;
}

/// ���������� ������������ ������ � ������������������
UIntegralPerfomanceResults UChannelProfiler::GetIntegralPerfomanceResults(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return IntegralPerfomanceResults;
}
/// --------------------------

/// --------------------------
/// ������ ������ ������������ ����������
/// --------------------------
/// ���������� ������ � ������� �������� ��� ������ RT
std::string UChannelProfiler::CalcRtTimeText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UIntegralPerfomanceResults integral_perf=IntegralPerfomanceResults;
 string result=string("MT: ")+RDK::get_text_time_from_seconds(integral_perf.ModelTime,'.', ':',true);
 result+=string(" RT: ")+RDK::get_text_time_from_seconds(integral_perf.RealTime,'.', ':',true);

 return result;
}

/// ���������� ������ � ������� �������� ��� �������� ������
std::string UChannelProfiler::CalcNormalTimeText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UIntegralPerfomanceResults integral_perf=IntegralPerfomanceResults;
 string str_real_time=RDK::get_text_time_from_seconds(integral_perf.RealTime,'.', ':',true);
 return string("Time: ")+str_real_time;
}


/// ���������� ������ � ������������������� ��� ������ Rt
std::string UChannelProfiler::CalcRtPerfomanceText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UIntegralPerfomanceResults integral_perf=IntegralPerfomanceResults;
 return string("RT Coeff: ")+sntoa(integral_perf.RtPerfomance,3)+string("=")+sntoa(integral_perf.RtModelDuration,3)+string("/")+sntoa(integral_perf.RtCalcDuration,3);
}

/// ���������� ������ � ������� ������������� ���� ������� ��� ����������� ������
std::string UChannelProfiler::CalcNormalStepDurationText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UPerfomanceData model_perf=ModelPerfomance;
 UPerfomanceData other_perf=OtherPerfomance;
 return string("Step: ")+sntoa(model_perf.AvgDuration,3)+"/"+sntoa(model_perf.AvgDuration+other_perf.AvgDuration,3)+"s";
}

/// ���������� ������ � ������� FPS ��� ����������� ������
std::string UChannelProfiler::CalcNormalFpsText(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 UIntegralPerfomanceResults integral_perf=IntegralPerfomanceResults;
 return string("Fps: ")+
                sntoa(integral_perf.FullFps,1)+string("/")+
                sntoa(integral_perf.ModelFps,1);
}


/// ������ �������-����� � �������� ��� ��������� � ������ � �����
/// (�� �������� ���������������� ���������)
std::list<pair<std::string, UPerfomanceResults> > UChannelProfiler::GetComponentsProfilerOutputData(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return ComponentsProfilerOutputData;
}

/// ������ �������-����� � �������� ���������� Gui
/// �������� ����������� ������ � ������������ � GuiNames
std::list<pair<std::string, UPerfomanceResults> > UChannelProfiler::GetGuiProfilerOutputData(void) const
{
 // UGenericMutexExclusiveLocker locker(Mutex);
 return GuiProfilerOutputData;
}

/// --------------------------


// --------------------------
// ����� ������ ���������� ������������
// --------------------------
// ��������� ��������� ���������� � xml
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

// ��������� ��������� ���������� �� xml
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
