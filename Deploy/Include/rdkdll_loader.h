#ifndef rdkdll_loaderH
#define rdkdll_loaderH

#include "rdk.h"

namespace RDK {

// ��������� ���������� � �������������� ��� ��������� �� �������
// ���������� 0 � ������ ������
int LoadDll(const char *dllname);

// ��������� ���������� � ���������� ��������� �� ���
UALibrary* LoadULibrary(const string &dllname, const string &libname);

// ��������� ����� � ���������� ��������� �� ����
UAContainer* LoadUClass(const string &dllname, const string &classname);

// ��������� ������ ������������� � ���������� ��������� �� Storage � Environment
// �������� ��, ���� ������� � dll �� �������
// ���������� 0 � ������ ������
//int DllInit(RDK::UIniFile<char> &inifile, const string &libsection,
}
#endif
