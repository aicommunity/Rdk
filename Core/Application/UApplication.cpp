#ifndef UApplication_CPP
#define UApplication_CPP

#include "UApplication.h"

//#include "UAppCore.cpp"
//#include "UProject.cpp"
//#include "UIVisualController.cpp"
//#include "URpcDispatcherQueues.cpp"
//#include "URpcDispatcher.cpp"
//#include "URpcDecoder.cpp"
//#include "URpcDecoderInternal.cpp"
//#include "URpcCommand.cpp"
//#include "URpcCommandInternal.cpp"

              
#ifdef __BORLANDC__
//#include "Bcb/Application.bcb.cpp"
#endif

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UApplication::UApplication(void)
{

}

UApplication::~UApplication(void)
{

}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
/// ������� �������
const std::string& UApplication::GetWorkDirectory(void) const
{
 return WorkDirectory;
}

bool UApplication::SetWorkDirectory(const std::string& value)
{
 WorkDirectory=value;
 return true;
}
// --------------------------

// --------------------------
// ������ �������������
// --------------------------
/// ������������� ������ � ���������� ������
UEPtr<URpcDispatcher> UApplication::GetRpcDispatcher(void)
{
 return RpcDispatcher;
}

/// ������������� ����� ��������� ������
/// ��������������� �� ������������ ������ ���������� ����� �� ���������� �������
bool UApplication::SetRpcDispatcher(const UEPtr<URpcDispatcher> &value)
{
 RpcDispatcher=value;
 return true;
}

/// �������������� ����������
bool UApplication::Init(void)
{
 return true;
}

/// ���������������� ����������
bool UApplication::UnInit(void)
{
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
/// ������� ������ (����� ���������� � ��������)
bool UApplication::CreateProject(const std::string &filename)
{
 return true;
}

/// ��������� ������
bool UApplication::OpenProject(const std::string &filename)
{
 return true;
}

/// ��������� ������
bool UApplication::SaveProject(void)
{
 return true;
}

bool UApplication::SaveProjectAs(const std::string &filename)
{
 return true;
}

/// ��������� ������
bool UApplication::CloseProject(void)
{
 return true;
}

/// ��������� ������ � ����� ������������
bool UApplication::CloneProject(const std::string &filename)
{
 return true;
}
// --------------------------

}

#endif

