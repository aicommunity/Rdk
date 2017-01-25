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

namespace Ui {
class UGEngineControllWidget;
}

/// UGEngineControllWidget class - ������� ���� ����������
///
/// �������� ��������� �� ��� ������ ���� � ��������� �� � ������� �������� � ������.

class UGEngineControllWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit UGEngineControllWidget(QWidget *parent = 0);
    virtual ~UGEngineControllWidget();

public slots:
    void showLinksForSingleComponent(QString componentName);
    void showLinksForTwoComponents(QString firstComponentName, QString secondComponentName);

    //settings
    void readSettings(QString file, QString group = "UGEngineControllWidget");
    void writeSettings(QString file, QString group);

    //actions:
    void actionComponentsControl();
    void actionChannelsControl();

    void actionLoadConfig();
    void actionCreateConfig();
    void actionSaveConfig();

    void actionReloadParameters();
    void actionStart();
    void actionPause();
    void actionReset();
    void actionStep();

    void actionImages();

signals:
    void readSettingsSignal(QString file);
/*
protected:
    void timerEvent(QTimerEvent *);*/

private slots:
    void on_actionExit_triggered();

private:
    //data
    Ui::UGEngineControllWidget *ui;

    QString settingsFileName;
    QString settingsGroupName;
    QString configFileName;

    /// �������� ��������� �������� ������
    RDK::URpcDecoderInternal rpcDecoder;

    /// �������� ������ ���������� ������
    RDK::URpcDispatcher rpcDispatcher;

    /// �������� ������ ����������
    RDK::UApplication application;

    /// ��������� ������ ����������� �������
    RDK::UServerControl serverControl;
    RDK::URpcDecoderCommon rpcDecoderCommon;

    /// ��������� ������ ����������� �������
    UEngineControlQt engineControl;

    /// �������� ������ �������
    RDK::UProject project;

    //widgets
    UComponentsListWidget *componentsList;
    UDrawEngineWidget *drawEngine;
    UComponentLinksWidget *componentLinks;
    UImagesWidget *images;
    QMainWindow *imagesWindow;
    UCalculationChannelsWidget *channels;
    ULoggerWidget *logger;
    UCreateConfigurationWizardWidget *createConfigurationWizardWidget;

    //methods
    void initGraphicalEngine();

    ///if chanelIndex == -1 ��������� ��� ������ �������
    void startChannel(int chanelIndex);

    ///if chanelIndex == -1 ������������� ��� ������ �������
    void pauseChannel(int chanelIndex);

    ///if chanelIndex == -1 ���������� ��� ������ �������
    void resetChannel(int chanelIndex);

    ///if chanelIndex == -1 ������ ��� �� ��� ������ �������
    void calcOneStepChannel(int chanelIndex);

    ///������� � �������� ���������� ���� ��� ���������� UVisualControllerWidget
    void execDialogUVisualControllWidget(UVisualControllerWidget* widget);
};

/// �� ������������� QMdiSubwindow ��� ����������� ����� ������
class SubWindowCloseIgnore: public QMdiSubWindow
{
public:
    explicit SubWindowCloseIgnore(QWidget *parent = 0, Qt::WindowFlags flags = 0):QMdiSubWindow(parent, flags){}
protected:
    void closeEvent(QCloseEvent *event){event->ignore();}
};

/// �������� ����� ��� �������� ���������� ������ �� ������ �������
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
