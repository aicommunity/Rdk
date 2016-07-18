#include "../../../Rtv/Deploy/Include/rtv_initdll.h"
#include "../../../Rdk/Deploy/Include/myrdk.h"

#include <QDateTime>
#include <QDebug>
#include <QCloseEvent>
#include <QItemSelectionModel>
#include <QMdiSubWindow>
#include <QSettings>

//#include <QDateTime>

#include "ModelEditorWindow.h"
#include "ImagesWindow.h"
#include "MainWindow.h"
#include "ModelItem.h"

MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent)
{
 ui.setupUi(this);

 ui.pMenuWindow->addAction(ui.logDockWidget->toggleViewAction());
 ui.menuToolbars->addAction(ui.pMainToolBar->toggleViewAction());
 toggleModelStructure = ui.modelViewDockWidget->toggleViewAction();
 ui.pMenuWindow->addAction(toggleModelStructure);

 performance = new PerformanceView(this);
 ui.performanceViewDockWidget->setWidget(performance);

 settingsFileName = "Settings.ini";

 connect(ui.actionCreateProject, SIGNAL(triggered()), this, SLOT(CreateProjectTriggered()));
 connect(ui.actionLoadProject, SIGNAL(triggered()), this, SLOT(LoadProjectTriggered()));
 connect(ui.actionSaveProject, SIGNAL(triggered()), this, SLOT(SaveProjectTriggered()));
 connect(ui.actionCopyProject, SIGNAL(triggered()), this, SLOT(CopyProjectTriggered()));

 connect(ui.actionCreateModel, SIGNAL(triggered()), this, SLOT(CreateModelTriggered()));
 connect(ui.actionLoadModel, SIGNAL(triggered()), this, SLOT(LoadModelTriggered()));
 connect(ui.actionSaveModel, SIGNAL(triggered()), this, SLOT(SaveModelTriggered()));

 connect(ui.actionReloadParameters, SIGNAL(triggered()), this, SLOT(ReloadParametersTriggered()));

 connect(&cap, SIGNAL(ImgSequenceEnded()), this, SLOT(StopTriggered()));
 connect(ui.actionStart, SIGNAL(triggered()), this, SLOT(StartTriggered()));
 connect(ui.actionStop, SIGNAL(triggered()), this, SLOT(StopTriggered()));
 connect(ui.actionReset, SIGNAL(triggered()), this, SLOT(ResetTriggered()));
 connect(ui.actionStep, SIGNAL(triggered()), this, SLOT(StepTriggered()));

 connect(ui.actionDrawModel, SIGNAL(triggered()), this, SLOT(DrawModelTriggered()));
 connect(ui.actionImages, SIGNAL(triggered()), this, SLOT(ImagesTriggered()));

 connect(ui.actionTabbedView, SIGNAL(triggered()), this, SLOT(SetTabbedViewMode()));
 connect(ui.actionSubwindowView, SIGNAL(triggered()), this, SLOT(SetSubwindowViewMode()));
 connect(ui.actionCascadeSubwindows, SIGNAL(triggered()), ui.mdiArea, SLOT(cascadeSubWindows()));
 connect(ui.actionTileSubwindows, SIGNAL(triggered()), ui.mdiArea, SLOT(tileSubWindows()));

 connect(ui.setParametersButton, SIGNAL(clicked()), this, SLOT(SetParametersClicked()));
 connect(ui.setStateButton, SIGNAL(clicked()), this, SLOT(SetStateClicked()));

 ReadSettings();
 
 //GraphicalEngineInit(23, 1, 1, 640, 480, false, ExceptionHandler);
 
 // количество открываемых captures
 openCapturesNumber = 1;

 InitializeGraphicalEngine();
}

MainWindow::~MainWindow()
{
 
}

void MainWindow::InitializeGraphicalEngine()
{
 StopTriggered();
 if (cap.IsOpened(0))
  CloseCaptures(); // if needed
 OpenCaptures();

 // source 0
 GraphicalEngineInit(29, 1, 1, cap.GetSize(0).first, cap.GetSize(0).second, false, (void*)ExceptionHandler);
 Env_SetDebugMode(true);
 //SetSystemDir("e:/Work/RTC/14/RTV/Bin/Platform/Win/");
 //Engine_LoadFonts();

 //qDebug() << "Graphical engine was initialized with resolution" << cap.GetSize(0);
}

void MainWindow::ShowModelStructure()
{
 model = new ModelStructure(this);
 ui.modelStructureView->setModel(model);
 ui.modelStructureView->expandAll();

 modelStructureSelection = ui.modelStructureView->selectionModel();
 connect(modelStructureSelection, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
         this, SLOT(SelectionChangedSlot(const QItemSelection &, const QItemSelection &)));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
 WriteSettings();
 event->accept();
}

void MainWindow::WriteSettings()
{
 QSettings settings(settingsFileName, QSettings::IniFormat);

 settings.beginGroup("mainwindow");

 // Dock widgets!Если закрыть программу с floating dock widget, 
 // то при повторном запуске он больше не docks.
 settings.setValue("geometry", saveGeometry());
 settings.setValue("state", saveState());
 settings.setValue("modelStructureSplitter_state", ui.splitter->saveState());
 settings.endGroup();

 cap.WriteSettings(settingsFileName);
}

void MainWindow::ReadSettings()
{
 QSettings settings(settingsFileName, QSettings::IniFormat);

 settings.beginGroup("mainwindow");
 restoreGeometry(settings.value("geometry").toByteArray());
 restoreState(settings.value("state").toByteArray());
 ui.splitter->restoreState(settings.value("modelStructureSplitter_state").toByteArray());
 settings.endGroup();

 cap.ReadSettings(settingsFileName);
}

void MainWindow::CreateProjectTriggered()
{
 ui.log->append("Create project.");
}

void MainWindow::LoadProjectTriggered()
{
 ui.log->append("Load project.");
}

void MainWindow::SaveProjectTriggered()
{
 ui.log->append("Save project.");
}

void MainWindow::CopyProjectTriggered()
{
 ui.log->append("Copy project.");
}

void MainWindow::StartTriggered()
{
 // only for the source #0
 double fps = 0;
 fps = cap.GetFps(0);
 if (fps == 0)
  fps = 25; // default value
 if (captureImgTimer.isActive())
  captureImgTimer.stop();
 captureImgTimer.start(1000/fps, Qt::PreciseTimer, this);
 if (processImgTimer.isActive())
  processImgTimer.stop();
 processImgTimer.start(100, Qt::PreciseTimer, this);

 SkipCounter=0;
 MaxSkipCounter=0;

 emit StartProcessing();
}

void MainWindow::StopTriggered()
{
 captureImgTimer.stop();
 processImgTimer.stop();
}

void MainWindow::ResetTriggered()
{
 Env_Reset(0);
}

void MainWindow::StepTriggered()
{
 CaptureFrames();
 ProcessFrame();
}

void MainWindow::CreateModelTriggered()
{
 ui.log->append("Create Model.");

 ui.modelStructureView->model()->~QAbstractItemModel();

 //Model_Create("Pipeline");
 //
 //qDebug() << "Checking if the model exists: " << Model_Check();
 //qDebug() << "Checking if the root of the model exists: " << Model_CheckComponent("");
 //qDebug() << "Number of components in the root: " << Model_GetNumComponents("");
 //qDebug() << "Names of components in the root: " << Model_GetComponentsNameList("");
 //qDebug() << "What was added to the \"\": " << Model_AddComponent("", "Pipeline");
 //qDebug() << "Number of components in the root: " << Model_GetNumComponents("");
 //qDebug() << "Names of components in the root: " << Model_GetComponentsNameList("");
 //Model_AddComponent("Pipeline", "QSolarize");
 //Model_AddComponent("Pipeline", "VideoSimulatorSimple");
 //Model_AddComponent("Pipeline", "BitmapSourceFile");
 //qDebug() << "Names of components in \"Pipeline\": " << Model_GetComponentsNameList("Pipeline");

// ShowModelStructure();
 modelStructureSelection = ui.modelStructureView->selectionModel();
 connect(modelStructureSelection, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
  this, SLOT(SelectionChangedSlot(const QItemSelection &, const QItemSelection &)));

}

void MainWindow::LoadModelTriggered()
{
 ui.log->append("Load Model.");

}

void MainWindow::SaveModelTriggered()
{
 ui.log->append("Save Model.");
}

void MainWindow::SelectionChangedSlot(const QItemSelection & newSelection, const QItemSelection & oldSelection)
{
 selectedComponentLongName = model->GetComponentLongName(ui.modelStructureView->selectionModel()->currentIndex());

 ui.parametersText->clear();
 ui.parametersText->insertPlainText(Model_GetComponentProperties(selectedComponentLongName.toLocal8Bit(), ptPubParameter));

 ui.stateText->clear();
 ui.stateText->insertPlainText(Model_GetComponentProperties(selectedComponentLongName.toLocal8Bit(), ptPubState));

 ui.inputsText->clear();
 ui.inputsText->insertPlainText(Model_GetComponentProperties(selectedComponentLongName.toLocal8Bit(), ptPubInput));

 ui.outputsText->clear();
 ui.outputsText->insertPlainText(Model_GetComponentProperties(selectedComponentLongName.toLocal8Bit(), ptPubOutput));
}

void MainWindow::DrawModelTriggered()
{
 QList<QMdiSubWindow *> windows = ui.mdiArea->subWindowList();
 for (int i = 0; i < windows.size(); i++)
 {
  // "Model Editor" надо взять более человеческим способом из класса ModelEditorWindow
  if (windows.at(i)->widget()->windowTitle() == "Model Editor")
  {
   SetActiveSubWindow(windows.at(i));
   return;
  }
 }

 ModelEditorWindow *mdiChild =  new ModelEditorWindow(ui.mdiArea);
 ui.mdiArea->addSubWindow(mdiChild);
 mdiChild->setAttribute(Qt::WA_DeleteOnClose, true);
 mdiChild->show();
}

void MainWindow::ImagesTriggered()
{
 //// Сейчас можно открыть только одно подобное окно, как и в том, что было

 //// Есть ли более простой способ проверить, открыто ли уже нужное окно, и получить при этом QMdiSubWindow*?
 //QList<QMdiSubWindow *> windows = ui.mdiArea->subWindowList();
 //for (int i = 0; i < windows.size(); i++)
 //{
 // // "Images" надо взять более человеческим способом из класса ImagesWindow
 // if (windows.at(i)->widget()->windowTitle() == "Images")
 // {
 //  SetActiveSubWindow(windows.at(i));
 //  return;
 // }
 //}

 ImagesWindow *mdiChild =  new ImagesWindow(&cap, ui.mdiArea);
 ui.mdiArea->addSubWindow(mdiChild);
 mdiChild->setAttribute(Qt::WA_DeleteOnClose, true);
 connect(this, SIGNAL(ShowNewImages()), mdiChild, SLOT(UpdateImages()));
 connect(this, SIGNAL(StartProcessing()), mdiChild, SLOT(Start()));
 connect(mdiChild, SIGNAL(InputConfigurationChanged()), this, SLOT(InitializeGraphicalEngine()));
 mdiChild->show();
}

void MainWindow::SetTabbedViewMode()
{
 ui.mdiArea->setViewMode(QMdiArea::TabbedView);
 ui.actionCascadeSubwindows->setEnabled(false);
 ui.actionTileSubwindows->setEnabled(false);
}

void MainWindow::SetSubwindowViewMode()
{
 ui.mdiArea->setViewMode(QMdiArea::SubWindowView);
 ui.actionCascadeSubwindows->setEnabled(true);
 ui.actionTileSubwindows->setEnabled(true);
}

void MainWindow::SetActiveSubWindow(QWidget *window)
{
 if (!window)
 {
  return;
 }
 ui.mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::SetParametersClicked()
{
 Model_SetComponentProperties(selectedComponentLongName.toLocal8Bit(), ui.parametersText->toPlainText().toLocal8Bit());
 //Model_LoadComponentParameters(selectedComponentLongName.toLocal8Bit(), ui.parametersText->toPlainText().toLocal8Bit());
}

void MainWindow::SetStateClicked()
{
 //Model_SetComponentProperties(selectedComponentLongName.toLocal8Bit(), ui.stateText->toPlainText().toLocal8Bit());
}

void MainWindow::ReloadParametersTriggered()
{
 QFile file("Parameters.xml");
 if (!file.open(QFile::ReadOnly | QFile::Text)) 
  qDebug() << "The file cannot be opened";
 else
 {
  QTextStream in(&file);
  QByteArray fileText = in.readAll().toLocal8Bit();
  Model_LoadComponentParameters("", fileText.data());
  file.close();
 }
}

void MainWindow::CaptureFrames()
{
 for (int i = 0; i < openCapturesNumber; i++)
 {
  cap.CaptureFrame(i);
 }
}

void MainWindow::OpenCaptures() 
{
 for (int i = 0; i < openCapturesNumber; i++)
 {
  cap.OpenCapture(i);
 }
}

void MainWindow::CloseCaptures()
{
 for (int i = 0; i < openCapturesNumber; i++)
 {
  cap.ReleaseCapture(i);
 }
}

void MainWindow::ProcessFrame()
{
 if(SkipCounter<MaxSkipCounter)
 {
  ++SkipCounter;
  return;
 }
 else
 {
  SkipCounter=0;
 }

 //Env_SetInputImage(0, (*cap.GetFrame(0)).data, (*cap.GetFrame(0)).cols, (*cap.GetFrame(0)).rows, 3);
 RDK::UBitmap bmp;
 bmp.AttachBuffer((*cap.GetFrame(0)).cols, (*cap.GetFrame(0)).rows, (*cap.GetFrame(0)).data, RDK::ubmRGB24);
 if(TempBmp.GetByteLength() == bmp.GetByteLength())
 {
  if(memcmp(bmp.GetData(),TempBmp.GetData(),bmp.GetByteLength()) == 0)
  {
   bmp.DetachBuffer();
   return;
  }
 }

 Model_SetComponentBitmapOutputByIndex("", 0, &bmp);
 TempBmp=bmp;
 bmp.DetachBuffer();
 
 QDateTime timeData = QDateTime::currentDateTime();
 Model_SetDoubleSourceTimeAll(static_cast<double>(timeData.toMSecsSinceEpoch()/86400000.0 + 25569.0));

 Env_Calculate(0);

 performance->CalculateDurations();
 emit ShowNewImages();
 //qDebug() << "FPS:" << cap.GetFps(0) << "Size:" << cap.GetSize(0);
}

ModelStructure* MainWindow::GetModel()
{
 return model;
}

void MainWindow::AddLog(QString newLog)
{
 ui.log->append(newLog);
}

void MainWindow::timerEvent(QTimerEvent* event)
{
 if (event->timerId() == processImgTimer.timerId())
 {
  ProcessFrame();
 }

 else if (event->timerId() == captureImgTimer.timerId())
 {
  CaptureFrames();
 }
}
