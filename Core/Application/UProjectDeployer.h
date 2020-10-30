#ifndef UProjectDeployerH
#define UProjectDeployerH

//#include "../../Deploy/Include/rdk.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "UIVisualController.h"

namespace RDK{
class UApplication;

class RDK_LIB_TYPE UProjectDeployer: public UAppController
{
friend class UApplication;
protected: // Параметры

protected: // Данные
/// Указатель на экземпляр приложения
UEPtr<UApplication> Application;
protected: // Параметры

protected: // Данные

//Данные для входа:
std::string database_address;
std::string database_name;
std::string database_login;
std::string database_password;

std::string ftp_remote_path;

/*
//Отпилили предварительно (не появится ранее дочернего класса!)
//Пути к проекту (вар-т 1)
std::string project_path;
std::string project_url;
//Параметры проекта
int project_gt_id;
int project_sln_id;
int project_weigts_id;
int project_script_id;
*/

public: // Методы

// --------------------------
// Методы доступа к данным
// --------------------------
/// Возвращает указатель на экземпляр приложения
UEPtr<UApplication> GetApplication(void);
bool SetApplication(UEPtr<UApplication> value);
// --------------------------

///Запустить подготовку выполнения задачи,заданной пользоватлем (на вход идет индекс в базе данных)
virtual int StartProjectDeployment(int task_id);

///Задать пар-ры конннекта к СУБД
virtual void SetDatabaseAccess(const std::string &db_address, const std::string &db_name, const std::string &db_login, const std::string &db_password);
/// Осуществить соединение
virtual void AConnectToDatabase();
///
//virtual void SetProjectIndices(int gt_id, int sln_id, int weights_id, int script_id);

virtual int GetDeploymentState();
virtual int GetStageCap();
virtual int GetStageProgress();

virtual std::string GetLastError();
virtual std::string GetProjectFileName();

virtual void SetFtpRemotePath(const std::string &path);

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProjectDeployer(void);
virtual ~UProjectDeployer(void);
// --------------------------

// --------------------------
// Методы транспортировки данных
// --------------------------

/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
//virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);
};

}//namespace RDK
#endif
