#ifndef UENV_EXCEPTION_CPP
#define UENV_EXCEPTION_CPP

#include "UEnvException.h"

namespace RDK {


// --------------------------
// ������������ � �����������
// --------------------------
UIContainerException::UIContainerException(void)
{

}

UIContainerException::UIContainerException(const UAContainer *cont)
{
 if(!cont)
  return;

 // �������� ��� ���������� � ������� ������������� ����������
 Name=cont->GetName();

 // �������� ������������� ���������� � ������� ������������� ����������
 Id=cont->GetId();

 // ������ ��� ��������� ���������� � ������� ������������� ����������
 if(cont->GetOwner())
 {
  cont->GetOwner()->GetFullName(OwnerName);

  // ������ ������������� ��������� ���������� � ������� ������������� ����������
  OwnerId=cont->GetOwner()->GetFullId();
 }

 if(cont->GetMainOwner())
 {
  // ������ ��� �������� ��������� ���������� � ������� ������������� ����������
  cont->GetMainOwner()->GetFullName(MainOwnerName);

  // ������ ������������� �������� ��������� ���������� � ������� ������������� ����������
  MainOwnerId=cont->GetMainOwner()->GetFullId();
 }
}


UIContainerException::UIContainerException(const UIContainerException &copy)
{
 // �������� ��� ���������� � ������� ������������� ����������
 Name=copy.Name;

 // �������� ������������� ���������� � ������� ������������� ����������
 Id=copy.Id;

 // ������ ��� ��������� ���������� � ������� ������������� ����������
 OwnerName=copy.OwnerName;

 // ������ ������������� ��������� ���������� � ������� ������������� ����������
 OwnerId=copy.OwnerId;

 // ������ ��� �������� ��������� ���������� � ������� ������������� ����������
 MainOwnerName=copy.MainOwnerName;

 // ������ ������������� �������� ��������� ���������� � ������� ������������� ����������
 MainOwnerId=copy.MainOwnerId;
}

UIContainerException::~UIContainerException(void)
{

}
// --------------------------


// class UEnvFatalException: public UFatalException, public UIContainerException
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UEnvFatalException::UEnvFatalException(void)
{

}

UEnvFatalException::UEnvFatalException(const UAContainer *cont)
 : UFatalException(), UIContainerException(cont)
{

}

UEnvFatalException::UEnvFatalException(const UEnvFatalException &copy)
 : UFatalException(copy), UIContainerException(copy)
{

}

UEnvFatalException::~UEnvFatalException(void)
{

}
// --------------------------



}
#endif

