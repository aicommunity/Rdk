#ifndef UGENGINECONTROLLWIDGET_H
#define UGENGINECONTROLLWIDGET_H

#include <QMainWindow>
#include <QSettings>
#include <QMdiSubWindow>

#include <rdk_application.h>

#include "UVisualControllerWidget.h"
#include "UComponentsListWidget.h"
#include "UDrawEngineWidget.h"
#include "UComponentLinksWidget.h"
#include "UImagesWidget.h"
#include "UCalculationChannelsWidget.h"
#include "UEngineControlQt.h"
#include "ULoggerWidget.h"
#include "UCreateConfigurationWizardWidget.h"
#include "UCreateTestWidget.h"

#include "UComponentPropertyChanger.h"

namespace Ui {
class UGEngineControllWidget;
}

/// UGEngineControllWidget class - главное окно интерфейса
///
/// —одержит указатели на все другие окна и св€зывает их с помощью сигналов и слотов.

class UGEngineControllWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit UGEngineControllWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UGEngineControllWidget();

public slots:
    void showLinksForSingleComponent(QString componentName);
    void showLinksForTwoComponents(QString firstComponentName, QString secondComponentName);

    // settings
    void readSettings(QString file, QString group = "UGEngineControllWidget");
    void writeSettings(QString file, QString group = "UGEngineControllWidget");

    // actions:

    // file menu
    void actionLoadConfig();
    void actionCreateConfig();
    void actionSaveConfig();
    void actionExit();

    // calculate menu
    void actionReloadParameters();
    void actionStart();
    void actionPause();
    void actionReset();
    void actionStep();

    // window menu
    void actionImages();
    void actionComponentsControl();
    void actionChannelsControl();
    void actionLogger();
    void actionTestCreator();
/*
signals:
    void readSettingsSignal(QString file);

protected:
    void timerEvent(QTimerEvent *);*/

private:
    // data
    Ui::UGEngineControllWidget *ui;

    QString settingsFileName;
    QString settingsGroupName;
    QString configFileName;

    // widgets
    UComponentPropertyChanger *propertyChanger;
    UDrawEngineWidget *drawEngine;
    UComponentLinksWidget *componentLinks;
    UImagesWidget *images;
    QMainWindow *imagesWindow;
    UCalculationChannelsWidget *channels;
    ULoggerWidget *logger;
    UCreateConfigurationWizardWidget *createConfigurationWizardWidget;
    UCreateTestWidget *createTestWidget;

    /// Ёкзепл€р класса приложени€
    RDK::UApplication *application;

    // methods

    //void initGraphicalEngine();

    ///if chanelIndex == -1 запускает все каналы расчета
    void startChannel(int chanelIndex);

    ///if chanelIndex == -1 останавливает все каналы расчета
    void pauseChannel(int chanelIndex);

    ///if chanelIndex == -1 сбрасывает все каналы расчета
    void resetChannel(int chanelIndex);

    ///if chanelIndex == -1 делает шаг на все каналы расчета
    void calcOneStepChannel(int chanelIndex);

    ///—оздает и вызывает диалоговое окно дл€ наследника UVisualControllerWidget
    void execDialogUVisualControllWidget(UVisualControllerWidget* widget);
};

/// Ќе закрывающийс€ QMdiSubwindow дл€ отображени€ схемы модели
class SubWindowCloseIgnore: public QMdiSubWindow
{
public:
    explicit SubWindowCloseIgnore(QWidget *parent = 0, Qt::WindowFlags flags = 0):QMdiSubWindow(parent, flags){}
protected:
    void closeEvent(QCloseEvent *event){event->ignore();}
};

/// тестовый класс дл€ проверки обновлени€ окошек из других потоков
class ThreadGo: public QObject
{
    Q_OBJECT

public:
    explicit ThreadGo(UVisualControllerWidget *obj = NULL, QObject *parent = NULL) : QObject(parent)
    {
        ob = obj;
    }
        ~ThreadGo(){ob = NULL;}
public slots:
    void startThread()
    {
        if(ob)
        ob->UpdateInterface(true);
        emit testFinished();

    }
signals:
    void testFinished();


private:
    UVisualControllerWidget *ob;
};

#endif // UGENGINECONTROLLWIDGET_H
