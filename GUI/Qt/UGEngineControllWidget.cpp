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

int heheheCounter = 0;
void hehehe(){qDebug("hehehe %d", ++heheheCounter);}

UGEngineControllWidget::UGEngineControllWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UGEngineControllWidget)
{
    ui->setupUi(this);

    application.SetApplicationFileName(QApplication::applicationFilePath().toLocal8Bit().constData());
    rpcDispatcher.SetDecoderPrototype(&rpcDecoder);
    rpcDispatcher.SetCommonDecoder(&rpcDecoderCommon);
    application.SetRpcDispatcher(&rpcDispatcher);
    application.SetServerControl(&serverControl);
    application.SetEngineControl(&engineControl);
    application.SetProject(&project);
    application.SetLogDir((QApplication::applicationDirPath()+"/").toLocal8Bit().constData());
    application.SetDebugMode(true);
    application.Init();

//    initGraphicalEngine();
    settingsFileName = "settings.qt";
    settingsGroupName = "UGEngineControllWidget";

    componentsList = NULL;
    drawEngine = NULL;
    componentLinks = NULL;
    images = NULL;
    imagesWindow = NULL;
    channels = NULL;
    createConfigurationWizardWidget = NULL;

    readSettings(settingsFileName, settingsGroupName);

    componentsList = new UComponentsListWidget(this, settingsFileName);
    ui->dockWidgetComponentsList->setWidget(componentsList);

    drawEngine = new UDrawEngineWidget(this, settingsFileName);
    QMdiSubWindow *drawEngineSbWindow = new SubWindowCloseIgnore(ui->mdiArea, Qt::SubWindow);
    drawEngineSbWindow->setWidget(drawEngine);
    drawEngineSbWindow->show();
    drawEngineSbWindow->showMaximized();

    // связывание схемы модели и списка отображения компонентов модели
    //  схема -> список
    connect(componentsList, SIGNAL(componentDoubleClick(QString)),
            drawEngine, SLOT(componentDoubleClick(QString)));
    connect(componentsList, SIGNAL(componentSelected(QString)),
            drawEngine, SLOT(componentSingleClick(QString)));
    connect(componentsList, SIGNAL(updateScheme(bool)),
            drawEngine, SLOT(updateScheme(bool)));

    //  список -> схема
    connect(drawEngine, SIGNAL(componentSelectedFromScheme(QString)),
            componentsList, SLOT(componentSelectedFromScheme(QString)));
    connect(drawEngine, SIGNAL(componentDoubleClickFromScheme(QString)),
            componentsList, SLOT(componentDoubleClickFromScheme(QString)));
    connect(drawEngine, SIGNAL(componentStapBackFromScheme()),
            componentsList, SLOT(componentStapBackFromScheme()));
    connect(drawEngine, SIGNAL(updateComponentsListFromScheme()),
            componentsList, SLOT(updateComponentsListFromScheme()));

    componentLinks = new UComponentLinksWidget(this, settingsFileName);
    componentLinks->hide();

    // связывание схемы модели и окна отображения связей
    //  связи -> схема
    connect(componentLinks, SIGNAL(updateScheme(bool)), drawEngine, SLOT(updateScheme(bool)));

    //  схема -> связи
    // но слоты находятся в окне главного интерфейса, так как необходимо сначала создать
    // диалоговые окна (QDialog) для отображения виджета связей
    connect(drawEngine, SIGNAL(viewLinksFromScheme(QString)), this, SLOT(showLinksForSingleComponent(QString)));
    connect(drawEngine, SIGNAL(createLinksFromScheme(QString,QString)), this, SLOT(showLinksForTwoComponents(QString,QString)));

    images = new UImagesWidget(this, settingsFileName);
    images->hide();

    channels = new UCalculationChannelsWidget(this, &application);
    ui->dockWidgetChannels->setWidget(channels);

    logger = new ULoggerWidget(this, &application);
    ui->dockWidgetLoger->setWidget(logger);

    createConfigurationWizardWidget=new UCreateConfigurationWizardWidget(this);

    // GUI actions:
    connect(ui->actionComponentsControl, SIGNAL(triggered(bool)), this, SLOT(actionComponentsControl()));
    connect(ui->actionChannelsControl, SIGNAL(triggered(bool)), this, SLOT(actionChannelsControl()));
    connect(ui->actionCreateConfig, SIGNAL(triggered(bool)), this, SLOT(actionCreateConfig()));
    connect(ui->actionLoadConfig, SIGNAL(triggered(bool)), this, SLOT(actionLoadConfig()));
    connect(ui->actionSaveConfig, SIGNAL(triggered(bool)), this, SLOT(actionSaveConfig()));
    connect(ui->actionStart, SIGNAL(triggered(bool)), this, SLOT(actionStart()));
    connect(ui->actionPause, SIGNAL(triggered(bool)), this, SLOT(actionPause()));
    connect(ui->actionReset, SIGNAL(triggered(bool)), this, SLOT(actionReset()));
    connect(ui->actionStep, SIGNAL(triggered(bool)), this, SLOT(actionStep()));
    connect(ui->actionImagesFromWindow, SIGNAL(triggered(bool)), this, SLOT(actionImages()));
    //connect(ui->action, SIGNAL(triggered(bool)), this, SLOT(action));



}

UGEngineControllWidget::~UGEngineControllWidget()
{
    application.UnInit();
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

void UGEngineControllWidget::actionComponentsControl()
{
    ui->dockWidgetComponentsList->show();
}

void UGEngineControllWidget::actionChannelsControl()
{
    ui->dockWidgetChannels->show();
}

void UGEngineControllWidget::actionLoadConfig()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open config file"), QApplication::applicationDirPath()+"/../../../Configs", tr("*.ini"));
    if (fileName != "")
    {
        application.OpenProject(fileName.toLocal8Bit().constData());
        configFileName = fileName;
        this->setWindowTitle("project: " + configFileName);
        QStringList list = configFileName.split("/");
        list.pop_back();
        readSettings(list.join("/") + "/settings.qt");
        RDK::UIVisualControllerStorage::UpdateInterface(true);
        drawEngine->updateScheme(true);
    }
}

void UGEngineControllWidget::actionCreateConfig()
{
    createConfigurationWizardWidget->show();
    //QMessageBox::information(this,"Create new project", "He-he-he, NO! >:]", QMessageBox::Ok);
}

void UGEngineControllWidget::actionSaveConfig()
{
    application.SaveProject();
    writeSettings(settingsFileName);
}

void UGEngineControllWidget::actionReloadParameters()
{
    application.ReloadParameters();
}
//int ts; // костыль
void UGEngineControllWidget::actionStart()
{

    startChannel(-1);
    //ts = startTimer(150); // костыль
    ui->statusBar->showMessage("Calculation in progress");
}

void UGEngineControllWidget::actionPause()
{
    pauseChannel(-1);
    //killTimer(ts); // костыль
    ui->statusBar->showMessage("Calculation at pause");
}

void UGEngineControllWidget::actionReset()
{
    resetChannel(-1);
    ui->statusBar->showMessage("Calculation reseted", 5000);
}

void UGEngineControllWidget::actionStep()
{
    calcOneStepChannel(-1);
}

void UGEngineControllWidget::actionImages()
{
    if(!imagesWindow)
    {
        imagesWindow = new QMainWindow(this);
        imagesWindow->setCentralWidget(images);
    }
    imagesWindow->resize(images->size());
    images->show();
    //imagesWindow->show();
    imagesWindow->showNormal();
    imagesWindow->activateWindow();
}

/*void UGEngineControllWidget::timerEvent(QTimerEvent *) // костыль
{
    application.GetEngineControl()->TimerExecute();
    //RDK::UIVisualControllerStorage::UpdateInterface(false);
}*/

/*void UGEngineControllWidget::testUpdate()
{
    //Interface update test
    ThreadGo *worker = new ThreadGo(drawEngine);
    QThread *thread = new QThread(this);
    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(startThread()));
    connect(worker, SIGNAL(testFinished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(testFinished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void UGEngineControllWidget::initGraphicalEngine()
{
    Core_ChannelInit(0, (void*)hehehe);
    //std::string font_path=Extract;
    QString font_path = QCoreApplication::applicationDirPath()+"/";
    Core_SetSystemDir(font_path.toLocal8Bit().constData());
    Core_LoadFonts();
    Env_SetDebugMode(true); //неизвестное применение
//    MLog_SetDebugMode(0, true);
    //    MLog_SetEventsLogMode(0, true);
}*/

void UGEngineControllWidget::startChannel(int chanelIndex)
{
    if(!application.GetProjectOpenFlag())
        return;

    application.StartChannel(chanelIndex);
}

void UGEngineControllWidget::pauseChannel(int chanelIndex)
{
    if(!application.GetProjectOpenFlag())
        return;

    application.PauseChannel(chanelIndex);
}

void UGEngineControllWidget::resetChannel(int chanelIndex)
{
    if(!application.GetProjectOpenFlag())
        return;

    application.ResetChannel(chanelIndex);
}

void UGEngineControllWidget::calcOneStepChannel(int chanelIndex)
{
    if(!application.GetProjectOpenFlag())
        return;

    application.StepChannel(chanelIndex);
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

void UGEngineControllWidget::writeSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.endGroup();
    if(componentsList) componentsList->writeSettings(settingsFileName);
    if(drawEngine) drawEngine->writeSettings(settingsFileName);
    if(componentLinks) componentLinks->writeSettings(settingsFileName);
    if(images) images->writeSettings(settingsFileName);
}

void UGEngineControllWidget::readSettings(QString file, QString group)
{
    settingsFileName = file;
    settingsGroupName = group;
    QSettings settings(settingsFileName, QSettings::IniFormat);
    settings.beginGroup(settingsGroupName);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
    if(componentsList) componentsList->readSettings(settingsFileName);
    if(drawEngine) drawEngine->readSettings(settingsFileName);
    if(componentLinks) componentLinks->readSettings(settingsFileName);
    if(images) images->readSettings(settingsFileName);
}

void UGEngineControllWidget::on_actionExit_triggered()
{
 QApplication::quit();
}
