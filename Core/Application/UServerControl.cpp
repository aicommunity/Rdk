#include "UServerControl.h"

#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "UProject.h"
#include "UApplication.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UServerControl::UServerControl(void)
{
 AutoStartFlag=false;
 ServerName="Server";
 ServerId="Server1";
 AverageIterations=4;
 PerformancePushIndex=0;
 DebugFolder = "";
 InitFlag=false;

 Name="ServerControl";
}

UServerControl::~UServerControl(void)
{

}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
/// Флаг разрешения запуска сервера при старте
bool UServerControl::GetAutoStartFlag(void) const
{
 return AutoStartFlag;
}

bool UServerControl::SetAutoStartFlag(bool value)
{
 if(AutoStartFlag == value)
  return true;
 AutoStartFlag=value;
 return true;
}

/// Имя сервера
const std::string& UServerControl::GetServerName(void) const
{
 return ServerName;
}

bool UServerControl::SetServerName(const std::string& value)
{
 ServerName=value;
 return true;
}

/// Идентификатор сервера
const std::string& UServerControl::GetServerId(void) const
{
 return ServerId;
}

bool UServerControl::SetServerId(const std::string& value)
{
 ServerId=value;
 return true;
}

/// Число шагов усреднения оценки производительности
int UServerControl::GetAverageIterations(void) const
{
 return AverageIterations;
}

bool UServerControl::SetAverageIterations(int value)
{
 AverageIterations=value;
 return true;
}

const std::string& UServerControl::GetMetaComponentName(void) const
{
 return MetaComponentName;
}

bool UServerControl::SetMetaComponentName(const std::string& value)
{
 MetaComponentName=value;
 return true;
}

const std::string& UServerControl::GetMetaComponentStateName(void) const
{
 return MetaComponentStateName;
}

bool UServerControl::SetMetaComponentStateName(const std::string& value)
{
 MetaComponentStateName=value;
 return true;
}

///Папка для сохранения отладочных данных
const std::string& UServerControl::GetDebugFolder(void) const
{
 return DebugFolder;
}

bool UServerControl::SetDebugFolder(const std::string& value)
{
 if(DebugFolder == value)
  return true;
 DebugFolder=value;
 return true;
}

///Полный путь на одну сессию
const std::string& UServerControl::GetDebugOutputPath(void) const
{
 return DebugOutputPath;
}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Возвращает указатель на экземпляр приложения
UEPtr<UApplication> UServerControl::GetApplication(void)
{
 return Application;
}

bool UServerControl::SetApplication(UEPtr<UApplication> value)
{
 if(Application == value)
  return true;

 Application=value;
 return true;
}
// --------------------------

// --------------------------
// Данные для оценки производительности и сохранения отладочной информации
// --------------------------
/// Результаты измерений производительности, мс
const std::vector<std::vector<RDK::ULongTime> >& UServerControl::GetModelPerformanceResults(void) const
{
 return ModelPerformanceResults;
}

const std::vector<std::vector<RDK::ULongTime> >& UServerControl::GetTransportPerformanceResults(void) const
{
 return TransportPerformanceResults;
}

/// Средние значения затраченного времени на расчет модели и обмена
const std::vector<RDK::ULongTime>& UServerControl::GetModelAvg(void) const
{
 return ModelAvg;
}

const std::vector<RDK::ULongTime>& UServerControl::GetTransportAvg(void) const
{
 return TransportAvg;
}
// --------------------------


// --------------------------
// Методы управления
// --------------------------
/// Инициализация (выполняется первой)
void UServerControl::Init(void)
{
 if(IsInit())
  return;

 InitFlag=true;
}

/// Деинициализация (выполняется последней)
void UServerControl::UnInit(void)
{
 if(!IsInit())
  return;

 InitFlag=false;
}

/// Проверка состояния инициализации
bool UServerControl::IsInit(void) const
{
 return InitFlag;
}
// --------------------------

// --------------------------
// Методы управления вещателями
// --------------------------
/// Регистрирует удаленный приемник метаданных
int UServerControl::RegisterMetadataReceiver(const std::string &address, int port)
{
 return 1;
}

/// Удаляет удаленный приемник метаданных
int UServerControl::UnRegisterMetadataReceiver(const std::string &address, int port)
{
 return 1;
}
// --------------------------

// --------------------------
// Общие методы управления контроллером
// --------------------------
// Сохраняет параметры интерфейса в xml
void UServerControl::SaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("AverageIterations",AverageIterations);
// xml.WriteInteger("ServerControlPort", GetServerBindingPort());
// xml.WriteString("ServerControlAddress", GetServerBindingInterfaceAddress());

// xml.WriteInteger("Num",GetNumEngines());
 xml.WriteInteger("AutoStartFlag",AutoStartFlag);
 xml.WriteString("ServerName",ServerName);
 xml.WriteString("ServerId",ServerId);

 xml.WriteString("MetadataComponentName",MetaComponentName);
 xml.WriteString("MetadataComponentStateName",MetaComponentStateName);

}

// Загружает параметры интерфейса из xml
void UServerControl::LoadParameters(RDK::USerStorageXML &xml)
{
 SetAverageIterations(xml.ReadInteger("AverageIterations",AverageIterations));

 SetMetaComponentName(xml.ReadString("MetadataComponentName",""));
 SetMetaComponentStateName(xml.ReadString("MetadataComponentStateName",""));

 int source_num_channels=xml.ReadInteger("NumberOfChannels",Core_GetNumChannels());
// SetNumEngines(source_num_channels);
 TProjectConfig config=GetApplication()->GetProjectConfig();
 for(size_t i=0;i<GetApplication()->GetProjectConfig().ChannelsConfig.size();i++)
 {
  config.ChannelsConfig[i].ChannelName=xml.ReadString(std::string("ChannelName_")+RDK::sntoa(i),RDK::sntoa(i));
 }
 GetApplication()->SetProjectConfig(config);

 SetAutoStartFlag(xml.ReadInteger("AutoStartFlag",true));
 SetServerName(xml.ReadString("ServerName","Server"));
 SetServerId(xml.ReadString("ServerId","Server"));
}
// --------------------------

// --------------------------
// Вспомогательные методы
// --------------------------
/// Рассчитывает производительность сервера
/// и сохраняет на диск логи производительности
void UServerControl::CalculatePerformance(void)
{
 if(PerformancePushIndex>=int(ModelPerformanceResults.size()))
  PerformancePushIndex=0;

 if(ModelPerformanceResults.size() == 0)
  AfterReset();
 int num_channels=Core_GetNumChannels();
 ModelPerformanceResults[PerformancePushIndex].assign(num_channels,0);
 TransportPerformanceResults[PerformancePushIndex].assign(num_channels,0);
 for(size_t i=0;i<ModelPerformanceResults[PerformancePushIndex].size();i++)
 {
  UELockPtr<UEngine> engine=GetEngineLock(int(i));
  if(!engine)
   continue;
  if(!engine->Model_Check())
   continue;
  RDK::ULongTime model_time=engine->Model_GetFullStepDuration("");
  RDK::ULongTime ext_gui=engine->Model_GetInterstepsInterval("");
  ModelPerformanceResults[PerformancePushIndex][i]=model_time;
  TransportPerformanceResults[PerformancePushIndex][i]=ext_gui;
 }
 ++PerformancePushIndex;
 if(PerformancePushIndex>=int(ModelPerformanceResults.size()))
  PerformancePushIndex=0;

 ModelAvg.assign(num_channels,0);
 TransportAvg.assign(num_channels,0);
 int sum_number=0;

 /// Результаты измерений производительности, мс
 const std::vector<std::vector<RDK::ULongTime> >& model_performance=ModelPerformanceResults;
 const std::vector<std::vector<RDK::ULongTime> >& transport_performance=TransportPerformanceResults;

 for(size_t i=0;i<model_performance.size();i++)
 {
  for(size_t j=0;j<model_performance[i].size();j++)
  {
   ModelAvg[j]+=model_performance[i][j];
   TransportAvg[j]+=transport_performance[i][j];
  }
  if(model_performance[i].size()>0)
   ++sum_number;
 }

 if(sum_number>0)
  for(size_t j=0;j<ModelAvg.size();j++)
  {
   ModelAvg[j]/=sum_number;
   TransportAvg[j]/=sum_number;
  }
  /*
 perf_data.resize(ModelAvg.size());
 aver_perf_data.resize(ModelAvg.size());
 for(size_t j=0;j<ModelAvg.size();j++)
 {
  perf_data[j].push_back(ModelAvg[j]);
 }

 ///Количество отсчетов для усреднения
 const int average_size = 20;

 for(unsigned int k=0; k<ModelAvg.size(); k++)
 {
	 unsigned long long current_perf=0;
	 if(perf_data[k].size()<=average_size)
	 {
		unsigned long long sum=0;
		for(unsigned int i=0; i<perf_data[k].size(); i++)
		{
			sum+=perf_data[k][i];
		}
		current_perf = sum/perf_data[k].size();
	 }
	 else
	 {
		unsigned long long sum=0;
		for(unsigned int i=perf_data[k].size()-average_size; i<perf_data[k].size(); i++)
		{
			sum+=perf_data[k][i];
		}
		current_perf = sum/average_size;
	 }
	 aver_perf_data[k].push_back(current_perf);
 }

 std::ofstream f;
 //Сформировать строку названия
	if(DebugOutputPath.empty())
	{
	 time_t t = time(NULL);
	 std::string st = ctime(&t);
	 st[st.size()-1]='-';
	 for(int i=0; i<st.size(); i++)
	 {
		 if(st[i]==' ')
			st[i]='_';
		 if(st[i]==':')
			st[i]='.';
	 }
	 if(!DebugFolder.empty()>0)
	 {
		std::string s = DebugFolder+st;
		RDK::CreateNewDirectory(s.c_str());
		DebugOutputPath = DebugFolder+st+"/";//"D:\\ML2\\";
	 }
	}

 f.open((DebugOutputPath+"performance.m").c_str());//String("D:\\Output\\performance.m").c_str());
 for(unsigned int k=0; k<aver_perf_data.size(); k++)
 {
	 f<<"P_"<<k<<" = [";
	 for(unsigned int i=0; i<aver_perf_data[k].size(); i++)
	 {
		 f<<aver_perf_data[k][i]<<"; ";
	 }
	 f<<"];\n\n";
 }
 f<<"Idx = [";
 if(!aver_perf_data.empty())
 for(unsigned int k=0; k<aver_perf_data[0].size(); k++)
 {
	f<<k<<"; ";
 }
 f<<"];\n\n";
 f<<"plot(";
 for(unsigned int k=0; k<aver_perf_data.size(); k++)
 {
	 f<<"Idx, P_"<<k<<",'k-'";
	 if(k<aver_perf_data.size()-1)
		f<<",";
 }
 f<<")";
 f.close();  */
}

// Метод, вызываемый после сброса модели
void UServerControl::AfterReset(void)
{
 PerformancePushIndex=0;
 if(AverageIterations<=0)
  AverageIterations=4;
 ModelPerformanceResults.resize(AverageIterations);
 TransportPerformanceResults.resize(AverageIterations);
 for(size_t i=0;i<ModelPerformanceResults.size();i++)
 {
  ModelPerformanceResults[i].clear();
  TransportPerformanceResults[i].clear();
 }
}

// Метод, вызываемый после шага расчета
void UServerControl::AfterCalculate(void)
{
 CalculatePerformance();
}
// --------------------------


// --------------------------
/// Управление числом каналов
/// Выполнение вспомогательных методов
/// Вызывается из UApplication
// --------------------------
bool UServerControl::SetNumChannels(int num)
{

 if(!ASetNumChannels(num))
  return false;
 AfterReset();
 return true;
}

bool UServerControl::ASetNumChannels(int num)
{
 return true;
}

bool UServerControl::InsertChannel(int index)
{

 if(!AInsertChannel(index))
  return false;
 AfterReset();
 return true;
}

bool UServerControl::AInsertChannel(int index)
{
 return true;
}

bool UServerControl::DeleteChannel(int index)
{

 if(!ADeleteChannel(index))
  return false;
 AfterReset();
 return true;
}

bool UServerControl::ADeleteChannel(int index)
{
 return true;
}
// --------------------------


}
