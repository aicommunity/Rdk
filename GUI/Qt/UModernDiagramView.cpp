#include "UModernDiagramView.h"
#include "UModernDiagramWidget.h"
#include "UModernDiagramScene.h"
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramCoordinateManager.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QScrollBar>
#include <QApplication>
#include <QMimeData>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QDataStream>
#include <QFileInfo>
#include <QTimer>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QEvent>
#include <QPointF>
#include <QRectF>
#include <QPoint>
#include <QRect>
#include <QLineF>
#include <cmath>
#include "../../Deploy/Include/rdk_init.h"
#include "../../Deploy/Include/rdk_cpp_init.h"
#include "UEngineSelectionSync.h"
#include "../Core/Engine/UEngine.h"
#include "../Core/Engine/UNet.h"
#include "../Core/Application/UIVisualController.h"

UModernDiagramView::UModernDiagramView(UModernDiagramWidget* owner, QGraphicsScene* scene)
    : QGraphicsView(scene), m_owner(owner)
{
    setAcceptDrops(true);
    // Оптимизация: настраиваем режим обновления viewport для лучшей производительности
    // SmartViewportUpdate обновляет только измененные области
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    // Включаем кэширование фона для ускорения прокрутки
    setCacheMode(QGraphicsView::CacheBackground);
}

// КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Переопределяем ensureVisible для блокировки автоматической прокрутки при выделении
// Примечание: ensureVisible не является виртуальным в QGraphicsView, но мы можем скрыть его
void UModernDiagramView::ensureVisible(const QRectF& rect, int xmargin, int ymargin)
{
    // Блокируем автоматическую прокрутку при выделении компонентов
    // Это предотвращает движение скроллбаров при выделении
    if(m_owner)
    {
        // Блокируем прокрутку во время движения компонента
        if(m_owner->m_isComponentMoving)
        {
            return; // Не прокручиваем во время движения компонента
        }

        // Блокируем прокрутку при выделении компонентов (проверяем, есть ли выделенные NodeItem)
        QList<QGraphicsItem*> selectedItems = scene()->selectedItems();
        for(QGraphicsItem* item : selectedItems)
        {
            // Если есть выделенные NodeItem, блокируем автоматическую прокрутку
            if(dynamic_cast<UModernDiagramNodeItem*>(item))
            {
                return; // Не прокручиваем при выделении компонентов
            }
        }
    }
    QGraphicsView::ensureVisible(rect, xmargin, ymargin);
}

// КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Переопределяем centerOn для блокировки автоматического центрирования при выделении
// Примечание: centerOn не является виртуальным в QGraphicsView, но мы можем скрыть его
void UModernDiagramView::centerOn(const QPointF& pos)
{
    // Блокируем автоматическое центрирование при выделении компонентов
    if(m_owner)
    {
        // Блокируем центрирование во время движения компонента
        if(m_owner->m_isComponentMoving)
        {
            return; // Не центрируем во время движения компонента
        }

        // Блокируем центрирование при выделении компонентов
        QList<QGraphicsItem*> selectedItems = scene()->selectedItems();
        for(QGraphicsItem* item : selectedItems)
        {
            // Если есть выделенные NodeItem, блокируем автоматическое центрирование
            if(dynamic_cast<UModernDiagramNodeItem*>(item))
            {
                return; // Не центрируем при выделении компонентов
            }
        }
    }
    QGraphicsView::centerOn(pos);
}

void UModernDiagramView::centerOn(const QGraphicsItem* item)
{
    // Блокируем автоматическое центрирование при выделении компонентов
    if(m_owner)
    {
        // Блокируем центрирование во время движения компонента
        if(m_owner->m_isComponentMoving)
        {
            return; // Не центрируем во время движения компонента
        }

        // Блокируем центрирование, если это NodeItem
        if(dynamic_cast<const UModernDiagramNodeItem*>(item))
        {
            return; // Не центрируем при выделении компонентов
        }
    }
    QGraphicsView::centerOn(item);
}

void UModernDiagramView::mousePressEvent(QMouseEvent *event)
{
    // Проверяем, нажата ли левая кнопка мыши вместе с Ctrl
    if(event->button() == Qt::LeftButton && (event->modifiers() & Qt::ControlModifier))
    {
        // Начинаем прокрутку
        m_isPanning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Проверяем, кликнули ли на компонент
    // Если кликнули на компонент, блокируем автоматическое центрирование Qt
    // Это предотвращает движение скроллбара при клике на компонент
    if(event->button() == Qt::LeftButton && !(event->modifiers() & Qt::ControlModifier))
    {
        QPointF scenePos = mapToScene(event->pos());
        auto* clickedNode = m_owner ? m_owner->pickNode(scenePos) : nullptr;

        if(clickedNode)
        {
            // Кликнули на компонент - блокируем автоматическое центрирование
            // Сохраняем текущую позицию прокрутки
            QPointF currentCenter = mapToScene(viewport()->rect().center());

            // Сохраняем начальную позицию для RubberBandDrag
            m_rubberBandStartViewPos = event->pos();
            m_isRubberBandDragging = true;

            // Передаем событие в базовый класс, но затем восстанавливаем позицию
            QGraphicsView::mousePressEvent(event);

            // Восстанавливаем позицию прокрутки, если она изменилась
            QPointF newCenter = mapToScene(viewport()->rect().center());
            if(currentCenter != newCenter)
            {
                centerOn(currentCenter);
            }

            return;
        }

        // Не кликнули на компонент - обычная обработка
        m_rubberBandStartViewPos = event->pos();
        m_isRubberBandDragging = true;
    }

    // Иначе передаем событие в базовый класс
    QGraphicsView::mousePressEvent(event);
}

void UModernDiagramView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isPanning)
    {
        // Вычисляем смещение
        QPoint delta = event->pos() - m_lastPanPoint;

        // Прокручиваем viewport через scrollbars
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

        // Обновляем последнюю позицию
        m_lastPanPoint = event->pos();
        event->accept();
        return;
    }

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Блокируем автоматическую прокрутку во время RubberBandDrag
    // Qt автоматически прокручивает viewport, когда курсор выходит за границы видимой области
    // Это вызывает движение скроллбара при выделении группы объектов
    if(m_isRubberBandDragging)
    {
        // Сохраняем текущую позицию прокрутки
        int savedHScroll = horizontalScrollBar()->value();
        int savedVScroll = verticalScrollBar()->value();

        // Передаем событие в базовый класс
        QGraphicsView::mouseMoveEvent(event);

        // Восстанавливаем позицию прокрутки, если она изменилась
        if(horizontalScrollBar()->value() != savedHScroll)
        {
            horizontalScrollBar()->setValue(savedHScroll);
        }
        if(verticalScrollBar()->value() != savedVScroll)
        {
            verticalScrollBar()->setValue(savedVScroll);
        }

        return;
    }

    // Иначе передаем событие в базовый класс
    QGraphicsView::mouseMoveEvent(event);
}

void UModernDiagramView::mouseReleaseEvent(QMouseEvent *event)
{
    // Обработка прокрутки (Ctrl+ЛКМ)
    if(m_isPanning && event->button() == Qt::LeftButton)
    {
        // Завершаем прокрутку
        m_isPanning = false;
        unsetCursor();
        event->accept();
        return;
    }

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Проверяем, не была ли перемещена группа объектов
    // ВАЖНО: Проверяем ДО вызова базового класса, так как базовый класс может сбросить флаг
    // и вызвать UModernDiagramScene::mouseReleaseEvent, который сбросит m_isGroupMoving
    bool wasGroupMoved = false;
    if(m_owner && m_owner->m_scene)
    {
        // Проверяем флаг через публичный метод isGroupMoving()
        if(auto* modernScene = dynamic_cast<UModernDiagramScene*>(m_owner->m_scene))
        {
            wasGroupMoved = modernScene->isGroupMoving();
        }
    }

    // Сначала вызываем базовый класс, чтобы Qt обработал RubberBandDrag
    // Qt автоматически рисует прямоугольник и обрабатывает выделение
    QGraphicsView::mouseReleaseEvent(event);

    // После обработки базовым классом проверяем, был ли это RubberBand drag
    // и исправляем выделение, если нужно
    // НО НЕ вызываем selectNodesInRect, если группа была перемещена
    if(event->button() == Qt::LeftButton &&
       !(event->modifiers() & Qt::ControlModifier) &&
       m_isRubberBandDragging &&
       !wasGroupMoved)  // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: не вызываем selectNodesInRect после перемещения группы
    {
        // Вычисляем расстояние drag для проверки, был ли это реальный drag
        QPoint endPos = event->pos();
        QPoint delta = endPos - m_rubberBandStartViewPos;
        int dragDistanceSquared = delta.x() * delta.x() + delta.y() * delta.y();

        // Если был реальный drag (не просто клик)
        if(dragDistanceSquared > 25)  // 5 * 5 = 25
        {
            // Создаем прямоугольник выделения в координатах view
            QRect rubberBandRect = QRect(m_rubberBandStartViewPos, endPos).normalized();

            // Преобразуем прямоугольник из координат view в координаты scene
            QPointF topLeft = mapToScene(rubberBandRect.topLeft());
            QPointF bottomRight = mapToScene(rubberBandRect.bottomRight());
            QRectF selectionRect = QRectF(topLeft, bottomRight).normalized();

            // ВАЖНО: Базовый класс QGraphicsView::mouseReleaseEvent уже передал событие в сцену,
            // и базовый класс QGraphicsScene::mouseReleaseEvent сбросил выделение при клике на фоне.
            // Поэтому мы должны исправить выделение ПОСЛЕ того, как все обработчики завершились.
            // Используем QTimer::singleShot для отложенного вызова, чтобы он выполнился после
            // всех обработчиков событий, включая UModernDiagramScene::mouseReleaseEvent
            bool addToSelection = (event->modifiers() & Qt::ShiftModifier) != 0;

            // Сохраняем параметры для отложенного вызова
            QRectF savedSelectionRect = selectionRect;
            bool savedAddToSelection = addToSelection;

            // Вызываем selectNodesInRect сразу (для немедленного выделения)
            m_owner->selectNodesInRect(savedSelectionRect, savedAddToSelection);

            // ВАЖНО: Вызываем selectNodesInRect еще раз через QTimer::singleShot,
            // чтобы исправить выделение ПОСЛЕ того, как UModernDiagramScene::mouseReleaseEvent
            // завершится и базовый класс Qt сбросит выделение
            // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Сохраняем флаг wasGroupMoved в лямбда-функцию,
            // чтобы не вызывать selectNodesInRect, если группа была перемещена
            QTimer::singleShot(0, [this, savedSelectionRect, savedAddToSelection, wasGroupMoved]() {
                // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Если группа была перемещена, НЕ вызываем selectNodesInRect,
                // чтобы не сбросить выделение, которое уже было восстановлено в UModernDiagramScene::mouseReleaseEvent
                if(wasGroupMoved)
                {
                    return;
                }

                m_owner->selectNodesInRect(savedSelectionRect, savedAddToSelection);
            });
        }

        m_isRubberBandDragging = false;
    }
}

bool UModernDiagramView::viewportEvent(QEvent *event)
{
    if(event->type() == QEvent::Wheel)
    {
        auto* wheel = static_cast<QWheelEvent*>(event);
        const double factor = wheel->angleDelta().y() > 0 ? 1.15 : 0.87;

        // 1. Сохраняем позицию курсора в координатах сцены до масштабирования
        QPointF cursorViewportPos = wheel->position();
        QPointF scenePosBefore = mapToScene(cursorViewportPos.toPoint());

        // 2. Применяем масштабирование
        scale(factor, factor);

        // 3. Вычисляем новую позицию курсора в координатах сцены после масштабирования
        QPointF scenePosAfter = mapToScene(cursorViewportPos.toPoint());

        // 4. Корректируем позицию, чтобы точка под курсором осталась на месте
        QPointF delta = scenePosBefore - scenePosAfter;
        QPointF currentCenter = mapToScene(viewport()->rect().center());
        QPointF newCenter = currentCenter + delta;
        centerOn(newCenter);

        // 5. Проверяем видимость диаграммы и корректируем при необходимости
        if(m_owner && m_owner->m_scene)
        {
            QRectF sceneBounds = m_owner->m_scene->itemsBoundingRect();
            if(!sceneBounds.isNull())
            {
                // Получаем видимую область в координатах сцены после первой корректировки
                QRectF viewportRect = viewport()->rect();
                QPointF topLeft = mapToScene(viewportRect.topLeft().toPoint());
                QPointF bottomRight = mapToScene(viewportRect.bottomRight().toPoint());
                QRectF visibleRect(topLeft, bottomRight);

                // Проверяем, помещается ли вся диаграмма в видимую область
                bool fitsInView = visibleRect.contains(sceneBounds);

                if(!fitsInView)
                {
                    // Диаграмма не помещается - центрируем относительно курсора
                    centerOn(scenePosBefore);
                }
                else
                {
                    // Диаграмма помещается, но может быть частично за границами
                    // Корректируем позицию для сохранения видимости, если это возможно
                    QPointF finalCenter = newCenter;
                    bool needsAdjustment = false;

                    // Проверяем и корректируем по горизонтали
                    if(sceneBounds.left() < visibleRect.left())
                    {
                        // Диаграмма выходит за левую границу - сдвигаем вправо
                        double shiftX = visibleRect.left() - sceneBounds.left();
                        finalCenter.setX(finalCenter.x() + shiftX);
                        needsAdjustment = true;
                    }
                    else if(sceneBounds.right() > visibleRect.right())
                    {
                        // Диаграмма выходит за правую границу - сдвигаем влево
                        double shiftX = sceneBounds.right() - visibleRect.right();
                        finalCenter.setX(finalCenter.x() - shiftX);
                        needsAdjustment = true;
                    }

                    // Проверяем и корректируем по вертикали
                    if(sceneBounds.top() < visibleRect.top())
                    {
                        // Диаграмма выходит за верхнюю границу - сдвигаем вниз
                        double shiftY = visibleRect.top() - sceneBounds.top();
                        finalCenter.setY(finalCenter.y() + shiftY);
                        needsAdjustment = true;
                    }
                    else if(sceneBounds.bottom() > visibleRect.bottom())
                    {
                        // Диаграмма выходит за нижнюю границу - сдвигаем вверх
                        double shiftY = sceneBounds.bottom() - visibleRect.bottom();
                        finalCenter.setY(finalCenter.y() - shiftY);
                        needsAdjustment = true;
                    }

                    // Применяем корректировку только если она не слишком большая
                    // (чтобы не нарушать позицию относительно курсора слишком сильно)
                    if(needsAdjustment)
                    {
                        QPointF centerDelta = finalCenter - newCenter;
                        double maxShift = qMin(visibleRect.width(), visibleRect.height()) * 0.2; // Максимум 20% от размера viewport
                        if(centerDelta.manhattanLength() < maxShift)
                        {
                            centerOn(finalCenter);
                        }
                    }
                }
            }
        }

        return true;
    }
    return QGraphicsView::viewportEvent(event);
}

void UModernDiagramView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("Component"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void UModernDiagramView::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasFormat("Component"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void UModernDiagramView::dropEvent(QDropEvent *event)
{
    if(!event->mimeData()->hasFormat("Component"))
    {
        event->ignore();
        return;
    }

    if(!m_owner || !m_owner->m_application)
    {
        event->ignore();
        return;
    }

    QByteArray itemData = event->mimeData()->data("Component");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);
    QString classname;
    dataStream >> classname;

    if(classname.isEmpty())
    {
        event->ignore();
        return;
    }

    // Если конфигурация не открыта, то создать новую
    if(!m_owner->m_application->GetProjectOpenFlag())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning",
            "Config not created. Auto-create one-channel configuration and model from this component?",
            QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            std::string file_name;

            QString configs_path = QString::fromLocal8Bit(
                (m_owner->m_application->GetWorkDirectory() + "/../../Configs/").c_str());
            QDir path1(configs_path);
            if(!path1.exists(configs_path))
            {
                configs_path = QString::fromLocal8Bit(
                    (m_owner->m_application->GetWorkDirectory() + "/../../../Configs/").c_str());
                QDir path2(configs_path);
                if(!path2.exists(configs_path))
                {
                    configs_path = QString::fromLocal8Bit(
                        m_owner->m_application->GetWorkDirectory().c_str());
                }
            }

            QString default_path = configs_path;
            if(!m_owner->m_application->GetUserName().empty())
            {
                std::string userPathRel = m_owner->m_application->GetUserConfigPath();
                if(!userPathRel.empty())
                {
                    QString users_dir = configs_path + "Users";
                    QDir pathUsers(users_dir);
                    if(!pathUsers.exists())
                        RDK::CreateNewDirectory(users_dir.toLocal8Bit());
                    QString user_path = configs_path + QString::fromLocal8Bit(userPathRel.c_str());
                    QDir pathUser(user_path);
                    if(!pathUser.exists())
                        RDK::CreateNewDirectory(user_path.toLocal8Bit());
                    default_path = user_path;
                }
            }

            std::string path_dialog = default_path.toUtf8().data();

            // Создание папки проекта автоматическое либо выбор существующей
            if(QMessageBox::question(this, "Info", "Autocreate configuration folder?",
                QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
            {
                time_t curr_time;
                time(&curr_time);

                // Возвращает время в виде понятной строки вида YYYY.MM.DD HH:MM:SS
                std::string folder = RDK::get_text_time(curr_time, '.', '_');
                path_dialog += std::string("/Autocreate") + folder.c_str();

                if(RDK::CreateNewDirectory(std::string(path_dialog).c_str()) != 0)
                {
                    event->ignore();
                    return;
                }
            }
            else
            {
                path_dialog = QFileDialog::getExistingDirectory(this, tr("Select project directory"),
                    default_path, QFileDialog::ShowDirsOnly).toUtf8().data();
            }

            if(path_dialog.empty())
            {
                event->ignore();
                return;
            }

            file_name = path_dialog + "/Project.ini";

            m_owner->m_application->CreateProject(file_name, classname.toLocal8Bit().constData());

            m_owner->Reload();
            emit m_owner->componentSelected(QString());
            emit m_owner->updateComponentsList();
        }
        event->accept();
        return;
    }

    // Если модель не существует, спросить не создать ли ее
    if(!Model_Check())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning",
            "Model not exist. Create new model from this class?",
            QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            // Создать новую модель
            Model_Create(classname.toLocal8Bit());
            m_owner->Reload();
            emit m_owner->componentSelected(QString());
            emit m_owner->updateComponentsList();
        }
        event->accept();
        return;
    }

    const int channel = Core_GetSelectedChannelIndex();
    const QByteArray classUtf8 = classname.toUtf8();
    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLock(channel);
    RDK::UELockPtr<RDK::UStorage> stor = RDK::GetStorageLock(channel);
    if(!eng || !stor)
    {
        event->ignore();
        return;
    }

    const DiagramAddComponentResult added = addComponentUnderModelScopeWithLocks(
        eng.Get(), stor.Get(), m_owner->diagramScopeLongName(), classUtf8);
    if(!added.ok)
    {
        event->ignore();
        return;
    }

    syncEngineCurrentComponentWithEngine(eng.Get(), m_owner->diagramScopeLongName());

    const QPointF scenePos = mapToScene(event->pos());
    const QPointF absoluteScenePos = scenePos + m_owner->m_coordinateManager->getNormalizationOffset();
    m_owner->m_coordinateManager->saveCoord(added.long_name, absoluteScenePos);

    m_owner->Reload();

    if(auto* node = m_owner->m_nodeByName.value(added.short_name))
    {
        m_owner->m_isProgrammaticSelection = true;
        m_owner->m_scene->clearSelection();
        node->setSelected(true);
        QTimer::singleShot(0, m_owner, [owner = m_owner]() {
            if(owner)
                owner->m_isProgrammaticSelection = false;
        });
    }

    emit m_owner->updateComponentsList();

    const QString selectedLongName = added.long_name;
    QTimer::singleShot(0, m_owner, [owner = m_owner, selectedLongName]() {
        if(owner)
            emit owner->componentSelected(selectedLongName);
    });
    event->accept();
}

