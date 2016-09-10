#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QBasicTimer>
#include <QtWidgets/QMainWindow>

#include "ModelStructure.h"
#include "PerformanceView.h"
#include "ui_mainwindow.h"
#include "VideoCaptureClass.h"
#include "../../Deploy/Include/myrdk.h"

extern void ExceptionHandler();

class MainWindow : public QMainWindow
{
 Q_OBJECT

public:
 MainWindow(QWidget *parent = 0);
 ~MainWindow();

 ModelStructure* GetModel();
 void AddLog(QString newLog);
 void ShowModelStructure();

private:
 Ui::MainWindowClass ui;
 ModelStructure *model;
 QString selectedComponentLongName;
 QItemSelectionModel *modelStructureSelection;
 QAction *toggleModelStructure;
 QBasicTimer captureImgTimer;
 QBasicTimer processImgTimer;
 VideoCaptureClass cap;
 int openCapturesNumber;
 QString settingsFileName;
 PerformanceView* performance;

 int MaxSkipCounter;
 int SkipCounter;

 RDK::UBitmap TempBmp;

 void closeEvent(QCloseEvent *event);
 void ReadSettings();
 void WriteSettings();
 void timerEvent(QTimerEvent* event);

private slots:
 void CreateProjectTriggered();
 void LoadProjectTriggered();
 void SaveProjectTriggered();
 void CopyProjectTriggered();
 void CreateModelTriggered();
 void LoadModelTriggered();
 void SaveModelTriggered();
 void StartTriggered();
 void StopTriggered();
 void ResetTriggered();
 void StepTriggered();
 void DrawModelTriggered();
 void ImagesTriggered();
 void ReloadParametersTriggered();
 void SelectionChangedSlot(const QItemSelection & newSelection, const QItemSelection & oldSelection);
 void SetTabbedViewMode();
 void SetSubwindowViewMode();
 void SetActiveSubWindow(QWidget *window);
 void SetParametersClicked();
 void SetStateClicked();
 void OpenCaptures();
 void CloseCaptures();
 void ProcessFrame();
 void CaptureFrames();

public slots:
 void InitializeGraphicalEngine();
 
signals:
 void TimerTick();
 void StartProcessing();
 void ProcessImgTimerTimeOut();
 void CaptureImgTimerTimeOut();
 void ShowNewImages();
};

#endif // MAINWINDOW_H
