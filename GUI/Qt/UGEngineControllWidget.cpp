#include "UGEngineControllWidget.h"
#include "ui_UGEngineControllWidget.h"


#include <rdk_application.h>
#include <QSettings>
#include <QDebug>
#include <QDialog>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QInputDialog>

/*int heheheCounter = 0;
void hehehe(){qDebug("hehehe %d", ++heheheCounter);}*/

UGEngineControllWidget::UGEngineControllWidget(QWidget *parent, RDK::UApplication *app) :
    QMainWindow(parent),
    ui(new Ui::UGEngineControllWidget)
{
    ui->setupUi(this);
    setAccessibleName("UGEngineControllWidget");

    application = app;

    if(application == NULL)
      QApplication::exit(-1);

    settings = NULL;
    propertyChanger = NULL;
    drawEngine = NULL;
    componentLinks = NULL;
    images = NULL;
    imagesWindow = NULL;
    channels = NULL;
    createConfigurationWizardWidget = NULL;
    createTestWidget = NULL;
    statusPanel = NULL;

    settings = new USettingsReaderWidget(this);
    connect(settings, SIGNAL(readSetting()) , this, SLOT(readSettings()));
    connect(settings, SIGNAL(writeSetting()), this, SLOT(writeSettings()));

    propertyChanger = new UComponentPropertyChanger(this, application);
    ui->dockWidgetComponentsList->setWidget(propertyChanger);

    drawEngine = new UDrawEngineWidget(this, application);
    QMdiSubWindow *drawEngineSbWindow = new SubWindowCloseIgnore(ui->mdiArea, Qt::SubWindow);
    drawEngineSbWindow->setWidget(drawEngine);
    drawEngineSbWindow->show();
    drawEngineSbWindow->showMaximized();

    // связывание схемы модели и списка отображения компонентов модели
    //  схема -> список
    connect(propertyChanger->componentsList, SIGNAL(componentDoubleClick(QString)),
            drawEngine, SLOT(componentDoubleClick(QString)));
    connect(propertyChanger->componentsList, SIGNAL(componentSelected(QString)),
            drawEngine, SLOT(componentSingleClick(QString)));
    connect(propertyChanger->componentsList, SIGNAL(updateScheme(bool)),
            drawEngine, SLOT(updateScheme(bool)));

    //  список -> схема
    connect(drawEngine, SIGNAL(componentSelectedFromScheme(QString)),
            propertyChanger->componentsList, SLOT(componentSelectedFromScheme(QString)));
    connect(drawEngine, SIGNAL(componentDoubleClickFromScheme(QString)),
            propertyChanger->componentsList, SLOT(componentDoubleClickFromScheme(QString)));
    connect(drawEngine, SIGNAL(componentStapBackFromScheme()),
            propertyChanger->componentsList, SLOT(componentStapBackFromScheme()));
    connect(drawEngine, SIGNAL(updateComponentsListFromScheme()),
            propertyChanger->componentsList, SLOT(updateComponentsListFromScheme()));

    componentLinks = new UComponentLinksWidget(this, application);
    componentLinks->hide();

    // связывание схемы модели и окна отображения связей
    //  связи -> схема
    connect(componentLinks, SIGNAL(updateScheme(bool)), drawEngine, SLOT(updateScheme(bool)));

    //  схема -> связи
    // обнако слоты находятся в окне главного интерфейса, так как необходимо сначала создать
    // диалоговые окна (QDialog) для отображения виджета связей
    connect(drawEngine, SIGNAL(viewLinksFromScheme(QString)), this, SLOT(showLinksForSingleComponent(QString)));
    connect(drawEngine, SIGNAL(createLinksFromScheme(QString,QString)), this, SLOT(showLinksForTwoComponents(QString,QString)));

    images = new UImagesWidget(this, application);
    images->hide();

    channels = new UCalculationChannelsWidget(this, application);
    ui->dockWidgetChannels->setWidget(channels);

    logger = new ULoggerWidget(this, application);
    ui->dockWidgetLoger->setWidget(logger);

    graphWindowWidget = new UGraphWidget(this, application);
    ui->dockWidgetGraph->setWidget(graphWindowWidget);

    createConfigurationWizardWidget=new UCreateConfigurationWizardWidget(this, application);

    createTestWidget = new UCreateTestWidget(this, application);
    createTestWidget->hide();

    statusPanel = new UStatusPanel(this, application);
    ui->statusBar->addWidget(statusPanel, 1);
    connect(statusPanel, SIGNAL(saveConfig()), this, SLOT(actionSaveConfig()));
    connect(statusPanel, SIGNAL(setPropertyUpdateInterval(long)),
            propertyChanger->componentsList, SLOT(setUpdateInterval(long)));

    // GUI actions:

    // file menu actions:
    connect(ui->actionCreateConfig, SIGNAL(triggered(bool)), this, SLOT(actionCreateConfig()));
    connect(ui->actionLoadConfig, SIGNAL(triggered(bool)), this, SLOT(actionLoadConfig()));
    connect(ui->actionSaveConfig, SIGNAL(triggered(bool)), this, SLOT(actionSaveConfig()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(actionExit()));

    // calculate menu actions:
    connect(ui->actionStart, SIGNAL(triggered(bool)), this, SLOT(actionStart()));
    connect(ui->actionPause, SIGNAL(triggered(bool)), this, SLOT(actionPause()));
    connect(ui->actionReset, SIGNAL(triggered(bool)), this, SLOT(actionReset()));
    connect(ui->actionStep, SIGNAL(triggered(bool)), this, SLOT(actionStep()));
    connect(ui->actionRunNSteps, SIGNAL(triggered(bool)), this, SLOT(actionRunNSteps()));

    // window menu actions:
    connect(ui->actionImagesFromWindow, SIGNAL(triggered(bool)), this, SLOT(actionImages()));
    connect(ui->actionComponentsControl, SIGNAL(triggered(bool)), this, SLOT(actionComponentsControl()));
    connect(ui->actionChannelsControl, SIGNAL(triggered(bool)), this, SLOT(actionChannelsControl()));
    connect(ui->actionLogger, SIGNAL(triggered(bool)), this, SLOT(actionLogger()));
    connect(ui->actionTestCreator, SIGNAL(triggered(bool)), this, SLOT(actionTestCreator()));
    connect(ui->actionWatchWindow, SIGNAL(triggered(bool)), this, SLOT(actionWatchWindow()));
    //connect(ui->action, SIGNAL(triggered(bool)), this, SLOT(action)));

    readSettings();
}

UGEngineControllWidget::~UGEngineControllWidget()
{
    application->UnInit();
    delete ui;
}

void UGEngineControllWidget::showLinksForSingleComponent(QString componentName)
{
    componentLinks->initWidget(componentName);
    execDialogUVisualControllWidget(componentLinks);
}

void UGEngineControllWidget::showLinksForTwoComponents(QString firstComponentName, QString secondComponentName)
{
    componentLinks->initWidget(firstComponentName, secondComponentName);
    execDialogUVisualControllWidget(componentLinks);
}

// file menu actions

void UGEngineControllWidget::actionLoadConfig()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open config file"), QApplication::applicationDirPath()+"/../../../Configs", tr("*.ini"));

    if (fileName.isEmpty())
      return;
    try
    {
      application->OpenProject(fileName.toLocal8Bit().constData());

      this->setWindowTitle("project: " + fileName);

      /*QStringList list = configFileName.split("/");
      list.pop_back();*/

      RDK::UIVisualControllerStorage::UpdateInterface(true);
      //drawEngine->updateScheme(true);
    }
    catch(RDK::UException& e)
    {
      QMessageBox::critical(this,"Error at load project", QString(e.what()), QMessageBox::Ok);
    }
    catch(std::exception& e)
    {
      QMessageBox::critical(this,"Error at load project", QString(e.what()), QMessageBox::Ok);
    }
}

void UGEngineControllWidget::actionCreateConfig()
{
    createConfigurationWizardWidget->show();
    //QMessageBox::information(this,"Create new project", "He-he-he, NO! >:]", QMessageBox::Ok);
}

void UGEngineControllWidget::actionSaveConfig()
{
    application->SaveProject();
}

void UGEngineControllWidget::actionExit()
{
  QApplication::quit();
}

// calculate menu actions

void UGEngineControllWidget::actionReloadParameters()
{
    application->ReloadParameters();
}
//int ts; // костыль
void UGEngineControllWidget::actionStart()
{
  try
  {
    startChannel(-1);
    //ts = startTimer(150); // костыль
//    ui->statusBar->showMessage("Calculation in progress");
  }
  catch(RDK::UException& e)
  {
    QMessageBox::critical(this,"UError at start calculation", QString(e.what()), QMessageBox::Ok);
  }
  catch(std::exception& e)
  {
    QMessageBox::critical(this,"Error at start calculation", QString(e.what()), QMessageBox::Ok);
  }
}

void UGEngineControllWidget::actionPause()
{
    pauseChannel(-1);
    //killTimer(ts); // костыль
//    ui->statusBar->showMessage("Calculation at pause");
}

void UGEngineControllWidget::actionReset()
{
    resetChannel(-1);
//    ui->statusBar->showMessage("Calculation reseted", 5000);
}

void UGEngineControllWidget::actionStep()
{
  calcOneStepChannel(-1);
}

void UGEngineControllWidget::actionRunNSteps()
{
  bool ok;
  QString text = QInputDialog::getText(this, tr("Run several steps"),
                                       tr("Enter steps number: "), QLineEdit::Normal,
                                       "", &ok);
  unsigned int stepsCounter = text.toUInt();
  if (ok && stepsCounter > 0)
  {
    for(unsigned int i = 0; i < stepsCounter; ++i)
      application->StepChannel(-1);
  }
}

// window menu action

void UGEngineControllWidget::actionImages()
{
    if(!imagesWindow)
    {
        imagesWindow = new QMainWindow(this);
        imagesWindow->setCentralWidget(images);
    }
    imagesWindow->resize(images->size());
    images->show();
    imagesWindow->showNormal();
    imagesWindow->activateWindow();
}

void UGEngineControllWidget::actionComponentsControl()
{
    ui->dockWidgetComponentsList->show();
}

void UGEngineControllWidget::actionChannelsControl()
{
  ui->dockWidgetChannels->show();
}

void UGEngineControllWidget::actionLogger()
{
  ui->dockWidgetLoger->show();
}

void UGEngineControllWidget::actionTestCreator()
{
  execDialogUVisualControllWidget(createTestWidget);
}

void UGEngineControllWidget::actionWatchWindow()
{
    //отобразить *graphWindowWidget
    ui->dockWidgetGraph->show();
}

void UGEngineControllWidget::startChannel(int chanelIndex)
{
    if(!application->GetProjectOpenFlag())
        return;

    application->StartChannel(chanelIndex);
}

void UGEngineControllWidget::pauseChannel(int chanelIndex)
{
    if(!application->GetProjectOpenFlag())
        return;

    application->PauseChannel(chanelIndex);
}

void UGEngineControllWidget::resetChannel(int chanelIndex)
{
    if(!application->GetProjectOpenFlag())
        return;

    application->ResetChannel(chanelIndex);
}

void UGEngineControllWidget::calcOneStepChannel(int chanelIndex)
{
    if(!application->GetProjectOpenFlag())
        return;

    application->StepChannel(chanelIndex);
}

void UGEngineControllWidget::execDialogUVisualControllWidget(UVisualControllerWidget *widget)
{
    QWidget *widgetOldParent = widget->parentWidget();

    QDialog dialogWindow(this);
    QHBoxLayout *lay = new QHBoxLayout(&dialogWindow);
    lay->setMargin(0);
    lay->addWidget(widget);
    dialogWindow.resize(widget->size());
    dialogWindow.setLayout(lay);
    dialogWindow.setWindowTitle(widget->accessibleName());
    connect(&dialogWindow, SIGNAL(rejected()), widget, SLOT(unInit()));
    connect(widget, SIGNAL(closeWindow()), &dialogWindow, SLOT(reject()));
    widget->show();
    dialogWindow.exec();
    widget->hide();
    widget->setParent(widgetOldParent);
}

void UGEngineControllWidget::writeSettings()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("state",    saveState());

    if(imagesWindow)
    {
      settings.setValue("ImagesGeometry", imagesWindow->saveGeometry());
      settings.setValue("ImagesState",    imagesWindow->saveState());
    }

    settings.endGroup();
}

void UGEngineControllWidget::readSettings()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());

    if(!imagesWindow)
    {
        imagesWindow = new QMainWindow(this);
        imagesWindow->setCentralWidget(images);
    }
    imagesWindow->resize(images->size());
    imagesWindow->restoreGeometry(settings.value("ImagesGeometry").toByteArray());
    imagesWindow->restoreState(settings.value("ImagesState").toByteArray());

    settings.endGroup();
}
