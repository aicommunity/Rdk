#ifndef VIDEOCAPTURECLASS_H
#define VIDEOCAPTURECLASS_H

#include <QReadWriteLock>
#include <QObject>
#include <QStringList>
#include <vector>

#include "opencv2/opencv.hpp"
#include "Sources.h"


class VideoCaptureClass : public QObject
{
 Q_OBJECT

public:
 VideoCaptureClass(void);
 ~VideoCaptureClass(void);

 void WriteSettings(QString &fileName);
 void ReadSettings(QString &fileName);

 // Open capture at index i
 bool OpenCapture(int i);

 // Open capture at index i with string (IP camera or video file) as its source
 void OpenCapture(int i, QString string); 

 // read a frame from the capture at index i and save it in "frame"
 void CaptureFrame(int i);

 // return "frame" pointer
 const cv::Mat* GetFrame(int i); 

 // return the number of elements in "capturesList"
 int GetNumCaptures() const;

 // view and edit "sources"
 QPair<int, QString> GetCaptureSource(int i) const;
 void SetCaptureSource(QPair<int, QString> src, int i);
 void SetCaptureSource(int type, const char* url, int i);

 // add an element to the end of "capturesList"
 // index of the new capture is returned
 int AddCapture();

 // delete an element from the end of "capturesList"
 void DeleteCapture();
 
 // get the number of frames of capture #i 
 // in the case of a video file returns the number of frames in it
 // in the case of an image sequence returns the size of "files" (number of images)
 // otherwise returns 0
 int GetNumFrames(int i) const;

 // get current frame of capture #i 
 // in the case of a video file or an image sequence returns the number of current frame or image
 // otherwise returns 0
 int GetPosFrame(int i) const;

 // set current frame of capture #i
 void SetPosFrame(int pos, int i);

 // checks if a capture is opened
 bool IsOpened(int i);

 // releases a capture
 void ReleaseCapture(int i);

 //
 double GetFps(int i) const;

 bool HasElement(int i);

 QPair<int, int> GetSize(int i) const;

private:
 std::vector<cv::Mat> frames; // его размер пока такой же, как у capturesList
 std::vector<cv::VideoCapture *> capturesList;
 
 // Источники для capturesList. Т.к. аргументами cv::VideoCapture::open() м.б.
 // int или std::string, в случае устройства как источника (device)
 // нужно преобразовать строку в int перед тем, как отправлять её в open().
 std::vector<QPair<int, QString> > sources;

 // when an image sequence is opened, folder contents are stored here
 QStringList files;
 int imgSequenceFileToBeCaptured;

 QReadWriteLock rwLock;
  
 void OpenImgSequence(int i);

signals:
 void ImgSequenceEnded();
};

#endif // VIDEOCAPTURECLASS_H