#ifndef UProjectDeployerQtH
#define UProjectDeployerQtH

#include "../../../Deploy/Include/rdk.h"
#include "../UProjectDeployer.h"
#include "../UIVisualController.h"
#include <QtSql/QtSql>
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

///���������, ���������� ����� ����� ������ � ���������� ��������� ��������
/// ���� ��� ������, �� ������ �� ������������
struct MLlibDescr
{
 std::string LibName;               //��� ���������� (��� ���������� ����� ������)
 std::string LibScriptFileTagName;  //��� ����� �������
 std::string LibWeightFileTagName;  //��� ����� �����
 std::string LibConfigFileTagName;  //��� ����� ������������
 std::string LibClassCountTagName;  //��� ���������� ������� � ����������
 std::string LibUseFullPathTagName; //��� ������������ �������� ������ ������� ����
 MLlibDescr(){
 LibName="";
 LibScriptFileTagName="";
 LibWeightFileTagName="";
 LibConfigFileTagName="";
 LibClassCountTagName="";
 LibUseFullPathTagName = "UseFullPath";//���������� � ������������ ������, ������ ������������ �� ���������
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
    DS_Calculation=16,
    DS_ProjectClosed=17,
    DS_CopyResults=18,
    DS_PackResults=19,
    DS_UploadResults=20,
    DS_UploadFinished=21,
    DS_Error=100
};

enum DatabaseTaskStatus
{
    TS_New=0,
    TS_Accepted=1,
    TS_Deployment=2,
    TS_Calculation=3,
    TS_Finished=4,
    TS_Error=1000
};

enum TemplateType
{
    TT_VideoAnalytics=1,
    TT_NeuralInterface=2
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
    void SetStorageMountPath(const QString& path);
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
    QString GetDownloadTempPath(); //��������� ����� ��� ������� � ������������� ������ ������

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

    bool RecursiveCopyFiles(const QString& src_dir_path, const QString& dst_dir_path);

    void DeployTemplate();
    void DeployTemplate2();
    void DeployDirectory(const QString& file_with_template);
    bool VerifyTemplate();
    void DeployScript2();
    void DeployScript();
    bool VerifyScript();
    void DeployWeights2();
    void DeployWeights();
    bool VerifyWeights();
    void DeployData2();
    void DeployData();
    bool VerifyData();

private:
    QMutex deploymentStateMutex;
    QMutex deploymentProgressMutex;
    QMutex deploymentProgressCapMutex;

    QString databasePath;
    QString ftpRemoteBasePath;
    QString downloadTempPath;
    QString storageMountPath;

    bool template_dr, weights_dr, script_dr, videosource_dr;
    int videosource_type;
    QString template_path, weights_path, script_path, videosource_path;

    DeploymentState deploymentState;
    int deploymentProgressCap;
    int deploymentProgress;

    QProcess zip_process;

    QString lastError;

    CURL *curl;
    CURLcode res;
};

class RDK_LIB_TYPE UProjectResultsUploadingThread: public QThread
{
Q_OBJECT

public:
     UProjectResultsUploadingThread();
    ~UProjectResultsUploadingThread();

    void run() override;

    void SetProjectResultsDirPath(const QString& results_dir);
    QString GetProjectResultsDirPath();

    void SetStorageResultsDirPath(const QString& results_dir);
    QString GetStorageResultsDirPath();

    void SetDatabasePath(const QString& database_path);
    QString GetDatabasePath();

    void SetRemoteFtpPath(const QString& ftp_path);
    QString GetRemoteFtpPath();

    void SetRemoteStoragePath(const QString& strg_path);
    QString GetRemoteStoragePath();

    QString GetLastError();
    DeploymentState GetUploadState();

private:
    bool ZipResults();
    bool UploadResultsViaFtp();

    bool CopyResultsToDestinationDir();
    bool CopyResultsToRemoteStorageDir();


    bool RecursiveCopyFiles(const QString& src_dir_path, const QString& dst_dir_path);

private slots:
    void processReadyReadStandardError();
    void processReadyReadStandardOutput();

private:
    QString projectResultsDir;
    QString storageResultsDir;
    QString databasePath;
    QString remoteFtpPath;
    QString remoteStoragePath;

    QString lastError;
    DeploymentState uploadState;

    QProcess zip_process;

};

//��������� �������, ������� �������� �������� �� ��,
//����� ����� ��������� ����� (���������) ����� ����������� ����� �� ���� ��������
//������� ����� ����������� - ��� ����� ��������������� �������, ������� ����������
//�������� ��������� �� ������
enum ProjectRunState {
    PS_Undefined = 0,               //�������� ��� �������������, �������� ��� ������ ��� �� ��������
    PS_Initialization = 100,        //������ ������-������ ������������, ������� ��������������� � ������� ����������
    PS_Deployment = 101,            //��� ������ ������ ��������� � ���� ������� � ��������� NS_Connected ��������� � ����� �����������
    PS_ProjectPreparation = 102,    //��� ������ ������� ����������� ������ ��������������� ��������, ��������� � ����� ����������
    PS_ProjectOpening = 103,        //��������� ����������� ������
    PS_Calculation = 104,           //������������ ������ �������
    PS_Finalization = 105,          //������ ������� ����������, ��������� ������
    PS_ResultsUploading = 106,      //�������� �����������
    PS_Termination = 107,           //�������� ������� �� ���������, ����� ����� ��������
};

class RDK_LIB_TYPE UProjectRunThread: public QThread
{
Q_OBJECT

public:
     UProjectRunThread(UProjectDeployer* deployer);
    ~UProjectRunThread();

    void run() override;

private:
    UProjectDeployer* Deployer;

    unsigned long long last_frame_count;

    ProjectRunState projectRunState;

    std::string logFileName;

private:

    void WriteLog(const string& s);

    std::string ParseDeploymentState(DeploymentState state);

    ///����������� � ������� ������:
    /// 1. ����������� �� ��������� ���������
    /// 2. ������� � �������� ������ � ��������� ���� � �����
    /// 3. �������
    void PrepProject();

    ///������� �������������� ������
    void OpenProject();

    /// ��������� �������������� ������
    void RunProject();

    /// ��������� ������ �������
    void FinishProject();

    /// ��������� ���������� �������
    void UploadResults();

    void ProjectStateInitialization();
    void ProjectStateDeployment();
    void ProjectStateCalculation();
    void ProjectStateFinalization();
};


class RDK_LIB_TYPE UProjectDeployerQt: public UProjectDeployer
{
friend class UApplication;
protected: // ���������

protected: // ������
/// ��������� �� ��������� ����������
QSqlDatabase *db;

//protected: // ���������

protected: // ������

//��������� ������
QString task_name;     //��� ������
int task_id;           //������ ������ (��� ����������� �����������/������ ����������� � ����)
int task_template_id;  //������ ������� ������
int task_weights_id;   //������ �����
int task_src_type;     //��� ��������� ����������� (0-�����, 1-����� �����������)
int task_src_id;       //������ ��������� � ��������������� �������

//���������� ������� - ��������� �� �������� ������� �������
bool task_template_download_required;
//������������ ������� (� ��� ������ ����?)
QString task_template_name;
//���� � ������� (�������������)
QString task_template_path;
//��� ����� ������� (project.ini, project_full.ini � ��) ��� ��������,
// ����������, ��� ��� ����� ������
QString task_template_file_name;

//���������� �����
//���� � �����
QString task_weights_path;
//���� �� ������� ����
bool task_weights_download_required;
//���� �� ������� ������ (������ ����� �� ����, ��� ��� ������ ���������� � ������ ��
bool task_weights_conf_download_required;
//����� ������� ��� ��������� �����
int weights_classes_number;
//���� � ������������ - �������� �� �����
QString task_weights_config_path;
//���������� ���� � ����� ������������ (����� ��� ��������� �������)
QString absolute_config_file;
//���������� ���� � ����� ����� (����� ��� ��������� �������)
QString absolute_weights_file;
//������ ����
//std::string task_weights_zip_url;
//std::string task_weights_conf_zip_url;

//���������� ��������
//������ �������
int task_script_id;
//���� � ������� (�������������)
QString task_script_path;
//����� �� �������� �������
bool task_script_download_required;
//���������� ���� � ����� �������
QString absolute_script_file;

//���������� ��������� �����������
//���� � ��������� (� ������ ���� ��� ��� ��� ��������)
QString task_src_path;
//������ ���� � ��������� (��� ���������)
QString task_src_fullpath;
//���� �� ���������
bool task_src_download_required;
//���� � ������ (�� ���� ��� ����� ������ ���)
QString task_src_zip_url;
//��������� ���� ��� �������� (���� ��� � {Database}/Temp �� ���������)
QString download_temp_path;
//���������� ������ � ��������� (�� ������ ����,
// ������ ��� ��� ��������� ������ � ������ ������)
int task_src_frame_length;

//��������� �� ����� ������ � FTP
UProjectDeployProcessingThread *deployProcessingThread;

//��������� ������ � ������ ��������/��������/���������/�������/��������
std::string lastError;

//��������� ���������� �������
//�������������� ��������� ��� ��������� ����� GetPreparationResult
int preparationResult;

//��������� �����������
DeploymentState deploymentState;

//��������� ��������� ����������� � �����������
// �� ���� (����������� � ������������)
std::map<std::string, MLlibDescr> file_tags;
std::vector<std::string> component_classes;

std::map<std::string, CaptureLibDescr> capture_tags;
//��� �� ����� �����
//std::vector<std::string> capture_classes;

//��������� ������ � ����� ������� ��� ���������� ������
std::string capture_class_name;
std::string capture_component_name;

std::string predictor_class_name;
std::string predictor_component_name;

//���� � ����� ��� ���������� � �������
QDateTime processing_start_datetime;
QDateTime processing_end_datetime;

UProjectResultsUploadingThread *projectResultsUploadingThread;

//������ ��� ���������� � ������ standalone (���������� ����)
int serverStandaloneTask;

TemplateType template_type;

protected://������

//������� ����� ��������� ��� �������
bool DestroyProcessingThread();

public: // ������

///��������� ���������� ���������� ������,�������� ������������ (�� ���� ���� ������ � ���� ������)
/// standalone - ������ ��� ����
virtual int StartProjectDeployment(int deploy_task_id, bool standalone);

///����������� � ������� ������:
/// 1. ����������� �� ��������� ���������
/// 2. ������� � �������� ������ � ��������� ���� � �����?
/// 3. �������
virtual int PrepareProject(std::string &response);
/// �������� ��������� ���������� �������
virtual int GetPreparationResult(std::string &response);
///������� �������������� ������
virtual int OpenPreparedProject(std::string &response);
/// ��������� �������������� ������
virtual int RunPreparedProject();

///�� ����� ��� ������� ���� ������� � ����
/// ������� ������� ������ ��� ��������� ����� �, �2
/// ��� � ��������� ���� (�� �� �������)
virtual void AConnectToDatabase();

/// �������� ��������� �����������
virtual int GetDeploymentState();
/// �������� �������� ���������
virtual int GetStageCap();
/// �������� ������� ��������� ���������
virtual int GetStageProgress();
/// �������� ��������� ������
virtual std::string GetLastError();
/// �������� ��� ����� ������� (������, �����?)
virtual std::string GetProjectFileName();

///���������� ��������� ������ ������� (������ -2/0/1 ������� � ����)
virtual int GetCalculationState();
///���������� ��������� ��������� ���������� �������
/// ���������� false ��� ������ ��������� ���������
/// @state - ������ ��������� �������
/// @frame_id - ������ �������� �����
virtual bool GetCaptureState(int &state, unsigned long long& frame_id, unsigned long long& max_frame_id);

bool GetCaptureStateVideoAnalysis(int &state, unsigned long long& frame_id, unsigned long long& max_frame_id);

bool GetCaptureStateNeuralInterface(int &state, unsigned long long& frame_id, unsigned long long& max_frame_id);

///������������ ������������ � ���������� ������ ���
/// ���������� false ���� ���� ��������� ������, ����� true
/// @error - ����� ������ �� ���� ����������
virtual bool ProcessCalculationLog(std::string &error);
///��������� ������ �������, �������� ��������������� ��������� ������� � ���� ������
virtual bool FinishCalculation();
///��������� ���������� ������� (���������� ����� Results) � ��������������� ����� ���������� ���������,
/// ��������� ������� �������� � �������� ������ � ��������� ���������
virtual bool UploadCalculationResults();
///���������� �������� �������, ������� ������� �� ���� ������ ������������
/// ������� ���������� ������, ��������� ��������� ���������� ������� � �.�.
virtual bool CloseSolver();
///�������� ��������� ��������
virtual int GetUploadState();
///�������� ������ ������ � ���� ������
virtual void UpdateTaskStateInDb(int task_id, const DatabaseTaskStatus &status, float progress=-1.0f, const QString &start_time="", const QString& end_time="");

/// ������ ��� ������� ��� ����
virtual void SetStandaloneTask(int task);
virtual int GetStandaloneTask();


public: // ������ ������� � ������

/// �������� ������ �� ���� ������
QSqlDatabase *GetDatabase() {return db;};

// --------------------------
// ������������ � �����������
// --------------------------
UProjectDeployerQt(void);
virtual ~UProjectDeployerQt(void);
// --------------------------

protected:
// --------------------------
// ������ ��������������� ������
// --------------------------
///����������� ������ �� ��������� ����������, � ������� ����� ����������� ������
int CopyProjectToTempFolder();
/// ������� ������ � ����������� ������
int OpenProjectMockMode();
/// ������ ��������� ������� � ����������� ������
int SetupProjectMockParameters();

///��������� ��������� ������� � ������ �������������� (�����, ������ ������, ��� ��� ���)
int SetupProjectMockParametersVideoAnalysis();

///��������� ��������� ������� ��� ���������������, �.�. �������� ��� ��-�������.
int SetupProjectMockParametersNeuralInterface();
/// ������� ����������� ������
int CloseMockProject();
/// ������� ������ � ������ ������
/// (���� �� ����� � �������� �������)
//int OpenPreparedProject();

/// ���������������� ��� �� ������� ������� ������
/// �������� ������ (0-2, Unknown, Fatal, Error)
/// ����������: -1 - ��� ������, 0-2 - ������� ������
int AnalyzeLogForErrors(std::string &problem_string);

QString GetTimeStampInPSqlFormat(const QDateTime &now);

/// ���������������� ������ � ����
void RegisterSolverToDatabase();
/// ��������� ������ �� ����
void UnRegisterSolverFromDatabase();

/// ������ �������� ����� �� ���������� ���������, �������� ��������
/// ������ ������������ ������� ��� ����������� �� ���� ����������
//virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);
};

}//namespace RDK
#endif
