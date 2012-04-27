/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MDVECTOR_H
#define MDVECTOR_H

namespace RDK {

			   /*
#ifdef MVECTOR_H
typedef MVector<double> MDVector;
#else

class MDVector
{
public: // ������
double x,y,z; // ���������� �-��.

public: // ������
//---------------------------
// ������������ � �����������
//---------------------------
//  ������ �-� ������� �����.
MDVector(void);

// ������ �-� ��� �����. �������� = ksi.
MDVector(double ksi);

// ������ �-� � �����. xx,yy,zz.
MDVector(double xx,double yy,double zz);

// ������ �-� ������ �-�� v
MDVector(const MDVector &v);
//---------------------------

//---------------------------
// ��������� ���������
//---------------------------
// ������ � �����. �-�� �� �� ������. ������.
double& operator [](int n);

// ��������� ����� �������
double operator !(void) const;

// ������� ������������ �������.
MDVector& Normalize(void);
//---------------------------

//---------------------------
// ��������� ������������
//---------------------------
// ������������ �-�� �-��.
MDVector& operator = (const MDVector &v);

// ������������ ���� �����. �-�� �������� double
MDVector& operator = (double f);
//---------------------------

//---------------------------
// ���������� ���������
//---------------------------
// �������� �� ���������.
bool operator == (const MDVector& v) const;

// �������� �� �����������.
bool operator != (const MDVector& v) const;

// �������� "��� �����. ������ ������".
bool operator < (double v);

// �������� "��� �����. ������ ������".
bool operator > (double v);
//---------------------------

//---------------------------
// �������������� ���������
//---------------------------
// ����� ����� � ���� �����. �-��.
MDVector operator - (void) const;

// ���������� � ������� �������.
MDVector& operator += (const MDVector &v);

// ��������� �� ������� �������.
MDVector& operator -= (const MDVector &v);

// ��������� ���������� ������� �� ������.
MDVector& operator ^= (const MDVector &v);

// ��������� �-�� �� �����
MDVector& operator *= (double f);

// ������� �-�� �� �����
MDVector& operator /= (double f);

// ��������� ������� �-�� �� �-�
MDVector& operator /= (const MDVector &v);
//---------------------------
};

// ����������
MDVector Normalize(const MDVector &v);

// �������� ��������.
MDVector operator + (const MDVector &u,const MDVector &v);

// ��������� ��������.
MDVector operator - (const MDVector &u,const MDVector &v);

// ��������� ��������� ��������.
MDVector operator ^ (const MDVector &u,const MDVector &v);

// ��������� ��������� ��������.
double operator * (const MDVector &u,const MDVector &v);

// ��������� ������� � ����� ������.
MDVector operator * (const MDVector &v,double f);

// ��������� ������� � ����� �����.
MDVector operator * (double f,const MDVector &v);

// ������� ������� � �����.
MDVector operator / (const MDVector &v,double f);

// �������������� ������� �-���
MDVector operator / (const MDVector &u,const MDVector &v);

#endif    */
}
#endif
