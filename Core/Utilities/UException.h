#ifndef UEXCEPTION_H
#define UEXCEPTION_H

namespace RDK {

/* ������� ����� ���������� */
class UException
{

};

/* ��������� ������ (��������� �� 0 ���������� � �.�.) */
class UFatalException: public UException
{

};

/* ������, �������������� ������������� */
class UErrException: public UException
{

};

/* �������������� (�������� �� ������������� ������������� ��������) */
class UWarningException: public UException
{

};

/* �������������� ���������, ������ ������� ������������ ������������� */
class UInfoException: public UException
{

};

}

#endif

