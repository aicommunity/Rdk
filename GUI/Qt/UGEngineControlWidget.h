#ifndef UGENGINECONTROLWIDGET_H
#define UGENGINECONTROLWIDGET_H

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
#include "UWatch.h"
#include "UClDescEditor.h"
#include "UTcpServerControlWidget.h"
//////////////////////////
#include "UCurlFtpClientTestWidget.h"
#include "UAboutDialog.h"

#ifndef RDK_DISABLE_EXT_GUI
#include "UVideoAnalyticsSimpleSettingsWidget.h"
#endif

namespace Ui {
class UGEngineControllWidget;
}

struct USubTabDescription
{
/// ��� �������
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

/// UGEngineControllWidget class - ������� ���� ����������
///
/// �������� ��������� �� ��� ������ ���� � ��������� �� � ������� �������� � ������.

class UGEngineControlWidget : public UVisualControllerMainWidget
{
    Q_OBJECT

public:
    explicit UGEngineControlWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UGEngineControlWidget();

    ///���������� ������ � ������������� �������
    void showChannelsWidget(void);

    ///��������� ������ �� �������� ��������� (���� ������ �� ������������� �� ������ ����)
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
    void actionClDesc();

private slots:
    void on_mdiArea_destroyed(QObject *arg1);

    void closeEvent(QCloseEvent *event);

    void updateChannelsVisibility();


    /// ������� ������ ����������� ��������
    void delImagesWidged(size_t index);
    void delImagesWidgetSlot(QObject* obj);

    /// ������� ������ ����������� ��������
    void delWatchesWidgetSlot(QObject* obj);
    void delWatchesWidged(size_t index);

    void on_actionAbout_triggered();

    void on_actionWatches_triggered();

    void on_actionImages_triggered();

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
    QMainWindow *clDescWindow;
    QMainWindow *graphWindow;
    QMainWindow *profilingWindow;
    UCalculationChannelsWidget *channels;
    ULoggerWidget *logger;
    UCreateConfigurationWizardWidget *createConfigurationWizardWidget;
    UCreateTestWidget *createTestWidget;
    UStatusPanel *statusPanel;
    UGraphWidget *graphWindowWidget;
 //   UWatchFormWidget *watchFormWidget;
    UTableInfo *profilingWindowWidget;
    UWatch *watchWindow;
    UClDescEditor *clDesc;
    QMainWindow *tcpServerControlWindow;
    UTcpServerControlWidget *tcpServerControlWidget;
    UAboutDialog *aboutDialog;
#ifndef RDK_DISABLE_EXT_GUI
    UVideoAnalyticsSimpleSettingsWidget *videoAnalyticsSimpleWidget;
#endif
    UCurlFtpClientTestWidget *curlFtpClientTestWidget;

    /// ������ �������� ����������� ��������
    std::vector<UImagesWidget*> imagesVector;

    /// ������ �������� ����������� ��������
    std::vector<UWatchTab*> watchesVector;


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

    /// ��������� ����� ������ ����������� ��������
    void addImagesWidged();

    /// ��������� ����� ������ ����������� ��������
    void addWatchesWidged();

    /// ��������� ����� ������, ������ �������
    /// � ����� � ���� Choose Storage Build Mode [N]
    /// ����� ����� ����� ������ ������ (����������)
    void updateShemeClassesList();

    // ���������� ����������
    virtual void AUpdateInterface(void);

    // ������� ���������� � �������� ���������
    virtual void AClearInterface(void);

    // �����, ���������� ����� �������� �������
    virtual void AAfterLoadProject(void);

    // �����, ���������� ����� ��������� �������
    virtual void ABeforeCloseProject(void);

    // �����, ���������� ����� ������� ������
    virtual void ABeforeReset(void);

    // �����, ���������� ����� ������ ������
    virtual void AAfterReset(void);

    // �����, ���������� ����� ����� �������
    virtual void ABeforeCalculate(void);

    // �����, ���������� ����� ���� �������
    virtual void AAfterCalculate(void);

    // ��������� ��������� ���������� � xml
    virtual void ASaveParameters(RDK::USerStorageXML &xml);

    // ��������� ��������� ���������� �� xml
    virtual void ALoadParameters(RDK::USerStorageXML &xml);



};

/// �� ������������� QMdiSubwindow ��� ����������� ����� ������
class SubWindowCloseIgnore: public QMdiSubWindow
{
public:
    explicit SubWindowCloseIgnore(QWidget *parent = 0, Qt::WindowFlags flags = 0):QMdiSubWindow(parent, flags){}
protected:
    void closeEvent(QCloseEvent *event){event->ignore();}
};

/*
/// �� ������������� QMdiSubwindow ��� ����������� ����� ������
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


#endif // UGENGINECONTROLWIDGET_H
