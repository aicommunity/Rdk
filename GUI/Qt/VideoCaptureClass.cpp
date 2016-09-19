#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QReadLocker>
#include <QWriteLocker>

#include "VideoCaptureClass.h"


VideoCaptureClass::VideoCaptureClass(void)
{
 //ReadSettings();
}

VideoCaptureClass::~VideoCaptureClass(void)
{
 //WriteSettings();

 for(std::vector<cv::VideoCapture *>::size_type i = 0; i < capturesList.size(); i++)
 {
  delete capturesList.at(i);
 }
}

void VideoCaptureClass::CaptureFrame(int i)
{
 try
 {
  QReadLocker rLocker(&rwLock);
  switch(sources.at(i).first)
  {
  case device:
  case ipCamera:
  case videoFile:
   if (capturesList.at(i)->isOpened())
   {
    if(!capturesList.at(i)->read(frames.at(i)))
    {
     ReleaseCapture(i);
     OpenCapture(i);
    }

    //if (sources.at(i).first == ipCamera)
    // if (capturesList.at(i)->get(CV_CAP_PROP_POS_FRAMES) >= capturesList.at(i)->get(CV_CAP_PROP_FRAME_COUNT))
    //  break;
    //if (capturesList.at(i)->read(frames.at(i)))
    //{
    // cv::cvtColor(frames.at(i), frames.at(i), CV_BGR2RGB); 
    //}
   }
   break;

  case pictureFile:
   break;

  case imgSequence:

   if (imgSequenceFileToBeCaptured >= files.size())
   {
    emit ImgSequenceEnded();
    break;
   }

   //frames.at(i) = cv::imread(QString(sources.at(i).second + "/" + files.at(imgSequenceFileToBeCaptured)).toStdString());
   cv::cvtColor(frames.at(i), frames.at(i), CV_BGR2RGB);
   imgSequenceFileToBeCaptured++;
   break;
  }
 }
 catch(cv::Exception &ex)
 {
  ex.formatMessage();
  QMessageBox msgBox;
  msgBox.setText(QString(ex.msg.c_str()));
  msgBox.exec();
 }
}

const cv::Mat* VideoCaptureClass::GetFrame(int i)
{
 return &frames.at(i);
}

void VideoCaptureClass::OpenCapture(int i, QString string)
{
 try
 {
  capturesList.at(i)->open(string.toStdString());
 }
 catch(cv::Exception &ex)
 {
  ex.formatMessage();
  QMessageBox msgBox;
  msgBox.setText(QString(ex.msg.c_str()));
  msgBox.exec();
 }
}

bool VideoCaptureClass::OpenCapture(int i)
{
 if (i < 0)
  return false;

 try
 {
  switch(sources.at(i).first)
  {
  case device:
   return capturesList.at(i)->open(sources.at(i).second.toInt());
   break;

  case ipCamera:
  case videoFile:
   return capturesList.at(i)->open(sources.at(i).second.toUtf8().constData());
   break;

  case pictureFile:
   // do smth with it
//   frames.at(i) = cv::imread(sources.at(i).second.toStdString());
   cv::cvtColor(frames.at(i), frames.at(i), CV_BGR2RGB);
   return true; // ?
   break;

  case imgSequence:
   {
    OpenImgSequence(i);
    return true; // ?
    break;
   }

  default:
   return false;
  }
 }
 catch(cv::Exception &ex)
 {
  ex.formatMessage();
  QMessageBox msgBox;
  msgBox.setText(QString(ex.msg.c_str()));
  msgBox.exec();
  return false;
 }
}

int VideoCaptureClass::GetNumCaptures() const
{
 return capturesList.size();
}

void VideoCaptureClass::WriteSettings(QString &fileName)
{
 QSettings settings(fileName, QSettings::IniFormat);
 settings.beginGroup("VideoCaptures");
 settings.setValue("num_captures", int(capturesList.size()));
 settings.remove("sources");

 for(std::vector<cv::VideoCapture *>::size_type i = 0; i < capturesList.size(); i++)
 {
   settings.setValue(QString("sources/_%1_.first").arg(i), sources[i].first);
   settings.setValue(QString("sources/_%1_.second").arg(i), sources[i].second);
 }
 settings.endGroup();
}

void VideoCaptureClass::ReadSettings(QString &fileName)
{
 QSettings settings(fileName, QSettings::IniFormat);
 settings.beginGroup("VideoCaptures");
 int num = settings.value("num_captures").toInt();
 if (num > 0)
 {
  for (int i = 0; i < num; i++)
  {
   frames.push_back(cv::Mat());
   capturesList.push_back(new cv::VideoCapture);
   sources.push_back(QPair<int, QString>((settings.value(QString("sources/_%1_.first").arg(i))).toInt(), 
    (settings.value(QString("sources/_%1_.second").arg(i))).toString()));
  }
 }
 else
 {
  // This will be the default value for now
  capturesList.push_back(new cv::VideoCapture);
  sources.push_back(QPair<int, QString>(ipCamera, "http://admin:memory@194.85.99.29/video4.mjpg"));
  frames.push_back(cv::Mat());
 }
}

int VideoCaptureClass::AddCapture()
{
 QWriteLocker wLocker(&rwLock);
 capturesList.push_back(new cv::VideoCapture);
 frames.push_back(cv::Mat());
 sources.push_back(QPair<int, QString>(-1, "")); // int -?
 return sources.size()-1;
}

void VideoCaptureClass::DeleteCapture()
{
 QWriteLocker wLocker(&rwLock);
 capturesList.pop_back();
 sources.pop_back();
}

QPair<int, QString> VideoCaptureClass::GetCaptureSource(int i) const
{
 return sources.at(i);
}

void VideoCaptureClass::SetCaptureSource(QPair<int, QString> src, int i)
{
 sources.at(i) = src;
}

void VideoCaptureClass::SetCaptureSource(int type, const char* url, int i)
{
 SetCaptureSource(QPair<int, QString>(type, url), i);
}

int VideoCaptureClass::GetNumFrames(int i) const
{
 switch (sources.at(i).first)
 {
 case videoFile:
  return capturesList.at(i)->get(CV_CAP_PROP_FRAME_COUNT);
  break;

 case imgSequence:
  return files.size();
  break;

 default:
  return 0;
  break;
 }
}

int VideoCaptureClass::GetPosFrame(int i) const
{
 switch (sources.at(i).first)
 {
 case videoFile:
  return capturesList.at(i)->get(CV_CAP_PROP_POS_FRAMES);
  break;

 case imgSequence:
  return imgSequenceFileToBeCaptured;
  break;

 default:
  return 0;
  break;
 }
}

void VideoCaptureClass::SetPosFrame(int pos, int i)
{
 switch (sources.at(i).first)
 {
 case videoFile:
  capturesList.at(i)->set(CV_CAP_PROP_POS_FRAMES, pos);
  break;

 case imgSequence:
  imgSequenceFileToBeCaptured = pos;
  break;
 }
}

bool VideoCaptureClass::IsOpened(int i)
{
 return capturesList.at(i)->isOpened();
}

void VideoCaptureClass::ReleaseCapture(int i)
{
 capturesList.at(i)->release();
}

double VideoCaptureClass::GetFps(int i) const
{
 return capturesList.at(i)->get(CV_CAP_PROP_FPS);
}

QPair<int, int> VideoCaptureClass::GetSize(int i) const
{
 if ((sources.at(i).first == device) || (sources.at(i).first == ipCamera) || (sources.at(i).first == videoFile))
  return QPair<int, int> (capturesList.at(i)->get(CV_CAP_PROP_FRAME_WIDTH), capturesList.at(i)->get(CV_CAP_PROP_FRAME_HEIGHT));
 if ((sources.at(i).first == pictureFile) || (sources.at(i).first == imgSequence))
 {
  return QPair<int, int> (frames.at(i).size().width, frames.at(i).size().height);
 }
 return QPair<int, int>(0, 0);
}

void VideoCaptureClass::OpenImgSequence(int i)
{
 QDir dir(sources.at(i).second);
 QStringList filters;
 filters << "*.bmp" << "*.png" << "*.jpg" << "*.tiff" << "*.tif";
 dir.setNameFilters(filters);

 if (!files.isEmpty())
  files.clear();
 files = dir.entryList();

 imgSequenceFileToBeCaptured = 0;
 if (!files.empty())
 {
//  frames.at(imgSequenceFileToBeCaptured) = cv::imread(QString(sources.at(imgSequenceFileToBeCaptured).second
//   + "/" + files.at(imgSequenceFileToBeCaptured)).toStdString());
 }
}

bool VideoCaptureClass::HasElement(int i)
{
 if ((i >= 0) && (i < static_cast<int>(capturesList.size()))) // ??
  return true;
 else
  return false;
}
