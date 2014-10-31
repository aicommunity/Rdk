#ifndef RDK_SYSTEM_H
#define RDK_SYSTEM_H   

#include <vector>
#include <string>
#include "USharedMemoryLoader.h"
#include "UGenericMutex.h"


namespace RDK {

extern "C++" {

/// ���������� ������� ����� � ������������� �� ���������� �������������� �������
/// (������� �� ����������)
RDK_LIB_TYPE unsigned long long GetCurrentStartupTime(void);

/// ��������� ������� �� ������� � �������������
RDK_LIB_TYPE unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2);

/// �������� ������� �� �������� ����� �����������
RDK_LIB_TYPE void Sleep(int value);

/// ������� �������
/// ���������� 0 � ������ ������ ��� ���� ������� ��� ����������
/// 1 - ���� ��� ���������� ���� � ����� ������
/// 2 - ���� ����� ���� �� ����������
/// 3 - ���� ��������� ������ ������
RDK_LIB_TYPE int CreateNewDirectory(const char* path);

/// �������� ������ ������ ��� ��������� �� ��������� ����
RDK_LIB_TYPE int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results);

/// �������� ����
RDK_LIB_TYPE int RdkCopyFile(const std::string &source_file, const std::string &dest_file);

/// �������� ������� � ����������
RDK_LIB_TYPE int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask);


}

}

#endif

