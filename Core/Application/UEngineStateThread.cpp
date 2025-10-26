#ifndef UENGINE_STATE_THREAD_CPP
#define UENGINE_STATE_THREAD_CPP

#include "UEngineStateThread.h"
#include "UEngineControlThread.h"
#include "UApplication.h"
#include "UEngineControl.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>

void ExceptionHandler(int channel_index)
{
 using namespace RDK;
 if(!UEngineStateThread::GetRdkExceptionHandlerMutex())
  return;

 UGenericMutexExclusiveLocker locker(UEngineStateThread::GetRdkExceptionHandlerMutex());
 std::list<int>& ch_indexes_ref=UEngineStateThread::GetUnsentLogChannelIndexes();

 if(find(ch_indexes_ref.begin(), ch_indexes_ref.end(),channel_index) == ch_indexes_ref.end())
  ch_indexes_ref.push_back(channel_index);
}

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineStateThread::UEngineStateThread(UEngineControl* engine_control)
: EngineControl(engine_control)
{
 if(!GetRdkExceptionHandlerMutex())
  GetRdkExceptionHandlerMutex()=UCreateMutex();

 GetUnsentLogChannelIndexes();
 #ifdef RDK_MUTEX_DEADLOCK_DEBUG
 TUThreadInfo info;
 info.Name="UEngineStateThread";
 GlobalThreadInfoMap[info.Pid]=info;
 #endif

 CalcState=UCreateEvent(false);

 CalcEnable=UCreateEvent(false);

 CalcStarted=UCreateEvent(false);

 CalculationNotInProgress=UCreateEvent(true);

 CalculationInProgress=UCreateMutex();

 Terminated=false;
 Thread=boost::thread(boost::bind(&UEngineStateThread::Execute, boost::ref(*this)));

 NumAvgIterations=200;
 AvgThreshold=5.0;
// EventsLogFlag=true;
// LogFlag=true;
}

UEngineStateThread::~UEngineStateThread(void)
{
 Terminate();

 UDestroyEvent(CalcState);
 UDestroyEvent(CalcEnable);
 UDestroyEvent(CalcStarted);
 UDestroyEvent(CalculationNotInProgress);
 UDestroyMutex(CalculationInProgress);

 if(GetRdkExceptionHandlerMutex())
 {
  UDestroyMutex(GetRdkExceptionHandlerMutex());
  GetRdkExceptionHandlerMutex()=0;
 }

}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Флаг разрешения логгирования
//bool UEngineStateThread::GetLogFlag(void) const
//{
// return LogFlag;
//}
//
//bool UEngineStateThread::SetLogFlag(bool value)
//{
// if(LogFlag == value)
//  return true;
//
// LogFlag=value;
// RecreateEventsLogFile();
// return true;
//}
// --------------------------

// --------------------------
// Методы доступа к данным состояния модулей
// --------------------------
/// Возвращает вектор состояний тредов
std::vector<UEngineStateThread::UCalcState> UEngineStateThread::ReadCalcThreadStates(void) const
{
 return CalcThreadStates;
}

/// Возвращает состояние одного потока
UEngineStateThread::UCalcState UEngineStateThread::ReadCalcThreadState(int channel_index)
{
 std::vector<UCalcState> tmp=CalcThreadStates;
 if(channel_index<0 || channel_index>int(tmp.size()))
  return csUnknown;

 return tmp[channel_index];
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
/// Возвращает класс-владелец потока
UEngineControl* UEngineStateThread::GetEngineControl(void)
{
 return EngineControl;
}


/// Регистрация потока расчета
void UEngineStateThread::RegisterCalcThread(int index, UEngineControlThread *calc_thread)
{
 int num_engines=Core_GetNumChannels();
 CalcThreads.resize(num_engines,0);
 if(index<0 || index>=num_engines)
  return;

 CalcThreads[index]=calc_thread;
}

void UEngineStateThread::UnRegisterCalcThread(int index)
{
 int num_engines=Core_GetNumChannels();
 CalcThreads.resize(num_engines,0);
 if(index<0 || index>=num_engines)
  return;

 CalcThreads[index]=0;
}

void UEngineStateThread::Execute(void)
{
 while(!Terminated)
 {
  try
  {
   if(CalcStarted->wait(100) == false)
   {
	if(!Terminated) // TODO: Эта проверка - костыль. не должно возникать такой ситуации. Поток должен остановится раньше, чем разрушится модель
	 ProcessLog();
	continue;
   }

   if(CalculationNotInProgress->wait(100) == false)
   {
	if(!Terminated) // TODO: Эта проверка - костыль. не должно возникать такой ситуации. Поток должен остановится раньше, чем разрушится модель
 	 ProcessLog();
	continue;
   }

   CalculationNotInProgress->reset();

   // Определяем состояние тредов расчета
   std::vector<UCalcState> calc_thread_states;

   int num_channels=Core_GetNumChannels();
   calc_thread_states.assign(num_channels,csStopped);
   CalcThreadStateTime.resize(num_channels,0);
   CalcThreadSuccessTime.resize(num_channels,0);
   AvgIterations.resize(num_channels);

   for(int i=0;i<num_channels;i++)
   {
	if(int(CalcThreads.size())<=i)
     continue;
	UEngineControlThread *thread=CalcThreads[i];
	if(thread)
	{
	 if(!thread->IsCalcStarted())
	 {
	  calc_thread_states[i]=csStopped;
	  continue;
	 }

	 double last_calc_time=thread->GetRealLastCalculationTime();
	 if(CalcThreadSuccessTime[i] != last_calc_time)
	 {
	  CalcThreadSuccessTime[i]=last_calc_time;
	  AvgIterations[i].push_back(last_calc_time);
	  if(int(AvgIterations[i].size())>NumAvgIterations)
	   AvgIterations[i].erase(AvgIterations[i].begin());

	  CalcThreadStateTime[i]=GetVariantLocalTime();
	  calc_thread_states[i]=csRunning;
	 }
	 else
	 {
	  double avg_diff(0.0);
	  std::list<double>::iterator I=AvgIterations[i].begin();
	  std::list<double>::iterator J=I;

	  if(I != AvgIterations[i].end())
	  {
       ++I;
	   for(;I!=AvgIterations[i].end();I++,J++)
	   {
		double diff=*I-*J;
		if(diff>avg_diff)
		 avg_diff=diff;
	   }
	   avg_diff/=1000;
	  }

	  if(fabs(avg_diff) < 1e-8 || (GetVariantLocalTime()-CalcThreadStateTime[i])*86400.0>AvgThreshold*avg_diff)
	   calc_thread_states[i]=csHanging;
	  else
	   calc_thread_states[i]=csRunning;
	 }
	}
	else
	{
	 calc_thread_states[i]=csStopped;
	}
   }

   CalcThreadStates=calc_thread_states;

   AdditionExecute();
  }
  catch(UException &ex)
  {
   CalculationNotInProgress->set();
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_DEBUG, (string("UEngineStateThread Rdk exception: ")+ex.what()).c_str());
  }
  catch(std::exception &ex)
  {
   CalculationNotInProgress->set();
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_DEBUG, (string("UEngineStateThread std exception: ")+ex.what()).c_str());
  }
  catch(...)
  {
   CalculationNotInProgress->set();
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_DEBUG, (string("UEngineStateThread unknown exception")).c_str());
  }

  ProcessLog();
  CalculationNotInProgress->set();
  Sleep(100);
 }
}

void UEngineStateThread::AdditionExecute(void)
{

}

/// Функция обеспечивает закрытие текущего файла логов и создание нового
//void UEngineStateThread::RecreateEventsLogFile(void)
//{
// if(!CalculationNotInProgress)
//  return;
// if(!CalculationNotInProgress->wait(100))
//  return;
// CalculationNotInProgress->reset();
//
// Logger.Clear();
// std::string log_dir;
// if(EngineControl && EngineControl->GetApplication())
//  log_dir=EngineControl->GetApplication()->CalcCurrentLogDir();
// else
//  log_dir="EventsLog/";
//
// Logger.SetLogDir(log_dir);
// if(Logger.InitLog() != RDK_SUCCESS)
// {
//  EventsLogFlag=false;
//  return;
// }
// else
// {
//  EventsLogFilePath=log_dir;
// }
//
// /// Сохраняем лог в файл если это необходимо
// if(!LogFlag)
//  EventsLogFlag=false;
// else
//  EventsLogFlag=true;
//
// CalculationNotInProgress->set();
//}
//
///// Закрывает текущий лог
//void UEngineStateThread::CloseEventsLogFile(void)
//{
// if(!CalculationNotInProgress)
//  return;
// if(!CalculationNotInProgress->wait(100))
//  return;
// CalculationNotInProgress->reset();
//
// Logger.Clear();
// CalculationNotInProgress->set();
//}

/// Временная переменная в которой хранится весь еще не отображенный в интерфейсе лог
/// Очищается каждый раз при запросе этой переменной
std::list<std::string> UEngineStateThread::ReadGuiUnsentLog(void)
{
 if(!CalculationInProgress->exclusive_lock(100))
  return std::list<std::string>();

// if(!CalculationNotInProgress)
//  return std::list<std::string>();
// if(!CalculationNotInProgress->wait(100))
//  return std::list<std::string>();
// CalculationNotInProgress->reset();

 std::list<std::string> buffer=GuiUnsentLog;

 GuiUnsentLog.clear();
 CalculationInProgress->exclusive_unlock();
// CalculationNotInProgress->set();
 return buffer;
}

/// Прерывает исполнение потока
void UEngineStateThread::Terminate(void)
{
 Terminated=true;
 Thread.join();
 CalcStarted->reset();

 if(!CalculationNotInProgress->wait(0) || !CalculationNotInProgress->wait(10000))
  return;
 CalculationNotInProgress->reset();
 CalcState->reset();
}

// Общедоступные данные логгирования
UGenericMutex*& UEngineStateThread::GetRdkExceptionHandlerMutex(void)
{
 static UGenericMutex* RdkExceptionHandlerMutex=0;
 return RdkExceptionHandlerMutex;
}

std::list<int>& UEngineStateThread::GetUnsentLogChannelIndexes(void)
{
 static std::list<int> UnsentLogChannelIndexes;
 return UnsentLogChannelIndexes;
}
// --------------------------

// --------------------------
// Вспомогательные методы
// --------------------------
void UEngineStateThread::ProcessLog(void)
{
 if(!GetRdkExceptionHandlerMutex())
  return;
 
 // Прочитать новые строки из файла логов glog
 ReadNewLogLines();
/*
 try
 {
  while(!UnsentLog.empty())
  {
   if(EventsLogFlag)
   {
	Logger.WriteMessageToFile(UnsentLog.front());// TODO: Проверить на RDK_SUCCESS
   }

   UnsentLog.pop_front();
  }
 }
 catch(...)
 {
  throw;
 }      */
}
// --------------------------

// --------------------------
// Методы мониторинга файла логов glog
// --------------------------

/// Сбросить позицию чтения файла
void UEngineStateThread::ResetLogFilePosition()
{
    CurrentLogFilePath.clear();
    LogFileReadPosition = 0;
    LastLogFileModTime = 0;
}

/// Получить путь к последнему файлу логов glog
std::string UEngineStateThread::GetLatestGlogFile()
{
    std::string log_dir = EngineControl->GetApplication()->CalcCurrentLogDir();
    
    // Формат имени файла glog: <program_name>.<hostname>.<user>.<severity>.<date>-<time>.<pid>
    // Ищем файлы *.INFO.* как основной файл логов
    
    DIR* dir = opendir(log_dir.c_str());
    if(!dir) return "";
    
    std::string latest_file;
    std::time_t latest_time = 0;
    
    struct dirent* entry;
    while((entry = readdir(dir)) != NULL)
    {
        std::string filename = entry->d_name;
        if(filename.find(".INFO.") != std::string::npos)
        {
            std::string full_path = log_dir + filename;
            struct stat st;
            if(stat(full_path.c_str(), &st) == 0)
            {
                if(st.st_mtime > latest_time)
                {
                    latest_time = st.st_mtime;
                    latest_file = full_path;
                }
            }
        }
    }
    closedir(dir);
    
    return latest_file;
}

/// Прочитать новые строки из файла логов
void UEngineStateThread::ReadNewLogLines()
{
    std::string log_file = GetLatestGlogFile();
    if(log_file.empty()) return;
    
    // Если файл изменился, сбросить позицию
    if(log_file != CurrentLogFilePath)
    {
        CurrentLogFilePath = log_file;
        LogFileReadPosition = 0;
    }
    
    std::ifstream file(log_file);
    if(!file.is_open()) return;
    
    // Перейти к последней прочитанной позиции
    file.seekg(LogFileReadPosition);
    
    std::string line;
    while(std::getline(file, line))
    {
        // Парсить строку glog: I1026 12:34:56.789012 12345 file.cpp:123] Message
        if(line.empty()) continue;
        
        // Конвертировать в формат RDK
        std::string formatted_line = ConvertGlogToRdkFormat(line);
        
        if(CalculationInProgress->exclusive_lock(10000))
        {
            GuiUnsentLog.push_back(formatted_line);
            CalculationInProgress->exclusive_unlock();
        }
    }
    
    // Сохранить позицию
    LogFileReadPosition = file.tellg();
}

/// Конвертировать формат glog в RDK
std::string UEngineStateThread::ConvertGlogToRdkFormat(const std::string& glog_line)
{
    // Формат glog: I1026 12:34:56.789012 12345 file.cpp:123] Message
    // Формат RDK: >1>2024.10.26 12:34:56> Message
    
    if(glog_line.empty()) return "";
    
    char severity = glog_line[0];
    int rdk_level = RDK_EX_INFO;
    
    switch(severity)
    {
        case 'I': rdk_level = RDK_EX_INFO; break;
        case 'W': rdk_level = RDK_EX_WARNING; break;
        case 'E': rdk_level = RDK_EX_ERROR; break;
        case 'F': rdk_level = RDK_EX_FATAL; break;
        default: rdk_level = RDK_EX_UNKNOWN; break;
    }
    
    // Найти начало сообщения (после ']')
    size_t msg_start = glog_line.find(']');
    if(msg_start == std::string::npos) return glog_line;
    
    std::string message = glog_line.substr(msg_start + 1);
    
    // Извлечь время из glog (MMDD HH:MM:SS)
    std::string time_str;
    if(glog_line.size() >= 18 && glog_line[0] >= 'A' && glog_line[0] <= 'Z')
    {
        // Формат: I1026 19:11:42.058125
        std::string month_day = glog_line.substr(1, 4);
        std::string time_part = glog_line.substr(6, 8);
        time_str = "2024." + month_day.substr(0, 2) + "." + month_day.substr(2, 2) + " " + time_part;
    }
    else
    {
        // Если не удалось распарсить, использовать текущее время
        time_t now = time(0);
        struct tm* tm_info = localtime(&now);
        char buffer[26];
        strftime(buffer, 26, "%Y.%m.%d %H:%M:%S", tm_info);
        time_str = buffer;
    }
    
    // Формат: >level>time> message
    std::ostringstream oss;
    oss << ">" << rdk_level << ">" << time_str << ">" << message;
    
    return oss.str();
}

// --------------------------


}

#endif
