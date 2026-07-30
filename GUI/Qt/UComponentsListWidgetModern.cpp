#include "UComponentsListWidgetModern.h"
#include "ui_UComponentsListWidgetModern.h"

#include <QDebug>
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include <QList>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <QSet>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QTabBar>

#include "UGuiTelemetry.h"
#include "UEngineSelectionSync.h"
#include "UComponentGuiService.h"
#include "UComponentFormRegistry.h"

UComponentsListWidgetModern::UComponentsListWidgetModern(QWidget *parent, RDK::UApplication *app, int channel_mode) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UComponentsListWidgetModern)
{
    channelMode=channel_mode;
    CheckModelFlag=false;
    ui->setupUi(this);

    // Создание кнопки переключения режимов
    toggleModeButton = new QToolButton(this);
    toggleModeButton->setText(tr("☰"));
    toggleModeButton->setToolTip(tr("Показать/скрыть дерево компонентов"));
    toggleModeButton->setCheckable(true);
    toggleModeButton->setChecked(false); // false = компактный режим
    toggleModeButton->setMaximumWidth(30);
    ui->horizontalLayoutTreeWidget->insertWidget(1, toggleModeButton);
    connect(toggleModeButton, &QToolButton::clicked, this, &UComponentsListWidgetModern::toggleTreeViewMode);

    // Создание дерева компонентов
    componentsTree = new UComponentListTreeWidgetModern(this);
    QWidget *treeContainer = ui->treeContainer;
    QVBoxLayout *treeLayout = qobject_cast<QVBoxLayout*>(treeContainer->layout());
    if (!treeLayout) {
        treeLayout = new QVBoxLayout(treeContainer);
        treeLayout->setContentsMargins(0,0,0,0);
    }
    filterLineEdit = new QLineEdit(this);
    filterLineEdit->setObjectName(QStringLiteral("componentsFilterLineEdit"));
    filterLineEdit->setPlaceholderText(tr("Фильтр компонентов..."));
    filterLineEdit->setClearButtonEnabled(true);
    connect(filterLineEdit, &QLineEdit::textChanged,
            this, &UComponentsListWidgetModern::handleFilterTextChanged);
    // Виджеты будут перемещаться между контейнерами
    treeLayout->addWidget(filterLineEdit);
    treeLayout->addWidget(componentsTree);

    // Создание popup окна для дерева компонентов
    treePopupDialog = new QDialog(this, Qt::Popup);
    treePopupDialog->setWindowModality(Qt::NonModal);
    treePopupDialog->setAttribute(Qt::WA_DeleteOnClose, false);
    treePopupDialog->setStyleSheet(
        "QDialog { "
        "background-color: palette(window); "
        "border: 1px solid palette(mid); "
        "border-radius: 4px; "
        "}"
    );
    // Закрытие при потере фокуса (клик вне окна)
    connect(treePopupDialog, &QDialog::finished, this, [this]() {
        if (treePopupDialog->isVisible()) {
            hideTreePopup();
        }
    });
    treePopupContainer = new QWidget(treePopupDialog);
    treePopupLayout = new QVBoxLayout(treePopupContainer);
    treePopupLayout->setContentsMargins(4, 4, 4, 4);
    treePopupLayout->setSpacing(4);

    QVBoxLayout *dialogLayout = new QVBoxLayout(treePopupDialog);
    dialogLayout->setContentsMargins(0, 0, 0, 0);
    dialogLayout->addWidget(treePopupContainer);

    // Устанавливаем фильтр событий для обработки Esc
    treePopupDialog->installEventFilter(this);

    connect(componentsTree, SIGNAL(moveComponentUp()), this, SLOT(componentMoveUp()));
    connect(componentsTree, SIGNAL(moveComponentDown()), this, SLOT(componentMoveDown()));

    // Всегда используем компактный режим - дерево скрыто в основном layout
    ui->treeContainer->hide();
    toggleModeButton->setChecked(false);
    toggleModeButton->setText(tr("☰"));
    toggleModeButton->setToolTip(tr("Показать дерево компонентов"));

    currentChannel = Core_GetSelectedChannelIndex();
    ui->listWidgetChannelSelection->hide();
    channelsSelectionVisible = false;

    UpdateInterval = -1;
    setAccessibleName("UComponentsListWidgetModern"); // пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    //readSettings(app, settingsGroup);

    renderedSnapshotVersion = 0;
    lastSnapshot = NMSDK::UGuiModelSnapshot::Instance().CurrentSnapshot();
    componentFilterText.clear();
    connect(&NMSDK::UGuiModelSnapshot::Instance(), &NMSDK::UGuiModelSnapshot::SnapshotUpdated,
            this, &UComponentsListWidgetModern::handleSnapshotUpdated);

    UpdateInterface(true);

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
    connect(componentsTree, SIGNAL(itemSelectionChanged()),
            this, SLOT(componentListItemSelectionChanged()));

    // Обработка одинарного клика для popup режима
    connect(componentsTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(onComponentItemClicked(QTreeWidgetItem*, int)));

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    connect(componentsTree, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(drawSelectedComponent(QModelIndex)));

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ propertys
    connect(ui->treeWidgetParameters, SIGNAL(itemSelectionChanged()),
            this, SLOT(parametersListSelectionChanged()));
    connect(ui->treeWidgetState, SIGNAL(itemSelectionChanged()),
            this, SLOT(stateListSelectionChanged()));
    connect(ui->treeWidgetInputs, SIGNAL(itemSelectionChanged()),
            this, SLOT(inputsListSelectionChanged()));
    connect(ui->treeWidgetOutputs, SIGNAL(itemSelectionChanged()),
            this, SLOT(outputsListSelectionChanged()));
    connect(ui->treeWidgetFavorites, SIGNAL(itemSelectionChanged()),
            this, SLOT(favoritesListSelectionChanged()));

    connect(ui->treeWidgetParameters, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
            this, SLOT(parametersListItemChanged(QTreeWidgetItem *, int )));
    connect(ui->treeWidgetFavorites, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
            this, SLOT(favoritesListItemChanged(QTreeWidgetItem *, int )));

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
    connect(ui->listWidgetChannelSelection, SIGNAL(itemSelectionChanged()), this, SLOT(channelsListSelectionChanged()));

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    QAction *actionSeparator1 = new QAction(this);
    actionSeparator1->setSeparator(true);
    QAction *actionSeparator2 = new QAction(this);
    actionSeparator2->setSeparator(true);
    QAction *actionSeparator3 = new QAction(this);
    actionSeparator3->setSeparator(true);
    QAction *actionSeparator4 = new QAction(this);
    actionSeparator4->setSeparator(true);
    QAction *actionSeparator5 = new QAction(this);
    actionSeparator5->setSeparator(true);
    componentsTree->addAction(ui->actionComponentMoveUp);
    componentsTree->addAction(ui->actionComponentMoveDown);
    componentsTree->addAction(actionSeparator1);
    componentsTree->addAction(ui->actionComponentRename);
    componentsTree->addAction(ui->actionComponentDelete);
    componentsTree->addAction(actionSeparator2);
    componentsTree->addAction(ui->actionComponentReset);
    componentsTree->addAction(ui->actionComponentCalculate);
    componentsTree->addAction(ui->actionDefaultAllParameters);
    componentsTree->addAction(ui->actionComponentInit);
    componentsTree->addAction(ui->actionComponentUnInit);
    componentsTree->addAction(actionSeparator3);
    componentsTree->addAction(ui->actionCopyNameToClipboard);
    componentsTree->addAction(ui->actionCopyLongNameToClipboard);
    componentsTree->addAction(ui->actionCopyClassNameToClipboard);
    componentsTree->addAction(actionSeparator4);
    componentsTree->addAction(ui->actionComponentGUI);
    componentsTree->addAction(actionSeparator5);
    componentsTree->addAction(ui->actionReloadTree);
    ui->actionComponentGUI->setEnabled(false); // пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ, пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ enable
    connect(ui->actionComponentMoveUp, SIGNAL(triggered()), this, SLOT(componentMoveUp()));
    connect(ui->actionComponentMoveDown, SIGNAL(triggered()), this, SLOT(componentMoveDown()));
    connect(ui->actionComponentRename, SIGNAL(triggered()), this, SLOT(componentRename()));
    connect(ui->actionComponentDelete, SIGNAL(triggered()), this, SLOT(componentDelete()));
    connect(ui->actionComponentReset, SIGNAL(triggered()), this, SLOT(componentReset()));
    connect(ui->actionComponentCalculate, SIGNAL(triggered()), this, SLOT(componentCalculate()));
 //   connect(ui->actionDefaultAllParameters, SIGNAL(triggered()), this, SLOT(on_actionDefaultAllParameters_triggered()));

    connect(ui->actionComponentInit, SIGNAL(triggered()), this, SLOT(componentInit()));
    connect(ui->actionComponentUnInit, SIGNAL(triggered()), this, SLOT(componentUnInit()));
    connect(ui->actionCopyNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyNameToClipboard()));
    connect(ui->actionCopyLongNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyLongNameToClipboard()));
    connect(ui->actionCopyClassNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyClassNameToClipboard()));
    connect(ui->actionComponentGUI, SIGNAL(triggered()), this, SLOT(componentGUI()));

    QAction *actionSeparatorProperty1 = new QAction(this);
    actionSeparatorProperty1->setSeparator(true);
    ui->treeWidgetParameters->addAction(ui->actionCopyPropertyNameToClipboard);
    ui->treeWidgetState->addAction(ui->actionCopyPropertyNameToClipboard);
    ui->treeWidgetInputs->addAction(ui->actionCopyPropertyNameToClipboard);
    ui->treeWidgetOutputs->addAction(ui->actionCopyPropertyNameToClipboard);
    connect(ui->actionCopyPropertyNameToClipboard, SIGNAL(triggered()), this, SLOT(propertyCopyNameToClipboard()));

    QAction *actionSeparatorProperty2 = new QAction(this);
    actionSeparatorProperty2->setSeparator(true);
    ui->treeWidgetParameters->addAction(ui->actionCopyPropertyValueToClipboard);
    ui->treeWidgetState->addAction(ui->actionCopyPropertyValueToClipboard);
    ui->treeWidgetInputs->addAction(ui->actionCopyPropertyValueToClipboard);
    ui->treeWidgetOutputs->addAction(ui->actionCopyPropertyValueToClipboard);
    connect(ui->actionCopyPropertyValueToClipboard, SIGNAL(triggered()), this, SLOT(propertyCopyValueToClipboard()));

    QAction *actionSeparatorProperty3 = new QAction(this);
    actionSeparatorProperty3->setSeparator(true);
    ui->treeWidgetParameters->addAction(ui->actionPastePropertyValueFromClipboard);
    ui->treeWidgetState->addAction(ui->actionPastePropertyValueFromClipboard);
    ui->treeWidgetInputs->addAction(ui->actionPastePropertyValueFromClipboard);
    ui->treeWidgetOutputs->addAction(ui->actionPastePropertyValueFromClipboard);
    connect(ui->actionPastePropertyValueFromClipboard, SIGNAL(triggered()), this, SLOT(propertyPasteValueFromClipboard()));

    // Favorites показываем только при наличии избранных свойств
    updateFavoritesTabVisibility(false, false);
}

UComponentsListWidgetModern::~UComponentsListWidgetModern()
{
    delete ui;
}

void UComponentsListWidgetModern::updateComponentsListFromScheme()
{
    UpdateInterface(true);
}

void UComponentsListWidgetModern::AUpdateInterface()
{
    NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UComponentsListModern"), accessibleName());

    QString oldRootItem = currentDrawComponentName;
    QString oldSelectedItem = selectedComponentLongName;

    int componentsListScrollMaximum = componentsTree->verticalScrollBar()->maximum();
    int componentsListScrollPosition = componentsTree->verticalScrollBar()->value();

    // Сохраняем состояние развернутости всех узлов перед очисткой дерева
    QSet<QString> expandedItems;
    {
        QTreeWidgetItemIterator it(componentsTree);
        while (*it) {
            QTreeWidgetItem *item = *it;
            if (item->isExpanded() && item->childCount() > 0) {
                QString itemName = item->data(0, Qt::UserRole).toString();
                if (!itemName.isEmpty())
                    expandedItems.insert(itemName);
            }
            ++it;
        }
    }

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Блокируем сигналы во время обновления интерфейса,
    // чтобы предотвратить вызов componentListItemSelectionChanged при восстановлении выделения
    // через setCurrentItem() в addComponentSons()
    m_isUpdatingFromScheme = true;
    componentsTree->blockSignals(true);

    if(lastSnapshot && !lastSnapshot->Components.isEmpty())
    {
        rebuildTreeFromSnapshot(lastSnapshot);
        if(expandedItems.isEmpty())
            componentsTree->expandAll();
        restoreTreeSelection(oldRootItem, oldSelectedItem);
        m_treeRebuildRetryCount = 0;
    }
    else
    {
        componentsTree->clear();

        QTreeWidgetItem *rootItem = new QTreeWidgetItem(componentsTree);
        rootItem->setText(0, "Model");
        rootItem->setData(0, Qt::UserRole, QString());
        rootItem->setExpanded(true);
        addComponentSons("", rootItem, oldRootItem, oldSelectedItem, expandedItems);

        applyFilter(rootItem);

        // При первой загрузке (expandedItems пустой) разворачиваем всё дерево
        if(expandedItems.isEmpty())
            componentsTree->expandAll();
        m_treeRebuildRetryCount = 0;
    }

    componentsTree->verticalScrollBar()->setMaximum(componentsListScrollMaximum);
    componentsTree->verticalScrollBar()->setValue(componentsListScrollPosition);

    // Разблокируем сигналы после завершения обновления
    componentsTree->blockSignals(false);

    // Сбрасываем флаг после небольшой задержки, чтобы все события успели обработаться
    QTimer::singleShot(0, [this]() {
        m_isUpdatingFromScheme = false;
    });

    if(channelsSelectionVisible)
    {
      redrawChannelsList();
    }
}
void UComponentsListWidgetModern::AClearInterface()
{
 componentsTree->clear();
}

void UComponentsListWidgetModern::ASaveParameters()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    settings.setValue("splitterState", ui->splitter->saveState());
    settings.setValue("treeWidgetInputsHeader", ui->treeWidgetInputs->header()->saveState());
    settings.setValue("treeWidgetOutputsHeader", ui->treeWidgetOutputs->header()->saveState());
    settings.setValue("treeWidgetParameters", ui->treeWidgetParameters->header()->saveState());
    settings.setValue("treeWidgetState", ui->treeWidgetState->header()->saveState());
    settings.endGroup();
}

void UComponentsListWidgetModern::ALoadParameters()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    ui->splitter->restoreState(settings.value("splitterState").toByteArray());
    ui->treeWidgetInputs->header()->restoreState(settings.value("treeWidgetInputsHeader").toByteArray());
    ui->treeWidgetOutputs->header()->restoreState(settings.value("treeWidgetOutputsHeader").toByteArray());
    ui->treeWidgetParameters->header()->restoreState(settings.value("treeWidgetParameters").toByteArray());
    ui->treeWidgetState->header()->restoreState(settings.value("treeWidgetState").toByteArray());
    settings.endGroup();
}

void UComponentsListWidgetModern::setVerticalOrientation(bool vertical)
{
    if (vertical)
        ui->splitter->setOrientation(Qt::Vertical);
    else
        ui->splitter->setOrientation(Qt::Horizontal);
}

QString UComponentsListWidgetModern::getSelectedComponentLongName()
{
    if(!componentsTree->currentItem())
      return "";
    else
    {
      if(ui->tabWidgetComponentInfo->currentWidget() == ui->tabFavorites)
      {
        QTreeWidgetItem* item = ui->treeWidgetFavorites->currentItem();
        if(item && item->childCount() == 0)
            return item->data(2, Qt::UserRole).toString();
        return "";
      }
      else
      {
        return componentsTree->currentItem()->data(0, Qt::UserRole).toString();
      }
    }

}

void UComponentsListWidgetModern::openTabN(int n)
{
    QWidget* tab = widgetFromLogicalTabIndex(n);
    if(!tab)
        return;
    if(ui->tabWidgetComponentInfo->indexOf(tab) < 0)
        return;
    ui->tabWidgetComponentInfo->setCurrentWidget(tab);
}

int UComponentsListWidgetModern::currentTabIndex()
{
    return logicalTabIndexFromWidget(ui->tabWidgetComponentInfo->currentWidget());
}

QString UComponentsListWidgetModern::getSelectedPropertyName()
{
  QWidget* tab = ui->tabWidgetComponentInfo->currentWidget();
  if(tab == ui->tabParameters)
  {
      if(!ui->treeWidgetParameters->currentItem())
        return "";
      return ui->treeWidgetParameters->currentItem()->data(0, Qt::DisplayRole).toString();
  }
  if(tab == ui->tabState)
  {
      if(!ui->treeWidgetState->currentItem())
        return "";
      return ui->treeWidgetState->currentItem()->data(0, Qt::DisplayRole).toString();
  }
  if(tab == ui->tabInputs)
  {
      if(!ui->treeWidgetInputs->currentItem())
        return "";
      return ui->treeWidgetInputs->currentItem()->data(0, Qt::DisplayRole).toString();
  }
  if(tab == ui->tabOutputs)
  {
      if(!ui->treeWidgetOutputs->currentItem())
        return "";
      return ui->treeWidgetOutputs->currentItem()->data(0, Qt::DisplayRole).toString();
  }
  if(tab == ui->tabFavorites)
  {
      QTreeWidgetItem* item = ui->treeWidgetFavorites->currentItem();
      if(!item || item->childCount() > 0)
        return "";
      return item->data(0, Qt::UserRole).toString();
  }
  return "";
}

int UComponentsListWidgetModern::getSelectedChannelIndex()
{
    return currentChannel;
}

/// пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
/// 0 - пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
/// 1 - пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
void UComponentsListWidgetModern::setChannelMode(int mode)
{
 channelMode=mode;
}

/// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
/// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
int UComponentsListWidgetModern::getWorkChannelIndex()
{
 return (channelMode == 0)?Core_GetSelectedChannelIndex():currentChannel;
}


void UComponentsListWidgetModern::setEnableTabN(int n, bool enable)
{
    QWidget* tab = widgetFromLogicalTabIndex(n);
    if(!tab)
      return;
    const int index = ui->tabWidgetComponentInfo->indexOf(tab);
    if(index >= 0)
      ui->tabWidgetComponentInfo->setTabEnabled(index, enable);
}

void UComponentsListWidgetModern::setChannelsListVisible(bool value)
{
    ui->listWidgetChannelSelection->setVisible(value);
    channelsSelectionVisible = value;
    redrawChannelsList();
}

void UComponentsListWidgetModern::componentListItemSelectionChanged()
{
    QTreeWidgetItem * item = componentsTree->currentItem();
    if(!item)
    {
        // DEBUG: Commented out to reduce log flood
        // QString logMsg = QString("[SELECTION_DEBUG] UComponentsListWidgetModern::componentListItemSelectionChanged: no current item, returning");
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        return;
    }

    selectedComponentLongName = item->data(0,Qt::UserRole).toString();
    m_propertyReloadRetryCount = 0;
    m_propertyReloadRetryTarget = selectedComponentLongName;

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Не эмитируем componentSelected если выделение обновляется программно из схемы
    // Это предотвращает бесконечный цикл: selectComponent -> componentSelected -> componentSingleClick -> selectComponent
    if(m_isUpdatingFromScheme)
    {
        // DEBUG: Commented out to reduce log flood
        // QString logMsg = QString("[SELECTION_DEBUG] UComponentsListWidgetModern::componentListItemSelectionChanged: skipping emit (m_isUpdatingFromScheme=true) for '%1'")
        //     .arg(selectedComponentLongName);
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        reloadPropertys();
        return;
    }

    // DEBUG: Commented out to reduce log flood
    // QString logMsg = QString("[SELECTION_DEBUG] UComponentsListWidgetModern::componentListItemSelectionChanged: current item changed, emitting componentSelected('%1')")
    //     .arg(selectedComponentLongName);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

    currentDrawComponentName = selectedComponentLongName;
    // Do not Env_Select on selection (TD-111 reverted): FindComponent stays at Model.
    reloadPropertys();
    emit componentSelected(selectedComponentLongName);
}

void UComponentsListWidgetModern::schedulePropertyReloadRetry()
{
    if(m_propertyReloadRetryTarget != selectedComponentLongName)
    {
        m_propertyReloadRetryCount = 0;
        m_propertyReloadRetryTarget = selectedComponentLongName;
    }
    if(m_propertyReloadRetryCount >= kMaxPropertyReloadRetries)
        return;

    ++m_propertyReloadRetryCount;
    QTimer::singleShot(200, this, [this]() {
        if(m_propertyReloadRetryTarget != selectedComponentLongName)
            return;
        reloadPropertys(true);
    });
}

void UComponentsListWidgetModern::scheduleTreeRebuildRetry()
{
    if(m_treeRebuildRetryCount >= kMaxTreeRebuildRetries)
        return;

    ++m_treeRebuildRetryCount;
    QTimer::singleShot(200, this, [this]() {
        UpdateInterface(true);
    });
}

void UComponentsListWidgetModern::restoreTreeSelection(const QString& oldRootItem,
                                                       const QString& oldSelectedItem)
{
    const QString selectName = !oldSelectedItem.isEmpty() ? oldSelectedItem : oldRootItem;
    if(selectName.isEmpty())
        return;

    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole).toString() == selectName)
        {
            QTreeWidgetItem* item = *iterator;
            while(item)
            {
                item->setExpanded(true);
                item = item->parent();
            }
            componentsTree->setCurrentItem(*iterator);
            componentsTree->scrollToItem(*iterator, QAbstractItemView::EnsureVisible);
            return;
        }
        ++iterator;
    }
}

void UComponentsListWidgetModern::reloadPropertys(bool forceReload)
{
    if(currentDrawPropertyComponentName == selectedComponentLongName && !forceReload)
      return;

    const QString targetComponent = selectedComponentLongName;

    std::map<std::string, std::string> Favorites;
    // описание класса компонента, нужно ниже для проверки алиасов избранных свойств
    RDK::UEPtr<RDK::UContainerDescription> class_desc;

    const QString classNameStr =
        componentClassNameFromModelScope(getWorkChannelIndex(), targetComponent);

    if(!classNameStr.isEmpty())
    {
        class_desc = RDK::GetStorageLock()->GetClassDescription(classNameStr.toLocal8Bit().constData(), true);

        if(class_desc)
            Favorites = class_desc->GetFavorites();

        ui->labelComponentClassName->setText(classNameStr);
    }

    // пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ treeWidget'пїЅпїЅ
    int paramScrollPosition = ui->treeWidgetParameters->verticalScrollBar()->value(),
        stateScrollPosition = ui->treeWidgetState->verticalScrollBar()->value(),
        inputsScrollPosition = ui->treeWidgetInputs->verticalScrollBar()->value(),
        outputsScrollPosition = ui->treeWidgetOutputs->verticalScrollBar()->value(),
        favoritesScrollPosition = ui->treeWidgetFavorites->verticalScrollBar()->value();

    try
    {
     UpdateInterfaceFlag=true;
        // Use timeout to avoid blocking UI for too long during calculation
        RDK::UELockPtr<RDK::UContainer> model =
            RDK::GetModelLockTimeout(getWorkChannelIndex(), kModelLockTimeoutMs);
        if (!model) {
            UpdateInterfaceFlag=false;
            schedulePropertyReloadRetry();
            return;
        }

        RDK::UEPtr<RDK::UContainer> cont;
        if (targetComponent.isEmpty())
            cont = model.Get();
        else
            cont = model->GetComponentL(targetComponent.toLocal8Bit().constData(), true);

        if(!cont)
        {
         UpdateInterfaceFlag=false;
         schedulePropertyReloadRetry();
         return;
        }

    ui->treeWidgetParameters->clear();
    ui->treeWidgetState->clear();
    ui->treeWidgetInputs->clear();
    ui->treeWidgetOutputs->clear();
    ui->treeWidgetFavorites->clear();
        RDK::UComponent::VariableMapT varMap = cont->GetPropertiesList();
        std::string buffer;

        bool is_new_outputs(false);
        bool is_new_inputs(false);

        for(RDK::UComponent::VariableMapIteratorT i = varMap.begin(); i != varMap.end(); ++i)
        {
            if (i->second.CheckMask(ptPubInput))
            {
             std::string::size_type k=i->first.find("DataInput");
             if(k != 0 || k == std::string::npos)
              is_new_inputs=true;
            }

            if (i->second.CheckMask(ptPubOutput))
            {
             std::string::size_type k=i->first.find("DataOutput");
             if(k != 0 || k == std::string::npos)
              is_new_outputs=true;
            }

            if(is_new_inputs && is_new_outputs)
             break;
        }

        for(RDK::UComponent::VariableMapIteratorT i = varMap.begin(); i != varMap.end();)
        {
            if (i->second.CheckMask(ptPubInput) && is_new_inputs)
            {
             std::string::size_type k=i->first.find("DataInput");
             if(k == 0)
             {
              i = varMap.erase(i);
             }
             else
              ++i;
            }
            else
            if (i->second.CheckMask(ptPubOutput) && is_new_outputs)
            {
             std::string::size_type k=i->first.find("DataOutput");
             if(k == 0)
             {
              i = varMap.erase(i);
             }
             else
              ++i;
            }
            else
            {
             ++i;
            }
        }


        for(RDK::UComponent::VariableMapIteratorT i = varMap.begin(); i != varMap.end(); ++i)
        {
            if (i->second.CheckMask(ptPubParameter) && ui->tabWidgetComponentInfo->currentWidget() == ui->tabParameters)
            {
                QTreeWidgetItem* parametersItem = new QTreeWidgetItem(ui->treeWidgetParameters);
                QString parameterName = QString::fromLocal8Bit(i->first.c_str());
                parametersItem->setText(0, parameterName);
                cont->GetPropertyValue(i->first, buffer);
                parametersItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
                parametersItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                if(parameterName == selectedParameterName)
                    ui->treeWidgetParameters->setCurrentItem(parametersItem);
                if(i->second.Property->GetLanguageType() == typeid(bool))
                {
                 parametersItem->setFlags(parametersItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

                 const bool* val=reinterpret_cast<const bool*>(i->second.Property->GetMemoryArea());
                 if(*val)
                  parametersItem->setCheckState(1,Qt::Checked);
                 else
                  parametersItem->setCheckState(1,Qt::Unchecked);
                }
            }
            if (i->second.CheckMask(ptPubState) && ui->tabWidgetComponentInfo->currentWidget() == ui->tabState)
            {
                QTreeWidgetItem* stateItem = new QTreeWidgetItem(ui->treeWidgetState);
                QString stateName = QString::fromLocal8Bit(i->first.c_str());
                stateItem->setText(0, stateName);
                cont->GetPropertyValue(i->first, buffer);
                stateItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
                stateItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                if(stateName == selectedStateName)
                    ui->treeWidgetState->setCurrentItem(stateItem);
            }
            if (i->second.CheckMask(ptPubInput) && ui->tabWidgetComponentInfo->currentWidget() == ui->tabInputs)
            {
                QTreeWidgetItem* inputItem = new QTreeWidgetItem(ui->treeWidgetInputs);
                QString inputName = QString::fromLocal8Bit(i->first.c_str());
                inputItem->setText(0, inputName);
                //inputItem->setText(1, QString::fromLocal8Bit(cont->GetPropertyValue(i->first, buffer).c_str()));
                inputItem->setText(2, QString(i->second.Property->GetLanguageType().name()));
                /*if(i->second.Property->GetIoType() & ipRange) inputItem->setText(3, QString("range"));
                else
                    if(i->second.Property->IsConnected()) inputItem->setText(3, QString("connected"));*/
                if(inputName == selectedInputName)
                    ui->treeWidgetInputs->setCurrentItem(inputItem);
            }
            if (i->second.CheckMask(ptPubOutput) && ui->tabWidgetComponentInfo->currentWidget() == ui->tabOutputs)
            {
                QTreeWidgetItem* outputItem = new QTreeWidgetItem(ui->treeWidgetOutputs);
                QString outputName = QString::fromLocal8Bit(i->first.c_str());
                outputItem->setText(0, outputName);
                cont->GetPropertyValue(i->first, buffer);
                outputItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
                outputItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                outputItem->setText(2, QString(i->second.Property->GetLanguageType().name()));
                if(outputName == selectedOutputName)
                    ui->treeWidgetOutputs->setCurrentItem(outputItem);
            }
        }


        const bool favoritesTabWasPresent =
            ui->tabWidgetComponentInfo->indexOf(ui->tabFavorites) >= 0;
        const bool componentChangedForFavorites =
            (currentDrawPropertyComponentName != targetComponent);

        QTreeWidgetItem* groupParameters = new QTreeWidgetItem(ui->treeWidgetFavorites);
        groupParameters->setText(0, tr("Parameters"));
        QTreeWidgetItem* groupOutputs = new QTreeWidgetItem(ui->treeWidgetFavorites);
        groupOutputs->setText(0, tr("Outputs"));
        QTreeWidgetItem* groupInputs = new QTreeWidgetItem(ui->treeWidgetFavorites);
        groupInputs->setText(0, tr("Inputs"));
        QTreeWidgetItem* groupStates = new QTreeWidgetItem(ui->treeWidgetFavorites);
        groupStates->setText(0, tr("States"));

        for(std::map<std::string, std::string>::iterator i = Favorites.begin(); i != Favorites.end(); ++i)
        {
            QString favoriteName = QString::fromLocal8Bit(i->first.c_str());
            QString favoritePath = QString::fromLocal8Bit(i->second.c_str());
            favoritePath.replace("{CompName}", targetComponent);

            // Проверяем, является ли это алиасом
            bool isAlias = class_desc && class_desc->IsFavoriteAlias(i->first);

            // Parse path - для алиасов путь может быть в формате "ComponentPath.PropertyName"
            QString component_long_name;
            QString prop_name;

            if (isAlias && class_desc)
            {
                // Для алиаса разбираем путь через ParseFavoritePath
                std::string componentPath, propertyName;
                if (class_desc->ParseFavoritePath(i->second, componentPath, propertyName))
                {
                    // Формируем полный путь: текущий компонент + путь к вложенному компоненту
                    component_long_name = targetComponent;
                    if (!componentPath.empty())
                    {
                        component_long_name += "." + QString::fromStdString(componentPath);
                    }
                    prop_name = QString::fromStdString(propertyName);
                }
            }
            else
            {
                // Старый формат: "ComponentName:PropertyName"
                QStringList vals = favoritePath.split(":");
                if(vals.size()==2)
                {
                    component_long_name = vals[0];
                    prop_name = vals[1];
                }
            }

            RDK::UEPtr<RDK::UContainer> child_cont;
            child_cont = model->GetComponentL(component_long_name.toLocal8Bit().constData(), true);

            QTreeWidgetItem* groupParent = groupParameters;
            if(child_cont)
            {
                RDK::UComponent::VariableMapT childVars = child_cont->GetPropertiesList();
                RDK::UComponent::VariableMapIteratorT propIt = childVars.find(prop_name.toStdString());
                if(propIt != childVars.end())
                {
                    // Приоритет роли: Parameter > Output > Input > State
                    if(propIt->second.CheckMask(ptPubParameter))
                        groupParent = groupParameters;
                    else if(propIt->second.CheckMask(ptPubOutput))
                        groupParent = groupOutputs;
                    else if(propIt->second.CheckMask(ptPubInput))
                        groupParent = groupInputs;
                    else if(propIt->second.CheckMask(ptPubState))
                        groupParent = groupStates;
                }
            }

            QTreeWidgetItem* favoriteItem = new QTreeWidgetItem(groupParent);

            if (isAlias)
                favoriteItem->setText(0, favoriteName + " [Alias]");
            else
                favoriteItem->setText(0, favoriteName);

            favoriteItem->setText(2, favoritePath);
            favoriteItem->setToolTip(0, favoritePath);
            favoriteItem->setToolTip(1, favoritePath);
            favoriteItem->setToolTip(2, favoritePath);

            favoriteItem->setData(0, Qt::UserRole, prop_name);
            favoriteItem->setData(2, Qt::UserRole, component_long_name);

            if(child_cont)
            {
                child_cont->GetPropertyValue(prop_name.toStdString(), buffer);
                auto prop = child_cont->FindProperty(prop_name.toStdString());

                favoriteItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
                favoriteItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));

                if(favoriteItem->text(0) == selectedFavName)
                    ui->treeWidgetFavorites->setCurrentItem(favoriteItem);

                if(prop && prop->GetLanguageType() == typeid(bool))
                {
                 favoriteItem->setFlags(favoriteItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

                 const bool* val=reinterpret_cast<const bool*>(prop->GetMemoryArea());
                 if(*val)
                  favoriteItem->setCheckState(1,Qt::Checked);
                 else
                  favoriteItem->setCheckState(1,Qt::Unchecked);
                }
            }

        }

        // Удаляем пустые группы, оставшиеся разворачиваем
        QList<QTreeWidgetItem*> groups;
        groups << groupParameters << groupOutputs << groupInputs << groupStates;
        for(QTreeWidgetItem* group : groups)
        {
            if(group->childCount() == 0)
                delete group;
            else
                group->setExpanded(true);
        }

        const bool hasFavorites = !Favorites.empty();
        const bool selectFavorites =
            hasFavorites && (componentChangedForFavorites || !favoritesTabWasPresent);

        // пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ treeWidget'пїЅпїЅ
        ui->treeWidgetParameters->verticalScrollBar()->setMaximum(paramScrollPosition);
        ui->treeWidgetParameters->verticalScrollBar()->setValue(paramScrollPosition);
        ui->treeWidgetState->verticalScrollBar()->setMaximum(stateScrollPosition);
        ui->treeWidgetState->verticalScrollBar()->setValue(stateScrollPosition);
        ui->treeWidgetInputs->verticalScrollBar()->setMaximum(inputsScrollPosition);
        ui->treeWidgetInputs->verticalScrollBar()->setValue(inputsScrollPosition);
        ui->treeWidgetOutputs->verticalScrollBar()->setMaximum(outputsScrollPosition);
        ui->treeWidgetOutputs->verticalScrollBar()->setValue(outputsScrollPosition);
        ui->treeWidgetFavorites->verticalScrollBar()->setMaximum(favoritesScrollPosition);
        ui->treeWidgetFavorites->verticalScrollBar()->setValue(favoritesScrollPosition);
        currentDrawPropertyComponentName = targetComponent;
        m_propertyReloadRetryCount = 0;
        UpdateInterfaceFlag=false;

        updateFavoritesTabVisibility(hasFavorites, selectFavorites);
    }
    catch (RDK::UException &exception)
    {
     UpdateInterfaceFlag=false;
        schedulePropertyReloadRetry();
        RDK::Logging::SystemLog(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
     UpdateInterfaceFlag=false;
        schedulePropertyReloadRetry();
        RDK::Logging::SystemLog(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
}

void UComponentsListWidgetModern::parametersListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetParameters->currentItem();
    if(!item)
      return;

    QString changedParameterName = item->data(0, Qt::DisplayRole).toString();
    if(selectedParameterName == changedParameterName)
      return;

    selectedParameterName = changedParameterName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::parametersListItemChanged(QTreeWidgetItem *item, int column)
{
 try
 {
  if(UpdateInterfaceFlag)
   return;
  // Use timeout to avoid blocking UI during calculation
  RDK::UELockPtr<RDK::UContainer> model =
      RDK::GetModelLockTimeout(getWorkChannelIndex(), kModelLockTimeoutMs);
  if (!model)
  {
   schedulePropertyReloadRetry();
   return;
  }

  RDK::UEPtr<RDK::UContainer> cont;
  if (currentDrawPropertyComponentName.isEmpty())
   cont = model.Get();
  else
   cont = model->GetComponentL(currentDrawPropertyComponentName.toLocal8Bit().constData(), true);

  if(!cont)
   return;

  QString parameterName=item->text(0);
  RDK::UEPtr<RDK::UIProperty> property;

  property=cont->FindProperty(parameterName.toLocal8Bit().constData());

  if(!property)
   return;

  if(property->GetLanguageType() == typeid(bool))
  {
   bool value(false);
   if(item->checkState(1) == Qt::Checked)
    value=true;
   else
    value=false;
   property->ReadFromMemory(&value);
  }
 }
catch (RDK::UException &exception)
{
    RDK::Logging::SystemLog(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
}
catch (std::exception &exception)
{
    RDK::Logging::SystemLog(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
}
}

void UComponentsListWidgetModern::stateListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetState->currentItem();
    if(!item)
      return;

    QString changedStateName = item->data(0, Qt::DisplayRole).toString();
    if(selectedStateName == changedStateName)
      return;

    selectedStateName = changedStateName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::inputsListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetInputs->currentItem();
    if(!item)
      return;

    QString changedInputName = item->data(0, Qt::DisplayRole).toString();
    if(selectedInputName == changedInputName)
      return;

    selectedInputName = changedInputName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::outputsListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetOutputs->currentItem();
    if(!item)
      return;

    QString changedOutputName = item->data(0, Qt::DisplayRole).toString();
    if(selectedOutputName == changedOutputName)
      return;

    selectedOutputName = changedOutputName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::favoritesListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetFavorites->currentItem();
    if(!item || item->childCount() > 0)
      return;

    if(item->data(0, Qt::UserRole).toString().isEmpty())
      return;

    QString changedFavName = item->data(0, Qt::DisplayRole).toString();
    if(selectedFavName == changedFavName)
      return;

    selectedFavName = changedFavName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidgetModern::favoritesListItemChanged(QTreeWidgetItem *item, int column)
{

try
{
    if(UpdateInterfaceFlag)
        return;

     if(!item || item->childCount() > 0)
        return;

     QString prop_name = item->data(0, Qt::UserRole).toString();
     QString component_long_name = item->data(2, Qt::UserRole).toString();
     if(prop_name.isEmpty() || component_long_name.isEmpty())
        return;

     // Use timeout to avoid blocking UI during calculation
     RDK::UELockPtr<RDK::UContainer> model =
         RDK::GetModelLockTimeout(getWorkChannelIndex(), kModelLockTimeoutMs);
     if (!model)
      return;

     RDK::UEPtr<RDK::UContainer> cont;

     cont = model->GetComponentL(component_long_name.toLocal8Bit().constData(), true);

     if(!cont)
      return;

     RDK::UEPtr<RDK::UIProperty> property;

     property=cont->FindProperty(prop_name.toLocal8Bit().constData());

     if(!property)
      return;

     if(property->GetLanguageType() == typeid(bool))
     {
      bool value(false);
      if(item->checkState(1) == Qt::Checked)
       value=true;
      else
       value=false;
      property->ReadFromMemory(&value);
     }
}

catch (RDK::UException &exception)
{
    RDK::Logging::SystemLog(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
}
catch (std::exception &exception)
{
    RDK::Logging::SystemLog(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
}

}

void UComponentsListWidgetModern::handleSnapshotUpdated(NMSDK::UGuiSnapshotPtr snapshot,
                                                  const QStringList &,
                                                  const QStringList &,
                                                  const QStringList &)
{
    lastSnapshot = snapshot;
    if (!snapshot)
        return;

    if (UpdateInterfaceFlag) {
        QMetaObject::invokeMethod(this, [this]() { UpdateInterface(true); }, Qt::QueuedConnection);
    } else {
        UpdateInterface(true);
    }
}

void UComponentsListWidgetModern::handleFilterTextChanged(const QString &text)
{
    if(!componentsTree)
        return;

    componentFilterText = text.trimmed();
    applyFilter(componentsTree->invisibleRootItem());

    // Обновляем виджет для отображения изменений
    componentsTree->update();
}

void UComponentsListWidgetModern::rebuildTreeFromSnapshot(const NMSDK::UGuiSnapshotPtr &snapshot)
{
    if (!snapshot)
        return;

    const int scrollMax = componentsTree->verticalScrollBar()->maximum();
    const int scrollPos = componentsTree->verticalScrollBar()->value();

    QSignalBlocker blocker(componentsTree);
    componentsTree->clear();

    auto *rootItem = new QTreeWidgetItem(componentsTree);
    rootItem->setText(0, tr("Model"));
    rootItem->setData(0, Qt::UserRole, QString());
    rootItem->setExpanded(true);

    QHash<QString, QTreeWidgetItem*> items;
    items.insert(QString(), rootItem);

    const auto componentNames = snapshot->Components.keys();
    for (const QString &name : componentNames) {
        const auto summary = snapshot->Components.value(name);
        QTreeWidgetItem *parent = items.value(summary.ParentName, rootItem);
        if (!parent)
            parent = rootItem;
        auto *item = new QTreeWidgetItem(parent);
        item->setText(0, summary.ShortName);
        item->setToolTip(0, summary.LongName + QStringLiteral("\n") + summary.ClassName);
        item->setData(0, Qt::UserRole, summary.LongName);
        items.insert(summary.LongName, item);
    }

    applyFilter(rootItem);
    componentsTree->verticalScrollBar()->setMaximum(scrollMax);
    componentsTree->verticalScrollBar()->setValue(scrollPos);

    if(channelsSelectionVisible)
    {
      redrawChannelsList();
    }
}

bool UComponentsListWidgetModern::applyFilter(QTreeWidgetItem *item)
{
    if (!item)
        return false;

    bool matches = componentFilterText.isEmpty()
            || item->text(0).contains(componentFilterText, Qt::CaseInsensitive)
            || item->data(0, Qt::UserRole).toString().contains(componentFilterText, Qt::CaseInsensitive);

    bool childMatches = false;
    for (int i = 0; i < item->childCount(); ++i) {
        childMatches |= applyFilter(item->child(i));
    }

    const bool isRoot = item->data(0, Qt::UserRole).toString().isEmpty();
    const bool visible = matches || childMatches || isRoot;
    item->setHidden(!visible);
    // Разворачиваем узлы только при активном фильтре; при пустом фильтре не меняем развёрнутость
    if (visible && matches && !isRoot && !componentFilterText.isEmpty()) {
        item->setExpanded(true);
    }
    return visible;
}

void UComponentsListWidgetModern::componentSelectedFromScheme(QString name)
{
    const QString resolvedName =
        resolveComponentLongNameFromModelRoot(getWorkChannelIndex(), name);

    m_propertyReloadRetryCount = 0;
    m_propertyReloadRetryTarget = resolvedName;
    selectedComponentLongName = resolvedName;
    currentDrawComponentName = resolvedName;

    m_isUpdatingFromScheme = true;
    componentsTree->blockSignals(true);

    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole).toString() == resolvedName)
        {
            QTreeWidgetItem *item = *iterator;
            while(item)
            {
                item->setExpanded(true);
                item = item->parent();
            }

            componentsTree->setCurrentItem(*iterator);
            componentsTree->scrollToItem(*iterator, QAbstractItemView::EnsureVisible);
            break;
        }
        ++iterator;
    }

    componentsTree->blockSignals(false);
    reloadPropertys(true);

    QTimer::singleShot(0, [this]() {
        m_isUpdatingFromScheme = false;
    });
}

void UComponentsListWidgetModern::componentDoubleClickFromScheme(QString name)
{
    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole) == name)
        {
            componentsTree->setCurrentItem(*iterator);
            (*iterator)->setExpanded(true);
            currentDrawComponentName = (*iterator)->data(0, Qt::UserRole).toString();
            return;
        }
        ++iterator;
    }
}

void UComponentsListWidgetModern::componentStapBackFromScheme()
{
    QStringList list = currentDrawComponentName.split(".");
    if (list.isEmpty()) return;
    list.pop_back();
    currentDrawComponentName = list.join(".");
    componentSelectedFromScheme(currentDrawComponentName);
    emit componentSelected(currentDrawComponentName);
}

void UComponentsListWidgetModern::channelsListSelectionChanged()
{
  if(ui->listWidgetChannelSelection->currentItem())
  {
    currentChannel = ui->listWidgetChannelSelection->currentItem()->data(Qt::UserRole).toInt();
  }
}

void UComponentsListWidgetModern::onComponentItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if(!item) return;

    // Раскрываем путь к компоненту (все родительские элементы)
    QTreeWidgetItem *currentItem = item;
    while (currentItem) {
        currentItem->setExpanded(true);
        currentItem = currentItem->parent();
    }

    // Устанавливаем текущий элемент (если еще не установлен)
    if(componentsTree->currentItem() != item) {
        componentsTree->setCurrentItem(item);
    }

    // Прокручиваем к выбранному элементу
    componentsTree->scrollToItem(item, QAbstractItemView::EnsureVisible);
}

void UComponentsListWidgetModern::drawSelectedComponent(QModelIndex index)
{
    // Получаем данные компонента из index
    QString componentName = index.data(Qt::UserRole).toString();
    if(componentName.isEmpty()) return;

    // Находим элемент в дереве по данным
    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole) == componentName)
        {
            // Устанавливаем текущий элемент
            componentsTree->setCurrentItem(*iterator);

            // Вызываем обработчик выбора компонента (как при одинарном клике)
            componentListItemSelectionChanged();

            // Закрываем popup после выбора компонента
            if (treePopupDialog->isVisible()) {
                hideTreePopup();
            }
            return;
        }
        ++iterator;
    }
}


void UComponentsListWidgetModern::componentMoveUp()
{
    if(componentsTree->currentItem())
    {
        MModel_ChangeComponentPosition(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit(),-1);
        UpdateInterface(true);
    }
}

void UComponentsListWidgetModern::componentMoveDown()
{
    if(componentsTree->currentItem())
    {
        MModel_ChangeComponentPosition(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit(), 1);
        UpdateInterface(true);
    }
}

void UComponentsListWidgetModern::componentRename()
{
    if(componentsTree->currentItem())
    {
        bool ok;
        QString restoreName = componentsTree->currentItem()->data(0, Qt::DisplayRole).toString();
        QString text = QInputDialog::getText(this, tr("Rename component"),
                                             tr("Enter new component name: "), QLineEdit::Normal,
                                             restoreName, &ok);
        if (ok && !text.isEmpty())
        {
            std::string new_name(text.toLocal8Bit());
            MModel_SetComponentPropertyData(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit(),"Name", &new_name);

            QStringList nameSeparator = selectedComponentLongName.split(".");
            nameSeparator.pop_back();
            nameSeparator.push_back(text);
            selectedComponentLongName = nameSeparator.join(".");
            emit updateScheme(true);
            UpdateInterface(true);
        }
    }
}

void UComponentsListWidgetModern::componentDelete()
{
    if(componentsTree->currentItem())
    {
        if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Are you sure you want to delete component "+selectedComponentLongName+"?", QMessageBox::Yes|QMessageBox::Cancel);
            if (reply != QMessageBox::Yes) return;
        }

        const int rc = MModel_DelComponent(getWorkChannelIndex(), "", selectedComponentLongName.toLocal8Bit());
        if(rc != RDK_SUCCESS)
        {
            QMessageBox::warning(this, "Warning",
                QString("Failed to delete component %1 (code %2).")
                    .arg(selectedComponentLongName).arg(rc));
            return;
        }
        RDK::UIVisualControllerStorage::UpdateInterface(true);
        emit updateScheme(true);
    }
}

void UComponentsListWidgetModern::componentCopyNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QStringList list = selectedComponentLongName.split(".");
    QString name = list.last();
    clipboard->setText(name);
}

void UComponentsListWidgetModern::componentCopyLongNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(selectedComponentLongName);
}

void UComponentsListWidgetModern::componentCopyClassNameToClipboard()
{
    const QString className =
        componentClassNameFromModelScope(getWorkChannelIndex(), selectedComponentLongName);
    if(!className.isEmpty())
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(className);
    }
}

void UComponentsListWidgetModern::componentReset()
{
    if(componentsTree->currentItem())
    {
        Env_Reset(selectedComponentLongName.toLocal8Bit());
        UpdateInterface(true);
    }
}

void UComponentsListWidgetModern::componentCalculate()
{
    if(componentsTree->currentItem())
    {
        Env_Calculate(selectedComponentLongName.toLocal8Bit());
        RDK::UIVisualControllerStorage::UpdateInterface();
    }
}

void UComponentsListWidgetModern::componentInit()
{
 if(componentsTree->currentItem())
 {
     Env_ModelInit(selectedComponentLongName.toLocal8Bit());
     RDK::UIVisualControllerStorage::UpdateInterface();
 }
}

void UComponentsListWidgetModern::componentUnInit()
{
 if(componentsTree->currentItem())
 {
     Env_ModelUnInit(selectedComponentLongName.toLocal8Bit());
     RDK::UIVisualControllerStorage::UpdateInterface();
 }
}

void UComponentsListWidgetModern::componentGUI()
{
    if(!componentsTree->currentItem() || selectedComponentLongName.isEmpty())
        return;

    const QString componentClassName =
        componentClassNameFromModelScope(getWorkChannelIndex(), selectedComponentLongName);

    UComponentGuiContext context;
    context.componentLongName = selectedComponentLongName;
    context.componentClassName = componentClassName;
    context.channelIndex = Core_GetSelectedChannelIndex();

    emit openComponentGuiRequested(context);

    if(receivers(SIGNAL(openComponentGuiRequested(UComponentGuiContext))) == 0)
    {
        if(!UComponentFormRegistry::instance().canOpen(context))
        {
            QMessageBox::information(this, "Component GUI", "Component GUI form is not registered for this class.");
            return;
        }
        static UComponentGuiService fallbackService;
        fallbackService.setApplication(application);
        if(!fallbackService.createOrActivate(this, context))
            QMessageBox::information(this, "Component GUI", "Failed to open component GUI form.");
    }
}

void UComponentsListWidgetModern::setUpdateInterval(long value)
{
  UpdateInterval = value;
}

void UComponentsListWidgetModern::addComponentSons(QString componentName, QTreeWidgetItem *treeWidgetFather, QString oldRootItem, QString oldSelectedItem, const QSet<QString> &expandedItems)
{
 // Use timeout to avoid blocking UI during calculation
 RDK::UELockPtr<RDK::UEngine> engine=
     RDK::GetEngineLockTimeout<RDK::UEngine>(getWorkChannelIndex(), kModelLockTimeoutMs);
 if (!engine)
 {
  scheduleTreeRebuildRetry();
  return;
 }
    const char * stringBuff = MModel_GetComponentsNameList(getWorkChannelIndex(), componentName.toLocal8Bit());
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    QString str;
    if(!componentNames.empty()&&componentNames[0]!="")
    {
        QString father;
        if(!componentName.isEmpty()) father = componentName + ".";
        foreach(str, componentNames)
        {
            QString fullName = father + str;
            QTreeWidgetItem* childItem = new QTreeWidgetItem(treeWidgetFather);
            childItem->setText(0, str);
            childItem->setData(0, Qt::UserRole, fullName);

            if (!expandedItems.isEmpty() && expandedItems.contains(fullName))
                childItem->setExpanded(true);

            if(oldRootItem == fullName)
            {
                componentsTree->setCurrentItem(childItem);
                childItem->setExpanded(true);
            }
            if(oldSelectedItem == fullName)
            {
                componentsTree->setCurrentItem(childItem);
                childItem->setExpanded(true);
            }

            addComponentSons(father+str, childItem, oldRootItem, oldSelectedItem, expandedItems);
        }
    }
}

void UComponentsListWidgetModern::redrawChannelsList()
{
  ui->listWidgetChannelSelection->clear();
  int channelsCounter = Core_GetNumChannels();
  for(int i = 0; i < channelsCounter; i++)
  {
    QListWidgetItem *item = new QListWidgetItem(ui->listWidgetChannelSelection);
    item->setText(QString::number(i) + " ch.");
    item->setData(Qt::UserRole, i);
    if(i == currentChannel)
      ui->listWidgetChannelSelection->setCurrentItem(item);
  }
}

/// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
std::string& UComponentsListWidgetModern::EraseLeadEndls(std::string &value)
{
 std::string::size_type data_i=value.find_first_of("\n");
 if(data_i != std::string::npos)
 {
  value.erase(value.begin(),value.begin()+data_i+1);
 }
 return value;
}

/// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
std::string& UComponentsListWidgetModern::EraseRangeEndls(std::string &value)
{
 std::string::size_type data_i=value.find_first_of("\n");
 if(data_i != std::string::npos && data_i<2)
 {
  value.erase(value.begin(),value.begin()+data_i+1);
 }

 data_i=value.find_last_of("\r\n");
 if(data_i != std::string::npos && data_i>=value.size()-1)
 {
  value.erase(value.begin()+data_i,value.end());
 }
 return value;
}

/// пїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ, пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
/// пїЅпїЅ "[SEE BELOW]"
std::string& UComponentsListWidgetModern::PreparePropertyValueToListView(std::string &value)
{
 EraseRangeEndls(value);
 if(value.empty())
 {
  value="[EMPTY]";
 }
 else
 {
  if(value.find_first_of("\n") != std::string::npos)
   value="[SEE BELOW]";
 }
 return value;
}


void UComponentsListWidgetModern::propertyCopyNameToClipboard()
{
 QClipboard *clipboard = QApplication::clipboard();

 QString value;
 QWidget* tab = ui->tabWidgetComponentInfo->currentWidget();
 if(tab == ui->tabParameters)
 {
     if(ui->treeWidgetParameters->currentItem())
       value=ui->treeWidgetParameters->currentItem()->data(0, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabState)
 {
     if(ui->treeWidgetState->currentItem())
       value=ui->treeWidgetState->currentItem()->data(0, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabInputs)
 {
     if(ui->treeWidgetInputs->currentItem())
       value=ui->treeWidgetInputs->currentItem()->data(0, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabOutputs)
 {
     if(ui->treeWidgetOutputs->currentItem())
       value=ui->treeWidgetOutputs->currentItem()->data(0, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabFavorites)
 {
     QTreeWidgetItem* item = ui->treeWidgetFavorites->currentItem();
     if(item && item->childCount() == 0)
       value=item->data(0, Qt::UserRole).toString();
 }

 clipboard->setText(value);
}

void UComponentsListWidgetModern::propertyCopyValueToClipboard()
{
 QClipboard *clipboard = QApplication::clipboard();

 QString value;
 QWidget* tab = ui->tabWidgetComponentInfo->currentWidget();
 if(tab == ui->tabParameters)
 {
     if(ui->treeWidgetParameters->currentItem())
       value=ui->treeWidgetParameters->currentItem()->data(1, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabState)
 {
     if(ui->treeWidgetState->currentItem())
       value=ui->treeWidgetState->currentItem()->data(1, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabInputs)
 {
     if(ui->treeWidgetInputs->currentItem())
       value=ui->treeWidgetInputs->currentItem()->data(1, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabOutputs)
 {
     if(ui->treeWidgetOutputs->currentItem())
       value=ui->treeWidgetOutputs->currentItem()->data(1, Qt::DisplayRole).toString();
 }
 else if(tab == ui->tabFavorites)
 {
     QTreeWidgetItem* item = ui->treeWidgetFavorites->currentItem();
     if(item && item->childCount() == 0)
       value=item->data(1, Qt::DisplayRole).toString();
 }

 clipboard->setText(value);
}

void UComponentsListWidgetModern::propertyPasteValueFromClipboard()
{
 QClipboard *clipboard = QApplication::clipboard();

 QString value=clipboard->text();
 QWidget* tab = ui->tabWidgetComponentInfo->currentWidget();
 if(tab == ui->tabParameters)
 {
     if(ui->treeWidgetParameters->currentItem())
     {
      ui->treeWidgetParameters->currentItem()->setData(1, Qt::UserRole, value);
      std::string temp=value.toLocal8Bit().constData();
      ui->treeWidgetParameters->currentItem()->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
     }
 }
 else if(tab == ui->tabState)
 {
     if(ui->treeWidgetState->currentItem())
     {
      ui->treeWidgetState->currentItem()->setData(1, Qt::UserRole, value);
      std::string temp=value.toLocal8Bit().constData();
      ui->treeWidgetState->currentItem()->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
     }
 }
 else if(tab == ui->tabInputs)
 {
     if(ui->treeWidgetInputs->currentItem())
     {
      ui->treeWidgetInputs->currentItem()->setData(1, Qt::UserRole, value);
      std::string temp=value.toLocal8Bit().constData();
      ui->treeWidgetInputs->currentItem()->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
     }
 }
 else if(tab == ui->tabOutputs)
 {
     if(ui->treeWidgetOutputs->currentItem())
     {
      ui->treeWidgetOutputs->currentItem()->setData(1, Qt::UserRole, value);
      std::string temp=value.toLocal8Bit().constData();
      ui->treeWidgetOutputs->currentItem()->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
     }
 }
 else if(tab == ui->tabFavorites)
 {
     QTreeWidgetItem* item = ui->treeWidgetFavorites->currentItem();
     if(item && item->childCount() == 0)
     {
      item->setData(1, Qt::UserRole, value);
      std::string temp=value.toLocal8Bit().constData();
      item->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
     }
 }
}


void UComponentsListWidgetModern::on_actionReloadTree_triggered()
{
    UpdateInterface(true);
}


void UComponentsListWidgetModern::on_tabWidgetComponentInfo_currentChanged(int index)
{
    reloadPropertys(true);
}


void UComponentsListWidgetModern::on_actionDefaultAllParameters_triggered()
{
    if(componentsTree->currentItem())
    {
        if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Are you sure you want to reset all parameters for component "+selectedComponentLongName+" to default values?", QMessageBox::Yes|QMessageBox::Cancel);
            if (reply == QMessageBox::Cancel) return;
        }

        RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
        std::string stringid = selectedComponentLongName.toLocal8Bit().constData();
        RDK::UEPtr<RDK::UNet> object;
        if(stringid.empty())
         object=RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetModel());
        else
         object=RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetEngine()->FindComponent(stringid.c_str()));

        RDK::UEPtr<RDK::UNet> owner = RDK::dynamic_pointer_cast<RDK::UNet>(object->GetOwner());
        RDK::UStringLinksList links_list;

        if(owner)
         object->GetLinks(links_list, owner, true, object);
        storage->DefaultObject(object);
        if(owner)
         object->CreateLinks(links_list, owner);
        RDK::UIVisualControllerStorage::UpdateInterface(true);
    }
}

void UComponentsListWidgetModern::toggleTreeViewMode()
{
    if (treePopupDialog->isVisible()) {
        // Если popup открыт, закрываем его
        hideTreePopup();
    } else {
        // Если popup закрыт, открываем его
        showTreePopup();
    }
}


void UComponentsListWidgetModern::showTreePopup()
{
    // Перемещаем виджеты из основного контейнера в popup
    QWidget *treeContainer = ui->treeContainer;
    QVBoxLayout *treeLayout = qobject_cast<QVBoxLayout*>(treeContainer->layout());

    if (treeLayout) {
        // Удаляем виджеты из основного layout
        treeLayout->removeWidget(filterLineEdit);
        treeLayout->removeWidget(componentsTree);
    }

    // Добавляем виджеты в popup layout (если их еще нет)
    if (treePopupLayout->indexOf(filterLineEdit) == -1) {
        treePopupLayout->addWidget(filterLineEdit);
    }
    if (treePopupLayout->indexOf(componentsTree) == -1) {
        treePopupLayout->addWidget(componentsTree);
    }

    // Вычисляем размер и позицию popup
    QPoint globalPos = toggleModeButton->mapToGlobal(QPoint(0, toggleModeButton->height()));

    // Современный способ получения геометрии экрана без использования устаревшего QDesktopWidget
    QScreen *screen = nullptr;
    if (window()) {
        screen = window()->screen();
    }
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }

    QRect screenGeometry;
    if (screen) {
        // Используем перегрузку без аргументов (совместимо с Qt 5.15.2)
        screenGeometry = screen->availableGeometry();
    } else {
        // Резервный вариант на случай отсутствия данных об экране
        screenGeometry = QRect(globalPos.x(), globalPos.y(), 800, 600);
    }

    // Ширина popup = минимальная ширина
    int popupWidth = 300;
    // Высота = доступная высота экрана минус позиция минус отступ
    int popupHeight = qMin(screenGeometry.height() - globalPos.y() - 10, 600);
    popupHeight = qMax(popupHeight, 200); // Минимальная высота

    // Проверяем, не выходит ли popup за границы экрана
    if (globalPos.x() + popupWidth > screenGeometry.right()) {
        globalPos.setX(screenGeometry.right() - popupWidth);
    }
    if (globalPos.x() < screenGeometry.left()) {
        globalPos.setX(screenGeometry.left());
    }

    treePopupDialog->setGeometry(globalPos.x(), globalPos.y(), popupWidth, popupHeight);
    treePopupDialog->show();
    treePopupDialog->raise();
    treePopupDialog->activateWindow();

    // Разворачиваем все элементы дерева
    componentsTree->expandAll();

    // Устанавливаем фокус на поле фильтра
    filterLineEdit->setFocus();

    toggleModeButton->setChecked(true);
    toggleModeButton->setText(tr("☷"));
    toggleModeButton->setToolTip(tr("Скрыть дерево компонентов"));
}

void UComponentsListWidgetModern::hideTreePopup()
{
    if (!treePopupDialog->isVisible()) {
        return;
    }

    // Перемещаем виджеты обратно в основной контейнер
    treePopupLayout->removeWidget(filterLineEdit);
    treePopupLayout->removeWidget(componentsTree);

    QWidget *treeContainer = ui->treeContainer;
    QVBoxLayout *treeLayout = qobject_cast<QVBoxLayout*>(treeContainer->layout());
    if (treeLayout) {
        treeLayout->addWidget(filterLineEdit);
        treeLayout->addWidget(componentsTree);
    }

    treePopupDialog->hide();

    toggleModeButton->setChecked(false);
    toggleModeButton->setText(tr("☰"));
    toggleModeButton->setToolTip(tr("Показать дерево компонентов"));
}

bool UComponentsListWidgetModern::eventFilter(QObject *obj, QEvent *event)
{
    // Обработка Esc для закрытия popup
    if (obj == treePopupDialog && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            hideTreePopup();
            return true;
        }
    }

    return UVisualControllerWidget::eventFilter(obj, event);
}

void UComponentsListWidgetModern::updateFavoritesTabVisibility(bool hasFavorites, bool selectFavorites)
{
    QTabWidget* tabs = ui->tabWidgetComponentInfo;
    const int favIndex = tabs->indexOf(ui->tabFavorites);

    if(!hasFavorites)
    {
        if(favIndex >= 0)
            tabs->removeTab(favIndex);
        return;
    }

    {
        const QSignalBlocker blocker(tabs);
        if(favIndex < 0)
            tabs->insertTab(0, ui->tabFavorites, tr("Favorites"));
        else if(favIndex != 0)
            tabs->tabBar()->moveTab(favIndex, 0);

        if(selectFavorites)
            tabs->setCurrentWidget(ui->tabFavorites);
    }
}

int UComponentsListWidgetModern::logicalTabIndexFromWidget(QWidget* tab) const
{
    if(tab == ui->tabParameters)
        return 0;
    if(tab == ui->tabState)
        return 1;
    if(tab == ui->tabInputs)
        return 2;
    if(tab == ui->tabOutputs)
        return 3;
    if(tab == ui->tabFavorites)
        return 4;
    return -1;
}

QWidget* UComponentsListWidgetModern::widgetFromLogicalTabIndex(int logicalIndex) const
{
    switch(logicalIndex)
    {
    case 0: return ui->tabParameters;
    case 1: return ui->tabState;
    case 2: return ui->tabInputs;
    case 3: return ui->tabOutputs;
    case 4: return ui->tabFavorites;
    default: return nullptr;
    }
}

