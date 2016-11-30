#ifndef RDK_SYSTEM_QT_CPP
#define RDK_SYSTEM_QT_CPP

#include <ctime>
#include "../rdk_system.h"     
#include "USharedMemoryLoader.qt.cpp"
#include "UGenericMutex.qt.cpp"
#include <QtCore/QWaitCondition>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QMutex>
#include <QtCore/QDateTime>
#include <QtCore/QtDebug>

namespace RDK {

// Возвращает текущее время в миллисекундах от некоторого фиксированного момента
// (зависит от реализации)
unsigned long long GetCurrentStartupTime(void)
{
 QTime timedata = QTime::currentTime();
 return timedata.hour()*60*60*1000 +
        timedata.minute()*60*1000 +
        timedata.second()*1000 +
        timedata.msec();
}

// Записывает в seconds и useconds текущие значения секунд и микросекунд,
// прошедших с некоторого фиксированного момента
void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds)
{
 unsigned long long currentTime = GetCurrentStartupTime();
 seconds = currentTime/1000;
 useconds = (currentTime % 1000) * 1000;
}

// Вычисляет разницу во времени в миллисекундах
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 else
  return time2-time1;
}


/// Возвращает локальное время в днях (с точностью до миллисекунд) от начала времен
double GetVariantLocalTime(void)
{
    return static_cast<double>(QDateTime::currentMSecsSinceEpoch())/86400000.0;
// TODO:
}


// Усыпляет процесс на заданное число миллисекунд
void Sleep(int value)
{
 QWaitCondition sleep;
 QMutex mutex;
 mutex.lock();
 sleep.wait(&mutex, value);
 mutex.unlock();
}

// Создает каталог
// Возвращает 0 в случае успеха или если каталог уже существует
// 1 - если уже существует файл с таким именем
// 2 - если такой путь не существует
// 3 - если произошла другая ошибка
int CreateNewDirectory(const char* path)
{
 //Проверить существование директории
 QDir dir(path);
 QString dirName = dir.dirName();
 if(dir.exists())
	return 0;
 //Проверить существование пути
 dir.cdUp();
 if(!dir.exists())
	return 2;
 //Проверить существоание файла
 QString tstp = QString(path)+QString(".*");
 QFileInfoList list = dir.entryInfoList();
 for(int i = 0; i<list.size();i++)
 {
  if(dirName==list[i].baseName())
   return 1;
 }
 if(dir.mkdir(dirName))
  return 0;
 else
  return 3;
}

// Получает список файлов или каталогов по заданному пути
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
{
 //Проверить существование директории
 QDir dir(path.c_str());
 if(!dir.exists())
  return 1;
 //Получить список файлов
 QFileInfoList list = dir.entryInfoList(QStringList(mask.c_str()));
 for(int i = 0; i<list.size();i++)
 {
  if(list[i].isDir()&&!isfile)
  {
   if(list[i].baseName()!="")
    results.push_back(list[i].baseName().toStdString());
  }
  else if(list[i].isFile()&&isfile)
  {
   results.push_back(list[i].fileName().toStdString());
  }
 }
 return 0;
}

int CopyFile(const std::string &source_file, const std::string &dest_file)
{
 return 1;
}

int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask)
{
 std::vector<std::string> results;

 int res=FindFilesList(source_dir, mask, true, results);
 if(!res)
 {
  for(size_t i=0;i<results.size();i++)
   if(CopyFile(source_dir+results[i],dest_dir+results[i]))
    return 1;
 }
 return 0;
}


/// Функция осуществляет вывод в отладочный лог, если сборка в отладке
void RdkDebuggerMessage(const std::string &message)
{
    qDebug() << QString::fromStdString(message);
}

}
#endif

