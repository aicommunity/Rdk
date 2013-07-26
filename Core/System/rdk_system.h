#ifndef RDK_SYSTEM_H
#define RDK_SYSTEM_H   

#include <vector>
#include <string>
#include "USharedMemoryLoader.h"

namespace RDK {

/// ���������� ������� ����� � ������������� �� ���������� �������������� �������
/// (������� �� ����������)
unsigned long long GetCurrentStartupTime(void);

/// ��������� ������� �� ������� � �������������
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2);

/// �������� ������� �� �������� ����� �����������
void Sleep(int value);

/// ������� �������
/// ���������� 0 � ������ ������ ��� ���� ������� ��� ����������
/// 1 - ���� ��� ���������� ���� � ����� ������
/// 2 - ���� ����� ���� �� ����������
/// 3 - ���� ��������� ������ ������
int CreateNewDirectory(const char* path);    

/// �������� ������ ������ ��� ��������� �� ��������� ����
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results);
          
/// �������� ����
int CopyFile(const std::string &source_file, const std::string &dest_file);
                
/// �������� ������� � ����������
int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask);



}

#endif

