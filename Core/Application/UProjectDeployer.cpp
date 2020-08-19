#include "UProjectDeployer.h"
#include "UApplication.h"

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
UEPtr<UApplication> UProjectDeployer::GetApplication(void)
{
 return Application;
}

bool UProjectDeployer::SetApplication(UEPtr<UApplication> value)
{
 if(Application == value)
  return true;

 Application=value;
 return true;
}



void UProjectDeployer::SetDatabaseCredentials(const std::string &db_address,
                                              const std::string &db_port,
                                              const std::string &db_login,
                                              const std::string &db_password)
{
    database_port = db_port;
    database_address = db_address;
    database_login = db_login;
    database_password = db_password;
}

void UProjectDeployer::SetProjectIndices(int gt_id, int sln_id, int weights_id, int script_id)
{
    project_gt_id = gt_id;
    project_sln_id = sln_id;
    project_weigts_id = weights_id;
    project_script_id = script_id;
}


// --------------------------
// ������ ��������������� ������
// --------------------------



/// ������ �������� ����� �� ���������� ���������, �������� ��������
/// ������ ������������ ������� ��� ����������� �� ���� ����������
//int UProjectDeployer::ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes)
//{

//}


}//namespace RDK
