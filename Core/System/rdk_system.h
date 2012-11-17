#ifndef RDK_SYSTEM_H
#define RDK_SYSTEM_H

namespace RDK {

// ���������� ������� ����� � ������������� �� ���������� �������������� �������
// (������� �� ����������)
unsigned long long GetCurrentStartupTime(void);

// ��������� ������� �� ������� � �������������
unsigned long long CalcDiffTime(unsigned long long time1, unsigned long long time2);

// �������� ������� �� �������� ����� �����������
void Sleep(int value);

// ������� �������
// ���������� 0 � ������ ������ ��� ���� ������� ��� ����������
// 1 - ���� ��� ���������� ���� � ����� ������
// 2 - ���� ����� ���� �� ����������
// 3 - ���� ��������� ������ ������
int CreateNewDirectory(const char* path);

}

#endif

