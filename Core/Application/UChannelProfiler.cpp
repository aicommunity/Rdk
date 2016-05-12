#include "UChannelProfiler.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "UIVisualController.h"

namespace RDK {

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
 : AvgDuration(0.0), Percentage(0.0)
{
}



/// ��������� ������ � �������
void UPerfomanceData::AddHistory(long long value, int max_values)
{
 CalcDurationHistory.push_back(value);
 while(int(CalcDurationHistory.size())>max_values)
  CalcDurationHistory.erase(CalcDurationHistory.begin());
}

/// ������� ������������������ ���������� ��� �������� ����������
//struct RDK_LIB_TYPE UPerfomanceData: public UPerfomanceResults
/// ������ ��������
void UPerfomanceData::CalcAverage(void)
{
 AvgDuration=0;

 for(size_t i=0;i<CalcDurationHistory.size();i++)
  AvgDuration+=double(CalcDurationHistory[i])/1000.0;

 if(!CalcDurationHistory.empty())
  AvgDuration/=double(CalcDurationHistory.size());
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
 Mutex=UCreateMutex();
 AverageIterations=10;
 ChannelIndex=0;
}

UChannelProfiler::~UChannelProfiler(void)
{
 if(Mutex)
  UDestroyMutex(Mutex);
 Mutex=0;
}
/// --------------------------

/// --------------------------
/// ������ ���������� �����������
/// --------------------------
/// ������ ������
int UChannelProfiler::GetChannelIndex(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return ChannelIndex;
}

bool UChannelProfiler::SetChannelIndex(int index)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(ChannelIndex == index)
  return true;
 ChannelIndex=index;
 return true;
}

/// ����� ����������
int UChannelProfiler::GetAverageIterations(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return AverageIterations;
}

bool UChannelProfiler::SetAverageIterations(int num)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(AverageIterations == num)
  return true;

 AverageIterations=num;
 return true;
}

/// ������ ������� ���� ����������� ���������
std::vector<std::string> UChannelProfiler::GetComponentsName(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return ComponentsName;
}

bool UChannelProfiler::SetComponentsName(const std::vector<std::string>& value)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(ComponentsName == value)
  return true;
 ComponentsName=value;
 ClearPerfomanceData();
 return true;
}

/// ��������� ����� ��������� � ������
bool UChannelProfiler::AddComponent(const std::string &name)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 ComponentsName.push_back(name);
 ClearPerfomanceData();
 return true;
}

/// ��������� ����� ��������� � ������
bool UChannelProfiler::AddAllComponents(const std::string &name)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 ComponentsName.clear();

 const char* components_name_list=MModel_GetComponentsNameList(ChannelIndex,name.c_str());
 std::vector<std::string> names;
 int numcomponents=RDK::separatestring<char>(components_name_list,names,',');
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
  ComponentsName.push_back(names[i]);
 }

 ClearPerfomanceData();
 return true;
}

/// ������� ���������
bool UChannelProfiler::DelComponent(const std::string &name)
{
 std::vector<std::string>::iterator I=find(ComponentsName.begin(),ComponentsName.end(),name);
 if(I != ComponentsName.end())
  ComponentsName.erase(I);
 ClearPerfomanceData();
 return true;
}

bool UChannelProfiler::DelComponent(int index)
{
 if(index>=0 && index<int(ComponentsName.size()))
 {
  ComponentsName.erase(ComponentsName.begin()+index);
  ClearPerfomanceData();
 }
 return true;
}

/// ��������� ����� ��������� � ������
void UChannelProfiler::DelAllComponents(void)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 ComponentsName.clear();
 ClearPerfomanceData();
}

/// ������ ���� ����������� �����������
std::vector<std::string> UChannelProfiler::GetGuiNames(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return GuiNames;
}

bool UChannelProfiler::SetGuiNames(const std::vector<std::string>& value)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(GuiNames == value)
  return true;
 GuiNames=value;
 ClearPerfomanceData();
 return true;
}

/// ��������� ��� ���������� � ������
bool UChannelProfiler::AddAllGui(void)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<RDK::UIVisualController*> &interfaces=RDK::UIVisualControllerStorage::InterfaceUpdaters;
 GuiNames.clear();
 for(size_t i=0;i<interfaces.size();i++)
 {
  GuiNames.push_back(interfaces[i]->GetName());
 }
 ClearPerfomanceData();
 return true;
}

/// ���������� ��� ��������� �� �������
std::string UChannelProfiler::GetComponentName(int index) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(index<0 || index>=int(ComponentsName.size()))
  return std::string("");
 return ComponentsName[index];
}

/// ���� ������ �� ����� ����������
int UChannelProfiler::FindComponentIndex(const std::string &name) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 std::vector<std::string>::const_iterator I=find(ComponentsName.begin(),ComponentsName.end(),name);
 if(I == ComponentsName.end())
  return -1;
 return int(I-ComponentsName.begin());
}

/// ���������� ��� gui �� �������
std::string UChannelProfiler::GetGuiName(int index) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(index<0 || index>=int(GuiNames.size()))
  return std::string("");
 return GuiNames[index];
}
/// --------------------------

/// --------------------------
/// ������ ���������� �������
/// --------------------------
/// ������� ��� ����������� ������
void UChannelProfiler::ClearPerfomanceData(void)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 ComponentsPerfomance.clear();
 GuiPerfomance.clear();
 SummaryGuiPerfomance=UPerfomanceData();
 ModelPerfomance=UPerfomanceData();
 OtherPerfomance=UPerfomanceData();
 IntegralPerfomanceResults=UIntegralPerfomanceResults();
 ComponentsPerfomance.resize(ComponentsName.size());
 GuiPerfomance.resize(GuiNames.size());
 GuiProfilerOutputData.clear();
 ComponentsProfilerOutputData.clear();
}

/// ��������� ������ ��� ���������� ����������
void UChannelProfiler::AddComponentPerfomanceData(int index, long long value)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(index<0 || index>=int(ComponentsPerfomance.size()))
  return;

 ComponentsPerfomance[index].AddHistory(value,AverageIterations);
}

void UChannelProfiler::AddComponentPerfomanceData(const std::string &name, long long value)
{
 AddComponentPerfomanceData(FindComponentIndex(name), value);
}

/// ��������� ������ ��� ���������� gui
void UChannelProfiler::AddGuiPerfomanceData(int index, long long value)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(index<0 || index>=int(GuiPerfomance.size()))
  return;

 GuiPerfomance[index].AddHistory(value,AverageIterations);
}

/// ��������� ���������� ����� ������ ���� � ������������������
void UChannelProfiler::LoadCorePerfomanceData(void)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 UELockPtr<UEngine> engine(GetEngineLock(ChannelIndex));
 UEPtr<UNet> model(GetModel<UNet>(ChannelIndex));
 if(!model)
  return;
 try
 {
  for(size_t i=0;i<ComponentsName.size();i++)
  {
   UEPtr<UNet> component=model->GetComponentL<UNet>(ComponentsName[i],true);
   if(component)
	AddComponentPerfomanceData(i, component->GetFullStepDuration());
  }
  long long full_step=model->GetFullStepDuration();
  long long interstep_interval=model->GetInterstepsInterval();
  ModelPerfomance.AddHistory(full_step,AverageIterations);
  OtherPerfomance.AddHistory(interstep_interval,AverageIterations);

  UEPtr<UEnvironment> env=GetEnvironment(ChannelIndex);
  IntegralPerfomanceResults.ModelTime=env->GetTime().GetDoubleTime();
  IntegralPerfomanceResults.RealTime=env->GetTime().GetDoubleRealTime();
  IntegralPerfomanceResults.RtCalcDuration=env->GetRTLastDuration();
  IntegralPerfomanceResults.RtModelDuration=env->GetRTModelCalcTime();
  IntegralPerfomanceResults.RtPerfomance=env->CalcRTPerformance();
 }
 catch(UException &ex)
 {
  engine->ProcessException(ex);
 }
}

/// ��������� ���������� ����� ������ gui � ������������������
void UChannelProfiler::LoadGuiPerfomanceData(void)
{
 std::vector<RDK::UIVisualController*> &interfaces=RDK::UIVisualControllerStorage::InterfaceUpdaters;
 for(size_t i=0;i<GuiPerfomance.size();i++)
 {
  AddGuiPerfomanceData(i, interfaces[i]->GetUpdateTime());
 }
 UGenericMutexExclusiveLocker locker(Mutex);
 SummaryGuiPerfomance.AddHistory(RDK::UIVisualControllerStorage::GetUpdateTime(),AverageIterations);
}

/// ���������� ������ ������� ������������������ ���� �� ����� ������
void UChannelProfiler::CalcCorePerfomance(void)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 ModelPerfomance.CalcAverage();
 OtherPerfomance.CalcAverage();
 double avg_full_step=ModelPerfomance.AvgDuration+OtherPerfomance.AvgDuration;
 ModelPerfomance.CalcPercentage(avg_full_step);
 OtherPerfomance.CalcPercentage(avg_full_step);

 if(ModelPerfomance.AvgDuration>0)
  IntegralPerfomanceResults.ModelFps=1.0/ModelPerfomance.AvgDuration;
 else
  IntegralPerfomanceResults.ModelFps=0.0;

 if(avg_full_step>0)
  IntegralPerfomanceResults.FullFps=1.0/avg_full_step;
 else
  IntegralPerfomanceResults.FullFps=0.0;

 for(size_t i=0;i<ComponentsPerfomance.size();i++)
 {
  ComponentsPerfomance[i].CalcAverage();
  ComponentsPerfomance[i].CalcPercentage(ModelPerfomance.AvgDuration);
 }
}

/// ���������� ������ ������� ������������������ gui �� ����� ������
void UChannelProfiler::CalcGuiPerfomance(void)
{
 UGenericMutexExclusiveLocker locker(Mutex);

 SummaryGuiPerfomance.CalcAverage();
 SummaryGuiPerfomance.CalcPercentage(SummaryGuiPerfomance.AvgDuration);

 for(size_t i=0;i<GuiPerfomance.size();i++)
 {
  GuiPerfomance[i].CalcAverage();
  GuiPerfomance[i].CalcPercentage(SummaryGuiPerfomance.AvgDuration);
 }
}

/// ���������� ������ �������� ������ ����������
void UChannelProfiler::CalcProfilerOutputData(void)
{
 UGenericMutexExclusiveLocker locker(Mutex);

 // ������������ �������� ������
 std::pair<std::string, UPerfomanceResults> perf;
 GuiProfilerOutputData.clear();
 for(size_t i=0;i<GuiPerfomance.size();i++)
 {
  perf.first=GuiNames[i];
  perf.second=GuiPerfomance[i];
  GuiProfilerOutputData.push_back(perf);
 }

 double sum(0.0), perc_sum(0.0);
 ComponentsProfilerOutputData.clear();
 for(size_t i=0;i<ComponentsName.size();i++)
 {
  std::string legend=ComponentsName[i];

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
  perf.second=ComponentsPerfomance[i];
  ComponentsProfilerOutputData.push_back(perf);
  sum+=ComponentsPerfomance[i].AvgDuration;
  perc_sum+=ComponentsPerfomance[i].Percentage;
 }
 if(!ComponentsName.empty())
 {
  perf.first="Others";
  perf.second.AvgDuration=ModelPerfomance.AvgDuration-sum;
  perf.second.Percentage=ModelPerfomance.Percentage-perc_sum;
  ComponentsProfilerOutputData.push_back(perf);
 }

 perf.first="Model";
 perf.second=ModelPerfomance;
 ComponentsProfilerOutputData.push_back(perf);

 perf.first="Gui";
 perf.second=SummaryGuiPerfomance;
 ComponentsProfilerOutputData.push_back(perf);
 perf.first="Overhead";
 perf.second.AvgDuration=OtherPerfomance.AvgDuration;
 perf.second.Percentage=OtherPerfomance.Percentage;
 ComponentsProfilerOutputData.push_back(perf);

 perf.first="Full Step";
 perf.second.AvgDuration=OtherPerfomance.AvgDuration+ModelPerfomance.AvgDuration;
 perf.second.Percentage=OtherPerfomance.Percentage+ModelPerfomance.Percentage;
 ComponentsProfilerOutputData.push_back(perf);
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

 UGenericMutexExclusiveLocker locker(Mutex);
 return ComponentsPerfomance[i];
}

UPerfomanceResults UChannelProfiler::GetComponentPerfomance(int index) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(index<0 || index>=int(ComponentsPerfomance.size()))
  return UPerfomanceResults();
 return ComponentsPerfomance[index];
}

/// ����������� ������ ������������������ ��� gui
UPerfomanceResults UChannelProfiler::GetGuiPerfomance(int index) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 if(index<0 || index>=int(GuiPerfomance.size()))
  return UPerfomanceResults();
 return GuiPerfomance[index];
}

/// ������������������ ���� ������
UPerfomanceData UChannelProfiler::GetModelPerfomance(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return ModelPerfomance;
}

/// ������������������ ����� ����� ������
UPerfomanceData UChannelProfiler::GetOtherPerfomance(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return OtherPerfomance;
}

/// ��������� ������������������ Gui
UPerfomanceData UChannelProfiler::GetSummaryGuiPerfomance(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return SummaryGuiPerfomance;
}

/// ���������� ������������ ������ � ������������������
UIntegralPerfomanceResults UChannelProfiler::GetIntegralPerfomanceResults(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return IntegralPerfomanceResults;
}
/// --------------------------

/// --------------------------
/// ������ ������ ������������ ����������
/// --------------------------
/// ���������� ������ � ������� �������� ��� ������ RT
std::string UChannelProfiler::CalcRtTimeText(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 string str_model_time=RDK::get_text_time_from_seconds(IntegralPerfomanceResults.ModelTime);
 double diff=IntegralPerfomanceResults.RealTime-IntegralPerfomanceResults.ModelTime;
 string str_diff_time=RDK::get_text_time_from_seconds(diff);
 string result=string("Time: ")+str_model_time.c_str();
 if(diff>0)
  result+=string(" (")+str_diff_time+")";
 return result;
}

/// ���������� ������ � ������� �������� ��� �������� ������
std::string UChannelProfiler::CalcNormalTimeText(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 string str_real_time=RDK::get_text_time_from_seconds(IntegralPerfomanceResults.RealTime);
 return string("Time: ")+str_real_time;
}


/// ���������� ������ � ������������������� ��� ������ Rt
std::string UChannelProfiler::CalcRtPerfomanceText(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);

 return string("RT: ")+sntoa(IntegralPerfomanceResults.RtPerfomance,3)+string("=")+sntoa(IntegralPerfomanceResults.RtModelDuration,3)+string("/")+sntoa(IntegralPerfomanceResults.RtCalcDuration,3);
}

/// ���������� ������ � ������� ������������� ���� ������� ��� ����������� ������
std::string UChannelProfiler::CalcNormalStepDurationText(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);

 return string("Step: ")+sntoa(ModelPerfomance.AvgDuration,3)+"/"+sntoa(ModelPerfomance.AvgDuration+OtherPerfomance.AvgDuration,3)+"s";
}

/// ���������� ������ � ������� FPS ��� ����������� ������
std::string UChannelProfiler::CalcNormalFpsText(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return string("Fps: ")+
				sntoa(IntegralPerfomanceResults.FullFps,1)+string("/")+
				sntoa(IntegralPerfomanceResults.ModelFps,1);
}


/// ������ �������-����� � �������� ��� ��������� � ������ � �����
/// (�� �������� ���������������� ���������)
std::list<pair<std::string, UPerfomanceResults> > UChannelProfiler::GetComponentsProfilerOutputData(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return ComponentsProfilerOutputData;
}

/// ������ �������-����� � �������� ���������� Gui
/// �������� ����������� ������ � ������������ � GuiNames
std::list<pair<std::string, UPerfomanceResults> > UChannelProfiler::GetGuiProfilerOutputData(void) const
{
 UGenericMutexExclusiveLocker locker(Mutex);
 return GuiProfilerOutputData;
}

/// --------------------------


// --------------------------
// ����� ������ ���������� ������������
// --------------------------
// ��������� ��������� ���������� � xml
void UChannelProfiler::SaveParameters(RDK::USerStorageXML &xml)
{
 UGenericMutexExclusiveLocker locker(Mutex);
 xml.WriteInteger("AverageIterations",AverageIterations);

 xml.SelectNodeForce("Components");
 xml.DelNodeInternalContent();
 for(size_t i=0;i<ComponentsName.size();i++)
 {
  xml.WriteString(RDK::sntoa(i),ComponentsName[i]);
 }
 xml.SelectUp();

}

// ��������� ��������� ���������� �� xml
void UChannelProfiler::LoadParameters(RDK::USerStorageXML &xml)
{
 UGenericMutexExclusiveLocker locker(Mutex);
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
