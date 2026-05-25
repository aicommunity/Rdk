#include "UGEngineControlWidget.h"
#include "ui_UGEngineControllWidget.h"
#include "UStyleManager.h"
#include "UComponentGuiBootstrap.h"
#include "UComponentGuiDndPayload.h"


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
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QTimer>
#include <QInputDialog>
#include <QLineEdit>
#include <QActionGroup>
#include <QTabBar>
#include <QKeyEvent>
#include <QEvent>
#include <QMenuBar>
#include <QMenu>
#include <QProcessEnvironment>
#include <algorithm>
#include <QCursor>
#include <QSet>
#include <QToolButton>
#include <QDialog>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>

/*int heheheCounter = 0;
void hehehe(){qDebug("hehehe %d", ++heheheCounter);}*/

UGEngineControlWidget::UGEngineControlWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerMainWidget(parent,app),
    ui(new Ui::UGEngineControllWidget),
    m_componentGuiService(app)
{
    ui->setupUi(this);
    setAccessibleName("UGEngineControllWidget");

    application = app;

    if(application == NULL)
      QApplication::exit(-1);

    qApp->installEventFilter(this);

    m_componentGuiService.setHostMainWindow(this);
    setDockOptions(QMainWindow::AllowNestedDocks |
                   QMainWindow::AllowTabbedDocks |
                   QMainWindow::GroupedDragging);

    QString caption_line=(application->GetProgramName()+" ").c_str();
    caption_line += QCoreApplication::applicationVersion();
    if(!application->GetUserName().empty())
    {
     caption_line=caption_line+" ["+application->GetUserName().c_str();
     if(application->GetUserId()>=0)
      caption_line=caption_line+":"+RDK::sntoa(application->GetUserId()).c_str();
     caption_line=caption_line+="]";
    }
     this->setWindowTitle(caption_line);

    settings = NULL;
    propertyChanger = NULL;
    modernDiagram = NULL;
    componentLinks = NULL;
    breadcrumbsWidget = NULL;
    images = NULL;
    imagesWindow = NULL;
    channels = NULL;
    createConfigurationWizardWidget = NULL;
    createTestWidget = NULL;
    statusPanel = NULL;
#ifndef RDK_DISABLE_EXT_GUI
    videoAnalyticsSimpleWidget=NULL;
#endif
    graphWindowWidget=NULL;
    graphWindow=NULL;
    profilingWindow=NULL;
    profilingWindowWidget=NULL;
 //   watchFormWidget=NULL;
    watchWindow = NULL;
    projectDescriptionWindow = NULL;
    clDescWindow = NULL;
    tcpServerControlWindow=NULL;
    tcpServerControlWidget=0;
    curlFtpClientTestWidget=NULL;

    // Initialize theme menu actions
    m_lightThemeAction = NULL;
    m_darkThemeAction = NULL;

    settings = new USettingsReaderWidget(this);
    connect(settings, SIGNAL(readSetting()) , this, SLOT(readSettings()));
    connect(settings, SIGNAL(writeSetting()), this, SLOT(writeSettings()));

    propertyChanger = new UComponentPropertyChanger(this, application);
    ui->dockWidgetComponentsList->setWidget(propertyChanger);

    // Функция для обновления стилей QTabBar в QMdiArea
    // Используется при переключении темы и при создании новых subWindow
    auto updateMdiAreaTabBarStyles = [this]() {
        if (ui && ui->mdiArea)
        {
            QTabBar* tabBar = ui->mdiArea->findChild<QTabBar*>();
            if (tabBar)
            {
                // Очищаем локальные стили, чтобы применились глобальные из QSS
                tabBar->setStyleSheet("");
                tabBar->style()->unpolish(tabBar);
                tabBar->style()->polish(tabBar);
                tabBar->update();
            }
        }
    };

    // Обновляем стили при активации subWindow (когда QTabBar может быть пересоздан)
    connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, [updateMdiAreaTabBarStyles](QMdiSubWindow* window) {
        Q_UNUSED(window);
        QTimer::singleShot(0, updateMdiAreaTabBarStyles);
    });
    ui->mdiArea->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->mdiArea->setAcceptDrops(false);
    connect(ui->mdiArea, &QMdiArea::customContextMenuRequested, this, [this](const QPoint& pos) {
        QMdiSubWindow* sub = nullptr;
        const QList<QMdiSubWindow*> subwindows = ui->mdiArea->subWindowList(QMdiArea::StackingOrder);
        for(auto it = subwindows.crbegin(); it != subwindows.crend(); ++it)
        {
            if(*it && (*it)->geometry().contains(pos))
            {
                sub = *it;
                break;
            }
        }
        if(!sub)
            sub = ui->mdiArea->activeSubWindow();
        if(!sub)
            return;

        UVisualControllerWidget* widget = qobject_cast<UVisualControllerWidget*>(sub->widget());
        if(!widget)
            return;

        showComponentGuiHostMenu(widget, QCursor::pos());
    });
    // Discoverable tab-level menu for component GUI tabs in tabbed MDI mode.
    connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, [this](QMdiSubWindow* window) {
        Q_UNUSED(window);
        QTimer::singleShot(0, this, [this]() {
            const QList<QMdiSubWindow*> subWindows = ui->mdiArea->subWindowList(QMdiArea::CreationOrder);
            for(QMdiSubWindow* sub : subWindows)
            {
                if(!sub)
                    continue;
                sub->setProperty("componentGuiDragSource", true);
                sub->installEventFilter(this);
                if(QWidget* hosted = sub->widget())
                    installDragFilterRecursively(hosted);
            }
            QTabBar* tabBar = ui->mdiArea->findChild<QTabBar*>();
            if(!tabBar)
                return;
            tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
            tabBar->setAcceptDrops(true);
            tabBar->installEventFilter(this);
            QObject::connect(tabBar, &QTabBar::customContextMenuRequested, this, [this, tabBar](const QPoint& pos) {
                const int index = tabBar->tabAt(pos);
                if(index < 0)
                    return;
                const QList<QMdiSubWindow*> list = ui->mdiArea->subWindowList(QMdiArea::CreationOrder);
                if(index >= list.size() || !list[index])
                    return;
                UVisualControllerWidget* widget = qobject_cast<UVisualControllerWidget*>(list[index]->widget());
                if(!widget)
                    return;
                showComponentGuiHostMenu(widget, tabBar->mapToGlobal(pos));
            }, Qt::UniqueConnection);
        });
    });

    // Создаем breadcrumbs виджет
    breadcrumbsWidget = new UBreadcrumbsWidget(this);
    breadcrumbsWidget->setMinimumHeight(30);
    breadcrumbsWidget->setMaximumHeight(35);
    // Добавляем breadcrumbsWidget в layout перед mdiArea
    ui->verticalLayout->insertWidget(0, breadcrumbsWidget);

    // Создаем современную диаграмму
    modernDiagram = new UModernDiagramContainerWidget(this, application);
    qRegisterMetaType<UComponentGuiContext>("UComponentGuiContext");
    QMdiSubWindow *modernDiagramSbWindow = new SubWindowCloseIgnore(ui->mdiArea, Qt::SubWindow);
    modernDiagramSbWindow->setWidget(modernDiagram);
    modernDiagramSbWindow->setWindowTitle("Scheme");
    modernDiagramSbWindow->show();
    modernDiagramSbWindow->showMaximized();

    // Настройка синхронизации навигации между breadcrumbs, ComponentsList и Diagram
    // Breadcrumbs -> ComponentsList + Diagram
    connect(breadcrumbsWidget, SIGNAL(componentPathSelected(QString)),
            propertyChanger->componentsList, SLOT(componentSelectedFromScheme(QString)));
    connect(breadcrumbsWidget, SIGNAL(componentPathSelected(QString)),
            modernDiagram, SLOT(componentSingleClick(QString)));

    // ComponentsList -> Breadcrumbs + Diagram
    connect(propertyChanger->componentsList, SIGNAL(componentSelected(QString)),
            breadcrumbsWidget, SLOT(updateBreadcrumbs(QString)));

    // Diagram -> Breadcrumbs + ComponentsList
    connect(modernDiagram, SIGNAL(componentSelectedFromScheme(QString)),
            breadcrumbsWidget, SLOT(updateBreadcrumbs(QString)));


    // связывание схемы модели и списка отображения компонентов модели
    //  схема -> список
    connect(propertyChanger->componentsList, SIGNAL(componentDoubleClick(QString)),
            modernDiagram, SLOT(componentDoubleClick(QString)));
    connect(propertyChanger->componentsList, SIGNAL(componentSelected(QString)),
            modernDiagram, SLOT(componentSingleClick(QString)));
    connect(propertyChanger->componentsList, SIGNAL(updateScheme(bool)),
            modernDiagram, SLOT(updateScheme(bool)));
    connect(propertyChanger->componentsList, &UComponentsListWidgetModern::openComponentGuiRequested,
            this, &UGEngineControlWidget::openComponentGuiFromScheme);

    //  список -> схема
    connect(modernDiagram, SIGNAL(componentSelectedFromScheme(QString)),
            propertyChanger->componentsList, SLOT(componentSelectedFromScheme(QString)));
    connect(modernDiagram, SIGNAL(componentDoubleClickFromScheme(QString)),
            propertyChanger->componentsList, SLOT(componentDoubleClickFromScheme(QString)));
    connect(modernDiagram, SIGNAL(componentStapBackFromScheme()),
            propertyChanger->componentsList, SLOT(componentStapBackFromScheme()));
    connect(modernDiagram, SIGNAL(updateComponentsListFromScheme()),
            propertyChanger->componentsList, SLOT(updateComponentsListFromScheme()));

    componentLinks = new UComponentLinksWidget(this, application);
    componentLinks->hide();

    // связывание схемы модели и окна отображения связей
    //  связи -> схема
    connect(componentLinks, SIGNAL(updateScheme(bool)), modernDiagram, SLOT(updateScheme(bool)));

    //  схема -> связи
    // обнако слоты находятся в окне главного интерфейса, так как необходимо сначала создать
    // диалоговые окна (QDialog) для отображения виджета связей
    connect(modernDiagram, SIGNAL(viewLinksFromScheme(QString)), this, SLOT(showLinksForSingleComponent(QString)));
    connect(modernDiagram, SIGNAL(createLinksFromScheme(QString,QString)), this, SLOT(showLinksForTwoComponents(QString,QString)));
    connect(modernDiagram, SIGNAL(switchLinksFromScheme(QString,QString)), this, SLOT(switchLinksForTwoComponents(QString,QString)));
    connect(modernDiagram, &UModernDiagramContainerWidget::openComponentGuiFromScheme,
            this, &UGEngineControlWidget::openComponentGuiFromScheme);
    connect(modernDiagram, SIGNAL(openProjectDescriptionRequested()), this, SLOT(actionProjectDescription()));

    images = new UImagesWidget(this, application);
    images->hide();

    channels = new UCalculationChannelsWidget(this, application);
    QRect rect=channels->geometry();
    rect.setWidth(50);
    channels->setGeometry(rect);
    ui->dockWidgetChannels->setWidget(channels);
    ui->dockWidgetChannels->hide();

    //channels menu actions:
    connect(ui->actionAddCh, SIGNAL(triggered(bool)), channels, SLOT(actionAddChannel()));
    connect(ui->actionInsertCh, SIGNAL(triggered(bool)), channels, SLOT(actionInsertChannel()));
    connect(ui->actionDeleteSelectedCh, SIGNAL(triggered(bool)), channels, SLOT(actionDeleteSelectedChannel()));
    connect(ui->actionCloneCh, SIGNAL(triggered(bool)), channels, SLOT(actionCloneChannel()));
    connect(ui->actionStartCh, SIGNAL(triggered(bool)), channels, SLOT(actionStartChannel()));
    connect(ui->actionPauseCh, SIGNAL(triggered(bool)), channels, SLOT(actionPauseChannel()));
    connect(ui->actionResetCh, SIGNAL(triggered(bool)), channels, SLOT(actionResetChannel()));

    connect(channels, SIGNAL(updateVisibility()), this, SLOT(updateChannelsVisibility()));


    logger = new ULoggerWidget(this, application);
    ui->dockWidgetLoger->setWidget(logger);

    /*
    graphWindowWidget = new UGraphWidget(this, application);
//    ui->dockWidgetGraph->setWidget(graphWindowWidget);
    graphWindowWidget->setWindowTitle("Graph");
       graphWindowWidget->hide();
    ui->dockWidgetGraph->hide();
    */

 //   watchFormWidget= new UWatchFormWidget(this, application);
 //   watchFormWidget->setWindowTitle("Watches");
 //   watchFormWidget->hide();

    watchWindow = new UWatch(this);
    watchWindow->setWindowTitle("Watch window");
    watchWindow->hide();

    // hide profiling widget by default
    profilingWindowWidget = new UTableInfo(this, application);
    ui->dockWidgetProfiling->setWidget(profilingWindowWidget);
    profilingWindowWidget->setWindowTitle("Profiling");
    ui->dockWidgetProfiling->hide();

    createConfigurationWizardWidget=new UCreateConfigurationWizardWidget(this, application);

    createTestWidget = new UCreateTestWidget(this, application);
    createTestWidget->hide();

    tcpServerControlWidget = new UTcpServerControlWidget(this, application);
    tcpServerControlWidget->hide();

    statusPanel = new UStatusPanel(this, application);
    ui->statusBar->addWidget(statusPanel, 1);

    curlFtpClientTestWidget = new UCurlFtpClientTestWidget(NULL, application);
    curlFtpClientTestWidget->hide();

    connect(statusPanel, SIGNAL(saveConfig()), this, SLOT(actionSaveConfig()));
    connect(statusPanel, SIGNAL(setPropertyUpdateInterval(long)),
            propertyChanger->componentsList, SLOT(setUpdateInterval(long)));

    // GUI actions:

    // file menu actions:
    connect(ui->actionCreateConfig, SIGNAL(triggered(bool)), this, SLOT(actionCreateConfig()));
    connect(ui->actionCreateSimple, SIGNAL(triggered(bool)), this, SLOT(actionCreateSimple()));
    connect(ui->actionLoadConfig, SIGNAL(triggered(bool)), this, SLOT(actionLoadConfig()));
    connect(ui->actionSaveConfig, SIGNAL(triggered(bool)), this, SLOT(actionSaveConfig()));
    connect(ui->actionCloseConfig, SIGNAL(triggered(bool)), this, SLOT(actionCloseConfig()));
    connect(ui->actionCopyConfig, SIGNAL(triggered(bool)), this, SLOT(actionCopyConfig()));
    connect(ui->actionAutocopyConfig, SIGNAL(triggered(bool)), this, SLOT(actionAutoCopyConfig()));
    connect(ui->actionRenameConfig, SIGNAL(triggered(bool)), this, SLOT(actionRenameConfig()));

    connect(ui->actionConfigOptions, SIGNAL(triggered(bool)), this, SLOT(actionConfigOptions()));

    updateShemeClassesList();
    connect(ui->actionBuildMode1,  SIGNAL(triggered(bool)), this, SLOT(actionBuildMode1()));
    connect(ui->actionBuildMode2,  SIGNAL(triggered(bool)), this, SLOT(actionBuildMode2()));
    connect(ui->actionBuildMode3,  SIGNAL(triggered(bool)), this, SLOT(actionBuildMode3()));

    connect(ui->actionCreateSaveMockLibs,  SIGNAL(triggered(bool)), this, SLOT(actionCreateSaveMockLibs()));

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
    connect(ui->actionProjectDescription, SIGNAL(triggered(bool)), this, SLOT(actionProjectDescription()));
    connect(ui->actionProfiling, SIGNAL(triggered(bool)), this, SLOT(actionProfiling()));
#ifndef RDK_DISABLE_EXT_GUI
    connect(ui->actionVASimpleSettings, SIGNAL(triggered(bool)), this, SIGNAL(showSimpleSettings()));
    connect(ui->actionVASimpleSettings, SIGNAL(triggered(bool)), this, SLOT(actionVASimpleSettings()));
#endif
    connect(ui->actionTcpServer, SIGNAL(triggered(bool)), this, SLOT(actionTcpServer()));
    connect(ui->actionFtpTest, SIGNAL(triggered(bool)), this, SLOT(actionFtpTest()));
    //connect(ui->action, SIGNAL(triggered(bool)), this, SLOT(action)));


    clDesc = new UClDescEditor(this, application);
    clDesc->hide();

    connect(ui->actionClDesc, SIGNAL(triggered(bool)), this, SLOT(actionClDesc()));

    // Theme switcher menu
    createThemeMenu();
    QAction* componentGuiHostAction = ui->menuWindow->addAction(tr("Component GUI Tab Host..."));
    connect(componentGuiHostAction, &QAction::triggered, this, [this]() { promptAndOpenComponentGuiTabHost(); });
    QAction* componentGuiSecondaryHostAction = ui->menuWindow->addAction(tr("Component GUI Secondary Host..."));
    connect(componentGuiSecondaryHostAction, &QAction::triggered, this, &UGEngineControlWidget::showComponentGuiSecondaryHostWindow);

    updateRecentConfigsMenu();

    // Исправление проблемы с кликами в меню File
    // На Linux нативное меню может блокировать клики, поэтому отключаем его
    // и убеждаемся, что меню правильно позиционировано
    if (ui->menuBar) {
        ui->menuBar->setNativeMenuBar(false);

        // Убеждаемся, что меню видимо и правильно позиционировано
        ui->menuBar->setVisible(true);
        ui->menuBar->raise(); // Поднимаем меню наверх z-order

        // Диагностика: проверяем виджеты в области меню
        QTimer::singleShot(100, this, [this]() {
            if (ui->menuBar) {
                QRect menuBarRect = ui->menuBar->geometry();
                QPoint globalTopLeft = ui->menuBar->mapToGlobal(menuBarRect.topLeft());

                // Проверяем несколько точек в области меню File (первый пункт)
                for (int x = 0; x < 100 && x < menuBarRect.width(); x += 20) {
                    QPoint testPoint = globalTopLeft + QPoint(x, menuBarRect.height() / 2);
                    QWidget* widget = QApplication::widgetAt(testPoint);

                    if (widget && widget != ui->menuBar && !ui->menuBar->isAncestorOf(widget)) {
                        qWarning() << "MenuBar: Potential blocking widget at position" << testPoint
                                   << ":" << widget->objectName() << widget->metaObject()->className();
                        // Если найден блокирующий виджет, поднимаем меню еще выше
                        ui->menuBar->raise();
                    }
                }
            }
        });
    }

    readSettings();

    aboutDialog = 0;
    helpWindow = 0;
}

bool UGEngineControlWidget::eventFilter(QObject* watched, QEvent* event)
{
    if(event && event->type() == QEvent::Resize && watched && watched->property("componentGuiQuickActionsHost").toBool())
    {
        UVisualControllerWidget* host = qobject_cast<UVisualControllerWidget*>(watched);
        if(host)
            positionComponentGuiQuickActions(host);
    }
    return UVisualControllerMainWidget::eventFilter(watched, event);
}



UGEngineControlWidget::~UGEngineControlWidget()
{
    application->UnInit();
    delete ui;
}


#ifndef RDK_DISABLE_EXT_GUI
void UGEngineControllWidget::setExternVideoAnalyticsSimpleWidget(UVideoAnalyticsSimpleSettingsWidget *externalWidget)
{
    if(externalWidget!=NULL)
    {
        videoAnalyticsSimpleWidget = externalWidget;
    }
}
#endif

void UGEngineControlWidget::showLinksForSingleComponent(QString componentName)
{
    componentLinks->initWidget(componentName);
    execDialogUVisualControllWidget(componentLinks);
}

void UGEngineControlWidget::showLinksForTwoComponents(QString firstComponentName, QString secondComponentName)
{
    componentLinks->initWidget(firstComponentName, secondComponentName);
    execDialogUVisualControllWidget(componentLinks);
}

void UGEngineControlWidget::switchLinksForTwoComponents(QString firstComponentName, QString secondComponentName)
{
    componentLinks->initWidget(firstComponentName, secondComponentName, 3);
    execDialogUVisualControllWidget(componentLinks);
}

void UGEngineControlWidget::openComponentGuiFromScheme(const UComponentGuiContext& context)
{
    if(!m_componentSpecialFormsEnabled)
    {
        QMessageBox::information(this, "Component GUI", "Component special forms are disabled by feature flag.");
        return;
    }

    UComponentGuiHostMode savedMode = UComponentGuiHostMode::Mdi;
    QString savedContainerId;
    const bool hasSavedPlacement = m_componentGuiService.tryGetHostPlacementByContext(context, savedMode, savedContainerId);

    UVisualControllerWidget* widget = m_componentGuiService.createOrActivate(this, context);
    if(!widget)
    {
        QMessageBox::information(this, "Component GUI", "No GUI form is registered for this component class.");
        return;
    }
    ensureComponentGuiQuickActionsInstalled(widget);

    if(hasSavedPlacement)
    {
        if(savedMode == UComponentGuiHostMode::SecondaryDock)
            moveContextToSecondaryHost(context);
        else if(savedMode == UComponentGuiHostMode::TabHost && !savedContainerId.isEmpty())
            moveContextToTabHost(context, savedContainerId);
    }
}

// file menu actions

void UGEngineControlWidget::actionLoadConfig()
{
    // Директория проектов (Configs)
    QString configs_path=QString::fromLocal8Bit((application->GetWorkDirectory()+"/../../Configs/").c_str());
    QDir path1(configs_path);
    if(!path1.exists(configs_path))
    {
        configs_path=QString::fromLocal8Bit((application->GetWorkDirectory()+"/../../../Configs/").c_str());
        QDir path2(configs_path);
        if(!path2.exists(configs_path))
        {
            configs_path=QString::fromLocal8Bit(application->GetWorkDirectory().c_str());
        }
    }

    // Создать Configs/Users и Configs/Users/UserName при необходимости (для других действий)
    if(!application->GetUserName().empty())
    {
        std::string userPathRel = application->GetUserConfigPath();
        if(!userPathRel.empty())
        {
            QString users_dir = configs_path + "Users";
            QDir pathUsers(users_dir);
            if(!pathUsers.exists())
                RDK::CreateNewDirectory(users_dir.toLocal8Bit());
            QString default_user_path = configs_path + QString::fromLocal8Bit(userPathRel.c_str());
            QDir pathUser(default_user_path);
            if(!pathUser.exists())
                RDK::CreateNewDirectory(default_user_path.toLocal8Bit());
        }
    }

    // Начальная папка диалога: последняя открытая или Configs
    QSettings settings("NeuroModeler", "NeuroModeler");
    QString dialog_initial_dir = settings.value("LastConfigDialogDir").toString();
    if(dialog_initial_dir.isEmpty() || !QDir(dialog_initial_dir).exists())
        dialog_initial_dir = configs_path;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open config file"), dialog_initial_dir, tr("*.ini"));

    if (fileName.isEmpty())
      return;
    try
    {
      if(watchWindow == NULL)
      {
          watchWindow = new UWatch(this);
          watchWindow->setWindowTitle("Watch window");
      }


      application->OpenProject(fileName.toLocal8Bit().constData());
      UpdateInterface();

      settings.setValue("LastConfigDialogDir", QFileInfo(fileName).absolutePath());

      addToRecentConfigs(fileName);

      /*QStringList list = configFileName.split("/");
      list.pop_back();*/

      RDK::UIVisualControllerStorage::UpdateInterface(true);
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

void UGEngineControlWidget::loadProjectExternal(const QString &config_path)
{
 try
 {
  if (watchWindow == NULL)
  {
      watchWindow = new UWatch(this);
      watchWindow->setWindowTitle("Watch window");
  }

  application->OpenProject(config_path.toLocal8Bit().constData());
  UpdateInterface();

  addToRecentConfigs(config_path);

  RDK::UIVisualControllerStorage::UpdateInterface(true);
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

void UGEngineControlWidget::actionCreateConfig()
{
 if(application->GetProjectOpenFlag())
 {
  // Убеждаемся, что виджет видим и активен перед показом диалога (важно для Windows)
  if (!isVisible() || !isActiveWindow()) {
      raise();
      activateWindow();
  }

  QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Another configuration is open. Close?", QMessageBox::Save|QMessageBox::Close|QMessageBox::Cancel);
  if (reply == QMessageBox::Save)
  {
   application->SaveProject();
   application->CloseProject();
  }
  else
  if(reply == QMessageBox::Close)
  {
   application->CloseProject();
  }
  else
   return;
 }

 createConfigurationWizardWidget->restart();
 createConfigurationWizardWidget->show();
 UpdateInterface();
// if(application->GetProjectOpenFlag())
}

void UGEngineControlWidget::actionCreateSimple()
{
  try
  {
    // Закрытие проекта
    if(application->GetProjectOpenFlag())
    {
        // Убеждаемся, что виджет видим и активен перед показом диалога (важно для Windows)
        if (!isVisible() || !isActiveWindow()) {
            raise();
            activateWindow();
        }

        QMessageBox::StandardButton reply = QMessageBox::question(this, "Info", "Close current config?", QMessageBox::Yes|QMessageBox::No);
        if (reply != QMessageBox::Yes)
        {
            return;
        }
        application->PauseChannel(-1);
        application->CloseProject();
    }

    // Директория проектов: Configs, затем Configs/Users/UserName
    QString configs_path=QString::fromLocal8Bit((application->GetWorkDirectory()+"/../../Configs/").c_str());
    QDir path1(configs_path);
    if(!path1.exists(configs_path))
    {
        configs_path=QString::fromLocal8Bit((application->GetWorkDirectory()+"/../../../Configs/").c_str());
        QDir path2(configs_path);
        if(!path2.exists(configs_path))
        {
            configs_path=QString::fromLocal8Bit(application->GetWorkDirectory().c_str());
        }
    }

    QString default_path = configs_path;
    if(!application->GetUserName().empty())
    {
        std::string userPathRel = application->GetUserConfigPath();
        if(!userPathRel.empty())
        {
            QString users_dir = configs_path + "Users";
            QDir pathUsers(users_dir);
            if(!pathUsers.exists())
                RDK::CreateNewDirectory(users_dir.toLocal8Bit());
            QString default_user_path = configs_path + QString::fromLocal8Bit(userPathRel.c_str());
            QDir pathUser(default_user_path);
            if(!pathUser.exists())
                RDK::CreateNewDirectory(default_user_path.toLocal8Bit());
            default_path = default_user_path;
        }
    }

    std::string path_dialog=default_path.toUtf8().data();

    // Создание папки проекта автоматическое либо выбор существующей
    // Убеждаемся, что виджет видим и активен перед показом диалога (важно для Windows)
    if (!isVisible() || !isActiveWindow()) {
        raise();
        activateWindow();
    }

    QMessageBox::StandardButton reply2 = QMessageBox::question(this, "Info", "Autocreate configuration folder?", QMessageBox::Yes|QMessageBox::No);
    if (reply2 == QMessageBox::Yes)
    {
        time_t curr_time;
        time(&curr_time);

        // Возвращает время в виде понятной строки вида YYYY.MM.DD HH:MM:SS
        std::string folder=RDK::get_text_time(curr_time, '.', '_');
        path_dialog+=std::string("/Autocreate")+folder.c_str();

        if(RDK::CreateNewDirectory(std::string(path_dialog).c_str()) != 0)
            return;
    }
    else
    {
        path_dialog = QFileDialog::getExistingDirectory(this, tr("Select project directory"), default_path, QFileDialog::ShowDirsOnly).toUtf8().data();
    }

    std::string file_name = path_dialog +"/project.ini";
    std::string classname="Model";

    application->CreateProject(file_name, classname);

    RDK::UIVisualControllerStorage::UpdateInterface();
  }
  catch(RDK::UException& e)
  {
    QMessageBox::critical(this,"Error at creatng simple project", QString(e.what()), QMessageBox::Ok);
  }
  catch(std::exception& e)
  {
    QMessageBox::critical(this,"Error at creatng simple project", QString(e.what()), QMessageBox::Ok);
  }
}

void UGEngineControlWidget::actionSaveConfig()
{
    application->SaveProject();
}

void UGEngineControlWidget::actionCloseConfig()
{
 try
 {
  application->PauseChannel(-1);
  application->CloseProject();
  RDK::UIVisualControllerStorage::UpdateInterface(true);
  AUpdateInterface();
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

void UGEngineControlWidget::actionCopyConfig()
{
 try
 {
 QString default_path=QString::fromLocal8Bit((application->GetWorkDirectory()+"/../../Configs/").c_str());
 QDir path1(default_path);
 if(!path1.exists(default_path))
 {
  default_path=QString::fromLocal8Bit((application->GetWorkDirectory()+"/../../../Configs/").c_str());
  QDir path2(default_path);
  if(!path2.exists(default_path))
  {
   default_path=QString::fromLocal8Bit(application->GetWorkDirectory().c_str());
  }
 }

 QString res_path=QFileDialog::getExistingDirectory(this, tr("Create project directory"), default_path, QFileDialog::ShowDirsOnly);
 res_path+="/";

 application->CopyProject(res_path.toLocal8Bit().constData());

 // Убеждаемся, что виджет видим и активен перед показом диалога (важно для Windows)
 if (!isVisible() || !isActiveWindow()) {
     raise();
     activateWindow();
 }

 QMessageBox::StandardButton reply = QMessageBox::question(this, "Info", "Current configuration has been copied to selected destination. Switch to new destination? If you select NO we continue work with previous configuration.", QMessageBox::Yes|QMessageBox::No);
 if(reply == QMessageBox::Yes)
 {
  std::string project_file_name=application->GetProjectFileName();
  actionCloseConfig();
  std::string open_file_name=res_path.toLocal8Bit().constData();
  open_file_name+=project_file_name;
  application->OpenProject(open_file_name);
  RDK::UIVisualControllerStorage::UpdateInterface(true);
  addToRecentConfigs(QString::fromLocal8Bit((application->GetProjectPath()+application->GetProjectFileName()).c_str()));
 }

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

void UGEngineControlWidget::actionAutoCopyConfig()
{
  try
  {
    // Убеждаемся, что виджет видим и активен перед показом диалога (важно для Windows)
    if (!isVisible() || !isActiveWindow()) {
        raise();
        activateWindow();
    }

    if(!application->GetProjectOpenFlag())
    {
        QMessageBox::question(this, "Error", "Please open configuration for copy first!", QMessageBox::Ok);
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Info", "Are you sure to autocreate copy of current config?", QMessageBox::Yes|QMessageBox::No);
    if (reply != QMessageBox::Yes)
    {
        return;
    }

    std::string config_path=application->GetProjectPath();
    if(config_path.empty())
        return;

    time_t curr_time;
    time(&curr_time);

    size_t n=config_path.find_last_not_of("\\/");
    if(config_path.find_last_of("\\/") == config_path.size()-1)
        config_path.resize(n+1);

    // Возвращает время в виде понятной строки вида YYYY.MM.DD HH:MM:SS
    std::string date_time_str=RDK::get_text_time(curr_time, '.', '_');
    config_path+=std::string(" ")+date_time_str+"/";

    application->CopyProject(config_path);
    std::string name = application->GetProjectFileName();
    application->OpenProject(config_path+application->GetProjectFileName());
    addToRecentConfigs(QString::fromLocal8Bit((application->GetProjectPath()+application->GetProjectFileName()).c_str()));
  }
  catch(RDK::UException& e)
  {
    QMessageBox::critical(this,"Error at auto copying project", QString(e.what()), QMessageBox::Ok);
  }
  catch(std::exception& e)
  {
    QMessageBox::critical(this,"Error at auto copying project", QString(e.what()), QMessageBox::Ok);
  }
}

void UGEngineControlWidget::actionRenameConfig()
{
  try
  {
    // Убеждаемся, что виджет видим и активен перед показом диалога (важно для Windows)
    if (!isVisible() || !isActiveWindow()) {
        raise();
        activateWindow();
    }

    if(!application->GetProjectOpenFlag())
    {
        QMessageBox::question(this, "Error", "Please open configuration for copy first!", QMessageBox::Ok);
        return;
    }


    std::string config_path=application->GetProjectPath();
    if(config_path.empty())
        return;

    size_t n=config_path.find_last_not_of("\\/");
    if(config_path.find_last_of("\\/") == config_path.size()-1)
       config_path.resize(n+1);

    std::string project_name=RDK::extract_file_name(config_path.c_str());

    std::string project_path = RDK::extract_file_path(config_path);


    bool ok;
    std::string new_name = QInputDialog::getText(this, tr("Select new configuration name"),
                                         tr("Please enter new name: "), QLineEdit::Normal,
                                         QString::fromStdString(project_name), &ok).toStdString();

    if (ok && !new_name.empty())
    {
        if(!application->RenameProject(project_path+new_name))
        {
            // Убеждаемся, что виджет видим и активен перед показом диалога (важно для Windows)
            if (!isVisible() || !isActiveWindow()) {
                raise();
                activateWindow();
            }
            QMessageBox::question(this, "Error", "Falied to rename configuration!", QMessageBox::Ok);
        }
    }
  }
  catch(RDK::UException& e)
  {
    QMessageBox::critical(this,"Error at renaming project", QString(e.what()), QMessageBox::Ok);
  }
  catch(std::exception& e)
  {
    QMessageBox::critical(this,"Error at renaming project", QString(e.what()), QMessageBox::Ok);
  }
}

void UGEngineControlWidget::actionExit()
{
  QApplication::quit();
}

void UGEngineControlWidget::actionConfigOptions()
{
 createConfigurationWizardWidget->restart();
 createConfigurationWizardWidget->show();
}

void UGEngineControlWidget::actionCreateSaveMockLibs()
{
 if(application)
  application->CreateSaveMockLibs();
}

void UGEngineControlWidget::updateShemeClassesList()
{
 if(modernDiagram)
 {
  modernDiagram->updateScheme(true);
  modernDiagram->updateClassesList();
 }
 int build_mode = application->GetStorageBuildMode();
 ui->menuChooseBuildStorageMode->setTitle("Choose Build Storage Mode [" + QString::number(build_mode) +"]");
}

void UGEngineControlWidget::actionBuildMode1()
{
 if(application)
 {
  application->SetStorageBuildMode(1);
  updateShemeClassesList();
 }
}

void UGEngineControlWidget::actionBuildMode2()
{
 if(application)
 {
  application->SetStorageBuildMode(2);
  updateShemeClassesList();
 }
}

void UGEngineControlWidget:: actionBuildMode3()
{
 if(application)
 {
  application->SetStorageBuildMode(3);
  updateShemeClassesList();
 }
}


// calculate menu actions

void UGEngineControlWidget::actionReloadParameters()
{
    application->ReloadParameters();
}
//int ts; // костыль
void UGEngineControlWidget::actionStart()
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

void UGEngineControlWidget::actionPause()
{
    pauseChannel(-1);
    //killTimer(ts); // костыль
//    ui->statusBar->showMessage("Calculation at pause");
}

void UGEngineControlWidget::actionReset()
{
    resetChannel(-1);
//    ui->statusBar->showMessage("Calculation reseted", 5000);
}

void UGEngineControlWidget::actionStep()
{
  calcOneStepChannel(-1);
}

void UGEngineControlWidget::actionRunNSteps()
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

void UGEngineControlWidget::actionImages()
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

void UGEngineControlWidget::actionNewImages()
{
 addImagesWidged();
}

void UGEngineControlWidget::actionComponentsControl()
{
    ui->dockWidgetComponentsList->show();
}

void UGEngineControlWidget::actionChannelsControl()
{
  ui->dockWidgetChannels->show();
}

void UGEngineControlWidget::actionLogger()
{
  ui->dockWidgetLoger->show();
}

void UGEngineControlWidget::actionTestCreator()
{
  execDialogUVisualControllWidget(createTestWidget);
}

void UGEngineControlWidget::actionWatchWindow()
{
    if(watchWindow != NULL)
    {
        watchWindow->show();
        watchWindow->showNormal();
    }
    else
    {
        watchWindow = new UWatch(this);
        watchWindow->setWindowTitle("Watch window");
        watchWindow->show();
    }
}

void UGEngineControlWidget::actionProjectDescription()
{
    if(projectDescriptionWindow != NULL)
    {
        // Устанавливаем размер окна равным размеру главного окна
        projectDescriptionWindow->resize(this->size());
        projectDescriptionWindow->setWindowState(projectDescriptionWindow->windowState() & ~Qt::WindowMaximized);
        projectDescriptionWindow->show();
        projectDescriptionWindow->showNormal();
        projectDescriptionWindow->raise();
        projectDescriptionWindow->activateWindow();
        // Обновляем содержимое при показе окна (на случай, если проект был открыт после создания окна)
        if(application && application->GetProjectOpenFlag())
        {
            projectDescriptionWindow->AUpdateInterface();
        }
    }
    else
    {
        projectDescriptionWindow = new UProjectDescriptionWindow(this, application);
        projectDescriptionWindow->setWindowTitle("Project Description");

        // Устанавливаем размер окна равным размеру главного окна
        projectDescriptionWindow->resize(this->size());
        projectDescriptionWindow->setWindowState(Qt::WindowNoState);
        projectDescriptionWindow->show();
        projectDescriptionWindow->raise();
        projectDescriptionWindow->activateWindow();
    }
}

void UGEngineControlWidget::actionProfiling()
{
    if (!ui->dockWidgetProfiling->isVisible())
    {
        ui->dockWidgetProfiling->show();
    }
    /*if(!profilingWindow)
    {
        profilingWindow = new QMainWindow(this);
        profilingWindow->setCentralWidget(profilingWindowWidget);
    }
    profilingWindow->resize(profilingWindowWidget->size());
    profilingWindow->setWindowTitle("Profiling");
    profilingWindow->show();
    profilingWindow->showNormal();
    profilingWindow->activateWindow();*/

    //отобразить *graphWindowWidget
//    ui->dockWidgetGraph->show();
}

void UGEngineControlWidget::actionTcpServer()
{
    if(!tcpServerControlWindow )
    {
        tcpServerControlWindow = new QMainWindow(this);
        tcpServerControlWindow->setWindowTitle("TcpServerControl");
        //graphWindow->setCentralWidget(graphWindowWidget);
        //graphWindowWidget->show();
        tcpServerControlWindow->setCentralWidget(tcpServerControlWidget);
    }

    tcpServerControlWindow->resize(tcpServerControlWidget->size());
    tcpServerControlWidget->show();
    tcpServerControlWindow->show();
    tcpServerControlWindow->showNormal();
    tcpServerControlWindow->activateWindow();
}

void UGEngineControlWidget::actionFtpTest()
{
    curlFtpClientTestWidget->show();
    curlFtpClientTestWidget->activateWindow();
}

void UGEngineControlWidget::actionClDesc()
{
    /*
    if(clDescWindow != NULL)
        clDescWindow->show();
    else
    {
        clDescWindow = new UClDescEditor();
        clDescWindow->setWindowTitle("Classes Description Editor");
        clDescWindow->show();
    }
    */
    if(!clDescWindow)
    {
        clDescWindow = new QMainWindow(this);
        clDescWindow->setCentralWidget(clDesc);
        clDescWindow->setWindowTitle("Classes Description Editor");
    }
    clDescWindow->resize(clDesc->size());
    clDesc->show();
    clDescWindow->showNormal();
    clDescWindow->activateWindow();
}

void UGEngineControlWidget::actionNewWatches()
{
    addWatchesWidged();
}

#ifndef RDK_DISABLE_EXT_GUI
void UGEngineControllWidget::actionVASimpleSettings()
{
    if(videoAnalyticsSimpleWidget)
    {
        videoAnalyticsSimpleWidget->show();
    }
}
#endif

void UGEngineControlWidget::startChannel(int chanelIndex)
{
    if(!application->GetProjectOpenFlag())
        return;

    application->StartChannel(chanelIndex);
}

void UGEngineControlWidget::pauseChannel(int chanelIndex)
{
    if(!application->GetProjectOpenFlag())
        return;

    application->PauseChannel(chanelIndex);
}

void UGEngineControlWidget::resetChannel(int chanelIndex)
{
    if(!application->GetProjectOpenFlag())
        return;

    application->ResetChannel(chanelIndex);
}

void UGEngineControlWidget::calcOneStepChannel(int chanelIndex)
{
    if(!application->GetProjectOpenFlag())
        return;

    application->StepChannel(chanelIndex);
}

/// Добавляет новый виджет в imagesVector
void UGEngineControlWidget::addImagesWidged()
{
    int index = 1;
    if(!imagesVector.empty())
        index = imagesVector.back()->accessibleName().replace("Images_","").toInt()+1;

    //создаем каждую новый виджет с именем Images + номер
    imagesVector.push_back(new UImagesWidget(this));
    imagesVector.back()->setAccessibleName(QString("Images_")+RDK::sntoa(index).c_str());
    imagesVector.back()->setWindowTitle(QString("Images_")+RDK::sntoa(index).c_str());

    QMdiSubWindow *imagesSbWindow = new QMdiSubWindow(ui->mdiArea, Qt::SubWindow);
    imagesSbWindow->setWidget(imagesVector.back());
    imagesSbWindow->setAttribute(Qt::WA_DeleteOnClose);
    imagesSbWindow->show();
    imagesSbWindow->showMaximized();

    // когда закрывается вкладка Watches, необходиом грамотно очистить вектор
    connect(imagesSbWindow, SIGNAL(destroyed(QObject*)), this, SLOT(delImagesWidgetSlot(QObject*)) );
}

void UGEngineControlWidget::delImagesWidgetSlot(QObject* obj)
{
    // Виджет Watches уже удален на данный момент
    // он не виден в отличие от других, необходиом его вычислить и удалить из массива
    int index = -1;
    for(size_t i=0;i<imagesVector.size(); i++)
    {
        if(!imagesVector[i]->isVisible())
        {
            index = int(i);
            break;
        }
    }
    delImagesWidged(size_t(index));
}

/// Удаляет виджет из imagesVector по имени
void UGEngineControlWidget::delImagesWidged(size_t index)
{
 if(index>=imagesVector.size())
     return;

 delete imagesVector[index];
 imagesVector.erase(imagesVector.begin()+int(index));
}


/// Добавляет новый виджет отображения графиков
void UGEngineControlWidget::addWatchesWidged()
{
    int index = 1;
    if(!watchesVector.empty())
        index = watchesVector.back()->accessibleName().replace("Watches_","").toInt()+1;

    //создаем каждую новый виджет с именем Watches + номер
    watchesVector.push_back(new UWatchTab(this));
    watchesVector.back()->setAccessibleName(QString("Watches_")+RDK::sntoa(index).c_str());
    watchesVector.back()->setWindowTitle(QString("Watches_")+RDK::sntoa(index).c_str());

    QMdiSubWindow *imagesSbWindow = new QMdiSubWindow(ui->mdiArea, Qt::SubWindow);
    imagesSbWindow->setWidget(watchesVector.back());
    imagesSbWindow->setAttribute(Qt::WA_DeleteOnClose);
    imagesSbWindow->show();
    imagesSbWindow->showMaximized();

    // когда закрывается вкладка Watches, необходиом грамотно очистить вектор
    connect(imagesSbWindow, SIGNAL(destroyed(QObject*)), this, SLOT(delWatchesWidgetSlot(QObject*)) );
}

void UGEngineControlWidget::delWatchesWidgetSlot(QObject* obj)
{
    // Виджет Watches уже удален на данный момент
    // он не виден в отличие от других, необходиом его вычислить и удалить из массива
    int index = -1;
    for(size_t i=0;i<watchesVector.size(); i++)
    {
        if(!watchesVector[i]->isVisible())
        {
            index = int(i);
            break;
        }
    }
    delWatchesWidged(size_t(index));
}

/// Удаляет виджет отображения графиков
void UGEngineControlWidget::delWatchesWidged(size_t index)
{
    if(index>=watchesVector.size())
        return;

    delete watchesVector[index];
    watchesVector.erase(watchesVector.begin()+int(index));
}


void UGEngineControlWidget::execDialogUVisualControllWidget(UVisualControllerWidget *widget)
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

void UGEngineControlWidget::writeSettings()
{
    if(!application) return;

    QSettings projectSettings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    projectSettings.beginGroup(accessibleName());

    projectSettings.setValue("geometry", saveGeometry());
    projectSettings.setValue("state",    saveState());

    // Save current theme
    UStyleManager* styleManager = UStyleManager::instance();
    projectSettings.setValue("theme", styleManager->getThemeName());
    projectSettings.setValue("EnableComponentSpecialFormsQt", m_componentSpecialFormsEnabled);
    projectSettings.setValue("EnableComponentSpecialFormsQt.MotionControl", m_componentSpecialFormsMotionControlEnabled);
    projectSettings.setValue("EnableComponentSpecialFormsQt.PulseLib", m_componentSpecialFormsPulseLibEnabled);
    projectSettings.setValue("EnableComponentSpecialFormsQt.BasicLib", m_componentSpecialFormsBasicLibEnabled);
    projectSettings.setValue("EnableComponentSpecialFormsQt.CvBasicLib", m_componentSpecialFormsCvBasicLibEnabled);
    projectSettings.setValue("EnableComponentSpecialFormsQt.HardwareLib", m_componentSpecialFormsHardwareLibEnabled);

    if(imagesWindow)
    {
      projectSettings.setValue("ImagesGeometry", imagesWindow->saveGeometry());
      projectSettings.setValue("ImagesState",    imagesWindow->saveState());
    }

    writeComponentGuiSettings(projectSettings);

    projectSettings.endGroup();
}

void UGEngineControlWidget::readSettings()
{
    if(!application) return;

    QSettings projectSettings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    projectSettings.beginGroup(accessibleName());

    restoreGeometry(projectSettings.value("geometry").toByteArray());
    restoreState(projectSettings.value("state").toByteArray());

    // Load saved theme (defaults to "Modern Light" if not saved)
    QString savedTheme = projectSettings.value("theme", "Modern Light").toString();
    switchToTheme(savedTheme);

    // Feature flags: component special forms
    // Priority: environment variable > settings value > default(true)
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    auto parseEnabled = [](const QString& value) -> bool
    {
        const QString normalized = value.trimmed().toLower();
        return (normalized == "1" || normalized == "true" || normalized == "yes" || normalized == "on");
    };
    auto readFlag = [&](const char* envName, const char* settingsName, bool defaultValue) -> bool
    {
        if(env.contains(envName))
            return parseEnabled(env.value(envName));
        return projectSettings.value(settingsName, defaultValue).toBool();
    };
    m_componentSpecialFormsEnabled =
        readFlag("NMSDK_ENABLE_COMPONENT_SPECIAL_FORMS_QT", "EnableComponentSpecialFormsQt", true);
    m_componentSpecialFormsMotionControlEnabled =
        readFlag("NMSDK_ENABLE_COMPONENT_SPECIAL_FORMS_QT_MOTIONCONTROL", "EnableComponentSpecialFormsQt.MotionControl", true);
    m_componentSpecialFormsPulseLibEnabled =
        readFlag("NMSDK_ENABLE_COMPONENT_SPECIAL_FORMS_QT_PULSELIB", "EnableComponentSpecialFormsQt.PulseLib", true);
    m_componentSpecialFormsBasicLibEnabled =
        readFlag("NMSDK_ENABLE_COMPONENT_SPECIAL_FORMS_QT_BASICLIB", "EnableComponentSpecialFormsQt.BasicLib", true);
    m_componentSpecialFormsCvBasicLibEnabled =
        readFlag("NMSDK_ENABLE_COMPONENT_SPECIAL_FORMS_QT_CVBASICLIB", "EnableComponentSpecialFormsQt.CvBasicLib", true);
    m_componentSpecialFormsHardwareLibEnabled =
        readFlag("NMSDK_ENABLE_COMPONENT_SPECIAL_FORMS_QT_HARDWARELIB", "EnableComponentSpecialFormsQt.HardwareLib", true);

    if(m_componentSpecialFormsEnabled)
    {
        UComponentGuiRegistrationOptions options;
        options.enableMotionControl = m_componentSpecialFormsMotionControlEnabled;
        options.enablePulseLib = m_componentSpecialFormsPulseLibEnabled;
        options.enableBasicLib = m_componentSpecialFormsBasicLibEnabled;
        options.enableCvBasicLib = m_componentSpecialFormsCvBasicLibEnabled;
        options.enableHardwareLib = m_componentSpecialFormsHardwareLibEnabled;
        RegisterComponentGuiForms(application, options);
    }

    if(!imagesWindow)
    {
        imagesWindow = new QMainWindow(this);
        imagesWindow->setCentralWidget(images);
    }
    imagesWindow->resize(images->size());
    imagesWindow->restoreGeometry(projectSettings.value("ImagesGeometry").toByteArray());
    imagesWindow->restoreState(projectSettings.value("ImagesState").toByteArray());

    readComponentGuiSettings(projectSettings);

    projectSettings.endGroup();
}

void UGEngineControlWidget::on_mdiArea_destroyed(QObject *arg1)
{
 /*
 for(size_t i=0;i<imagesVector.size();i++)
 {
  if(imagesVector[i].SubWindow == arg1)
  {
   delImagesWidged(i);
   break;
  }
 }*/
}

void UGEngineControlWidget::showChannelsWidget (void)
{
    channels->show();
}

void UGEngineControlWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F1)
    {
        openHelpWindow();
        event->accept();
        return;
    }
    UVisualControllerMainWidget::keyPressEvent(event);
}

void UGEngineControlWidget::openHelpWindow()
{
    on_actionUserGuide_triggered();
}

void UGEngineControlWidget::showCustomWidgetById(const QString& id)
{
    createOrActivateCustomWidget(id);
}

static QMenu* menuForPath(QMenuBar* menuBar, const QString& menuPath)
{
    if(!menuBar || menuPath.isEmpty())
        return nullptr;
    QStringList parts = menuPath.split(QLatin1Char('/'), Qt::SkipEmptyParts);
    if(parts.isEmpty())
        return nullptr;

    QMenu* currentMenu = nullptr;
    const QString first = parts.first();
    for(QAction* action : menuBar->actions())
    {
        QMenu* menu = action->menu();
        if(menu && menu->title() == first)
        {
            currentMenu = menu;
            break;
        }
    }
    if(!currentMenu)
        currentMenu = menuBar->addMenu(first);

    for(int i = 1; i < parts.size(); ++i)
    {
        const QString& segment = parts[i];
        QMenu* nextMenu = nullptr;
        for(QAction* action : currentMenu->actions())
        {
            QMenu* submenu = action->menu();
            if(submenu && submenu->title() == segment)
            {
                nextMenu = submenu;
                break;
            }
        }
        if(!nextMenu)
            nextMenu = currentMenu->addMenu(segment);
        currentMenu = nextMenu;
    }
    return currentMenu;
}

void UGEngineControlWidget::appendMenuAction(const QString& menuPath, QAction* action)
{
    if(!action || !ui || !ui->menuBar)
        return;
    QMenu* menu = menuForPath(ui->menuBar, menuPath);
    if(menu)
        menu->addAction(action);
}

void UGEngineControlWidget::registerCustomWidget(const UCustomWidgetDescriptor &descriptor)
{
    if (descriptor.id.isEmpty() || !descriptor.factory)
        return;

    // Сохраняем дескриптор
    customWidgets.push_back(descriptor);

    // Создаём QAction в меню по menuPath
    if (ui && ui->menuBar && !descriptor.menuPath.isEmpty())
    {
        QMenu* currentMenu = menuForPath(ui->menuBar, descriptor.menuPath);
        if(currentMenu)
        {
            QAction *action = currentMenu->addAction(descriptor.title);
            if (!descriptor.shortcut.isEmpty())
            {
                action->setShortcut(descriptor.shortcut);
            }
            action->setData(descriptor.id);
            QObject::connect(action, &QAction::triggered,
                             this, &UGEngineControlWidget::handleCustomWidgetActionTriggered);
        }
    }
}

void UGEngineControlWidget::handleCustomWidgetActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;
    const QString id = action->data().toString();
    if (id.isEmpty())
        return;
    createOrActivateCustomWidget(id);
}

void UGEngineControlWidget::createOrActivateCustomWidget(const QString &id)
{
    // Находим дескриптор
    const UCustomWidgetDescriptor *found = nullptr;
    for (const auto &desc : customWidgets)
    {
        if (desc.id == id)
        {
            found = &desc;
            break;
        }
    }
    if (!found || !application)
        return;

    auto &instances = customWidgetInstances[id];

    // Если singleInstance и уже есть живой экземпляр — просто активируем его
    if (found->singleInstance)
    {
        for (auto &ptr : instances)
        {
            if (!ptr.isNull())
            {
                QWidget *w = ptr.data();
                w->show();
                w->raise();
                w->activateWindow();
                return;
            }
        }
    }

    // Создаём новый виджет через фабрику
    UVisualControllerWidget *widget = found->factory(application);
    if (!widget)
        return;

    widget->setParent(this);

    if (found->placement == UCustomWidgetPlacement::Dock)
    {
        auto *dock = new QDockWidget(found->title, this);
        dock->setWidget(widget);
        addDockWidget(found->defaultDockArea, dock);
        dock->show();
    }
    else
    {
        if (ui && ui->mdiArea)
        {
            QMdiSubWindow *sub = new QMdiSubWindow(ui->mdiArea, Qt::SubWindow);
            sub->setWidget(widget);
            sub->setAttribute(Qt::WA_DeleteOnClose);
            ui->mdiArea->addSubWindow(sub);
            sub->show();
            sub->showMaximized();
        }
        else
        {
            widget->show();
            widget->raise();
            widget->activateWindow();
        }
    }

    instances.push_back(QPointer<UVisualControllerWidget>(widget));
}

void UGEngineControlWidget::closeEvent(QCloseEvent *event)
{
 application->PauseChannel(-1);
 //application->CloseProject();
 application->UnInit();
 event->accept();
 //   if (maybeSave()) {
 //       writeSettings();
 //       event->accept();
 //   } else {
 //       event->ignore();
 //   }
}

void UGEngineControlWidget::updateChannelsVisibility()
{
    if(application->GetNumChannels()>1)
    {
        if(!ui->dockWidgetChannels->isVisible())
        {
            ui->dockWidgetChannels->show();
        }
    }

    if(application->GetNumChannels()==1)
    {
        if(ui->dockWidgetChannels->isVisible())
        {
            ui->dockWidgetChannels->hide();
        }
    }
}

// Обновление интерфейса
void UGEngineControlWidget::AUpdateInterface(void)
{
 QString caption_line=(application->GetProgramName()+" ").c_str();
 caption_line += QCoreApplication::applicationVersion();
 if(!application->GetUserName().empty())
 {
  caption_line=caption_line+" ["+application->GetUserName().c_str();
  if(application->GetUserId()>=0)
   caption_line=caption_line+":"+RDK::sntoa(application->GetUserId()).c_str();
  caption_line=caption_line+="]";
 }
 if(application->GetProjectOpenFlag())
  caption_line=caption_line+" [Configuration: " + (application->GetProjectPath()+application->GetProjectFileName()).c_str()+"]";
  this->setWindowTitle(caption_line);
}

// Возврат интерфейса в исходное состояние
void UGEngineControlWidget::AClearInterface(void)
{
 AUpdateInterface();

 //Очистка Watches окон и Images окон
    int count=int(watchesVector.size());
    for(int i=count-1;i>=0;i--)
        delete watchesVector[i];

    watchesVector.clear();

    count=int(imagesVector.size());
    for(int i=count-1;i>=0;i--)
        delete imagesVector[i];

    imagesVector.clear();

    QList<QMdiSubWindow *> SubWindows = ui->mdiArea->findChildren<QMdiSubWindow *>();
    foreach(QWidget * widget, SubWindows)
    {
        // Игнорируем UModernDiagramContainerWidget
        if(dynamic_cast<SubWindowCloseIgnore*>(widget))
            continue;

        // Удаляем все остальные subWindows (Images, Watches)
        QMdiSubWindow* wid = dynamic_cast<QMdiSubWindow*>(widget);
        if(wid!=nullptr)
            delete widget;
    }
}

// Метод, вызываемый после загрузки проекта
void UGEngineControlWidget::AAfterLoadProject(void)
{
 UpdateInterface();
 if(propertyChanger->componentsList->GetUpdateInterval()>0)
  statusPanel->ChangeAutoupdateProperties(true);
 else
  statusPanel->ChangeAutoupdateProperties(false);
}

// Метод, вызываемый перед закрытием проекта
void UGEngineControlWidget::ABeforeCloseProject(void)
{
    m_componentGuiService.clearAllInstances();
    m_componentGuiTabHosts.clear();
}

// Метод, вызываемый перед сбросом модели
void UGEngineControlWidget::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void UGEngineControlWidget::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void UGEngineControlWidget::ABeforeCalculate(void)
{

}

// Метод, вызываемый после шага расчета
void UGEngineControlWidget::AAfterCalculate(void)
{

}

// Сохраняет параметры интерфейса в xml
void UGEngineControlWidget::ASaveParameters(RDK::USerStorageXML &xml)
{
    xml.WriteInteger("WatchesCount", int(watchesVector.size()));
    xml.SelectNodeForce("Watches");

    for(size_t i=0; i < watchesVector.size(); i++)
    {
        QString watches_name = watchesVector[i]->accessibleName();
        xml.WriteString("name_"+RDK::sntoa(i+1), watches_name.toStdString().c_str());
    }
    xml.SelectUp();

    xml.WriteInteger("ImagesCount", int(imagesVector.size()));
    xml.SelectNodeForce("Images");

    for(size_t i=0; i < imagesVector.size(); i++)
    {
        QString images_name = imagesVector[i]->accessibleName();
        xml.WriteString("name_"+RDK::sntoa(i+1), images_name.toStdString().c_str());
    }
    xml.SelectUp();

    saveComponentGuiLayoutToXml(xml);
}

// Загружает параметры интерфейса из xml
void UGEngineControlWidget::ALoadParameters(RDK::USerStorageXML &xml)
{
    // Очистка существующих Watches
    size_t watches_size = watchesVector.size();
    for(size_t i=0; i < watches_size; i++)
    {
        delete watchesVector[0];
        watchesVector.erase(watchesVector.begin());
    }

    int count=xml.ReadInteger("WatchesCount", 0);

    for(int i=0; i < count; i++)
        addWatchesWidged();

    xml.SelectNodeForce("Watches");
    for(size_t i=0; i < watchesVector.size(); i++)
    {
        QString watches_name = xml.ReadString("name_"+RDK::sntoa(i+1), "Watches_" + RDK::sntoa(i+1)).c_str();
        watchesVector.at(i)->setAccessibleName(watches_name);
        watchesVector.at(i)->setWindowTitle(watches_name);
    }
    xml.SelectUp();


    // Очистка существующих Images
    size_t images_size = imagesVector.size();
    for(size_t i=0; i < images_size; i++)
    {
        delete imagesVector[0];
        imagesVector.erase(imagesVector.begin());
    }

    count=xml.ReadInteger("ImagesCount", 0);

    for(int i=0; i < count; i++)
        addImagesWidged();

    xml.SelectNodeForce("Images");
    for(size_t i=0; i < imagesVector.size(); i++)
    {
        QString images_name = xml.ReadString("name_"+RDK::sntoa(i+1), "Images_" + RDK::sntoa(i+1)).c_str();
        imagesVector.at(i)->setAccessibleName(images_name);
        imagesVector.at(i)->setWindowTitle(images_name);
    }
    xml.SelectUp();

    loadComponentGuiLayoutFromXml(xml);
}

UComponentGuiTabHostWidget* UGEngineControlWidget::ensureComponentGuiTabHost(const QString& hostId)
{
    if(hostId == QStringLiteral("Secondary"))
        return ensureComponentGuiSecondaryTabHost();

    if(m_componentGuiTabHosts.contains(hostId) && !m_componentGuiTabHosts[hostId].isNull())
        return m_componentGuiTabHosts[hostId].data();

    UComponentGuiTabHostWidget* host = new UComponentGuiTabHostWidget(hostId, &m_componentGuiService, ui->mdiArea, application);
    QMdiSubWindow* sub = ui->mdiArea->addSubWindow(host, Qt::SubWindow);
    if(sub)
    {
        sub->setAttribute(Qt::WA_DeleteOnClose, true);
        sub->setWindowTitle(QStringLiteral("Component Tab Host: %1").arg(hostId));
        sub->show();
    }
    m_componentGuiTabHosts[hostId] = host;
    m_componentGuiService.setTabHostMainWindow(nullptr);
    wireComponentGuiTabHostPruning(host);
    return host;
}

UComponentGuiTabHostWidget* UGEngineControlWidget::findComponentGuiTabHost(const QString& hostId) const
{
    if(!m_componentGuiTabHosts.contains(hostId) || m_componentGuiTabHosts.value(hostId).isNull())
        return nullptr;
    return m_componentGuiTabHosts.value(hostId).data();
}

QStringList UGEngineControlWidget::componentGuiTabHostIds() const
{
    QStringList ids;
    for(auto it = m_componentGuiTabHosts.constBegin(); it != m_componentGuiTabHosts.constEnd(); ++it)
    {
        if(!it.value().isNull())
            ids.push_back(it.key());
    }
    if(!m_componentGuiSecondaryTabHost.isNull() && !ids.contains(QStringLiteral("Secondary")))
        ids.push_back(QStringLiteral("Secondary"));
    ids.sort();
    return ids;
}

bool UGEngineControlWidget::moveContextToTabHost(const UComponentGuiContext& context, const QString& hostId)
{
    UComponentGuiTabHostWidget* host = findComponentGuiTabHost(hostId);
    if(!host)
        return false;
    wireComponentGuiTabHostPruning(host);
    return host->assignContext(context);
}

UComponentGuiTabHostWidget* UGEngineControlWidget::ensureComponentGuiSecondaryTabHost()
{
    if(!m_componentGuiSecondaryTabHost.isNull())
        return m_componentGuiSecondaryTabHost.data();

    if(m_componentGuiSecondaryHostWindow.isNull())
        showComponentGuiSecondaryHostWindow();

    QMainWindow* secondary = m_componentGuiSecondaryHostWindow.data();
    if(!secondary)
        return nullptr;

    auto* host = new UComponentGuiTabHostWidget(QStringLiteral("Secondary"),
                                                &m_componentGuiService,
                                                secondary,
                                                application);
    secondary->setCentralWidget(host);
    m_componentGuiSecondaryTabHost = host;
    m_componentGuiTabHosts[QStringLiteral("Secondary")] = host;
    m_componentGuiService.setSecondaryTabHostWidget(host);
    wireComponentGuiTabHostPruning(host);
    return host;
}

bool UGEngineControlWidget::moveContextToSecondaryHost(const UComponentGuiContext& context)
{
    showComponentGuiSecondaryHostWindow();
    UComponentGuiTabHostWidget* host = ensureComponentGuiSecondaryTabHost();
    if(!host)
        return false;
    if(!host->assignContext(context))
        return false;
    pruneEmptyTabHostSlotsForContext(context);
    return true;
}

void UGEngineControlWidget::wireComponentGuiTabHostPruning(UComponentGuiTabHostWidget* host)
{
    if(!host)
        return;
    const QString hostId = host->hostId();
    host->setAfterAssignContextHook([this, hostId](const UComponentGuiContext& ctx) {
        pruneEmptyTabHostSlotsForContext(ctx);
        if(hostId == QStringLiteral("Secondary"))
        {
            m_componentGuiService.assignHostModeForContext(ctx,
                                                           UComponentGuiHostMode::SecondaryDock,
                                                           QStringLiteral("Secondary"));
        }
    });
}

void UGEngineControlWidget::pruneEmptyTabHostSlotsForContext(const UComponentGuiContext& context)
{
    for(auto it = m_componentGuiTabHosts.begin(); it != m_componentGuiTabHosts.end(); ++it)
    {
        if(it.value().isNull())
            continue;
        it.value()->pruneStaleTabForContext(context);
    }
}

void UGEngineControlWidget::saveComponentGuiLayoutToXml(RDK::USerStorageXML &xml)
{
    const QList<UComponentGuiSessionSnapshot> sessions = m_componentGuiService.snapshotOpenSessions();

    xml.SelectNodeForce("ComponentGuiLayout");
    xml.DelNodeInternalContent();
    xml.WriteInteger("SchemaVersion", kComponentGuiLayoutSchemaVersion);
    xml.WriteInteger("SessionCount", sessions.size());

    for(int i = 0; i < sessions.size(); ++i)
    {
        const UComponentGuiSessionSnapshot& s = sessions[i];
        xml.SelectNodeForce("Session_" + RDK::sntoa(i + 1));
        xml.WriteString("SessionId", s.sessionId.toStdString());
        xml.WriteString("FormId", s.formId.toStdString());
        xml.WriteString("ComponentClassName", s.componentClassName.toStdString());
        xml.WriteString("ComponentLongName", s.componentLongName.toStdString());
        xml.WriteInteger("ChannelIndex", s.channelIndex);
        xml.WriteString("HostMode", hostModeToString(s.hostMode).toStdString());
        xml.WriteString("ContainerId", s.containerId.toStdString());
        xml.WriteInteger("CellRow", s.cellRow);
        xml.WriteInteger("CellCol", s.cellCol);
        xml.WriteInteger("OrderIndex", s.orderIndex);
        xml.WriteBool("IsActive", s.isActive);
        xml.SelectUp();
    }

    const QStringList tabHostIds = componentGuiTabHostIds();
    xml.WriteInteger("TabHostCount", tabHostIds.size());
    for(int i = 0; i < tabHostIds.size(); ++i)
    {
        xml.SelectNodeForce("TabHost_" + RDK::sntoa(i + 1));
        xml.WriteString("HostId", tabHostIds[i].toStdString());
        xml.SelectUp();
    }
    xml.SelectUp();
}

void UGEngineControlWidget::loadComponentGuiLayoutFromXml(RDK::USerStorageXML &xml)
{
    if(!xml.SelectNode("ComponentGuiLayout"))
        return;

    const int hostCount = xml.ReadInteger("TabHostCount", 0);
    for(int i = 0; i < hostCount; ++i)
    {
        if(!xml.SelectNode("TabHost_" + RDK::sntoa(i + 1)))
            continue;
        const QString hostId = QString::fromStdString(xml.ReadString("HostId", "MainTabHost"));
        ensureComponentGuiTabHost(hostId);
        xml.SelectUp();
    }

    // Legacy compatibility: old grid layout nodes map to a tab host.
    const int legacyGridCount = xml.ReadInteger("GridCount", 0);
    if(legacyGridCount > 0)
        ensureComponentGuiTabHost(QStringLiteral("MainTabHost"));

    QList<UComponentGuiSessionSnapshot> sessions;
    const int sessionCount = xml.ReadInteger("SessionCount", 0);
    sessions.reserve(sessionCount);
    for(int i = 0; i < sessionCount; ++i)
    {
        if(!xml.SelectNode("Session_" + RDK::sntoa(i + 1)))
            continue;
        UComponentGuiSessionSnapshot s;
        s.sessionId = QString::fromStdString(xml.ReadString("SessionId", ""));
        s.formId = QString::fromStdString(xml.ReadString("FormId", ""));
        s.componentClassName = QString::fromStdString(xml.ReadString("ComponentClassName", ""));
        s.componentLongName = QString::fromStdString(xml.ReadString("ComponentLongName", ""));
        s.channelIndex = xml.ReadInteger("ChannelIndex", -1);
        s.hostMode = hostModeFromString(QString::fromStdString(xml.ReadString("HostMode", "mdi")));
        s.containerId = QString::fromStdString(xml.ReadString("ContainerId", ""));
        s.cellRow = xml.ReadInteger("CellRow", -1);
        s.cellCol = xml.ReadInteger("CellCol", -1);
        s.orderIndex = xml.ReadInteger("OrderIndex", i);
        s.isActive = xml.ReadBool("IsActive", false);
        sessions.push_back(s);
        xml.SelectUp();
    }
    xml.SelectUp();

    std::sort(sessions.begin(), sessions.end(), [](const UComponentGuiSessionSnapshot& a, const UComponentGuiSessionSnapshot& b) {
        return a.orderIndex < b.orderIndex;
    });

    bool hasActive = false;
    UComponentGuiContext activeContext;
    for(const UComponentGuiSessionSnapshot& s : sessions)
    {
        UComponentGuiContext context;
        context.componentClassName = s.componentClassName;
        context.componentLongName = s.componentLongName;
        context.channelIndex = s.channelIndex;

        UVisualControllerWidget* widget = m_componentGuiService.createOrActivate(this, context);
        if(!widget)
            continue;
        ensureComponentGuiQuickActionsInstalled(widget);

        if(s.hostMode == UComponentGuiHostMode::Floating)
        {
            m_componentGuiService.detachToFloating(context);
            pruneEmptyTabHostSlotsForContext(context);
        }
        else if((s.hostMode == UComponentGuiHostMode::TabHost || s.hostMode == UComponentGuiHostMode::Grid) && !s.containerId.isEmpty())
        {
            UComponentGuiTabHostWidget* host = ensureComponentGuiTabHost(s.containerId);
            if(host)
                host->assignContext(context);
        }
        else if(s.hostMode == UComponentGuiHostMode::SecondaryDock)
        {
            moveContextToSecondaryHost(context);
        }
        if(s.isActive && !hasActive)
        {
            hasActive = true;
            activeContext = context;
        }
    }

    if(application)
    {
        QSettings projectSettings(QString::fromLocal8Bit(application->GetProjectPath().c_str()) + "settings.qt",
                                  QSettings::IniFormat);
        projectSettings.beginGroup(accessibleName());
        readComponentGuiSettings(projectSettings);
        projectSettings.endGroup();
    }

    if(hasActive)
    {
        UVisualControllerWidget* widget = m_componentGuiService.createOrActivate(this, activeContext);
        ensureComponentGuiQuickActionsInstalled(widget);
    }
}

void UGEngineControlWidget::writeComponentGuiSettings(QSettings& projectSettings)
{
    const QList<UComponentGuiSessionSnapshot> sessions = m_componentGuiService.snapshotOpenSessions();
    QSet<QString> liveFloatingSessions;
    QSet<QString> liveTabHostIds;

    projectSettings.beginGroup("ComponentGui");
    projectSettings.beginGroup("Floating");
    const QStringList storedFloating = projectSettings.childGroups();
    projectSettings.endGroup();
    projectSettings.beginGroup("TabHost");
    const QStringList storedHosts = projectSettings.childGroups();
    projectSettings.endGroup();
    projectSettings.endGroup();

    for(const UComponentGuiSessionSnapshot& s : sessions)
    {
        if(s.hostMode != UComponentGuiHostMode::Floating)
            continue;
        liveFloatingSessions.insert(s.sessionId);
        m_componentGuiService.captureFloatingState(s.sessionId);
        const QString prefix = QStringLiteral("ComponentGui/Floating/%1").arg(s.sessionId);
        projectSettings.setValue(prefix + "/Geometry", m_componentGuiService.floatingGeometry(s.sessionId));
        projectSettings.setValue(prefix + "/WindowState", m_componentGuiService.floatingWindowState(s.sessionId));
    }
    for(const QString& staleId : storedFloating)
    {
        if(!liveFloatingSessions.contains(staleId))
            projectSettings.remove(QStringLiteral("ComponentGui/Floating/%1").arg(staleId));
    }

    const QStringList hostIds = componentGuiTabHostIds();
    for(const QString& hostId : hostIds)
    {
        UComponentGuiTabHostWidget* host = findComponentGuiTabHost(hostId);
        if(!host)
            continue;
        liveTabHostIds.insert(hostId);
        projectSettings.setValue(QStringLiteral("ComponentGui/TabHost/%1/Geometry").arg(hostId),
                                 host->saveGeometry());
        projectSettings.setValue(QStringLiteral("ComponentGui/TabHost/%1/State").arg(hostId),
                                 host->saveState());
    }
    for(const QString& staleHost : storedHosts)
    {
        if(!liveTabHostIds.contains(staleHost))
            projectSettings.remove(QStringLiteral("ComponentGui/TabHost/%1").arg(staleHost));
    }

    if(m_componentGuiSecondaryHostWindow)
    {
        projectSettings.setValue("ComponentGui/SecondaryWindow/Geometry", m_componentGuiSecondaryHostWindow->saveGeometry());
        projectSettings.setValue("ComponentGui/SecondaryWindow/State", m_componentGuiSecondaryHostWindow->saveState());
    }
}

void UGEngineControlWidget::readComponentGuiSettings(QSettings& projectSettings)
{
    projectSettings.beginGroup("ComponentGui/TabHost");
    const QStringList hostIds = projectSettings.childGroups();
    projectSettings.endGroup();
    for(const QString& hostId : hostIds)
    {
        const QByteArray tabHostGeometry = projectSettings.value(QStringLiteral("ComponentGui/TabHost/%1/Geometry").arg(hostId)).toByteArray();
        const QByteArray tabHostState = projectSettings.value(QStringLiteral("ComponentGui/TabHost/%1/State").arg(hostId)).toByteArray();
        if(tabHostGeometry.isEmpty() && tabHostState.isEmpty())
            continue;
        UComponentGuiTabHostWidget* host = nullptr;
        if(hostId == QStringLiteral("Secondary"))
            host = ensureComponentGuiSecondaryTabHost();
        else
            host = ensureComponentGuiTabHost(hostId);
        if(host)
        {
            if(!tabHostGeometry.isEmpty())
                host->restoreGeometry(tabHostGeometry);
            if(!tabHostState.isEmpty())
                host->restoreState(tabHostState);
        }
    }

    const QList<UComponentGuiSessionSnapshot> sessions = m_componentGuiService.snapshotOpenSessions();
    for(const UComponentGuiSessionSnapshot& s : sessions)
    {
        if(s.hostMode != UComponentGuiHostMode::Floating)
            continue;
        const QString prefix = QStringLiteral("ComponentGui/Floating/%1").arg(s.sessionId);
        const QByteArray geometry = projectSettings.value(prefix + "/Geometry").toByteArray();
        const QByteArray state = projectSettings.value(prefix + "/WindowState").toByteArray();
        m_componentGuiService.applyFloatingState(s.sessionId, geometry, state);
    }

    const QByteArray secondaryGeometry = projectSettings.value("ComponentGui/SecondaryWindow/Geometry").toByteArray();
    const QByteArray secondaryState = projectSettings.value("ComponentGui/SecondaryWindow/State").toByteArray();
    if(!secondaryGeometry.isEmpty() || !secondaryState.isEmpty())
    {
        showComponentGuiSecondaryHostWindow();
        if(m_componentGuiSecondaryHostWindow)
        {
            if(!secondaryGeometry.isEmpty())
                m_componentGuiSecondaryHostWindow->restoreGeometry(secondaryGeometry);
            if(!secondaryState.isEmpty())
                m_componentGuiSecondaryHostWindow->restoreState(secondaryState);
        }
    }
}

QString UGEngineControlWidget::hostModeToString(UComponentGuiHostMode mode) const
{
    switch(mode)
    {
    case UComponentGuiHostMode::Mdi:
        return QStringLiteral("mdi");
    case UComponentGuiHostMode::Floating:
        return QStringLiteral("floating");
    case UComponentGuiHostMode::TabHost:
        return QStringLiteral("tabhost");
    case UComponentGuiHostMode::SecondaryDock:
        return QStringLiteral("secondary_dock");
    case UComponentGuiHostMode::Grid:
        return QStringLiteral("grid");
    }
    return QStringLiteral("mdi");
}

UComponentGuiHostMode UGEngineControlWidget::hostModeFromString(const QString& mode) const
{
    if(mode == QStringLiteral("floating"))
        return UComponentGuiHostMode::Floating;
    if(mode == QStringLiteral("tabhost"))
        return UComponentGuiHostMode::TabHost;
    if(mode == QStringLiteral("secondary_dock"))
        return UComponentGuiHostMode::SecondaryDock;
    if(mode == QStringLiteral("grid"))
        return UComponentGuiHostMode::TabHost;
    return UComponentGuiHostMode::Mdi;
}

void UGEngineControlWidget::showComponentGuiHostMenu(UVisualControllerWidget* widget, const QPoint& globalPos)
{
    UComponentGuiContext context;
    UComponentGuiHostMode mode = UComponentGuiHostMode::Mdi;
    if(!resolveComponentGuiWidgetContext(widget, context, &mode))
        return;

    QMenu menu(this);
    QAction* detachAction = menu.addAction("Detach");
    QAction* attachAction = menu.addAction("Attach to MDI");
    QAction* moveToTabHostAction = menu.addAction("Move to Tab Host...");
    QAction* moveToSecondaryDockAction = menu.addAction("Move to Secondary Host");
    detachAction->setEnabled(mode == UComponentGuiHostMode::Mdi ||
                             mode == UComponentGuiHostMode::TabHost ||
                             mode == UComponentGuiHostMode::SecondaryDock);
    attachAction->setEnabled(mode == UComponentGuiHostMode::Floating);
    moveToTabHostAction->setEnabled(mode != UComponentGuiHostMode::TabHost);
    moveToSecondaryDockAction->setEnabled(mode != UComponentGuiHostMode::SecondaryDock);

    QAction* chosen = menu.exec(globalPos);
    if(chosen == detachAction)
    {
        m_componentGuiService.detachToFloating(context);
        pruneEmptyTabHostSlotsForContext(context);
        return;
    }
    if(chosen == attachAction)
    {
        m_componentGuiService.attachToMdi(context, ui->mdiArea);
        pruneEmptyTabHostSlotsForContext(context);
        return;
    }
    if(chosen == moveToSecondaryDockAction)
    {
        if(!moveContextToSecondaryHost(context))
        {
            QMessageBox::warning(this, tr("Move to Secondary Host"),
                                 tr("Failed to move GUI to secondary host window."));
        }
        return;
    }
    if(chosen == moveToTabHostAction)
    {
        const QStringList hosts = componentGuiTabHostIds();
        if(hosts.isEmpty())
        {
            ensureComponentGuiTabHost(QStringLiteral("MainTabHost"));
        }

        bool ok = false;
        const QString hostId = QInputDialog::getItem(this,
                                                     tr("Move to Tab Host"),
                                                     tr("Host ID:"),
                                                     componentGuiTabHostIds(),
                                                     0,
                                                     false,
                                                     &ok);
        if(!ok || hostId.trimmed().isEmpty())
            return;

        if(!moveContextToTabHost(context, hostId))
        {
            QMessageBox::warning(this, tr("Move to Tab Host"),
                                 tr("Failed to move GUI to tab host '%1'.").arg(hostId));
        }
    }
}

bool UGEngineControlWidget::resolveComponentGuiWidgetContext(UVisualControllerWidget* widget,
                                                             UComponentGuiContext& context,
                                                             UComponentGuiHostMode* mode) const
{
    if(!widget)
        return false;
    if(!m_componentGuiService.tryGetContextByWidget(widget, context))
        return false;
    if(mode)
    {
        UComponentGuiHostMode resolved = UComponentGuiHostMode::Mdi;
        if(m_componentGuiService.tryGetHostModeByWidget(widget, resolved))
            *mode = resolved;
    }
    return true;
}

void UGEngineControlWidget::promptAndOpenComponentGuiTabHost()
{
    UComponentGuiTabHostWidget* host = ensureComponentGuiTabHost(QStringLiteral("MainTabHost"));
    if(!host)
        return;
    host->show();
    host->raise();
    host->activateWindow();
}

void UGEngineControlWidget::startComponentGuiDrag(const UComponentGuiContext& context, QWidget* dragSource, bool detachOnIgnoredDrop)
{
    QDrag* drag = new QDrag(dragSource ? dragSource : this);
    QMimeData* mime = new QMimeData();
    mime->setData(UComponentGuiDndPayload::mimeType(), UComponentGuiDndPayload::encode(context));
    drag->setMimeData(mime);
    const Qt::DropAction result = drag->exec(Qt::MoveAction);
    if(detachOnIgnoredDrop && result != Qt::MoveAction)
    {
        m_componentGuiService.detachToFloating(context);
        pruneEmptyTabHostSlotsForContext(context);
    }
}

void UGEngineControlWidget::ensureComponentGuiDragSourcesInstalled(UVisualControllerWidget* widget)
{
    Q_UNUSED(widget);
}

void UGEngineControlWidget::installDragFilterRecursively(QWidget* root)
{
    Q_UNUSED(root);
}

void UGEngineControlWidget::ensureComponentGuiQuickActionsInstalled(UVisualControllerWidget* widget)
{
    if(!widget)
        return;

    QWidget* panel = widget->findChild<QWidget*>(QStringLiteral("ComponentGuiQuickActionsPanel"));
    if(!panel)
    {
        panel = new QWidget(widget);
        panel->setObjectName(QStringLiteral("ComponentGuiQuickActionsPanel"));
        panel->setStyleSheet(QStringLiteral(
            "QWidget#ComponentGuiQuickActionsPanel {"
            " background: rgba(40,40,40,120);"
            " border-radius: 4px;"
            "}"
            "QToolButton {"
            " color: white;"
            " padding: 2px 6px;"
            "}"
        ));
        auto* layout = new QHBoxLayout(panel);
        layout->setContentsMargins(4, 2, 4, 2);
        layout->setSpacing(4);

        auto makeButton = [panel, layout](const QString& text, const QString& tip) -> QToolButton*
        {
            QToolButton* button = new QToolButton(panel);
            button->setText(text);
            button->setToolTip(tip);
            layout->addWidget(button);
            return button;
        };

        QToolButton* toMdi = makeButton(QStringLiteral("To MDI"), tr("Move to MDI area"));
        QToolButton* toTab = makeButton(QStringLiteral("To Tab"), tr("Move to Tab Host"));
        QToolButton* toSecondary = makeButton(QStringLiteral("To Secondary"), tr("Move to Secondary Host"));

        connect(toMdi, &QToolButton::clicked, this, [this, widget]() {
            UComponentGuiContext context;
            if(!m_componentGuiService.tryGetContextByWidget(widget, context))
                return;
            m_componentGuiService.attachToMdi(context, ui->mdiArea);
            pruneEmptyTabHostSlotsForContext(context);
        });
        connect(toTab, &QToolButton::clicked, this, [this, widget]() {
            UComponentGuiContext context;
            if(!m_componentGuiService.tryGetContextByWidget(widget, context))
                return;
            ensureComponentGuiTabHost(QStringLiteral("MainTabHost"));
            moveContextToTabHost(context, QStringLiteral("MainTabHost"));
        });
        connect(toSecondary, &QToolButton::clicked, this, [this, widget]() {
            UComponentGuiContext context;
            if(!m_componentGuiService.tryGetContextByWidget(widget, context))
                return;
            moveContextToSecondaryHost(context);
        });
    }

    widget->setProperty("componentGuiQuickActionsHost", true);
    widget->installEventFilter(this);
    positionComponentGuiQuickActions(widget);
    panel->show();
    panel->raise();
}

void UGEngineControlWidget::positionComponentGuiQuickActions(UVisualControllerWidget* widget)
{
    if(!widget)
        return;
    QWidget* panel = widget->findChild<QWidget*>(QStringLiteral("ComponentGuiQuickActionsPanel"));
    if(!panel)
        return;
    panel->adjustSize();
    const int margin = 8;
    const int x = std::max(0, widget->width() - panel->width() - margin);
    const int y = margin;
    panel->move(x, y);
}

void UGEngineControlWidget::showComponentGuiSecondaryHostWindow()
{
    if(m_componentGuiSecondaryHostWindow.isNull())
    {
        QMainWindow* secondary = new QMainWindow(this);
        secondary->setObjectName(QStringLiteral("ComponentGuiSecondaryHostWindow"));
        secondary->setWindowTitle(tr("Component GUI Secondary Host"));
        secondary->setAttribute(Qt::WA_DeleteOnClose, true);
        secondary->setAcceptDrops(true);
        secondary->installEventFilter(this);
        connect(secondary, &QObject::destroyed, this, [this]() {
            m_componentGuiSecondaryHostWindow = nullptr;
            m_componentGuiSecondaryTabHost = nullptr;
            m_componentGuiTabHosts.remove(QStringLiteral("Secondary"));
            m_componentGuiService.setSecondaryHostMainWindow(nullptr);
            m_componentGuiService.setSecondaryTabHostWidget(nullptr);
        });
        m_componentGuiSecondaryHostWindow = secondary;
        m_componentGuiService.setSecondaryHostMainWindow(secondary);
    }
    ensureComponentGuiSecondaryTabHost();
    m_componentGuiSecondaryHostWindow->show();
    m_componentGuiSecondaryHostWindow->raise();
    m_componentGuiSecondaryHostWindow->activateWindow();
}

bool UGEngineControlWidget::handleDropToSecondaryHost(const QMimeData* mimeData)
{
    UComponentGuiContext context;
    QString sourceHostId;
    int sourceIndex = -1;
    int sourceCol = -1;
    if(!UComponentGuiDndPayload::decode(mimeData, context, sourceHostId, sourceIndex, sourceCol))
        return false;
    Q_UNUSED(sourceCol);

    if(!moveContextToSecondaryHost(context))
        return false;
    Q_UNUSED(sourceHostId);
    Q_UNUSED(sourceIndex);
    return true;
}

bool UGEngineControlWidget::handleDropToTabHost(const QMimeData* mimeData, const QString& hostId)
{
    UComponentGuiContext context;
    QString sourceHostId;
    int sourceIndex = -1;
    int sourceCol = -1;
    if(!UComponentGuiDndPayload::decode(mimeData, context, sourceHostId, sourceIndex, sourceCol))
        return false;
    Q_UNUSED(sourceCol);

    if(!moveContextToTabHost(context, hostId))
        return false;
    Q_UNUSED(sourceHostId);
    Q_UNUSED(sourceIndex);
    return true;
}

QString UGEngineControlWidget::resolveTabHostDropTargetAtCursor() const
{
    const QPoint cursorPos = QCursor::pos();
    QWidget* const underCursor = QApplication::widgetAt(cursorPos);
    auto belongsTo = [](QWidget* root, QWidget* candidate) -> bool
    {
        QWidget* current = candidate;
        while(current)
        {
            if(current == root)
                return true;
            current = current->parentWidget();
        }
        return false;
    };

    for(auto it = m_componentGuiTabHosts.constBegin(); it != m_componentGuiTabHosts.constEnd(); ++it)
    {
        if(it.value().isNull())
            continue;
        QWidget* hostWidget = it.value().data();
        if(underCursor && belongsTo(hostWidget, underCursor))
            return it.key();
        const QRect globalRect(hostWidget->mapToGlobal(QPoint(0, 0)), hostWidget->size());
        if(globalRect.contains(cursorPos))
            return it.key();
    }
    return QString();
}

bool UGEngineControlWidget::isCursorOverSecondaryHost() const
{
    if(m_componentGuiSecondaryHostWindow.isNull())
        return false;

    const QPoint cursorPos = QCursor::pos();
    QWidget* const underCursor = QApplication::widgetAt(cursorPos);
    auto belongsTo = [](QWidget* root, QWidget* candidate) -> bool
    {
        QWidget* current = candidate;
        while(current)
        {
            if(current == root)
                return true;
            current = current->parentWidget();
        }
        return false;
    };

    QWidget* secondary = m_componentGuiSecondaryHostWindow.data();
    if(underCursor && belongsTo(secondary, underCursor))
        return true;
    const QRect secondaryRect(secondary->mapToGlobal(QPoint(0, 0)), secondary->size());
    return secondaryRect.contains(cursorPos);
}

void UGEngineControlWidget::on_actionAbout_triggered()
{
 if(!aboutDialog)
 {
  aboutDialog = new UAboutDialog(this, application);
 }
 aboutDialog->show();
}

void UGEngineControlWidget::on_actionUserGuide_triggered()
{
 if(!helpWindow)
 {
  helpWindow = new UHelpWindow(this, application);
 }
 helpWindow->show();
 helpWindow->raise();
 helpWindow->activateWindow();
}


void UGEngineControlWidget::on_actionWatches_triggered()
{
 addWatchesWidged();
}


void UGEngineControlWidget::on_actionImages_triggered()
{
 addImagesWidged();
}

void UGEngineControlWidget::createThemeMenu()
{
    // Create Theme submenu in Window menu
    QMenu* themeMenu = new QMenu(tr("Theme"), this);

    QActionGroup* themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);

    m_lightThemeAction = themeMenu->addAction(tr("Light"));
    m_lightThemeAction->setCheckable(true);
    themeGroup->addAction(m_lightThemeAction);

    m_darkThemeAction = themeMenu->addAction(tr("Dark"));
    m_darkThemeAction->setCheckable(true);
    themeGroup->addAction(m_darkThemeAction);

    // Connect theme actions
    connect(m_lightThemeAction, &QAction::triggered, this, [this]() {
        switchToTheme("Modern Light");
        // Save theme to settings when user manually switches
        if (application)
        {
            QSettings projectSettings(QString::fromLocal8Bit(
                                 application->GetProjectPath().c_str())+"settings.qt",
                               QSettings::IniFormat);
            projectSettings.beginGroup(accessibleName());
            projectSettings.setValue("theme", "Modern Light");
            projectSettings.endGroup();
        }
    });

    connect(m_darkThemeAction, &QAction::triggered, this, [this]() {
        switchToTheme("Modern Dark");
        // Save theme to settings when user manually switches
        if (application)
        {
            QSettings projectSettings(QString::fromLocal8Bit(
                                 application->GetProjectPath().c_str())+"settings.qt",
                               QSettings::IniFormat);
            projectSettings.beginGroup(accessibleName());
            projectSettings.setValue("theme", "Modern Dark");
            projectSettings.endGroup();
        }
    });

    // Add theme menu to Window menu
    ui->menuWindow->addSeparator();
    ui->menuWindow->addMenu(themeMenu);

    // Update menu state based on current theme
    updateThemeMenuState();
}

void UGEngineControlWidget::updateThemeMenuState()
{
    if (!m_lightThemeAction || !m_darkThemeAction)
        return;

    UStyleManager* styleManager = UStyleManager::instance();
    QString currentTheme = styleManager->getThemeName();

    // Update checkboxes based on current theme
    m_lightThemeAction->setChecked(currentTheme == "Modern Light");
    m_darkThemeAction->setChecked(currentTheme == "Modern Dark");
}

static QString recentConfigDisplayName(const QString& path)
{
    QFileInfo info(path);
    QString canonical = QDir::cleanPath(info.absoluteFilePath());
    QString fileName = info.fileName();
    QString parentDirName = info.dir().dirName();

    if (fileName.isEmpty())
        return path;

    QString pathWithSlash = QDir::fromNativeSeparators(canonical);
    QString basePath;
    int configsIdx = pathWithSlash.indexOf(QStringLiteral("Configs"));
    if (configsIdx >= 0)
        basePath = pathWithSlash.mid(configsIdx);
    else
        basePath = pathWithSlash;

    QStringList segments = basePath.split(QLatin1Char('/'), Qt::SkipEmptyParts);
    if (segments.size() <= 2)
        return basePath;
    return segments.first() + QStringLiteral("/.../") + parentDirName + QLatin1Char('/') + fileName;
}

void UGEngineControlWidget::updateRecentConfigsMenu()
{
    ui->menuRecentConfigs->clear();

    QSettings settings("NeuroModeler", "NeuroModeler");
    QStringList paths = settings.value("RecentConfigs").toStringList();

    if (paths.isEmpty())
    {
        QAction* noRecent = ui->menuRecentConfigs->addAction(tr("No recent configs"));
        noRecent->setEnabled(false);
        return;
    }

    for (const QString& path : paths)
    {
        QString displayName = recentConfigDisplayName(path);
        if (displayName.isEmpty())
            displayName = path;
        QAction* action = ui->menuRecentConfigs->addAction(displayName);
        action->setData(path);
        action->setToolTip(path);
        connect(action, &QAction::triggered, this, [this, path]() { loadProjectExternal(path); });
    }
}

void UGEngineControlWidget::addToRecentConfigs(const QString& path)
{
    QString canonical = QDir::cleanPath(QFileInfo(path).absoluteFilePath());
    if (canonical.isEmpty())
        return;

    QSettings settings("NeuroModeler", "NeuroModeler");
    QStringList paths = settings.value("RecentConfigs").toStringList();
    paths.removeAll(canonical);
    paths.prepend(canonical);
    while (paths.size() > kMaxRecentConfigs)
        paths.removeLast();
    settings.setValue("RecentConfigs", paths);
    updateRecentConfigsMenu();
}

void UGEngineControlWidget::switchToTheme(const QString& themeName)
{
    UStyleManager* styleManager = UStyleManager::instance();
    QApplication* app = qobject_cast<QApplication*>(QCoreApplication::instance());

    if (styleManager->switchTheme(themeName, app))
    {
        // Force update of all widgets
        for (QWidget* widget : QApplication::allWidgets())
        {
            widget->style()->unpolish(widget);
            widget->style()->polish(widget);
            widget->update();
        }

        // Принудительно обновляем стили QTabBar в QMdiArea
        // Это необходимо, так как QMdiArea создает свой собственный QTabBar
        // Используем QTimer для гарантированного обновления после применения глобальных стилей
        QTimer::singleShot(0, this, [this]() {
            if (ui && ui->mdiArea)
            {
                QTabBar* tabBar = ui->mdiArea->findChild<QTabBar*>();
                if (tabBar)
                {
                    // Очищаем локальные стили, чтобы применились глобальные из QSS
                    tabBar->setStyleSheet("");
                    tabBar->style()->unpolish(tabBar);
                    tabBar->style()->polish(tabBar);
                    tabBar->update();
                }
            }
        });

        // Update the modern diagram widget if it exists
        // This invalidates cache and forces repaint of all nodes with new theme colors
        if (modernDiagram)
        {
            modernDiagram->updateTheme();
        }

        // Update Watch window if it exists
        if (watchWindow)
        {
            watchWindow->updateTheme();
        }

        // Update all Watch tabs in MDI area
        for(size_t i = 0; i < watchesVector.size(); i++)
        {
            if(watchesVector[i])
            {
                watchesVector[i]->updateTheme();
            }
        }

        // Update menu state
        updateThemeMenuState();
    }
    else
    {
        QMessageBox::warning(this, tr("Theme Error"),
            tr("Failed to switch to theme: %1").arg(themeName));
    }
}

// --------------------------- SubWindowCloseIgnore ---------------------------

void SubWindowCloseIgnore::keyPressEvent(QKeyEvent *event)
{
    // Forward F1 to parent window if it's UGEngineControlWidget
    if(event->key() == Qt::Key_F1)
    {
        QWidget* parent = parentWidget();
        while(parent)
        {
            if(auto* mainWidget = qobject_cast<UGEngineControlWidget*>(parent))
            {
                mainWidget->openHelpWindow();
                event->accept();
                return;
            }
            parent = parent->parentWidget();
        }
    }
    QMdiSubWindow::keyPressEvent(event);
}

