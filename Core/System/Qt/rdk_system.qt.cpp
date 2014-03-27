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

namespace RDK {

// ���������� ������� ����� � ������������� �� ���������� �������������� �������
// (������� �� ����������)
unsigned long long GetCurrentStartupTime(void)
{
 time_t timedata;
 time(&timedata);
 return timedata*1000;
}

// ��������� ������� �� ������� � �������������
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 else
  return time2-time1;
}             

// �������� ������� �� �������� ����� �����������
void Sleep(int value)
{
 QWaitCondition sleep;
 QMutex mutex;
 sleep.wait(&mutex, ms);
}

// ������� �������
// ���������� 0 � ������ ������ ��� ���� ������� ��� ����������
// 1 - ���� ��� ���������� ���� � ����� ������
// 2 - ���� ����� ���� �� ����������
// 3 - ���� ��������� ������ ������
int CreateNewDirectory(const char* path)
{
 //��������� ������������� ����������
 QDir dir(path);
 QString dirName = dir.dirName();
 if(dir.exists())
	return 0;
 //��������� ������������� ����
 dir.cdUp();
 if(!dir.exists())
	return 2;
 //��������� ������������ �����
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

// �������� ������ ������ ��� ��������� �� ��������� ����
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
{
 //��������� ������������� ����������
 QDir dir(path.c_str());
 if(!dir.exists())
  return 1;
 //�������� ������ ������
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

}
#endif

