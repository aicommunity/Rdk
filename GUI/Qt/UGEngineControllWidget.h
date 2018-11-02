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

/// UGEngineControllWidget class - главное окно интерфейса
///
/// —одержит указатели на все другие окна и св€зывает их с помощью сигналов и слотов.

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

    /// Ёкзепл€р класса приложени€
    RDK::UApplication *application;

    // methods

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

#endif // UGENGINECONTROLLWIDGET_H
