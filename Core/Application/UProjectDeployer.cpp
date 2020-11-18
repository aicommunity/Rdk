#include "UProjectDeployer.h"
#include "UApplication.h"

namespace RDK{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProjectDeployer::UProjectDeployer(void)
{

}
UProjectDeployer::~UProjectDeployer(void)
{

}
// --------------------------

/// Возвращает указатель на экземпляр приложения
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

int UProjectDeployer::StartProjectDeployment(int task_id)
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

///Подготовить к запуску проект:
/// 1. Скопировать во временное хранилище
/// 2. Открыть в тестовом режиме и настроить пути и связи?
/// 3. Закрыть
int UProjectDeployer::PrepareProject(std::string &response)
{
    response="base class called";
    return -1;
}

///Открыть подготовленный проект
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

std::string UProjectDeployer::GetLastError()
{
    return std::string();
}

std::string UProjectDeployer::GetProjectFileName()
{
    return std::string();
}

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
// Методы транспортировки данных
// --------------------------



/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
//int UProjectDeployer::ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes)
//{

//}


}//namespace RDK
