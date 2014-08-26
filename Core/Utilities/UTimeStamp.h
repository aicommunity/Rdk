/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2005.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru
Version:        3.0.0

This file - part of the project: RDK

File License:		BSD License
Project License:	BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UTimeStampH
#define UTimeStampH


#include <string>

namespace RDK {

using namespace std;

struct RDK_LIB_TYPE UTimeStamp
{
public: // ������
int Hours;
unsigned char Minutes;
unsigned char Seconds;
unsigned char Frames;

double FPS;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UTimeStamp(void);
UTimeStamp(const UTimeStamp &copy);
UTimeStamp(double seconds, double fps);
UTimeStamp(long frames, double fps);
~UTimeStamp(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UTimeStamp& operator = (const UTimeStamp &copy);
UTimeStamp& operator = (double seconds);

// �������� �������������� � �������
double operator()(void) const;

// ��������� ���������
bool operator == (const UTimeStamp &copy);
bool operator != (const UTimeStamp &copy);
bool operator < (const UTimeStamp &copy);
bool operator > (const UTimeStamp &copy);

// �������������� ���������
UTimeStamp& operator -= (const UTimeStamp &copy);
UTimeStamp& operator -= (double seconds);
friend UTimeStamp operator - (const UTimeStamp &copy1,const UTimeStamp &copy2);
friend UTimeStamp operator - (const UTimeStamp &copy,double seconds);
friend UTimeStamp operator - (double seconds,const UTimeStamp &copy);

UTimeStamp& operator += (const UTimeStamp &copy);
UTimeStamp& operator += (double seconds);
friend UTimeStamp operator + (const UTimeStamp &copy1,const UTimeStamp &copy2);
friend UTimeStamp operator + (const UTimeStamp &copy,double seconds);
friend UTimeStamp operator + (double seconds,const UTimeStamp &copy);

// ��������� ����� ������ � ������
// �����������':'
string& operator >> (string &str) const;
UTimeStamp& operator << (const string &str);
// --------------------------
};

}
#endif
