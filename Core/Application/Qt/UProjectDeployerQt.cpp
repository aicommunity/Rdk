#include "UProjectDeployerQt.h"
#include "../UApplication.h"

namespace RDK{

#define RDK_CAPTURE_EMPTY 0
#define RDK_CAPTURE_CREATED 64
#define RDK_CAPTURE_INITIALIZATION 1
#define RDK_CAPTURE_CONNECTED 2
#define RDK_CAPTURE_PAUSED 4
#define RDK_CAPTURE_ACTIVE 8
#define RDK_CAPTURE_DISCONNECTED 16
#define RDK_CAPTURE_RECONNECT 32


static
size_t write_response(void *ptr, size_t size, size_t nmemb, void *data)
{
  FILE *writehere = (FILE *)data;
  char* str = static_cast<char*>(ptr);
  //std::string s(str);
  //std::cout<<str<<std::endl;
  return nmemb;
}


UProjectDeployProcessingThread::UProjectDeployProcessingThread()
{
    deploymentState = DeploymentState::DS_Unknown;

    connect(&zip_process, SIGNAL(readyReadStandardError()), this, SLOT(processReadyReadStandardError()));
    connect(&zip_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processReadyReadStandardOutput()));

}

UProjectDeployProcessingThread::~UProjectDeployProcessingThread()
{

}




void UProjectDeployProcessingThread::processReadyReadStandardError()
{
    //std::cout<<"Unzip process std_error: "<<zip_process.readAllStandardError();

}

void UProjectDeployProcessingThread::processReadyReadStandardOutput()
{
    /*QString s = zip_process.readAllStandardOutput();
    //TODO: рассмотреть, что там на выхлопе и научиться скачивать прогресс
    std::cout<<"Unzip process std_output: "<<s;*/
}

void UProjectDeployProcessingThread::SetDatabasePath(const QString& path)
{
 databasePath = path;
}

void UProjectDeployProcessingThread::SetDownloadTempPath(const QString& path)
{
    downloadTempPath = path;
}

void UProjectDeployProcessingThread::SetFtpRemoteBasePath(const QString& path)
{
    ftpRemoteBasePath = path;
}

void UProjectDeployProcessingThread::SetStorageMountPath(const QString& path)
{
    storageMountPath = path;
}

void UProjectDeployProcessingThread::SetDeploymentState(const DeploymentState& state)
{
 QMutexLocker locker(&deploymentStateMutex);
 deploymentState = state;
}

void UProjectDeployProcessingThread::SetDeploymentProgress(const int &progress)
{
 QMutexLocker locker(&deploymentProgressMutex);
 deploymentProgress = progress;
}

void UProjectDeployProcessingThread::SetDeploymentProgressCap(const int &value)
{
 QMutexLocker locker(&deploymentProgressCapMutex);
 deploymentProgressCap = value;
}

void UProjectDeployProcessingThread::PrepareProjectDeployment(bool task_template_dr, const QString& task_template_path,
                                                              bool task_weights_dr, const QString& task_weights_path,
                                                              bool task_script_dr, const QString& task_script_path,
                                                               bool task_vsrc_dr, int task_vrc_type, const QString& task_vsrc_path)
{
    template_dr = task_template_dr;
    template_path = task_template_path;

    weights_dr = task_weights_dr;
    weights_path = task_weights_path;

    script_dr = task_script_dr;
    script_path = task_script_path;

    videosource_type = task_vrc_type;
    videosource_dr = task_vsrc_dr;
    videosource_path = task_vsrc_path;

    //Тут возможно еще какой-то код подготовки к деплою проекта со стороны потока
}

void UProjectDeployProcessingThread::RunProjectDeployment()
{
    //Запустить процесс деплоймента
    this->start();
}

///Get
QString UProjectDeployProcessingThread::GetDatabasePath()
{
 return databasePath;
}

QString UProjectDeployProcessingThread::GetFtpRemoteBasePath()
{
 return ftpRemoteBasePath;
}

QString UProjectDeployProcessingThread::GetDownloadTempPath()
{
 return downloadTempPath;
}

DeploymentState UProjectDeployProcessingThread::GetDeploymentState()
{
 return deploymentState;
}

/*
std::string UProjectDeployProcessingThread::GetDeploymentStateString()
{
 std::string result;
 switch(deploymentState)
 {
    case DeploymentState::DS_UnpackData:
     result = "DS_UnpackData";
     break;
 case DeploymentState::DS_DownloadData:
      result = "DS_DownloadData";
      break;
 case DeploymentState::DS_UnpackTemplate:
      result = "DS_UnpackTemplate";
      break;
 case DeploymentState::DS_UnpackScripts:
      result = "DS_UnpackScripts";
      break;
 case DeploymentState::DS_UnpackWeights:
      result = "DS_UnpackWeights";
      break;
 case DeploymentState::DS_DownloadTemplate:
      result = "DS_DownloadTemplate";
      break;
 case DeploymentState::DS_DownloadScripts:
      result = "DS_DownloadScripts";
      break;
 case DeploymentState::DS_DownloadWeights:
      result = "DS_DownloadWeights";
      break;
 default:
     result="ERROR";
 }
 return result;
}
*/

int UProjectDeployProcessingThread::GetDeploymentProgress()
{
    QMutexLocker locker(&deploymentProgressMutex);
    return deploymentProgress;
}

int UProjectDeployProcessingThread::GetDeploymentProgressCap()
{
    QMutexLocker locker(&deploymentProgressCapMutex);
    return deploymentProgressCap;
}

bool UProjectDeployProcessingThread::RecursiveCopyFiles(const QString& src_dir_path, const QString& dst_dir_path)
{
    //Перечислить папки
    QDir src_dir(src_dir_path);
    QDir dst_dir(dst_dir_path);

    //Перепроверить наличие
    if(!src_dir.exists())
    {
        lastError = "Copy err: source dir not exists "+src_dir_path;
        return false;
    }
    if(!dst_dir.exists())
    {
        if(!dst_dir.mkpath(dst_dir.path()))
        {
            lastError = "Error create dst dir: "+dst_dir.path();
            return false;
        }
    }

    //Рекурсивно скопировать подпапки
    QStringList src_dir_dirlist = src_dir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::Dirs);
    for(QString subdir_name: src_dir_dirlist)
    {
        QString src_subdir_path = src_dir_path+"/"+subdir_name;
        QString dst_subdir_path = dst_dir_path+"/"+subdir_name;

        QDir dst_subdir(dst_subdir_path);
        if(!dst_subdir.exists())
        {
            if(!dst_subdir.mkpath(dst_subdir.path()))
            {
                lastError = "Error create dst subdir: "+dst_dir.path();
                return false;
            }
        }
        if(!RecursiveCopyFiles(src_subdir_path, dst_subdir_path))
        {
            return false;
        }
    }

    //Скопировать файлы
    //Перечислить файлы:
    QStringList src_dir_filelist = src_dir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::Files);
    for(QString src_file_name: src_dir_filelist)
    {
        QString src_file_path = src_dir_path+"/"+src_file_name;
        QFile src_file(src_file_path);
        if(!src_file.exists())
        {
            lastError = "Src file not exists: "+src_file_path;
            return false;
        }

        QString dst_file_path = dst_dir_path+"/"+src_file_name;
        QFile dst_file(dst_file_path);
        if(!dst_file.exists())
        {
            if(!src_file.copy(dst_file_path))
            {
                lastError = "Error copy file "+src_file_path+" to "+dst_file_path;
                return false;
            }
        }
    }

    return true;
}


void UProjectDeployProcessingThread::run()
{
    DeployTemplate2();
    if(!VerifyTemplate())
    {


        //Обработка ошибок, сигнализация о проблемах и выход из потока
        //Запись чего-то в лог
        //Изначально самая примитивная верификация, потом усложним
    }

    DeployScript2();
    if(!VerifyScript())
    {
        //Обработка ошибок, сигнализация о проблемах и выход из потока
        //Запись чего-то в лог
        //Изначально самая примитивная верификация, потом усложним
    }

    DeployWeights2();
    if(!VerifyWeights())
    {
        //Обработка ошибок, сигнализация о проблемах и выход из потока
        //Запись чего-то в лог
        //Изначально самая примитивная верификация, потом усложним
    }

    DeployData2();
    if(!VerifyData())
    {
        //Обработка ошибок, сигнализация о проблемах и выход из потока
        //Запись чего-то в лог
        //Изначально самая примитивная верификация, потом усложним
    }
    deploymentState = DS_DeployFinished;
}

std::string UProjectDeployProcessingThread::GetLastError()
{
    return lastError.toUtf8().constData();
}

bool UProjectDeployProcessingThread::DownloadZip(const QString &remote_url, const QString& dst_zip_file)
{
    SetDeploymentProgressCap(100);
    SetDeploymentProgress(0);

    if(remote_url=="")
    {
        //8QMessageBox::information(this, "error", "Remote file path is empty");

        std::cout<<"Remote file path is empty";

        std::stringstream ss;
        lastError = "Zip download file path was empty";
        return false;
    }
    if(dst_zip_file=="")
    {
        //QMessageBox::information(this, "error", "Path to receive file is empty");
        std::cout<<"Path to receive file is empty";
        lastError = "Zip destination file path was empty";
        return false;
    }

    QString save_filename = "";
    bool rename = false;

    QFileInfo fi(dst_zip_file);
    if(!fi.isDir())
    {
     //std::cout<<"File name is not directory, so downloaded file will be renamed";
     //rename=true;
     if(fi.exists())
     {
      std::cout<<"File "<<fi.absoluteFilePath().toUtf8().constData()<< " already exists";
      //Перекачивать мы не будем, мб потом еще каких-то проверок навертеть
      return true;
     }
     else
     {
      save_filename = fi.absoluteFilePath();
     }
    }
    else
    {
     if(!fi.exists())
     {
      std::cout<<"Directory to save file is not exists, create it before start";
      lastError = "Attempt to download/unpack to inexistent directory";
      return false;
     }

     QStringList ls = dst_zip_file.split("/");
     if(ls.size()<2)
     {
         std::cout<<"Wrong remote file path '"<<dst_zip_file.toUtf8().constData();
         lastError = "Wrong zip download remote file path";
         return false;
     }


     //Назвать как папку в папке стремно, требуется проверка в процессе отладки
     //TODO: возможно, припиливать исходное имя файла?
     QString fn = ls[ls.size()-1];
     QString filename = dst_zip_file+"/"+fn;
     QFileInfo fi2(filename);
     if(fi2.exists())
     {
      std::cout<<"Change save file name, current already exists: "<<fi2.absoluteFilePath().toUtf8().constData();
      return false;
     }
     save_filename = filename;
    }

    QString download_filename = save_filename;
    if(rename)
    {
     int id=0;
     QFileInfo fi(download_filename);
     do
     {
       download_filename = fi.path()+"/"+"temp_"+QString::number(id);
       id+=1;
     }
     while(QFileInfo(download_filename).exists());
    }

    std::string s = save_filename.toUtf8().constData();
    struct FtpFile ftp_file;
    ftp_file.stream=NULL;
    ftp_file.filename = s.c_str();

    curl = curl_easy_init();

    if(curl)
    {
        // Положить ссылку на удаленный объект
        curl_easy_setopt(curl, CURLOPT_URL, remote_url.toUtf8().constData());

        // Добавляем функцию для отображения процесса
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_response);

        /* Define our callback to get called when there's data to be written */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
        /* Set a pointer to our struct to pass to the callback */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftp_file);

        /* Switch on full protocol/debug output */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);

        if(CURLE_OK != res) {
          /* we failed */
          std::cerr<<"curl told us "<<res<<std::endl;
          std::stringstream ss_curl;
          ss_curl<<"curl told us "<<res;
          lastError = ss_curl.str().c_str();
          return false;
        }
    }
    if(ftp_file.stream!=NULL)
    {
        fclose(ftp_file.stream);
    }

    SetDeploymentProgress(100);
    //Тут еще переименование, копирование, обработка и тп??
   return true;
}

bool UProjectDeployProcessingThread::UnpackZipFolder(const QString &local_zip_folder, const QString& local_dst_folder)
{
    SetDeploymentProgressCap(100);
    SetDeploymentProgress(0);

    QDir dst_dir(local_dst_folder);
    if(!dst_dir.exists())
        dst_dir.mkpath(dst_dir.path());

    QString cmd = "unzip -o "+local_zip_folder+" -d " + local_dst_folder;

    zip_process.start(cmd);
    while(!zip_process.waitForFinished(2))
    {
        QCoreApplication::processEvents();
    }
    SetDeploymentProgress(100);
    return true;
}

bool UProjectDeployProcessingThread::UnpackZipFile(const QString &local_zip_folder, const QString& local_dst_folder)
{
    SetDeploymentProgressCap(100);
    SetDeploymentProgress(0);
    //TODO: Убедиться, что файл залетает куда надо после распаковки
    QString cmd = "unzip -o "+local_zip_folder+" -d " + local_dst_folder;

    zip_process.start(cmd);
    while(!zip_process.waitForFinished(2))
    {
        QCoreApplication::processEvents();
    }
    SetDeploymentProgress(100);
    return true;
}

void UProjectDeployProcessingThread::DeployDirectory(const QString& file_with_template)
{
    QFileInfo tfi(file_with_template);
    QString project_dir_basic = tfi.path();

    QString database_template_dir_path = project_dir_basic;
    database_template_dir_path.replace("{Database}", databasePath);

    QString storage_template_dir_path = project_dir_basic;
    storage_template_dir_path.replace("{Database}", storageMountPath);

    //По умолчанию - все файлы, включая нашего, сложены в папочку
    //если файл наш один - он все равно сложен в папочку чтобы можно было паковать

    //Копируем папку
    RecursiveCopyFiles(storage_template_dir_path, database_template_dir_path);
}

void UProjectDeployProcessingThread::DeployTemplate2()
{
    if(!template_dr)
    {
        std::cout<<"Template already exists";
        return;
    }
    DeployDirectory(template_path);
}

void UProjectDeployProcessingThread::DeployTemplate()
{
    if(!template_dr)
    {
        std::cout<<"Template already exists";
        return;
    }

    //QString template_proj_path = template_path;

    QFileInfo tfi(template_path);

    QString project_dir_basic = tfi.path();

    QString ftp_project_path = project_dir_basic+".zip";
    ftp_project_path.replace("{Database}", ftpRemoteBasePath);

    QString download_zip_file = project_dir_basic+".zip";
    download_zip_file.replace("{Database}", downloadTempPath);

    QString unpack_zip_path = project_dir_basic;
    unpack_zip_path.replace("{Database}", databasePath);

    QFileInfo dwd_fi(download_zip_file);

    if(!dwd_fi.exists())
    {
        QDir dwd_fi_d(dwd_fi.absolutePath());
        if(!dwd_fi_d.exists())
        {
            dwd_fi_d.mkpath(dwd_fi_d.path());
        }
    }

    //Подготовить место для распаковки
    QDir du_d(unpack_zip_path);
    if(!du_d.exists())
    {
        du_d.mkpath(du_d.path());
    }

    //Теперь шаг 1 - загрузить
    //Предварительно - только если файла еще нет
    if(!dwd_fi.exists())
    {
        if(!DownloadZip(ftp_project_path, download_zip_file))
        {
            std::cout<<"Download ERROR";
            lastError+=" Download Error";
            SetDeploymentState(DeploymentState::DS_Error);
            return;
        }
    }


    //Шаг 2 = распаковать
    //Поехали распаковывать
    SetDeploymentState(DeploymentState::DS_UnpackData);
    if(!UnpackZipFolder(download_zip_file, du_d.path()))
    {
        std::cout<<"Unpack ERROR";
        lastError+=" Unpack Error";
        SetDeploymentState(DeploymentState::DS_Error);
        return;
    }
    std::cout<<"Unpack SUCCESS";



    //template_proj_path.replace("{Database}", downloadTempPath);

    /*std::cout<<"Start deploy template";
    //Рассмотрим, надо ли нам грузить по идее данные:
    if(template_dr)
    {
        QString template_proj_path = template_path;
        template_proj_path.replace("{Database}", downloadTempPath);

        //В файл
        QFileInfo proj_file(template_proj_path);
        QString template_zip_path = proj_file.absolutePath()+".zip";

        //На сервере он лежит по аналогии
        QString ftp_zip_path = template_zip_path;
        ftp_zip_path.replace(downloadTempPath, ftpRemoteBasePath);

        //Куда распаковывать еще надо будет уточнить, утром
        QString unpackZipPath = proj_file.absolutePath();
        unpackZipPath.replace(downloadTempPath, databasePath);

        QFileInfo db_fi(unpackZipPath);

        std::cout<<"template_zip_path = "<<template_zip_path.toUtf8().constData()<<" ftp_zip_path="<<ftp_zip_path.toUtf8().constData();

        QFileInfo fi(template_zip_path);
        if(!fi.exists())
        {
            //переключим:
            SetDeploymentState(DeploymentState::DS_DownloadTemplate);
            //инициируем загрузку
            std::cout<<"Start download";
            QDir dwd_path(fi.absoluteDir());
            dwd_path.mkpath(dwd_path.path());
            if(!DownloadZip(ftp_zip_path, template_zip_path))
            {
                //Как будем отработывать косяк... нипанятня
                SetDeploymentState(DeploymentState::DS_Error);
                return;
            }
        }
        else
        {
            //Чето тут тоже надо, когда уже все есть
        }

        std::cout<<"Downloaded file exists="<<fi.exists();
        if(fi.exists())
        {
            //Поехали распаковывать
            SetDeploymentState(DeploymentState::DS_UnpackData);

            //absolutePath() - вот это не точно, уточнить с примером
            if(!UnpackZipFolder(fi.absoluteFilePath(), db_fi.absolutePath()))
            {
                std::cout<<"Unpack ERROR";
                SetDeploymentState(DeploymentState::DS_Error);
                return;
            }
            std::cout<<"Unpack SUCCESS";
            //SetDeploymentState(DeploymentState::DS_);
        }
    }
    else
    {
        {
            //Как будем отработывать косяк... нипанятня
            //SetDeploymentState(DeploymentState::DS_Error);
            std::cout<<"Template already exists";
            return;
        }
    }*/
}

bool UProjectDeployProcessingThread::VerifyTemplate()
{
    QString proj_file_path = template_path;
    proj_file_path.replace("{Database}", databasePath);

    QFileInfo fi(proj_file_path);
    if(!fi.exists())
    {
        //Вообще,можно было бы и остальные проанализировать, ну, как вариант
        //Но это на этапе открытия вскроется
        return false;
    }
    return true;
}

void UProjectDeployProcessingThread::DeployScript2()
{
    if(!script_dr)
    {
        std::cout<<"Script already exists";
        return;
    }
    DeployDirectory(script_path);
}

void UProjectDeployProcessingThread::DeployScript()
{
    if(!script_dr)
        return;


    QString simulated_abs_file_path = script_path;
    QFileInfo fi(simulated_abs_file_path);
    QString base_path = fi.dir().path();

    QString remote_ftp_zip_path = base_path+".zip";
    remote_ftp_zip_path.replace("{Database}", ftpRemoteBasePath);

    QString zip_download_path = base_path+".zip";
    zip_download_path.replace("{Database}", downloadTempPath);

    QString deployment_unpack_dir = base_path;
    deployment_unpack_dir.replace("{Database}", databasePath);

    //Подготовить место для загрузки
    QFileInfo zd_fi(zip_download_path);
    QDir zd_d(zd_fi.absoluteDir());
    if(!zd_d.exists())
    {
        zd_d.mkpath(zd_d.path());
    }

    //Подготовить место для распаковки
    QDir du_d(deployment_unpack_dir);
    if(!du_d.exists())
    {
        du_d.mkpath(du_d.path());
    }



    //remote_ftp_zip_path = "ftp://192.168.102.171/database/CloudFtpDatabase/Configs/Detection/SqueezeDetHH.zip";
    //zip_download_path = "/home/ivan/RTV-VideoAnalytics/ExpDatabase/Temp/Configs/Detection/SqueezeDetHH.zip";

    //Теперь шаг 1 - загрузить
    //Предварительно - только если файла еще нет
    if(!zd_fi.exists())
    {
        if(!DownloadZip(remote_ftp_zip_path, zip_download_path))
        {
            std::cout<<"Download ERROR";
            lastError+=" Download Error";
            SetDeploymentState(DeploymentState::DS_Error);
            return;
        }
    }


    //Шаг 2 = распаковать
    //Поехали распаковывать
    SetDeploymentState(DeploymentState::DS_UnpackData);
    if(!UnpackZipFolder(zip_download_path, du_d.path()))
    {
        std::cout<<"Unpack ERROR";
        lastError+=" Unpack Error";
        SetDeploymentState(DeploymentState::DS_Error);
        return;
    }
    std::cout<<"Unpack SUCCESS";

}

bool UProjectDeployProcessingThread::VerifyScript()
{
 return true;
}

void UProjectDeployProcessingThread::DeployWeights2()
{
    if(!weights_dr)
    {
        std::cout<<"Script already exists";
        return;
    }
    DeployDirectory(weights_path);
}

void UProjectDeployProcessingThread::DeployWeights()
{
    if(!weights_dr)
        return;

    QFileInfo fi(weights_path);
    QString weights_base_path = fi.dir().path()+"/"+fi.baseName();

    QString dwd_zip_file = weights_base_path+".zip";
    dwd_zip_file.replace("{Database}", downloadTempPath);

    QString ftp_zip_file = weights_base_path+".zip";
    ftp_zip_file.replace("{Database}", ftpRemoteBasePath);

    QString zip_unpack_dir = fi.dir().path();
    zip_unpack_dir.replace("{Database}", databasePath);

    QFileInfo dwd_zip_fi(dwd_zip_file);
    QDir dwd_d(dwd_zip_fi.absoluteDir());
    if(!dwd_d.exists())
    {
        dwd_d.mkpath(dwd_d.path());
    }

    QDir zud(zip_unpack_dir);
    if(!zud.exists())
    {
        zud.mkpath(zud.path());
    }

    if(!dwd_zip_fi.exists())
    {
        if(!DownloadZip(ftp_zip_file, dwd_zip_file))
        {
           qDebug()<<"Download error";
            lastError+=" Download Error";
            SetDeploymentState(DeploymentState::DS_Error);
           return;
        }
    }
    if(dwd_zip_fi.exists())
    {
        if(!UnpackZipFolder(dwd_zip_file, zip_unpack_dir))
        {
            qDebug()<<"Unpack error";
            lastError+=" Unpack Error";
            SetDeploymentState(DeploymentState::DS_Error);
            return;
        }
    }


}

bool UProjectDeployProcessingThread::VerifyWeights()
{
    return true;
}


void UProjectDeployProcessingThread::DeployData2()
{
    if(!videosource_dr)
    {
        std::cout<<"Script already exists";
        return;
    }

    if(videosource_type==0)
    {
        QString database_videofile_path = videosource_path;
        database_videofile_path.replace("{Database}", databasePath);
        QFileInfo db_qfi(database_videofile_path);
        if(!db_qfi.exists())
        {
            QString storage_videofile_path = videosource_path;
            storage_videofile_path.replace("{Database}", storageMountPath);
            QFile st_qf(storage_videofile_path);

            if(st_qf.exists())
            {
                QDir db_vf_dir = db_qfi.absoluteDir();
                if(!db_vf_dir.exists())
                {
                    db_vf_dir.mkpath(db_vf_dir.path());
                }

                if(!st_qf.copy(database_videofile_path))
                {
                    lastError = "Error copy file "+storage_videofile_path+" to "+database_videofile_path;
                    return;
                }
            }
        }

    }
    else if(videosource_type==1)
    {
        //Набор картинок, имя соответствует имени папки,которую надо скопировать непосредственно

        QString database_template_dir_path = videosource_path;
        database_template_dir_path.replace("{Database}", databasePath);

        QString storage_template_dir_path = videosource_path;
        storage_template_dir_path.replace("{Database}", storageMountPath);

        //Копируем папку
        RecursiveCopyFiles(storage_template_dir_path, database_template_dir_path);
    }

}

void UProjectDeployProcessingThread::DeployData()
{
    if(!videosource_dr)
        return;

    QString data_base_path = videosource_path;

    QString dwd_zip_file = "";
    QString ftp_remote_path = "";
    QString unpack_zip_fiolder = "";

    if(videosource_type==0)
    {
        //Видеоролик: имя файла соответствует имени видео + *.zip

        dwd_zip_file = data_base_path+".zip";
        dwd_zip_file.replace("{Database}", downloadTempPath);

        ftp_remote_path = data_base_path+".zip";
        ftp_remote_path.replace("{Database}", ftpRemoteBasePath);

        QFileInfo qfi(data_base_path);
        unpack_zip_fiolder = qfi.dir().path();
        unpack_zip_fiolder.replace("{Database}", databasePath);
    }
    else if(videosource_type==1)
    {
        //Набор картинок, имя архива соответствует имени папки с картинками
        //распаковка в эту папку соответственно, картинки в корне архива

        dwd_zip_file = data_base_path+".zip";
        dwd_zip_file.replace("{Database}", downloadTempPath);

        ftp_remote_path = data_base_path+".zip";
        ftp_remote_path.replace("{Database}", ftpRemoteBasePath);

        QFileInfo qfi(data_base_path);
        unpack_zip_fiolder = qfi.dir().path()+"/"+qfi.baseName();
        unpack_zip_fiolder.replace("{Database}", databasePath);
    }

    QFileInfo dwd_fi(dwd_zip_file);
    QDir dwd_d(dwd_fi.absolutePath());

    if(!dwd_d.exists())
        dwd_d.mkpath(dwd_d.path());

    QDir unp_dir(unpack_zip_fiolder);
    if(!unp_dir.exists())
    {
        unp_dir.mkpath(unp_dir.path());
    }

    //Загрузить
    if(!DownloadZip(ftp_remote_path, dwd_zip_file))
    {
        qDebug()<<"Dowonlad error";
        lastError+=" Download Error";
        SetDeploymentState(DeploymentState::DS_Error);
        return;
    }

    if(!dwd_fi.exists())
    {
        qDebug()<<"File "<<dwd_fi.absoluteFilePath()<<" not exists, cannot be unpacked";
        lastError+=" Downloaded file not exists, cannot be unpacked";
        SetDeploymentState(DeploymentState::DS_Error);
        return;
    }

    if(!UnpackZipFolder(dwd_zip_file, unpack_zip_fiolder))
    {
        qDebug()<<"Zip file "+dwd_zip_file+" unpack error";
        lastError+=" Unpack error";
        SetDeploymentState(DeploymentState::DS_Error);
        return;
    }
}

bool UProjectDeployProcessingThread::VerifyData()
{
 return true;
}

//====================================================================
UProjectResultsUploadingThread::UProjectResultsUploadingThread()
{

}

UProjectResultsUploadingThread::~UProjectResultsUploadingThread()
{

}

void UProjectResultsUploadingThread::SetProjectResultsDirPath(const QString& results_dir)
{
    projectResultsDir = results_dir;
}

QString UProjectResultsUploadingThread::GetProjectResultsDirPath()
{
    return projectResultsDir;
}

void UProjectResultsUploadingThread::SetStorageResultsDirPath(const QString& results_dir)
{
    storageResultsDir = results_dir;
}

QString UProjectResultsUploadingThread::GetStorageResultsDirPath()
{
    return storageResultsDir;
}

void UProjectResultsUploadingThread::SetDatabasePath(const QString& database_path)
{
    databasePath = database_path;
}

QString UProjectResultsUploadingThread::GetDatabasePath()
{
    return databasePath;
}

void UProjectResultsUploadingThread::SetRemoteFtpPath(const QString& ftp_path)
{
    remoteFtpPath = ftp_path;
}

QString UProjectResultsUploadingThread::GetRemoteFtpPath()
{
    return remoteFtpPath;
}

void UProjectResultsUploadingThread::SetRemoteStoragePath(const QString& strg_path)
{
    remoteStoragePath = strg_path;
}

QString UProjectResultsUploadingThread::GetRemoteStoragePath()
{
    return remoteStoragePath;
}

QString UProjectResultsUploadingThread::GetLastError()
{
    return lastError;
}

DeploymentState UProjectResultsUploadingThread::GetUploadState()
{
    return uploadState;
}

void UProjectResultsUploadingThread::run()
{
    uploadState = DS_CopyResults;
    if(!CopyResultsToDestinationDir())
    {
        uploadState = DS_Error;
        return;
    }

    if(!CopyResultsToRemoteStorageDir())
    {
        uploadState = DS_Error;
        return;
    }

    /*
    uploadState = DS_PackResults;
    if(!ZipResults())
    {
        uploadState = DS_Error;
        return;
    }
    uploadState = DS_UploadResults;
    if(!UploadResultsViaFtp())
    {
        uploadState = DS_Error;
        return;
    }
    */

    uploadState = DS_UploadFinished;
}

bool UProjectResultsUploadingThread::CopyResultsToDestinationDir()
{
    //Разберем куда надо копировать
    if(projectResultsDir=="")
    {
        lastError = "projectResultsDir empty";
        return false;
    }

    QDir project_results_dir(projectResultsDir);
    if(!project_results_dir.exists())
    {
        lastError = "projectResultsDir not exists: "+projectResultsDir;
        return false;
    }

    if(storageResultsDir=="")
    {
        lastError = "storageResultsDir empty";
        return false;
    }

    if(databasePath=="")
    {
        lastError = "databasePath empty";
        return false;
    }

    QDir db_path_dir(databasePath);
    if(!db_path_dir.exists())
    {
        //lastError = "databasePath not exists: "+databasePath;
        db_path_dir.mkpath(db_path_dir.path());
        //return false;
    }

    QString storage_dest_dir_path = storageResultsDir;
    storage_dest_dir_path.replace("{Database}", databasePath);

    QDir storage_dest_dir(storage_dest_dir_path);

    if(!storage_dest_dir.exists())
    {
        lastError = "storage_dest_dir not exists: "+storage_dest_dir.path();
        return false;
    }

    bool copy_res = RecursiveCopyFiles(projectResultsDir, storage_dest_dir.path());

    if(!copy_res)
        return false;

    return true;
}

bool UProjectResultsUploadingThread::CopyResultsToRemoteStorageDir()
{
    QString remote_storage_dest_dir_path = storageResultsDir;
    remote_storage_dest_dir_path.replace("{Database}", remoteStoragePath);

    QDir storage_dest_dir(remote_storage_dest_dir_path);

    if(!storage_dest_dir.exists())
    {
        //lastError = "storage_dest_dir not exists: "+storage_dest_dir.path();
        //return false;
        storage_dest_dir.mkpath(storage_dest_dir.path());
    }

    bool copy_res = RecursiveCopyFiles(projectResultsDir, storage_dest_dir.path());

    if(!copy_res)
        return false;

    return true;
}

bool UProjectResultsUploadingThread::RecursiveCopyFiles(const QString& src_dir_path, const QString& dst_dir_path)
{
    //Перечислить папки
    QDir src_dir(src_dir_path);
    QDir dst_dir(dst_dir_path);

    //Перепроверить наличие
    if(!src_dir.exists())
    {
        lastError = "Copy err: source dir not exists "+src_dir_path;
        return false;
    }
    if(!dst_dir.exists())
    {
        if(!dst_dir.mkpath(dst_dir.path()))
        {
            lastError = "Error create dst dir: "+dst_dir.path();
            return false;
        }
    }

    //Рекурсивно скопировать подпапки
    QStringList src_dir_dirlist = src_dir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::Dirs);
    for(QString subdir_name: src_dir_dirlist)
    {
        QString src_subdir_path = src_dir_path+"/"+subdir_name;
        QString dst_subdir_path = dst_dir_path+"/"+subdir_name;

        QDir dst_subdir(dst_subdir_path);
        if(!dst_subdir.exists())
        {
            if(!dst_subdir.mkpath(dst_subdir.path()))
            {
                lastError = "Error create dst subdir: "+dst_dir.path();
                return false;
            }
        }
        if(!RecursiveCopyFiles(src_subdir_path, dst_subdir_path))
        {
            return false;
        }
    }

    //Скопировать файлы
    //Перечислить файлы:
    QStringList src_dir_filelist = src_dir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::Files);
    for(QString src_file_name: src_dir_filelist)
    {
        QString src_file_path = src_dir_path+"/"+src_file_name;
        QFile src_file(src_file_path);
        if(!src_file.exists())
        {
            lastError = "Src file not exists: "+src_file_path;
            return false;
        }
        QString dst_file_path = dst_dir_path+"/"+src_file_name;
        QFile dst_file(dst_file_path);
        if(!dst_file.exists())
        {
            if(!src_file.copy(dst_file_path))
            {
                lastError = "Error copy file "+src_file_path+" to "+dst_file_path;
                return false;
            }
        }
    }

    return true;
}

bool UProjectResultsUploadingThread::ZipResults()
{
    return true;
}

void UProjectResultsUploadingThread::processReadyReadStandardError()
{

}

void UProjectResultsUploadingThread::processReadyReadStandardOutput()
{

}

bool UProjectResultsUploadingThread::UploadResultsViaFtp()
{
    return true;
}








//====================================================================
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProjectDeployerQt::UProjectDeployerQt(void):
    db(NULL),
    deployProcessingThread(NULL),
    projectResultsUploadingThread(NULL),
    deploymentState(DS_NULL),
    preparationResult(-1)
{
    //Инициализация curl, которую надо выполнить только один раз
    //Этот класс же тоочно не создается дважды?
    curl_global_init(CURL_GLOBAL_DEFAULT);

  MLlibDescr classification;
  classification.LibName = "TPyUBitmapClassifier";
  classification.LibConfigFileTagName = "";
  classification.LibScriptFileTagName = "PythonScriptFileName";
  classification.LibWeightFileTagName = "WeightsPath";
  classification.LibClassCountTagName = "NumClasses";
  file_tags["TPyUBitmapClassifier"] = classification;
  component_classes.push_back("TPyUBitmapClassifier");
  MLlibDescr sq_det;
  sq_det.LibName = "TPyObjectDetectorSqueezeDet";
  sq_det.LibConfigFileTagName="ConfigPath";
  sq_det.LibScriptFileTagName="PythonScriptFileName";
  sq_det.LibWeightFileTagName="WeightsPath";
  sq_det.LibClassCountTagName = "";
  file_tags["TPyObjectDetectorSqueezeDet"] = sq_det;
  component_classes.push_back("TPyObjectDetectorSqueezeDet");
  MLlibDescr det_yolo;
  det_yolo.LibName="TPyObjectDetector";
  det_yolo.LibConfigFileTagName="ConfigPathYOLO";
  det_yolo.LibScriptFileTagName="PythonScriptFileName";
  det_yolo.LibWeightFileTagName="WeightsPathYOLO";
  det_yolo.LibClassCountTagName = "";
  file_tags["TPyObjectDetector"] = det_yolo;
  component_classes.push_back("TPyObjectDetector");
  MLlibDescr protobuf;
  protobuf.LibName = "TPySegmentatorProtobuf";
  protobuf.LibConfigFileTagName = "JSONPath";
  protobuf.LibScriptFileTagName = "PythonScriptFileName";
  protobuf.LibWeightFileTagName = "ProtobufPath";
  protobuf.LibClassCountTagName = "";
  file_tags["TPySegmentatorProtobuf"] = protobuf;
  component_classes.push_back("TPySegmentatorProtobuf");
  MLlibDescr unet;
  unet.LibName = "TPySegmentatorUNet";
  unet.LibConfigFileTagName = "";
  unet.LibScriptFileTagName = "PythonScriptFileName";
  unet.LibWeightFileTagName = "WeightsPath";
  unet.LibClassCountTagName = "";
  file_tags["TPySegmentatorUNet"] = unet;
  component_classes.push_back("TPySegmentatorUNet");
  MLlibDescr classifier_opencv;
  classifier_opencv.LibName = "UOpenCvClassifier";
  classifier_opencv.LibConfigFileTagName = "";
  classifier_opencv.LibScriptFileTagName = "";
  classifier_opencv.LibWeightFileTagName = "ModelPath";
  classifier_opencv.LibClassCountTagName = "NumClasses";
  file_tags["UOpenCvClassifier"] = classifier_opencv;
  component_classes.push_back("UOpenCvClassifier");
  MLlibDescr detector_opencv;
  detector_opencv.LibName = "UOpenCvDetector";
  detector_opencv.LibConfigFileTagName = "ConfigPath";
  detector_opencv.LibScriptFileTagName = "";
  detector_opencv.LibWeightFileTagName = "ModelPath";
  detector_opencv.LibClassCountTagName = "";
  file_tags["UOpenCvDetector"] = detector_opencv;
  /*
  component_classes.push_back("UOpenCvDetector");
  MLlibDescr classification_tester;
  detector_opencv.LibName = "TPyPredictSort";
  detector_opencv.LibConfigFileTagName = "ConfigPath";
  detector_opencv.LibScriptFileTagName = "PythonScriptFileName";
  detector_opencv.LibWeightFileTagName = "WeightPath";
  detector_opencv.LibClassCountTagName = "";
  file_tags["TPyPredictSort"] = classification_tester;
  component_classes.push_back("TPyPredictSort");
  */

  CaptureLibDescr opencv_videocapture;
  opencv_videocapture.LibName = "TCaptureOpenCV";
  opencv_videocapture.LibFrameIdStateName = "FramePosition";
  opencv_videocapture.LibCaptureStateTagName = "CaptureState";
  capture_tags["TCaptureOpenCV"] = opencv_videocapture;

  CaptureLibDescr imseq_capture;
  imseq_capture.LibName = "TCaptureImageSequence";
  imseq_capture.LibFrameIdStateName = "FramePosition";
  imseq_capture.LibCaptureStateTagName = "CaptureState";
  capture_tags["TCaptureImageSequence"] = imseq_capture;

}
UProjectDeployerQt::~UProjectDeployerQt(void)
{
    UnRegisterSolverFromDatabase();
    curl_global_cleanup();
    if(!DestroyProcessingThread())
    {
        std::cout<<"Force delete processing thread!";
        delete deployProcessingThread;
    }
    if(db!=NULL)
    {
        delete db;
    }
}
// --------------------------

//Удалить поток обработки или вернуть
bool UProjectDeployerQt::DestroyProcessingThread()
{
    if(deployProcessingThread==NULL)
        return true;

    if(deployProcessingThread!=NULL)
    {
        if(deployProcessingThread->GetDeploymentState()==DeploymentState::DS_Error ||
           deployProcessingThread->GetDeploymentState()==DeploymentState::DS_Unknown)
        {
            delete deployProcessingThread;
            deployProcessingThread=NULL;
            return true;
        }
        else
        {
            if(!deployProcessingThread->isRunning())
            {
                delete deployProcessingThread;
                deployProcessingThread=NULL;
                return true;
            }
        }
    }
    return false;
}

int UProjectDeployerQt::StartProjectDeployment(int deploy_task_id)
{
 task_id = deploy_task_id;
 int ds = GetDeploymentState();
 if(ds!=DeploymentState::DS_NULL)
 {
    lastError = "Deployment thread exists and currently active";
     return 20;
 }

 deploymentState=DS_NULL;

 if(!DestroyProcessingThread())
 {
    lastError = "Deployment thread exists and cannot be destroyed";
     return 20;
 }

 if(task_id<0)
 {
    std::stringstream ss;
    ss<<"Wrong task_id sent by server controller "<<task_id;
    lastError = ss.str();
    return 1;
 }

 if(!db)
 {
    lastError = "Database pointer is NULL";
     return 1;
 }
 if(!db->isOpen()&&db->isValid())
 {
    lastError = "Database connection is not opened or database connection is not valid";
    return 2;
 }

 //Получить данные из базы (по индексу)
 QSqlQuery q(*db);
 q.prepare("SELECT task_name, task_template, task_weights, task_src_type, task_src_id FROM vid_an.task_list WHERE task_id="+QString::number(task_id)+";");
 q.exec();
 q.first();
 if(!q.isValid())
 {
     lastError = "Task query by task id invalid";
     return 3;
 }


 task_name = q.value(0).toString();
 task_template_id = q.value(1).toInt();
 task_weights_id = q.value(2).toInt();
 task_src_type = q.value(3).toInt();
 task_src_id = q.value(4).toInt();

 q.finish();
 q.clear();

 //Извлеаем последовательно данные о том, где должны располагаться распакованные файлы
 //Сначала проект
 q.prepare("SELECT template_name, template_file, template_script FROM vid_an.templates WHERE template_id="+QString::number(task_template_id)+";");
 q.exec();
 q.first();
 if(!q.isValid())
 {
    lastError = "Project query by project id invalid";
     return 4;
 }

 task_template_name = q.value(0).toString();
 task_template_path = q.value(1).toString();
 task_script_id = q.value(2).toInt();

 q.finish();
 q.clear();

 QString database_path = Application->GetDatabaseMainPath().c_str();
 QString storage_path = Application->GetStorageMountPoint().c_str();

 QString abs_template_file = task_template_path;
 abs_template_file.replace("{Database}", database_path);

 QFileInfo t_file(abs_template_file);

 task_template_file_name = t_file.fileName();

 if(!t_file.exists())
 {
     task_template_download_required = true;
 }
 else
 {
     task_template_download_required = false;
 }

 q.prepare("SELECT weight_path, weight_confpath, weight_numcls FROM vid_an.weights WHERE weight_id="+QString::number(task_weights_id)+";");
 q.exec();
 q.first();

 if(!q.isValid())
 {
    lastError = "Weights query by weights id invalid";
     return 5;
 }

 task_weights_path = q.value(0).toString();
 QString weights_conf_path = q.value(1).toString();
 weights_classes_number = q.value(2).toInt();
 //task_script_id = q.value(3).toInt();

 q.finish();
 q.clear();

 if(task_weights_path!="" && weights_classes_number>0)
 {
     absolute_weights_file = task_weights_path;
     absolute_weights_file.replace("{Database}", database_path);
     absolute_config_file = weights_conf_path;
     absolute_config_file.replace("{Database}", database_path);

     QFileInfo fi_weights_path(absolute_weights_file), fi_weights_conf_path(absolute_config_file);
     if(!fi_weights_path.exists() || ((weights_conf_path!="")&&(!fi_weights_conf_path.exists())))
     {
         task_weights_download_required = true;
         /*
         task_weights_zip_url = QString(fi_weights_path.filePath()+".zip").replace(database_path, ftp_main_path).toUtf8().constData();
         if(fi_weights_path.filePath()!=fi_weights_conf_path.filePath())
         {
             task_weights_conf_download_required = true;
             task_weights_conf_zip_url = QString(fi_weights_conf_path.filePath()+".zip").replace(database_path, ftp_main_path).toUtf8().constData();
         }*/
     }
     else
     {
         task_weights_download_required = false;
     }
 }

 if(task_script_id==0 || task_script_id==-1)
 {
    task_script_download_required=false;
    task_script_path="";
 }
 else
 {
     q.prepare("SELECT script_path,script_name FROM vid_an.scripts WHERE script_id="+QString::number(task_script_id)+";");
     q.exec();
     q.first();
     if(!q.isValid())
     {
        lastError = "Script query by script id invalid";
        QString s = q.lastError().text();
         return 6;
     }
     task_script_path = q.value(0).toString();
     QString script_name = q.value(1).toString();

     //TODO: Do something if dummy id change
     if(task_script_id==2 && script_name=="NoScript")
     {
        task_script_download_required=false;
        task_script_path="";
     }
     else
     {
         absolute_script_file = task_script_path;
         absolute_script_file.replace("{Database}", database_path);
         QFileInfo fi_script_path(absolute_script_file);

         q.finish();
         q.clear();

         if(!fi_script_path.exists())
         {
             task_script_download_required=true;
             //task_script_zip_url = QString(fi_script_path.filePath()+".zip").replace(database_path, ftp_main_path).toUtf8().constData();
         }
         else
         {
             task_script_download_required=false;
         }
     }
 }
 if(task_src_type==0) //video file
 {
     q.prepare("SELECT video_relpath,video_frlen FROM vid_an.videos WHERE video_id="+QString::number(task_src_id)+";");
     q.exec();
     q.first();
     if(!q.isValid())
     {
        lastError = "Video query by video id invalid";
         return 7;
     }
     task_src_path = q.value(0).toString();
     task_src_frame_length = q.value(1).toInt();
     task_src_fullpath = task_src_path;
     task_src_fullpath.replace("{Database}", database_path);
     QFileInfo qfi_src_file(task_src_fullpath);
     if(!qfi_src_file.exists())
     {
         task_src_download_required = true;
         //task_src_zip_url = QString(qfi_src_file.filePath()+".zip").replace(database_path, ftp_main_path).toUtf8().constData();
     }
     else
     {
         task_src_download_required = false;
     }
 }
 else if(task_src_type==1) //image sequence
 {
     q.prepare("SELECT seq_path,seq_frlen FROM vid_an.img_seqs WHERE seq_id="+QString::number(task_src_id)+";");
     q.exec();
     q.first();
     if(!q.isValid())
     {
        lastError = "Image sequence query by imseq id invalid";
         return 8;
     }
     task_src_path = q.value(0).toString();
     task_src_frame_length = q.value(1).toInt();
     task_src_fullpath = task_src_path;
     task_src_fullpath.replace("{Database}", database_path);
     //QFileInfo qfi_src_file(abs_src_path);
     QDir seq_dir(task_src_fullpath);
     if(seq_dir.exists())
     {
         QStringList ldr = seq_dir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::Files);
         if(ldr.size()>=task_src_frame_length)
         {
             task_src_download_required = false;
         }
         else
         {
             task_src_download_required = true;
         }
     }
     else
     {
         task_src_download_required = true;
     }
 }
 else
 {
     lastError = "Invalid video source type index";
     return 10;
 }

 //Потом и вообще, возможно, "/tmp/..."
 //Это куда грузить, распаковка должна идти по стандартному пути в {Database}/...
 download_temp_path = database_path+"/Temp";


 if(deployProcessingThread)
 {
     if(!DestroyProcessingThread())
     {
         lastError = "Unable to destroy processing thread";
         return 20;
     }
 }

 if(task_template_download_required||task_weights_download_required||
    task_script_download_required||task_src_download_required)
 {
     deployProcessingThread = new UProjectDeployProcessingThread();

     deployProcessingThread->SetDatabasePath(database_path);
     deployProcessingThread->SetDeploymentState(DS_Unknown);
     deployProcessingThread->SetDownloadTempPath(download_temp_path);
     deployProcessingThread->SetStorageMountPath(storage_path);
     /*
     if(ftp_remote_path!="")
     {
        deployProcessingThread->SetFtpRemoteBasePath(QString(ftp_remote_path.c_str()));
     }
     else
     {
        qDebug("Empty ftp_remote_path on deploy processing stage");
     }
     */

     std::cout<<"Initialize deployment thread: ttdr="<<task_template_download_required<<" atf="<<abs_template_file.toUtf8().constData()<<
            " twdr="<<task_weights_download_required<<" twp="<<task_weights_path.toUtf8().constData()<<" tsdr="<<task_script_download_required<<
            " tsp="<<task_script_path.toUtf8().constData()<<" tsrcdr="<<task_src_download_required<<" tst="<<task_src_type<<" tsp="<<task_src_path.toUtf8().constData();

     deployProcessingThread->PrepareProjectDeployment(task_template_download_required, task_template_path,
                                                      task_weights_download_required, task_weights_path,
                                                      task_script_download_required, task_script_path,
                                                      task_src_download_required, task_src_type, task_src_path);
     deployProcessingThread->RunProjectDeployment();
 }
 else
 {
    //Что по идее не совсем правда, так как доставка и не начиналась, но будет ли работать?
    this->deploymentState = DS_DeployFinished;
 }

 //lastError = "";
 return 0;
}

int UProjectDeployerQt::StartProjectRun(int task_id)
{
    if(task_id<0)
    {
       std::stringstream ss;
       ss<<"Wrong task_id sent by server controller "<<task_id;
       lastError = ss.str();
       return 1;
    }

    if(!db)
    {
       lastError = "Database pointer is NULL";
        return 1;
    }
    if(!db->isOpen()&&db->isValid())
    {
       lastError = "Database connection is not opened or database connection is not valid";
       return 2;
    }

    //Получить данные из базы (по индексу)
    QSqlQuery q(*db);
    q.prepare("SELECT task_name, task_template, task_weights, task_src_type, task_src_id FROM vid_an.task_list WHERE task_id="+QString::number(task_id)+";");
    q.exec();
    q.first();
    if(!q.isValid())
    {
        lastError = "Task query by task id invalid";
        return 3;
    }


    task_name = q.value(0).toString();
    task_template_id = q.value(1).toInt();
    task_weights_id = q.value(2).toInt();
    task_src_type = q.value(3).toInt();
    task_src_id = q.value(4).toInt();

    q.finish();
    q.clear();

    //Извлеаем последовательно данные о том, где должны располагаться распакованные файлы
    //Сначала проект
    q.prepare("SELECT template_name, template_file, template_script FROM vid_an.templates WHERE template_id="+QString::number(task_template_id)+";");
    q.exec();
    q.first();
    if(!q.isValid())
    {
       lastError = "Project query by project id invalid";
        return 4;
    }

    task_template_name = q.value(0).toString();
    task_template_path = q.value(1).toString();
    task_script_id = q.value(2).toInt();

    q.finish();
    q.clear();

    QString database_path = Application->GetDatabaseMainPath().c_str();

    QString abs_template_file = task_template_path;
    abs_template_file.replace("{Database}", database_path);

    QFileInfo t_file(abs_template_file);

    task_template_file_name = t_file.fileName();

    q.prepare("SELECT weight_path, weight_confpath, weight_numcls FROM vid_an.weights WHERE weight_id="+QString::number(task_weights_id)+";");
    q.exec();
    q.first();

    if(!q.isValid())
    {
       lastError = "Weights query by weights id invalid";
        return 5;
    }

    task_weights_path = q.value(0).toString();
    QString weights_conf_path = q.value(1).toString();
    weights_classes_number = q.value(2).toInt();
    //task_script_id = q.value(3).toInt();

    q.finish();
    q.clear();

    absolute_weights_file = task_weights_path;
    absolute_weights_file.replace("{Database}", database_path);
    absolute_config_file = weights_conf_path;
    absolute_config_file.replace("{Database}", database_path);

    QFileInfo fi_weights_path(absolute_weights_file), fi_weights_conf_path(absolute_config_file);


    if(task_script_id==0 || task_script_id==-1)
    {
       task_script_path="";
    }
    else
    {
        q.prepare("SELECT script_path,script_name FROM vid_an.scripts WHERE script_id="+QString::number(task_script_id)+";");
        q.exec();
        q.first();
        if(!q.isValid())
        {
           lastError = "Script query by script id invalid";
           QString s = q.lastError().text();
            return 6;
        }
        task_script_path = q.value(0).toString();
        QString script_name = q.value(1).toString();

        //TODO: Do something if dummy id change
        if(task_script_id==2 && script_name=="NoScript")
        {
           task_script_path="";
        }
        else
        {
            absolute_script_file = task_script_path;
            absolute_script_file.replace("{Database}", database_path);
            QFileInfo fi_script_path(absolute_script_file);

            q.finish();
            q.clear();
        }
    }
    if(task_src_type==0) //video file
    {
        q.prepare("SELECT video_relpath,video_frlen FROM vid_an.videos WHERE video_id="+QString::number(task_src_id)+";");
        q.exec();
        q.first();
        if(!q.isValid())
        {
           lastError = "Video query by video id invalid";
            return 7;
        }
        task_src_path = q.value(0).toString();
        task_src_frame_length = q.value(1).toInt();
        task_src_fullpath = task_src_path;
        task_src_fullpath.replace("{Database}", database_path);
        QFileInfo qfi_src_file(task_src_fullpath);
    }
    else if(task_src_type==1) //image sequence
    {
        q.prepare("SELECT seq_path,seq_frlen FROM vid_an.img_seqs WHERE seq_id="+QString::number(task_src_id)+";");
        q.exec();
        q.first();
        if(!q.isValid())
        {
           lastError = "Image sequence query by imseq id invalid";
            return 8;
        }
        task_src_path = q.value(0).toString();
        task_src_frame_length = q.value(1).toInt();
        task_src_fullpath = task_src_path;
        task_src_fullpath.replace("{Database}", database_path);
        //QFileInfo qfi_src_file(abs_src_path);
    }
    else
    {
        lastError = "Invalid video source type index";
        return 10;
    }
    this->deploymentState = DS_DeployFinished;

    return 0;
}

void UProjectDeployerQt::AConnectToDatabase()
{
    if(db==NULL)
    {
        db=new QSqlDatabase();
    }
    else
    {
        if(db->isOpen())
            db->close();
        delete db;
        db=new QSqlDatabase();
    }

    *db = QSqlDatabase::addDatabase("QPSQL");

    db->setHostName(database_address.c_str());
    db->setDatabaseName(database_name.c_str());
    db->setUserName(database_login.c_str());
    db->setPassword(database_password.c_str());

    if(!db->open())
    {
        db->close();
        delete db;
        exit(1);
    }

    RegisterSolverToDatabase();
}

/*
enum DeploymentState
{
    DS_NULL,
    DS_Unknown,
    DS_DownloadTemplate,
    DS_UnpackTemplate,
    DS_DownloadWeights,
    DS_UnpackWeights,
    DS_DownloadScripts,
    DS_UnpackScripts,
    DS_DownloadData,
    DS_UnpackData,
    DS_Error
};
*/

std::string UProjectDeployerQt::GetLastError()
{
    std::string le = "zero le test";
    if(lastError!="")
    {
        le = std::string("UProjectDeployerQt lastError=")+lastError+std::string(" ");
    }
    if(deployProcessingThread==NULL)
    {
        if(le=="")
            le=le+std::string("DeployProcessingThread lastError: Deploy thread not exists. ");
    }
    else
    {
        std::string dtle = deployProcessingThread->GetLastError();
        if(dtle!="")
            le=le+std::string("DeployProcessingThread lastError: ")+dtle;
    }
    return le;
}

int UProjectDeployerQt::GetDeploymentState()
{
    if(deployProcessingThread==NULL)
    {
        return deploymentState;
    }
    else
    {
        bool t = deployProcessingThread->isRunning();
        return deployProcessingThread->GetDeploymentState();
    }
}

std::string UProjectDeployerQt::GetProjectFileName()
{
    if(task_template_path!="")
    {
        QFileInfo fi(task_template_path);
        if(fi.exists())
        {
            return task_template_path.toUtf8().constData();
        }
        else
        {
            return std::string();
        }
    }
    else
    {
        return std::string();
    }
}

int UProjectDeployerQt::GetStageCap()
{
    if(deployProcessingThread==NULL)
    {
        return -1;
    }
    else
    {
        return deployProcessingThread->GetDeploymentProgressCap();
    }
}

int UProjectDeployerQt::GetStageProgress()
{
    if(deployProcessingThread==NULL)
    {
        return -1;
    }
    else
    {
        return deployProcessingThread->GetDeploymentProgress();
    }
}

///Подготовить к запуску проект:
/// 1. Скопировать во временное хранилище
/// 2. Открыть в тестовом режиме и настроить пути и связи?
/// 3. Закрыть
int UProjectDeployerQt::PrepareProject(std::string &response)
{
    //lastError="";
    if(this->GetDeploymentState()!=DS_DeployFinished)
    {
        response = "Project deployment is not finished, can't start preparations";
        lastError=response;
        preparationResult=1;
        return 1;
    }

    //02.02.2021: Уничтожить поток (такое костыльное (?) решение, поток больше не нужен, удалить)
    DestroyProcessingThread();

    std::cerr<<"Deployment!\n";
    UpdateTaskStateInDb(task_id, TS_Deployment, -1.0f);

    this->deploymentState = DS_CopyProject;
    std::cerr<<"Copy to temporary folder!\n";
    if(CopyProjectToTempFolder()!=0)
    {
        this->deploymentState=DS_Error;
        response = "error during copying";
        lastError=response;
        preparationResult=1;
        UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
        return 1;
    }

    std::cerr<<"Start project preparation!\n";
    this->deploymentState = DS_PrepareProject;
    if(OpenProjectMockMode()!=0)
    {
        this->deploymentState=DS_Error;
        response = "error during opening in mock mode";
        UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
        lastError=response;
        preparationResult=1;
        return 1;
    }
    std::cerr<<"Analyze log for errors!\n";
    std::string bug_string="";
    if(AnalyzeLogForErrors(bug_string)>=0)
    {
        lastError="Mock open error: "+bug_string;
        UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
        preparationResult=1;
        return 1;
    }
    std::cerr<<"Setup project mock parameters!\n";
    if(SetupProjectMockParameters()!=0)
    {
        this->deploymentState=DS_Error;
        response = "error during mock parameters setup";
        UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
        lastError=response;
        preparationResult=1;
        return 1;
    }
    std::cerr<<"Close mock Project!\n";
    if(CloseMockProject()!=0)
    {
        this->deploymentState=DS_Error;
        response = "error during mocked project closing";
        UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
        lastError=response;
        preparationResult=1;
        return 1;
    }
    std::cerr<<"Analyze log for errors!\n";
    if(AnalyzeLogForErrors(bug_string)>=0)
    {
        lastError="Mock open error: "+bug_string;
        UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
        preparationResult=1;
        return 1;
    }
    std::cerr<<"Project prepared success!\n";
    this->deploymentState=DS_ProjectPrepared;
    response = "project prepared successfully";
    lastError=response;
    preparationResult=0;
    return 0;
}

int UProjectDeployerQt::AnalyzeLogForErrors(std::string &problem_string)
{
    std::list<std::string> gui_unsent_log = Application->GetEngineControl()->GetEngineStateThread()->ReadGuiUnsentLog();

    for(std::string log_entry: gui_unsent_log)
    {
        QString msg_line = log_entry.c_str();
        //Take string and split into parts
        int sys_info_end = msg_line.indexOf("] ", 0)+1;//index of first space of message

        if(sys_info_end>=0)
        {
            //Process line taking into account lines saved previously
            QString sys_info_line = msg_line.left(sys_info_end);
            QString message_data = msg_line.right(msg_line.length()-sys_info_end);
            QStringList str_parts = sys_info_line.split(" ");

            //QString src_info = str_parts[0];
            //src_info = src_info.replace(">", "").trimmed();
            //QString date_info = str_parts[1];
            //QString time_info = str_parts[2];
            QString error_level_i = str_parts[3];
            int error_level = error_level_i.replace(">", "").trimmed().toInt();
            //QString error_level_s = str_parts[4];
            //Unknown, Fatal, Error levels
            if(error_level>=0 && error_level<=2)
            {
                //std::cerr<<message_data.toUtf8().constData()<<"\n\n";
                problem_string = message_data.toUtf8().constData();
                //Found fatal error, nothing needed to read any longer
                return error_level;
            }
        }

    }

   //std::string log_path = Application->Get
   problem_string = "No error occured";
   return -1;
}

int UProjectDeployerQt::GetPreparationResult(std::string &response)
{
    response=lastError;
    return preparationResult;
}

///Открыть подготовленный проект
int UProjectDeployerQt::OpenPreparedProject(std::string &response)
{
    if(this->deploymentState!=DS_ProjectPrepared)
    {
        response = "Project is not prepared yet";
        lastError=response;
        return 1;
    }
    deploymentState = DS_OpenProject;
    //Здесь какие-то инструкции для правильного открытия проекта
    //RDK::Sleep(3);
    Application->SetStorageBuildMode(1);
    QString pdpath = GetTempProjectDeploymentPath().c_str();
    QString project_path = pdpath+"/"+task_template_file_name;

    std::cerr<<"Before OpenProject!\n";

    if(!Application->OpenProject(project_path.toUtf8().constData()))
    {
        deploymentState = DS_Error;
        lastError="UApplication OpenProject normally error";
        response = lastError;
        return 1;
    }

    std::cerr<<"After OpenProject!\n";

    if(AnalyzeLogForErrors(response)>=0)
    {
        lastError=response;
        deploymentState = DS_Error;
        return 1;
    }
    ////
    deploymentState = DS_ProjectOpened;
    response = "project opened successfully";
    lastError=response;
    return 0;
}

// --------------------------
// Методы транспортировки данных
// --------------------------

///Скопировать проект во временную директорию, в которой будет выполняться работа
int UProjectDeployerQt::CopyProjectToTempFolder()
{
    QString pdpath = GetTempProjectDeploymentPath().c_str();
    qDebug()<<"GetTempProjectDeploymentPath() = "<<pdpath;
    if(pdpath=="")
    {
        lastError="project temporary deployment path empty - cannot copy";
        return 1;
    }
    QString abs_ttp = task_template_path;

    qDebug()<<"task_template_path = "<<task_template_path;

    //std::cout<<"TASK TEMPLATE PATH = "<<task_template_path.toUtf8().constData()<<"\n";
    QString db_p = Application->GetDatabaseMainPath().c_str();

    qDebug()<<"db_p = "<<db_p;

    abs_ttp.replace("{Database}", db_p);

    qDebug()<<"abs_ttp = "<<abs_ttp;

    QFileInfo fi(abs_ttp);
    QDir src_dir(fi.absoluteDir());
    QStringList files_dirs = src_dir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::Files);
    qDebug()<<"Copy "<<src_dir.path()<<" to "<<pdpath;
    //Скопировать все файлы
    for(QString fn:files_dirs)
    {
        QFile::copy(src_dir.path()+"/"+fn, pdpath+"/"+fn);
    }
    QDir pddir(pdpath);
    //Обязательная для лога событий
    pddir.mkdir("EventsLog");
    //..и предполагаемая для результатов
    pddir.mkdir("Results");

    //RDK::Sleep(3);
    return 0;
}

/// Открыть проект в заглушенном режиме
int UProjectDeployerQt::OpenProjectMockMode()
{
    //Сделать заготовки библиотек
    Application->CreateSaveMockLibs();
    //Это у нас заглушки вместо компонентов
    Application->SetStorageBuildMode(3);
    QString pdpath = GetTempProjectDeploymentPath().c_str();
    QString project_path = pdpath+"/"+task_template_file_name;
    if(!Application->OpenProject(project_path.toUtf8().constData()))
    {
        lastError="UApplication OpenProject Mock Components error";
        return 1;
    }
    return 0;
}

/// Задать параметры проекта в заглушенном режиме
int UProjectDeployerQt::SetupProjectMockParameters()
{
    RDK::Sleep(3);

    RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>(0);

    if(!model) return 1;

    //Настроить видеоисточник

    RDK::UEPtr<RDK::UContainer> video_cont;
    RDK::UEPtr<RDK::UContainer> imseq_cont;
    std::vector<std::string> vid_names;
    vid_names = model->GetComponentsNameByClassName("TCaptureOpenCV", vid_names);
    std::vector<std::string> imseq_names;
    imseq_names = model->GetComponentsNameByClassName("TCaptureImageSequence", imseq_names);
    std::vector<std::string> predictor_names;
    predictor_names = model->GetComponentsNameByClassName("TPyPredictSort", predictor_names);

    std::cerr<<"vid_names = "<<vid_names.size()<<" imseq_names="<<imseq_names.size()<<" predictor_names="<<predictor_names.size()<<"\n";

    //Разобрать по условиям - какого типа у нас проект
    if(!vid_names.empty()||!imseq_names.empty())
    {
        std::cerr<<"VA variant\n";
        template_type = TemplateType::TT_VideoAnalytics;
        return SetupProjectMockParametersVideoAnalysis();
    }
    else if(!predictor_names.empty())
    {
        std::cerr<<"NIface variant\n";
        template_type = TemplateType::TT_NeuralInterface;
        return SetupProjectMockParametersNeuralInterface();
    }

    return 0;
}

int UProjectDeployerQt::SetupProjectMockParametersNeuralInterface()
{
    RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>(0);

    if(!model) return 1;

    //Настроить видеоисточник
    std::vector<std::string> predictor_names;
    predictor_names = model->GetComponentsNameByClassName("TPyPredictSort", predictor_names);

    //Тут в дальнейшем наверняка будет настройка разных предикторов, пока втупую один этот

    predictor_class_name = "TPyPredictSort";
    predictor_component_name = predictor_names[0];

    std::cerr<<"Pred: "<<predictor_class_name.c_str()<<" compname: "<<predictor_component_name.c_str()<<"\n";

    RDK::UEPtr<RDK::UContainer> predictor_container;
    predictor_container = model->GetComponentL(predictor_names[0]);
    std::string *sequence_path = predictor_container->AccessPropertyData<std::string>("ImagesDir");

    if(sequence_path!=NULL)
    {
        std::cerr<<"sequence_path!=NULL\n";
        *sequence_path = task_src_fullpath.toUtf8().constData();
    }


    std::string *weight_path = predictor_container->AccessPropertyData<std::string>("WeightPath");
    if(weight_path!=NULL)
    {
        //std::cerr<<"weight_path!=NULL\n";
        *weight_path = absolute_weights_file.toUtf8().constData();
    }

    //std::cerr<<"configuration_path\n";
    std::string *configuration_path = predictor_container->AccessPropertyData<std::string>("ConfigPath");
    *configuration_path = absolute_config_file.toUtf8().constData();

    //std::cerr<<"python_script_file_name\n";
    std::string *python_script_file_name = predictor_container->AccessPropertyData<std::string>("PythonScriptFileName");
    *python_script_file_name = absolute_script_file.toUtf8().constData();

    //Предположительно, это надо делать так:
    //std::cerr<<"python_script_file_name\n";

    return 0;
}

int UProjectDeployerQt::SetupProjectMockParametersVideoAnalysis()
{
    RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>(0);

    if(!model) return 1;

    //Настроить видеоисточник

    RDK::UEPtr<RDK::UContainer> video_cont;
    RDK::UEPtr<RDK::UContainer> imseq_cont;
    std::vector<std::string> vid_names;
    vid_names = model->GetComponentsNameByClassName("TCaptureOpenCV", vid_names);
    std::vector<std::string> imseq_names;
    imseq_names = model->GetComponentsNameByClassName("TCaptureImageSequence", imseq_names);
    std::vector<std::string> predictor_names;
    predictor_names = model->GetComponentsNameByClassName("TPyPredictSort", predictor_names);

    if(task_src_type==0)//TODO: Проверить - видео
    {
        if(vid_names.empty())
        {
            //Видеозаписей - нет
            lastError="Model does not contain video data source, fix project file/task parameters";
            return 1;
        }
        capture_class_name = "TCaptureOpenCV";
        capture_component_name = vid_names[0];
        video_cont = model->GetComponentL(vid_names[0]);
        bool *video_Activity = video_cont->AccessPropertyData<bool>("Activity");
        bool *video_EnableCapture = video_cont->AccessPropertyData<bool>("EnableCapture");
        std::string *video_CameraPath = video_cont->AccessPropertyData<std::string>("CameraPath");
        bool *video_OneShotRun = video_cont->AccessPropertyData<bool>("OneShotRun");
        bool *video_ProcessEvenFrames = video_cont->AccessPropertyData<bool>("ProcessEvenFrames");
        int *video_ProcessEveryXFrame = video_cont->AccessPropertyData<int>("ProcessEveryXFrame");
        std::string *video_DllName = video_cont->AccessPropertyData<std::string>("DllName");

        //bool *video_RepeatFlag = video_cont->AccessPropertyData<bool>("RepeatFlag");
        int *video_RestartMode = video_cont->AccessPropertyData<int>("RestartMode");

        bool *video_UseRelativePathFromConfig = video_cont->AccessPropertyData<bool>("UseRelativePathFromConfig");
        bool *video_UseRelativePathFromDir = video_cont->AccessPropertyData<bool>("UseRelativePathFromDir");

        *video_Activity = true;
        *video_EnableCapture = true;
        *video_CameraPath = task_src_fullpath.toUtf8().constData();
        *video_DllName = "VideoCaptureOpenCVDll";
        *video_OneShotRun = true;
        *video_ProcessEvenFrames = true;
        if(video_ProcessEveryXFrame)
            *video_ProcessEveryXFrame = 1;
        if(video_UseRelativePathFromConfig!=NULL)
            *video_UseRelativePathFromConfig = false;
        if(video_UseRelativePathFromDir!=NULL)
            *video_UseRelativePathFromDir = false;
        //*video_RepeatFlag = false;
        if(video_RestartMode)
            *video_RestartMode = 0;

        if(!imseq_names.empty())
        {
            imseq_cont = model->GetComponentL(imseq_names[0]);
            bool *act = imseq_cont->AccessPropertyData<bool>("Activity");
            //TODO: Brake wrong link
            model->BreakAllOutgoingLinks(imseq_names[0]);
            *act = false;
        }
    }
    else if(task_src_type==1)//TODO: Проверить - картинки
    {
        if(imseq_names.empty())
        {
            //Компонента с картинками - нет
            lastError="Model does not contain image sequence data source, fix project file/task parameters";
            return 1;
        }

        capture_class_name = "TCaptureImageSequence";
        capture_component_name = imseq_names[0];
        imseq_cont = model->GetComponentL(imseq_names[0]);
        bool *imseq_Activity = imseq_cont->AccessPropertyData<bool>("Activity");
        std::string *imseq_Path = imseq_cont->AccessPropertyData<std::string>("Path");
        bool *imseq_EnableCapture = imseq_cont->AccessPropertyData<bool>("EnableCapture");
        double *imseq_DesiredFps = imseq_cont->AccessPropertyData<double>("DesiredFps");
        bool *imseq_RepeatFlag = imseq_cont->AccessPropertyData<bool>("RepeatFlag");
        int  *imseq_RestartMode = imseq_cont->AccessPropertyData<int>("RestartMode");
        bool *imseq_UseRelativePathFromConfig = imseq_cont->AccessPropertyData<bool>("UseRelativePathFromConfig");
        bool *imseq_UseRelativePathFromDir = imseq_cont->AccessPropertyData<bool>("UseRelativePathFromDir");

        int *test = imseq_cont->AccessPropertyData<int>("ReconnectTimeout");

        *imseq_Activity = true;
        *imseq_Path = task_src_fullpath.toUtf8().constData();
        *imseq_EnableCapture = true;
        *imseq_DesiredFps = 0;
        *imseq_RepeatFlag = false;
        if(imseq_RestartMode)
            *imseq_RestartMode = 0;
        *imseq_UseRelativePathFromConfig = false;
        if(imseq_UseRelativePathFromDir)
            *imseq_UseRelativePathFromDir = false;

        if(!vid_names.empty())
        {
            video_cont = model->GetComponentL(vid_names[0]);
            bool *act = video_cont->AccessPropertyData<bool>("Activity");
            //TODO: Brake wrong link
            model->BreakAllOutgoingLinks(vid_names[0]);
            *act = false;
        }
    }
    else
    {
        //Тип источника странный, не поддерживается системой
        lastError="Video source type has undefined value";
        return 1;
    }

    //Дальше настройка конкретного компонента
    //std::vector<std::string> pipelineNames;
    /*RDK::UEPtr<RDK::UContainer> pipeline;
    pipeline = model->GetComponentL("Pipeline1");
    if(!pipeline.Get())
    {
        lastError="VideoAnalytics Pipeline1 class undetected";
        return 1;
    }*/
    std::vector<std::string> components_names;
    std::string class_name = "";
    for(std::string cls_name: component_classes)
    {
        components_names = model->GetComponentsNameByClassName(cls_name.c_str(), components_names, true);
        if(!components_names.empty())
        {
            class_name = cls_name;
            break;
        }
    }

    // !!! Here I do not take into account situation when      !!!
    // !!! more than one neural component present into project !!!
    if(components_names.empty() || class_name=="")
    {
        //No classes found
        lastError="No one neural network related component found";

        //16.02.2021. Изменено в связи с тем, как стал использоваться проект, иногда не требуется сеть
        //Требуется структурная переработка в будущем чтобы настройка была проще
        return 0;
        //return 1;
    }

    //Here component class found
    RDK::UEPtr<RDK::UContainer> neural_cont;
    neural_cont = model->GetComponentL(components_names[0]);

    bool *nn_Activity = neural_cont->AccessPropertyData<bool>("Activity");
    std::string *nn_ScriptFile;
    if(file_tags[class_name].LibScriptFileTagName!="")
        nn_ScriptFile = neural_cont->AccessPropertyData<std::string>(file_tags[class_name].LibScriptFileTagName.c_str());
    std::string *nn_WeightsFile = neural_cont->AccessPropertyData<std::string>(file_tags[class_name].LibWeightFileTagName.c_str());
    std::string *nn_ConfigFile;
    int *nn_ClassCount;
    if(file_tags[class_name].LibConfigFileTagName!="")
        nn_ConfigFile = neural_cont->AccessPropertyData<std::string>(file_tags[class_name].LibConfigFileTagName.c_str());

    if(file_tags[class_name].LibClassCountTagName!="")
        nn_ClassCount = neural_cont->AccessPropertyData<int>(file_tags[class_name].LibClassCountTagName.c_str());

    bool *nn_UseFullPath=NULL;
    if(file_tags[class_name].LibClassCountTagName!="")
        nn_UseFullPath = neural_cont->AccessPropertyData<bool>(file_tags[class_name].LibUseFullPathTagName.c_str());

    if(nn_UseFullPath!=NULL)
        *nn_UseFullPath = true;

    *nn_Activity = true;
    if(file_tags[class_name].LibScriptFileTagName!="")
        *nn_ScriptFile = absolute_script_file.toUtf8().constData();
    *nn_WeightsFile = absolute_weights_file.toUtf8().constData();
    if(file_tags[class_name].LibConfigFileTagName!="")
        *nn_ConfigFile = absolute_config_file.toUtf8().constData();
    if(file_tags[class_name].LibClassCountTagName!="")
    {
        if(nn_ClassCount)
            *nn_ClassCount = weights_classes_number;
    }

    return 0;
}

/// Закрыть заглушенный проект
int UProjectDeployerQt::CloseMockProject()
{
    //RDK::Sleep(3);
    Application->SaveProject();
    Application->CloseProject();
    return 0;
}

QString UProjectDeployerQt::GetTimeStampInPSqlFormat(const QDateTime &now)
{
    QString datetime = now.toString("yyyy-MM-dd hh:mm:ss");
    int offset_secs = now.offsetFromUtc();
    int offset_hours = offset_secs/3600;
    QString offset = (offset_hours>0)?("+"+QString::number(offset_hours).rightJustified(2, '0')):(QString::number(offset_hours).rightJustified(2, '0'));
    return (datetime+offset);
}

/// Запустить подготовленный проект
int UProjectDeployerQt::RunPreparedProject()
{
    //Сюда еще пойдет всякая херня типа сохранения даты и прочего, но это потом
    processing_start_datetime = QDateTime::currentDateTime();
    Application->StartChannel(-1);
    deploymentState = DS_Calculation;
    UpdateTaskStateInDb(task_id, TS_Calculation, -1.0f, GetTimeStampInPSqlFormat(processing_start_datetime));
    return 0;
}
///Возвращает состояние потока расчета (аналог -2/0/1 столбца в Гуях)
int UProjectDeployerQt::GetCalculationState()
{
    std::vector<UEngineStateThread::UCalcState> thread_states = Application->GetEngineControl()->GetEngineStateThread()->ReadCalcThreadStates();
    if(thread_states.empty())
    {
        lastError = "Calculation threads empty!";
        UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
        return -1;
    }
    if(thread_states.size()>1)
    {
        lastError = "More than one calculation threads in solver!";
        UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
        return -1;
    }
    UEngineStateThread::UCalcState &state = thread_states[0];
    return state;
}

///Возвращает состояние активного компонента захвата
/// возвращает false при ошибке получение состояния
/// @state - индекс состояния захвата (по состояниям, либо 10000 - захват закончил работу)
/// @frame_id - индекс текущего кадра
bool UProjectDeployerQt::GetCaptureState(int &state, unsigned long long& frame_id, unsigned long long& max_frame_id)
{
    if(template_type==TT_VideoAnalytics)
    {
        std::cerr<<"Get capture state VA\n";
        return GetCaptureStateVideoAnalysis(state, frame_id, max_frame_id);
    }
    else if(template_type==TT_NeuralInterface)
    {
        std::cerr<<"Get capture state NI\n";
        return GetCaptureStateNeuralInterface(state, frame_id, max_frame_id);
    }
    else
    {
        return false;
    }
}

bool UProjectDeployerQt::GetCaptureStateVideoAnalysis(int &state, unsigned long long& frame_id, unsigned long long& max_frame_id)
{
    if(capture_class_name=="")
    {
       state=-1;
       frame_id=-1;
       max_frame_id=-1;
       lastError = "Capture class name indefined";
       UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
       return false;
    }

    if(capture_component_name=="")
    {
       state=-1;
       frame_id=-1;
       max_frame_id=-1;
       lastError = "Capture component name indefined";
       UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
       return false;
    }

    RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>(0);

    if(!model)
    {
       state=-1;
       frame_id=-1;
       max_frame_id=-1;
       lastError = "RTVModel access error";
       UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
       return false;
    }

    RDK::UEPtr<RDK::UContainer> capture_container;
    capture_container = model->GetComponentL(capture_component_name);

    CaptureLibDescr &lib_descr = capture_tags[capture_class_name];
    if(lib_descr.LibName!=capture_class_name)
    {
       state=-1;
       frame_id=-1;
       lastError = "Capture class description access error";
       UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
       return false;
    }

    int *capture_state = capture_container->AccessPropertyData<int>(lib_descr.LibCaptureStateTagName.c_str());
    unsigned long long *fr_id = capture_container->AccessPropertyData<unsigned long long>(lib_descr.LibFrameIdStateName.c_str());

    if(capture_state)
        state=*capture_state;

    if(lib_descr.LibFrameIdStateName!="")
        frame_id=*fr_id;

    max_frame_id = task_src_frame_length;

    UpdateTaskStateInDb(task_id, TS_Calculation, static_cast<float>(frame_id)/task_src_frame_length);

    return true;
}

bool UProjectDeployerQt::GetCaptureStateNeuralInterface(int &state, unsigned long long& frame_id, unsigned long long& max_frame_id)
{
    RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>(0);

    if(!model)
    {
       state=-1;
       frame_id=-1;
       max_frame_id=-1;
       lastError = "RTVModel access error";
       UpdateTaskStateInDb(task_id, TS_Error, -1.0f);
       return false;
    }

    RDK::UEPtr<RDK::UContainer> predictor_container;
    predictor_container = model->GetComponentL(predictor_component_name);


    //Получить статус предсказания
    int *predict_status = predictor_container->AccessPropertyData<int>("PredictStatus");

    if(predict_status!=NULL)
    {
        std::cerr<<"Prediction status = "<<*predict_status<<"\n";

        if(*predict_status==-1)
        {
            frame_id=0;
            max_frame_id=100;
            state=0;
            std::cerr<<"Set state=0\n";
            return false;
        }
        if(*predict_status==0)
        {
            bool *start_prediction = predictor_container->AccessPropertyData<bool>("StartPredict");
            std::cerr<<"Start prediction = "<<*start_prediction<<"\n";
            *start_prediction = true;
        }
        if(*predict_status==1)
        {
            state = RDK_CAPTURE_ACTIVE;
            frame_id=50;
            max_frame_id=100;
            std::cerr<<"Set state=8\n";
            return true;
        }
        if(*predict_status==2)
        {
            state = 10000;
            frame_id=100;
            max_frame_id=100;
            std::cerr<<"Set state=10000\n";
            return true;
        }
    }
}

///Обрабатывает накопившийся с последнего вызова лог
/// возвращает false если были фатальные ошибки, иначе true
/// @error - текст ошибки из лога приложения
bool UProjectDeployerQt::ProcessCalculationLog(std::string &error)
{
    int error_level = AnalyzeLogForErrors(error);
    if(error_level>=0 && error_level<=1)
    {
        //добавлено 15.02.2020 - вылетаем, если ловим фаталочку, экспериментально
        exit(1);
    }
    if(error_level>=0)
    {
        lastError = error;
        return false;
    }
    error="No log errors";
    return true;
}


///Завершить расчет проекта, положить соответствующий результат запуска в базу данных
bool UProjectDeployerQt::FinishCalculation()
{
    //0. Зафиксировать время закрытия проекта
    //Вообще говоря, не предполагается что мы будем делать это 10 раз, но
    // потом возможно стоит как-то хранить время РЕАЛЬНОГО конца работы проекта
    processing_end_datetime = QDateTime::currentDateTime();
    //1. Закрыть проект
    Application->CloseProject();
    //2. Прочекать закрытие на ошибки
    std::string err="";
    int err_log_res = AnalyzeLogForErrors(err);
    deploymentState = DS_ProjectClosed;
    if(err_log_res>=0)
    {
        lastError = "Close project log error: "+err;
        UpdateTaskStateInDb(task_id, TS_Error, -1.0f, "", GetTimeStampInPSqlFormat(processing_end_datetime));
        return false;
    }
    return true;
}

///Отправить результаты расчета (содержимое папки Results) в соответствующую папку локального хранилища,
/// запустить процесс упаковки и отправки данных в удаленное хранилище
bool UProjectDeployerQt::UploadCalculationResults()
{
    //Если расчет по нормальному завершить не получилось, то эта функция не вызовется
    //не знаю, насколько это логично...

    //Проект закрыт
    //1. Проанализировать папку с результатами
    QString pdpath = GetTempProjectDeploymentPath().c_str();
    QString results_dir_path = pdpath+"/"+"Results";
    QDir results_dir(results_dir_path);
    QString results_path="";

    if(deploymentState>=DS_UploadFinished)
    {
        lastError = "Upload finished already";
        return false;
    }

    QString results_deploy_end_path="";

    if(results_dir.exists())
    {
        QStringList rd_contents = results_dir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::Dirs|QDir::Filter::Files);
        if(rd_contents.size()>0)
        {
            //Сформируем путь
            QString db_path = Application->GetDatabaseMainPath().c_str();
            QString relative_database_res_path = "{Database}/Results";//Потенциально задавать извне?
            QString main_database_res_path = relative_database_res_path;
            main_database_res_path.replace("{Database}", db_path);
            QDir main_database_res_dir(main_database_res_path);
            if(!main_database_res_dir.exists())
            {
                main_database_res_dir.mkpath(main_database_res_dir.path());
            }
            QString task_dir_path = main_database_res_path+"/"+QString::number(task_id);
            QDir task_dir(task_dir_path);
            if(!task_dir.exists())
            {
                task_dir.mkpath(task_dir.path());
            }
            QString run_start_time_str = GetTimeStampInPSqlFormat(processing_start_datetime);
            run_start_time_str.replace(":", "-");
            run_start_time_str.replace("+", " ");
            QString results_destination_dir_path = task_dir_path+"/"+run_start_time_str;
            QDir results_destination_dir(results_destination_dir_path);

            if(results_destination_dir.exists())
            {
                int index=1;
                while(results_destination_dir.exists())
                {
                    QString new_dest_dir_path = results_destination_dir_path+"_"+QString::number(index);
                    results_destination_dir.setPath(new_dest_dir_path);
                    index+=1;
                }
                results_destination_dir_path = results_destination_dir_path+"_"+QString::number(index);
            }

            results_destination_dir.mkpath(results_destination_dir.path());
            QString relative_results_destination_dir_path = results_destination_dir_path;
            relative_results_destination_dir_path.replace(db_path, "{Database}");
            //Задать параметры потока
            //Переключить состояние (?)
            //Стартовать поток
            if(projectResultsUploadingThread!=NULL)
            {
                delete projectResultsUploadingThread;
            }
            projectResultsUploadingThread = new UProjectResultsUploadingThread();
            projectResultsUploadingThread->SetDatabasePath(db_path);
            projectResultsUploadingThread->SetRemoteFtpPath(QString(ftp_remote_path.c_str()));
            projectResultsUploadingThread->SetProjectResultsDirPath(results_dir_path);
            projectResultsUploadingThread->SetStorageResultsDirPath(relative_results_destination_dir_path);
            projectResultsUploadingThread->SetRemoteStoragePath(QString(Application->GetStorageMountPoint().c_str()));
            projectResultsUploadingThread->start();

            results_deploy_end_path = relative_results_destination_dir_path;

            lastError = "Upload started";
        }
        else
        {
            lastError = "Upload skipped";
            deploymentState = DS_UploadFinished;
        }
    }
    else
    {
        lastError = "Upload skipped";
        deploymentState = DS_UploadFinished;
    }

    //TODO: Вообще говоря, отправка результатов может сломаться, но от этого место,
    //      куда результаты заливаются не изменится, поэтому запилим тему тут:

    QString processing_start_time_str = GetTimeStampInPSqlFormat(processing_start_datetime);
    QString processing_end_time_str = GetTimeStampInPSqlFormat(processing_end_datetime);
    //task_id
    //results_deploy_end_path
    QSqlQuery q(*db);
    QString query = "UPDATE vid_an.task_list SET task_start='"+processing_start_time_str+"',"+
                                                    "task_progress="+QString::number(1.0f)+","+
                                                    "task_end='"+processing_end_time_str+"',"+
                                                    "task_results='"+results_deploy_end_path+"',"+
                                                    "task_status='"+QString::number(static_cast<int>(TS_Finished))+"' "+
                                                    "WHERE task_id="+QString::number(task_id)+";";
    q.prepare(query);
    if(!q.exec())
    {
        std::cerr<<query.toUtf8().constData();
        lastError = (QString("Error add result record to database: ")+q.lastError().text()).toUtf8().constData();
        deploymentState = DS_Error;
        return false;
    }


    return true;
}

///Аккуратное закрытие солвера, команда которая по идее должна инициировать
/// процесс завершения работы, поочищать аккуратно выделенные ресурсы и т.п.
bool UProjectDeployerQt::CloseSolver()
{
    //TODO: For now its working only if everything is allright, that is wrong!
    UnRegisterSolverFromDatabase();
    return true;
}

///Получить состояние загрузки
int UProjectDeployerQt::GetUploadState()
{
    if(projectResultsUploadingThread!=NULL)
    {
        if(projectResultsUploadingThread->isRunning())
        {
            int upl_state = projectResultsUploadingThread->GetUploadState();
            this->lastError = std::string("Upload thread LE: ")+projectResultsUploadingThread->GetLastError().toUtf8().constData();
            return upl_state;
        }
        else if(projectResultsUploadingThread->isFinished())
        {
            return DS_UploadFinished;
        }
    }
    else
    {
        //Пока что по умолчанию предположим что деплоймент нормально отработает
        //по окончании работы тут тоже будет верное состояние
        return deploymentState;
    }
}

///Обновить статус задачи в базе данных
void UProjectDeployerQt::UpdateTaskStateInDb(int task_id, const DatabaseTaskStatus &status, float progress, const QString &start_time, const QString& end_time)
{
    QString preparation_request="";
    preparation_request+="UPDATE vid_an.task_list ";
    preparation_request+="SET task_status="+QString::number(static_cast<int>(status));
    if(progress>=0)
    {
        preparation_request+=",task_progress="+QString::number(progress);
    }
    if(start_time!="")
    {
        preparation_request+=",task_start='"+start_time+"'";
    }
    if(end_time!="")
    {
        preparation_request+=",task_end='"+end_time+"'";
    }
    preparation_request+=" WHERE task_id="+QString::number(task_id)+";";

    QSqlQuery q(*db);
    q.prepare(preparation_request);
    if(!q.exec())
    {
        lastError = QString("Database update error: "+q.lastError().text()).toUtf8().constData();
        deploymentState = DS_Error;
    }
}

void UProjectDeployerQt::RegisterSolverToDatabase()
{
    QString ip_addr = GetApplication()->GetServerControl()->GetServerTransport()->GetServerBindingInterfaceAddress().c_str();
    int port = GetApplication()->GetServerControl()->GetServerTransport()->GetServerBindingPort();
    if(ip_addr!="" && port>0)
    {
        QSqlQuery q(*db);

        q.prepare("SELECT solver_id FROM vid_an.cloud_solvers WHERE solver_address='"+
                                   ip_addr+"' AND solver_port="+QString::number(port)+";");
        if(q.exec())
        {
            q.first();
            if(q.isValid())
            {
                qDebug()<<"Record already exists, smth went wrong earlier";
                return;
            }
        }

        q.finish();
        q.clear();
        q.prepare("INSERT INTO vid_an.cloud_solvers(solver_address,solver_port) VALUES ('"+
                             ip_addr+"',"+QString::number(port)+");");
        if(!q.exec())
        {
            qDebug()<<"Error add record to Db: "<<q.lastError().text();
        }
    }
}

void UProjectDeployerQt::UnRegisterSolverFromDatabase()
{
    QString ip_addr = GetApplication()->GetServerControl()->GetServerTransport()->GetServerBindingInterfaceAddress().c_str();
    int port = GetApplication()->GetServerControl()->GetServerTransport()->GetServerBindingPort();

    QSqlQuery q(*db);

    q.prepare("DELETE FROM vid_an.cloud_solvers WHERE solver_address='"+
                                   ip_addr+"' AND solver_port="+QString::number(port)+";");

    if(!q.exec())
    {
        qDebug()<<"Record deletion error: "<<q.lastError().text();
    }
}


/// Задача для запуска без сети
void UProjectDeployerQt::SetStandaloneTask(int task)
{
    serverStandaloneTask = task;
}

int UProjectDeployerQt::GetStandaloneTask()
{
    return serverStandaloneTask;
}



UProjectRunThread::UProjectRunThread(UProjectDeployer* deployer):
    last_frame_count(0)
{
    projectRunState = ProjectRunState::PS_Initialization;
    Deployer = deployer;
}

UProjectRunThread::~UProjectRunThread()
{

}

std::string UProjectRunThread::GetLastError()
{
    return error_string;
}

void UProjectRunThread::run()
{
    bool stop_thread = false;
    while(!stop_thread)
    {
        switch(projectRunState)
        {
            case ProjectRunState::PS_Initialization:
                ProjectStateInitialization();
                break;

            case ProjectRunState::PS_Deployment:
            case ProjectRunState::PS_ProjectPreparation:
            case ProjectRunState::PS_ProjectOpening:
                ProjectStateDeployment();
                break;


            case ProjectRunState::PS_Calculation:
                ProjectStateCalculation();
                break;

            case ProjectRunState::PS_Finalization:
                ProjectStateFinalization();
                break;

            case ProjectRunState::PS_Termination:
                stop_thread = true;
                break;

            default:
                std::cout << "Current project run state is undefined";
                break;
        }
    }

    std::cout << "Project Run Thread stopped" << std::endl;
}

void UProjectRunThread::ProjectStateInitialization()
{
    if(Deployer->StartProjectRun(Deployer->GetStandaloneTask()))
    {
        std::cout << Deployer->GetLastError() << std::endl;
        projectRunState = ProjectRunState::PS_Termination;
        return;
    }
    else
    {
        std::cout << "Set state: PS_Deployment" << std::endl;
        projectRunState = ProjectRunState::PS_Deployment;
    }
}

void UProjectRunThread::ProjectStateDeployment()
{
    //SetConnectionState("connected, ping OK");
    //int state=0;

    std::string last_error = "";
    int state = Deployer->GetDeploymentState();
    int cap = Deployer->GetStageCap();
    int progress =Deployer->GetStageProgress();
    last_error = Deployer->GetLastError();
    if(!last_error.empty())
    {
        if(state<0 || cap<0 || progress<0)
        {
            //WriteLog("Rpc_GetDeploymentState returned wrong string");
        }
        DeploymentState ds = static_cast<DeploymentState>(state);

        //TODO: Проверка состояния на попадание в правильный диапазон

        std::cout << "Deployment state: "+ ParseDeploymentState(ds) << std::endl;

        if(ds==DS_DeployFinished)
        {
            PrepProject();
        }
        else if(ds==DS_ProjectPrepared)
        {
            OpenProject();
        }
        else if(ds==DS_ProjectOpened)
        {
            RunProject();
        }

        if(ds==DS_Error)
        {
            std::cout << "last_error = " << last_error;
            projectRunState = ProjectRunState::PS_Termination;
        }
        //ui->labelLastError->setText("LE: "+QString(last_error));
    }
}

std::string UProjectRunThread::ParseDeploymentState(DeploymentState state)
{
    //Тут место для анализа состояния
    std::string r="";
    switch(state)
    {
    case DeploymentState::DS_NULL:
        r = "deployment conponent not exists";
        break;
    case DeploymentState::DS_Error:
        r = "deployment error occured";
        break;
    case DeploymentState::DS_Unknown:
        r = "deployment state unknown";
        break;
    case DeploymentState::DS_DownloadTemplate:
        r = "downloading config template";
        break;
    case DeploymentState::DS_UnpackTemplate:
        r = "unpacking config template";
        break;
    case DeploymentState::DS_DownloadWeights:
        r = "downloading weights";
        break;
    case DeploymentState::DS_UnpackWeights:
        r = "unpacking weights";
        break;
    case DeploymentState::DS_DownloadScripts:
        r = "downloading script files";
        break;
    case DeploymentState::DS_UnpackScripts:
        r = "unpacking script files";
        break;
    case DeploymentState::DS_DownloadData:
        r = "downloading video/image source";
        break;
    case DeploymentState::DS_UnpackData:
        r = "unpacking video/image source";
        break;
    case DeploymentState::DS_DeployFinished:
        r = "resources deployment finished";
        break;
    case DeploymentState::DS_CopyProject:
        r = "copying project to temporary location";
        break;
    case DeploymentState::DS_PrepareProject:
        r = "solver preparing project";
        break;
    case DeploymentState::DS_ProjectPrepared:
        r = "project preparation finished";
        break;
    case DeploymentState::DS_OpenProject:
        r = "solver opening prepared project";
        break;
    case DeploymentState::DS_ProjectOpened:
        r = "project opened successfully";
        break;
    case DeploymentState::DS_ProjectClosed:
        r = "project closed";
        break;

    case DeploymentState::DS_CopyResults:
        r = "copying results";
        break;

    case DeploymentState::DS_PackResults:
        r = "packing results";
        break;

    case DeploymentState::DS_UploadResults:
        r = "uploading results";
        break;

    case DeploymentState::DS_UploadFinished:
        r = "upload finished";
        break;

    default:
        r = "ERROR - unknown value";
        break;
    }

    return r;
}

void UProjectRunThread::ProjectStateCalculation()
{
    std::cout << "Calculation in progress: " << std::endl;
    int calculation_state=-1;
    int capture_state=-1;
    static unsigned long long capture_frame_id=0;
    static unsigned long long capture_max_frame_id=0;
    unsigned long long capture_frid=0;
    unsigned long long capture_maxfrid=0;
    std::string message="";

    int calc_state = Deployer->GetCalculationState();
    if(calc_state>=0)
    {
        calculation_state = calc_state;
        //Выгребаем логи, проверяем нет ли ошибок там:
        std::string log_err="";
        bool process_log_res = Deployer->ProcessCalculationLog(log_err);
        if(!process_log_res)
        {
            message = std::string("Log error: ") + log_err;
            calculation_state = 3;
        }
        else
        {
            if(calc_state==2)
            {
                message = "Calculation started but not active (2).";
            }
            else if(calc_state==1 || calc_state==0)
            {
                int cap_state_out = -1;
                bool res_capstate = Deployer->GetCaptureState(cap_state_out, capture_frame_id, capture_max_frame_id);
                if(!res_capstate)
                {
                    std::string err="";
                    err = Deployer->GetLastError();
                    message = "Get capture state error: "+err;
                }
                else
                {
                    capture_state = cap_state_out;
                    capture_frid = capture_frame_id;
                    capture_maxfrid = capture_max_frame_id;
                    message = "Calculation is active";
                }
            }
            else
            {
                message = "Error: Wrong calculation state number";
            }
        }

    }
    else
    {
        std::string err="";
        err = Deployer->GetLastError();
        message = "GetCalculationState() error: "+err;
    }

        //if(message!=NULL)
            //std::cout << "Capture state: "+ message << std::endl;

        /// Состояние тредов расчета
        /// 0 - запущен
        /// 1 - расчет остановлен
        /// 2 - расчет запущен, но не выполняется
        /// 3 - ошибка найдена в логах
        std::string cstate_str = "";
        switch (calculation_state) {
            case -1:
                cstate_str = "Error get calc_state (-1)";
            break;
            case 0:
                cstate_str = "Calc thread active (0)";
            break;
            case 1:
                cstate_str = "Calc thread paused (1)";
            break;
            case 2:
                cstate_str = "Calc thread started but inactive (2)";
            break;
            case 3:
                std::cout << "Fatal error occured during calculation: "+std::string(message);
                projectRunState = ProjectRunState::PS_Termination;
            break;
            default:
                cstate_str = "Calc thread result default ("+std::to_string(calculation_state)+")";
            break;
        }
        //Состояния
        #define RDK_CAPTURE_EMPTY 0
        #define RDK_CAPTURE_CREATED 64
        #define RDK_CAPTURE_INITIALIZATION 1
        #define RDK_CAPTURE_CONNECTED 2
        #define RDK_CAPTURE_PAUSED 4
        #define RDK_CAPTURE_ACTIVE 8
        #define RDK_CAPTURE_DISCONNECTED 16
        #define RDK_CAPTURE_RECONNECT 32
        std::string capstate_str="Cap state: ";

        if(capture_state&RDK_CAPTURE_EMPTY)
        {
            capstate_str+="(RDK_CAPTURE_EMPTY)";
        }
        if(capture_state&RDK_CAPTURE_CREATED)
        {
            capstate_str+="(RDK_CAPTURE_CREATED)";
        }
        if(capture_state&RDK_CAPTURE_INITIALIZATION)
        {
            capstate_str+="(RDK_CAPTURE_INITIALIZATION)";
        }
        if(capture_state&RDK_CAPTURE_CONNECTED)
        {
            capstate_str+="(RDK_CAPTURE_CONNECTED)";
        }
        if(capture_state&RDK_CAPTURE_PAUSED)
        {
            capstate_str+="(RDK_CAPTURE_PAUSED)";
        }
        if(capture_state&RDK_CAPTURE_ACTIVE)
        {
            capstate_str+="(RDK_CAPTURE_ACTIVE)";
        }
        if(capture_state&RDK_CAPTURE_DISCONNECTED)
        {
            capstate_str+="(RDK_CAPTURE_DISCONNECTED)";
        }
        if(capture_state&RDK_CAPTURE_RECONNECT)
        {
            capstate_str+="(RDK_CAPTURE_RECONNECT)";
        }

        //std::cout <<  cstate_str+"\n"+capstate_str;



        if(capture_frame_id>0 && capture_max_frame_id>0)
        {
            qDebug()<<"capture_frame_id="<<capture_frame_id<<" capture_max_frame_id="<<capture_max_frame_id;

            //std::cout << capture_frame_id;
            //Мы докатились до конца, переключаемся на финализацию
            if(capture_frame_id>=capture_max_frame_id || last_frame_count>capture_frame_id)
            {
                FinishProject();
            }

            last_frame_count = capture_frame_id;
        }

        //TODO: Обработка фатальных ошибок на этапе расчета


}

void UProjectRunThread::ProjectStateFinalization()
{
    projectRunState = ProjectRunState::PS_Finalization;
}

void UProjectRunThread::ProjectStateTermination()
{

}

void UProjectRunThread::PrepProject()
{
    std::string response;
    if(Deployer->PrepareProject(response))
    {
        std::cout << Deployer->GetLastError() << std::endl;
        projectRunState = ProjectRunState::PS_Termination;
        return;
    }
    else
    {
        std::cout << "Set state: PS_ProjectPreparation" << std::endl;
        projectRunState = ProjectRunState::PS_ProjectPreparation;
    }
}

void UProjectRunThread::OpenProject()
{
    std::string response;
    if(Deployer->OpenPreparedProject(response))
    {
        std::cout << Deployer->GetLastError() << std::endl;
        projectRunState = ProjectRunState::PS_Termination;
        return;
    }
    else
    {
        std::cout << "Set state: PS_ProjectOpening" << std::endl;
        projectRunState = ProjectRunState::PS_ProjectOpening;
    }
}

void UProjectRunThread::RunProject()
{
    if(Deployer->RunPreparedProject())
    {
        std::cout << Deployer->GetLastError() << std::endl;
        projectRunState = ProjectRunState::PS_Termination;
        return;
    }
    else
    {
        std::cout << "Set state: PS_Calculation" << std::endl;
        projectRunState = ProjectRunState::PS_Calculation;
    }
}

void UProjectRunThread::FinishProject()
{   
    if(Deployer->FinishCalculation())
    {
        std::string error = Deployer->GetLastError();
        std::cout << Deployer->GetLastError() << std::endl;
        projectRunState = ProjectRunState::PS_Termination;
        return;
    }
    else
    {
        std::cout << "Set state: PS_Finalization" << std::endl;
        projectRunState = ProjectRunState::PS_Finalization;
    }
}


}//namespace RDK
