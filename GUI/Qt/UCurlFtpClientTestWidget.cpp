#ifndef UCurlFtpClientTestWidget_CPP
#define UCurlFtpClientTestWidget_CPP

#include "UCurlFtpClientTestWidget.h"
#include "ui_UCurlFtpClientTestWidget.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

#include <sys/types.h>
#include <sys/stat.h>


struct FtpFile {
  const char *filename;
  FILE *stream;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
  struct FtpFile *out = (struct FtpFile *)stream;
  if(!out->stream) {
    //open file for writing
    out->stream = fopen(out->filename, "wb");
    if(!out->stream)
      return -1; // failure, can't open file to write
  }
  return fwrite(buffer, size, nmemb, out->stream);
}


static
size_t write_response(void *ptr, size_t size, size_t nmemb, void *data)
{
  FILE *writehere = (FILE *)data;
  char* str = static_cast<char*>(ptr);
  //std::string s(str);
  qDebug()<<str;
  return nmemb;
}

UCurlFtpClientTestWidget::UCurlFtpClientTestWidget(QWidget *parent, RDK::UApplication *app) :
  UVisualControllerWidget(parent, app),
  ui(new Ui::UCurlFtpClientTestWidget)
{
  ui->setupUi(this);
  UpdateInterval = 0;
  setAccessibleName("UCurlFtpClientTestWidget");

  //Инициализация curl, которую надо выполнить только один раз
  curl_global_init(CURL_GLOBAL_DEFAULT);

  connect(ui->pushButtonBrowse, SIGNAL(clicked()), this, SLOT(onPushButtonBrowseClick()));
  connect(ui->pushButtonSendFile, SIGNAL(clicked()), this, SLOT(onPushButtonSendFileClick()));
  connect(ui->pushButtonReceiveFile, SIGNAL(clicked()), this, SLOT(onPushButtonReceiveFileClick()));
  connect(ui->pushButtonBrowseReceivePath, SIGNAL(clicked()), this, SLOT(onPushButtonBrowseReceiveFilePathClick()));

  connect(ui->pushButtonModelExample, SIGNAL(clicked()), this, SLOT(pushButtonModelExampleClicked()));
}

UCurlFtpClientTestWidget::~UCurlFtpClientTestWidget()
{
  curl_global_cleanup();
  delete ui;
}

void UCurlFtpClientTestWidget::onPushButtonBrowseClick()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose file to upload"), QDir::currentPath());
    ui->lineEditSendFilePath->setText(fileName);
}

void UCurlFtpClientTestWidget::onPushButtonBrowseReceiveFilePathClick()
{
    QString res_path=QFileDialog::getExistingDirectory(this, tr("Choose directory to download"), QDir::currentPath(), QFileDialog::ShowDirsOnly);
    ui->lineEditReceiveFilePath->setText(res_path+"/");
}

void UCurlFtpClientTestWidget::onPushButtonSendFileClick()
{
    if(ui->lineEditSendFilePath->text()=="")
    {
        QMessageBox::information(this, "error", "Send file path is empty");
        return;
    }
    QFileInfo fi(ui->lineEditSendFilePath->text());
    if(!fi.exists())
    {
        QMessageBox::information(this, "error", "Send file not exists, check path and click again");
        return;
    }
    if(ui->lineEditRemotePath->text()=="")
    {
        QMessageBox::information(this, "error", "Remote file path is empty");
        return;
    }

    FILE *hd_src;
    struct stat file_info;
    struct curl_slist *headerlist = NULL;

    //Если понадобится - переименовать на удаленной машине.
    //static const char* upload_as = ("RNFR "+ui->lineEditRemotePath->text()).toUtf8().constData();
    //static const char* rename_to = ("RNTO "+ui->lineEditRemotePath->text()).toUtf8().constData();

    curl_off_t fsize;
    fsize = fi.size();

    /* get a FILE * of the same file */
    hd_src = fopen(ui->lineEditSendFilePath->text().toUtf8().constData(), "rb");

    curl = curl_easy_init();

    if(curl) {
      /* build a list of commands to pass to libcurl */
      //headerlist = curl_slist_append(headerlist, upload_as);
      //headerlist = curl_slist_append(headerlist, rename_to);

      /* we want to use our own read function */
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

      /* enable uploading */
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

      /* specify target */
      curl_easy_setopt(curl, CURLOPT_URL, ui->lineEditRemotePath->text().toUtf8().constData());

      /* pass in that last of FTP commands to run after the transfer */
      //curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

      /* now specify which file to upload */
      curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

      /* Set the size of the file to upload (optional).  If you give a *_LARGE
         option you MUST make sure that the type of the passed-in argument is a
         curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
         make sure that to pass in a type 'long' argument. */
      curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                       (curl_off_t)fsize);

      /* Now run off and do what you've been told! */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

      /* clean up the FTP commands list */
      curl_slist_free_all(headerlist);

      /* always cleanup */
      curl_easy_cleanup(curl);
    }
    fclose(hd_src); /* close the local file */
}

void UCurlFtpClientTestWidget::onPushButtonReceiveFileClick()
{
 if(ui->lineEditRemotePath->text()=="")
 {
     QMessageBox::information(this, "error", "Remote file path is empty");
     return;
 }
 if(ui->lineEditReceiveFilePath->text()=="")
 {
  QMessageBox::information(this, "error", "Path to receive file is empty");
  return;
 }

 QString save_filename = "";
 bool rename = false;

 QFileInfo fi(ui->lineEditReceiveFilePath->text());
 if(!fi.isDir())
 {
  qDebug()<<"File name is not directory, so downloaded file will be renamed";
  rename=true;
  if(fi.exists())
  {
   QMessageBox::information(this, "error", "Change file name, file already exists");
   return;
  }
 }
 else
 {
  if(!fi.exists())
  {
   QMessageBox::information(this, "error", "Directory to save file is not exists, create it before start");
   return;
  }
  QStringList ls = ui->lineEditRemotePath->text().split("/");
  if(ls.size()<2)
  {
      QMessageBox::information(this, "error", "Wrong remote file path");
      return;
  }
  QString fn = ls[ls.size()-1];
  QString filename = ui->lineEditReceiveFilePath->text()+"/"+fn;
  QFileInfo fi2(filename);
  if(fi2.exists())
  {
   QMessageBox::information(this, "error", "Change save file name, current already exists");
   return;
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

 struct FtpFile ftp_file;
 ftp_file.stream=NULL;
 ftp_file.filename = save_filename.toUtf8().constData();

 curl = curl_easy_init();
 if(curl)
 {
     // Положить ссылку на удаленный объект
     curl_easy_setopt(curl, CURLOPT_URL, ui->lineEditRemotePath->text().toUtf8().constData());
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
       fprintf(stderr, "curl told us %d\n", res);
     }
 }
 if(ftp_file.stream!=NULL)
 {
     fclose(ftp_file.stream);
 }
 //Тут еще переименование, копирование, обработка и тп??

}

/// обновление интерфейса
void UCurlFtpClientTestWidget::AUpdateInterface()
{

}

void UCurlFtpClientTestWidget::ASaveParameters()
{
  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt", QSettings::IniFormat);
  settings.beginGroup(accessibleName());
  settings.setValue("geometry", saveGeometry());
  settings.endGroup();
}

void UCurlFtpClientTestWidget::ALoadParameters()
{
  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt", QSettings::IniFormat);
  settings.beginGroup(accessibleName());
  restoreGeometry(settings.value("geometry").toByteArray());
  settings.endGroup();
}

bool UCurlFtpClientTestWidget::DownloadZip(const QString &remote_url, const QString& dst_zip_file)
{
    if(remote_url=="")
    {
        //8QMessageBox::information(this, "error", "Remote file path is empty");

        std::cout<<"Remote file path is empty";
        return false;
    }
    if(dst_zip_file=="")
    {
        //QMessageBox::information(this, "error", "Path to receive file is empty");
        std::cout<<"Path to receive file is empty";
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
      return false;
     }

     QStringList ls = dst_zip_file.split("/");
     if(ls.size()<2)
     {
         std::cout<<"Wrong remote file path '"<<dst_zip_file.toUtf8().constData();
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

    struct FtpFile ftp_file;
    ftp_file.stream=NULL;
    ftp_file.filename = save_filename.toUtf8().constData();

    curl = curl_easy_init();

    if(curl)
    {
        // Положить ссылку на удаленный объект
        curl_easy_setopt(curl, CURLOPT_URL, remote_url.toUtf8().constData());

        // Добавляем функцию для отображения процесса
        //curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_response);

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
        }
    }
    if(ftp_file.stream!=NULL)
    {
        fclose(ftp_file.stream);
    }

    //Тут еще переименование, копирование, обработка и тп??
   return true;
}

bool UCurlFtpClientTestWidget::UnpackZipFolder(const QString &local_zip_folder, const QString& local_dst_folder)
{
    QDir dst_dir(local_dst_folder);
    if(!dst_dir.exists())
        dst_dir.mkpath(dst_dir.path());

    QString cmd = "unzip -o "+local_zip_folder+" -d " + local_dst_folder;

    zip_process.start(cmd);
    while(!zip_process.waitForFinished(2))
    {
        QCoreApplication::processEvents();
    }
    return true;
}

bool UCurlFtpClientTestWidget::UnpackZipFile(const QString &local_zip_folder, const QString& local_dst_folder)
{
    //TODO: Убедиться, что файл залетает куда надо после распаковки
    QString cmd = "unzip -o "+local_zip_folder+" -d " + local_dst_folder;

    zip_process.start(cmd);
    while(!zip_process.waitForFinished(2))
    {
        QCoreApplication::processEvents();
    }
    return true;
}


void UCurlFtpClientTestWidget::pushButtonModelExampleClicked()
{
    QString remote_ftp_zip_path = "ftp://192.168.102.171/database/CloudFtpDatabase/Configs/Detection/SqueezeDetCubes.zip";
    QString zip_download_path = "/home/ivan/RTV-VideoAnalytics/ExpDatabase/Temp/Configs/Detection/SqueezeDetCubes.zip";
    QString unpack_dir = "/home/ivan/RTV-VideoAnalytics/ExpDatabase/unpack_test";

    QDir du_d(unpack_dir);

    if(du_d.exists())
        du_d.rmpath(du_d.path());

    if(!du_d.exists())
        du_d.mkpath(du_d.path());

    //Теперь шаг 1 - загрузить
    //Предварительно - только если файла еще нет
    //if(!zd_fi.exists())
    {
        if(!DownloadZip(remote_ftp_zip_path, zip_download_path))
        {
            qDebug()<<"Download ERROR";
            //SetDeploymentState(DeploymentState::DS_Error);
            return;
        }
    }


    //Шаг 2 = распаковать
    //Поехали распаковывать
    //SetDeploymentState(DeploymentState::DS_UnpackData);
    if(!UnpackZipFolder(zip_download_path, du_d.path()))
    {
        qDebug()<<"Unpack ERROR";
        //SetDeploymentState(DeploymentState::DS_Error);
        return;
    }
    qDebug()<<"Unpack SUCCESS";
}

#endif //UCurlFtpClientTestWidget_CPP
