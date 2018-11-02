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
#include "UComponentPropertyChanger.h"
#include "UCreateConfigurationWizardWidget.h"
#include "UCreateTestWidget.h"
#include "UStatusPanel.h"
#include "USettingsReaderWidget.h"
#include "UGraphWidget.h"
//////////////////////////
#include "UVideoAnalyticsSimpleSettingsWidget.h"

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
    explicit UGEngineControllWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UGEngineControllWidget();

    void setExternVideoAnalyticsSimpleWidget(UVideoAnalyticsSimpleSettingsWidget *externalWidget);

public slots:
    // settings
    void readSettings();
    void writeSettings();

    void showLinksForSingleComponent(QString componentName);
    void showLinksForTwoComponents(QString firstComponentName, QString secondComponentName);
    void switchLinksForTwoComponents(QString firstComponentName, QString secondComponentName);


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
    void actionRunNSteps();

    // window menu
    void actionImages();
    void actionComponentsControl();
    void actionChannelsControl();
    void actionLogger();
    void actionTestCreator();
    void actionWatchWindow();
    void actionVASimpleSettings();

private:
    // data
    Ui::UGEngineControllWidget *ui;

    // widgets
    USettingsReaderWidget *settings;
    UComponentPropertyChanger *propertyChanger;
    UDrawEngineWidget *drawEngine;
    UComponentLinksWidget *componentLinks;
    UImagesWidget *images;
    QMainWindow *imagesWindow;
    UCalculationChannelsWidget *channels;
    ULoggerWidget *logger;
    UCreateConfigurationWizardWidget *createConfigurationWizardWidget;
    UCreateTestWidget *createTestWidget;
    UStatusPanel *statusPanel;
    UGraphWidget *graphWindowWidget;
    UVideoAnalyticsSimpleSettingsWidget *videoAnalyticsSimpleWidget;

    /// �������� ������ ����������
    RDK::UApplication *application;

    // methods

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

#endif // UGENGINECONTROLLWIDGET_H
