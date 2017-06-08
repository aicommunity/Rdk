//---------------------------------------------------------------------------

#pragma hdrstop

#include "UTestManager.h"

namespace RDK {

/// --------------------
/// ������ �������������
/// --------------------
/// ���������� ��������� �� ����������� ����������
UEPtr<UApplication> UTestManager::GetApplication(void)
{
 return Application;
}

/// ������ ����������� ����������
void UTestManager::SetApplication(const UEPtr<UApplication> &value)
{
 if(Application == value)
  return;

 Application=value;
}
/// --------------------

// --------------------
// ������ ������������
// --------------------
/// �������� ������
int UTestManager::LoadTests(const std::string &file_name)
{
 if(file_name.empty())
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "Tests description file name is empty!");
  return 1001;
 }

 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "UTestManager::LoadTests called");
 return RDK_SUCCESS;
}


/// �������� ������������
/// ���������� � �������� ������ ���������� ������
/// ���������� ��� ������ ������������
int UTestManager::ProcessTests(void)
{
 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "UTestManager::ProcessTests called");
 return RDK_SUCCESS;
}
// --------------------


}
