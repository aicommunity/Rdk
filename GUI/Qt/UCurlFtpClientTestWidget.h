#ifndef UCurlFtpClientTestWidget_H
#define UCurlFtpClientTestWidget_H

#include "UVisualControllerWidget.h"
#ifdef __unix__
#include <curl/curl.h>
#endif

#include <QProcess>
namespace Ui {
  class UCurlFtpClientTestWidget;
}

/* NOTE: if you want this example to work on Windows with libcurl as a
   DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
   Failing to do so will give you a crash since a DLL may not use the
   variable's memory when passed in to it from an app like this. */
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
//  curl_off_t nread;
  /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */
  size_t retcode = fread(ptr, size, nmemb, (FILE*)stream);

//  nread = (curl_off_t)retcode;

  /*fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
          " bytes from file\n", nread);*/
  return retcode;
}

class UCurlFtpClientTestWidget : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit UCurlFtpClientTestWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
  ~UCurlFtpClientTestWidget();

  /// запись файла настроек
  virtual void ASaveParameters();
  /// считывание файла настроек
  virtual void ALoadParameters();
  /// обновление интерфейса
  virtual void AUpdateInterface();

bool DownloadZip(const QString &remote_url, const QString& dst_zip_file);
bool UnpackZipFolder(const QString &local_zip_folder, const QString& local_dst_folder);
bool UnpackZipFile(const QString &local_zip_folder, const QString& local_dst_folder);

private slots:
    void onPushButtonBrowseClick();
    void onPushButtonBrowseReceiveFilePathClick();
    void onPushButtonSendFileClick();
    void onPushButtonReceiveFileClick();

    void pushButtonModelExampleClicked();

private:
  Ui::UCurlFtpClientTestWidget *ui;
  CURL *curl;
  CURLcode res;

  QProcess zip_process;

};

#endif //UCurlFtpClientTestWidget_H
