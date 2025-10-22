#include "UProjectDeployer.h"
#include "UApplication.h"
#include "../System/NamespaceIsolation.h"

namespace RDK{
// --------------------------
// ������������ � �����������
// --------------------------
UProjectDeployer::UProjectDeployer(void)
{

}
UProjectDeployer::~UProjectDeployer(void)
{

}
// --------------------------

/// ���������� ��������� �� ��������� ����������
std::shared_ptr<UApplication> UProjectDeployer::GetApplication(void)
{
 return Application;
}

bool UProjectDeployer::SetApplication(std::shared_ptr<UApplication> value)
{
 if(Application == value)
  return true;

 Application=value;
 return true;
}


void UProjectDeployer::AConnectToDatabase()
{

}

void UProjectDeployer::SetDatabaseAccess(const std::string &db_address,
                                         const std::string &db_name,
                                         const std::string &db_login,
                                         const std::string &db_password)
{
    database_name = db_name;
    database_address = db_address;
    database_login = db_login;
    database_password = db_password;

    AConnectToDatabase();
}

int UProjectDeployer::GetDeploymentState()
{
 return 0;
}

int UProjectDeployer::GetStageCap()
{
return 100;
}

int UProjectDeployer::GetStageProgress()
{
return 0;
}

int UProjectDeployer::StartProjectDeployment(int task_id, bool standalone)
{
    return 0;
}

void UProjectDeployer::SetFtpRemotePath(const std::string &path)
{
    ftp_remote_path = path;
}

void UProjectDeployer::SetTempProjectDeploymentPath(const std::string &path)
{
    temp_project_deployment_path = path;
}

std::string UProjectDeployer::GetTempProjectDeploymentPath()
{
    return temp_project_deployment_path;
}

///����������� � ������� ������:
/// 1. ����������� �� ��������� ���������
/// 2. ������� � �������� ������ � ��������� ���� � �����?
/// 3. �������
int UProjectDeployer::PrepareProject(std::string &response)
{
    response="base class called";
    return -1;
}

///������� �������������� ������
int UProjectDeployer::OpenPreparedProject(std::string &response)
{
    response="base class called";
    return -1;
}

int UProjectDeployer::GetPreparationResult(std::string &response)
{
    response="base class called";
    return -1;
}

///��������� �������������� ������
int UProjectDeployer::RunPreparedProject()
{
    return -1;
}


std::string UProjectDeployer::GetLastError()
{
    return std::string();
}

std::string UProjectDeployer::GetProjectFileName()
{
    return std::string();
}

///���������� ��������� ������ ������� (������ -2/0/1 ������� � ����)
int UProjectDeployer::GetCalculationState()
{
    return -1;
}

/// ������ ��� ������� ��� ����
void UProjectDeployer::SetStandaloneTask(int task)
{

}

int UProjectDeployer::GetStandaloneTask()
{
    return -1;
}

///���������� ��������� ��������� ���������� �������
/// ���������� false ��� ������ ��������� ���������
/// @state - ������ ��������� �������
/// @frame_id - ������ �������� �����
bool UProjectDeployer::GetCaptureState(int &state, unsigned long long& frame_id, unsigned long long &max_frame_id)
{
    state=-1;
    frame_id=-1;
    max_frame_id=-1;
    return false;
}
///������������ ������������ � ���������� ������ ���
/// ���������� false ���� ���� ��������� ������, ����� true
/// @error - ����� ������ �� ���� ����������
bool UProjectDeployer::ProcessCalculationLog(std::string &error)
{
    error="ProcessCalculationLog in base class called!";
    return false;
}

///���������� true ���� ��������� ������/������ ������ �� ������ ��������
/// �� ���������� ����� ��������� � false ���� ���
/*bool UProjectDeployer::CaptureProcessingFinished(int frame_number)
{
    return false;
}*/

/*
void UProjectDeployer::SetProjectIndices(int gt_id, int sln_id, int weights_id, int script_id)
{
    project_gt_id = gt_id;
    project_sln_id = sln_id;
    project_weigts_id = weights_id;
    project_script_id = script_id;
}
*/


// --------------------------
// ������ ��������������� ������
// --------------------------



/// ������ �������� ����� �� ���������� ���������, �������� ��������
/// ������ ������������ ������� ��� ����������� �� ���� ����������
//int UProjectDeployer::ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes)
//{

//}


}//namespace RDK
