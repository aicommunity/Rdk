#ifndef UProjectDeployerQtH
#define UProjectDeployerQtH

#include "../../../Deploy/Include/rdk.h"
#include "../UProjectDeployer.h"
#include "../UIVisualController.h"
#include <QtSql>
#include <QThread>
#include <QMutex>
#include <thread>
#include <mutex>
#ifdef __unix__
#include <curl/curl.h>
#endif

struct FtpFile {
  const char *filename;
  FILE *stream;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
  struct FtpFile *out = (struct FtpFile *)stream;
  if(!out->stream) {
    /* open file for writing */
    out->stream = fopen(out->filename, "wb");
    if(!out->stream)
    {
        std::cerr<<"Stream out failure";
        return -1; /* failure, can't open file to write */
    }
  }

  size_t res = fwrite(buffer, size, nmemb, out->stream);
  std::cerr<<"Res = "<<res<<std::endl;

  return res;
}

namespace RDK{
class UApplication;

enum DeploymentState
{
    DS_NULL=0,
    DS_Unknown=1,
    DS_DownloadTemplate=2,
    DS_UnpackTemplate=3,
    DS_DownloadWeights=4,
    DS_UnpackWeights=5,
    DS_DownloadScripts=6,
    DS_UnpackScripts=7,
    DS_DownloadData=8,
    DS_UnpackData=9,
    DS_Finished=10,
    DS_Error=100
};

class RDK_LIB_TYPE UProjectDeployProcessingThread: public QThread
{
Q_OBJECT

public:
    UProjectDeployProcessingThread();
    ~UProjectDeployProcessingThread();

    void run() override;

    void SetDatabasePath(const QString& path);
    void SetDownloadTempPath(const QString& path);
    void SetDeploymentTempPath(const QString& path);
    void SetFtpRemoteBasePath(const QString& path);
    void SetDeploymentState(const DeploymentState& state);
    void SetDeploymentProgress(const int &progress);
    void SetDeploymentProgressCap(const int &value);

    void PrepareProjectDeployment(bool task_template_dr, const QString& task_template_path,
                                  bool task_weights_dr, const QString& task_weights_path,
                                  bool task_script_dr, const QString& task_script_path,
                                  bool task_vsrc_dr, int task_vrc_type, const QString& task_vsrc_path);

    void RunProjectDeployment();

    ///Get
    QString GetDatabasePath();
    QString GetFtpRemoteBasePath();
    QString GetDeploymentTempPath();
    QString GetDownloadTempPath(); //Временные папки для закачки и развертывания обычно разнче

    DeploymentState GetDeploymentState();
    int GetDeploymentProgress();
    int GetDeploymentProgressCap();
    std::string GetLastError();

private slots:
    void processReadyReadStandardError();
    void processReadyReadStandardOutput();

private:
    bool DownloadZip(const QString &remote_url, const QString& dst_zip_file);
    bool UnpackZipFolder(const QString &local_zip_folder, const QString& local_dst_folder);
    bool UnpackZipFile(const QString &local_zip_folder, const QString& local_dst_folder);

    void DeployTemplate();
    bool VerifyTemplate();
    void DeployScript();
    bool VerifyScript();
    void DeployWeights();
    bool VerifyWeights();
    void DeployData();
    bool VerifyData();

private:
    QMutex deploymentStateMutex;
    QMutex deploymentProgressMutex;
    QMutex deploymentProgressCapMutex;

    QString databasePath;
    QString ftpRemoteBasePath;
    QString deploymentTempPath;
    QString downloadTempPath;

    bool template_dr, weights_dr, script_dr, videosource_dr;
    int videosource_type;
    QString template_path, weights_path, script_path, videosource_path;

    DeploymentState deploymentState;
    int deploymentProgressCap;
    int deploymentProgress;

    QProcess zip_process;

    std::string lastError;

    CURL *curl;
    CURLcode res;
};


class RDK_LIB_TYPE UProjectDeployerQt: public UProjectDeployer
{
friend class UApplication;
protected: // Параметры

protected: // Данные
/// Указатель на экземпляр приложения
QSqlDatabase *db;


protected: // Параметры

protected: // Данные

//Параметры задачи
QString task_name; //Имя задачи
int task_template_id;  //Индекс шаблона задачи
int task_weights_id;   //Индекс весов
int task_src_type;     //Тип источника изображений (0-видео, 1-набор изображений)
int task_src_id;       //Индекс источника в соответствующей таблице

//Подготовка шаблона - требуется ли загрузка шаблона проекта
bool task_template_download_required;
//Наименование шаблона (а оно вообще надо?)
QString task_template_name;
//Путь к шаблону (относительный)
QString task_template_path;
//Путь к ZIP шаблона, пока не требуем
//std::string task_template_zip_url;

//Подготовка весов
//Путь к весам
QString task_weights_path;
//Надо ли грузить веса
bool task_weights_download_required;
//Надо ли грузить конфиг (скорее всего не надо, так как полная корреляция с весами ДБ
bool task_weights_conf_download_required;
//Путь к конфигурации - возможно не нужен
QString task_weights_config_path;
//Уберем пока
//std::string task_weights_zip_url;
//std::string task_weights_conf_zip_url;

//Подготовка скриптов
//Индекс скрипта
int task_script_id;
//Путь к скрипту (относительный)
QString task_script_path;
//Нужна ли загрузка скрипта
bool task_script_download_required;
//Архив уберем пока
//std::string task_script_zip_url;

//Подготовка источника изображений
//Путь к источнику (с учетом того что тип уже известен)
QString task_src_path;
//Надо ли загружать
bool task_src_download_required;
//Путь к архиву (не факт что нужен именно тут)
QString task_src_zip_url;

QString deployment_temp_path;
QString download_temp_path;

UProjectDeployProcessingThread *deployProcessingThread;

std::string lastError;

/*
//Дубликация?
//Пути к проекту (вар-т 1)
std::string project_path;
std::string project_url;

//Параметры проекта
int project_gt_id;
int project_sln_id;
int project_weigts_id;
int project_script_id;
*/

protected://Методы

//Удалить поток обработки или вернуть
bool DestroyProcessingThread();

public: // Методы

///Запустить подготовку выполнения задачи,заданной пользоватлем (на вход идет индекс в базе данных)
virtual int StartProjectDeployment(int task_id);

///Создать и настроить соединение с СУБД
//virtual void SetDatabaseAccess(const std::string &db_address, const std::string &db_name, const std::string &db_login, const std::string &db_password);
//virtual void SetProjectIndices(int gt_id, int sln_id, int weights_id, int script_id);

virtual void AConnectToDatabase();

virtual int GetDeploymentState();
virtual int GetStageCap();
virtual int GetStageProgress();
virtual std::string GetLastError();
virtual std::string GetProjectFileName();

public: // Методы доступа к данным

QSqlDatabase *GetDatabase() {return db;};

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
