#ifndef RDK_SYSTEM_QT_CPP
#define RDK_SYSTEM_QT_CPP

#include <ctime>
#include "../rdk_system.h"     
//#include "USharedMemoryLoader.qt.cpp"
//#include "UGenericMutex.qt.cpp"
#include "UDllLoader.qt.cpp"
//#include "../UDllLoader.h"
#include <QtCore/QWaitCondition>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QMutex>
#include <QtCore/QDateTime>
#include <QtCore/QtDebug>

namespace RDK {

// ���������� ������� ����� � ������������� �� ���������� �������������� �������
// (������� �� ����������)
unsigned long long GetCurrentStartupTime(void)
{
 /*QDateTime timedata = QDateTime::currentDateTime();
 return timedata.hour()*60*60*1000 +
        timedata.minute()*60*1000 +
        timedata.second()*1000 +
        timedata.msec();*/
 return QDateTime::currentMSecsSinceEpoch();
}

// ���������� � seconds � useconds ������� �������� ������ � �����������,
// ��������� � ���������� �������������� �������
void GetTimeOfDayInMicroseconds(unsigned long long &seconds, unsigned long long &useconds)
{
 unsigned long long currentTime = GetCurrentStartupTime();
 seconds = currentTime/1000;
 useconds = (currentTime % 1000) * 1000;
}

// ��������� ������� �� ������� � �������������
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2)
{
 if(time1>time2)
  return time1-time2;
 else
  return time2-time1;
}


/// ���������� ��������� ����� � ���� (� ��������� �� �����������) �� ������ ������
double GetVariantLocalTime(void)
{
    return static_cast<double>(QDateTime::currentMSecsSinceEpoch())/86400000.0;
// TODO:
}


// �������� ������� �� �������� ����� �����������
void Sleep(int value)
{
 QWaitCondition sleep;
 QMutex mutex;
 mutex.lock();
 sleep.wait(&mutex, value);
 mutex.unlock();
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
    results.push_back(list[i].baseName().toLocal8Bit().constData());
  }
  else if(list[i].isFile()&&isfile)
  {
   results.push_back(list[i].fileName().toLocal8Bit().constData());
  }
 }
 return 0;
}

int RdkCopyFile(const std::string &source_file, const std::string &dest_file)
{
    QFile::copy(QString::fromLocal8Bit(source_file.c_str()), QString::fromLocal8Bit(dest_file.c_str()));
 return 1;
}

int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask)
{
 std::vector<std::string> results;

 int res=FindFilesList(source_dir, mask, true, results);
 if(!res)
 {
  for(size_t i=0;i<results.size();i++)
   if(RdkCopyFile(source_dir+results[i],dest_dir+results[i]))
    return 1;
 }
 return 0;
}


/// ������� ������������ ����� � ���������� ���, ���� ������ � �������
void RdkDebuggerMessage(const std::string &message)
{
    qDebug() << QString::fromLocal8Bit(message.c_str());
}

/// ������� ������� ���������� ������������ ��������� � �������� ��� ���� Load(dll_name)
RDK_LIB_TYPE UDllLoader* UCreateAndLoadDllLoader(const std::string &dll_name)
{
    UDllLoader *loader = new UDllLoaderQt(dll_name);
    loader->Load();
    return loader;
}

/// ������� ���������� ������� ���������� ������������ ����������, �� ��������� ����������
RDK_LIB_TYPE void UDestroyDllLoader(UDllLoader *handle)
{
    if(handle)
        delete handle;
}

/// ���������� ����� ������������ ����������� ������
/// ���� �� ������� ���������� �� ���������� false
bool ReadUsedMemoryInfo(unsigned long long &total_used_memory, unsigned long long &largest_free_block)
{
 return false;
}


/// ����������� ������� ������ � �������
std::string& narrow(const std::wstring& wstr, std::string &result, unsigned codepage)
{
 QString qs(QString::fromStdWString(wstr));
 result=qs.toStdString();
 return result;
 /*
 unsigned real_codepage=(codepage==0)?CP_ACP:codepage;

    int slength = (int)wstr.length() + 1;
    int len = WideCharToMultiByte(real_codepage, 0, wstr.c_str(), slength, 0, 0, 0, 0);
    result.resize(len, '\0');
    WideCharToMultiByte(real_codepage, 0, wstr.c_str(), slength, &result[0], len, 0, 0);

//	int r_size1=r.size();
//	int r_length1=r.length();
    if(!result.empty())
     result.resize(len-1);

//	int r_size2=r.size();
//	int r_length2=r.length();
    return result;*/
}

/// Deprecated
std::string& narrow(const std::wstring& wstr, const std::locale& loc, std::string &result)
{
 return narrow(wstr, result);
}


/// ����������� ������� ������ � �������
std::wstring& widen(const std::string& str, std::wstring &result, unsigned codepage)
{
 QString qs(QString::fromStdString(str.c_str()));
 result=qs.toStdWString();
 return result;
 /*
 unsigned real_codepage=(codepage==0)?CP_ACP:codepage;
    int slength = (int)str.length() + 1;
    int len = MultiByteToWideChar(real_codepage, 0, str.c_str(), slength, 0, 0);
    result.resize(len, L'\0');
    MultiByteToWideChar(real_codepage, 0, str.c_str(), slength, &result[0], len);

 //	int r_size1=r.size();
 //	int r_length1=r.length();
    if(!result.empty())
     result.resize(len-1);
 //	int r_size2=r.size();
 //	int r_length2=r.length();
    return result;*/
}


/// Deprecated
std::wstring& widen(const std::string& str, const std::locale& loc, std::wstring &result)
{
 return widen(str, result);
}

}
#endif

