#ifndef UGENGINECONTROLLWIDGET_H
#define UGENGINECONTROLLWIDGET_H

#include <QMainWindow>
#include <QSettings>
#include <QMdiSubWindow>

#include <rdk_application.h>

#include "UVisualControllerMainWidget.h"
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
#include "UTableInfo.h"
#include "UWatchFormWidget.h"
#include"UWatch.h"
#include "UTcpServerControlWidget.h"
//////////////////////////
#include "UCurlFtpClientTestWidget.h"

#ifndef RDK_DISABLE_EXT_GUI
#include "UVideoAnalyticsSimpleSettingsWidget.h"
#endif

namespace Ui {
class UGEngineControllWidget;
}

struct USubTabDescription
{
/// Имя вкладки
QString Name;

QMdiSubWindow *SubWindow;

USubTabDescription(void)
    : SubWindow(NULL)
{}

};

struct USubTabDescriptionImages: public USubTabDescription
{
UImagesWidget* Images;

USubTabDescriptionImages(void)
    : USubTabDescription(),Images(NULL)
{};
};

struct USubTabDescriptionWatches: public USubTabDescription
{
UGraphWidget* Watches;

USubTabDescriptionWatches(void)
    : USubTabDescription(),Watches(NULL)
{};
};

/// UGEngineControllWidget class - главное окно интерфейса
///
/// Содержит указатели на все другие окна и связывает их с помощью сигналов и слотов.

class UGEngineControllWidget : public UVisualControllerMainWidget
{
    Q_OBJECT

public:
    explicit UGEngineControllWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UGEngineControllWidget();

    ///Показывает виджет с переключением каналов
    void showChannelsWidget(void);

    ///Загрузить проект из внешнего источника (пока только из конфигуратора на основе СУБД)
    void loadProjectExternal(const QString &config_path);

#ifndef RDK_DISABLE_EXT_GUI
    void setExternVideoAnalyticsSimpleWidget(UVideoAnalyticsSimpleSettingsWidget *externalWidget);
#endif
signals:
    void showSimpleSettings();

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
    void actionCreateSimple();
    void actionSaveConfig();
    void actionCloseConfig();
    void actionCopyConfig();
    void actionAutoCopyConfig();
    void actionRenameConfig();
    void actionExit();
    void actionConfigOptions();
    void actionCreateSaveMockLibs();
    void actionBuildMode1();
    void actionBuildMode2();
    void actionBuildMode3();


    // calculate menu
    void actionReloadParameters();
    void actionStart();
    void actionPause();
    void actionReset();
    void actionStep();
    void actionRunNSteps();

    // window menu
    void actionImages();
    void actionNewImages();
    void actionComponentsControl();
    void actionChannelsControl();
    void actionLogger();
    void actionTestCreator();
    void actionWatchWindow();
    void actionNewWatches();
    void actionProfiling();
    void actionTcpServer();
#ifndef RDK_DISABLE_EXT_GUI
    void actionVASimpleSettings();
#endif
    void actionFtpTest();

private slots:
    void on_mdiArea_destroyed(QObject *arg1);

    void closeEvent(QCloseEvent *event);

    void updateChannelsVisibility();

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
    QMainWindow *graphWindow;
    QMainWindow *profilingWindow;
    UCalculationChannelsWidget *channels;
    ULoggerWidget *logger;
    UCreateConfigurationWizardWidget *createConfigurationWizardWidget;
    UCreateTestWidget *createTestWidget;
    UStatusPanel *statusPanel;
    UGraphWidget *graphWindowWidget;
    UWatchFormWidget *watchFormWidget;
    UTableInfo *profilingWindowWidget;
    UWatch *watchWindow;
    QMainWindow *tcpServerControlWindow;
    UTcpServerControlWidget *tcpServerControlWidget;
#ifndef RDK_DISABLE_EXT_GUI
    UVideoAnalyticsSimpleSettingsWidget *videoAnalyticsSimpleWidget;
#endif
    UCurlFtpClientTestWidget *curlFtpClientTestWidget;

    /// Массив виджетов отображения картинок
    std::vector<USubTabDescriptionImages> imagesVector;

    /// Массив виджетов отображения графиков
    std::vector<USubTabDescriptionWatches> watchesVector;


    /// Экзепляр класса приложения
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

    ///Создает и вызывает диалоговое окно для наследника UVisualControllerWidget
    void execDialogUVisualControllWidget(UVisualControllerWidget* widget);

    /// Добавляет новый виджет отображения картинок
    void addImagesWidged();

    /// Удаляет виджет отображения картинок
    void delImagesWidged(size_t index);

    /// Добавляет новый виджет отображения графиков
    void addWatchesWidged();

    /// Удаляет виджет отображения графиков
    void delWatchesWidged(size_t index);

    /// Обновляет схему модели, список классов
    /// и текст в меню Choose Storage Build Mode [N]
    /// Вызов после смены режима сборки (пересборки)
    void updateShemeClassesList();

    // Обновление интерфейса
    virtual void AUpdateInterface(void);

    // Возврат интерфейса в исходное состояние
    virtual void AClearInterface(void);

    // Метод, вызываемый после загрузки проекта
    virtual void AAfterLoadProject(void);

    // Метод, вызываемый перед закрытием проекта
    virtual void ABeforeCloseProject(void);

    // Метод, вызываемый перед сбросом модели
    virtual void ABeforeReset(void);

    // Метод, вызываемый после сброса модели
    virtual void AAfterReset(void);

    // Метод, вызываемый перед шагом расчета
    virtual void ABeforeCalculate(void);

    // Метод, вызываемый после шага расчета
    virtual void AAfterCalculate(void);

    // Сохраняет параметры интерфейса в xml
    virtual void ASaveParameters(RDK::USerStorageXML &xml);

    // Загружает параметры интерфейса из xml
    virtual void ALoadParameters(RDK::USerStorageXML &xml);



};

/// Не закрывающийся QMdiSubwindow для отображения схемы модели
class SubWindowCloseIgnore: public QMdiSubWindow
{
public:
    explicit SubWindowCloseIgnore(QWidget *parent = 0, Qt::WindowFlags flags = 0):QMdiSubWindow(parent, flags){}
protected:
    void closeEvent(QCloseEvent *event){event->ignore();}
};

/*
/// Не закрывающийся QMdiSubwindow для отображения схемы модели
class SubWindowCloseExt: public QMdiSubWindow
{
public:
    explicit SubWindowCloseIgnore(UGEngineControllWidget* owner, QWidget *parent = 0, Qt::WindowFlags flags = 0):QMdiSubWindow(parent, flags), Owner(owner){}
protected:
    void closeEvent(QCloseEvent *event)
    {
     if(Owner)
      Owner->
    }

private:
UGEngineControllWidget* Owner;
};*/


#endif // UGENGINECONTROLLWIDGET_H
