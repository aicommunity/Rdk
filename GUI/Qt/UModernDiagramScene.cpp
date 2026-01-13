#include "UModernDiagramScene.h"
#include "UModernDiagramWidget.h"
#include "UModernDiagramView.h"
#include "UModernDiagramLinkItem.h"
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramTooltipGenerator.h"
#include "UModernDiagramViewportManager.h"
#include "UModernDiagramCoordinateManager.h"
#include "UModernDiagramContextMenu.h"

#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QPointF>
#include <QLineF>
#include <QMenu>
#include <QRectF>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QMap>
#include <QSet>
#include <QList>
#include <QGraphicsItem>
#include <QTransform>
#include <QCursor>
#include <limits>
#include "../../Deploy/Include/rdk_init.h"
#include "../Core/Engine/UEngine.h"
#include "../Core/Engine/UNet.h"
#include <QMessageBox>
#include <QVector>

UModernDiagramScene::UModernDiagramScene(UModernDiagramWidget* owner)
    : m_owner(owner), m_isRubberBandActive(false), m_isGroupSelected(false), m_isGroupMoving(false), m_lastHoverPos()
{
    m_hoverTimer = new QTimer(this);
    m_hoverTimer->setInterval(80); // ~12 fps, достаточно для отслеживания
    connect(m_hoverTimer, &QTimer::timeout, this, &UModernDiagramScene::pollHover);
    m_hoverTimer->start();
}

void UModernDiagramScene::pollHover()
{
    if(!m_owner)
        return;
    if(views().isEmpty())
        return;

    // Берем первую view (у нас одна основная)
    QGraphicsView* view = views().first();
    QPointF scenePos = view->mapToScene(view->mapFromGlobal(QCursor::pos()));

    // Оптимизация: проверяем, изменилась ли позиция курсора значительно
    // Если позиция не изменилась более чем на 5 пикселей, пропускаем обновление hover для узлов
    const double minHoverDistance = 5.0;
    bool positionChanged = (QLineF(m_lastHoverPos, scenePos).length() > minHoverDistance);
    if(!positionChanged && !m_lastHoverPos.isNull())
    {
        // Позиция не изменилась значительно, но все равно обновляем временную линию если она активна
        if(m_owner->m_activeTempLink && !m_owner->m_isWaitingForPortSelection)
        {
            QPointF targetPos = m_owner->m_isLineFrozen ?
                                m_owner->m_frozenTargetPortPos : scenePos;
            m_owner->m_activeTempLink->updateGeometry(targetPos);
        }
        return;
    }
    m_lastHoverPos = scenePos;

    // Оптимизация: пропускаем обновление hover во время обработки событий мыши
    // Это предотвращает задержки при клике
    if(m_isProcessingMouseEvent)
    {
        return;
    }

    // Получаем видимую область viewport для оптимизации
    QRectF visibleRect = view->mapToScene(view->viewport()->rect()).boundingRect();

    // Обновляем состояние заморозки соединения
    if(m_owner->m_activeTempLink)
    {
        const double portFreezeRadius = 35.0; // Радиус буферной зоны вокруг порта (пиксели)
        const double widgetFreezeDistance = 60.0; // Расстояние до окна дерева портов (пиксели)

        bool shouldFreezeLine = false;
        QPointF targetPortPos;

        // Проверяем буферную зону вокруг входного порта
        QPointF portPos;
        UModernDiagramNodeItem* portNode = nullptr;
        const UModernDiagramWidget::Port* port = m_owner->pickPortDetailed(scenePos, true, portNode, portPos);
        if(port && port->isInput)
        {
            double distToPort = QLineF(scenePos, portPos).length();
            if(distToPort <= portFreezeRadius)
            {
                shouldFreezeLine = true;
                targetPortPos = portPos;
            }
        }

        // Проверяем буферную зону вокруг окна дерева портов
        if(!shouldFreezeLine)
        {
            for(UModernDiagramNodeItem* node : m_owner->m_nodes)
            {
                if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
                {
                    QRectF widgetRect = node->m_portListWidgetProxy->mapToScene(
                        node->m_portListWidgetProxy->boundingRect()).boundingRect();
                    QRectF expandedRect = widgetRect.adjusted(-widgetFreezeDistance, -widgetFreezeDistance,
                                                                  widgetFreezeDistance, widgetFreezeDistance);
                    if(expandedRect.contains(scenePos))
                    {
                        // Находим ближайший входной порт этого узла
                        QPointF nearestInputPortPos;
                        double minDist = std::numeric_limits<double>::max();
                        bool foundPort = false;
                        for(const UModernDiagramWidget::Port& p : node->inputs)
                        {
                            QPointF pPos = node->mapToScene(p.pos);
                            double dist = QLineF(scenePos, pPos).length();
                            if(dist < minDist)
                            {
                                minDist = dist;
                                nearestInputPortPos = pPos;
                                foundPort = true;
                            }
                        }
                        if(foundPort && minDist <= portFreezeRadius)
                        {
                            shouldFreezeLine = true;
                            targetPortPos = nearestInputPortPos;
                            break;
                        }
                    }
                }
            }
        }

        // Проверяем, есть ли видимое окно выбора портов - если есть, не обновляем временную линию
        bool hasVisiblePortListWidget = false;
        for(UModernDiagramNodeItem* node : m_owner->m_nodes)
        {
            if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
            {
                hasVisiblePortListWidget = true;
                break;
            }
        }

        // Обновляем состояние заморозки
        m_owner->m_isLineFrozen = shouldFreezeLine;
        if(shouldFreezeLine)
        {
            m_owner->m_frozenTargetPortPos = targetPortPos;
        }

        // Обновляем геометрию временной линии только если не ожидается выбор порта
        // и нет видимого окна выбора портов
        if(!m_owner->m_isWaitingForPortSelection && !hasVisiblePortListWidget)
        {
            QPointF targetPos = m_owner->m_isLineFrozen ?
                                m_owner->m_frozenTargetPortPos : scenePos;
            m_owner->m_activeTempLink->updateGeometry(targetPos);
        }
    }
    else
    {
        // Сбрасываем состояние заморозки, если нет активной связи
        m_owner->m_isLineFrozen = false;
    }

    // Оптимизация: обновляем hover только для узлов в видимой области или под курсором
    for(UModernDiagramNodeItem* node : m_owner->m_nodes)
    {
        // Проверяем, находится ли узел в видимой области или под курсором
        QRectF nodeRect = node->sceneBoundingRect();
        if(visibleRect.intersects(nodeRect) || nodeRect.contains(scenePos))
        {
            node->refreshHoverAtScenePos(scenePos);
        }
    }
}

void UModernDiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Устанавливаем флаг обработки событий мыши для пропуска pollHover
    m_isProcessingMouseEvent = true;

    // Обработка правого клика для отмены активной связи
    if(event->button() == Qt::RightButton && m_owner->m_activeTempLink)
    {
        m_owner->m_scene->removeItem(m_owner->m_activeTempLink);
        delete m_owner->m_activeTempLink;
        m_owner->m_activeTempLink = nullptr;
        m_owner->m_activeSourceNode = nullptr;
        m_owner->m_activeSourcePort = nullptr;
        m_owner->m_isLineFrozen = false;
        m_owner->m_isWaitingForPortSelection = false;

        // Сбрасываем курсор
        if(m_owner->m_mainView)
        {
            m_owner->m_mainView->unsetCursor();
        }

        // Закрываем все открытые деревья портов
        for(UModernDiagramNodeItem* node : m_owner->m_nodes)
        {
            node->hidePortListWidget();
        }

        event->accept();
        return;
    }

    // Сохраняем начальную позицию для RubberBandDrag при ЛКМ без Ctrl
    // Это нужно для корректной работы выделения прямоугольником
    if(event->button() == Qt::LeftButton && !(event->modifiers() & Qt::ControlModifier))
    {
        m_rubberBandStartPos = event->scenePos();
        m_isRubberBandActive = true;
    }

    if(event->button() == Qt::LeftButton)
    {
        QPointF portPos;
        UModernDiagramNodeItem* node = nullptr;

        // СНАЧАЛА проверяем входные порты (даже если есть активная связь)
        // Это позволяет открывать окно выбора входов во время создания соединения
        const UModernDiagramWidget::Port* inputPort = m_owner->pickPortDetailed(event->scenePos(), true, node, portPos);
        if(node && inputPort && inputPort->isInput)
        {
            // Устанавливаем hoveredPort для корректной работы showPortListWidget
            node->m_hoveredPort = inputPort;

            // Если есть активная связь - открываем список входов для выбора целевого порта
            // Завершение связи будет обработано в mouseReleaseEvent при клике на выбранный порт
            // Если нет активной связи - также открываем список входов для просмотра

            if(inputPort->fullPath.isEmpty() &&
               (inputPort->category == UModernDiagramWidget::PortCategory::Own ||
                inputPort->category == UModernDiagramWidget::PortCategory::Child ||
                inputPort->category == UModernDiagramWidget::PortCategory::Alias))
            {
                // Проверяем, есть ли порты для этой категории
                bool hasPorts = false;
                if (inputPort->category == UModernDiagramWidget::PortCategory::Own)
                {
                    hasPorts = !node->getOwnInputPorts().isEmpty();
                }
                else if (inputPort->category == UModernDiagramWidget::PortCategory::Child)
                {
                    hasPorts = !node->getChildInputPorts().isEmpty();
                }
                    else if (inputPort->category == UModernDiagramWidget::PortCategory::Alias)
                    {
                        hasPorts = !node->getAliasInputPorts().isEmpty();
                    }

                    if (hasPorts)
                    {
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO,
                            "mousePressEvent: Opening input port list for category", 0);

                        // Если есть активная связь, активируем состояние ожидания выбора порта
                        if(m_owner->m_activeTempLink)
                        {
                            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO,
                                "mousePressEvent: Active connection exists, setting waiting state", 0);

                            // Получаем текущую позицию курсора в сцене
                            QPointF currentCursorPos;
                            if(m_owner->m_mainView)
                            {
                                QPoint globalPos = QCursor::pos();
                                QPoint viewPos = m_owner->m_mainView->mapFromGlobal(globalPos);
                                currentCursorPos = m_owner->m_mainView->mapToScene(viewPos);
                            }
                            else
                            {
                                currentCursorPos = event->scenePos();
                            }

                            // Устанавливаем состояние ДО вызова showPortListWidget, чтобы mouseMoveEvent мог его увидеть
                            m_owner->m_isWaitingForPortSelection = true;
                            // Замораживаем временную линию на текущей позиции курсора
                            // Используем замороженную позицию, если уже заморожена, иначе текущую позицию курсора
                            bool wasFrozen = m_owner->m_isLineFrozen;
                            QPointF freezePos = wasFrozen ?
                                m_owner->m_frozenTargetPortPos : currentCursorPos;
                            m_owner->m_isLineFrozen = true;
                            m_owner->m_frozenTargetPortPos = freezePos;
                            // Обновляем геометрию временной линии один раз для заморозки
                            m_owner->m_activeTempLink->updateGeometry(freezePos);


                            // Принудительно обновляем сцену, чтобы изменения вступили в силу немедленно
                            if(m_owner->m_scene)
                            {
                                m_owner->m_scene->update();
                            }
                        }
                        else
                        {
                        }

                        node->showPortListWidget(event->scenePos());
                        event->accept();
                        return;
                    }
                else
                {
                    // Порт недоступен, ничего не делаем
                    event->accept();
                    return;
                }
            }
            else
            {
                // Для обычных входных портов (с полным путем) или портов без категории
                // показываем список всех доступных входных портов
                // Проверяем, есть ли вообще входные порты (проверяем все категории)
                QVector<UModernDiagramWidget::Port> allInputPorts;
                allInputPorts.append(node->getOwnInputPorts());
                allInputPorts.append(node->getChildInputPorts());
                allInputPorts.append(node->getAliasInputPorts());

                // Также проверяем вложенные порты
                QVector<UModernDiagramWidget::Port> nestedPorts = node->getNestedPorts(true, false);
                allInputPorts.append(nestedPorts);

                if(!allInputPorts.isEmpty())
                {
                    // Если есть активная связь, активируем состояние ожидания выбора порта
                    if(m_owner->m_activeTempLink)
                    {
                        // Получаем текущую позицию курсора в сцене
                        QPointF currentCursorPos;
                        if(m_owner->m_mainView)
                        {
                            QPoint globalPos = QCursor::pos();
                            QPoint viewPos = m_owner->m_mainView->mapFromGlobal(globalPos);
                            currentCursorPos = m_owner->m_mainView->mapToScene(viewPos);
                        }
                        else
                        {
                            currentCursorPos = event->scenePos();
                        }

                        // Устанавливаем состояние ДО вызова showPortListWidget, чтобы mouseMoveEvent мог его увидеть
                        m_owner->m_isWaitingForPortSelection = true;
                        // Замораживаем временную линию на текущей позиции курсора
                        // Используем замороженную позицию, если уже заморожена, иначе текущую позицию курсора
                        bool wasFrozen = m_owner->m_isLineFrozen;
                        QPointF freezePos = wasFrozen ?
                            m_owner->m_frozenTargetPortPos : currentCursorPos;
                        m_owner->m_isLineFrozen = true;
                        m_owner->m_frozenTargetPortPos = freezePos;
                        // Обновляем геометрию временной линии один раз для заморозки
                        m_owner->m_activeTempLink->updateGeometry(freezePos);

                        // Принудительно обновляем сцену, чтобы изменения вступили в силу немедленно
                        if(m_owner->m_scene)
                        {
                            m_owner->m_scene->update();
                        }
                    }
                    node->showPortListWidget(event->scenePos());
                    event->accept();
                    return;
                }
                else
                {
                    // Входных портов нет, ничего не делаем
                    event->accept();
                    return;
                }
            }
        }

        // Если есть активная связь из дерева портов, не начинаем новую связь через drag & drop
        // (только если мы НЕ кликнули на входной порт)
        if(m_owner->m_activeTempLink)
        {
            // Просто обновляем позицию временной линии, но не отменяем активную связь
            event->accept();
            return;
        }

        // Проверяем выходные порты
        const UModernDiagramWidget::Port* port = m_owner->pickPortDetailed(event->scenePos(), false, node, portPos);
        if(node && port && !port->isInput)
        {
            // Обработка выходных портов
            // Если это категоризированный порт (с пустым fullPath), показываем дерево портов
            if(port->fullPath.isEmpty() &&
               (port->category == UModernDiagramWidget::PortCategory::Own ||
                port->category == UModernDiagramWidget::PortCategory::Child ||
                port->category == UModernDiagramWidget::PortCategory::Alias))
            {
                // Проверяем, есть ли порты для этой категории
                bool hasPorts = false;
                if (port->category == UModernDiagramWidget::PortCategory::Own)
                {
                    hasPorts = !node->getOwnOutputPorts().isEmpty();
                }
                else if (port->category == UModernDiagramWidget::PortCategory::Child)
                {
                    hasPorts = !node->getChildOutputPorts().isEmpty();
                }
                else if (port->category == UModernDiagramWidget::PortCategory::Alias)
                {
                    hasPorts = !node->getAliasOutputPorts().isEmpty();
                }

                // Показываем дерево портов только если есть порты для этой категории
                if (hasPorts)
                {
                    node->showPortListWidget(event->scenePos());
                    event->accept();
                    return;
                }
                else
                {
                    // Порт недоступен, ничего не делаем
                    event->accept();
                    return;
                }
            }

            // Для обычных выходных портов начинаем drag & drop
            m_owner->m_dragSourceNode = node;
            m_owner->m_dragSourcePort = port;
            m_owner->m_dragSourcePortPos = portPos;
            m_owner->m_tempLink = new UModernDiagramLinkItem(node, event->scenePos());
            m_owner->m_scene->addItem(m_owner->m_tempLink);
            event->accept();
            return;
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        // Проверяем, попали ли мы в узел
        auto* node = m_owner->pickNode(event->scenePos());
        if(node)
        {
            QString fullName = m_owner->m_componentName.isEmpty() ? node->nodeName
                                                                  : m_owner->m_componentName + "." + node->nodeName;
            // DEBUG: Commented out to reduce log flood
            // QString logMsg = QString("[SELECTION_DEBUG] UModernDiagramScene::mousePressEvent: right click on node '%1', setting m_isProgrammaticSelection=true, calling clearSelection() and setSelected(true)")
            //     .arg(fullName);
            // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

            // Используем флаг m_isProgrammaticSelection, чтобы предотвратить эмиссию componentSelected
            // при программном изменении выделения (эмиттим сигнал вручную ниже)
            m_owner->m_isProgrammaticSelection = true;
            m_owner->m_scene->clearSelection();
            node->setSelected(true);
            // Сбрасываем флаг перед эмиссией сигнала, чтобы сигнал был обработан
            m_owner->m_isProgrammaticSelection = false;
            emit m_owner->componentSelected(fullName);
            // Показываем контекстное меню
            QPoint globalPos = m_owner->m_mainView->mapToGlobal(m_owner->m_mainView->mapFromScene(event->scenePos()));
            if(m_owner->m_contextMenuManager)
                m_owner->m_contextMenuManager->showMenu(globalPos, node);
            event->accept();
            return;
        }
    }

    // Проверяем, был ли клик на фоне (не на узле, не на порте, не на окне со списком портов)
    bool clickedOnBackground = true;

    // Проверяем, попали ли в узел
    auto* clickedNode = m_owner->pickNode(event->scenePos());
    if(clickedNode)
    {
        clickedOnBackground = false;

        // Если клик на выделенном узле и есть группа выделенных объектов, сохраняем выделение для перемещения
        if(event->button() == Qt::LeftButton && !(event->modifiers() & Qt::ControlModifier) && clickedNode->isSelected())
        {
            // Устанавливаем флаг движения компонентов для отложенного обновления sceneRect
            m_owner->m_isComponentMoving = true;

            QList<QGraphicsItem*> selectedItems = m_owner->m_scene->selectedItems();
            int selectedNodeCount = 0;
            for(QGraphicsItem* item : selectedItems)
            {
                auto* selectedNode = dynamic_cast<UModernDiagramNodeItem*>(item);
                if(selectedNode && selectedNode->isSelected())
                {
                    selectedNodeCount++;
                }
            }

            // Если выделено больше одного объекта, сохраняем выделение для перемещения группы
            if(selectedNodeCount > 1)
            {
                m_savedSelection.clear();
                for(QGraphicsItem* item : selectedItems)
                {
                    auto* selectedNode = dynamic_cast<UModernDiagramNodeItem*>(item);
                    if(selectedNode && selectedNode->isSelected())
                    {
                        m_savedSelection.append(selectedNode);
                    }
                }
                m_isGroupMoving = true;
                m_isGroupSelected = true;
            }
        }
        else if(event->button() == Qt::LeftButton && !(event->modifiers() & Qt::ControlModifier))
        {
            // Клик на узле (даже если не выделен) - может начаться движение
            // Устанавливаем флаг движения компонентов
            m_owner->m_isComponentMoving = true;
        }
    }

    // Проверяем, попали ли в порт
    if(clickedOnBackground)
    {
        QPointF portPos;
        UModernDiagramNodeItem* portNode = nullptr;
        const UModernDiagramWidget::Port* port = m_owner->pickPortDetailed(event->scenePos(), false, portNode, portPos);
        if(port)
        {
            clickedOnBackground = false;
        }
    }

    // Проверяем, попали ли в окно со списком портов
    if(clickedOnBackground)
    {
        for(UModernDiagramNodeItem* inner_node : m_owner->m_nodes)
        {
            if(inner_node->m_portListWidgetProxy && inner_node->m_portListWidgetProxy->isVisible())
            {
                QRectF widgetRect = inner_node->m_portListWidgetProxy->mapToScene(
                    inner_node->m_portListWidgetProxy->boundingRect()).boundingRect();
                if(widgetRect.contains(event->scenePos()))
                {
                    clickedOnBackground = false;
                    // Логируем, что клик попал в окно выбора портов
                    // Передаем событие виджету, чтобы он мог обработать клик
                    QGraphicsScene::mousePressEvent(event);
                    return;
                }
            }
        }
    }

    // Если клик на фоне, закрываем все окна со списком портов
    if(clickedOnBackground)
    {
        for(UModernDiagramNodeItem* node : m_owner->m_nodes)
        {
            node->hidePortListWidget();
        }
        // Если было состояние ожидания выбора порта, сбрасываем его
        if(m_owner->m_isWaitingForPortSelection)
        {
            m_owner->m_isWaitingForPortSelection = false;
            m_owner->m_isLineFrozen = false;
        }

        // Сбрасываем выделение при клике на фоне (левой или правой кнопкой)
        // Используем флаг m_isProgrammaticSelection, чтобы предотвратить эмиссию componentSelected
        // при программном сбросе выделения
        if(m_isGroupSelected || m_isGroupMoving || !m_owner->m_scene->selectedItems().isEmpty())
        {
            int selectedCount = m_owner->m_scene->selectedItems().size();
            // DEBUG: Commented out to reduce log flood
            // QString logMsg = QString("[SELECTION_DEBUG] UModernDiagramScene::mousePressEvent: clicked on background, isGroupSelected=%1, isGroupMoving=%2, selectedItems.count=%3, setting m_isProgrammaticSelection=true, calling clearSelection()")
            //     .arg(m_isGroupSelected ? "true" : "false")
            //     .arg(m_isGroupMoving ? "true" : "false")
            //     .arg(selectedCount);
            // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

            m_owner->m_isProgrammaticSelection = true;
            m_owner->m_scene->clearSelection();
            m_isGroupSelected = false;
            m_isGroupMoving = false;
            m_savedSelection.clear();

            // Отложенный сброс флага, чтобы он оставался установленным во время обработки всех событий
            QTimer::singleShot(0, [this]() {
                m_owner->m_isProgrammaticSelection = false;
            });
        }

        // Если клик на фоне и ЛКМ без Ctrl (не прокрутка), передаем событие для RubberBandDrag
        // Ctrl+ЛКМ обрабатывается в UModernDiagramView для прокрутки
        if(event->button() == Qt::LeftButton && !(event->modifiers() & Qt::ControlModifier))
        {
            // Начальная позиция уже сохранена в начале метода
            // Передаем событие в базовый класс для обработки RubberBandDrag
            QGraphicsScene::mousePressEvent(event);
            return;
        }
    }

    // Если не клик на фоне, но это ЛКМ без Ctrl, флаг уже установлен в начале метода
    // Иначе сбрасываем флаг RubberBandDrag
    if(event->button() != Qt::LeftButton || (event->modifiers() & Qt::ControlModifier))
    {
        m_isRubberBandActive = false;
    }
    QGraphicsScene::mousePressEvent(event);

    // Сбрасываем флаг после обработки события
    m_isProcessingMouseEvent = false;
}

void UModernDiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_owner->m_tempLink)
    {
        m_owner->m_tempLink->updateGeometry(event->scenePos());
        event->accept();
        return;
    }

    // Сначала проверяем, есть ли видимое окно выбора портов - если есть, не обновляем временную линию
    bool hasVisiblePortListWidget = false;
    for(UModernDiagramNodeItem* node : m_owner->m_nodes)
    {
        if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
        {
            hasVisiblePortListWidget = true;
            break;
        }
    }

    // Обновляем временную линию активной связи из дерева портов
    if(m_owner->m_activeTempLink)
    {
        // Если ожидается выбор порта из окна выбора ИЛИ есть видимое окно выбора портов,
        // не обновляем временную линию и передаем событие дальше
        if(m_owner->m_isWaitingForPortSelection || hasVisiblePortListWidget)
        {
            // Передаем событие дальше в иерархию, чтобы оно могло обрабатываться окном выбора
            // Но не обновляем геометрию временной линии
            // ВАЖНО: Используем ignore() вместо accept(), чтобы событие могло быть обработано виджетом
            event->ignore();
            QGraphicsScene::mouseMoveEvent(event);
            return;
        }

        const double portFreezeRadius = 35.0; // Радиус буферной зоны вокруг порта (пиксели)
        const double widgetFreezeDistance = 60.0; // Расстояние до окна дерева портов (пиксели)

        bool shouldFreezeLine = false;
        QPointF targetPortPos;

        // Проверяем буферную зону вокруг входного порта
        QPointF portPos;
        UModernDiagramNodeItem* portNode = nullptr;
        const UModernDiagramWidget::Port* port = m_owner->pickPortDetailed(event->scenePos(), true, portNode, portPos);
        if(port && port->isInput)
        {
            double distToPort = QLineF(event->scenePos(), portPos).length();
            if(distToPort <= portFreezeRadius)
            {
                shouldFreezeLine = true;
                targetPortPos = portPos;
            }
        }

        // Проверяем буферную зону вокруг окна дерева портов
        if(!shouldFreezeLine)
        {
            for(UModernDiagramNodeItem* node : m_owner->m_nodes)
            {
                if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
                {
                    QRectF widgetRect = node->m_portListWidgetProxy->mapToScene(
                        node->m_portListWidgetProxy->boundingRect()).boundingRect();
                    QRectF expandedRect = widgetRect.adjusted(-widgetFreezeDistance, -widgetFreezeDistance,
                                                              widgetFreezeDistance, widgetFreezeDistance);
                    if(expandedRect.contains(event->scenePos()))
                    {
                        // Находим ближайший входной порт этого узла
                        QPointF nearestInputPortPos;
                        double minDist = std::numeric_limits<double>::max();
                        bool foundPort = false;
                        for(const UModernDiagramWidget::Port& p : node->inputs)
                        {
                            QPointF pPos = node->mapToScene(p.pos);
                            double dist = QLineF(event->scenePos(), pPos).length();
                            if(dist < minDist)
                            {
                                minDist = dist;
                                nearestInputPortPos = pPos;
                                foundPort = true;
                            }
                        }
                        if(foundPort && minDist <= portFreezeRadius)
                        {
                            shouldFreezeLine = true;
                            targetPortPos = nearestInputPortPos;
                            break;
                        }
                    }
                }
            }
        }

        // Обновляем состояние заморозки
        m_owner->m_isLineFrozen = shouldFreezeLine;
        if(shouldFreezeLine)
        {
            m_owner->m_frozenTargetPortPos = targetPortPos;
        }

        // Обновляем геометрию временной линии только если не ожидается выбор порта
        // и нет видимого окна выбора портов
        if(!m_owner->m_isWaitingForPortSelection && !hasVisiblePortListWidget)
        {
            QPointF targetPos = m_owner->m_isLineFrozen ?
                                m_owner->m_frozenTargetPortPos : event->scenePos();
            m_owner->m_activeTempLink->updateGeometry(targetPos);

            // Обновляем узлы для отображения подсветки входных портов
            for(UModernDiagramNodeItem* node : m_owner->m_nodes)
            {
                node->update();
            }

            event->accept();
            return;
        }
        else
        {
            // Если окно выбора портов открыто, передаем событие дальше в иерархию
            // и не обновляем временную линию
            event->ignore();
            QGraphicsScene::mouseMoveEvent(event);
            return;
        }
    }

    QGraphicsScene::mouseMoveEvent(event);

    // Set canvas tooltip when hovering over empty area (not over node or link)
    if(m_owner && m_owner->m_mainView)
    {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        // Only set canvas tooltip if not hovering over a node or link
        if(!item || (!dynamic_cast<UModernDiagramNodeItem*>(item) &&
                     !dynamic_cast<UModernDiagramLinkItem*>(item)))
        {
            QString tooltip = UModernDiagramTooltipGenerator::generateCanvasTooltip();
            m_owner->m_mainView->setToolTip(tooltip);
        }
        else
        {
            // Clear tooltip when hovering over items (they have their own tooltips)
            m_owner->m_mainView->setToolTip(QString());
        }
    }
}

void UModernDiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Устанавливаем флаг обработки событий мыши для пропуска pollHover
    m_isProcessingMouseEvent = true;

    // Сначала обрабатываем drag & drop связь (m_tempLink), если она активна
    if(m_owner->m_tempLink && event->button() == Qt::LeftButton)
    {
        QPointF portPos;
        UModernDiagramNodeItem* targetNode = nullptr;
        const UModernDiagramWidget::Port* targetPort = m_owner->pickPortDetailed(event->scenePos(), true, targetNode, portPos);
        if(targetNode && targetPort && m_owner->m_dragSourceNode && m_owner->m_dragSourcePort &&
           targetNode != m_owner->m_dragSourceNode)
        {
            // Формируем полные имена компонентов
            QString srcName = m_owner->m_dragSourceNode->nodeName;
            QString dstName = targetNode->nodeName;
            QString fullSrc = m_owner->m_componentName.isEmpty() ? srcName : m_owner->m_componentName + "." + srcName;
            QString fullDst = m_owner->m_componentName.isEmpty() ? dstName : m_owner->m_componentName + "." + dstName;

            // Формируем пути свойств
            QString srcProp = m_owner->m_dragSourcePort->fullPath.isEmpty() ?
                             m_owner->m_dragSourcePort->name : m_owner->m_dragSourcePort->fullPath;
            QString dstProp = targetPort->fullPath.isEmpty() ?
                             targetPort->name : targetPort->fullPath;

            // Учитываем вложенные компоненты
            if(!m_owner->m_dragSourcePort->componentName.isEmpty() &&
               m_owner->m_dragSourcePort->componentName != srcName)
            {
                srcProp = m_owner->m_dragSourcePort->componentName + "." + srcProp;
            }
            if(!targetPort->componentName.isEmpty() &&
               targetPort->componentName != dstName)
            {
                dstProp = targetPort->componentName + "." + dstProp;
            }

            // Применяем связь к ядру
            int result = Model_CreateLinkByName(
                fullSrc.toStdString().c_str(),
                srcProp.toStdString().c_str(),
                fullDst.toStdString().c_str(),
                dstProp.toStdString().c_str()
            );

            // Удаляем временную линию
            m_owner->m_scene->removeItem(m_owner->m_tempLink);
            delete m_owner->m_tempLink;
            m_owner->m_tempLink = nullptr;

            // Сбрасываем состояние
            m_owner->m_dragSourceNode = nullptr;
            m_owner->m_dragSourcePort = nullptr;

            // Обновляем схему (перестраиваем связи)
            if(result == 0) // Только при успешном создании связи
            {
                m_owner->Reload();
            }
            else
            {
            }
            emit m_owner->updateComponentsList();

            // Сбрасываем флаг после обработки события
            m_isProcessingMouseEvent = false;
            event->accept();
            return;
        }
        else
        {
            // Отменяем drag & drop, если не попали на входной порт
            m_owner->m_scene->removeItem(m_owner->m_tempLink);
            delete m_owner->m_tempLink;
            m_owner->m_tempLink = nullptr;
            m_owner->m_dragSourceNode = nullptr;
            m_owner->m_dragSourcePort = nullptr;
            // Сбрасываем флаг после обработки события
            m_isProcessingMouseEvent = false;
            event->accept();
            return;
        }
    }

    // Обработка завершения связи из дерева портов
    // ВАЖНО: Проверяем только если НЕТ активной drag & drop связи
    if(m_owner->m_activeTempLink && !m_owner->m_tempLink && event->button() == Qt::LeftButton)
    {
        // Сначала проверяем, не попал ли клик на окно выбора входов
        // Если да, не завершаем соединение - пользователь должен выбрать порт из списка
        bool clickedOnPortListWidget = false;
        for(UModernDiagramNodeItem* node : m_owner->m_nodes)
        {
            if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
            {
                QRectF widgetRect = node->m_portListWidgetProxy->mapToScene(
                    node->m_portListWidgetProxy->boundingRect()).boundingRect();
                if(widgetRect.contains(event->scenePos()))
                {
                    clickedOnPortListWidget = true;
                    break;
                }
            }
        }

        // Если клик попал на окно выбора, обрабатываем выбор порта напрямую
        if(clickedOnPortListWidget)
        {
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO,
                "mouseReleaseEvent: Click detected on port list widget", 0);

            // Находим узел с открытым окном выбора портов
            for(UModernDiagramNodeItem* node : m_owner->m_nodes)
            {
                if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
                {
                    QRectF widgetRect = node->m_portListWidgetProxy->mapToScene(
                        node->m_portListWidgetProxy->boundingRect()).boundingRect();
                    if(widgetRect.contains(event->scenePos()))
                    {
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO,
                            "mouseReleaseEvent: Found visible port list widget", 0);

                        // Преобразуем позицию клика в локальные координаты виджета
                        QPointF widgetLocalPos = node->m_portListWidgetProxy->mapFromScene(event->scenePos());
                        QPoint widgetPoint = widgetLocalPos.toPoint();

                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO,
                            "mouseReleaseEvent: Converting coordinates for itemAt", 0);

                        // Попробуем несколько способов найти элемент:
                        // 1. Через itemAt с преобразованными координатами
                        QTreeWidgetItem* itemUnderCursor = node->m_portListWidget->itemAt(widgetPoint);

                        // 2. Если не нашли, попробуем через mapFromGlobal
                        if(!itemUnderCursor)
                        {
                            QPointF screenPos = event->screenPos();
                            QPoint globalPos = screenPos.toPoint();
                            QPoint widgetGlobalPoint = node->m_portListWidget->mapFromGlobal(globalPos);
                            itemUnderCursor = node->m_portListWidget->itemAt(widgetGlobalPoint);
                        }

                        // 3. Если все еще не нашли, используем currentItem
                        if(!itemUnderCursor)
                        {
                            itemUnderCursor = node->m_portListWidget->currentItem();
                        }

                        if(itemUnderCursor)
                        {
                            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO,
                                "mouseReleaseEvent: itemUnderCursor found", 0);

                            if(itemUnderCursor->parent() != nullptr)
                            {
                                // Это элемент порта (не категория) - завершаем соединение
                                QVariant data = itemUnderCursor->data(0, Qt::UserRole);
                                if(data.isValid())
                                {
                                    QMap<QString, QVariant> portData = data.value<QMap<QString, QVariant>>();
                                    bool isInput = portData["isInput"].toBool();
                                    QString portName = portData["name"].toString();

                                    QString logMsg = QString("mouseReleaseEvent: Clicked on %1 port '%2' in list, completing connection")
                                        .arg(isInput ? "input" : "output").arg(portName);
                                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

                                    node->onPortItemActivated(itemUnderCursor, 0);
                                    event->accept();
                                    return;
                                }
                                else
                                {
                                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING,
                                        "mouseReleaseEvent: Item data is invalid", 0);
                                }
                            }
                        }
                        else
                        {
                            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING,
                                "mouseReleaseEvent: No item found at cursor position", 0);
                        }

                        // Если не удалось найти элемент напрямую, передаем событие виджету
                        // чтобы он мог обработать клик через свой механизм
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO,
                            "mouseReleaseEvent: Forwarding event to QTreeWidget", 0);
                    }
                }
            }

            // Если не попали на элемент порта, передаем событие дальше
            QGraphicsScene::mouseReleaseEvent(event);
            return;
        }

        QPointF portPos;
        UModernDiagramNodeItem* targetNode = nullptr;
        const UModernDiagramWidget::Port* targetPort = m_owner->pickPortDetailed(event->scenePos(), true, targetNode, portPos);

        // Проверяем, что все необходимые указатели валидны
        // И что это НЕ категоризированный порт (для категоризированных портов нужно выбрать из списка)
        if(targetNode && targetPort && m_owner->m_activeSourceNode && m_owner->m_activeSourcePort &&
           targetNode != m_owner->m_activeSourceNode)
        {
            // Если это категоризированный порт (Own/Child/Alias), не завершаем соединение напрямую
            // Пользователь должен выбрать конкретный порт из открытого окна выбора
            if(targetPort->fullPath.isEmpty() &&
               (targetPort->category == UModernDiagramWidget::PortCategory::Own ||
                targetPort->category == UModernDiagramWidget::PortCategory::Child ||
                targetPort->category == UModernDiagramWidget::PortCategory::Alias))
            {
                // Это категоризированный порт - не завершаем соединение, оставляем окно выбора открытым
                event->accept();
                return;
            }

            // Это конкретный входной порт (не категоризированный) - завершаем соединение
            // Дополнительная проверка валидности указателя на порт
            // Сохраняем значения в локальные переменные для безопасности
            const UModernDiagramWidget::Port* activeSourcePort = m_owner->m_activeSourcePort;
            if(!activeSourcePort)
            {
                // Если указатель стал невалидным, отменяем операцию
                event->accept();
                return;
            }

            // Дополнительная проверка: убеждаемся, что узел все еще валиден
            UModernDiagramNodeItem* activeSourceNode = m_owner->m_activeSourceNode;
            if(!activeSourceNode)
            {
                event->accept();
                return;
            }

            // Используем сохраненные копии строковых полей порта вместо обращения к указателю
            // Это безопаснее, так как указатель может стать невалидным
            QString srcPortFullPath = m_owner->m_activeSourcePortFullPath;
            QString srcPortName = m_owner->m_activeSourcePortName;
            QString srcPortComponentName = m_owner->m_activeSourcePortComponentName;

            // Копируем только строковые поля целевого порта
            QString dstPortFullPath;
            QString dstPortName;
            QString dstPortComponentName;

            try
            {
                dstPortFullPath = targetPort->fullPath;
                dstPortName = targetPort->name;
                dstPortComponentName = targetPort->componentName;
            }
            catch(...)
            {
                // Если произошла ошибка при копировании, отменяем операцию
                event->accept();
                return;
            }

            // Формируем полные имена компонентов
            QString srcName = activeSourceNode->nodeName;
            QString dstName = targetNode->nodeName;
            QString fullSrc = m_owner->m_componentName.isEmpty() ? srcName : m_owner->m_componentName + "." + srcName;
            QString fullDst = m_owner->m_componentName.isEmpty() ? dstName : m_owner->m_componentName + "." + dstName;

            // Формируем пути свойств с использованием скопированных значений
            QString srcProp;
            if(srcPortFullPath.isEmpty())
            {
                srcProp = srcPortName;
            }
            else
            {
                srcProp = srcPortFullPath;
            }

            QString dstProp;
            if(dstPortFullPath.isEmpty())
            {
                dstProp = dstPortName;
            }
            else
            {
                dstProp = dstPortFullPath;
            }

            // Учитываем вложенные компоненты
            // Проверяем, нужно ли добавлять componentName к srcProp
            if(!srcPortComponentName.isEmpty() &&
               srcPortComponentName != srcName)
            {
                // Проверяем, начинается ли srcProp уже с componentName, чтобы избежать дублирования
                if(!srcProp.startsWith(srcPortComponentName + ".") && !srcProp.startsWith(srcPortComponentName))
                {
                    srcProp = srcPortComponentName + "." + srcProp;
                }
            }
            // Проверяем, нужно ли добавлять componentName к dstProp
            if(!dstPortComponentName.isEmpty() &&
               dstPortComponentName != dstName)
            {
                // Проверяем, начинается ли dstProp уже с componentName, чтобы избежать дублирования
                if(!dstProp.startsWith(dstPortComponentName + ".") && !dstProp.startsWith(dstPortComponentName))
                {
                    dstProp = dstPortComponentName + "." + dstProp;
                }
            }

            // Применяем связь к ядру
            int result = Model_CreateLinkByName(
                fullSrc.toStdString().c_str(),
                srcProp.toStdString().c_str(),
                fullDst.toStdString().c_str(),
                dstProp.toStdString().c_str()
            );

            // Удаляем временную линию
            m_owner->m_scene->removeItem(m_owner->m_activeTempLink);
            delete m_owner->m_activeTempLink;
            m_owner->m_activeTempLink = nullptr;

            // Сбрасываем состояние
            m_owner->m_activeSourceNode = nullptr;
            m_owner->m_activeSourcePort = nullptr;
            m_owner->m_isLineFrozen = false;
            m_owner->m_isWaitingForPortSelection = false;

            // Сбрасываем курсор
            if(m_owner->m_mainView)
            {
                m_owner->m_mainView->unsetCursor();
            }

            // Закрываем все открытые деревья портов
            for(UModernDiagramNodeItem* node : m_owner->m_nodes)
            {
                node->hidePortListWidget();
            }

            // Обновляем схему (перестраиваем связи)
            // Сначала отправляем сигнал, затем перестраиваем связи (чтобы избежать обращения к удаленным объектам)
            emit m_owner->updateComponentsList();
            if(result == 0) // Только при успешном создании связи
            {
                m_owner->rebuildLinks();
            }
            else
            {
            }

            event->accept();
            return;
        }
        else
        {
            // НЕ отменяем связь при клике на фон - пользователь может продолжить перемещение мыши
            // Связь будет отменена только при правом клике или нажатии Esc
            event->accept();
            return;
        }
    }

    if(m_owner->m_tempLink)
    {
        QPointF portPos;
        UModernDiagramNodeItem* targetNode = nullptr;
        const UModernDiagramWidget::Port* targetPort = m_owner->pickPortDetailed(event->scenePos(), true, targetNode, portPos);
        if(targetNode && targetPort && m_owner->m_dragSourceNode && m_owner->m_dragSourcePort &&
           targetNode != m_owner->m_dragSourceNode)
        {
            m_owner->m_scene->removeItem(m_owner->m_tempLink);
            delete m_owner->m_tempLink;
                    auto* finalLink = new UModernDiagramLinkItem(m_owner->m_dragSourceNode, targetNode, true, true);
            m_owner->m_scene->addItem(finalLink);
            m_owner->m_links.append(finalLink);
            // Обновляем кэш связей для узлов
            m_owner->m_dragSourceNode->m_connectedLinks.append(finalLink);
            targetNode->m_connectedLinks.append(finalLink);
            // Инвалидируем кэш paint() при создании новой связи
            m_owner->m_dragSourceNode->m_cacheValid = false;
            targetNode->m_cacheValid = false;
            m_owner->m_tempLink = nullptr;

            // Создание связи в ядре с использованием конкретных портов
            QString srcName = m_owner->m_dragSourceNode->nodeName;
            QString dstName = targetNode->nodeName;
            QString fullSrc = m_owner->m_componentName.isEmpty() ? srcName : m_owner->m_componentName + "." + srcName;
            QString fullDst = m_owner->m_componentName.isEmpty() ? dstName : m_owner->m_componentName + "." + dstName;

            // Проверяем, являются ли порты алиасами (по пометке [Alias] в displayName)
            bool srcIsAlias = m_owner->m_dragSourcePort->displayName.contains("[Alias]");
            bool dstIsAlias = targetPort->displayName.contains("[Alias]");

            int result = RDK_SUCCESS;

            // Если оба порта - алиасы, пытаемся использовать CreateLinkByAlias
            // Примечание: CreateLinkByAlias работает только если оба алиаса находятся в одной сети
            // Если они в разных сетях, используем обычный метод создания связи
            if(srcIsAlias && dstIsAlias && fullSrc == fullDst)
            {
                try
                {
                    RDK::UEPtr<RDK::UContainer> model = RDK::GetModel();
                    if(model)
                    {
                        RDK::UEPtr<RDK::UNet> srcNet = RDK::dynamic_pointer_cast<RDK::UNet>(
                            model->GetComponentL(fullSrc.toStdString(), true));
                        if(srcNet && srcNet->CheckPropertyAlias(m_owner->m_dragSourcePort->name.toStdString()) &&
                           srcNet->CheckPropertyAlias(targetPort->name.toStdString()))
                        {
                            // Используем CreateLinkByAlias (оба алиаса в одной сети)
                            bool linkCreated = srcNet->CreateLinkByAlias(
                                m_owner->m_dragSourcePort->name.toStdString(),
                                targetPort->name.toStdString());
                            if(linkCreated)
                            {
                                result = RDK_SUCCESS;
                            }
                            else
                            {
                                // ошибка при создании связи по алиасам
                                result = -1;
                            }
                        }
                        else
                        {
                            // алиасы не найдены или находятся в другой сети
                            result = -1;
                        }
                    }
                    else
                    {
                        // не удалось получить модель
                        result = -1;
                    }
                }
                catch(...)
                {
                    // любое исключение трактуем как ошибку создания связи по алиасам
                    result = -1;
                }
            }
            else
            {
                // Не используем алиасы, переходим к обычному методу
                result = -1;
            }

            // Если не удалось использовать алиасы, используем обычный метод
            if(result != RDK_SUCCESS)
            {
                // Используем полные пути для вложенных портов
                QString srcProp = m_owner->m_dragSourcePort->fullPath.isEmpty() ?
                                 m_owner->m_dragSourcePort->name : m_owner->m_dragSourcePort->fullPath;
                QString dstProp = targetPort->fullPath.isEmpty() ?
                                 targetPort->name : targetPort->fullPath;

                // Если порт принадлежит вложенному компоненту, добавляем путь компонента
                // Проверяем, нужно ли добавлять componentName к srcProp
                if(!m_owner->m_dragSourcePort->componentName.isEmpty() &&
                   m_owner->m_dragSourcePort->componentName != srcName)
                {
                    // Проверяем, начинается ли srcProp уже с componentName, чтобы избежать дублирования
                    if(!srcProp.startsWith(m_owner->m_dragSourcePort->componentName + ".") &&
                       !srcProp.startsWith(m_owner->m_dragSourcePort->componentName))
                    {
                        srcProp = m_owner->m_dragSourcePort->componentName + "." + srcProp;
                    }
                }
                // Проверяем, нужно ли добавлять componentName к dstProp
                if(!targetPort->componentName.isEmpty() &&
                   targetPort->componentName != dstName)
                {
                    // Проверяем, начинается ли dstProp уже с componentName, чтобы избежать дублирования
                    if(!dstProp.startsWith(targetPort->componentName + ".") &&
                       !dstProp.startsWith(targetPort->componentName))
                    {
                        dstProp = targetPort->componentName + "." + dstProp;
                    }
                }

                result = Model_CreateLinkByName(fullSrc.toStdString().c_str(), srcProp.toStdString().c_str(),
                                               fullDst.toStdString().c_str(), dstProp.toStdString().c_str());
            }
            if(result != RDK_SUCCESS)
            {
                // На этом этапе мы можем не знать конкретные src/dst свойства, поэтому выводим только имена компонентов
                QMessageBox::warning(m_owner, "Error",
                    QString("Failed to create link between %1 and %2")
                    .arg(fullSrc, fullDst));
            }
            else
            {
                m_owner->Reload();
                emit m_owner->updateComponentsList();
            }
        }
        else
        {
            m_owner->m_scene->removeItem(m_owner->m_tempLink);
            delete m_owner->m_tempLink;
            m_owner->m_tempLink = nullptr;
        }
        m_owner->m_dragSourceNode = nullptr;
        m_owner->m_dragSourcePort = nullptr;
        event->accept();
        return;
    }

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Если группа была перемещена, сохраняем выделение перед вызовом
    // базового класса, чтобы восстановить его после, так как базовый класс может сбросить выделение
    bool wasGroupMoving = m_isGroupMoving;
    QList<UModernDiagramNodeItem*> savedSelectedNodes;
    if(wasGroupMoving)
    {
        // Сохраняем все выделенные узлы перед вызовом базового класса
        QList<QGraphicsItem*> selectedItems = m_owner->m_scene->selectedItems();
        for(QGraphicsItem* item : selectedItems)
        {
            auto* node = dynamic_cast<UModernDiagramNodeItem*>(item);
            if(node && node->isSelected())
            {
                savedSelectedNodes.append(node);
            }
        }
    }

    // Вызываем базовый класс для обработки событий
    // Базовый класс QGraphicsView::mouseReleaseEvent уже был вызван в UModernDiagramView,
    // и он передал событие в сцену, поэтому здесь мы обрабатываем событие на уровне сцены
    QGraphicsScene::mouseReleaseEvent(event);

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Если группа была перемещена, восстанавливаем выделение,
    // так как базовый класс мог его сбросить
    // Используем отложенный вызов, чтобы восстановление произошло после всех обработчиков событий
    if(wasGroupMoving && !savedSelectedNodes.isEmpty())
    {
        // DEBUG: Commented out to reduce log flood
        // QString logMsg = QString("[SELECTION_DEBUG] UModernDiagramScene::mouseReleaseEvent: restoring group selection, nodes count=%1")
        //     .arg(savedSelectedNodes.size());
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

        // Сохраняем список узлов для отложенного восстановления
        QList<UModernDiagramNodeItem*> nodesToRestore = savedSelectedNodes;

        // Восстанавливаем выделение немедленно
        // Используем флаг m_isProgrammaticSelection, чтобы предотвратить эмиссию componentSelected
        m_owner->m_isProgrammaticSelection = true;
        m_owner->m_isBatchSelecting = true;
        m_owner->m_scene->blockSignals(true);

        for(UModernDiagramNodeItem* node : nodesToRestore)
        {
            if(node)
            {
                node->setSelected(true);
            }
        }

        m_owner->m_isBatchSelecting = false;
        m_owner->m_scene->blockSignals(false);

        // Отложенное восстановление выделения, чтобы оно произошло после всех обработчиков событий
        QTimer::singleShot(0, [this, nodesToRestore]() {
            // DEBUG: Commented out to reduce log flood
            // QString logMsg2 = QString("[SELECTION_DEBUG] UModernDiagramScene::mouseReleaseEvent: delayed restore, nodes count=%1, setting m_isProgrammaticSelection=true")
            //     .arg(nodesToRestore.size());
            // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg2.toStdString().c_str(), 0);

            // Восстанавливаем выделение для всех сохраненных узлов
            m_owner->m_isProgrammaticSelection = true;
            m_owner->m_isBatchSelecting = true;
            m_owner->m_scene->blockSignals(true);

            for(UModernDiagramNodeItem* node : nodesToRestore)
            {
                if(node)
                {
                    node->setSelected(true);
                }
            }

            m_owner->m_isBatchSelecting = false;
            m_owner->m_scene->blockSignals(false);

            // Отложенный сброс флага, чтобы он оставался установленным во время обработки всех событий
            QTimer::singleShot(0, [this]() {
                // DEBUG: Commented out to reduce log flood
                // QString logMsg3 = QString("[SELECTION_DEBUG] UModernDiagramScene::mouseReleaseEvent: resetting m_isProgrammaticSelection=false");
                // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg3.toStdString().c_str(), 0);
                m_owner->m_isProgrammaticSelection = false;
            });
        });
    }

    // После обработки базовым классом проверяем выделение и устанавливаем флаги
    if(event->button() == Qt::LeftButton && !(event->modifiers() & Qt::ControlModifier))
    {
        // Подсчитываем количество выделенных NodeItem
        QList<QGraphicsItem*> selectedItems = m_owner->m_scene->selectedItems();
        int selectedNodeCount = 0;
        QStringList selectedNodeNames;
        for(QGraphicsItem* item : selectedItems)
        {
            auto* node = dynamic_cast<UModernDiagramNodeItem*>(item);
            if(node && node->isSelected())
            {
                selectedNodeCount++;
                selectedNodeNames << node->nodeName;
            }
        }

        // DEBUG: Commented out to reduce log flood
        // QString logMsg4 = QString("[SELECTION_DEBUG] UModernDiagramScene::mouseReleaseEvent: after base class, selectedNodeCount=%1, nodeNames=[%2]")
        //     .arg(selectedNodeCount)
        //     .arg(selectedNodeNames.join(", "));
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg4.toStdString().c_str(), 0);

        // Устанавливаем флаг группы, если выделено больше одного объекта
        m_isGroupSelected = (selectedNodeCount > 1);

        // Сбрасываем флаг RubberBandDrag
        m_isRubberBandActive = false;
    }

    // Сбрасываем флаг после обработки события
    m_isProcessingMouseEvent = false;

    // Сбрасываем флаг перемещения группы при отпускании кнопки
    // НО НЕ сбрасываем выделение - оно должно остаться до клика на фоне или Esc
    if(m_isGroupMoving)
    {
        m_isGroupMoving = false;
        m_savedSelection.clear();
    }

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Все обновления параметров канвы выполняются ТОЛЬКО в mouseReleaseEvent
    // и ТОЛЬКО после временного сброса флага m_isComponentMoving
    // Это гарантирует, что во время движения (от mousePressEvent до mouseReleaseEvent) ничего не меняется

    // Временно сбрасываем флаг для выполнения обновлений
    bool wasComponentMoving = m_owner->m_isComponentMoving;
    m_owner->m_isComponentMoving = false;

    // Обновляем offset только если компоненты действительно вышли за видимую область
    if(!m_owner->m_componentsWithNegativePos.isEmpty())
    {
        // Дополнительная проверка: убеждаемся, что компоненты действительно вне видимой области
        // (на случай, если они вернулись в видимую область до mouseReleaseEvent)
        QSet<UModernDiagramNodeItem*> componentsOutsideVisibleArea;
        for(UModernDiagramNodeItem* node : m_owner->m_componentsWithNegativePos)
        {
            if(node && m_owner->isComponentOutsideVisibleArea(node))
            {
                componentsOutsideVisibleArea.insert(node);
            }
        }

        // Обновляем offset только если есть компоненты, которые действительно вне видимой области
        if(!componentsOutsideVisibleArea.isEmpty())
        {
            // Находим минимальную позицию среди всех компонентов вне видимой области
            QPointF minNormalizedPos(0, 0);
            bool first = true;

            for(UModernDiagramNodeItem* node : componentsOutsideVisibleArea)
            {
                if(node)
                {
                    // CRITICAL FIX: Use pos() instead of scenePos() to get normalized coordinates
                    // pos() returns normalized coordinates (relative to m_normalizationOffset)
                    // scenePos() returns absolute scene coordinates (pos() + m_normalizationOffset)
                    QPointF normalizedPos = node->pos();
                    if(first)
                    {
                        minNormalizedPos = normalizedPos;
                        first = false;
                    }
                    else
                    {
                        if(normalizedPos.x() < minNormalizedPos.x())
                            minNormalizedPos.setX(normalizedPos.x());
                        if(normalizedPos.y() < minNormalizedPos.y())
                            minNormalizedPos.setY(normalizedPos.y());
                    }
                }
            }

            // Если есть компоненты вне видимой области, обновляем offset один раз
            if(!first && (minNormalizedPos.x() < 0 || minNormalizedPos.y() < 0))
            {
                QString logMsg = QString("[UModernDiagramScene::mouseReleaseEvent] Updating offset after movement completion: componentsOutsideVisibleArea=%1, minNormalizedPos=(%2, %3)")
                    .arg(componentsOutsideVisibleArea.size())
                    .arg(minNormalizedPos.x()).arg(minNormalizedPos.y());
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

                // CRITICAL FIX: Save the set before clearing it, so updateNormalizationOffsetForMovement
                // knows which components to adjust
                m_owner->m_coordinateManager->updateNormalizationOffsetForMovement(minNormalizedPos, componentsOutsideVisibleArea);
            }
        }

        // Очищаем set после обновления offset (или если обновление не потребовалось)
        m_owner->m_componentsWithNegativePos.clear();

        // Очищаем сохраненные исходные абсолютные координаты
        m_owner->m_originalAbsolutePositions.clear();
    }

    // Обновляем sceneRect после завершения движения
    if(wasComponentMoving)
    {
        m_owner->updateSceneRect();
    }
}

void UModernDiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        // Сначала проверяем, попал ли двойной клик на окно выбора портов
        for(UModernDiagramNodeItem* node : m_owner->m_nodes)
        {
            if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
            {
                QRectF widgetRect = node->m_portListWidgetProxy->mapToScene(
                    node->m_portListWidgetProxy->boundingRect()).boundingRect();
                if(widgetRect.contains(event->scenePos()))
                {
                    // Преобразуем позицию клика в локальные координаты виджета
                    QPointF widgetLocalPos = node->m_portListWidgetProxy->mapFromScene(event->scenePos());
                    QPoint widgetPoint = widgetLocalPos.toPoint();

                    // Находим элемент под курсором
                    QTreeWidgetItem* itemUnderCursor = node->m_portListWidget->itemAt(widgetPoint);
                    if(itemUnderCursor && itemUnderCursor->parent() != nullptr)
                    {
                        // Это элемент порта (не категория) - завершаем соединение
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO,
                            "mouseDoubleClickEvent: Double-clicked on port item in list, completing connection", 0);
                        node->onPortItemActivated(itemUnderCursor, 0);
                        event->accept();
                        return;
                    }
                    else
                    {
                    }
                }
            }
        }

        // Проверяем, попали ли мы в порт
        QPointF portPos;
        UModernDiagramNodeItem* portNode = nullptr;
        const UModernDiagramWidget::Port* port = m_owner->pickPortDetailed(event->scenePos(), false, portNode, portPos);

        if(portNode && port)
        {
            // Если это выходной порт - начинаем создание связи
            if(!port->isInput)
            {
                // Сохраняем состояние активной связи
                m_owner->m_activeSourceNode = portNode;
                m_owner->m_activeSourcePort = port;
                // Сохраняем копии строковых полей для безопасного использования
                try
                {
                    m_owner->m_activeSourcePortName = port->name;
                    m_owner->m_activeSourcePortFullPath = port->fullPath;
                    m_owner->m_activeSourcePortComponentName = port->componentName;
                }
                catch(...)
                {
                    // Если произошла ошибка при копировании, отменяем операцию
                    m_owner->m_activeSourceNode = nullptr;
                    m_owner->m_activeSourcePort = nullptr;
                    event->accept();
                    return;
                }
                m_owner->m_activeSourcePortPos = portPos;

                // Создаем временную линию с сохраненной позицией порта
                m_owner->m_activeTempLink = new UModernDiagramLinkItem(portNode, portPos, portPos);
                m_owner->m_scene->addItem(m_owner->m_activeTempLink);

                // Сбрасываем состояние заморозки при создании новой связи
                m_owner->m_isLineFrozen = false;

                // Закрываем все открытые деревья портов
                for(UModernDiagramNodeItem* node : m_owner->m_nodes)
                {
                    node->hidePortListWidget();
                }

                event->accept();
                return;
            }
            // Если это входной порт и есть активная связь - завершаем её
            else if(port->isInput && m_owner->m_activeSourceNode && m_owner->m_activeSourcePort)
            {
                // Формируем полные имена компонентов
                QString srcName = m_owner->m_activeSourceNode->nodeName;
                QString dstName = portNode->nodeName;
                QString fullSrc = m_owner->m_componentName.isEmpty() ? srcName : m_owner->m_componentName + "." + srcName;
                QString fullDst = m_owner->m_componentName.isEmpty() ? dstName : m_owner->m_componentName + "." + dstName;

                // Формируем пути свойств, используя сохраненные копии вместо указателя
                QString srcProp;
                if(m_owner->m_activeSourcePortFullPath.isEmpty())
                {
                    srcProp = m_owner->m_activeSourcePortName;
                }
                else
                {
                    srcProp = m_owner->m_activeSourcePortFullPath;
                }

                QString dstProp;
                QString dstPortFullPath;
                QString dstPortName;
                QString dstPortComponentName;
                try
                {
                    dstPortFullPath = port->fullPath;
                    dstPortName = port->name;
                    dstPortComponentName = port->componentName;
                }
                catch(...)
                {
                    event->accept();
                    return;
                }

                if(dstPortFullPath.isEmpty())
                {
                    dstProp = dstPortName;
                }
                else
                {
                    dstProp = dstPortFullPath;
                }

                // Учитываем вложенные компоненты
                // Проверяем, нужно ли добавлять componentName к srcProp
                if(!m_owner->m_activeSourcePortComponentName.isEmpty() &&
                   m_owner->m_activeSourcePortComponentName != srcName)
                {
                    // Проверяем, начинается ли srcProp уже с componentName, чтобы избежать дублирования
                    if(!srcProp.startsWith(m_owner->m_activeSourcePortComponentName + ".") &&
                       !srcProp.startsWith(m_owner->m_activeSourcePortComponentName))
                    {
                        srcProp = m_owner->m_activeSourcePortComponentName + "." + srcProp;
                    }
                }
                // Проверяем, нужно ли добавлять componentName к dstProp
                if(!dstPortComponentName.isEmpty() && dstPortComponentName != dstName)
                {
                    // Проверяем, начинается ли dstProp уже с componentName, чтобы избежать дублирования
                    if(!dstProp.startsWith(dstPortComponentName + ".") &&
                       !dstProp.startsWith(dstPortComponentName))
                    {
                        dstProp = dstPortComponentName + "." + dstProp;
                    }
                }

                // Применяем связь к ядру
                int result = Model_CreateLinkByName(
                    fullSrc.toStdString().c_str(),
                    srcProp.toStdString().c_str(),
                    fullDst.toStdString().c_str(),
                    dstProp.toStdString().c_str()
                );

                // Удаляем временную линию
                if(m_owner->m_activeTempLink)
                {
                    m_owner->m_scene->removeItem(m_owner->m_activeTempLink);
                    delete m_owner->m_activeTempLink;
                    m_owner->m_activeTempLink = nullptr;
                }

                // Сбрасываем состояние
                m_owner->m_activeSourceNode = nullptr;
                m_owner->m_activeSourcePort = nullptr;
                m_owner->m_isLineFrozen = false;

                // Сбрасываем курсор
                if(m_owner->m_mainView)
                {
                    m_owner->m_mainView->unsetCursor();
                }

                // Закрываем все открытые деревья портов
                for(UModernDiagramNodeItem* node : m_owner->m_nodes)
                {
                    node->hidePortListWidget();
                }

            // Обновляем схему (перестраиваем связи)
            // Сначала отправляем сигнал, затем перестраиваем связи (чтобы избежать обращения к удаленным объектам)
            emit m_owner->updateComponentsList();
            if(result == 0) // Только при успешном создании связи
            {
                m_owner->rebuildLinks();
            }
            else
            {
            }

            event->accept();
                return;
            }
        }

        // Проверяем, попали ли мы в узел (но не в порт)
        auto* node = m_owner->pickNode(event->scenePos());
        if(node)
        {
            // Сохраняем состояние текущего компонента перед переходом
            if(m_owner->m_viewportManager)
                m_owner->m_viewportManager->saveCurrentViewState(m_owner->m_componentName);

            // Вход внутрь компонента: добавляем имя узла к пути
            QString newPath;
            if(m_owner->m_componentName.isEmpty())
                newPath = node->nodeName;
            else
                newPath = m_owner->m_componentName + "." + node->nodeName;
            m_owner->SetComponentName(newPath);
            m_owner->Reload();
            emit m_owner->componentDoubleClicked(newPath);
            event->accept();
            return;
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        // Правый двойной клик по холсту: поднимаемся на уровень выше
        if(!m_owner->m_componentName.isEmpty())
        {
            // Сохраняем состояние текущего компонента перед переходом
            if(m_owner->m_viewportManager)
                m_owner->m_viewportManager->saveCurrentViewState(m_owner->m_componentName);

            QStringList pathParts = m_owner->m_componentName.split(".");
            if(pathParts.size() > 0)
            {
                pathParts.removeLast();
            QString newPath = pathParts.join(".");
            m_owner->SetComponentName(newPath);
            m_owner->Reload();
            emit m_owner->componentStapBack();
            event->accept();
            return;
            }
        }
    }
    QGraphicsScene::mouseDoubleClickEvent(event);
}

