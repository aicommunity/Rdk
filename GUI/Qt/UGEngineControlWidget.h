#ifndef UGENGINECONTROLWIDGET_H
#define UGENGINECONTROLWIDGET_H

#include <QMainWindow>
#include <QSettings>
#include <QMdiSubWindow>
#include <QKeyEvent>
#include <QString>
#include <QKeySequence>
#include <QVector>
#include <QHash>
#include <QPointer>

#include <rdk_application.h>

#include <functional>

#include "UVisualControllerMainWidget.h"
#include "UComponentsListWidget.h"
#include "UModernDiagramContainerWidget.h"
#include "UComponentLinksWidget.h"
#include "UBreadcrumbsWidget.h"
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
#include "UHelpWindow.h"
#include "UProjectDescriptionWindow.h"
#include "UComponentGuiService.h"
#include "UComponentGuiContext.h"
#include "UComponentGuiGridContainerWidget.h"

#ifndef RDK_DISABLE_EXT_GUI
#include "UVideoAnalyticsSimpleSettingsWidget.h"
#endif

namespace Ui {
class UGEngineControllWidget;
}

struct USubTabDescription
{
/// пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
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

// Размещение пользовательского виджета в UGEngineControlWidget
enum class UCustomWidgetPlacement
{
    Dock,
    Mdi
};

// Описатель пользовательского виджета, который может быть зарегистрирован
// в UGEngineControlWidget и создан по требованию из меню/toolbar.
struct UCustomWidgetDescriptor
{
    QString id;   // стабильный идентификатор
    QString title;

    std::function<UVisualControllerWidget*(RDK::UApplication*)> factory;

    UCustomWidgetPlacement placement = UCustomWidgetPlacement::Dock;
    Qt::DockWidgetArea defaultDockArea = Qt::RightDockWidgetArea;
    bool singleInstance = true;

    QString menuPath;     // например, "Window/NeuroModeler/Manipulator"
    QString toolbarGroup; // опционально: имя группы/toolbar
    QKeySequence shortcut;
};

/// UGEngineControllWidget class - пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
///
/// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ.

class UGEngineControlWidget : public UVisualControllerMainWidget
{
    Q_OBJECT

public:
    explicit UGEngineControlWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UGEngineControlWidget();

    ///пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    void showChannelsWidget(void);

    /// загрузка проекта извне (используется, например, пунктами меню и автозагрузкой)
    void loadProjectExternal(const QString &config_path);

    /// Open help window (public method for use by child widgets)
    void openHelpWindow();

    // Регистрация пользовательского виджета (дополнительного окна/панели),
    // который затем создаётся по требованию из меню/toolbar.
    void registerCustomWidget(const UCustomWidgetDescriptor &descriptor);

#ifndef RDK_DISABLE_EXT_GUI
    void setExternVideoAnalyticsSimpleWidget(UVideoAnalyticsSimpleSettingsWidget *externalWidget);
#endif
signals:
    void showSimpleSettings();

public slots:
    // settings
    void readSettings();
    void writeSettings();

    // Theme switching
    void switchToTheme(const QString& themeName);

    void showLinksForSingleComponent(QString componentName);
    void showLinksForTwoComponents(QString firstComponentName, QString secondComponentName);
    void switchLinksForTwoComponents(QString firstComponentName, QString secondComponentName);
    void openComponentGuiFromScheme(const UComponentGuiContext& context);


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
    void actionProjectDescription();
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


    /// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    void delImagesWidged(size_t index);
    void delImagesWidgetSlot(QObject* obj);

    /// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    void delWatchesWidgetSlot(QObject* obj);
    void delWatchesWidged(size_t index);

    void on_actionAbout_triggered();
    void on_actionUserGuide_triggered();

    void on_actionWatches_triggered();

    void on_actionImages_triggered();

    // обработчик нажатий на actions зарегистрированных пользовательских виджетов
    void handleCustomWidgetActionTriggered();

private:
    static const int kMaxRecentConfigs = 10;
    static const int kComponentGuiLayoutSchemaVersion = 1;

    // Helper methods
    void createThemeMenu();
    void updateThemeMenuState();
    void updateRecentConfigsMenu();
    void addToRecentConfigs(const QString& path);

    // data
    Ui::UGEngineControllWidget *ui;

    // Theme menu actions
    QAction* m_lightThemeAction;
    QAction* m_darkThemeAction;

    // widgets
    USettingsReaderWidget *settings;
    UComponentPropertyChanger *propertyChanger;
    UModernDiagramContainerWidget *modernDiagram;
    UBreadcrumbsWidget *breadcrumbsWidget;
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
    UProjectDescriptionWindow *projectDescriptionWindow;
    UClDescEditor *clDesc;
    QMainWindow *tcpServerControlWindow;
    UTcpServerControlWidget *tcpServerControlWidget;
    UAboutDialog *aboutDialog;
    UHelpWindow *helpWindow;
#ifndef RDK_DISABLE_EXT_GUI
    UVideoAnalyticsSimpleSettingsWidget *videoAnalyticsSimpleWidget;
#endif
    UCurlFtpClientTestWidget *curlFtpClientTestWidget;

    /// пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    std::vector<UImagesWidget*> imagesVector;

    /// пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    std::vector<UWatchTab*> watchesVector;


    /// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    RDK::UApplication *application;

    // зарегистрированные пользовательские виджеты
    QVector<UCustomWidgetDescriptor> customWidgets;
    // активные экземпляры по id; QPointer обнуляется при удалении виджета
    QHash<QString, QList<QPointer<UVisualControllerWidget>>> customWidgetInstances;
    UComponentGuiService m_componentGuiService;
    bool m_componentSpecialFormsEnabled = true;
    bool m_componentSpecialFormsMotionControlEnabled = true;
    bool m_componentSpecialFormsPulseLibEnabled = true;
    bool m_componentSpecialFormsBasicLibEnabled = true;
    bool m_componentSpecialFormsCvBasicLibEnabled = true;
    bool m_componentSpecialFormsHardwareLibEnabled = true;
    QHash<QString, QPointer<UComponentGuiGridContainerWidget>> m_componentGuiGrids;

    // служебный метод для создания/активации пользовательского виджета
    void createOrActivateCustomWidget(const QString &id);
    UComponentGuiGridContainerWidget* ensureComponentGuiGrid(const QString& gridId, int rows = 1, int cols = 1);
    void saveComponentGuiLayoutToXml(RDK::USerStorageXML &xml);
    void loadComponentGuiLayoutFromXml(RDK::USerStorageXML &xml);
    void writeComponentGuiSettings(QSettings& projectSettings);
    void readComponentGuiSettings(QSettings& projectSettings);
    QString hostModeToString(UComponentGuiHostMode mode) const;
    UComponentGuiHostMode hostModeFromString(const QString& mode) const;
    void showComponentGuiHostMenu(UVisualControllerWidget* widget, const QPoint& globalPos);
    bool resolveComponentGuiWidgetContext(UVisualControllerWidget* widget,
                                          UComponentGuiContext& context,
                                          UComponentGuiHostMode* mode = nullptr) const;
    void promptAndOpenComponentGuiGrid();

    // methods

    ///if chanelIndex == -1 пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    void startChannel(int chanelIndex);

    ///if chanelIndex == -1 пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    void pauseChannel(int chanelIndex);

    ///if chanelIndex == -1 пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    void resetChannel(int chanelIndex);

    ///if chanelIndex == -1 пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    void calcOneStepChannel(int chanelIndex);

    ///пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ UVisualControllerWidget
    void execDialogUVisualControllWidget(UVisualControllerWidget* widget);

    /// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    void addImagesWidged();

    /// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    void addWatchesWidged();

    /// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    /// пїЅ пїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅ Choose Storage Build Mode [N]
    /// пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ (пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ)
    void updateShemeClassesList();

    // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    virtual void AUpdateInterface(void);

    // пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    virtual void AClearInterface(void);

    // пїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    virtual void AAfterLoadProject(void);

    // пїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    virtual void ABeforeCloseProject(void);

    // пїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
    virtual void ABeforeReset(void);

    // пїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
    virtual void AAfterReset(void);

    // пїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    virtual void ABeforeCalculate(void);

    // пїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    virtual void AAfterCalculate(void);

    // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ xml
    virtual void ASaveParameters(RDK::USerStorageXML &xml);

    // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ xml
    virtual void ALoadParameters(RDK::USerStorageXML &xml);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QPoint m_componentGuiTabDragStartPos;
    int m_componentGuiTabDragIndex = -1;

};

/// пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ QMdiSubwindow пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
class SubWindowCloseIgnore: public QMdiSubWindow
{
public:
    explicit SubWindowCloseIgnore(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags()):QMdiSubWindow(parent, flags){}
protected:
    void closeEvent(QCloseEvent *event) override {event->ignore();}
    void keyPressEvent(QKeyEvent *event) override;
};

/*
/// пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ QMdiSubwindow пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
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
