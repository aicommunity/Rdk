 #include "UModernDiagramWidget.h"
#include "UModernDiagramScene.h"
#include "UModernDiagramView.h"
#include "UModernDiagramLinkItem.h"
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramExternalSourceItem.h"
#include "UModernDiagramViewportManager.h"
#include "UModernDiagramCoordinateManager.h"
#include "UModernDiagramCacheManager.h"
#include "UModernDiagramContextMenu.h"
#include "UEngineSelectionSync.h"
#include "UStyleManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPen>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QMainWindow>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QFileDialog>
#include <QDir>
#include <QDataStream>
#include <QMimeData>
#include <QGraphicsSceneHoverEvent>
#include <QShortcut>
#include <QKeySequence>
#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>
#include <QDateTime>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <cmath>
#include <ctime>
#include <sstream>
#include "UClassDescriptionDisplay.h"
#include "UQuickLinkDialog.h"
#include "UEngineSelectionSync.h"
#include "../Core/Engine/UStorage.h"
#include "../Core/Engine/UEngine.h"
#include "../Core/Engine/UNet.h"
#include "../../Deploy/Include/rdk_init.h"
#include "../Core/Application/UIVisualController.h"
#include "UGuiTelemetry.h"
#include <sstream>

// Классы UModernDiagramScene и UModernDiagramView находятся в отдельных файлах:
// - UModernDiagramScene.h/cpp
// - UModernDiagramView.h/cpp

// --------------------------- Helpers ---------------------------

// LinkItem теперь выделен в отдельный класс UModernDiagramLinkItem
// Реализации методов находятся в UModernDiagramLinkItem.cpp

// NodeItem теперь выделен в отдельный класс UModernDiagramNodeItem
// Реализации методов находятся в UModernDiagramNodeItem.cpp

// --------------------------- Widget ---------------------------

UModernDiagramWidget::UModernDiagramWidget(QWidget *parent)
    : QWidget(parent)
    , m_scene(new UModernDiagramScene(this))
    , m_mainView(new UModernDiagramView(this, m_scene))
    , m_miniMap(new QGraphicsView(m_scene, this))
    , m_tempLink(nullptr)
    , m_dragSourceNode(nullptr)
    , m_activeSourceNode(nullptr)
    , m_activeSourcePort(nullptr)
    , m_activeTempLink(nullptr)
    , m_isLineFrozen(false)
    , m_frozenTargetPortPos(0, 0)
    , m_isWaitingForPortSelection(false)
    , m_application(nullptr)
    , m_cacheManager(new UModernDiagramCacheManager(this))
    , m_coordinateManager(new UModernDiagramCoordinateManager(this))
    , m_viewportManager(new UModernDiagramViewportManager(this))
    , m_contextMenuManager(new UModernDiagramContextMenu(this))
    , m_selectComponentRetryCount(0)
{
    m_mainView->setRenderHint(QPainter::Antialiasing, true);
    m_mainView->setDragMode(QGraphicsView::RubberBandDrag);
    m_mainView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    m_mainView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mainView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_miniMap->setFixedHeight(UModernDiagramConstants::MINIMAP_HEIGHT);
    m_miniMap->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_miniMap->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_miniMap->setInteractive(false);
    m_miniMap->scale(UModernDiagramConstants::MINIMAP_SCALE, UModernDiagramConstants::MINIMAP_SCALE);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(m_mainView);
    // Миникарта скрыта
    m_miniMap->hide();

    // Кнопка «Описание проекта» слева от кнопки сброса масштаба
    m_viewportManager->createProjectDescriptionButton(this);
    m_viewportManager->createResetZoomButton(this);
    m_viewportManager->createClassesListButton(this);

    const QColor diagramBackground = UStyleManager::instance()->getBackgroundAltColor();
    m_scene->setBackgroundBrush(diagramBackground);
    m_mainView->setBackgroundBrush(diagramBackground);
}

UModernDiagramWidget::~UModernDiagramWidget()
{
    // Автоматическое сохранение кэша при закрытии
    if(m_application)
    {
        const QHash<QString, UModernDiagramComponentCacheEntry>& entries = m_cacheManager->getComponentCache().getAllEntries();
        if(!entries.isEmpty())
        {
            // Пробуем сохранить в бинарном формате (быстрее), если не получится - в JSON
            QString binPath = m_cacheManager->getCacheFilePath("bin");
            if(!m_cacheManager->saveComponentCacheToFile(binPath, true))
            {
                QString jsonPath = m_cacheManager->getCacheFilePath("json");
                m_cacheManager->saveComponentCacheToFile(jsonPath, false);
            }
        }
        else
        {
            QString logMsg = QString("[UModernDiagramWidget] Component cache is empty, skipping save on exit");
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        }
    }
}

void UModernDiagramWidget::SetApplication(RDK::UApplication* app)
{
    m_application = app;

    // Автоматическая загрузка кэша при установке приложения
    if(m_application)
    {
        // Логируем информацию о проекте для диагностики
        QString projectPath = QString::fromLocal8Bit(m_application->GetProjectPath().c_str());
        QString logMsg = QString("[UModernDiagramWidget] SetApplication called, projectPath: '%1'").arg(projectPath.isEmpty() ? "<empty>" : projectPath);
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

        // Сначала пробуем загрузить бинарный формат (быстрее)
        QString binPath = m_cacheManager->getCacheFilePath("bin");
        QString jsonPath = m_cacheManager->getCacheFilePath("json");

        logMsg = QString("[UModernDiagramWidget] Attempting to load cache from binary: %1").arg(binPath.isEmpty() ? "<empty>" : binPath);
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

        bool loaded = m_cacheManager->loadComponentCacheFromFile(binPath, true);
        if(loaded)
        {
            logMsg = QString("[UModernDiagramWidget] Component cache loaded from binary file: %1").arg(binPath);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        }
        else
        {
            // Если бинарный не найден, пробуем JSON
            logMsg = QString("[UModernDiagramWidget] Binary cache not found, attempting JSON: %1").arg(jsonPath.isEmpty() ? "<empty>" : jsonPath);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

            loaded = m_cacheManager->loadComponentCacheFromFile(jsonPath, false);
            if(loaded)
            {
                logMsg = QString("[UModernDiagramWidget] Component cache loaded from JSON file: %1").arg(jsonPath);
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            }
            else
            {
                logMsg = QString("[UModernDiagramWidget] Component cache not found, starting with empty cache (checked: bin=%1, json=%2)")
                    .arg(binPath.isEmpty() ? "<empty>" : binPath)
                    .arg(jsonPath.isEmpty() ? "<empty>" : jsonPath);
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            }
        }
    }
    else
    {
        QString logMsg2 = QString("[UModernDiagramWidget] SetApplication called with nullptr, cache loading skipped");
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg2.toStdString().c_str(), 0);
    }
}

void UModernDiagramWidget::SetComponentName(const QString& name)
{
    if(m_componentName == name)
        return;
    m_componentName = name;
    // Drill updates GUI diagram_scope only — do not Env_Select (TD-111 reverted).
    emit diagramScopeChanged(name);
}

void UModernDiagramWidget::requestOpenProjectDescription()
{
    emit openProjectDescriptionRequested();
}

void UModernDiagramWidget::requestToggleClassesList()
{
    emit classesListToggleRequested();
}

void UModernDiagramWidget::setClassesListButtonChecked(bool checked)
{
    if(m_viewportManager)
        m_viewportManager->setClassesListButtonChecked(checked);
}

void UModernDiagramWidget::Reload()
{
    RDK::UELockPtr<RDK::UContainer> modelLock =
        RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
    if(!modelLock)
    {
        clearDiagram();
        return;
    }

    // Профилирование: начало операции Reload
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // QString componentDisplayName = m_componentName.isEmpty() ? "root" : m_componentName;
    // NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UModernDiagramWidget.Reload"), componentDisplayName);

    // Сохраняем текущее состояние viewport перед перезагрузкой
    // Сохраняем только если сцена уже содержит элементы (компонент был загружен ранее)
    if(!m_componentName.isEmpty() && !m_scene->items().isEmpty())
    {
        m_viewportManager->saveCurrentViewState(m_componentName);
    }

    // Оптимизация: проверяем кэш ПЕРЕД clearScene()
    bool structureUnchanged = false;
    if(!m_componentName.isEmpty() && m_cacheManager && m_cacheManager->hasLevelCache(m_componentName))
    {
        const UModernDiagramSceneCache& cache = m_cacheManager->getLevelCache(m_componentName);

        // Проверяем, изменилась ли структура компонентов
        const QStringList currentComponents =
            childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), m_componentName);

        // Сравниваем списки компонентов
        bool structureChanged = (currentComponents.size() != cache.componentNames.size());
        if(!structureChanged)
        {
            // Сортируем для сравнения
            QStringList sortedCurrent = currentComponents;
            QStringList sortedCached = cache.componentNames;
            sortedCurrent.sort();
            sortedCached.sort();
            structureChanged = (sortedCurrent != sortedCached);
        }

        structureUnchanged = !structureChanged;

        // ВРЕМЕННО ОТКЛЮЧЕНО: кэш указателей вызывает падения при повторном входе
        // Проблема: после clearScene() указатели становятся невалидными
        // TODO: переделать кэш на сохранение данных вместо указателей
        /*
        // Если структура не изменилась, восстанавливаем из кэша
        if(!structureChanged && cache.isValid)
        {
            restoreSceneFromCache(m_componentName);
            // Восстанавливаем состояние viewport для текущего компонента
            m_viewportManager->restoreViewState(m_componentName);
            qint64 elapsed = telemetry.Elapsed();
            QString logMsg = QString("[UModernDiagramWidget] Component: %1, Reload: SKIPPED (structure unchanged), Duration: %2ms")
                .arg(componentDisplayName).arg(elapsed);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            return;
        }
        */
        if(structureChanged)
        {
            // Структура изменилась, инвалидируем кэш
            m_cacheManager->invalidateLevelCache(m_componentName);
        }
    }

    // Оптимизация: отключаем обновления во время перестройки для ускорения
    setUpdatesEnabled(false);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(false);

    // Удалено избыточное логирование - создавало спам в INFO логах
    clearScene();
    buildScene();

    // Включаем обновления обратно
    setUpdatesEnabled(true);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(true);

    // Восстанавливаем состояние viewport для текущего компонента
    m_viewportManager->restoreViewState(m_componentName);

    // Логируем результат профилирования
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // qint64 elapsed = telemetry.Elapsed();
    // QString details = QString("nodes: %1, links: %2").arg(m_nodes.size()).arg(m_links.size());
    // if(structureUnchanged)
    // {
    //     details += ", structure unchanged";
    // }
    // QString logMsg = QString("[UModernDiagramWidget] Component: %1, Operation: Reload, Duration: %2ms, Details: %3")
    //     .arg(componentDisplayName).arg(elapsed).arg(details);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
}

void UModernDiagramWidget::FitToView()
{
    if(m_scene->items().isEmpty())
        return;
    m_mainView->fitInView(m_scene->itemsBoundingRect().adjusted(-40,-40,40,40), Qt::KeepAspectRatio);
}

void UModernDiagramWidget::updateSceneRect()
{
    if(!m_scene || m_nodes.isEmpty())
        return;

    QRectF bounds = m_scene->itemsBoundingRect();
    if(!bounds.isNull())
    {
            QRectF padded = bounds.adjusted(-UModernDiagramConstants::SCENE_RECT_PADDING,
                                            -UModernDiagramConstants::SCENE_RECT_PADDING,
                                            UModernDiagramConstants::SCENE_RECT_PADDING,
                                            UModernDiagramConstants::SCENE_RECT_PADDING);
        m_scene->setSceneRect(padded);
    }
}

bool UModernDiagramWidget::isComponentOutsideVisibleArea(UModernDiagramNodeItem* node) const
{
    if(!node || !m_mainView || !m_scene)
        return false;
    if(node->scene() != m_scene)
        return false;

    // Получаем видимую область viewport в координатах сцены
    QRectF viewportRect = m_mainView->mapToScene(m_mainView->viewport()->rect()).boundingRect();

    // Получаем границы узла в координатах сцены
    QRectF nodeRect = node->sceneBoundingRect();

    // Проверяем, пересекается ли узел с видимой областью
    return !viewportRect.intersects(nodeRect);
}

QPointF UModernDiagramWidget::testGetNormalizationOffset() const
{
    if(!m_coordinateManager)
        return QPointF(0, 0);
    return m_coordinateManager->getNormalizationOffset();
}

const QSet<UModernDiagramNodeItem*>& UModernDiagramWidget::testGetComponentsWithNegativePos() const
{
    return m_componentsWithNegativePos;
}

void UModernDiagramWidget::clearDiagram()
{
    const bool scopeChanged = !m_componentName.isEmpty();
    clearScene();
    m_componentName.clear();
    if(m_cacheManager)
    {
        m_cacheManager->invalidateLevelCache();
        m_cacheManager->invalidateComponentCache();
    }
    if(scopeChanged)
        emit diagramScopeChanged(QString());
}

void UModernDiagramWidget::clearScene()
{
    // Профилирование: начало операции clearScene
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // QString componentDisplayName = m_componentName.isEmpty() ? "root" : m_componentName;
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UModernDiagramWidget.clearScene"),
    //     componentDisplayName + " (" + QString::number(nodesCount) + " nodes)");

    // ВРЕМЕННО ОТКЛЮЧЕНО: сохранение в кэш вызывает падения при повторном входе
    // Проблема: после clearScene() указатели становятся невалидными
    // TODO: переделать кэш на сохранение данных вместо указателей
    /*
    // Сохраняем текущую сцену в кэш перед очисткой (если есть компонент)
    if(!m_componentName.isEmpty() && !m_nodes.isEmpty())
    {
        m_cacheManager->saveSceneToCache(m_componentName);
    }
    */

    // Перед очисткой сцены обнуляем указатели на прокси-виджеты,
    // чтобы предотвратить двойное удаление в деструкторе NodeItem.
    // Сцена владеет прокси-виджетами и удалит их при clear().
    for(auto* node : m_nodes)
    {
        if(node)
        {
            node->m_portListWidgetProxy = nullptr;
            node->m_portListWidget = nullptr;  // Принадлежит прокси, будет удалён вместе с ним
            if(node->m_hideTimer)
            {
                node->m_hideTimer->stop();
            }
            // Очищаем кэш связей
            node->m_connectedLinks.clear();
        }
    }

    m_nodes.clear();
    m_nodeByName.clear();
    m_links.clear();
    m_externalSources.clear();
    m_externalSourceByKey.clear();
    m_lastNodePositions.clear();  // Очищаем сохраненные позиции
    m_componentsWithNegativePos.clear();
    m_originalAbsolutePositions.clear();
    m_scene->clear();  // Удаляет все элементы, включая NodeItem и прокси-виджеты
    m_tempLink = nullptr;
    m_dragSourceNode = nullptr;
    m_activeTempLink = nullptr;
    m_activeSourceNode = nullptr;
    m_activeSourcePort = nullptr;
    m_isLineFrozen = false;
    m_frozenTargetPortPos = QPointF(0, 0);
    m_coordinateManager->setNormalizationOffset(QPointF(0, 0));

    // Логируем результат профилирования
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // qint64 elapsed = telemetry.Elapsed();
    // QString details = QString("removed %1 nodes, %2 links").arg(nodesCount).arg(linksCount);
    // QString logMsg = QString("[UModernDiagramWidget] Component: %1, Operation: clearScene, Duration: %2ms, Details: %3")
    //     .arg(componentDisplayName).arg(elapsed).arg(details);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
}

QStringList UModernDiagramWidget::loadComponentList() const
{
    return childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), m_componentName);
}

QPointF UModernDiagramWidget::loadAndCacheCoordinates(const QStringList& components, bool& coordsLoaded,
                                                       QHash<QString, QPointF>& coordCache, QHash<QString, bool>& coordLoadedCache)
{
    coordsLoaded = false;
    QPointF minKernel(0, 0);
    bool minSet = false;

    // Загружаем все координаты и находим минимальную для визуальной нормализации
    for(const QString& comp : components)
    {
        QString fullName = m_componentName.isEmpty() ? comp : m_componentName + "." + comp;
        QPointF kernelPos;
        bool loaded = m_coordinateManager->loadCoord(fullName, kernelPos);

        // Обновляем кэш с загруженными координатами
        UModernDiagramComponentCacheEntry* cacheEntry = m_cacheManager->getComponentCache().getEntry(fullName);
        if(cacheEntry)
        {
            cacheEntry->kernelPos = kernelPos;
            cacheEntry->hasKernelPos = loaded;
            cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
        }
        else
        {
            UModernDiagramComponentCacheEntry newEntry;
            newEntry.kernelPos = kernelPos;
            newEntry.hasKernelPos = loaded;
            newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
            m_cacheManager->getComponentCache().setEntry(fullName, newEntry);
        }

        coordCache[fullName] = kernelPos;
        coordLoadedCache[fullName] = loaded;

        if(loaded)
        {
            coordsLoaded = true;
            if(!minSet)
            {
                minKernel = kernelPos;
                minSet = true;
            }
            else
            {
                if(kernelPos.x() < minKernel.x()) minKernel.setX(kernelPos.x());
                if(kernelPos.y() < minKernel.y()) minKernel.setY(kernelPos.y());
            }
        }
    }

    // Вычисляем смещение для визуальной нормализации (только для отображения)
    QPointF minScenePos = coordsLoaded ? m_coordinateManager->scenePosFromKernel(minKernel) : QPointF(0, 0);
    m_coordinateManager->setNormalizationOffset(minScenePos);

    // DEBUG: Commented out to reduce log flood - Логирование для отладки загрузки координат
    // QString logMsg = QString("[UModernDiagramWidget::buildScene] Loading components: coordsLoaded=%1, minKernel=(%2, %3), minScenePos=(%4, %5), m_normalizationOffset=(%6, %7), m_coordScale=%8")
    //     .arg(coordsLoaded ? "true" : "false")
    //     .arg(minKernel.x()).arg(minKernel.y())
    //     .arg(minScenePos.x()).arg(minScenePos.y())
    //     .arg(m_coordinateManager->getNormalizationOffset().x()).arg(m_coordinateManager->getNormalizationOffset().y())
    //     .arg(m_coordinateManager->getCoordScale());
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

    return minScenePos;
}

void UModernDiagramWidget::createNodes(const QStringList& components, const QHash<QString, QPointF>& coordCache,
                                       const QHash<QString, bool>& coordLoadedCache, const QPointF& minScenePos,
                                       QList<UModernDiagramNodeItem*>& nodesToAdd, QHash<QString, QPointF>& nodePositions)
{
    QHash<QString, QString> classNameCache;
    int idx = 0;

    for(const QString& comp : components)
    {
        QString fullName = m_componentName.isEmpty() ? comp : m_componentName + "." + comp;

        // Кэшируем имя класса для минимизации вызовов API
        QString cls;
        UModernDiagramComponentCacheEntry* cacheEntry = m_cacheManager->getComponentCache().getEntry(fullName);
        if(cacheEntry && !cacheEntry->className.isEmpty())
        {
            cls = cacheEntry->className;
        }
        else if(classNameCache.contains(fullName))
        {
            cls = classNameCache[fullName];
        }
        else
        {
            cls = componentClassNameFromModelScope(Core_GetSelectedChannelIndex(), fullName);
            classNameCache[fullName] = cls;

            // Сохраняем имя класса в сессионный кэш
            if(cacheEntry)
            {
                cacheEntry->className = cls;
                cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
            }
            else
            {
                UModernDiagramComponentCacheEntry newEntry;
                newEntry.className = cls;
                newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
                m_cacheManager->getComponentCache().setEntry(fullName, newEntry);
            }
        }

        auto* node = new UModernDiagramNodeItem(this, comp, cls);
        QPointF loaded;
        QPointF kernelPos = coordCache.value(fullName);
        if(coordLoadedCache.value(fullName, false))
        {
            QPointF absoluteScenePos = m_coordinateManager->scenePosFromKernel(kernelPos);
            loaded = absoluteScenePos - minScenePos;

            // DEBUG: Commented out to reduce log flood
            // QString logMsg2 = QString("[UModernDiagramWidget::buildScene] Component '%1': kernelPos=(%2, %3), absoluteScenePos=(%4, %5), minScenePos=(%6, %7), loaded=(%8, %9)")
            //     .arg(fullName)
            //     .arg(kernelPos.x()).arg(kernelPos.y())
            //     .arg(absoluteScenePos.x()).arg(absoluteScenePos.y())
            //     .arg(minScenePos.x()).arg(minScenePos.y())
            //     .arg(loaded.x()).arg(loaded.y());
            // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg2.toStdString().c_str(), 0);
        }
        else
        {
            loaded = QPointF((idx % UModernDiagramConstants::GRID_COLUMNS) * UModernDiagramConstants::GRID_CELL_WIDTH,
                            (idx / UModernDiagramConstants::GRID_COLUMNS) * UModernDiagramConstants::GRID_CELL_HEIGHT);
            QString logMsg3 = QString("[UModernDiagramWidget::buildScene] Component '%1': coordinates not loaded, using grid: loaded=(%2, %3)")
                .arg(fullName)
                .arg(loaded.x()).arg(loaded.y());
            const QByteArray logMsg3Utf8 = logMsg3.toUtf8();
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg3Utf8.constData(), 0);
        }

        nodesToAdd.append(node);
        nodePositions[comp] = loaded;
        m_nodeByName.insert(comp, node);
        idx++;
    }
}

void UModernDiagramWidget::addNodesToScene(const QList<UModernDiagramNodeItem*>& nodesToAdd, const QHash<QString, QPointF>& nodePositions)
{
    m_isBuildingScene = true;

    for(auto* node : nodesToAdd)
    {
        m_scene->addItem(node);
        m_nodes.append(node);
        QString comp = node->nodeName;
        QPointF loaded = nodePositions[comp];
        node->setPos(loaded);
        m_lastNodePositions[node] = loaded;
    }

    m_isBuildingScene = false;
}

void UModernDiagramWidget::buildScene()
{
    if(!m_application)
        return;

    // Отключаем обновления во время массового создания узлов
    setUpdatesEnabled(false);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(false);

    // Загружаем список компонентов
    QStringList components = loadComponentList();
    if(components.isEmpty())
    {
        setUpdatesEnabled(true);
        if(m_mainView)
            m_mainView->setUpdatesEnabled(true);
        return;
    }

    // Загружаем и кэшируем координаты
    bool coordsLoaded = false;
    QHash<QString, QPointF> coordCache;
    QHash<QString, bool> coordLoadedCache;
    QPointF minScenePos = loadAndCacheCoordinates(components, coordsLoaded, coordCache, coordLoadedCache);

    // Создаем узлы
    QList<UModernDiagramNodeItem*> nodesToAdd;
    QHash<QString, QPointF> nodePositions;
    createNodes(components, coordCache, coordLoadedCache, minScenePos, nodesToAdd, nodePositions);

    // Добавляем узлы в сцену
    addNodesToScene(nodesToAdd, nodePositions);

    // Если координаты не загружены, используем сетку
    if(!coordsLoaded)
    {
        layoutGrid();
    }

    // Обновляем sceneRect
    updateSceneRect();

    // Включаем обновления перед созданием связей
    setUpdatesEnabled(true);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(true);

    // Создаем связи
    buildLinks();

    // Планируем отложенное сохранение кэша
    if(m_cacheManager) m_cacheManager->scheduleCacheSave();
}

UModernDiagramNodeItem* UModernDiagramWidget::addSingleComponent(const QString& fullName)
{
    if(!m_application)
        return nullptr;

    // Извлекаем короткое имя компонента
    QString shortName = fullName;
    if(!m_componentName.isEmpty())
    {
        QString prefix = m_componentName + ".";
        if(shortName.startsWith(prefix))
        {
            shortName = shortName.mid(prefix.length());
        }
    }

    // Проверяем, что компонент еще не добавлен
    if(m_nodeByName.contains(shortName))
    {
        return m_nodeByName.value(shortName);
    }

    const QString cls =
        componentClassNameFromModelScope(Core_GetSelectedChannelIndex(), fullName);

    // Загружаем координаты из ядра
    QPointF kernelPos;
    bool coordLoaded = m_coordinateManager->loadCoord(fullName, kernelPos);

    // Вычисляем нормализованную позицию используя текущий m_normalizationOffset
    // Это важно - мы НЕ пересчитываем m_normalizationOffset, чтобы не сдвигать существующие компоненты
    QPointF loaded;
    if(coordLoaded)
    {
        QPointF absoluteScenePos = m_coordinateManager->scenePosFromKernel(kernelPos);
        loaded = absoluteScenePos - m_coordinateManager->getNormalizationOffset();
    }
    else
    {
        // Если координат нет, размещаем в сетке
        int idx = m_nodes.size();
        loaded = QPointF((idx % UModernDiagramConstants::GRID_COLUMNS) * UModernDiagramConstants::GRID_CELL_WIDTH,
                        (idx / UModernDiagramConstants::GRID_COLUMNS) * UModernDiagramConstants::GRID_CELL_HEIGHT);
    }

    // Создаем узел
    auto* node = new UModernDiagramNodeItem(this, shortName, cls);

    // Устанавливаем флаг, чтобы предотвратить сохранение координат во время инициализации
    m_isBuildingScene = true;

    // Добавляем узел в сцену
    m_scene->addItem(node);
    m_nodes.append(node);
    m_nodeByName.insert(shortName, node);
    node->setPos(loaded);
    m_lastNodePositions[node] = loaded;

    // Сбрасываем флаг
    m_isBuildingScene = false;

    // Обновляем кэш компонента
    UModernDiagramComponentCacheEntry* cacheEntry = m_cacheManager->getComponentCache().getEntry(fullName);
    if(cacheEntry)
    {
        cacheEntry->className = cls;
        cacheEntry->kernelPos = kernelPos;
        cacheEntry->hasKernelPos = coordLoaded;
        cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
    }
    else
    {
        UModernDiagramComponentCacheEntry newEntry;
        newEntry.className = cls;
        newEntry.kernelPos = kernelPos;
        newEntry.hasKernelPos = coordLoaded;
        newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
        m_cacheManager->getComponentCache().setEntry(fullName, newEntry);
    }

    // Обновляем связи (это добавит связи для нового компонента)
    rebuildLinks();

    return node;
}

void UModernDiagramWidget::layoutGrid()
{
    if(m_nodes.isEmpty())
        return;

    QRectF bounds;
    for(auto* n : m_nodes) bounds |= n->sceneBoundingRect().translated(n->pos());
    QPointF center = bounds.center();
    for(auto* n : m_nodes)
        n->setPos(n->pos() - center);
}

UModernDiagramNodeItem* UModernDiagramWidget::pickPort(const QPointF& scenePos, bool requireInput, QPointF& portPos)
{
    UModernDiagramNodeItem* node = nullptr;
    // Нам нужен только найденный узел и позиция порта; возвращаемое значение не используем
    pickPortDetailed(scenePos, requireInput, node, portPos);
    return node;
}

const Port* UModernDiagramWidget::pickPortDetailed(
    const QPointF& scenePos, bool requireInput, UModernDiagramNodeItem*& node, QPointF& portPos)
{
    for(auto* n : m_nodes)
    {
        QPointF localPos = n->mapFromScene(scenePos);
        const Port* port = n->getPortAtPosition(localPos);
        if(port && port->isInput == requireInput)
        {
            node = n;
            portPos = n->mapToScene(port->pos);
            return port;
        }
    }
    node = nullptr;
    return nullptr;
}

UModernDiagramNodeItem* UModernDiagramWidget::pickNode(const QPointF& scenePos) const
{
    for(auto* node : m_nodes)
    {
        if(node->contains(node->mapFromScene(scenePos)))
            return node;
    }
    return nullptr;
}

void UModernDiagramWidget::buildLinks()
{
    // Профилирование: начало операции buildLinks
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // QString componentDisplayName = m_componentName.isEmpty() ? "root" : m_componentName;
    // NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UModernDiagramWidget.buildLinks"), componentDisplayName);

    // Оптимизация: отключаем обновления во время массового создания связей
    bool updatesWereEnabled = updatesEnabled();
    setUpdatesEnabled(false);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(false);

    const std::string linksXml =
        internalLinksXmlFromModelScope(Core_GetSelectedChannelIndex(), m_componentName);
    if(linksXml.empty())
    {
        buildExternalIncomingLinks();
        setUpdatesEnabled(updatesWereEnabled);
        if(m_mainView)
            m_mainView->setUpdatesEnabled(updatesWereEnabled);
        return;
    }
    RDK::USerStorageXML xml;
    if(!xml.Load(linksXml, "Links"))
    {
        buildExternalIncomingLinks();
        setUpdatesEnabled(updatesWereEnabled);
        if(m_mainView)
            m_mainView->setUpdatesEnabled(updatesWereEnabled);
        return;
    }

    RDK::UStringLinksList linkslist;
    xml >> linkslist;
    // Удалено избыточное логирование - создавало спам в INFO логах

    // Обновляем имя телеметрии с количеством связей
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // telemetry.Stop();
    // NMSDK::UGuiTelemetryScope telemetry2(QStringLiteral("UModernDiagramWidget.buildLinks"),
    //     componentDisplayName + " (" + QString::number(linkslist.GetSize()) + " links)");

    QPointF minPos = m_coordinateManager->currentMinScenePos();

    // Оптимизация: создаем все связи сначала, затем добавляем в сцену пакетами
    QList<UModernDiagramLinkItem*> linksToAdd;
    QHash<QPair<UModernDiagramNodeItem*, QString>, PortCategory> portCategoryCache;  // Кэш для determinePortCategory
    QHash<QString, UModernDiagramLinkItem*> aggregatedLinks;  // ключ: src|dst|srcCat|dstCat

    int added = 0;
    int skipped = 0;
    for(int i=0;i<linkslist.GetSize();++i)
    {
        const auto& link = linkslist[i]; // RDK::ULinkT<std::string>
        QString itemName = QString::fromStdString(link.Item.Name);
        std::string itemId = link.Item.Id;

        for(size_t c=0;c<link.Connector.size();++c)
        {
            const auto& connSide = link.Connector[c];
            QString connName = QString::fromStdString(connSide.Name);
            std::string connId = connSide.Id;
            // Удалено избыточное логирование - создавало спам в INFO логах
            UModernDiagramNodeItem* srcNode = resolveNodeOnDiagram(itemName);
            UModernDiagramNodeItem* dstNode = resolveNodeOnDiagram(connName);
            if(!srcNode)
                srcNode = resolveNodeByIdOnDiagram(QString::fromStdString(itemId));
            if(!dstNode)
                dstNode = resolveNodeByIdOnDiagram(QString::fromStdString(connId));

            // Если srcNode не найден, пропускаем связь (не можем определить категорию источника)
            if(!srcNode)
            {
                skipped++;
                continue;
            }

            // Оптимизация: кэшируем результаты determinePortCategory для источника
            QPair<UModernDiagramNodeItem*, QString> srcKey(srcNode, itemName);
            PortCategory srcCategory;
            if(portCategoryCache.contains(srcKey))
            {
                srcCategory = portCategoryCache[srcKey];
            }
            else
            {
                srcCategory = srcNode->determinePortCategory(itemName, false);
                portCategoryCache[srcKey] = srcCategory;
            }

            QString connIdStr = QString::fromStdString(connId);

            // Определяем категорию целевого порта
            PortCategory dstCategory;
            QString normalizedConnName;
            if(dstNode)
            {
                normalizedConnName = normalizeConnectorNameForDst(dstNode, connName, connIdStr);
                dstCategory = resolveInputPortCategory(dstNode, normalizedConnName, portCategoryCache);
            }
            else
            {
                normalizedConnName = connName;
                QString dstNodeName;
                if(!connIdStr.isEmpty())
                {
                    int dot = connIdStr.indexOf('.');
                    dstNodeName = dot >= 0 ? connIdStr.left(dot) : connIdStr;
                    if(!m_componentName.isEmpty() && dstNodeName.startsWith(m_componentName + "."))
                    {
                        dstNodeName = dstNodeName.mid(m_componentName.length() + 1);
                        int nextDot = dstNodeName.indexOf('.');
                        if(nextDot >= 0)
                            dstNodeName = dstNodeName.left(nextDot);
                    }
                }
                else if(!connName.isEmpty())
                {
                    int dot = connName.indexOf('.');
                    dstNodeName = dot >= 0 ? connName.left(dot) : connName;
                }

                if(m_componentName.isEmpty())
                {
                    if(connIdStr == dstNodeName)
                        normalizedConnName = connName;
                    else if(connIdStr.startsWith(dstNodeName + "."))
                    {
                        QString pathFromConnId = connIdStr.mid(dstNodeName.length() + 1);
                        if(!connName.contains('.'))
                            normalizedConnName = pathFromConnId + "." + connName;
                        else
                            normalizedConnName = connName;
                    }
                    else if(connName.startsWith(dstNodeName + "."))
                        normalizedConnName = connName.mid(dstNodeName.length() + 1);
                }
                else
                {
                    if(connName.startsWith(dstNodeName + "."))
                        normalizedConnName = connName.mid(dstNodeName.length() + 1);
                    else
                    {
                        QString fullPath = m_componentName + "." + dstNodeName;
                        if(connName.startsWith(fullPath + "."))
                            normalizedConnName = connName.mid(fullPath.length() + 1);
                        else if(connIdStr == dstNodeName ||
                                connIdStr.endsWith("." + dstNodeName) ||
                                connIdStr == fullPath ||
                                connIdStr.endsWith("." + fullPath))
                            normalizedConnName = connName;
                    }
                }

                if(!normalizedConnName.isEmpty() && normalizedConnName.contains('.'))
                {
                    QString firstPart = normalizedConnName.split('.').first();
                    QString fullName = m_componentName.isEmpty() ? srcNode->nodeName : m_componentName + "." + srcNode->nodeName;
                    const QStringList components =
                        childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), fullName);
                    dstCategory = components.contains(firstPart) ? PortCategory::Child : PortCategory::Own;
                }
                else
                    dstCategory = PortCategory::Own;
            }

            // Создаем / агрегируем LinkItem по (src, dst, srcCategory, dstCategory)
            const QString aggKey = QString::number(quintptr(srcNode)) + QLatin1Char('|')
                + QString::number(quintptr(dstNode)) + QLatin1Char('|')
                + QString::number(static_cast<int>(srcCategory)) + QLatin1Char('|')
                + QString::number(static_cast<int>(dstCategory));

            if(UModernDiagramLinkItem* existing = aggregatedLinks.value(aggKey, nullptr))
            {
                existing->incrementParallelCount();
                added++;
                continue;
            }

            auto* l = new UModernDiagramLinkItem(srcNode, dstNode, srcCategory, dstCategory);
            aggregatedLinks.insert(aggKey, l);

            if(dstNode)
            {
                // Обычная связь между узлами в сцене - добавляем в сцену
                linksToAdd.append(l);
                m_links.append(l);
                // Обновляем кэш связей для обоих узлов
                srcNode->m_connectedLinks.append(l);
                dstNode->m_connectedLinks.append(l);
                added++;
            }
            else
            {
                // Внешняя связь (dstNode не найден) - не добавляем в сцену, но учитываем в кэше
                // Такие связи важны для быстрой проверки соединений (например, для PGenerator)
                m_links.append(l);
                srcNode->m_connectedLinks.append(l);
                added++;
                // Не добавляем в linksToAdd, чтобы не добавлять в сцену

                // ДЕТАЛЬНОЕ ЛОГИРОВАНИЕ для диагностики внешних связей
                // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
                // if(srcNode->nodeName == "PGenerator")
                // {
                //     QString categoryStr = srcCategory == PortCategory::Own ? "Own" : (srcCategory == PortCategory::Child ? "Child" : "Alias");
                //     QString dstCategoryStr = dstCategory == PortCategory::Own ? "Own" : (dstCategory == PortCategory::Child ? "Child" : "Alias");
                //     QString logMsg = QString("[UModernDiagramWidget] Component: %1, buildLinks: Created EXTERNAL link from PGenerator (srcCategory=%2, dstCategory=%3, connName=%4)")
                //         .arg(componentDisplayName).arg(categoryStr).arg(dstCategoryStr).arg(connName);
                //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
                // }
            }
            // Удалено избыточное логирование - создавало спам в INFO логах
        }
    }

    // Добавляем все связи в сцену пакетами
    for(auto* l : linksToAdd)
    {
        m_scene->addItem(l);
    }

    // Вызываем updateGeometry() для всех связей после добавления в сцену
    // Профилирование: измеряем время обновления геометрии всех связей
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // NMSDK::UGuiTelemetryScope telemetry3(QStringLiteral("UModernDiagramWidget.LinkItem.updateGeometry"),
    //     componentDisplayName + " (" + QString::number(linksToAdd.size()) + " links)");
    for(auto* l : linksToAdd)
    {
        l->updateGeometry();
    }
    // telemetry3.Stop();
    // qint64 updateGeometryElapsed = telemetry3.Elapsed();

    // Инвалидируем кэш paint() для всех узлов после создания связей
    for(auto* node : m_nodes)
    {
        node->m_cacheValid = false;
    }

    buildExternalIncomingLinks();

    // Включаем обновления обратно
    setUpdatesEnabled(updatesWereEnabled);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(updatesWereEnabled);

    // Логируем результат профилирования
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // qint64 elapsed = telemetry2.Elapsed();
    // QString details = QString("created %1 links, skipped %2, updateGeometry: %3ms").arg(added).arg(skipped).arg(updateGeometryElapsed);
    // QString logMsg = QString("[UModernDiagramWidget] Component: %1, Operation: buildLinks, Duration: %2ms, Details: %3")
    //     .arg(componentDisplayName).arg(elapsed).arg(details);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
}

void UModernDiagramWidget::rebuildLinks()
{
    // Очищаем кэш связей для всех узлов перед удалением связей
    for(auto* node : m_nodes)
    {
        node->m_connectedLinks.clear();
        // Инвалидируем кэш paint() при изменении связей
        node->m_cacheValid = false;
    }

    // Удаляем все существующие связи
    for(auto* link : m_links)
    {
        m_scene->removeItem(link);
        delete link;
    }
    m_links.clear();
    clearExternalSources();

    // Перестраиваем связи
    buildLinks();

    // Инвалидируем кэш paint() для всех узлов после перестройки связей
    for(auto* node : m_nodes)
    {
        node->m_cacheValid = false;
    }

    // Инвалидируем кэш уровня при изменении связей
    if(!m_componentName.isEmpty())
    {
        if(m_cacheManager) m_cacheManager->invalidateLevelCache(m_componentName);
    }
}

// saveSceneToCache теперь в UModernDiagramCacheManager

// restoreSceneFromCache теперь в UModernDiagramCacheManager

// invalidateLevelCache теперь в UModernDiagramCacheManager

void UModernDiagramWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // Позиционируем кнопку сброса масштаба в правом верхнем углу
    if(m_viewportManager)
    {
        m_viewportManager->updateResetZoomButtonPosition(width(), height());
    }
}

void UModernDiagramWidget::keyPressEvent(QKeyEvent *event)
{
    // Обработка Enter для завершения соединения через окно выбора портов
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        // Проверяем, есть ли открытое окно выбора портов
        for(UModernDiagramNodeItem* node : m_nodes)
        {
            if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
            {
                if(node->m_portListWidget)
                {
                    QTreeWidgetItem* currentItem = node->m_portListWidget->currentItem();
                    if(currentItem && currentItem->parent() != nullptr) // Проверяем, что это элемент порта (не категория)
                    {
                        // Логируем через qDebug для гарантированного вывода
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO,
                            "keyPressEvent: Enter pressed, processing current item from port list", 0);
                        node->onPortItemActivated(currentItem, 0);
                        event->accept();
                        return;
                    }
                    else
                    {
                    }
                }
            }
        }
        // Если окно выбора портов не открыто, передаем событие дальше (не отменяем соединение!)
    }

    // Обработка Esc для отмены активной связи и закрытия окон со списком портов
    if(event->key() == Qt::Key_Escape)
    {
        // Сбрасываем выделение группы при нажатии Esc
        if(m_scene)
        {
            QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
            int selectedNodeCount = 0;
            for(QGraphicsItem* item : selectedItems)
            {
                auto* node = dynamic_cast<UModernDiagramNodeItem*>(item);
                if(node && node->isSelected())
                {
                    selectedNodeCount++;
                }
            }
            if(selectedNodeCount > 1)
            {
                // Сбрасываем выделение группы
                m_scene->clearSelection();
                event->accept();
                return;
            }
        }

        // Если ожидается выбор порта из окна выбора, отменяем это состояние
        if(m_isWaitingForPortSelection)
        {
            m_isWaitingForPortSelection = false;
            m_isLineFrozen = false;

            // Закрываем все открытые деревья портов
            for(UModernDiagramNodeItem* node : m_nodes)
            {
                node->hidePortListWidget();
            }

            // Возвращаемся в состояние создания соединения (временная линия продолжает следовать за мышью)
            event->accept();
            return;
        }

        if(m_activeTempLink)
        {
            m_scene->removeItem(m_activeTempLink);
            delete m_activeTempLink;
            m_activeTempLink = nullptr;
            m_activeSourceNode = nullptr;
            m_activeSourcePort = nullptr;
            m_isLineFrozen = false;
            m_isWaitingForPortSelection = false;

            // Сбрасываем курсор
            if(m_mainView)
            {
                m_mainView->unsetCursor();
            }
        }

        // Закрываем все открытые деревья портов (даже если нет активной связи)
        for(UModernDiagramNodeItem* node : m_nodes)
        {
            node->hidePortListWidget();
        }

        event->accept();
        return;
    }

    if(event->key() == Qt::Key_Delete)
    {
        QList<QGraphicsItem*> selected = m_scene->selectedItems();

        // Собираем все выделенные NodeItem
        QList<UModernDiagramNodeItem*> nodesToDelete;
        for(QGraphicsItem* it : selected)
        {
            auto* node = dynamic_cast<UModernDiagramNodeItem*>(it);
            if(node)
            {
                nodesToDelete.append(node);
            }
        }

        // Удаляем все выделенные компоненты через универсальный метод
        if(!nodesToDelete.isEmpty())
        {
            deleteComponents(nodesToDelete);
        }
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}


// recalculateNormalizationOffset теперь в UModernDiagramCoordinateManager
void UModernDiagramWidget::recalculateNormalizationOffset(const QPointF& pendingComponentPos, const QString& pendingComponentName)
{
    m_coordinateManager->recalculateNormalizationOffset(pendingComponentPos, pendingComponentName);
}

// updateNormalizationOffsetForMovement теперь в UModernDiagramCoordinateManager
void UModernDiagramWidget::updateNormalizationOffsetForMovement(const QPointF& newMinNormalizedPos, const QSet<UModernDiagramNodeItem*>& componentsToAdjust)
{
    m_coordinateManager->updateNormalizationOffsetForMovement(newMinNormalizedPos, componentsToAdjust);
}

// --------------------------- Slots ---------------------------

void UModernDiagramWidget::componentDoubleClick(QString name)
{
    // Сохраняем состояние текущего компонента перед переходом
    m_viewportManager->saveCurrentViewState(m_componentName);

    SetComponentName(name);
    Reload();
}

void UModernDiagramWidget::componentSingleClick(QString name)
{
    // DEBUG: Commented out to reduce log flood
    // QString logMsg = QString("[SELECTION_DEBUG] UModernDiagramWidget::componentSingleClick: called with name='%1'")
    //     .arg(name);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    selectComponent(name);
}

void UModernDiagramWidget::updateScheme(bool reloadXml)
{
    Q_UNUSED(reloadXml);
    Reload();
}

void UModernDiagramWidget::updateTheme()
{
    // Инвалидируем кэш отрисовки для всех узлов
    for(auto* node : m_nodes)
    {
        if(node)
        {
            node->m_cacheValid = false;
            node->update();
        }
    }

    // Обновляем все связи
    for(auto* link : m_links)
    {
        if(link)
        {
            link->update();
        }
    }

    // Обновляем всю сцену
    if(m_scene)
    {
        const QColor diagramBackground = UStyleManager::instance()->getBackgroundAltColor();
        m_scene->setBackgroundBrush(diagramBackground);
        if(m_mainView)
            m_mainView->setBackgroundBrush(diagramBackground);
        m_scene->update();
    }

    // Обновляем стили кнопки сброса масштаба
    m_viewportManager->updateResetZoomButtonStyle();
}

namespace {
void clearNodePortsCache(UModernDiagramNodeItem* node)
{
    if(!node)
        return;
    node->m_portsCacheValid = false;
    node->m_cachedOwnInputPorts.clear();
    node->m_cachedChildInputPorts.clear();
    node->m_cachedAliasInputPorts.clear();
    node->m_cachedOwnOutputPorts.clear();
    node->m_cachedChildOutputPorts.clear();
    node->m_cachedAliasOutputPorts.clear();
    node->m_hasConnectionsToInputCache.clear();
    node->m_hasConnectionsToOutputCache.clear();
    node->m_hasInputPortsCache.clear();
    node->m_hasOutputPortsCache.clear();
    node->m_cacheValid = false;
}
} // namespace

void UModernDiagramWidget::invalidatePortsCache()
{
    if(m_cacheManager)
        m_cacheManager->invalidateComponentCache();
    for(auto* node : m_nodes)
        clearNodePortsCache(node);
}

void UModernDiagramWidget::invalidatePortsCache(const QString& componentFullName)
{
    if(componentFullName.isEmpty())
    {
        invalidatePortsCache();
        return;
    }

    if(m_cacheManager)
        m_cacheManager->invalidateComponentCache(componentFullName);

    auto findAndClear = [this](const QString& name) -> bool {
        if(auto it = m_nodeByName.find(name); it != m_nodeByName.end())
        {
            clearNodePortsCache(it.value());
            return true;
        }
        return false;
    };

    if(findAndClear(componentFullName))
        return;

    if(!m_componentName.isEmpty() && componentFullName.startsWith(m_componentName + "."))
    {
        const QString relative = componentFullName.mid(m_componentName.size() + 1);
        const int dot = relative.indexOf('.');
        const QString top = dot >= 0 ? relative.left(dot) : relative;
        findAndClear(top);
        if(m_cacheManager && top != relative)
            m_cacheManager->invalidateComponentCache(m_componentName + "." + top);
        return;
    }

    const int dot = componentFullName.indexOf('.');
    const QString top = dot >= 0 ? componentFullName.left(dot) : componentFullName;
    findAndClear(top);
}

void UModernDiagramWidget::invalidatePortsCacheEverywhere(const QString& componentFullName)
{
    const QWidgetList widgets = QApplication::allWidgets();
    for(QWidget* w : widgets)
    {
        auto* diagram = qobject_cast<UModernDiagramWidget*>(w);
        if(!diagram)
            continue;
        if(componentFullName.isEmpty())
            diagram->invalidatePortsCache();
        else
            diagram->invalidatePortsCache(componentFullName);
    }
}


void UModernDiagramWidget::selectComponent(QString name)
{
    // DEBUG: Commented out to reduce log flood
    // QString logMsg = QString("[SELECTION_DEBUG] UModernDiagramWidget::selectComponent: called with name='%1', retryCount=%2")
    //     .arg(name)
    //     .arg(m_selectComponentRetryCount);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

    // Если имя пустое, ничего не делаем
    if(name.isEmpty())
    {
        m_selectComponentRetryCount = 0;
        return;
    }

    // Защита от бесконечной рекурсии
    if(m_selectComponentRetryCount >= 3)
    {
        m_selectComponentRetryCount = 0;
        return;
    }

    // Если имя совпадает с текущим путём, значит мы на верхнем уровне
    if(!m_componentName.isEmpty() && name == m_componentName)
    {
        m_selectComponentRetryCount = 0;
        return;
    }

    // Определяем относительное имя компонента в текущем контексте
    QString componentName = name;
    bool needToNavigate = false;
    QString targetPath;

    if(m_componentName.isEmpty())
    {
        // Мы на корневом уровне, компонент должен быть прямым потомком
        // Берем только первую часть пути
        componentName = name.split(".").first();

        // Если компонент имеет вложенность, нужно перейти внутрь
        if(name.contains("."))
        {
            needToNavigate = true;
            targetPath = componentName;
        }
    }
    else if(name.startsWith(m_componentName + "."))
    {
        // Компонент находится внутри текущего контекста
        componentName = name.mid(m_componentName.size() + 1);

        // Проверяем, есть ли вложенность (например, Component1.Component2.Component3)
        QStringList relativePathParts = componentName.split(".");
        if(relativePathParts.size() > 1)
        {
            // Компонент находится не на прямом уровне вложенности
            // Нужно перейти внутрь родительского компонента
            needToNavigate = true;
            QStringList fullPathParts = name.split(".");
            fullPathParts.removeLast(); // Убираем последний элемент (сам компонент)
            targetPath = fullPathParts.join(".");
        }
    }
    else
    {
        // Компонент находится вне текущего контекста
        // Нужно перейти на нужный уровень
        QStringList pathParts = name.split(".");

        if(pathParts.size() > 1)
        {
            // Компонент находится внутри другого компонента
            pathParts.removeLast();
            targetPath = pathParts.join(".");
        }
        else
        {
            // Компонент на корневом уровне
            targetPath = "";
        }
        needToNavigate = true;
    }

    // Если нужно перейти на другой уровень, используем componentDoubleClick
    if(needToNavigate)
    {
        m_selectComponentRetryCount++;
        componentDoubleClick(targetPath);

        // После перезагрузки выбираем компонент
        QTimer::singleShot(300, [this, name]() {
            selectComponent(name);
        });
        return;
    }

    // Компонент должен быть на текущем уровне - пытаемся найти и выделить его
    if(auto it = m_nodeByName.find(componentName); it != m_nodeByName.end())
    {
        // DEBUG: Commented out to reduce log flood
        // QString logMsg2 = QString("[SELECTION_DEBUG] UModernDiagramWidget::selectComponent: found node '%1', setting m_isProgrammaticSelection=true, calling clearSelection() and setSelected(true)")
        //     .arg(componentName);
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg2.toStdString().c_str(), 0);
        m_isProgrammaticSelection = true;
        m_scene->clearSelection();
        it.value()->setSelected(true);
        // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Откладываем сброс флага m_isProgrammaticSelection,
        // чтобы он оставался установленным во время обработки всех событий ItemSelectedHasChanged,
        // которые могут быть вызваны setSelected(true). Это предотвращает эмиссию componentSelected и рекурсию.
        QTimer::singleShot(0, [this]() {
            m_isProgrammaticSelection = false;
        });
        // Прокручиваем к выбранному узлу только если компонент не двигается
        // Это предотвращает движение скроллбара при клике на компонент во время перетаскивания
        if(m_mainView && !m_isComponentMoving)
            m_mainView->centerOn(it.value());

        // Сбрасываем счетчик после успешного выбора
        m_selectComponentRetryCount = 0;

        // Сигнал componentSelected будет эмитирован автоматически через механизм выделения узлов в схеме
        // (через itemChange в NodeItem)
    }
    else
    {
        // Компонент не найден на текущем уровне
        // Возможно, нужно подождать еще немного после перезагрузки
        if(m_selectComponentRetryCount < 3)
        {
            m_selectComponentRetryCount++;
            QTimer::singleShot(300, [this, name]() {
                selectComponent(name);
            });
        }
        else
        {
            m_selectComponentRetryCount = 0;
        }
    }
}

// --------------------------- Context Menu ---------------------------
// Context menu теперь в UModernDiagramContextMenu

void UModernDiagramWidget::deleteComponents(const QList<UModernDiagramNodeItem*>& nodesToDelete)
{
    if(nodesToDelete.isEmpty())
        return;

    // Запрос подтверждения пользователю (если не нажат Shift)
    if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
    {
        QString message;
        if(nodesToDelete.size() == 1)
        {
            QString fullName = m_componentName.isEmpty() ? nodesToDelete[0]->nodeName
                                                         : m_componentName + "." + nodesToDelete[0]->nodeName;
            message = "Are you sure you want to delete component " + fullName + "?";
        }
        else
        {
            message = "Are you sure you want to delete " + QString::number(nodesToDelete.size()) + " components?";
        }

        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", message,
            QMessageBox::Yes|QMessageBox::Cancel);
        if(reply != QMessageBox::Yes)
            return;
    }

    // Удаляем все компоненты
    for(UModernDiagramNodeItem* node : nodesToDelete)
    {
        QString fullName = m_componentName.isEmpty() ? node->nodeName
                                                     : m_componentName + "." + node->nodeName;
        const int rc = Model_DelComponent("", fullName.toLocal8Bit().constData());
        if(rc != RDK_SUCCESS)
        {
            QMessageBox::warning(this, "Warning",
                QString("Failed to delete component %1 (code %2).")
                    .arg(fullName).arg(rc));
            return;
        }
    }

    // Очищаем сохраненные позиции удаленных узлов
    for(UModernDiagramNodeItem* node : nodesToDelete)
    {
        m_lastNodePositions.remove(node);
    }

    Reload();
    emit updateComponentsList();
}

// Public methods for context menu manager to emit signals
void UModernDiagramWidget::emitViewLinks(const QString& componentName)
{
    emit viewLinks(componentName);
}

void UModernDiagramWidget::emitCreateLinks(const QString& firstComponentName, const QString& secondComponentName)
{
    emit createLinks(firstComponentName, secondComponentName);
}

void UModernDiagramWidget::emitSwitchLinks(const QString& firstComponentName, const QString& secondComponentName)
{
    emit switchLinks(firstComponentName, secondComponentName);
}

void UModernDiagramWidget::emitOpenComponentGui(const UComponentGuiContext& context)
{
    emit openComponentGuiRequested(context);
}

// Viewport State Management теперь в UModernDiagramViewportManager

void UModernDiagramWidget::SaveViewState()
{
    if(m_viewportManager)
    {
        m_viewportManager->saveToSettings();
    }
}

void UModernDiagramWidget::LoadViewState()
{
    if(m_viewportManager)
    {
        m_viewportManager->loadFromSettings();
    }
}

int UModernDiagramWidget::selectNodesInRect(const QRectF& selectionRect, bool addToSelection)
{
    QString logMsg;

    if(!m_scene)
    {
        return 0;
    }

    // Очищаем предыдущее выделение, если не добавляем к выделению
    if(!addToSelection)
    {
        m_scene->clearSelection();
    }

    // Используем прямой перебор всех узлов для более надежного выделения
    // Это гарантирует, что мы проверяем все NodeItem, а не только те, что вернул items()
    int selectedCount = 0;
    int totalNodes = m_nodes.size();

    if(totalNodes == 0)
    {
        return 0;
    }

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Собираем все узлы для выделения в список,
    // а затем устанавливаем выделение для всех одновременно.
    // Это предотвращает автоматический сброс выделения Qt при последовательном вызове setSelected(true)
    QList<UModernDiagramNodeItem*> nodesToSelect;

    for(UModernDiagramNodeItem* node : m_nodes)
    {
        if(!node)
            continue;

        // Получаем boundingRect узла в координатах scene
        QRectF nodeRect = node->sceneBoundingRect();

        // Проверяем, пересекается ли узел с прямоугольником выделения
        bool intersects = selectionRect.intersects(nodeRect);
        if(intersects)
        {
            nodesToSelect.append(node);
            selectedCount++;
        }
    }

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Устанавливаем выделение для всех узлов одновременно,
    // используя флаг m_isBatchSelecting, чтобы предотвратить обработку ItemSelectedHasChanged
    // в itemChange, которая сбрасывает выделение Qt
    if(!nodesToSelect.isEmpty())
    {
        // Блокируем сигналы сцены и устанавливаем флаг batch-выделения
        m_scene->blockSignals(true);
        m_isProgrammaticSelection = true;
        m_isBatchSelecting = true;

        // ДИАГНОСТИКА: Проверяем выделение ПЕРЕД установкой для всех узлов
        QList<QGraphicsItem*> beforeBatchSelect = m_scene->selectedItems();
        int beforeBatchSelectCount = 0;
        QStringList beforeBatchSelectNames;
        for(QGraphicsItem* item : beforeBatchSelect)
        {
            auto* n = dynamic_cast<UModernDiagramNodeItem*>(item);
            if(n && n->isSelected())
            {
                beforeBatchSelectCount++;
                beforeBatchSelectNames << n->nodeName;
            }
        }
        QString beforeBatchMsg = QString("selectNodesInRect: ПЕРЕД batch setSelected выделено %1 объектов: %2")
            .arg(beforeBatchSelectCount).arg(beforeBatchSelectNames.join(", "));

        // Устанавливаем выделение для всех узлов одновременно
        for(UModernDiagramNodeItem* node : nodesToSelect)
        {
            node->setSelected(true);
        }

        // Сбрасываем флаг batch-выделения и разблокируем сигналы сцены
        m_isBatchSelecting = false;
        m_scene->blockSignals(false);

        // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Откладываем сброс флага m_isProgrammaticSelection,
        // чтобы он оставался установленным во время обработки всех накопленных событий ItemSelectedHasChanged
        // после разблокировки сигналов. Это предотвращает эмиссию componentSelected и рекурсию.
        QTimer::singleShot(0, [this]() {
            m_isProgrammaticSelection = false;
        });

        // ДИАГНОСТИКА: Проверяем выделение ПОСЛЕ установки для всех узлов
        QList<QGraphicsItem*> afterBatchSelect = m_scene->selectedItems();
        int afterBatchSelectCount = 0;
        QStringList afterBatchSelectNames;
        for(QGraphicsItem* item : afterBatchSelect)
        {
            auto* n = dynamic_cast<UModernDiagramNodeItem*>(item);
            if(n && n->isSelected())
            {
                afterBatchSelectCount++;
                afterBatchSelectNames << n->nodeName;
            }
        }
        QString afterBatchMsg = QString("selectNodesInRect: ПОСЛЕ batch setSelected выделено %1 объектов: %2")
            .arg(afterBatchSelectCount).arg(afterBatchSelectNames.join(", "));

        if(afterBatchSelectCount != nodesToSelect.size())
        {
            QStringList expectedNames;
            for(UModernDiagramNodeItem* n : nodesToSelect) expectedNames << n->nodeName;
            QString warningMsg = QString("selectNodesInRect: ВНИМАНИЕ - после batch setSelected выделено %1 объектов вместо ожидаемых %2! Ожидались: %3, фактически выделены: %4")
                .arg(afterBatchSelectCount)
                .arg(nodesToSelect.size())
                .arg(expectedNames.join(", "))
                .arg(afterBatchSelectNames.join(", "));
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, warningMsg.toStdString().c_str(), 0);
        }
    }

    return selectedCount;
}

// Tooltip generation methods теперь в UModernDiagramTooltipGenerator

UModernDiagramNodeItem* UModernDiagramWidget::resolveNodeOnDiagram(const QString& fullOrRelative) const
{
    if(auto it = m_nodeByName.find(fullOrRelative); it != m_nodeByName.end())
        return it.value();
    if(!m_componentName.isEmpty() && fullOrRelative.startsWith(m_componentName + "."))
    {
        QString trimmed = fullOrRelative.mid(m_componentName.size() + 1);
        int dot = trimmed.indexOf('.');
        QString top = dot >= 0 ? trimmed.left(dot) : trimmed;
        if(auto it2 = m_nodeByName.find(top); it2 != m_nodeByName.end())
            return it2.value();
    }
    int dot = fullOrRelative.indexOf('.');
    QString top = dot >= 0 ? fullOrRelative.left(dot) : fullOrRelative;
    if(auto it = m_nodeByName.find(top); it != m_nodeByName.end())
        return it.value();
    return nullptr;
}

UModernDiagramNodeItem* UModernDiagramWidget::resolveNodeByIdOnDiagram(const QString& id) const
{
    QString base = id;
    int dot = base.indexOf('.');
    if(dot >= 0)
        base = base.left(dot);
    if(!m_componentName.isEmpty() && base.startsWith(m_componentName + "."))
        base = base.mid(m_componentName.size() + 1);
    if(auto it = m_nodeByName.find(base); it != m_nodeByName.end())
        return it.value();
    return nullptr;
}

UModernDiagramNodeItem* UModernDiagramWidget::resolveDestinationNodeOnDiagram(const QString& connName,
                                                                            const QString& connId) const
{
    if(UModernDiagramNodeItem* node = resolveNodeOnDiagram(connName))
        return node;
    if(UModernDiagramNodeItem* node = resolveNodeByIdOnDiagram(connId))
        return node;

    QString path = connId.isEmpty() ? connName : connId;
    if(!m_componentName.isEmpty())
    {
        const QString scopePrefix = m_componentName + QLatin1Char('.');
        if(path.startsWith(scopePrefix))
            path = path.mid(scopePrefix.size());
    }
    const int dot = path.indexOf(QLatin1Char('.'));
    const QString top = dot >= 0 ? path.left(dot) : path;
    if(!top.isEmpty())
    {
        if(auto it = m_nodeByName.find(top); it != m_nodeByName.end())
            return it.value();
    }

    if(connectorTargetsScopeInput(connName, connId))
    {
        if(UModernDiagramNodeItem* child = findOwnerInputTargetChild())
            return child;
    }
    return nullptr;
}

QString UModernDiagramWidget::scopeShortName() const
{
    if(m_componentName.isEmpty())
        return QString();
    return m_componentName.section(QLatin1Char('.'), -1);
}

QString UModernDiagramWidget::parentScopeName() const
{
    const int dot = m_componentName.lastIndexOf(QLatin1Char('.'));
    return dot >= 0 ? m_componentName.left(dot) : QString();
}

bool UModernDiagramWidget::connectorTargetsScopeInput(const QString& connName,
                                                      const QString& connId) const
{
    if(m_componentName.isEmpty())
        return false;

    const QString scopeShort = scopeShortName();
    const QString scopeLong = m_componentName;

    const auto matchesScope = [&](const QString& idOrName) {
        if(idOrName.isEmpty())
            return false;
        if(idOrName == scopeShort || idOrName == scopeLong)
            return true;
        return idOrName.endsWith(QLatin1Char('.') + scopeShort);
    };

    if(connName == QLatin1String("Input") && matchesScope(connId))
        return true;
    if(connName == scopeShort + QStringLiteral(".Input") || connName == scopeLong + QStringLiteral(".Input"))
        return true;
    if(connName.endsWith(QStringLiteral(".Input")))
    {
        const QString base = connName.left(connName.size() - QStringLiteral(".Input").size());
        if(matchesScope(base))
            return true;
    }
    return false;
}

UModernDiagramNodeItem* UModernDiagramWidget::findOwnerInputTargetChild() const
{
    if(auto it = m_nodeByName.constFind(QStringLiteral("Segment1")); it != m_nodeByName.constEnd())
        return it.value();

    const QStringList children = loadComponentList();
    for(const QString& name : children)
    {
        if(UModernDiagramNodeItem* node = m_nodeByName.value(name))
            return node;
    }
    return nullptr;
}

QString UModernDiagramWidget::formatExternalSourceLabel(const QString& itemId, const QString& itemName)
{
    if(itemId.isEmpty())
        return itemName;
    if(itemName.isEmpty())
        return itemId;
    if(itemId.endsWith(QLatin1Char('.') + itemName))
        return itemId;
    return itemId + QLatin1Char('.') + itemName;
}

QString UModernDiagramWidget::externalSourcePositionKey(const QString& sourceKey) const
{
    return m_componentName + QLatin1Char('|') + sourceKey;
}

bool UModernDiagramWidget::isLinkEndpointInsideCurrentScope(const QString& path) const
{
    if(path.isEmpty())
        return false;

    QString relative = path;
    if(!m_componentName.isEmpty())
    {
        if(relative == m_componentName)
            return false;
        const QString scopePrefix = m_componentName + QLatin1Char('.');
        if(relative.startsWith(scopePrefix))
            relative = relative.mid(scopePrefix.size());
        else if(relative.contains(QLatin1Char('.')))
        {
            const QString scopeShort = scopeShortName();
            if(relative.startsWith(scopeShort + QLatin1Char('.')))
                relative = relative.mid(scopeShort.size() + 1);
        }
    }

    const int dot = relative.indexOf(QLatin1Char('.'));
    const QString top = dot >= 0 ? relative.left(dot) : relative;
    return !top.isEmpty() && m_nodeByName.contains(top);
}

bool UModernDiagramWidget::isExternalLinkSource(const QString& itemName, const QString& itemId) const
{
    if(isLinkEndpointInsideCurrentScope(itemId) || isLinkEndpointInsideCurrentScope(itemName))
        return false;
    if(resolveNodeOnDiagram(itemName) || resolveNodeByIdOnDiagram(itemId))
        return false;
    return true;
}

bool UModernDiagramWidget::isConnectorNestedInsideVisibleChild(const QString& connName,
                                                               const QString& connId) const
{
    QString path = connId.isEmpty() ? connName : connId;
    if(path.isEmpty())
        return false;

    QString relative = path;
    if(!m_componentName.isEmpty())
    {
        if(relative == m_componentName)
            return false;
        const QString scopePrefix = m_componentName + QLatin1Char('.');
        if(relative.startsWith(scopePrefix))
            relative = relative.mid(scopePrefix.size());
        else if(relative.contains(QLatin1Char('.')))
        {
            const QString scopeShort = scopeShortName();
            if(relative.startsWith(scopeShort + QLatin1Char('.')))
                relative = relative.mid(scopeShort.size() + 1);
        }
    }

    const int dot = relative.indexOf(QLatin1Char('.'));
    if(dot < 0)
        return false;

    const QString top = relative.left(dot);
    if(!m_nodeByName.contains(top))
        return false;

    const QString rest = relative.mid(dot + 1);
    return rest.contains(QLatin1Char('.'));
}

QString UModernDiagramWidget::normalizeConnectorNameForDst(UModernDiagramNodeItem* dstNode,
                                                           const QString& connName,
                                                           const QString& connIdStr) const
{
    if(!dstNode)
        return connName;

    QString normalizedConnName = connName;
    const QString dstNodeName = dstNode->nodeName;
    const QString fullDstNodePath = m_componentName.isEmpty()
        ? dstNodeName
        : m_componentName + "." + dstNodeName;

    if(!connName.isEmpty())
    {
        if(m_componentName.isEmpty())
        {
            if(connName.startsWith(dstNodeName + "."))
                normalizedConnName = connName.mid(dstNodeName.length() + 1);
            else if(!connName.contains('.'))
            {
                if(!connIdStr.isEmpty() && connIdStr.startsWith(dstNodeName + "."))
                {
                    QString pathFromConnId = connIdStr.mid(dstNodeName.length() + 1);
                    normalizedConnName = pathFromConnId.contains('.')
                        ? pathFromConnId
                        : pathFromConnId + "." + connName;
                }
            }
            else if(!connIdStr.isEmpty() && connIdStr.startsWith(dstNodeName + "."))
            {
                QString pathFromConnId = connIdStr.mid(dstNodeName.length() + 1);
                normalizedConnName = pathFromConnId.contains('.') ? pathFromConnId : connName;
            }
        }
        else
        {
            if(connName.startsWith(fullDstNodePath + "."))
                normalizedConnName = connName.mid(fullDstNodePath.length() + 1);
            else if(connName.startsWith(dstNodeName + "."))
                normalizedConnName = connName.mid(dstNodeName.length() + 1);
        }
    }
    else if(!connIdStr.isEmpty())
    {
        if(connIdStr.startsWith(fullDstNodePath + "."))
            normalizedConnName = connIdStr.mid(fullDstNodePath.length() + 1);
        else if(connIdStr.startsWith(dstNodeName + "."))
            normalizedConnName = connIdStr.mid(dstNodeName.length() + 1);
        else
            normalizedConnName = connIdStr;
    }
    return normalizedConnName;
}

PortCategory UModernDiagramWidget::resolveInputPortCategory(
    UModernDiagramNodeItem* dstNode,
    const QString& normalizedConnName,
    QHash<QPair<UModernDiagramNodeItem*, QString>, PortCategory>& portCategoryCache) const
{
    QPair<UModernDiagramNodeItem*, QString> dstKey(dstNode, normalizedConnName);
    if(portCategoryCache.contains(dstKey))
        return portCategoryCache[dstKey];
    const PortCategory cat = dstNode->determinePortCategory(normalizedConnName, true);
    portCategoryCache[dstKey] = cat;
    return cat;
}

void UModernDiagramWidget::clearExternalSources()
{
    for(auto* ext : m_externalSources)
    {
        if(ext)
            m_externalSourcePosCache.insert(externalSourcePositionKey(ext->outputFullId()), ext->pos());
        if(ext && m_scene)
            m_scene->removeItem(ext);
        delete ext;
    }
    m_externalSources.clear();
    m_externalSourceByKey.clear();
}

void UModernDiagramWidget::processExternalIncomingFromLinksList(
    const RDK::UStringLinksList& linkslist,
    QHash<UModernDiagramNodeItem*, int>& stackCounter,
    QHash<QString, UModernDiagramLinkItem*>& aggregatedExternalLinks)
{
    QHash<QPair<UModernDiagramNodeItem*, QString>, PortCategory> portCategoryCache;

    for(int i = 0; i < linkslist.GetSize(); ++i)
    {
        const auto& link = linkslist[i];
        const QString itemName = QString::fromStdString(link.Item.Name);
        const QString itemId = QString::fromStdString(link.Item.Id);
        const QString displayLabel = formatExternalSourceLabel(itemId, itemName);

        if(!isExternalLinkSource(itemName, itemId))
            continue;

        const QString sourceKey = formatExternalSourceLabel(itemId, itemName);
        if(sourceKey.isEmpty())
            continue;

        for(size_t c = 0; c < link.Connector.size(); ++c)
        {
            const auto& connSide = link.Connector[c];
            const QString connName = QString::fromStdString(connSide.Name);
            const QString connIdStr = QString::fromStdString(connSide.Id);

            UModernDiagramNodeItem* dstNode = resolveDestinationNodeOnDiagram(connName, connIdStr);
            if(!dstNode)
                continue;

            const bool ownerInput = connectorTargetsScopeInput(connName, connIdStr);
            if(!ownerInput && isConnectorNestedInsideVisibleChild(connName, connIdStr))
                continue;

            const QString normalizedConnName = ownerInput
                ? QStringLiteral("Input")
                : normalizeConnectorNameForDst(dstNode, connName, connIdStr);
            const PortCategory dstCategory =
                resolveInputPortCategory(dstNode, normalizedConnName, portCategoryCache);

            UModernDiagramExternalSourceItem* ext = m_externalSourceByKey.value(sourceKey, nullptr);
            if(!ext)
            {
                ext = new UModernDiagramExternalSourceItem(this, displayLabel, itemName);
                m_externalSources.append(ext);
                m_externalSourceByKey.insert(sourceKey, ext);
                m_scene->addItem(ext);
            }

            const QString aggKey = sourceKey + QLatin1Char('|')
                + QString::number(quintptr(dstNode)) + QLatin1Char('|')
                + QString::number(static_cast<int>(dstCategory));
            if(UModernDiagramLinkItem* existing = aggregatedExternalLinks.value(aggKey, nullptr))
            {
                existing->incrementParallelCount();
                existing->updateGeometry();
                continue;
            }

            auto* linkItem = new UModernDiagramLinkItem(ext, dstNode, dstCategory, displayLabel);
            aggregatedExternalLinks.insert(aggKey, linkItem);
            m_links.append(linkItem);
            m_scene->addItem(linkItem);
            dstNode->m_connectedLinks.append(linkItem);
            linkItem->updateGeometry();
        }
    }
}

bool UModernDiagramWidget::appendExternalIncomingLinksFromXml(
    const std::string& linksXml,
    QHash<UModernDiagramNodeItem*, int>& stackCounter,
    QHash<QString, UModernDiagramLinkItem*>& aggregatedExternalLinks)
{
    if(linksXml.empty())
        return false;

    RDK::USerStorageXML xml;
    if(!xml.Load(linksXml, "Links"))
        return false;

    RDK::UStringLinksList linkslist;
    xml >> linkslist;
    if(linkslist.GetSize() == 0)
        return false;

    processExternalIncomingFromLinksList(linkslist, stackCounter, aggregatedExternalLinks);
    return true;
}

void UModernDiagramWidget::layoutExternalSources()
{
    if(m_externalSources.isEmpty())
        return;

    QRectF nodesBounds;
    QList<QRectF> obstacleRects;
    for(UModernDiagramNodeItem* node : m_nodes)
    {
        const QRectF nodeRect = node->sceneBoundingRect();
        nodesBounds = nodesBounds.isEmpty() ? nodeRect : nodesBounds.united(nodeRect);
        obstacleRects.append(nodeRect);
    }

    int stackIndex = 0;
    for(UModernDiagramExternalSourceItem* ext : m_externalSources)
    {
        if(!ext)
            continue;

        const QString posKey = externalSourcePositionKey(ext->outputFullId());
        if(m_externalSourcePosCache.contains(posKey))
        {
            ext->setPos(m_externalSourcePosCache.value(posKey));
            obstacleRects.append(ext->sceneBoundingRect());
            continue;
        }

        QList<UModernDiagramNodeItem*> targetNodes;
        for(UModernDiagramLinkItem* link : m_links)
        {
            if(link && link->externalSource() == ext && link->dst())
                targetNodes.append(link->dst());
        }

        ext->layoutOptimal(nodesBounds, obstacleRects, targetNodes, stackIndex * 24.0);
        m_externalSourcePosCache.insert(posKey, ext->pos());
        obstacleRects.append(ext->sceneBoundingRect());
        ++stackIndex;

        for(UModernDiagramLinkItem* link : m_links)
        {
            if(link && link->externalSource() == ext)
                link->updateGeometry();
        }
    }
}

void UModernDiagramWidget::buildExternalIncomingLinks()
{
    clearExternalSources();
    if(m_nodes.isEmpty() || !m_scene)
        return;

    const int channel = Core_GetSelectedChannelIndex();
    QHash<UModernDiagramNodeItem*, int> stackCounter;
    QHash<QString, UModernDiagramLinkItem*> aggregatedExternalLinks;

    if(!m_componentName.isEmpty())
    {
        // Personal/external links of the scope container (e.g. PGenerator -> AxoneChain3.Input).
        appendExternalIncomingLinksFromXml(
            personalLinksXmlFromModelScope(channel, m_componentName, parentScopeName(), true),
            stackCounter,
            aggregatedExternalLinks);

        // Parent-scope links: sources outside this diagram level.
        appendExternalIncomingLinksFromXml(
            internalLinksXmlFromModelScope(channel, parentScopeName(), true),
            stackCounter,
            aggregatedExternalLinks);
    }

    const QStringList children = loadComponentList();
    for(const QString& shortName : children)
    {
        const QString childLong = m_componentName.isEmpty()
            ? shortName
            : m_componentName + "." + shortName;
        appendExternalIncomingLinksFromXml(
            personalLinksXmlFromModelScope(channel, childLong, m_componentName, true),
            stackCounter,
            aggregatedExternalLinks);
    }

    layoutExternalSources();

    for(auto* node : m_nodes)
        node->m_cacheValid = false;

    updateSceneRect();
}

// --------------------------- ComponentCache ---------------------------


