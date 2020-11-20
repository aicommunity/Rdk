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

///Структура, содержащая имена тэгов класса в библиотеке машинного обучения
/// Если тэг пустой, то значит не используется
struct MLlibDescr
{
 std::string LibName;               //Имя библиотеки (Тэг компонента может гулять)
 std::string LibScriptFileTagName;  //Тэг файла скрипта
 std::string LibWeightFileTagName;  //Тэг файла весов
 std::string LibConfigFileTagName;  //Тэг файла конфигурации
 std::string LibClassCountTagName;  //Тэг количества классов в библиотеке
 MLlibDescr(){
 LibName="";
 LibScriptFileTagName="";
 LibWeightFileTagName="";
 LibConfigFileTagName="";
 LibClassCountTagName="";
 }
};

struct CaptureLibDescr
{
 std::string LibName;
 std::string LibFrameIdStateName;
 std::string LibCaptureStateTagName;


 CaptureLibDescr(){
 LibName="";
 LibFrameIdStateName="";
 LibCaptureStateTagName="";
 }
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
    DS_DeployFinished=10,
    DS_CopyProject=11,
    DS_PrepareProject=12,
    DS_ProjectPrepared=13,
    DS_OpenProject=14,
    DS_ProjectOpened=15,
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

//protected: // Параметры

protected: // Данные

//Параметры задачи
QString task_name;     //Имя задачи
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
//Имя файла проекта (project.ini, project_full.ini и тп) для открытия,
// выяснилось, что они могут гулять
QString task_template_file_name;

//Подготовка весов
//Путь к весам
QString task_weights_path;
//Надо ли грузить веса
bool task_weights_download_required;
//Надо ли грузить конфиг (скорее всего не надо, так как полная корреляция с весами ДБ
bool task_weights_conf_download_required;
//Число классов для настройки весов
int weights_classes_number;
//Путь к конфигурации - возможно не нужен
QString task_weights_config_path;
//Абсолютный путь к файлу конфигурации (нужен для настройки проекте)
QString absolute_config_file;
//Абсолютный путь к файлу весов (нужен для настройки проекте)
QString absolute_weights_file;
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
//Абсолютный путь к файлу скрипта
QString absolute_script_file;

//Подготовка источника изображений
//Путь к источнику (с учетом того что тип уже известен)
QString task_src_path;
//Полный путь к источнику (для настройки)
QString task_src_fullpath;
//Надо ли загружать
bool task_src_download_required;
//Путь к архиву (не факт что нужен именно тут)
QString task_src_zip_url;
//Временный путь для загрузки (пока что в {Database}/Temp по умолчанию)
QString download_temp_path;
//Количество кадров в источнике (по данным СУБД,
// потому что там посчитано руками и обычно точнее)
int task_src_frame_length;

//Указатель на поток деплоя с FTP
UProjectDeployProcessingThread *deployProcessingThread;

//Последняя ошибка в режиме доставки/открытия/настройки/запуска/закрытия
std::string lastError;

//Результат подготовки проекта
//альтернативный результат для получения через GetPreparationResult
int preparationResult;

//Состояние деплоймента
DeploymentState deploymentState;

//Параметры настройки компонентов в зависимости
// от типа (заполняется в конструкторе)
std::map<std::string, MLlibDescr> file_tags;
std::vector<std::string> component_classes;

std::map<std::string, CaptureLibDescr> capture_tags;
//Это не точно нужно
//std::vector<std::string> capture_classes;

//Параметры класса и имени захвата для дальнейшей работы
std::string capture_class_name;
std::string capture_component_name;

protected://Методы

//Удалить поток обработки или вернуть
bool DestroyProcessingThread();

public: // Методы

///Запустить подготовку выполнения задачи,заданной пользоватлем (на вход идет индекс в базе данных)
virtual int StartProjectDeployment(int task_id);

///Подготовить к запуску проект:
/// 1. Скопировать во временное хранилище
/// 2. Открыть в тестовом режиме и настроить пути и связи?
/// 3. Закрыть
virtual int PrepareProject(std::string &response);
/// Получить результат подготовки проекта
virtual int GetPreparationResult(std::string &response);
///Открыть подготовленный проект
virtual int OpenPreparedProject(std::string &response);
/// Запустить подготовленный проект
virtual int RunPreparedProject();

///То место где реально идет коннект к базе
/// попытка сделать аналог той структуры через А, А2
/// как в остальной либе (не оч удачная)
virtual void AConnectToDatabase();

/// Получить состояние деплоймента
virtual int GetDeploymentState();
/// Получить максимум прогресса
virtual int GetStageCap();
/// Получить текущее состояние прогресса
virtual int GetStageProgress();
/// Получить последнюю ошибку
virtual std::string GetLastError();
/// Получить имя файла проекта (кстати, зачем?)
virtual std::string GetProjectFileName();

///Возвращает состояние потока расчета (аналог -2/0/1 столбца в Гуях)
virtual int GetCalculationState();
///Возвращает состояние активного компонента захвата
/// возвращает false при ошибке получение состояния
/// @state - индекс состояния захвата
/// @frame_id - индекс текущего кадра
virtual bool GetCaptureState(int &state, int& frame_id, int& max_frame_id);
///Обрабатывает накопившийся с последнего вызова лог
/// возвращает false если были фатальные ошибки, иначе true
/// @error - текст ошибки из лога приложения
virtual bool ProcessCalculationLog(std::string &error);

public: // Методы доступа к данным

/// Получить ссылку на базу данных
QSqlDatabase *GetDatabase() {return db;};

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProjectDeployerQt(void);
virtual ~UProjectDeployerQt(void);
// --------------------------

protected:
// --------------------------
// Методы транспортировки данных
// --------------------------
///Скопировать проект во временную директорию, в которой будет выполняться работа
int CopyProjectToTempFolder();
/// Открыть проект в заглушенном режиме
int OpenProjectMockMode();
/// Задать параметры проекта в заглушенном режиме
int SetupProjectMockParameters();
/// Закрыть заглушенный проект
int CloseMockProject();
/// Открыть проект в боевом режиме
/// (пока мб сразу в основной функции)
//int OpenPreparedProject();

/// Проанализировать лог на предмет наличия ошибок
/// высокого уровня (0-2, Unknown, Fatal, Error)
/// возвращает: -1 - нет ошибок, 0-2 - уровень ошибки
int AnalyzeLogForErrors(std::string &problem_string);


/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
//virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);
};

}//namespace RDK
#endif
