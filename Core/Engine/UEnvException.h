#ifndef UENV_EXCEPTION_H
#define UENV_EXCEPTION_H

#include <string>
#include "UAEnvSupport.h"
#include "../Utilities/UException.h"

namespace RDK {

class UAContainer;

/* ������� ����� ���������� */
class UIContainerException
{
public: // ������ ����������
// �������� ��� ���������� � ������� ������������� ����������
std::string Name;

// �������� ������������� ���������� � ������� ������������� ����������
ULongId Id;

// ������ ��� ��������� ���������� � ������� ������������� ����������
std::string OwnerName;

// ������ ������������� ��������� ���������� � ������� ������������� ����������
ULongId OwnerId;

// ������ ��� �������� ��������� ���������� � ������� ������������� ����������
std::string MainOwnerName;

// ������ ������������� �������� ��������� ���������� � ������� ������������� ����������
ULongId MainOwnerId;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UIContainerException(void);
UIContainerException(const UAContainer *cont);
UIContainerException(const UIContainerException &copy);
virtual ~UIContainerException(void);
// --------------------------
};

class UEnvFatalException: public UFatalException, public UIContainerException
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UEnvFatalException(void);
UEnvFatalException(const UAContainer *cont);
UEnvFatalException(const UEnvFatalException &copy);
virtual ~UEnvFatalException(void);
// --------------------------
};



}

#endif

