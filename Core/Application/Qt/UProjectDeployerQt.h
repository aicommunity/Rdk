#ifndef UProjectDeployerQtH
#define UProjectDeployerQtH

#include "../../../Deploy/Include/rdk.h"
#include "../UIVisualController.h"

namespace RDK{
class UApplication;

class RDK_LIB_TYPE UProjectDeployerQt: public UAppController
{
friend class UApplication;
protected: // Параметры

protected: // Данные
/// Указатель на экземпляр приложения
UEPtr<UApplication> Application;
protected: // Параметры

protected: // Данные
//Пути к проекту (вар-т 1)
std::string project_path;
std::string project_url;

//Данные для входа:
std::string database_address;
std::string database_port;
std::string database_login;
std::string database_password;

//Параметры проекта
int project_gt_id;
int project_sln_id;
int project_weigts_id;
int project_script_id;

public: // Методы

virtual int StartProjectDeployment(const std::string &project_path, const std::string &project_url);
virtual void SetDatabaseCredentials(const std::string &db_address, const std::string &db_port, const std::string &db_login, const std::string &db_password);
virtual void SetProjectIndices(int gt_id, int sln_id, int weights_id, int script_id);

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProjectDeployerQt(void);
virtual ~UProjectDeployerQt(void);
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
