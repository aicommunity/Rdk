#include "UProjectDeployerQt.h"
#include "../UApplication.h"

namespace RDK{


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



void UProjectDeployProcessingThread::run()
{
    DeployTemplate();
    if(!VerifyTemplate())
    {


        //Обработка ошибок, сигнализация о проблемах и выход из потока
        //Запись чего-то в лог
        //Изначально самая примитивная верификация, потом усложним
    }

    DeployScript();
    if(!VerifyScript())
    {
        //Обработка ошибок, сигнализация о проблемах и выход из потока
        //Запись чего-то в лог
        //Изначально самая примитивная верификация, потом усложним
    }

    DeployWeights();
    if(!VerifyWeights())
    {
        //Обработка ошибок, сигнализация о проблемах и выход из потока
        //Запись чего-то в лог
        //Изначально самая примитивная верификация, потом усложним
    }

    DeployData();
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
    return lastError;
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
          lastError = ss_curl.str();
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

}

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProjectDeployerQt::UProjectDeployerQt(void):
    db(NULL),
    deployProcessingThread(NULL),
    deploymentState(DS_NULL)
{
    //Инициализация curl, которую надо выполнить только один раз
    //Этот класс же тоочно не создается дважды?
    curl_global_init(CURL_GLOBAL_DEFAULT);
}
UProjectDeployerQt::~UProjectDeployerQt(void)
{
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

int UProjectDeployerQt::StartProjectDeployment(int task_id)
{
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
 q.prepare("SELECT project_name, project_file, project_model, project_params, project_interface FROM vid_an.projects WHERE project_id="+QString::number(task_template_id)+";");
 q.exec();
 q.first();
 if(!q.isValid())
 {
    lastError = "Project query by project id invalid";
     return 4;
 }

 task_template_name = q.value(0).toString();
 task_template_path = q.value(1).toString();
 QString task_template_model = q.value(2).toString();
 QString task_template_params = q.value(3).toString();
 QString task_template_interface = q.value(4).toString();

 q.finish();
 q.clear();

 QString database_path = Application->GetDatabaseMainPath().c_str();

 QString abs_template_file = task_template_path;
 abs_template_file.replace("{Database}", database_path);
 QString abs_template_model = task_template_model;
 abs_template_model.replace("{Database}", database_path);
 QString abs_template_params = task_template_params;
 abs_template_params.replace("{Database}", database_path);
 QString abs_template_interface = task_template_interface;
 abs_template_interface.replace("{Database}", database_path);

 QFile t_file(abs_template_file), t_model(abs_template_model), t_params(abs_template_params), t_interf(abs_template_interface);

 if(!t_file.exists() || !t_model.exists() || !t_params.exists() || !t_interf.exists())
 {
     task_template_download_required = true;
 }
 else
 {
     task_template_download_required = false;
 }

 QFileInfo qfi(abs_template_file);

 /*
 QString task_template_destination_directory = qfi.filePath();

 QString ftp_main_path = "";//Application->MainFtpPath().c_str();
 QString ftp_template_zip_path = QString(task_template_destination_directory+".zip").replace(database_path, ftp_main_path);*/


 q.prepare("SELECT weight_path, weight_confpath, weight_numcls, weight_in_W, weight_in_H, weight_out_format, weight_script FROM vid_an.weights WHERE weight_id="+QString::number(task_weights_id)+";");
 q.exec();
 q.first();

 if(!q.isValid())
 {
    lastError = "Weights query by weights id invalid";
     return 5;
 }

 task_weights_path = q.value(0).toString();
 QString weights_conf_path = q.value(1).toString();
 int weights_num_cls = q.value(2).toInt();
 int weights_in_W = q.value(3).toInt();
 int weights_in_H = q.value(4).toInt();
 QString weights_out_format = q.value(5).toString();
 task_script_id = q.value(6).toInt();

 q.finish();
 q.clear();

 QString abs_weights_path = task_weights_path;
 abs_weights_path.replace("{Database}", database_path);
 QString abs_weights_conf_path = weights_conf_path;
 abs_weights_conf_path.replace("{Database}", database_path);

 QFileInfo fi_weights_path(abs_weights_path), fi_weights_conf_path(abs_weights_conf_path);
 if(!fi_weights_path.exists() || !fi_weights_conf_path.exists())
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

 q.prepare("SELECT script_path FROM vid_an.scripts WHERE script_id="+QString::number(task_script_id)+";");
 q.exec();
 q.first();
 if(!q.isValid())
 {
    lastError = "Script query by script id invalid";
     return 6;
 }
 task_script_path = q.value(0).toString();
 QString abs_script_path = task_script_path;
 abs_script_path.replace("{Database}", database_path);
 QFileInfo fi_script_path(abs_script_path);

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

 if(task_src_type==0) //video file
 {
     q.prepare("SELECT video_relpath FROM vid_an.videos WHERE video_id="+QString::number(task_src_id)+";");
     q.exec();
     q.first();
     if(!q.isValid())
     {
        lastError = "Video query by video id invalid";
         return 7;
     }
     task_src_path = q.value(0).toString();
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
     int frame_length = q.value(1).toInt();
     task_src_fullpath = task_src_path;
     task_src_fullpath.replace("{Database}", database_path);
     //QFileInfo qfi_src_file(abs_src_path);
     QDir seq_dir(task_src_fullpath);
     if(seq_dir.exists())
     {
         QStringList ldr = seq_dir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::Files);
         if(ldr.size()>=frame_length)
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
     if(ftp_remote_path!="")
     {
        deployProcessingThread->SetFtpRemoteBasePath(QString(ftp_remote_path.c_str()));
     }
     else
     {
        qDebug("Empty ftp_remote_path on deploy processing stage");
     }

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

 lastError = "";
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
    std::string le = "";
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
    lastError="";
    if(this->deploymentState!=DS_DeployFinished)
    {
        response = "Project deployment is not opened, can't start preparations";
        lastError=response;
        return 1;
    }
    this->deploymentState = DS_CopyProject;
    if(CopyProjectToTempFolder()!=0)
    {
        this->deploymentState=DS_Error;
        response = "error during copying";
        lastError=response;
        return 1;
    }
    this->deploymentState=DS_PrepareProject;
    if(OpenProjectMockMode()!=0)
    {
        this->deploymentState=DS_Error;
        response = "error during opening in mock mode";
        lastError=response;
        return 1;
    }
    if(SetupProjectMockParameters()!=0)
    {
        this->deploymentState=DS_Error;
        response = "error during mock parameters setup";
        lastError=response;
        return 1;
    }
    if(CloseMockProject()!=0)
    {
        this->deploymentState=DS_Error;
        response = "error during mocked project closing";
        lastError=response;
        return 1;
    }
    this->deploymentState=DS_ProjectPrepared;
    response = "project prepared successfully";
    lastError=response;
    return 0;
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
    QString project_path = pdpath+"/project_full.ini";
    if(!Application->OpenProject(project_path.toUtf8().constData()))
    {
        lastError="UApplication OpenProject normally error";
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
    if(pdpath=="")
    {
        lastError="project temporary deployment path empty - cannot copy";
        return 1;
    }
    QString abs_ttp = task_template_path;
    QString db_p = Application->GetDatabaseMainPath().c_str();
    abs_ttp.replace("{Database}", db_p);

    QFileInfo fi(abs_ttp);
    QDir src_dir(fi.absoluteDir());
    QStringList files_dirs = src_dir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::Files);
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
    QString project_path = pdpath+"/project_full.ini";
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

    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock(0);

    if(!model) return 1;

    //Настроить видеоисточник

    RDK::UEPtr<RDK::UContainer> video_cont;
    RDK::UEPtr<RDK::UContainer> imseq_cont;
    std::vector<std::string> vid_names;
    vid_names = model->GetComponentsNameByClassName("TCaptureOpenCV", vid_names);
    std::vector<std::string> imseq_names;
    imseq_names = model->GetComponentsNameByClassName("TCaptureImageSequence", imseq_names);

    if(task_src_type==0)//TODO: Проверить - видео
    {
        if(vid_names.empty())
        {
            //Видеозаписей - нет
            lastError="Model does not contain video data source, fix project file/task parameters";
            return 1;
        }
        video_cont = model->GetComponentL(vid_names[0]);
        bool *video_Activity = video_cont->AccessPropertyData<bool>("Activity");
        bool *video_EnableCapture = video_cont->AccessPropertyData<bool>("EnableCapture");
        std::string *video_CameraPath = video_cont->AccessPropertyData<std::string>("CameraPath");
        *video_Activity = true;
        *video_EnableCapture = true;
        *video_CameraPath = task_src_fullpath.toUtf8().constData();

        if(!imseq_names.empty())
        {
            imseq_cont = model->GetComponentL(imseq_names[0]);
            bool *act = imseq_cont->AccessPropertyData<bool>("Activity");
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
    }
    else
    {
        //Тип источника странный, не поддерживается системой
        lastError="Video source type has undefined value";
        return 1;
    }

    /*
    if(cont)
    {
    std::map<std::string, RTV::RTVAlarmPlan> *schedules =
      cont->AccessPropertyData<std::map<std::string, RTV::RTVAlarmPlan> >(schedulesPropertyName.toLocal8Bit().constData());

    int stepCounter = 0;
    for(std::map<std::string, RTV::RTVAlarmPlan>::iterator i = schedules->begin();
        i != schedules->end(); ++i, ++stepCounter)
    {
      ui->tableWidgetSchedule->insertRow(stepCounter);
      ui->tableWidgetSchedule->setItem(stepCounter, 0, new QTableWidgetItem(QString::fromLocal8Bit(i->first.c_str())));
      ui->tableWidgetSchedule->setItem(stepCounter, 1, new QTableWidgetItem(QString::number(i->second.StartTime)));
      ui->tableWidgetSchedule->setItem(stepCounter, 2, new QTableWidgetItem(QString::number(i->second.StopTime)));

      for(int checkBoxIterator = 0; checkBoxIterator < 8; ++checkBoxIterator)
      {
        QCheckBox *checkBox = new QCheckBox();
        if(i->second.DaysOfWeek & 1 << checkBoxIterator)
          checkBox->setChecked(true);
        ui->tableWidgetSchedule->setCellWidget(stepCounter, checkBoxIterator + 3, checkBox);
      }
    }
    }
    */

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






/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
//int UProjectDeployerQt::ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes)
//{

//}


}//namespace RDK
