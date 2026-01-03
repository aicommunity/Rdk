#include "UModernDiagramWidget.h"
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
#include "../Core/Engine/UStorage.h"
#include "../Core/Engine/UEngine.h"
#include "../Core/Engine/UNet.h"
#include "../../Deploy/Include/rdk_init.h"
#include "../Core/Application/UIVisualController.h"
#include "UGuiTelemetry.h"
#include <sstream>


// --------------------------- ModernScene ---------------------------

class ModernScene : public QGraphicsScene
{
public:
    explicit ModernScene(class UModernDiagramWidget* owner);
    // Публичный метод для проверки, идет ли перемещение группы
    bool isGroupMoving() const { return m_isGroupMoving; }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
private:
    UModernDiagramWidget* m_owner;
    QTimer* m_hoverTimer{nullptr};
    QPointF m_rubberBandStartPos;  // Начальная позиция для RubberBandDrag
    bool m_isRubberBandActive;  // Флаг активного RubberBandDrag
    bool m_isGroupSelected = false;  // Есть ли выделенная группа объектов
    bool m_isGroupMoving = false;    // Идет ли перемещение группы
    QList<UModernDiagramWidget::NodeItem*> m_savedSelection; // Сохраненное выделение для перемещения
    QPointF m_lastHoverPos;  // Последняя позиция курсора для оптимизации
    bool m_isProcessingMouseEvent = false;  // Флаг обработки событий мыши для пропуска pollHover
    void pollHover();
};

// --------------------------- ModernGraphicsView ---------------------------

class ModernGraphicsView : public QGraphicsView
{
public:
    ModernGraphicsView(UModernDiagramWidget* owner, QGraphicsScene* scene)
        : QGraphicsView(scene), m_owner(owner)
    {
        setAcceptDrops(true);
        // Оптимизация: настраиваем режим обновления viewport для лучшей производительности
        // SmartViewportUpdate обновляет только измененные области
        setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
        // Включаем кэширование фона для ускорения прокрутки
        setCacheMode(QGraphicsView::CacheBackground);
    }
protected:
    void mousePressEvent(QMouseEvent *event) override
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
        
        // Сохраняем начальную позицию для RubberBandDrag (если не Ctrl)
        if(event->button() == Qt::LeftButton && !(event->modifiers() & Qt::ControlModifier))
        {
            m_rubberBandStartViewPos = event->pos();
            m_isRubberBandDragging = true;
        }
        
        // Иначе передаем событие в базовый класс
        QGraphicsView::mousePressEvent(event);
    }
    
    void mouseMoveEvent(QMouseEvent *event) override
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
        
        // Иначе передаем событие в базовый класс
        QGraphicsView::mouseMoveEvent(event);
    }
    
    void mouseReleaseEvent(QMouseEvent *event) override
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
        // и вызвать ModernScene::mouseReleaseEvent, который сбросит m_isGroupMoving
        bool wasGroupMoved = false;
        if(m_owner && m_owner->m_scene)
        {
            // Проверяем флаг через публичный метод isGroupMoving()
            if(auto* modernScene = dynamic_cast<ModernScene*>(m_owner->m_scene))
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
                // всех обработчиков событий, включая ModernScene::mouseReleaseEvent
                bool addToSelection = (event->modifiers() & Qt::ShiftModifier) != 0;
                
                // Сохраняем параметры для отложенного вызова
                QRectF savedSelectionRect = selectionRect;
                bool savedAddToSelection = addToSelection;
                
                // Вызываем selectNodesInRect сразу (для немедленного выделения)
                m_owner->selectNodesInRect(savedSelectionRect, savedAddToSelection);
                
                // ВАЖНО: Вызываем selectNodesInRect еще раз через QTimer::singleShot,
                // чтобы исправить выделение ПОСЛЕ того, как ModernScene::mouseReleaseEvent
                // завершится и базовый класс Qt сбросит выделение
                // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Сохраняем флаг wasGroupMoved в лямбда-функцию,
                // чтобы не вызывать selectNodesInRect, если группа была перемещена
                QTimer::singleShot(0, [this, savedSelectionRect, savedAddToSelection, wasGroupMoved]() {
                    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Если группа была перемещена, НЕ вызываем selectNodesInRect,
                    // чтобы не сбросить выделение, которое уже было восстановлено в ModernScene::mouseReleaseEvent
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
    
    bool viewportEvent(QEvent *event) override
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
    
    void dragEnterEvent(QDragEnterEvent *event) override
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
    
    void dragMoveEvent(QDragMoveEvent *event) override
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
    
    void dropEvent(QDropEvent *event) override
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
                
                QString default_path = QString::fromLocal8Bit(
                    (m_owner->m_application->GetWorkDirectory() + "/../../Configs/").c_str());
                QDir path1(default_path);
                if(!path1.exists(default_path))
                {
                    default_path = QString::fromLocal8Bit(
                        (m_owner->m_application->GetWorkDirectory() + "/../../../Configs/").c_str());
                    QDir path2(default_path);
                    if(!path2.exists(default_path))
                    {
                        default_path = QString::fromLocal8Bit(
                            m_owner->m_application->GetWorkDirectory().c_str());
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
        
        // Создать компонент
        const char* pname = Model_AddComponent(m_owner->m_componentName.toLocal8Bit(), classname.toLocal8Bit());
        if(pname)
        {
            std::string name = pname;
            Engine_FreeBufString(pname);
            
            // Преобразовать координаты drop из view в сцену
            QPointF scenePos = mapToScene(event->pos());
            
            // Сохранить абсолютные координаты
            // При создании нового компонента координаты еще не нормализованы, так как сцена еще не перезагружена
            // Но нужно учесть текущее смещение нормализации для правильного сохранения
            QPointF absoluteScenePos = scenePos + m_owner->m_normalizationOffset;
            QString fullName = QString::fromStdString(name);
            m_owner->saveCoord(fullName, absoluteScenePos);
            
            // Перезагрузить сцену для обновления всех компонентов и связей
            m_owner->Reload();
            
            // Найти созданный узел и выделить его
            QString shortName = QString::fromStdString(name);
            if(!m_owner->m_componentName.isEmpty())
            {
                // Если мы внутри компонента, нужно извлечь короткое имя
                QString prefix = m_owner->m_componentName + ".";
                if(shortName.startsWith(prefix))
                {
                    shortName = shortName.mid(prefix.length());
                }
            }
            
            auto* node = m_owner->m_nodeByName.value(shortName, nullptr);
            if(node)
            {
                m_owner->m_scene->clearSelection();
                node->setSelected(true);
            }
            
            emit m_owner->componentSelected(QString::fromStdString(name));
            emit m_owner->updateComponentsList();
            
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
    
private:
    UModernDiagramWidget* m_owner;
    bool m_isPanning = false;
    QPoint m_lastPanPoint;
    QPoint m_rubberBandStartViewPos;  // Начальная позиция RubberBand в координатах view
    bool m_isRubberBandDragging = false;  // Флаг активного RubberBand drag
};

// --------------------------- Helpers ---------------------------

UModernDiagramWidget::NodeItem::NodeItem(UModernDiagramWidget* owner, const QString& name, const QString& cls)
    : QGraphicsRectItem()
    , nodeName(name)
    , className(cls)
    , m_owner(owner)
    , m_hoveredPort(nullptr)
    , m_portListWidgetProxy(nullptr)
    , m_portListWidget(nullptr)
    , m_hideTimer(new QTimer())
    , m_cacheValid(false)
    , m_portsCacheValid(false)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setAcceptHoverEvents(true);
    // Включаем кэширование для оптимизации отрисовки
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    
    // Создаем QTreeWidget для отображения портов
    m_portListWidget = new QTreeWidget();
    m_portListWidget->setHeaderHidden(true);
    m_portListWidget->setRootIsDecorated(true);
    m_portListWidget->setAlternatingRowColors(true);
    m_portListWidget->setMaximumHeight(300);
    m_portListWidget->setMinimumWidth(250);
    m_portListWidget->setMaximumWidth(350);
    m_portListWidget->setStyleSheet(UStyleManager::instance()->getTreeWidgetStyleSheet());
    // Устанавливаем политику фокуса при создании
    m_portListWidget->setFocusPolicy(Qt::StrongFocus);
    
    // Таймер для отложенного скрытия списка портов
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(1000); // 1 секунда после ухода курсора
    QObject::connect(m_hideTimer, &QTimer::timeout, [this]()
    {
        if(m_portListWidget && (m_portListWidget->underMouse() || m_portListWidget->hasFocus()))
        {
            // Пока мышь или фокус внутри списка, откладываем скрытие
            m_hideTimer->start(300);
            return;
        }
        hidePortListWidget();
    });
    
    // Создаем прокси для встраивания виджета в сцену
    if(m_owner && m_owner->m_scene)
    {
        m_portListWidgetProxy = m_owner->m_scene->addWidget(m_portListWidget);
        m_portListWidgetProxy->setVisible(false);
        m_portListWidgetProxy->setZValue(1000);  // Поднимаем на передний план
        // Настраиваем прокси для приема событий мыши
        m_portListWidgetProxy->setFlag(QGraphicsItem::ItemIsFocusable, true);
        m_portListWidgetProxy->setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
        m_portListWidgetProxy->setAcceptHoverEvents(true);
        
        // Подключаем обработку двойного клика
        QObject::connect(m_portListWidget, &QTreeWidget::itemDoubleClicked,
                        [this](QTreeWidgetItem* item, int column) {
                            // Логируем событие двойного клика (используем qDebug для гарантированного вывода)
                            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                                "itemDoubleClicked signal received", 0);
                            // Двойной клик всегда активирует порт
                            onPortItemActivated(item, column);
                        });
        
        // Подключаем обработку активации элемента (двойной клик или Enter)
        QObject::connect(m_portListWidget, &QTreeWidget::itemActivated,
                        [this](QTreeWidgetItem* item, int column) {
                            // Логируем событие активации (используем qDebug для гарантированного вывода)
                            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                                "itemActivated signal received", 0);
                            // Активация всегда обрабатывает порт
                            onPortItemActivated(item, column);
                        });
        
        // Обработка Enter через QShortcut
        // Используем WindowShortcut для работы даже когда виджет не имеет фокуса
        QShortcut* enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), m_portListWidget);
        QShortcut* enterShortcut2 = new QShortcut(QKeySequence(Qt::Key_Enter), m_portListWidget);
        // Используем WindowShortcut для работы даже когда виджет не имеет явного фокуса
        enterShortcut->setContext(Qt::WindowShortcut);
        enterShortcut2->setContext(Qt::WindowShortcut);
        QObject::connect(enterShortcut, &QShortcut::activated, [this]() {
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                "Enter shortcut activated", 0);
            if(m_portListWidget && m_portListWidgetProxy && m_portListWidgetProxy->isVisible())
            {
                QTreeWidgetItem* currentItem = m_portListWidget->currentItem();
                if(currentItem)
                {
                    QString logMsg = QString("Enter shortcut: currentItem found, calling onPortItemActivated. Item has parent: %1")
                        .arg(currentItem->parent() ? "yes" : "no");
                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
                    onPortItemActivated(currentItem, 0);
                }
                else
                {
                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
                        "Enter shortcut: currentItem is null", 0);
                }
            }
            else
            {
                QString warnMsg = QString("Enter shortcut: Conditions not met (widget=%1, proxy=%2, visible=%3)")
                    .arg(m_portListWidget ? "yes" : "no")
                    .arg(m_portListWidgetProxy ? "yes" : "no")
                    .arg(m_portListWidgetProxy && m_portListWidgetProxy->isVisible() ? "yes" : "no");
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, warnMsg.toStdString().c_str(), 0);
            }
        });
        QObject::connect(enterShortcut2, &QShortcut::activated, [this]() {
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                "Enter (numpad) shortcut activated", 0);
            if(m_portListWidget && m_portListWidgetProxy && m_portListWidgetProxy->isVisible())
            {
                QTreeWidgetItem* currentItem = m_portListWidget->currentItem();
                if(currentItem)
                {
                    QString logMsg = QString("Enter (numpad) shortcut: currentItem found, calling onPortItemActivated. Item has parent: %1")
                        .arg(currentItem->parent() ? "yes" : "no");
                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
                    onPortItemActivated(currentItem, 0);
                }
                else
                {
                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
                        "Enter (numpad) shortcut: currentItem is null", 0);
                }
            }
            else
            {
                QString warnMsg = QString("Enter (numpad) shortcut: Conditions not met (widget=%1, proxy=%2, visible=%3)")
                    .arg(m_portListWidget ? "yes" : "no")
                    .arg(m_portListWidgetProxy ? "yes" : "no")
                    .arg(m_portListWidgetProxy && m_portListWidgetProxy->isVisible() ? "yes" : "no");
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, warnMsg.toStdString().c_str(), 0);
            }
        });
        
        // Добавляем обработку одинарного клика для выходных и входных портов
        QObject::connect(m_portListWidget, &QTreeWidget::itemClicked,
                        [this](QTreeWidgetItem* item, int column) {
                            // Для выходных портов одинарный клик начинает соединение
                            if(item && item->parent() != nullptr)
                            {
                                QVariant data = item->data(0, Qt::UserRole);
                                if(data.isValid())
                                {
                                    QMap<QString, QVariant> portData = data.value<QMap<QString, QVariant>>();
                                    bool isInput = portData["isInput"].toBool();
                                    QString portName = portData["name"].toString();
                                    
                                    QString logMsg = QString("itemClicked: %1 port '%2', activeTempLink=%3")
                                        .arg(isInput ? "input" : "output")
                                        .arg(portName)
                                        .arg(m_owner && m_owner->m_activeTempLink ? "yes" : "no");
                                    
                                    // Для выходных портов начинаем соединение
                                    if(!isInput && !portName.isEmpty())
                                    {
                                        onPortItemActivated(item, column);
                                    }
                                    // Для входных портов завершаем соединение, если оно активно
                                    else if(isInput && !portName.isEmpty() && m_owner && m_owner->m_activeTempLink)
                                    {
                                        onPortItemActivated(item, column);
                                    }
                                    else if(isInput && !portName.isEmpty())
                                    {
                                        QString warnMsg = QString("itemClicked: Input port clicked but no active connection (activeTempLink=%1)")
                                            .arg(m_owner && m_owner->m_activeTempLink ? "yes" : "no");
                                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, warnMsg.toStdString().c_str(), 0);
                                    }
                                }
                                else
                                {
                                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
                                        "itemClicked: Port data is invalid", 0);
                                }
                            }
                        });
    }

    // Константы для размеров
    const double minWidth = 180.0;  // Увеличено в 1.5 раза (120 * 1.5)
    const double minHeight = 40.0;  // Минимальная высота для текста
    const double portSpacing = 20.0;
    const double topMargin = 30.0;  // Отступ сверху для текста
    const double bottomMargin = 10.0;  // Отступ снизу

    // Загружаем реальные порты компонента
    if(m_owner && m_owner->m_application)
    {
        QString fullName = m_owner->m_componentName.isEmpty() ? name : m_owner->m_componentName + "." + name;
        
        // Загружаем входные порты по категориям
        // Всегда создаем три порта: собственные, дочерние и алиасы
        // Если портов нет, порт все равно создается, но будет отображаться серым
        
        // 1. Собственные входные свойства компонента
        Port ownInputPort;
        ownInputPort.isInput = true;
        ownInputPort.name = QStringLiteral("Own");
        ownInputPort.componentName = name;
        ownInputPort.fullPath = QStringLiteral("");  // Маркер для фильтрации
        ownInputPort.displayName = QStringLiteral("Собственные");
        ownInputPort.category = PortCategory::Own;
        inputs.append(ownInputPort);
        
        // 2. Входные свойства дочерних компонентов
        Port childInputPort;
        childInputPort.isInput = true;
        childInputPort.name = QStringLiteral("Children");
        childInputPort.componentName = name;
        childInputPort.fullPath = QStringLiteral("");  // Маркер для фильтрации
        childInputPort.displayName = QStringLiteral("Дочерние");
        childInputPort.category = PortCategory::Child;
        inputs.append(childInputPort);
        
        // 3. Входные свойства алиасов
        Port aliasInputPort;
        aliasInputPort.isInput = true;
        aliasInputPort.name = QStringLiteral("Aliases");
        aliasInputPort.componentName = name;
        aliasInputPort.fullPath = QStringLiteral("");  // Маркер для фильтрации
        aliasInputPort.displayName = QStringLiteral("Алиасы");
        aliasInputPort.category = PortCategory::Alias;
        inputs.append(aliasInputPort);
        
        // Загружаем выходные порты по категориям
        // Всегда создаем три порта: собственные, дочерние и алиасы
        // Если портов нет, порт все равно создается, но будет отображаться серым
        
        // 1. Собственные выходные свойства компонента
        Port ownPort;
        ownPort.isInput = false;
        ownPort.name = QStringLiteral("Own");
        ownPort.componentName = name;
        ownPort.fullPath = QStringLiteral("");  // Маркер для фильтрации
        ownPort.displayName = QStringLiteral("Собственные");
        ownPort.category = PortCategory::Own;
        outputs.append(ownPort);
        
        // 2. Выходные свойства дочерних компонентов
        Port childPort;
        childPort.isInput = false;
        childPort.name = QStringLiteral("Children");
        childPort.componentName = name;
        childPort.fullPath = QStringLiteral("");  // Маркер для фильтрации
        childPort.displayName = QStringLiteral("Дочерние");
        childPort.category = PortCategory::Child;
        outputs.append(childPort);
        
        // 3. Выходные свойства алиасов
        Port aliasPort;
        aliasPort.isInput = false;
        aliasPort.name = QStringLiteral("Aliases");
        aliasPort.componentName = name;
        aliasPort.fullPath = QStringLiteral("");  // Маркер для фильтрации
        aliasPort.displayName = QStringLiteral("Алиасы");
        aliasPort.category = PortCategory::Alias;
        outputs.append(aliasPort);
    }
    else
    {
        // Fallback: используем дефолтные порты
        Port defaultInput;
        defaultInput.isInput = true;
        defaultInput.name = QStringLiteral("In");
        defaultInput.componentName = name;
        defaultInput.fullPath = QStringLiteral("In");
        defaultInput.displayName = QStringLiteral("In");
        inputs.append(defaultInput);
        
        Port defaultOutput;
        defaultOutput.isInput = false;
        defaultOutput.name = QStringLiteral("Out");
        defaultOutput.componentName = name;
        defaultOutput.fullPath = QStringLiteral("Out");
        defaultOutput.displayName = QStringLiteral("Out");
        outputs.append(defaultOutput);
    }
    
    // Вычисляем необходимую высоту на основе количества портов
    int maxPorts = qMax(inputs.size(), outputs.size());
    double requiredHeight = topMargin + bottomMargin;
    if(maxPorts > 0)
    {
        requiredHeight += (maxPorts - 1) * portSpacing;
    }
    requiredHeight = qMax(requiredHeight, minHeight);
    
    // Устанавливаем размер прямоугольника (центр в (0,0))
    double width = minWidth;
    double height = requiredHeight;
    setRect(-width/2, -height/2, width, height);
    
    // Распределяем входные порты
    if(inputs.size() == 1)
    {
        inputs[0].pos = QPointF(rect().left(), 0);
    }
    else if(inputs.size() > 1)
    {
        double startY = -height/2 + topMargin;
        for(int i = 0; i < inputs.size(); ++i)
        {
            inputs[i].pos = QPointF(rect().left(), startY + i * portSpacing);
        }
    }
    
    // Распределяем выходные порты
    if(outputs.size() == 1)
    {
        outputs[0].pos = QPointF(rect().right(), 0);
    }
    else if(outputs.size() > 1)
    {
        double startY = -height/2 + topMargin;
        for(int i = 0; i < outputs.size(); ++i)
        {
            outputs[i].pos = QPointF(rect().right(), startY + i * portSpacing);
        }
    }
}

UModernDiagramWidget::NodeItem::~NodeItem()
{
    // Останавливаем таймер
    if(m_hideTimer)
    {
        m_hideTimer->stop();
        delete m_hideTimer;
        m_hideTimer = nullptr;
    }
    
    // Если указатели обнулены (clearScene() был вызван), то сцена
    // сама удалит прокси-виджет и виджет списка портов.
    // Удаляем только если указатели ещё валидны (индивидуальное удаление узла).
    if(m_portListWidgetProxy)
    {
        if(m_owner && m_owner->m_scene)
        {
            // Проверяем, что прокси виджет еще в сцене
            if(m_portListWidgetProxy->scene() == m_owner->m_scene)
            {
                m_owner->m_scene->removeItem(m_portListWidgetProxy);
            }
        }
        // Удаление прокси-виджета также удалит m_portListWidget (он принадлежит прокси)
        delete m_portListWidgetProxy;
        m_portListWidgetProxy = nullptr;
        m_portListWidget = nullptr;  // Уже удалён вместе с прокси
    }
    else if(m_portListWidget)
    {
        // Прокси не был создан - удаляем виджет напрямую
        delete m_portListWidget;
        m_portListWidget = nullptr;
    }
}

QRectF UModernDiagramWidget::NodeItem::boundingRect() const
{
    return rect().adjusted(-4, -4, 4, 4);
}

void UModernDiagramWidget::NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    // Обновляем кэш, если он невалиден
    // ОПТИМИЗАЦИЯ: используем кэшированные порты вместо вызова методов получения портов
    // Это критично для производительности - вызовы API ядра очень медленные
    if(!m_cacheValid)
    {
        // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
        // NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UModernDiagramWidget.NodeItem.paint"), nodeName);
        
        // ОПТИМИЗАЦИЯ: загружаем порты только если кэш портов невалиден
        // Это позволяет избежать повторных вызовов API ядра при каждой инвалидации кэша paint
        qint64 portsLoadTime = 0;
        if(!m_portsCacheValid)
        {
            // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
            // NMSDK::UGuiTelemetryScope portsTelemetry(QStringLiteral("UModernDiagramWidget.NodeItem.paint.loadPorts"), nodeName);
            // Загружаем порты один раз - они не меняются при изменении связей
            getOwnInputPorts();  // Загрузит и закэширует
            getChildInputPorts();  // Загрузит и закэширует
            getAliasInputPorts();  // Загрузит и закэширует
            getOwnOutputPorts();  // Загрузит и закэширует
            getChildOutputPorts();  // Загрузит и закэширует
            getAliasOutputPorts();  // Загрузит и закэширует
            // portsLoadTime = portsTelemetry.Elapsed();
        }
        
        // Кэшируем результаты проверки наличия портов (используем уже закэшированные порты)
        m_hasInputPortsCache[PortCategory::Own] = !m_cachedOwnInputPorts.isEmpty();
        m_hasInputPortsCache[PortCategory::Child] = !m_cachedChildInputPorts.isEmpty();
        m_hasInputPortsCache[PortCategory::Alias] = !m_cachedAliasInputPorts.isEmpty();
        m_hasOutputPortsCache[PortCategory::Own] = !m_cachedOwnOutputPorts.isEmpty();
        m_hasOutputPortsCache[PortCategory::Child] = !m_cachedChildOutputPorts.isEmpty();
        m_hasOutputPortsCache[PortCategory::Alias] = !m_cachedAliasOutputPorts.isEmpty();
        
        // Кэшируем результаты проверки соединений с детальным профилированием
        qint64 connectionsCheckTime = 0;
        {
            // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
            // NMSDK::UGuiTelemetryScope connectionsTelemetry(QStringLiteral("UModernDiagramWidget.NodeItem.paint.checkConnections"), nodeName);
            m_hasConnectionsToInputCache[PortCategory::Own] = hasConnectionsToInputCategory(PortCategory::Own);
            m_hasConnectionsToInputCache[PortCategory::Child] = hasConnectionsToInputCategory(PortCategory::Child);
            m_hasConnectionsToInputCache[PortCategory::Alias] = hasConnectionsToInputCategory(PortCategory::Alias);
            m_hasConnectionsToOutputCache[PortCategory::Own] = hasConnectionsToOutputCategory(PortCategory::Own);
            m_hasConnectionsToOutputCache[PortCategory::Child] = hasConnectionsToOutputCategory(PortCategory::Child);
            m_hasConnectionsToOutputCache[PortCategory::Alias] = hasConnectionsToOutputCategory(PortCategory::Alias);
            // connectionsCheckTime = connectionsTelemetry.Elapsed();
        }
        
        m_cacheValid = true;
        
        // Логируем результат профилирования (только при инвалидации кэша)
        // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
        // qint64 elapsed = telemetry.Elapsed();
        // QString componentDisplayName = m_owner && !m_owner->m_componentName.isEmpty() ? m_owner->m_componentName : "root";
        // QString details = QString("cache invalidated, node: %1, portsLoaded: %2, portsTime: %3ms, connectionsTime: %4ms")
        //     .arg(nodeName).arg(m_portsCacheValid ? "yes" : "no").arg(portsLoadTime).arg(connectionsCheckTime);
        // QString logMsg = QString("[UModernDiagramWidget] Component: %1, Operation: NodeItem.paint, Duration: %2ms, Details: %3")
        //     .arg(componentDisplayName).arg(elapsed).arg(details);
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    }

    UStyleManager* style = UStyleManager::instance();
    double cornerRadius = style->getNodeCornerRadius();
    
    // Рисуем тень под узлом
    QColor shadowColor = style->getShadowColor();
    double shadowBlur = style->getShadowBlur();
    double shadowOffsetY = style->getShadowOffsetY();
    
    if (shadowColor.alpha() > 0)
    {
        QRectF shadowRect = rect().adjusted(2, 2, 2, 2).translated(0, shadowOffsetY);
        painter->setPen(Qt::NoPen);
        
        // Оптимизация: используем только 2 слоя вместо 4 для ускорения отрисовки
        // Внешний слой (более размытый)
        QColor outerColor = shadowColor;
        outerColor.setAlpha(shadowColor.alpha() / 3);
        painter->setBrush(outerColor);
        QRectF outerRect = shadowRect.adjusted(-shadowBlur, -shadowBlur, shadowBlur, shadowBlur);
        painter->drawRoundedRect(outerRect, cornerRadius + shadowBlur, cornerRadius + shadowBlur);
        
        // Внутренний слой (более четкий)
        QColor innerColor = shadowColor;
        innerColor.setAlpha(shadowColor.alpha() / 2);
        painter->setBrush(innerColor);
        QRectF innerRect = shadowRect.adjusted(-shadowBlur/2, -shadowBlur/2, shadowBlur/2, shadowBlur/2);
        painter->drawRoundedRect(innerRect, cornerRadius + shadowBlur/2, cornerRadius + shadowBlur/2);
    }
    
    // Градиентный фон узла
    QLinearGradient gradient = style->getNodeGradient(rect());
    if (isSelected())
    {
        // Для выделенного узла используем специальный цвет с большим контрастом
        QColor selectedColor = style->getNodeFillSelectedColor();
        gradient.setColorAt(0, selectedColor.lighter(110));
        gradient.setColorAt(1, selectedColor.darker(110));
    }
    
    // Рисуем основной прямоугольник узла
    QColor border = isSelected() ? style->getAccentColor() : style->getNodeBorderColor();
    // Для выделенных узлов делаем границу толще и ярче
    double borderWidth = isSelected() ? style->getNodeBorderWidth() * 2.5 : style->getNodeBorderWidth();
    QColor borderColor = isSelected() ? border.lighter(120) : border;
    painter->setPen(QPen(borderColor, borderWidth));
    painter->setBrush(gradient);
    painter->drawRoundedRect(rect(), cornerRadius, cornerRadius);
    
    // Для выделенных узлов добавляем дополнительную яркую рамку
    if (isSelected())
    {
        QColor highlightColor = style->getAccentColor();
        highlightColor.setAlpha(200);
        painter->setPen(QPen(highlightColor, 3.0));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(rect().adjusted(1, 1, -1, -1), cornerRadius - 1, cornerRadius - 1);
    }
    
    // Рисуем светлую линию сверху для эффекта объёма
    painter->setPen(QPen(QColor(255, 255, 255, 80), 1));
    QRectF topLine = rect().adjusted(cornerRadius, 1, -cornerRadius, 0);
    topLine.setHeight(0);
    painter->drawLine(topLine.topLeft(), topLine.topRight());

    // Текст: имя компонента
    QFont nameFont = painter->font();
    nameFont.setWeight(QFont::DemiBold);
    painter->setFont(nameFont);
    painter->setPen(style->getTextColor());
    painter->drawText(rect().adjusted(10, 8, -10, -8),
                     Qt::AlignTop | Qt::AlignLeft,
                     nodeName);
    
    // Класс компонента в квадратных скобках внизу (меньший размер, вторичный цвет)
    QFont classFont = painter->font();
    classFont.setWeight(QFont::Normal);
    classFont.setPointSizeF(classFont.pointSizeF() * 0.85);
    painter->setFont(classFont);
    painter->setPen(style->getTextSecondaryColor());
    QString classNameText = "[" + className + "]";
    painter->drawText(rect().adjusted(10, 8, -10, -8),
                     Qt::AlignBottom | Qt::AlignLeft,
                     classNameText);

    // Входные порты
    for (const Port& p : inputs) {
        bool isHovered = (m_hoveredPort == &p);
        
        // Проверяем, активна ли временная линия для создания соединения
        bool isActiveConnection = (m_owner && m_owner->m_activeTempLink && m_owner->m_activeSourceNode);
        
        // Используем кэшированные значения вместо дорогих вызовов методов
        bool hasPorts = false;
        if (p.category == PortCategory::Own)
        {
            hasPorts = m_hasInputPortsCache.value(PortCategory::Own, false);
        }
        else if (p.category == PortCategory::Child)
        {
            hasPorts = m_hasInputPortsCache.value(PortCategory::Child, false);
        }
        else if (p.category == PortCategory::Alias)
        {
            hasPorts = m_hasInputPortsCache.value(PortCategory::Alias, false);
        }
        
        // Проверяем, есть ли соединения к портам этой категории
        // Проверяем соединения для всех категоризированных портов (с пустым fullPath)
        bool hasConnections = false;
        if (p.fullPath.isEmpty())  // Только для категоризированных портов
        {
            hasConnections = m_hasConnectionsToInputCache.value(p.category, false);
        }
        
        // Определяем цвет порта:
        // - Если активна временная линия и есть порты: подсвечиваем ярче
        // - Если есть соединения: активный цвет (даже если портов нет, но есть соединения - это означает, что соединения идут к скрытым портам)
        // - Если портов нет и соединений нет: серый (недоступный)
        // - Если порты есть, но соединений нет: обычный цвет
        QColor portColor;
        if (isActiveConnection && hasPorts)
        {
            // Подсветка входных портов при активном соединении
            portColor = isHovered ? style->getPortInputHoverColor().lighter(120) : style->getPortInputColor().lighter(110);
        }
        else if (hasConnections)
        {
            // Порт с соединениями - активный цвет
            portColor = isHovered ? style->getPortInputHoverColor() : style->getPortInputColor();
        }
        else if (!hasPorts)
        {
            // Нет портов и нет соединений - серый (недоступный)
            portColor = QColor(150, 150, 150);
        }
        else
        {
            // Есть порты, но нет соединений - обычный цвет
            portColor = isHovered ? style->getPortInputHoverColor() : style->getPortInputColor();
        }
        
        double portSize = isHovered ? style->getPortHoverRadius() : style->getPortRadius();
        
        // Заливка порта
        painter->setPen(Qt::NoPen);
        painter->setBrush(portColor);
        
        // Рисуем разные формы в зависимости от категории порта
        if (p.category == PortCategory::Own)
        {
            // Круг для собственных свойств
            painter->drawEllipse(p.pos, portSize, portSize);
        }
        else if (p.category == PortCategory::Child)
        {
            // Квадрат для свойств дочерних компонентов
            QRectF squareRect(p.pos.x() - portSize, p.pos.y() - portSize, 
                             portSize * 2, portSize * 2);
            painter->drawRect(squareRect);
        }
        else if (p.category == PortCategory::Alias)
        {
            // Треугольник для алиасов (вписанный в круг)
            QPolygonF triangle;
            double angleStep = 2.0 * M_PI / 3.0;  // 120 градусов между точками
            for(int i = 0; i < 3; ++i)
            {
                double angle = i * angleStep - M_PI / 2.0;  // Начинаем сверху
                triangle << QPointF(p.pos.x() + portSize * cos(angle),
                                    p.pos.y() + portSize * sin(angle));
            }
            painter->drawPolygon(triangle);
        }
        else
        {
            // Fallback: круг по умолчанию
            painter->drawEllipse(p.pos, portSize, portSize);
        }
        
        // Белая обводка для контраста
        painter->setPen(QPen(style->getBackgroundAltColor(), 1.5));
        painter->setBrush(Qt::NoBrush);
        if (p.category == PortCategory::Own || p.category == PortCategory::Alias)
        {
            painter->drawEllipse(p.pos, portSize, portSize);
        }
        else if (p.category == PortCategory::Child)
        {
            QRectF squareRect(p.pos.x() - portSize, p.pos.y() - portSize, 
                             portSize * 2, portSize * 2);
            painter->drawRect(squareRect);
        }
        
        // Подсветка при hover
        if (isHovered)
        {
            QColor glowColor = portColor;
            glowColor.setAlpha(60);
            painter->setPen(Qt::NoPen);
            painter->setBrush(glowColor);
            painter->drawEllipse(p.pos, portSize + 4, portSize + 4);
        }
    }
    
    // Выходные порты
    for (const Port& p : outputs) {
        bool isHovered = (m_hoveredPort == &p);
        
        // Используем кэшированные значения вместо дорогих вызовов методов
        bool hasPorts = false;
        if (p.category == PortCategory::Own)
        {
            hasPorts = m_hasOutputPortsCache.value(PortCategory::Own, false);
        }
        else if (p.category == PortCategory::Child)
        {
            hasPorts = m_hasOutputPortsCache.value(PortCategory::Child, false);
        }
        else if (p.category == PortCategory::Alias)
        {
            hasPorts = m_hasOutputPortsCache.value(PortCategory::Alias, false);
        }
        
        // Проверяем, есть ли соединения от портов этой категории
        // Проверяем соединения для всех категоризированных портов (с пустым fullPath)
        bool hasConnections = false;
        if (p.fullPath.isEmpty())  // Только для категоризированных портов
        {
            hasConnections = m_hasConnectionsToOutputCache.value(p.category, false);
        }
        
        // Определяем цвет порта:
        // - Если есть соединения: активный цвет (даже если портов нет, но есть соединения - это означает, что соединения идут от скрытых портов)
        // - Если портов нет и соединений нет: серый (недоступный)
        // - Если порты есть, но соединений нет: обычный цвет
        QColor portColor;
        if (hasConnections)
        {
            // Порт с соединениями - активный цвет
            portColor = isHovered ? style->getPortOutputHoverColor() : style->getPortOutputColor();
        }
        else if (!hasPorts)
        {
            // Нет портов и нет соединений - серый (недоступный)
            portColor = QColor(150, 150, 150);
        }
        else
        {
            // Есть порты, но нет соединений - обычный цвет
            portColor = isHovered ? style->getPortOutputHoverColor() : style->getPortOutputColor();
        }
        
        double portSize = isHovered ? style->getPortHoverRadius() : style->getPortRadius();
        
        // Заливка порта
        painter->setPen(Qt::NoPen);
        painter->setBrush(portColor);
        
        // Рисуем разные формы в зависимости от категории порта
        if (p.category == PortCategory::Own)
        {
            // Круг для собственных свойств
            painter->drawEllipse(p.pos, portSize, portSize);
        }
        else if (p.category == PortCategory::Child)
        {
            // Квадрат для свойств дочерних компонентов
            QRectF squareRect(p.pos.x() - portSize, p.pos.y() - portSize, 
                             portSize * 2, portSize * 2);
            painter->drawRect(squareRect);
        }
        else if (p.category == PortCategory::Alias)
        {
            // Треугольник для алиасов (вписанный в круг)
            QPolygonF triangle;
            double angleStep = 2.0 * M_PI / 3.0;  // 120 градусов между точками
            for(int i = 0; i < 3; ++i)
            {
                double angle = i * angleStep - M_PI / 2.0;  // Начинаем сверху
                triangle << QPointF(p.pos.x() + portSize * cos(angle),
                                    p.pos.y() + portSize * sin(angle));
            }
            painter->drawPolygon(triangle);
        }
        else
        {
            // Fallback: круг по умолчанию
            painter->drawEllipse(p.pos, portSize, portSize);
        }
        
        // Белая обводка для контраста
        painter->setPen(QPen(style->getBackgroundAltColor(), 1.5));
        painter->setBrush(Qt::NoBrush);
        if (p.category == PortCategory::Own || p.category == PortCategory::Alias)
        {
            painter->drawEllipse(p.pos, portSize, portSize);
        }
        else if (p.category == PortCategory::Child)
        {
            QRectF squareRect(p.pos.x() - portSize, p.pos.y() - portSize, 
                             portSize * 2, portSize * 2);
            painter->drawRect(squareRect);
        }
        
        // Подсветка при hover
        if (isHovered)
        {
            QColor glowColor = portColor;
            glowColor.setAlpha(60);
            painter->setPen(Qt::NoPen);
            painter->setBrush(glowColor);
            painter->drawEllipse(p.pos, portSize + 4, portSize + 4);
        }
    }
}

QPointF UModernDiagramWidget::NodeItem::scenePortPos(bool output) const
{
    const QVector<Port>& ports = output ? outputs : inputs;
    if(ports.isEmpty())
        return scenePos();
    return mapToScene(ports.first().pos);
}

QPointF UModernDiagramWidget::NodeItem::scenePortPosByCategory(bool output, PortCategory category) const
{
    const QVector<Port>& ports = output ? outputs : inputs;
    for(const Port& port : ports)
    {
        if(port.category == category && port.fullPath.isEmpty())
        {
            return mapToScene(port.pos);
        }
    }
    // Fallback: возвращаем позицию первого порта
    return scenePortPos(output);
}

UModernDiagramWidget::PortCategory UModernDiagramWidget::NodeItem::determinePortCategory(const QString& propertyName, bool isInput) const
{
    // Сначала проверяем, содержит ли propertyName путь к дочернему компоненту
    // Это имеет приоритет, так как соединения к дочерним компонентам должны определяться как Child
    // На верхнем уровне (m_componentName.isEmpty()) propertyName уже является относительным путем
    QString fullName = m_owner ? (m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName) : nodeName;
    
    // Убеждаемся, что fullName не пустое перед запросом дочерних компонентов
    if(!fullName.isEmpty())
    {
        const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
        if(compList)
        {
            QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
            Engine_FreeBufString(compList);
            
            if(!components.isEmpty())
            {
                // Проверяем, начинается ли propertyName с любого дочернего компонента
                // Это важно для многоуровневых путей (например, "Dendrite1_1.ExcSynapse1.SynapticInputs")
                // На верхнем уровне propertyName уже относительный (например, "Dendrite1_1.ExcSynapse1")
                for(const QString& comp : components)
                {
                    if(propertyName.startsWith(comp + ".") || propertyName == comp)
                    {
                        return PortCategory::Child;
                    }
                }
                
                // Если propertyName начинается с nodeName, проверяем часть после nodeName
                if(propertyName.startsWith(nodeName + "."))
                {
                    QString afterNodeName = propertyName.mid(nodeName.length() + 1);
                    if(!afterNodeName.isEmpty())
                    {
                        QStringList parts = afterNodeName.split('.');
                        if(!parts.isEmpty())
                        {
                            QString firstPart = parts.first();
                            // Если первая часть после nodeName является дочерним компонентом - это Child
                            if(components.contains(firstPart))
                            {
                                return PortCategory::Child;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Извлекаем имя свойства (может быть "Property" или "Component.Property")
    QString propName = propertyName;
    if(propName.contains('.'))
    {
        propName = propName.split('.').last();
    }
    
    // Проверяем фактическое наличие свойства в каждой категории
    // Сначала проверяем алиасы (они имеют приоритет)
    QVector<Port> aliasPorts = isInput ? getAliasInputPorts() : getAliasOutputPorts();
    for(const Port& port : aliasPorts)
    {
        if(port.name == propName || 
           port.fullPath == propertyName ||
           port.fullPath.endsWith("." + propName))
        {
            return PortCategory::Alias;
        }
    }
    
    // Затем проверяем дочерние компоненты
    // Проверяем точное совпадение с портами из getChildInputPorts/getChildOutputPorts
    QVector<Port> childPorts = isInput ? getChildInputPorts() : getChildOutputPorts();
    for(const Port& port : childPorts)
    {
        // Проверяем различные форматы сопоставления
        if(port.name == propName || 
           port.fullPath == propertyName ||
           port.fullPath.endsWith("." + propName) ||
           propertyName.contains(port.componentName + "." + propName))
        {
            return PortCategory::Child;
        }
        
        // Проверяем, начинается ли propertyName с пути к дочернему компоненту
        if(propertyName.startsWith(port.componentName + "."))
        {
            return PortCategory::Child;
        }
        
        // Проверяем, содержит ли propertyName путь к дочернему компоненту
        if(propertyName.contains("." + port.componentName + ".") ||
           propertyName.startsWith(port.componentName + "."))
        {
            return PortCategory::Child;
        }
    }
    
    // Наконец проверяем собственные свойства
    QVector<Port> ownPorts = isInput ? getOwnInputPorts() : getOwnOutputPorts();
    for(const Port& port : ownPorts)
    {
        if(port.name == propName || 
           port.fullPath == propertyName ||
           port.fullPath == propName)
        {
            return PortCategory::Own;
        }
    }
    
    // Если свойство не найдено ни в одной категории, определяем по имени:
    // Если содержит точку и начинается с nodeName - проверяем, является ли первый компонент дочерним
    if(propertyName.contains('.'))
    {
        // Если не начинается с nodeName, это может быть дочерний компонент
        if(!propertyName.startsWith(nodeName + "."))
        {
            return PortCategory::Child;
        }
        // Если начинается с nodeName, но не нашли в категориях - по умолчанию Child
        return PortCategory::Child;
    }
    
    return PortCategory::Own;
}

QVariant UModernDiagramWidget::NodeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemPositionHasChanged && scene())
    {
        // Оптимизация: обновляем только связи, подключенные к этому узлу
        // Вместо обновления всех связей в сцене используем кэш m_connectedLinks
        for(LinkItem* link : m_connectedLinks)
        {
            if(link && link->scene())
                link->updateGeometry();
        }
        
        // Перемещение группы выделенных объектов
        if(m_owner && isSelected())
        {
            // Получаем текущую позицию
            QPointF newPos = pos();
            // Получаем старую позицию из хэш-таблицы
            QPointF oldPos = m_owner->m_lastNodePositions.value(this, newPos);
            
            // Вычисляем смещение
            QPointF delta = newPos - oldPos;
            
            // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Предотвращаем рекурсивное перемещение группы
            // Если уже идет групповое перемещение (m_isMovingGroup == true), не перемещаем другие узлы,
            // чтобы избежать бесконечной рекурсии
            if(!delta.isNull() && delta.manhattanLength() > 0.1 && !m_owner->m_isMovingGroup)
            {
                // Получаем все выделенные объекты
                QList<QGraphicsItem*> selectedItems = scene()->selectedItems();
                
                // Подсчитываем количество выделенных NodeItem
                int selectedNodeCount = 0;
                QStringList selectedNodeNames;
                for(QGraphicsItem* item : selectedItems)
                {
                    auto* node = dynamic_cast<UModernDiagramWidget::NodeItem*>(item);
                    if(node && node->isSelected())
                    {
                        selectedNodeCount++;
                        selectedNodeNames << node->nodeName;
                    }
                }
                
                // Если выделено больше одного объекта, перемещаем всю группу
                if(selectedNodeCount > 1)
                {
                    // Устанавливаем флаг группового перемещения, чтобы предотвратить рекурсию
                    m_owner->m_isMovingGroup = true;
                    
                    // ДИАГНОСТИКА: Логируем движение группы
                    for(QGraphicsItem* item : selectedItems)
                    {
                        auto* otherNode = dynamic_cast<UModernDiagramWidget::NodeItem*>(item);
                        // Пропускаем текущий объект (он уже перемещен)
                        if(otherNode && otherNode != this && otherNode->isSelected())
                        {
                            // Получаем старую позицию другого объекта
                            QPointF otherOldPos = m_owner->m_lastNodePositions.value(otherNode, otherNode->pos());
                            // Перемещаем другой выделенный объект на то же смещение
                            // m_isMovingGroup предотвратит рекурсивный вызов itemChange для других узлов
                            otherNode->setPos(otherOldPos + delta);
                            // Обновляем сохраненную позицию
                            m_owner->m_lastNodePositions[otherNode] = otherNode->pos();
                        }
                    }
                    
                    // Сбрасываем флаг группового перемещения
                    m_owner->m_isMovingGroup = false;
                }
            }
            
            // Обновляем сохраненную позицию для текущего объекта
            m_owner->m_lastNodePositions[this] = newPos;
        }
        
        // Сохранить координаты
        // Важно: сохраняем абсолютные координаты (с учетом визуальной нормализации)
        if(m_owner)
        {
            QString fullName = m_owner->m_componentName.isEmpty() ? nodeName
                                                                  : m_owner->m_componentName + "." + nodeName;
            // Сохраняем координаты напрямую из scenePos() без учета нормализации
            // Нормализация применяется только для визуального отображения, координаты в ядре должны быть абсолютными
            QPointF normalizedPos = scenePos();
            // Денормализуем: scenePos() уже нормализован, добавляем offset для получения абсолютных координат
            QPointF absoluteScenePos = normalizedPos + m_owner->m_normalizationOffset;
            // Удалено избыточное логирование - создавало спам в INFO логах
            m_owner->saveCoord(fullName, absoluteScenePos);
        }
    }
    else if(change == QGraphicsItem::ItemSelectedHasChanged && m_owner)
    {
        // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Если идет batch-выделение, не обрабатываем ItemSelectedHasChanged,
        // чтобы предотвратить сброс выделения Qt. Обработка будет выполнена после завершения batch-выделения.
        if(m_owner->m_isBatchSelecting)
        {
            // Пропускаем обработку ItemSelectedHasChanged во время batch-выделения
            return QGraphicsRectItem::itemChange(change, value);
        }
        
        // Отслеживаем изменение выбора компонента
        bool isNowSelected = value.toBool();
        if(isNowSelected)
        {
            QString fullName = m_owner->m_componentName.isEmpty() ? nodeName
                                                                  : m_owner->m_componentName + "." + nodeName;
            emit m_owner->componentSelected(fullName);
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

const UModernDiagramWidget::Port* UModernDiagramWidget::NodeItem::getPortAtPosition(const QPointF& localPos) const
{
    const double portRadius = 6.0;
    const QVector<Port>* portLists[] = {&inputs, &outputs};
    for(const QVector<Port>* ports : portLists)
    {
        for(const Port& port : *ports)
        {
            QPointF diff = localPos - port.pos;
            double dist = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());
            if(dist <= portRadius)
                return &port;
        }
    }
    return nullptr;
}

void UModernDiagramWidget::NodeItem::refreshHoverAtScenePos(const QPointF& scenePos)
{
    QPointF localPos = mapFromScene(scenePos);
    const Port* port = getPortAtPosition(localPos);
    bool portChanged = (port != m_hoveredPort);
    if(portChanged)
    {
        m_hoveredPort = port;
        update(); // Только визуальная подсветка
        // Убрано: showPortListWidget - окно открывается только при клике
    }
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getNestedPorts(bool isInput, bool includeNested) const
{
    QVector<Port> result;
    if(!m_owner || !m_owner->m_application)
        return result;
    
    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
    
    // Получаем порты текущего компонента
    unsigned int mask = isInput ? (ptPubInput | ptInput) : (ptPubOutput | ptOutput);
    const char* propsList = Model_GetComponentPropertiesLookupList(fullName.toStdString().c_str(), mask);
    if(propsList)
    {
        QStringList props = QString::fromUtf8(propsList).split(",", Qt::SkipEmptyParts);
        for(const QString& prop : props)
        {
            QStringList parts = prop.split(":");
            if(parts.size() >= 1)
            {
                QString propName = parts[0].trimmed();
                Port port;
                port.isInput = isInput;
                port.name = propName;
                port.componentName = nodeName;
                port.fullPath = propName;
                port.displayName = nodeName + "." + propName;
                result.append(port);
            }
        }
        Engine_FreeBufString(propsList);
    }
    
    // Добавляем алиасы свойств из UNet (если компонент является UNet)
    try
    {
        RDK::UEPtr<RDK::UContainer> model = RDK::GetModel();
        if(model)
        {
            RDK::UEPtr<RDK::UContainer> component;
            if(fullName.isEmpty())
                component = model;
            else
                component = model->GetComponentL(fullName.toStdString(), true);
            
            if(component)
            {
                RDK::UEPtr<RDK::UNet> net = RDK::dynamic_pointer_cast<RDK::UNet>(component);
                if(net)
                {
                    // Получаем алиасы нужного типа
                    // Константы типов свойств определены в rdk_init.h в глобальном пространстве имен
                    unsigned int aliasTypeMask = isInput ? (ptInput | ptPubInput) : (ptOutput | ptPubOutput);
                    std::vector<RDK::UPropertyAlias> aliases = net->GetPropertyAliasesByType(aliasTypeMask);
                    
                    for(const auto& alias : aliases)
                    {
                        Port port;
                        port.isInput = isInput;
                        port.name = QString::fromStdString(alias.AliasName);
                        port.componentName = nodeName;
                        // Для алиаса fullPath содержит полный путь к свойству
                        port.fullPath = QString::fromStdString(alias.GetFullPropertyPath());
                        port.displayName = QString::fromStdString(alias.AliasName) + " [Alias]";
                        result.append(port);
                    }
                }
            }
        }
    }
    catch(...)
    {
        // Игнорируем ошибки при получении алиасов
    }
    
    // Если нужно включить вложенные порты
    if(includeNested)
    {
        const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
        if(compList)
        {
            QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
            for(const QString& comp : components)
            {
                QString nestedFullName = fullName + "." + comp;
                const char* nestedProps = Model_GetComponentPropertiesLookupList(
                    nestedFullName.toStdString().c_str(), mask);
                if(nestedProps)
                {
                    QStringList nestedPropsList = QString::fromUtf8(nestedProps).split(",", Qt::SkipEmptyParts);
                    for(const QString& prop : nestedPropsList)
                    {
                        QStringList parts = prop.split(":");
                        if(parts.size() >= 1)
                        {
                            QString propName = parts[0].trimmed();
                            Port port;
                            port.isInput = isInput;
                            port.name = propName;
                            port.componentName = comp;
                            port.fullPath = comp + "." + propName;
                            port.displayName = nodeName + "." + comp + "." + propName;
                            result.append(port);
                        }
                    }
                    Engine_FreeBufString(nestedProps);
                }
            }
            Engine_FreeBufString(compList);
        }
    }
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getOwnOutputPorts() const
{
    // ОПТИМИЗАЦИЯ: используем кэш для избежания повторных вызовов API ядра
    if(m_portsCacheValid && !m_cachedOwnOutputPorts.isEmpty())
    {
        return m_cachedOwnOutputPorts;
    }
    
    QVector<Port> result;
    if(!m_owner || !m_owner->m_application)
        return result;
    
    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
    
    // Получаем собственные выходные свойства компонента (без точки в пути)
    const char* outputProps = Model_GetComponentPropertiesLookupList(
        fullName.toStdString().c_str(), ptPubOutput | ptOutput);
    if(outputProps)
    {
        QStringList outputList = QString::fromUtf8(outputProps).split(",", Qt::SkipEmptyParts);
        for(const QString& prop : outputList)
        {
            QStringList parts = prop.split(":");
            if(parts.size() >= 1)
            {
                QString propName = parts[0].trimmed();
                // Собственные свойства не содержат точки в пути
                if(!propName.contains('.'))
                {
                    Port port;
                    port.isInput = false;
                    port.name = propName;
                    port.componentName = nodeName;
                    port.fullPath = propName;
                    port.displayName = propName;
                    port.category = PortCategory::Own;
                    result.append(port);
                }
            }
        }
        Engine_FreeBufString(outputProps);
    }
    
    // Сохраняем в кэш
    m_cachedOwnOutputPorts = result;
    m_portsCacheValid = true;
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getChildOutputPorts() const
{
    // ОПТИМИЗАЦИЯ: используем кэш для избежания повторных вызовов API ядра
    if(m_portsCacheValid && !m_cachedChildOutputPorts.isEmpty())
    {
        return m_cachedChildOutputPorts;
    }
    
    QVector<Port> result;
    if(!m_owner || !m_owner->m_application)
        return result;
    
    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
    
    // Получаем список дочерних компонентов
    const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
    if(compList)
    {
        QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
        for(const QString& comp : components)
        {
            QString nestedFullName = fullName + "." + comp;
            const char* nestedProps = Model_GetComponentPropertiesLookupList(
                nestedFullName.toStdString().c_str(), ptPubOutput | ptOutput);
            if(nestedProps)
            {
                QStringList nestedPropsList = QString::fromUtf8(nestedProps).split(",", Qt::SkipEmptyParts);
                for(const QString& prop : nestedPropsList)
                {
                    QStringList parts = prop.split(":");
                    if(parts.size() >= 1)
                    {
                        QString propName = parts[0].trimmed();
                        Port port;
                        port.isInput = false;
                        port.name = propName;
                        port.componentName = comp;
                        port.fullPath = comp + "." + propName;
                        port.displayName = nodeName + "." + comp + "." + propName;
                        port.category = PortCategory::Child;
                        result.append(port);
                    }
                }
                Engine_FreeBufString(nestedProps);
            }
        }
        Engine_FreeBufString(compList);
    }
    
    // Сохраняем в кэш
    m_cachedChildOutputPorts = result;
    m_portsCacheValid = true;
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getAliasOutputPorts() const
{
    // ОПТИМИЗАЦИЯ: используем кэш для избежания повторных вызовов API ядра
    if(m_portsCacheValid && !m_cachedAliasOutputPorts.isEmpty())
    {
        return m_cachedAliasOutputPorts;
    }
    
    QVector<Port> result;
    if(!m_owner || !m_owner->m_application)
        return result;
    
    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
    
    // Получаем алиасы свойств из UNet (если компонент является UNet)
    try
    {
        RDK::UEPtr<RDK::UContainer> model = RDK::GetModel();
        if(model)
        {
            RDK::UEPtr<RDK::UContainer> component;
            if(fullName.isEmpty())
                component = model;
            else
                component = model->GetComponentL(fullName.toStdString(), true);
            
            if(component)
            {
                RDK::UEPtr<RDK::UNet> net = RDK::dynamic_pointer_cast<RDK::UNet>(component);
                if(net)
                {
                    // Получаем алиасы выходных свойств
                    unsigned int aliasTypeMask = ptOutput | ptPubOutput;
                    std::vector<RDK::UPropertyAlias> aliases = net->GetPropertyAliasesByType(aliasTypeMask);
                    
                    for(const auto& alias : aliases)
                    {
                        Port port;
                        port.isInput = false;
                        port.name = QString::fromStdString(alias.AliasName);
                        port.componentName = nodeName;
                        // Для алиаса fullPath содержит полный путь к свойству
                        port.fullPath = QString::fromStdString(alias.GetFullPropertyPath());
                        port.displayName = QString::fromStdString(alias.AliasName) + " [Alias]";
                        port.category = PortCategory::Alias;
                        result.append(port);
                    }
                }
            }
        }
    }
    catch(...)
    {
        // Игнорируем ошибки при получении алиасов
    }
    
    // Сохраняем в кэш
    m_cachedAliasOutputPorts = result;
    m_portsCacheValid = true;
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getOwnInputPorts() const
{
    // ОПТИМИЗАЦИЯ: сначала проверяем локальный кэш
    if(m_portsCacheValid && !m_cachedOwnInputPorts.isEmpty())
    {
        return m_cachedOwnInputPorts;
    }
    
    // ОПТИМИЗАЦИЯ: проверяем глобальный кэш компонентов
    QString fullName = m_owner ? (m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName) : nodeName;
    ComponentCacheEntry* cacheEntry = m_owner ? m_owner->m_componentCache.getEntry(fullName) : nullptr;
    if(cacheEntry && !cacheEntry->ownInputPorts.isEmpty())
    {
        // Восстанавливаем из глобального кэша в локальный
        m_cachedOwnInputPorts = cacheEntry->ownInputPorts;
        m_portsCacheValid = true;
        return m_cachedOwnInputPorts;
    }
    
    QVector<Port> result;
    if(!m_owner || !m_owner->m_application)
        return result;
    
    // Получаем собственные входные свойства компонента (без точки в пути)
    const char* inputProps = Model_GetComponentPropertiesLookupList(
        fullName.toStdString().c_str(), ptPubInput | ptInput);
    if(inputProps)
    {
        QStringList inputList = QString::fromUtf8(inputProps).split(",", Qt::SkipEmptyParts);
        for(const QString& prop : inputList)
        {
            QStringList parts = prop.split(":");
            if(parts.size() >= 1)
            {
                QString propName = parts[0].trimmed();
                // Собственные свойства не содержат точки в пути
                if(!propName.contains('.'))
                {
                    Port port;
                    port.isInput = true;
                    port.name = propName;
                    port.componentName = nodeName;
                    port.fullPath = propName;
                    port.displayName = propName;
                    port.category = PortCategory::Own;
                    result.append(port);
                }
            }
        }
        Engine_FreeBufString(inputProps);
    }
    
    // Сохраняем в локальный кэш
    m_cachedOwnInputPorts = result;
    m_portsCacheValid = true;
    
    // Сохраняем в глобальный кэш
    if(m_owner)
    {
        if(!cacheEntry)
        {
            ComponentCacheEntry newEntry;
            newEntry.ownInputPorts = result;
            newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
            m_owner->m_componentCache.setEntry(fullName, newEntry);
        }
        else
        {
            cacheEntry->ownInputPorts = result;
            cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
        }
    }
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getChildInputPorts() const
{
    // ОПТИМИЗАЦИЯ: используем кэш для избежания повторных вызовов API ядра
    if(m_portsCacheValid && !m_cachedChildInputPorts.isEmpty())
    {
        return m_cachedChildInputPorts;
    }
    
    QVector<Port> result;
    if(!m_owner || !m_owner->m_application)
        return result;
    
    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
    
    // Получаем список дочерних компонентов
    const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
    if(compList)
    {
        QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
        for(const QString& comp : components)
        {
            QString nestedFullName = fullName + "." + comp;
            const char* nestedProps = Model_GetComponentPropertiesLookupList(
                nestedFullName.toStdString().c_str(), ptPubInput | ptInput);
            if(nestedProps)
            {
                QStringList nestedPropsList = QString::fromUtf8(nestedProps).split(",", Qt::SkipEmptyParts);
                for(const QString& prop : nestedPropsList)
                {
                    QStringList parts = prop.split(":");
                    if(parts.size() >= 1)
                    {
                        QString propName = parts[0].trimmed();
                        Port port;
                        port.isInput = true;
                        port.name = propName;
                        port.componentName = comp;
                        port.fullPath = comp + "." + propName;
                        port.displayName = nodeName + "." + comp + "." + propName;
                        port.category = PortCategory::Child;
                        result.append(port);
                    }
                }
                Engine_FreeBufString(nestedProps);
            }
        }
        Engine_FreeBufString(compList);
    }
    
    // Сохраняем в кэш
    m_cachedChildInputPorts = result;
    m_portsCacheValid = true;
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getAliasInputPorts() const
{
    // ОПТИМИЗАЦИЯ: используем кэш для избежания повторных вызовов API ядра
    if(m_portsCacheValid && !m_cachedAliasInputPorts.isEmpty())
    {
        return m_cachedAliasInputPorts;
    }
    
    QVector<Port> result;
    if(!m_owner || !m_owner->m_application)
        return result;
    
    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
    
    // Получаем алиасы свойств из UNet (если компонент является UNet)
    try
    {
        RDK::UEPtr<RDK::UContainer> model = RDK::GetModel();
        if(model)
        {
            RDK::UEPtr<RDK::UContainer> component;
            if(fullName.isEmpty())
                component = model;
            else
                component = model->GetComponentL(fullName.toStdString(), true);
            
            if(component)
            {
                RDK::UEPtr<RDK::UNet> net = RDK::dynamic_pointer_cast<RDK::UNet>(component);
                if(net)
                {
                    // Получаем алиасы входных свойств
                    unsigned int aliasTypeMask = ptInput | ptPubInput;
                    std::vector<RDK::UPropertyAlias> aliases = net->GetPropertyAliasesByType(aliasTypeMask);
                    
                    for(const auto& alias : aliases)
                    {
                        Port port;
                        port.isInput = true;
                        port.name = QString::fromStdString(alias.AliasName);
                        port.componentName = nodeName;
                        // Для алиаса fullPath содержит полный путь к свойству
                        port.fullPath = QString::fromStdString(alias.GetFullPropertyPath());
                        port.displayName = QString::fromStdString(alias.AliasName) + " [Alias]";
                        port.category = PortCategory::Alias;
                        result.append(port);
                    }
                }
            }
        }
    }
    catch(...)
    {
        // Игнорируем ошибки при получении алиасов
    }
    
    // Сохраняем в кэш
    m_cachedAliasInputPorts = result;
    m_portsCacheValid = true;
    
    return result;
}

bool UModernDiagramWidget::NodeItem::hasConnectionsToInputCategory(PortCategory category) const
{
    if(!m_owner || !m_owner->m_application)
        return false;
    
    // ОПТИМИЗАЦИЯ: используем m_connectedLinks для быстрой проверки вместо вызова API
    // Это критично для производительности - вызов Model_GetComponentInternalLinks очень медленный
    bool hasConnection = false;
    bool usedFastPath = false;
    int linksChecked = 0;
    int linksWithCategories = 0;
    int linksWithoutCategories = 0;
    QList<PortCategory> foundCategories;  // Для отладки: собираем все найденные категории
    
    // ДЕТАЛЬНОЕ ЛОГИРОВАНИЕ для диагностики PGenerator
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // bool isPGenerator = (nodeName == "PGenerator");
    // if(isPGenerator)
    // {
    //     QString componentDisplayName = m_owner && !m_owner->m_componentName.isEmpty() ? m_owner->m_componentName : "root";
    //     QString categoryStr = category == PortCategory::Own ? "Own" : (category == PortCategory::Child ? "Child" : "Alias");
    //     QString logMsg = QString("[UModernDiagramWidget] Component: %1, NodeItem: %2, hasConnectionsToInputCategory(%3): starting check, m_connectedLinks.size()=%4")
    //         .arg(componentDisplayName).arg(nodeName).arg(categoryStr).arg(m_connectedLinks.size());
    //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    // }
    bool isPGenerator = false;  // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    
    for(LinkItem* link : m_connectedLinks)
    {
        if(!link || link->dst() != this)
            continue;  // Пропускаем связи, где этот узел не является получателем
        
        linksChecked++;
        
        // Проверяем категорию целевого порта
        if(link->hasCategories())
        {
            linksWithCategories++;
            PortCategory cat = link->dstCategory();
            if(!foundCategories.contains(cat))
                foundCategories.append(cat);
            if(cat == category)
            {
                hasConnection = true;
                usedFastPath = true;
                // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
                // if(isPGenerator)
                // {
                //     QString componentDisplayName = m_owner && !m_owner->m_componentName.isEmpty() ? m_owner->m_componentName : "root";
                //     QString categoryStr = category == PortCategory::Own ? "Own" : (category == PortCategory::Child ? "Child" : "Alias");
                //     QString logMsg = QString("[UModernDiagramWidget] Component: %1, NodeItem: %2, hasConnectionsToInputCategory(%3): FOUND via fast path (link hasCategories=true, dstCategory=%4)")
                //         .arg(componentDisplayName).arg(nodeName).arg(categoryStr).arg(cat == PortCategory::Own ? "Own" : (cat == PortCategory::Child ? "Child" : "Alias"));
                //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
                // }
                break;
            }
        }
        // Если категория не задана, но есть связь к входному порту, считаем что есть соединение
        // (для обратной совместимости со старыми связями)
        else if(!link->hasCategories() && link->useInput())
        {
            linksWithoutCategories++;
            // Для старых связей без категорий используем быструю проверку через кэшированные порты
            // Это быстрее, чем вызов API
            bool hasPorts = false;
            if(category == PortCategory::Own)
            {
                hasPorts = !m_cachedOwnInputPorts.isEmpty();
            }
            else if(category == PortCategory::Child)
            {
                hasPorts = !m_cachedChildInputPorts.isEmpty();
            }
            else if(category == PortCategory::Alias)
            {
                hasPorts = !m_cachedAliasInputPorts.isEmpty();
            }
            
            if(hasPorts)
            {
                hasConnection = true;
                usedFastPath = true;
                break;
            }
        }
    }
    
    // Если нашли соединение через быструю проверку, возвращаем результат
    if(hasConnection)
    {
        return true;
    }

    // ОПТИМИЗАЦИЯ: если вообще нет входящих связей для этого узла, то нет смысла запускать медленный fallback по XML.
    // Это особенно важно для узлов вроде PGenerator, у которых нет входов: linksChecked == 0 и fallback только тратит время.
    if(linksChecked == 0)
    {
        return false;
    }
    
    // ОПТИМИЗАЦИЯ: если все связи имеют категории, но ни одна из них не совпадает с проверяемой категорией,
    // то соединений для этой категории точно нет - возвращаем false БЕЗ вызова медленного fallback
    bool earlyExitCondition = (linksChecked > 0 && linksWithoutCategories == 0 && !foundCategories.contains(category));
    if(earlyExitCondition)
    {
        // Все связи имеют категории, но ни одна из них не совпадает с проверяемой категорией
        // Значит, соединений для этой категории нет - возвращаем false без вызова API
        // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
        // if(isPGenerator)
        // {
        //     QString componentDisplayName = m_owner && !m_owner->m_componentName.isEmpty() ? m_owner->m_componentName : "root";
        //     QString categoryStr = category == PortCategory::Own ? "Own" : (category == PortCategory::Child ? "Child" : "Alias");
        //     QString foundCategoriesStr;
        //     for(PortCategory cat : foundCategories)
        //     {
        //         if(!foundCategoriesStr.isEmpty()) foundCategoriesStr += ",";
        //         if(cat == PortCategory::Own) foundCategoriesStr += "Own";
        //         else if(cat == PortCategory::Child) foundCategoriesStr += "Child";
        //         else if(cat == PortCategory::Alias) foundCategoriesStr += "Alias";
        //     }
        //     QString logMsg = QString("[UModernDiagramWidget] Component: %1, NodeItem: %2, hasConnectionsToInputCategory(%3): early exit (all %4 links have categories, but none match, foundCategories: [%5])")
        //         .arg(componentDisplayName).arg(nodeName).arg(categoryStr).arg(linksChecked).arg(foundCategoriesStr);
        //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        // }
        return false;
    }
    
    // Логируем информацию о быстрой проверке (только если она не сработала и fallback будет вызван)
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // if(!usedFastPath && linksChecked > 0)
    // {
    //     QString componentDisplayName = m_owner && !m_owner->m_componentName.isEmpty() ? m_owner->m_componentName : "root";
    //     QString categoryStr = category == PortCategory::Own ? "Own" : (category == PortCategory::Child ? "Child" : "Alias");
    //     QString foundCategoriesStr;
    //     for(PortCategory cat : foundCategories)
    //     {
    //         if(!foundCategoriesStr.isEmpty()) foundCategoriesStr += ",";
    //         if(cat == PortCategory::Own) foundCategoriesStr += "Own";
    //         else if(cat == PortCategory::Child) foundCategoriesStr += "Child";
    //         else if(cat == PortCategory::Alias) foundCategoriesStr += "Alias";
    //     }
    //     QString earlyExitReason;
    //     if(linksChecked == 0)
    //         earlyExitReason = "no links checked";
    //     else if(linksWithoutCategories > 0)
    //         earlyExitReason = QString("has %1 links without categories").arg(linksWithoutCategories);
    //     else if(foundCategories.contains(category))
    //         earlyExitReason = QString("found category %1 in links").arg(categoryStr);
    //     else
    //         earlyExitReason = "unknown reason";
    //     
    //     QString logMsg = QString("[UModernDiagramWidget] Component: %1, NodeItem: %2, hasConnectionsToInputCategory(%3): fast path failed (checked %4 links, withCategories: %5, withoutCategories: %6, foundCategories: [%7], earlyExitCondition: %8, reason: %9), using fallback")
    //         .arg(componentDisplayName).arg(nodeName).arg(categoryStr).arg(linksChecked).arg(linksWithCategories).arg(linksWithoutCategories).arg(foundCategoriesStr).arg(earlyExitCondition ? "true" : "false").arg(earlyExitReason);
    //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    // }
    
    // Fallback: если быстрая проверка не дала результата, используем старый метод
    // (для старых связей без категорий или для сложных случаев)
    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
    
    // Получаем список портов категории
    QVector<Port> categoryPorts;
    if(category == PortCategory::Own)
    {
        categoryPorts = getOwnInputPorts();
    }
    else if(category == PortCategory::Child)
    {
        categoryPorts = getChildInputPorts();
    }
    else if(category == PortCategory::Alias)
    {
        categoryPorts = getAliasInputPorts();
    }
    
    // Для категории Own не возвращаем false сразу, если портов нет
    // Соединения могут быть к свойствам, которые не отображаются в списке портов
    if(categoryPorts.isEmpty() && category != PortCategory::Own)
        return false;
    
    // Получаем все связи компонента
    const char* xmlRaw = Model_GetComponentInternalLinks(m_owner->m_componentName.toStdString().c_str(), nullptr);
    if(!xmlRaw)
        return false;
    
    std::string raw(xmlRaw ? xmlRaw : "");
    Engine_FreeBufString(xmlRaw);
    
    if(raw.empty())
        return false;
    
    RDK::USerStorageXML xml;
    if(!xml.Load(raw, "Links"))
        return false;
    
    RDK::UStringLinksList linkslist;
    xml >> linkslist;
    
    // Проверяем, есть ли связи к портам категории
    for(int i = 0; i < linkslist.GetSize(); ++i)
    {
        const auto& link = linkslist[i];
        for(size_t c = 0; c < link.Connector.size(); ++c)
        {
            const auto& connSide = link.Connector[c];
            QString connName = QString::fromStdString(connSide.Name);
            QString connId = QString::fromStdString(connSide.Id);
            
            // Определяем, относится ли соединение к нашему компоненту
            // connId содержит полный путь компонента, например "ComponentName" или "Parent.ComponentName"
            bool isOurComponent = false;
            
            if(m_owner->m_componentName.isEmpty())
            {
                // Для верхнего уровня (m_componentName пустое)
                // connId может быть просто именем компонента или полным путем
                isOurComponent = (connId == nodeName || 
                                 connId.endsWith("." + nodeName) ||
                                 connId.startsWith(nodeName + ".") ||
                                 connId == nodeName);
            }
            else
            {
                // Для вложенных уровней
                QString targetComponentName = m_owner->m_componentName + "." + nodeName;
                isOurComponent = (connId == targetComponentName || 
                                 connId.endsWith("." + nodeName) ||
                                 connId == nodeName);
            }
            
            if(isOurComponent)
            {
                // Извлекаем имя свойства из connName
                // connName может быть просто именем свойства или "Component.Property"
                QString propName = connName;
                
                if(m_owner->m_componentName.isEmpty())
                {
                    // Для верхнего уровня
                    // connName может быть просто именем свойства (например, "Input") 
                    // или "Component.Property" (например, "Neuron.Input")
                    if(propName.contains('.'))
                    {
                        QStringList parts = propName.split('.');
                        // Если первая часть совпадает с именем компонента, берем последнюю часть
                        if(parts.size() >= 2 && parts.first() == nodeName)
                        {
                            propName = parts.last();
                        }
                        else
                        {
                            // Иначе это может быть дочерний компонент, берем последнюю часть
                            propName = parts.last();
                        }
                    }
                    // Если нет точки, это просто имя свойства
                }
                else
                {
                    // Для вложенных уровней
                    if(propName.contains('.'))
                    {
                        QStringList parts = propName.split('.');
                        // Если последняя часть совпадает с именем компонента, берем предпоследнюю
                        if(parts.size() >= 2 && parts.last() == nodeName)
                        {
                            propName = parts[parts.size() - 2];
                        }
                        else
                        {
                            propName = parts.last();
                        }
                    }
                }
                
                // Проверяем, относится ли свойство к категории
                // Сравниваем с полным путем свойства и именем
                for(const Port& port : categoryPorts)
                {
                    if(category == PortCategory::Child)
                    {
                        // Для дочерних компонентов connName должен содержать путь к дочернему компоненту
                        // Например: "SubComponent.Input" или "Neuron.SynapticInputs"
                        if(connName.contains(port.componentName + "." + propName) ||
                           connName == port.componentName + "." + propName ||
                           connName.endsWith("." + port.componentName + "." + propName) ||
                           (connName.contains('.') && connName.startsWith(port.componentName + ".")))
                        {
                            // Дополнительная проверка: убеждаемся, что это действительно дочерний компонент
                            if(port.fullPath.contains(port.componentName + "."))
                            {
                                return true;
                            }
                        }
                    }
                    else if(category == PortCategory::Own)
                    {
                        // Для собственных свойств connName не должен содержать точку (или быть просто именем свойства)
                        // И имя должно совпадать
                        if(port.name == propName)
                        {
                            // Если connName не содержит точки, это собственное свойство
                            if(!connName.contains('.'))
                            {
                                return true;
                            }
                            // Если содержит точку, но это не путь к дочернему компоненту (например, "Component.Property" где Component == nodeName)
                            if(connName.contains('.'))
                            {
                                QStringList parts = connName.split('.');
                                // Если первая часть - это имя компонента, а вторая - имя свойства, это собственное свойство
                                if(parts.size() == 2 && parts.first() == nodeName && parts.last() == propName)
                                {
                                    return true;
                                }
                            }
                        }
                        // Проверяем полный путь
                        if(port.fullPath == connName || port.fullPath == propName)
                        {
                            return true;
                        }
                    }
                    else if(category == PortCategory::Alias)
                    {
                        // Для алиасов проверяем по имени и полному пути
                        if(port.name == propName || 
                           port.fullPath == connName ||
                           port.fullPath == propName ||
                           port.fullPath.endsWith("." + propName))
                        {
                            return true;
                        }
                    }
                }
                
                // Для категории Own: если портов нет, но connName соответствует формату Own, считаем соединение найденным
                if(category == PortCategory::Own && categoryPorts.isEmpty())
                {
                    // Проверяем формат connName: если не содержит точки или имеет формат nodeName.Property, это Own
                    if(!connName.contains('.'))
                    {
                        return true;
                    }
                    if(connName.contains('.'))
                    {
                        QStringList parts = connName.split('.');
                        // Если первая часть - это имя компонента, и это не путь к дочернему компоненту
                        if(parts.size() >= 2 && parts.first() == nodeName)
                        {
                            // Проверяем, является ли вторая часть дочерним компонентом
                            QString inner_fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
                            const char* compList = Model_GetComponentsNameList(inner_fullName.toStdString().c_str());
                            if(compList)
                            {
                                QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
                                Engine_FreeBufString(compList);
                                
                                // Если вторая часть НЕ является дочерним компонентом, это собственное свойство
                                if(!components.contains(parts[1]))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
                
                // Для категории Child: проверяем многоуровневые пути
                if(category == PortCategory::Child)
                {
                    // connName может иметь формат nodeName.ChildComponent... или просто ChildComponent...
                    // Проверяем, начинается ли connName с пути к дочернему компоненту
                    QString checkName = connName;
                    if(checkName.startsWith(nodeName + "."))
                    {
                        checkName = checkName.mid(nodeName.length() + 1);
                    }
                    
                    // Проверяем, начинается ли checkName с имени дочернего компонента
                    for(const Port& port : categoryPorts)
                    {
                        if(checkName.startsWith(port.componentName + ".") || 
                           checkName == port.componentName ||
                           connName.contains(port.componentName + "."))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

bool UModernDiagramWidget::NodeItem::hasConnectionsToOutputCategory(PortCategory category) const
{
    if(!m_owner || !m_owner->m_application)
        return false;
    
    // ОПТИМИЗАЦИЯ: используем m_connectedLinks для быстрой проверки вместо вызова API
    // Это критично для производительности - вызов Model_GetComponentInternalLinks очень медленный
    bool hasConnection = false;
    bool usedFastPath = false;
    int linksChecked = 0;
    int linksWithCategories = 0;
    int linksWithoutCategories = 0;
    QList<PortCategory> foundCategories;  // Для отладки: собираем все найденные категории
    
    // ДЕТАЛЬНОЕ ЛОГИРОВАНИЕ для диагностики PGenerator
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // bool isPGenerator = (nodeName == "PGenerator");
    // if(isPGenerator)
    // {
    //     QString componentDisplayName = m_owner && !m_owner->m_componentName.isEmpty() ? m_owner->m_componentName : "root";
    //     QString categoryStr = category == PortCategory::Own ? "Own" : (category == PortCategory::Child ? "Child" : "Alias");
    //     QString logMsg = QString("[UModernDiagramWidget] Component: %1, NodeItem: %2, hasConnectionsToOutputCategory(%3): starting check, m_connectedLinks.size()=%4")
    //         .arg(componentDisplayName).arg(nodeName).arg(categoryStr).arg(m_connectedLinks.size());
    //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    // }
    bool isPGenerator = false;  // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    
    for(LinkItem* link : m_connectedLinks)
    {
        if(!link || link->src() != this)
            continue;  // Пропускаем связи, где этот узел не является источником
        
        linksChecked++;
        
        // Проверяем категорию исходного порта
        if(link->hasCategories())
        {
            linksWithCategories++;
            PortCategory cat = link->srcCategory();
            if(!foundCategories.contains(cat))
                foundCategories.append(cat);
            if(cat == category)
            {
                hasConnection = true;
                usedFastPath = true;
                // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
                // if(isPGenerator)
                // {
                //     QString componentDisplayName = m_owner && !m_owner->m_componentName.isEmpty() ? m_owner->m_componentName : "root";
                //     QString categoryStr = category == PortCategory::Own ? "Own" : (category == PortCategory::Child ? "Child" : "Alias");
                //     QString logMsg = QString("[UModernDiagramWidget] Component: %1, NodeItem: %2, hasConnectionsToOutputCategory(%3): FOUND via fast path (link hasCategories=true, srcCategory=%4)")
                //         .arg(componentDisplayName).arg(nodeName).arg(categoryStr).arg(cat == PortCategory::Own ? "Own" : (cat == PortCategory::Child ? "Child" : "Alias"));
                //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
                // }
                break;
            }
        }
        // Внешние связи (где dstNode == nullptr) также имеют категории и должны учитываться
        // Такие связи важны для узлов типа PGenerator, чьи выходы ведут к внешним компонентам
        // Если категория не задана, но есть связь от выходного порта, считаем что есть соединение
        // (для обратной совместимости со старыми связями)
        else if(!link->hasCategories() && link->useOutput())
        {
            linksWithoutCategories++;
            // Для старых связей без категорий используем быструю проверку через кэшированные порты
            // Это быстрее, чем вызов API
            bool hasPorts = false;
            if(category == PortCategory::Own)
            {
                hasPorts = !m_cachedOwnOutputPorts.isEmpty();
            }
            else if(category == PortCategory::Child)
            {
                hasPorts = !m_cachedChildOutputPorts.isEmpty();
            }
            else if(category == PortCategory::Alias)
            {
                hasPorts = !m_cachedAliasOutputPorts.isEmpty();
            }
            
            if(hasPorts)
            {
                hasConnection = true;
                usedFastPath = true;
                break;
            }
        }
    }
    
    // Если нашли соединение через быструю проверку, возвращаем результат
    if(hasConnection)
    {
        return true;
    }
    
    // ОПТИМИЗАЦИЯ: если все связи имеют категории, но ни одна из них не совпадает с проверяемой категорией,
    // то соединений для этой категории точно нет - возвращаем false БЕЗ вызова медленного fallback
    bool earlyExitCondition = (linksChecked > 0 && linksWithoutCategories == 0 && !foundCategories.contains(category));
    if(earlyExitCondition)
    {
        // Все связи имеют категории, но ни одна из них не совпадает с проверяемой категорией
        // Значит, соединений для этой категории нет - возвращаем false без вызова API
        // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
        // if(isPGenerator)
        // {
        //     QString componentDisplayName = m_owner && !m_owner->m_componentName.isEmpty() ? m_owner->m_componentName : "root";
        //     QString categoryStr = category == PortCategory::Own ? "Own" : (category == PortCategory::Child ? "Child" : "Alias");
        //     QString foundCategoriesStr;
        //     for(PortCategory cat : foundCategories)
        //     {
        //         if(!foundCategoriesStr.isEmpty()) foundCategoriesStr += ",";
        //         if(cat == PortCategory::Own) foundCategoriesStr += "Own";
        //         else if(cat == PortCategory::Child) foundCategoriesStr += "Child";
        //         else if(cat == PortCategory::Alias) foundCategoriesStr += "Alias";
        //     }
        //     QString logMsg = QString("[UModernDiagramWidget] Component: %1, NodeItem: %2, hasConnectionsToOutputCategory(%3): early exit (all %4 links have categories, but none match, foundCategories: [%5])")
        //         .arg(componentDisplayName).arg(nodeName).arg(categoryStr).arg(linksChecked).arg(foundCategoriesStr);
        //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        // }
        return false;
    }
    
    // Логируем информацию о быстрой проверке (только если она не сработала и fallback будет вызван)
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // if(!usedFastPath && linksChecked > 0)
    // {
    //     QString componentDisplayName = m_owner && !m_owner->m_componentName.isEmpty() ? m_owner->m_componentName : "root";
    //     QString categoryStr = category == PortCategory::Own ? "Own" : (category == PortCategory::Child ? "Child" : "Alias");
    //     QString foundCategoriesStr;
    //     for(PortCategory cat : foundCategories)
    //     {
    //         if(!foundCategoriesStr.isEmpty()) foundCategoriesStr += ",";
    //         if(cat == PortCategory::Own) foundCategoriesStr += "Own";
    //         else if(cat == PortCategory::Child) foundCategoriesStr += "Child";
    //         else if(cat == PortCategory::Alias) foundCategoriesStr += "Alias";
    //     }
    //     QString earlyExitReason;
    //     if(linksChecked == 0)
    //         earlyExitReason = "no links checked";
    //     else if(linksWithoutCategories > 0)
    //         earlyExitReason = QString("has %1 links without categories").arg(linksWithoutCategories);
    //     else if(foundCategories.contains(category))
    //         earlyExitReason = QString("found category %1 in links").arg(categoryStr);
    //     else
    //         earlyExitReason = "unknown reason";
    //     
    //     QString logMsg = QString("[UModernDiagramWidget] Component: %1, NodeItem: %2, hasConnectionsToOutputCategory(%3): fast path failed (checked %4 links, withCategories: %5, withoutCategories: %6, foundCategories: [%7], earlyExitCondition: %8, reason: %9), using fallback")
    //         .arg(componentDisplayName).arg(nodeName).arg(categoryStr).arg(linksChecked).arg(linksWithCategories).arg(linksWithoutCategories).arg(foundCategoriesStr).arg(earlyExitCondition ? "true" : "false").arg(earlyExitReason);
    //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    // }
    
    // Fallback: если быстрая проверка не дала результата, используем старый метод
    // (для старых связей без категорий или для сложных случаев)
    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
    
    // Получаем список портов категории
    QVector<Port> categoryPorts;
    if(category == PortCategory::Own)
    {
        categoryPorts = getOwnOutputPorts();
    }
    else if(category == PortCategory::Child)
    {
        categoryPorts = getChildOutputPorts();
    }
    else if(category == PortCategory::Alias)
    {
        categoryPorts = getAliasOutputPorts();
    }
    
    // Для категории Own не возвращаем false сразу, если портов нет
    // Соединения могут быть к свойствам, которые не отображаются в списке портов
    if(categoryPorts.isEmpty() && category != PortCategory::Own)
        return false;
    
    // Получаем все связи компонента
    const char* xmlRaw = Model_GetComponentInternalLinks(m_owner->m_componentName.toStdString().c_str(), nullptr);
    if(!xmlRaw)
        return false;
    
    std::string raw(xmlRaw ? xmlRaw : "");
    Engine_FreeBufString(xmlRaw);
    
    if(raw.empty())
        return false;
    
    RDK::USerStorageXML xml;
    if(!xml.Load(raw, "Links"))
        return false;
    
    RDK::UStringLinksList linkslist;
    xml >> linkslist;
    
    // Проверяем, есть ли связи от портов категории
    for(int i = 0; i < linkslist.GetSize(); ++i)
    {
        const auto& link = linkslist[i];
        QString itemName = QString::fromStdString(link.Item.Name);
        QString itemId = QString::fromStdString(link.Item.Id);
        
        // Определяем, относится ли соединение к нашему компоненту
        // itemId содержит полный путь компонента, например "ComponentName" или "Parent.ComponentName"
        bool isOurComponent = false;
        
        if(m_owner->m_componentName.isEmpty())
        {
            // Для верхнего уровня (m_componentName пустое)
            // itemId может быть просто именем компонента или полным путем
            isOurComponent = (itemId == nodeName || 
                             itemId.endsWith("." + nodeName) ||
                             itemId.startsWith(nodeName + ".") ||
                             itemId == nodeName);
        }
        else
        {
            // Для вложенных уровней
            QString sourceComponentName = m_owner->m_componentName + "." + nodeName;
            isOurComponent = (itemId == sourceComponentName || 
                             itemId.endsWith("." + nodeName) ||
                             itemId == nodeName);
        }
        
        if(isOurComponent)
        {
            // Извлекаем имя свойства из itemName
            // itemName может быть просто именем свойства или "Component.Property"
            QString propName = itemName;
            
            if(m_owner->m_componentName.isEmpty())
            {
                // Для верхнего уровня
                // itemName может быть просто именем свойства (например, "Output") 
                // или "Component.Property" (например, "Neuron.Output")
                if(propName.contains('.'))
                {
                    QStringList parts = propName.split('.');
                    // Если первая часть совпадает с именем компонента, берем последнюю часть
                    if(parts.size() >= 2 && parts.first() == nodeName)
                    {
                        propName = parts.last();
                    }
                    else
                    {
                        // Иначе это может быть дочерний компонент, берем последнюю часть
                        propName = parts.last();
                    }
                }
                // Если нет точки, это просто имя свойства
            }
            else
            {
                // Для вложенных уровней
                if(propName.contains('.'))
                {
                    QStringList parts = propName.split('.');
                    // Если последняя часть совпадает с именем компонента, берем предпоследнюю
                    if(parts.size() >= 2 && parts.last() == nodeName)
                    {
                        propName = parts[parts.size() - 2];
                    }
                    else
                    {
                        propName = parts.last();
                    }
                }
            }
            
            // Проверяем, относится ли свойство к категории
            // Сравниваем с полным путем свойства и именем
            for(const Port& port : categoryPorts)
                {
                    if(category == PortCategory::Child)
                    {
                        // Для дочерних компонентов itemName должен содержать путь к дочернему компоненту
                        // Например: "SubComponent.Output" или "Neuron.Output"
                        if(itemName.contains(port.componentName + "." + propName) ||
                           itemName == port.componentName + "." + propName ||
                           itemName.endsWith("." + port.componentName + "." + propName) ||
                           (itemName.contains('.') && itemName.startsWith(port.componentName + ".")))
                        {
                            // Дополнительная проверка: убеждаемся, что это действительно дочерний компонент
                            if(port.fullPath.contains(port.componentName + "."))
                            {
                                return true;
                            }
                        }
                    }
                    else if(category == PortCategory::Own)
                    {
                        // Для собственных свойств itemName не должен содержать точку (или быть просто именем свойства)
                        // И имя должно совпадать
                        if(port.name == propName)
                        {
                            // Если itemName не содержит точки, это собственное свойство
                            if(!itemName.contains('.'))
                            {
                                return true;
                            }
                            // Если содержит точку, но это не путь к дочернему компоненту (например, "Component.Property" где Component == nodeName)
                            if(itemName.contains('.'))
                            {
                                QStringList parts = itemName.split('.');
                                // Если первая часть - это имя компонента, а вторая - имя свойства, это собственное свойство
                                if(parts.size() == 2 && parts.first() == nodeName && parts.last() == propName)
                                {
                                    return true;
                                }
                            }
                        }
                        // Проверяем полный путь
                        if(port.fullPath == itemName || port.fullPath == propName)
                        {
                            return true;
                        }
                    }
                    else if(category == PortCategory::Alias)
                    {
                        // Для алиасов проверяем по имени и полному пути
                        if(port.name == propName || 
                           port.fullPath == itemName ||
                           port.fullPath == propName ||
                           port.fullPath.endsWith("." + propName))
                        {
                            return true;
                        }
                    }
                }
                
                // Для категории Own: если портов нет, но itemName соответствует формату Own, считаем соединение найденным
                if(category == PortCategory::Own && categoryPorts.isEmpty())
                {
                    // Проверяем формат itemName: если не содержит точки или имеет формат nodeName.Property, это Own
                    if(!itemName.contains('.'))
                    {
                        return true;
                    }
                    if(itemName.contains('.'))
                    {
                        QStringList parts = itemName.split('.');
                        // Если первая часть - это имя компонента, и это не путь к дочернему компоненту
                        if(parts.size() >= 2 && parts.first() == nodeName)
                        {
                            // Проверяем, является ли вторая часть дочерним компонентом
                            QString inner_fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
                            const char* compList = Model_GetComponentsNameList(inner_fullName.toStdString().c_str());
                            if(compList)
                            {
                                QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
                                Engine_FreeBufString(compList);
                                
                                // Если вторая часть НЕ является дочерним компонентом, это собственное свойство
                                if(!components.contains(parts[1]))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
                
                // Для категории Child: проверяем многоуровневые пути
                if(category == PortCategory::Child && !categoryPorts.isEmpty())
                {
                    // itemName может иметь формат nodeName.ChildComponent... или просто ChildComponent...
                    // Проверяем, начинается ли itemName с пути к дочернему компоненту
                    QString checkName = itemName;
                    if(checkName.startsWith(nodeName + "."))
                    {
                        checkName = checkName.mid(nodeName.length() + 1);
                    }
                    
                    // Проверяем, начинается ли checkName с имени дочернего компонента
                    for(const Port& port : categoryPorts)
                    {
                        if(checkName.startsWith(port.componentName + ".") || 
                           checkName == port.componentName ||
                           itemName.contains(port.componentName + "."))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    
    return false;
}

void UModernDiagramWidget::NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverEnterEvent(event);
    m_hoveredPort = getPortAtPosition(event->pos());
    update(); // Только визуальная подсветка
    // Убрано: showPortListWidget - окно открывается только при клике
    
    // Set initial tooltip
    if(m_hoveredPort && m_owner)
    {
        QString tooltip = m_owner->generatePortTooltip(*m_hoveredPort);
        setToolTip(tooltip);
    }
    else if(m_owner)
    {
        QString tooltip = m_owner->generateNodeTooltip(this);
        setToolTip(tooltip);
    }
}

void UModernDiagramWidget::NodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverMoveEvent(event);
    
    // Throttling: обновляем только если позиция изменилась значительно (минимум 3 пикселя)
    const double minUpdateDistance = 3.0;
    QPointF currentPos = event->pos();
    bool positionChanged = m_lastHoverMovePos.isNull() || 
                          (QLineF(m_lastHoverMovePos, currentPos).length() > minUpdateDistance);
    
    if(!positionChanged)
    {
        // Позиция не изменилась значительно, только обновляем tooltip если нужно
        if(m_hoveredPort && m_owner)
        {
            QString tooltip = m_owner->generatePortTooltip(*m_hoveredPort);
            setToolTip(tooltip);
        }
        else if(m_owner)
        {
            QString tooltip = m_owner->generateNodeTooltip(this);
            setToolTip(tooltip);
        }
        return;
    }
    
    m_lastHoverMovePos = currentPos;
    
    const Port* port = getPortAtPosition(event->pos());
    bool portChanged = (port != m_hoveredPort);
    if(portChanged)
    {
        m_hoveredPort = port;
        update(); // Только визуальная подсветка
        // Убрано: showPortListWidget - окно открывается только при клике
    }
    
    // Set tooltip based on hover state
    if(m_hoveredPort && m_owner)
    {
        QString tooltip = m_owner->generatePortTooltip(*m_hoveredPort);
        setToolTip(tooltip);
    }
    else if(m_owner)
    {
        QString tooltip = m_owner->generateNodeTooltip(this);
        setToolTip(tooltip);
    }
}

void UModernDiagramWidget::NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
    m_hoveredPort = nullptr;
    m_lastHoverMovePos = QPointF(); // Сбрасываем позицию при выходе
    if(m_hideTimer)
        m_hideTimer->start();
    setToolTip(QString()); // Clear tooltip on leave
    update();
}

void UModernDiagramWidget::NodeItem::showPortListWidget(const QPointF& scenePos)
{
    if(!m_hoveredPort || !m_owner || !m_portListWidgetProxy)
        return;
    
    // Для категоризированных портов проверяем, есть ли порты для этой категории
    if(m_hoveredPort->fullPath.isEmpty() && 
       (m_hoveredPort->category == PortCategory::Own || 
        m_hoveredPort->category == PortCategory::Child || 
        m_hoveredPort->category == PortCategory::Alias))
    {
        bool hasPorts = false;
        if (m_hoveredPort->isInput)
        {
            // Проверка для входных портов
            if (m_hoveredPort->category == PortCategory::Own)
            {
                hasPorts = !getOwnInputPorts().isEmpty();
            }
            else if (m_hoveredPort->category == PortCategory::Child)
            {
                hasPorts = !getChildInputPorts().isEmpty();
            }
            else if (m_hoveredPort->category == PortCategory::Alias)
            {
                hasPorts = !getAliasInputPorts().isEmpty();
            }
        }
        else
        {
            // Проверка для выходных портов
            if (m_hoveredPort->category == PortCategory::Own)
            {
                hasPorts = !getOwnOutputPorts().isEmpty();
            }
            else if (m_hoveredPort->category == PortCategory::Child)
            {
                hasPorts = !getChildOutputPorts().isEmpty();
            }
            else if (m_hoveredPort->category == PortCategory::Alias)
            {
                hasPorts = !getAliasOutputPorts().isEmpty();
            }
        }
        
        // Если портов нет, не показываем дерево портов
        if (!hasPorts)
            return;
    }
    
    // Закрываем все открытые деревья портов других узлов перед открытием нового
    for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
    {
        if(node != this && node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
        {
            node->hidePortListWidget();
        }
    }
    
    if(m_hideTimer)
        m_hideTimer->stop();

    bool shiftPressed = QApplication::keyboardModifiers() & Qt::ShiftModifier;
    updatePortListWidget(m_hoveredPort->isInput, shiftPressed);
    
    // Позиционируем виджет рядом с курсором
    QPointF widgetPos = scenePos + QPointF(20, 20);
    
    // Проверяем границы viewport и корректируем позицию при необходимости
    if(m_owner && m_owner->m_mainView)
    {
        QRectF viewportRect = m_owner->m_mainView->mapToScene(m_owner->m_mainView->viewport()->rect()).boundingRect();
        QRectF widgetRect(widgetPos, QSizeF(m_portListWidget->width(), m_portListWidget->height()));
        
        // Если виджет выходит за правую границу, позиционируем слева от курсора
        if(widgetRect.right() > viewportRect.right())
        {
            widgetPos.setX(scenePos.x() - m_portListWidget->width() - 20);
        }
        
        // Если виджет выходит за нижнюю границу, позиционируем выше курсора
        if(widgetRect.bottom() > viewportRect.bottom())
        {
            widgetPos.setY(scenePos.y() - m_portListWidget->height() - 20);
        }
    }
    
    m_portListWidgetProxy->setPos(widgetPos);
    m_portListWidgetProxy->setVisible(true);
    
    // ИСПРАВЛЕНИЕ: Устанавливаем фокус для работы клавиатуры
    // Сначала устанавливаем фокус на прокси-виджет, затем на сам виджет
    m_portListWidgetProxy->setFocus();
    m_portListWidget->setFocus();
    
    // Принудительно обновляем сцену, чтобы окно стало видимым немедленно
    if(m_owner && m_owner->m_scene)
    {
        m_owner->m_scene->update();
    }
    
    // Устанавливаем текущий элемент, если есть
    if(m_portListWidget->topLevelItemCount() > 0)
    {
        QTreeWidgetItem* firstItem = m_portListWidget->topLevelItem(0);
        if(firstItem && firstItem->childCount() > 0)
        {
            m_portListWidget->setCurrentItem(firstItem->child(0));
            m_portListWidget->expandItem(firstItem);
        }
    }
}

void UModernDiagramWidget::NodeItem::hidePortListWidget()
{
    if(m_portListWidgetProxy)
    {
        m_portListWidgetProxy->setVisible(false);
    }
}

void UModernDiagramWidget::NodeItem::onPortItemActivated(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    
    // Логируем вход в функцию в самом начале (используем qDebug для гарантированного вывода)
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
        "onPortItemActivated: Function called", 0);
    
    if(!item || !m_owner)
    {
        QString errorMsg = QString("onPortItemActivated: Invalid item or owner (item=%1, owner=%2)")
            .arg(item ? "yes" : "no")
            .arg(m_owner ? "yes" : "no");
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, errorMsg.toStdString().c_str(), 0);
        return;
    }
    
    // Проверяем, что это элемент порта (не компонент)
    if(item->parent() == nullptr)
    {
        return; // Это элемент компонента, не порт
    }
    
    // Получаем данные порта
    QVariant data = item->data(0, Qt::UserRole);
    if(!data.isValid())
    {
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
            "onPortItemActivated: Port data is invalid", 0);
        return;
    }
    
    QMap<QString, QVariant> portData = data.value<QMap<QString, QVariant>>();
    bool isInput = portData["isInput"].toBool();
    QString portName = portData["name"].toString();
    QString componentName = portData["componentName"].toString();
    QString fullPath = portData["fullPath"].toString();
    
    // Логируем начало обработки
    QString logMsg = QString("onPortItemActivated: Processing %1 port '%2' in component '%3', activeTempLink=%4")
        .arg(isInput ? "input" : "output")
        .arg(portName)
        .arg(componentName)
        .arg(m_owner->m_activeTempLink ? "yes" : "no");
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    
    // НЕ закрываем дерево портов сразу - оно закроется автоматически через таймер
    // Это предотвращает случайные клики на фон сразу после закрытия дерева
    if(m_hideTimer)
    {
        m_hideTimer->stop();
    }
    
    if(isInput)
    {
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
            "onPortItemActivated: Input port selected", 0);
        
        // Если выбран входной порт и есть активное соединение - завершаем связь
        // Проверяем наличие активного соединения через m_activeTempLink
        if(m_owner->m_activeTempLink)
        {
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                "onPortItemActivated: Active connection exists", 0);
            
            // Дополнительная проверка: должны быть установлены исходные данные
            if(!m_owner->m_activeSourceNode || 
               m_owner->m_activeSourcePortName.isEmpty())
            {
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
                    "onPortItemActivated: Cannot complete connection - source port data is missing", 0);
                return;
            }
            
            // Формируем полные имена компонентов
            QString srcName = m_owner->m_activeSourceNode->nodeName;
            // Для собственных портов используем nodeName, для дочерних - componentName
            // componentName равен nodeName для собственных портов, поэтому проверяем это
            QString dstName = (componentName.isEmpty() || componentName == nodeName) ? nodeName : componentName;
            QString fullSrc = m_owner->m_componentName.isEmpty() ? srcName : m_owner->m_componentName + "." + srcName;
            
            // Формируем fullDst: если componentName == nodeName, это собственный порт,
            // и мы должны использовать только nodeName (без добавления m_componentName, если оно уже содержит nodeName)
            QString fullDst;
            if(m_owner->m_componentName.isEmpty())
            {
                fullDst = dstName;
            }
            else
            {
                // Проверяем, не содержит ли m_componentName уже dstName (чтобы избежать дублирования)
                if(m_owner->m_componentName == dstName || m_owner->m_componentName.endsWith("." + dstName))
                {
                    fullDst = m_owner->m_componentName;
                }
                else
                {
                    fullDst = m_owner->m_componentName + "." + dstName;
                }
            }
            
            // Логируем промежуточные значения для диагностики
            
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
            
            QString dstProp = fullPath.isEmpty() ? portName : fullPath;
            
            // Учитываем вложенные компоненты
            if(!m_owner->m_activeSourcePortComponentName.isEmpty() && 
               m_owner->m_activeSourcePortComponentName != srcName)
            {
                srcProp = m_owner->m_activeSourcePortComponentName + "." + srcProp;
            }
            // Для дочерних компонентов добавляем componentName к dstProp только если он отличается от nodeName
            if(!componentName.isEmpty() && componentName != nodeName)
            {
                dstProp = componentName + "." + dstProp;
            }
            
            // Логируем параметры перед созданием соединения
            QString preMsg = QString("onPortItemActivated: Creating connection: %1.%2 -> %3.%4")
                .arg(fullSrc).arg(srcProp).arg(fullDst).arg(dstProp);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, preMsg.toStdString().c_str(), 0);
            
            // Применяем связь к ядру
            int result = Model_CreateLinkByName(
                fullSrc.toStdString().c_str(),
                srcProp.toStdString().c_str(),
                fullDst.toStdString().c_str(),
                dstProp.toStdString().c_str()
            );
            
            
            // Логируем результат создания соединения
            if(result == 0) // Предполагаем, что 0 означает успех
            {
                QString successMsg = QString("Connection created successfully: %1.%2 -> %3.%4")
                    .arg(fullSrc).arg(srcProp).arg(fullDst).arg(dstProp);
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, successMsg.toStdString().c_str(), 0);
            }
            else
            {
                QString errorMsg = QString("Failed to create connection: %1.%2 -> %3.%4 (error code: %5)")
                    .arg(fullSrc).arg(srcProp).arg(fullDst).arg(dstProp).arg(result);
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_ERROR, errorMsg.toStdString().c_str(), 0);
            }
            
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
            m_owner->m_isWaitingForPortSelection = false;
            
            // Сбрасываем курсор
            if(m_owner->m_mainView)
            {
                m_owner->m_mainView->unsetCursor();
            }
            
            // Закрываем все открытые деревья портов
            for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
        }
        else
        {
            // Логируем предупреждение: попытка завершить соединение без активного соединения
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
                "onPortItemActivated: Attempted to complete connection but no active connection exists", 0);
            return;
        }
    }
    else
    {
        // Если выбран выходной порт - начинаем pull-режим
        QString inner_logMsg = QString("onPortItemActivated: Starting connection from output port '%1'")
            .arg(portName);
        // Находим порт в реальных портах (не в категоризированных)
        const Port* selectedPort = nullptr;
        
        // Ищем порт во всех доступных выходных портах
        QVector<Port> allOutputPorts;
        allOutputPorts.append(getOwnOutputPorts());
        allOutputPorts.append(getChildOutputPorts());
        allOutputPorts.append(getAliasOutputPorts());
        
        for(const Port& p : allOutputPorts)
        {
            if(p.name == portName && (componentName.isEmpty() || p.componentName == componentName))
            {
                selectedPort = &p;
                break;
            }
        }
        
        // Если не нашли, проверяем также вложенные порты
        if(!selectedPort)
        {
            QVector<Port> nestedPorts = getNestedPorts(false, true);
            for(const Port& p : nestedPorts)
            {
                if(p.name == portName && (componentName.isEmpty() || p.componentName == componentName))
                {
                    selectedPort = &p;
                    break;
                }
            }
        }
        
        if(selectedPort)
        {
            // Логируем успешное начало соединения
            QString successMsg = QString("Connection started from port '%1' in component '%2'")
                .arg(portName).arg(nodeName);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, successMsg.toStdString().c_str(), 0);
            
            // Сохраняем состояние активной связи
            m_owner->m_activeSourceNode = this;
            m_owner->m_activeSourcePort = selectedPort;
            // Сохраняем копии строковых полей для безопасного использования
            try
            {
                m_owner->m_activeSourcePortName = selectedPort->name;
                m_owner->m_activeSourcePortFullPath = selectedPort->fullPath;
                m_owner->m_activeSourcePortComponentName = selectedPort->componentName;
            }
            catch(...)
            {
                // Если произошла ошибка при копировании, отменяем операцию
                QString errorMsg = QString("Failed to start connection: error copying port data for port '%1'")
                    .arg(portName);
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_ERROR, errorMsg.toStdString().c_str(), 0);
                m_owner->m_activeSourceNode = nullptr;
                m_owner->m_activeSourcePort = nullptr;
                return;
            }
            m_owner->m_activeSourcePortPos = mapToScene(selectedPort->pos);
            
            // Получаем текущую позицию курсора в сцене для начальной позиции временной линии
            QPointF cursorScenePos;
            if(m_owner->m_mainView)
            {
                QPoint globalPos = QCursor::pos();
                QPoint viewPos = m_owner->m_mainView->mapFromGlobal(globalPos);
                cursorScenePos = m_owner->m_mainView->mapToScene(viewPos);
            }
            else
            {
                cursorScenePos = m_owner->m_activeSourcePortPos;
            }
            
            // Создаем временную линию с сохраненной позицией порта
            m_owner->m_activeTempLink = new LinkItem(this, cursorScenePos, m_owner->m_activeSourcePortPos);
            m_owner->m_scene->addItem(m_owner->m_activeTempLink);
            
            // Сбрасываем состояние заморозки при создании новой связи
            m_owner->m_isLineFrozen = false;
            
            // Обновляем геометрию линии сразу после создания
            m_owner->m_activeTempLink->updateGeometry(cursorScenePos);
            
            // Визуальная обратная связь: изменяем курсор
            if(m_owner->m_mainView)
            {
                m_owner->m_mainView->setCursor(Qt::CrossCursor);
            }
            
            // Обновляем сцену для немедленного отображения временной линии
            m_owner->m_scene->update();
            
            // Закрываем дерево портов после создания временной линии
            hidePortListWidget();
        }
        else
        {
            // Логируем ошибку: порт не найден
            QString errorMsg = QString("Failed to start connection: port '%1' not found in component '%2'")
                .arg(portName).arg(nodeName);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, errorMsg.toStdString().c_str(), 0);
        }
    }
    
    // Закрываем дерево портов для входного порта тоже (если связь завершена)
    if(isInput && m_owner->m_activeSourceNode && m_owner->m_activeSourcePort)
    {
        hidePortListWidget();
    }
}

void UModernDiagramWidget::NodeItem::updatePortListWidget(bool isInput, bool includeNested)
{
    if(!m_portListWidget || !m_hoveredPort)
        return;
    
    m_portListWidget->clear();
    
    // Получаем список портов с улучшенной логикой
    QVector<Port> availablePorts;
    
    // Проверяем модификаторы для показа всех портов
    bool showAll = QApplication::keyboardModifiers() & Qt::ShiftModifier;
    
    if(m_hoveredPort->fullPath.isEmpty() && 
       (m_hoveredPort->category == PortCategory::Own || 
        m_hoveredPort->category == PortCategory::Child || 
        m_hoveredPort->category == PortCategory::Alias))
    {
        // Если это категоризированный порт
        if(showAll)
        {
            // Показываем все категории портов при нажатии Shift
            if(isInput)
            {
                availablePorts.append(getOwnInputPorts());
                availablePorts.append(getChildInputPorts());
                availablePorts.append(getAliasInputPorts());
            }
            else
            {
                availablePorts.append(getOwnOutputPorts());
                availablePorts.append(getChildOutputPorts());
                availablePorts.append(getAliasOutputPorts());
            }
        }
        else
        {
            // Показываем только порты выбранной категории
            if(isInput)
            {
                // Фильтрация для входных портов
                if(m_hoveredPort->category == PortCategory::Own)
                {
                    availablePorts = getOwnInputPorts();
                }
                else if(m_hoveredPort->category == PortCategory::Child)
                {
                    availablePorts = getChildInputPorts();
                }
                else if(m_hoveredPort->category == PortCategory::Alias)
                {
                    availablePorts = getAliasInputPorts();
                }
                else
                {
                    // Fallback: используем все порты
                    availablePorts = getNestedPorts(isInput, includeNested);
                }
            }
            else
            {
                // Фильтрация для выходных портов
                if(m_hoveredPort->category == PortCategory::Own)
                {
                    availablePorts = getOwnOutputPorts();
                }
                else if(m_hoveredPort->category == PortCategory::Child)
                {
                    availablePorts = getChildOutputPorts();
                }
                else if(m_hoveredPort->category == PortCategory::Alias)
                {
                    availablePorts = getAliasOutputPorts();
                }
                else
                {
                    // Fallback: используем все порты
                    availablePorts = getNestedPorts(isInput, includeNested);
                }
            }
        }
    }
    else
    {
        // Для обычных портов используем стандартную логику с улучшенной поддержкой вложенных портов
        availablePorts = getNestedPorts(isInput, includeNested || showAll);
    }
    
    if(availablePorts.isEmpty())
    {
        // Если портов нет, показываем только текущий порт
        QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_portListWidget);
        rootItem->setText(0, nodeName);
        QTreeWidgetItem* portItem = new QTreeWidgetItem(rootItem);
        portItem->setText(0, m_hoveredPort->name + " (текущий)");
        
        // Сохраняем данные порта
        QMap<QString, QVariant> portData;
        portData["isInput"] = m_hoveredPort->isInput;
        portData["name"] = m_hoveredPort->name;
        portData["componentName"] = m_hoveredPort->componentName;
        portData["fullPath"] = m_hoveredPort->fullPath;
        portItem->setData(0, Qt::UserRole, QVariant::fromValue(portData));
        
        rootItem->setExpanded(true);
        m_portListWidget->setCurrentItem(portItem);
    }
    else
    {
        // Группируем порты по компонентам
        QMap<QString, QTreeWidgetItem*> componentItems;
        QTreeWidgetItem* currentPortItem = nullptr;
        
        for(const Port& p : availablePorts)
        {
            QTreeWidgetItem* compItem = nullptr;
            if(componentItems.contains(p.componentName))
            {
                compItem = componentItems[p.componentName];
            }
            else
            {
                compItem = new QTreeWidgetItem(m_portListWidget);
                compItem->setText(0, p.componentName);
                componentItems[p.componentName] = compItem;
            }
            
            QTreeWidgetItem* portItem = new QTreeWidgetItem(compItem);
            QString portText = p.name;
            if(p.name == m_hoveredPort->name && p.componentName == nodeName)
            {
                portText += " (текущий)";
                currentPortItem = portItem;
            }
            portItem->setText(0, portText);
            
            // Сохраняем данные порта
            QMap<QString, QVariant> portData;
            portData["isInput"] = p.isInput;
            portData["name"] = p.name;
            portData["componentName"] = p.componentName;
            portData["fullPath"] = p.fullPath;
            portItem->setData(0, Qt::UserRole, QVariant::fromValue(portData));
        }
        
        // Разворачиваем все элементы
        for(QTreeWidgetItem* item : componentItems.values())
        {
            item->setExpanded(true);
        }
        
        // Выделяем текущий порт
        if(currentPortItem)
        {
            m_portListWidget->setCurrentItem(currentPortItem);
        }
    }
}

UModernDiagramWidget::LinkItem::LinkItem(NodeItem* src, NodeItem* dst, bool useOutput, bool useInput)
    : QGraphicsPathItem()
    , m_owner(src ? src->m_owner : nullptr)
    , m_src(src)
    , m_dst(dst)
    , m_useOutput(useOutput)
    , m_useInput(useInput)
    , m_isTemp(false)
    , m_srcCategory(PortCategory::Own)
    , m_dstCategory(PortCategory::Own)
    , m_hasCategories(false)
{
    UStyleManager* style = UStyleManager::instance();
    setPen(QPen(style->getLinkColor(), style->getLinkWidth(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    setAcceptHoverEvents(true);
    // Включаем кэширование для оптимизации отрисовки
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    updateGeometry();
}

UModernDiagramWidget::LinkItem::LinkItem(NodeItem* src, NodeItem* dst, PortCategory srcCategory, PortCategory dstCategory)
    : QGraphicsPathItem()
    , m_owner(src ? src->m_owner : nullptr)
    , m_src(src)
    , m_dst(dst)
    , m_useOutput(true)
    , m_useInput(true)
    , m_isTemp(false)
    , m_srcCategory(srcCategory)
    , m_dstCategory(dstCategory)
    , m_hasCategories(true)
{
    UStyleManager* style = UStyleManager::instance();
    setPen(QPen(style->getLinkColor(), style->getLinkWidth(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    setAcceptHoverEvents(true);
    // Включаем кэширование для оптимизации отрисовки
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    // ОПТИМИЗАЦИЯ: updateGeometry() вызывается отдельно после создания всех связей в buildLinks()
    // для ускорения массового создания связей
    // updateGeometry();
}

UModernDiagramWidget::LinkItem::LinkItem(NodeItem* src, const QPointF& tempEnd, const QPointF& startPos)
    : QGraphicsPathItem()
    , m_owner(src ? src->m_owner : nullptr)
    , m_src(src)
    , m_dst(nullptr)
    , m_useOutput(true)
    , m_useInput(true)
    , m_isTemp(true)
    , m_tempEnd(tempEnd)
    , m_startPos(startPos)
    , m_srcCategory(PortCategory::Own)
    , m_dstCategory(PortCategory::Own)
    , m_hasCategories(false)
{
    UStyleManager* style = UStyleManager::instance();
    setPen(QPen(style->getLinkTempColor(), style->getLinkWidth(), Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    // Temporary links don't need hover events
    setAcceptHoverEvents(false);
    // Временные связи не кэшируем, так как они часто меняются
    updateGeometry(tempEnd);
}

void UModernDiagramWidget::LinkItem::updateGeometry(const QPointF& cursorOverride)
{
    if(!m_src)
        return;
    
    QPointF start;
    // Для временной линии используем сохраненную позицию порта, если она задана
    if(m_isTemp && !m_startPos.isNull())
    {
        start = m_startPos;
    }
    else if(m_isTemp)
    {
        // Fallback: используем позицию выходного порта
        start = m_src->scenePortPos(false);
    }
    else
    {
        // Если категория известна, используем позицию порта этой категории
        if(m_hasCategories)
        {
            start = m_src->scenePortPosByCategory(true, m_srcCategory);
        }
        else
        {
            start = m_src->scenePortPos(true);
        }
    }
    
    QPointF end;
    if(m_dst)
    {
        // Если категория известна, используем позицию порта этой категории
        if(m_hasCategories)
        {
            end = m_dst->scenePortPosByCategory(false, m_dstCategory);
        }
        else
        {
            end = m_dst->scenePortPos(false);
        }
    }
    else
    {
        end = cursorOverride.isNull() ? m_tempEnd : cursorOverride;
    }

    QPainterPath path(start);
    QPointF c1 = start + QPointF((end.x() - start.x()) * 0.4, 0);
    QPointF c2 = end   - QPointF((end.x() - start.x()) * 0.4, 0);
    path.cubicTo(c1, c2, end);
    setPath(path);
}

void UModernDiagramWidget::LinkItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPathItem::hoverEnterEvent(event);
    if(m_owner && !m_isTemp)
    {
        QString tooltip = m_owner->generateLinkTooltip(this);
        setToolTip(tooltip);
    }
}

void UModernDiagramWidget::LinkItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPathItem::hoverLeaveEvent(event);
    setToolTip(QString());
}

// --------------------------- Widget ---------------------------

UModernDiagramWidget::UModernDiagramWidget(QWidget *parent)
    : QWidget(parent)
    , m_scene(new ModernScene(this))
    , m_mainView(new ModernGraphicsView(this, m_scene))
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
    , m_contextMenu(nullptr)
    , m_actionViewOrBreakLink(nullptr)
    , m_actionCreateLink(nullptr)
    , m_actionFinishLink(nullptr)
    , m_actionCancelLink(nullptr)
    , m_actionStartMoving(nullptr)
    , m_actionFinishMoving(nullptr)
    , m_actionCancelMoving(nullptr)
    , m_actionSwitchLink(nullptr)
    , m_actionFinishSwitching(nullptr)
    , m_actionCancelSwitching(nullptr)
    , m_actionCloneComponent(nullptr)
    , m_actionQuickLink(nullptr)
    , m_contextMenuNode(nullptr)
    , m_resetZoomButton(nullptr)
{
    m_mainView->setRenderHint(QPainter::Antialiasing, true);
    m_mainView->setDragMode(QGraphicsView::RubberBandDrag);
    m_mainView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    m_mainView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mainView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_miniMap->setFixedHeight(140);
    m_miniMap->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_miniMap->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_miniMap->setInteractive(false);
    m_miniMap->scale(0.2, 0.2);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(m_mainView);
    // Миникарта скрыта
    m_miniMap->hide();
    
    // Создание кнопки сброса масштаба
    m_resetZoomButton = new QPushButton(this);
    m_resetZoomButton->setText("⟲");
    m_resetZoomButton->setToolTip(tr("Reset zoom"));
    m_resetZoomButton->setFixedSize(32, 32);
    updateResetZoomButtonStyle();
    m_resetZoomButton->raise();
    connect(m_resetZoomButton, &QPushButton::clicked, this, &UModernDiagramWidget::onResetZoomClicked);
    
    // Позиционируем кнопку при первом создании
    QTimer::singleShot(0, this, [this]() {
        if(m_resetZoomButton)
        {
            int margin = 10;
            m_resetZoomButton->move(width() - m_resetZoomButton->width() - margin, margin);
        }
    });
    
    createContextMenu();
}

UModernDiagramWidget::~UModernDiagramWidget()
{
    // Автоматическое сохранение кэша при закрытии
    if(m_application)
    {
        const QHash<QString, ComponentCacheEntry>& entries = m_componentCache.getAllEntries();
        if(!entries.isEmpty())
        {
            // Пробуем сохранить в бинарном формате (быстрее), если не получится - в JSON
            QString binPath = getCacheFilePath("bin");
            if(!saveComponentCacheToFile(binPath, true))
            {
                QString jsonPath = getCacheFilePath("json");
                saveComponentCacheToFile(jsonPath, false);
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
        QString binPath = getCacheFilePath("bin");
        QString jsonPath = getCacheFilePath("json");
        
        logMsg = QString("[UModernDiagramWidget] Attempting to load cache from binary: %1").arg(binPath.isEmpty() ? "<empty>" : binPath);
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        
        bool loaded = loadComponentCacheFromFile(binPath, true);
        if(loaded)
        {
            QString logMsg = QString("[UModernDiagramWidget] Component cache loaded from binary file: %1").arg(binPath);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        }
        else
        {
            // Если бинарный не найден, пробуем JSON
            logMsg = QString("[UModernDiagramWidget] Binary cache not found, attempting JSON: %1").arg(jsonPath.isEmpty() ? "<empty>" : jsonPath);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            
            loaded = loadComponentCacheFromFile(jsonPath, false);
            if(loaded)
            {
                QString logMsg = QString("[UModernDiagramWidget] Component cache loaded from JSON file: %1").arg(jsonPath);
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            }
            else
            {
                QString logMsg = QString("[UModernDiagramWidget] Component cache not found, starting with empty cache (checked: bin=%1, json=%2)")
                    .arg(binPath.isEmpty() ? "<empty>" : binPath)
                    .arg(jsonPath.isEmpty() ? "<empty>" : jsonPath);
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            }
        }
    }
    else
    {
        QString logMsg = QString("[UModernDiagramWidget] SetApplication called with nullptr, cache loading skipped");
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    }
}

void UModernDiagramWidget::SetComponentName(const QString& name)
{
    m_componentName = name;
}

void UModernDiagramWidget::Reload()
{
    // Профилирование: начало операции Reload
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // QString componentDisplayName = m_componentName.isEmpty() ? "root" : m_componentName;
    // NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UModernDiagramWidget.Reload"), componentDisplayName);
    
    // Сохраняем текущее состояние viewport перед перезагрузкой
    // Сохраняем только если сцена уже содержит элементы (компонент был загружен ранее)
    if(!m_componentName.isEmpty() && !m_scene->items().isEmpty())
    {
        saveCurrentViewState();
    }
    
    // Оптимизация: проверяем кэш ПЕРЕД clearScene()
    bool structureUnchanged = false;
    if(!m_componentName.isEmpty() && m_levelCache.contains(m_componentName))
    {
        const SceneCache& cache = m_levelCache[m_componentName];
        
        // Проверяем, изменилась ли структура компонентов
        const char* compRaw = Model_GetComponentsNameList(m_componentName.toStdString().c_str());
        QString compListStr = QString::fromUtf8(compRaw ? compRaw : "");
        QStringList currentComponents = compListStr.split(",", Qt::SkipEmptyParts);
        Engine_FreeBufString(compRaw);
        
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
            restoreViewState(m_componentName);
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
            invalidateLevelCache(m_componentName);
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
    restoreViewState(m_componentName);
    
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

void UModernDiagramWidget::clearScene()
{
    // Профилирование: начало операции clearScene
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // QString componentDisplayName = m_componentName.isEmpty() ? "root" : m_componentName;
    int nodesCount = m_nodes.size();
    int linksCount = m_links.size();
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
        saveSceneToCache(m_componentName);
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
    m_lastNodePositions.clear();  // Очищаем сохраненные позиции
    m_scene->clear();  // Удаляет все элементы, включая NodeItem и прокси-виджеты
    m_tempLink = nullptr;
    m_dragSourceNode = nullptr;
    m_activeTempLink = nullptr;
    m_activeSourceNode = nullptr;
    m_activeSourcePort = nullptr;
    m_isLineFrozen = false;
    m_frozenTargetPortPos = QPointF(0, 0);
    m_normalizationOffset = QPointF(0, 0);
    
    // Логируем результат профилирования
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // qint64 elapsed = telemetry.Elapsed();
    // QString details = QString("removed %1 nodes, %2 links").arg(nodesCount).arg(linksCount);
    // QString logMsg = QString("[UModernDiagramWidget] Component: %1, Operation: clearScene, Duration: %2ms, Details: %3")
    //     .arg(componentDisplayName).arg(elapsed).arg(details);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
}

void UModernDiagramWidget::buildScene()
{
    // Удалено избыточное логирование - создавало спам в INFO логах
    if(!m_application)
    {
        // Удалено избыточное логирование - создавало спам в INFO логах
        return;
    }

    // Профилирование: начало операции buildScene
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // QString componentDisplayName = m_componentName.isEmpty() ? "root" : m_componentName;
    // NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UModernDiagramWidget.buildScene"), componentDisplayName);

    // Оптимизация: отключаем обновления во время массового создания узлов
    setUpdatesEnabled(false);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(false);

    const char* compRaw = Model_GetComponentsNameList(m_componentName.toStdString().c_str());
    QString compListStr = QString::fromUtf8(compRaw ? compRaw : "");
    QStringList components = compListStr.split(",", Qt::SkipEmptyParts);
    Engine_FreeBufString(compRaw);
    // Удалено избыточное логирование - создавало спам в INFO логах
    
    // Обновляем имя телеметрии с количеством узлов
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // telemetry.Stop();
    // NMSDK::UGuiTelemetryScope telemetry2(QStringLiteral("UModernDiagramWidget.buildScene"), 
    //     componentDisplayName + " (" + QString::number(components.size()) + " nodes)");

    // Оптимизация: кэшируем имена классов компонентов для минимизации вызовов API
    QHash<QString, QString> classNameCache;
    
    // Оптимизация: кэшируем координаты компонентов, чтобы не вызывать loadCoord() дважды
    QHash<QString, QPointF> coordCache;
    QHash<QString, bool> coordLoadedCache;
    
    int idx = 0;
    bool coordsLoaded = false;
    QPointF minKernel(0,0);
    bool minSet=false;
    // Сначала загружаем все координаты и находим минимальную для визуальной нормализации
    for(const QString& comp : components)
    {
        QString fullName = m_componentName.isEmpty() ? comp : m_componentName + "." + comp;
        QPointF kernelPos;
        bool loaded = false;
        
        // Пытаемся взять координаты из сессионного кэша компонентов
        ComponentCacheEntry* cacheEntry = m_componentCache.getEntry(fullName);
        bool usedCache = false;
        if(cacheEntry && cacheEntry->hasKernelPos)
        {
            kernelPos = cacheEntry->kernelPos;
            loaded = true;
            usedCache = true;
        }
        else
        {
            loaded = loadCoord(fullName, kernelPos);
            
            // Сохраняем координаты в кэш для ускорения последующих Reload
            if(cacheEntry)
            {
                cacheEntry->kernelPos = kernelPos;
                cacheEntry->hasKernelPos = loaded;
                cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
            }
            else
            {
                ComponentCacheEntry newEntry;
                newEntry.kernelPos = kernelPos;
                newEntry.hasKernelPos = loaded;
                newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
                m_componentCache.setEntry(fullName, newEntry);
            }
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
    // Это смещение НЕ сохраняется в ядре и пересчитывается при каждой загрузке
    QPointF minScenePos = coordsLoaded ? scenePosFromKernel(minKernel) : QPointF(0,0);
    // Сохраняем смещение нормализации ДО создания узлов, чтобы оно было доступно при сохранении координат
    // ВАЖНО: это смещение используется только для визуального отображения, координаты в ядре остаются абсолютными
    m_normalizationOffset = minScenePos;
    // Удалено избыточное логирование - создавало спам в INFO логах
    
    // Оптимизация: создаем все узлы сначала, затем добавляем в сцену пакетами
    QList<NodeItem*> nodesToAdd;
    QHash<QString, QPointF> nodePositions;
    
    for(const QString& comp : components)
    {
        QString fullName = m_componentName.isEmpty() ? comp : m_componentName + "." + comp;
        
        // Кэшируем имя класса для минимизации вызовов API
        QString cls;
        bool classNameFromCache = false;
        ComponentCacheEntry* cacheEntry = m_componentCache.getEntry(fullName);
        if(cacheEntry && !cacheEntry->className.isEmpty())
        {
            // Берем имя класса из сессионного кэша
            cls = cacheEntry->className;
            classNameFromCache = true;
        }
        else if(classNameCache.contains(fullName))
        {
            cls = classNameCache[fullName];
        }
        else
        {
            const char* clsRaw = Model_GetComponentClassName(fullName.toStdString().c_str());
            cls = QString::fromUtf8(clsRaw ? clsRaw : "");
            Engine_FreeBufString(clsRaw);
            classNameCache[fullName] = cls;
            
            // Сохраняем имя класса в сессионный кэш для ускорения последующих Reload
            if(cacheEntry)
            {
                cacheEntry->className = cls;
                cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
            }
            else
            {
                ComponentCacheEntry newEntry;
                newEntry.className = cls;
                newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
                m_componentCache.setEntry(fullName, newEntry);
            }
        }

        auto* node = new NodeItem(this, comp, cls);
        QPointF loaded;
        // Используем кэшированные координаты вместо повторного вызова loadCoord()
        QPointF kernelPos = coordCache.value(fullName);
        if(coordLoadedCache.value(fullName, false))
        {
            // Используем абсолютные координаты из ядра, нормализуем только для визуального отображения
            QPointF absoluteScenePos = scenePosFromKernel(kernelPos);
            loaded = absoluteScenePos - minScenePos;
            // Удалено избыточное логирование - создавало спам в INFO логах
        }
        else
        {
            loaded = QPointF((idx%4)*180, (idx/4)*140);
        }
        
        nodesToAdd.append(node);
        nodePositions[comp] = loaded;
        m_nodeByName.insert(comp, node);
        idx++;
    }
    
    // Добавляем узлы в сцену пакетами
    for(auto* node : nodesToAdd)
    {
        m_scene->addItem(node);
        m_nodes.append(node);
        QString comp = node->nodeName;
        QPointF loaded = nodePositions[comp];
        node->setPos(loaded);
        // Инициализируем сохраненную позицию для перемещения группы
        m_lastNodePositions[node] = loaded;
    }
    
    if(!coordsLoaded)
    {
        // нет координат из ядра — оставляем как есть и не перезаписываем в ядро,
        // чтобы при первом отображении не было автосжатия старого вида
        layoutGrid();
    }

    // Создаем связи сразу после узлов (синхронно)
    // Включаем обновления перед созданием связей для корректного отображения
    setUpdatesEnabled(true);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(true);
    
    // Создаем связи после всех узлов
    buildLinks();
    
    // ВРЕМЕННО ОТКЛЮЧЕНО: сохранение в кэш вызывает падения при повторном входе
    // Проблема: после clearScene() указатели становятся невалидными
    // TODO: переделать кэш на сохранение данных вместо указателей
    // saveSceneToCache(m_componentName);
    
    // Логируем результат профилирования buildScene
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // qint64 elapsed = telemetry2.Elapsed();
    // 
    // // Подсчитываем статистику использования кэша
    // int coordsFromCache = 0;
    // int classNamesFromCache = 0;
    // for(const QString& comp : components)
    // {
    //     QString fullName = m_componentName.isEmpty() ? comp : m_componentName + "." + comp;
    //     ComponentCacheEntry* cacheEntry = m_componentCache.getEntry(fullName);
    //     if(cacheEntry)
    //     {
    //         if(cacheEntry->hasKernelPos)
    //             coordsFromCache++;
    //         if(!cacheEntry->className.isEmpty())
    //             classNamesFromCache++;
    //     }
    // }
    // 
    // QString details = QString("created %1 nodes, coordsFromCache: %2/%3, classNamesFromCache: %4/%3")
    //     .arg(components.size()).arg(coordsFromCache).arg(components.size()).arg(classNamesFromCache);
    // QString logMsg = QString("[UModernDiagramWidget] Component: %1, Operation: buildScene, Duration: %2ms, Details: %3")
    //     .arg(componentDisplayName).arg(elapsed).arg(details);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    
    // Планируем отложенное сохранение кэша после завершения buildScene
    scheduleCacheSave();
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

UModernDiagramWidget::NodeItem* UModernDiagramWidget::pickPort(const QPointF& scenePos, bool requireInput, QPointF& portPos)
{
    NodeItem* node = nullptr;
    // Нам нужен только найденный узел и позиция порта; возвращаемое значение не используем
    pickPortDetailed(scenePos, requireInput, node, portPos);
    return node;
}

const UModernDiagramWidget::Port* UModernDiagramWidget::pickPortDetailed(
    const QPointF& scenePos, bool requireInput, NodeItem*& node, QPointF& portPos)
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

UModernDiagramWidget::NodeItem* UModernDiagramWidget::pickNode(const QPointF& scenePos) const
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
    
    const char* xmlRaw = Model_GetComponentInternalLinks(m_componentName.toStdString().c_str(), nullptr);
    if(!xmlRaw)
    {
        setUpdatesEnabled(updatesWereEnabled);
        if(m_mainView)
            m_mainView->setUpdatesEnabled(updatesWereEnabled);
        // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
        // qint64 elapsed = telemetry.Elapsed();
        // QString logMsg = QString("[UModernDiagramWidget] Component: %1, Operation: buildLinks, Duration: %2ms, Details: no links")
        //     .arg(componentDisplayName).arg(elapsed);
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        return;
    }
    std::string raw(xmlRaw ? xmlRaw : "");
    // Удалено избыточное логирование - создавало спам в INFO логах

    RDK::USerStorageXML xml;
    if(!xml.Load(raw, "Links"))
    {
        Engine_FreeBufString(xmlRaw);
        setUpdatesEnabled(updatesWereEnabled);
        if(m_mainView)
            m_mainView->setUpdatesEnabled(updatesWereEnabled);
        // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
        // qint64 elapsed = telemetry.Elapsed();
        // QString logMsg = QString("[UModernDiagramWidget] Component: %1, Operation: buildLinks, Duration: %2ms, Details: failed to load XML")
        //     .arg(componentDisplayName).arg(elapsed);
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        return;
    }

    RDK::UStringLinksList linkslist;
    xml >> linkslist;
    Engine_FreeBufString(xmlRaw);
    // Удалено избыточное логирование - создавало спам в INFO логах
    
    // Обновляем имя телеметрии с количеством связей
    // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    // telemetry.Stop();
    // NMSDK::UGuiTelemetryScope telemetry2(QStringLiteral("UModernDiagramWidget.buildLinks"), 
    //     componentDisplayName + " (" + QString::number(linkslist.GetSize()) + " links)");

    QPointF minPos = currentMinScenePos();

    auto resolveNode = [&](const QString& full)->NodeItem*{
        if(auto it = m_nodeByName.find(full); it != m_nodeByName.end())
            return it.value();
        if(!m_componentName.isEmpty() && full.startsWith(m_componentName + ".")) {
            QString trimmed = full.mid(m_componentName.size() + 1);
            int dot = trimmed.indexOf('.');
            QString top = dot >= 0 ? trimmed.left(dot) : trimmed;
            if(auto it = m_nodeByName.find(top); it != m_nodeByName.end())
                return it.value();
        }
        int dot = full.indexOf('.');
        QString top = dot >= 0 ? full.left(dot) : full;
        if(auto it = m_nodeByName.find(top); it != m_nodeByName.end())
            return it.value();
        return nullptr;
    };
    auto resolveById = [&](const QString& id)->NodeItem*{
        QString base = id;
        int dot = base.indexOf('.');
        if(dot >= 0)
            base = base.left(dot);
        // если это вложенное имя под текущей моделью, уберём префикс модели
        if(!m_componentName.isEmpty() && base.startsWith(m_componentName + "."))
        {
            QString trimmed = base.mid(m_componentName.size() + 1);
            base = trimmed;
        }
        if(auto it = m_nodeByName.find(base); it != m_nodeByName.end())
            return it.value();
        return nullptr;
    };

    // Оптимизация: создаем все связи сначала, затем добавляем в сцену пакетами
    QList<LinkItem*> linksToAdd;
    QHash<QPair<NodeItem*, QString>, PortCategory> portCategoryCache;  // Кэш для determinePortCategory
    
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
            NodeItem* srcNode = resolveNode(itemName);
            NodeItem* dstNode = resolveNode(connName);
            if(!srcNode)
                srcNode = resolveById(QString::fromStdString(itemId));
            if(!dstNode)
                dstNode = resolveById(QString::fromStdString(connId));
            
            // Если srcNode не найден, пропускаем связь (не можем определить категорию источника)
            if(!srcNode)
            {
                skipped++;
                continue;
            }
            
            // Оптимизация: кэшируем результаты determinePortCategory для источника
            QPair<NodeItem*, QString> srcKey(srcNode, itemName);
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
            
            // Для определения категории входного порта нужно нормализовать connName
            QString normalizedConnName = connName;
            QString dstNodeName;
            QString connIdStr = QString::fromStdString(connId);
            
            // Если dstNode найден, используем его имя для нормализации
            if(dstNode)
            {
                dstNodeName = dstNode->nodeName;
            }
            else
            {
                // Для внешних связей (dstNode не найден) пытаемся извлечь имя узла из connId или connName
                // Это нужно для определения категории целевого порта
                if(!connIdStr.isEmpty())
                {
                    int dot = connIdStr.indexOf('.');
                    dstNodeName = dot >= 0 ? connIdStr.left(dot) : connIdStr;
                    // Убираем префикс компонента, если есть
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
            }
            
            // На верхнем уровне (m_componentName.isEmpty()) connName уже является относительным путем
            // и не требует нормализации через удаление dstNodeName
            // НО: если connName - это просто имя свойства (без точки), а connId содержит путь к дочернему компоненту,
            // нужно извлечь путь из connId относительно dstNodeName
            if(m_componentName.isEmpty())
            {
                // Если connId указывает на dstNode напрямую (без дочерних компонентов), используем connName как есть
                if(connIdStr == dstNodeName)
                {
                    normalizedConnName = connName;
                }
                // Если connId содержит путь к дочернему компоненту (начинается с dstNodeName + ".")
                else if(connIdStr.startsWith(dstNodeName + "."))
                {
                    // Извлекаем путь к дочернему компоненту из connId
                    QString pathFromConnId = connIdStr.mid(dstNodeName.length() + 1);
                    // Если connName не содержит точки (просто имя свойства), используем путь из connId
                    if(!connName.contains('.'))
                    {
                        normalizedConnName = pathFromConnId + "." + connName;
                    }
                    // Если connName уже содержит путь, используем его
                    else
                    {
                        normalizedConnName = connName;
                    }
                }
                // Если connName начинается с dstNodeName, извлекаем часть после nodeName
                else if(connName.startsWith(dstNodeName + "."))
                {
                    normalizedConnName = connName.mid(dstNodeName.length() + 1);
                }
            }
            else
            {
                // Для вложенных уровней проверяем различные возможные форматы пути
                if(connName.startsWith(dstNodeName + "."))
                {
                    // connName начинается с dstNodeName, извлекаем часть после nodeName
                    normalizedConnName = connName.mid(dstNodeName.length() + 1);
                }
                else
                {
                    // Проверяем, начинается ли connName с полного пути через m_componentName
                    QString fullPath = m_componentName + "." + dstNodeName;
                    if(connName.startsWith(fullPath + "."))
                    {
                        normalizedConnName = connName.mid(fullPath.length() + 1);
                    }
                    else
                    {
                        // Если connId указывает на dstNode, используем connName как есть
                        // (он уже является относительным путем)
                        if(connIdStr == dstNodeName || 
                           connIdStr.endsWith("." + dstNodeName) ||
                           connIdStr == fullPath ||
                           connIdStr.endsWith("." + fullPath))
                        {
                            normalizedConnName = connName;
                        }
                    }
                }
            }
            
            // Определяем категорию целевого порта
            PortCategory dstCategory;
            if(dstNode)
            {
                // Если dstNode найден, используем обычный метод
                QPair<NodeItem*, QString> dstKey(dstNode, normalizedConnName);
                if(portCategoryCache.contains(dstKey))
                {
                    dstCategory = portCategoryCache[dstKey];
                }
                else
                {
                    dstCategory = dstNode->determinePortCategory(normalizedConnName, true);
                    portCategoryCache[dstKey] = dstCategory;
                }
            }
            else
            {
                // Для внешних связей (dstNode не найден) определяем категорию по эвристике
                // Внешние связи обычно идут к собственным портам внешних компонентов (Own)
                // Но если путь содержит точку после имени компонента, это может быть Child
                if(!normalizedConnName.isEmpty() && normalizedConnName.contains('.'))
                {
                    // Путь содержит точку - проверяем, является ли первая часть дочерним компонентом
                    QString firstPart = normalizedConnName.split('.').first();
                    // Если у srcNode есть дочерний компонент с таким именем, это Child
                    // Иначе это Own (связь к собственному порту внешнего компонента)
                    QString fullName = m_componentName.isEmpty() ? srcNode->nodeName : m_componentName + "." + srcNode->nodeName;
                    const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
                    bool isChild = false;
                    if(compList)
                    {
                        QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
                        isChild = components.contains(firstPart);
                        Engine_FreeBufString(compList);
                    }
                    dstCategory = isChild ? PortCategory::Child : PortCategory::Own;
                }
                else
                {
                    // Простое имя свойства без точки - это Own категория (собственный порт внешнего компонента)
                    dstCategory = PortCategory::Own;
                }
            }
            
            // Создаем LinkItem с категориями портов
            auto* l = new LinkItem(srcNode, dstNode, srcCategory, dstCategory);
            
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
    qint64 updateGeometryElapsed = 0;  // ОТЛАДОЧНОЕ ЛОГИРОВАНИЕ ЗАКОММЕНТИРОВАНО
    
    // Инвалидируем кэш paint() для всех узлов после создания связей
    for(auto* node : m_nodes)
    {
        node->m_cacheValid = false;
        // НЕ инвалидируем кэш портов - они не меняются при создании связей
        // node->m_portsCacheValid = false;
    }
    
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
        invalidateLevelCache(m_componentName);
    }
}

void UModernDiagramWidget::saveSceneToCache(const QString& componentName)
{
    if(componentName.isEmpty() || m_nodes.isEmpty())
        return;
    
    // ВАЖНО: сохраняем кэш только если элементы еще в сцене (не были удалены)
    // Проверяем, что хотя бы один узел еще в сцене
    bool hasValidNodes = false;
    for(auto* node : m_nodes)
    {
        if(node && node->scene() == m_scene)
        {
            hasValidNodes = true;
            break;
        }
    }
    if(!hasValidNodes)
        return;  // Элементы уже удалены, не сохраняем в кэш
    
    SceneCache cache;
    cache.nodes = m_nodes;
    cache.links = m_links;
    cache.nodeByName = m_nodeByName;
    cache.lastNodePositions = m_lastNodePositions;
    cache.normalizationOffset = m_normalizationOffset;
    
    // Сохраняем список компонентов для проверки изменений структуры
    const char* compRaw = Model_GetComponentsNameList(componentName.toStdString().c_str());
    QString compListStr = QString::fromUtf8(compRaw ? compRaw : "");
    cache.componentNames = compListStr.split(",", Qt::SkipEmptyParts);
    Engine_FreeBufString(compRaw);
    
    cache.isValid = true;
    m_levelCache[componentName] = cache;
}

void UModernDiagramWidget::restoreSceneFromCache(const QString& componentName)
{
    if(componentName.isEmpty() || !m_levelCache.contains(componentName))
        return;
    
    const SceneCache& cache = m_levelCache[componentName];
    if(!cache.isValid)
        return;
    
    // Оптимизация: отключаем обновления во время восстановления
    setUpdatesEnabled(false);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(false);
    
    // Восстанавливаем данные
    m_nodes = cache.nodes;
    m_links = cache.links;
    m_nodeByName = cache.nodeByName;
    m_lastNodePositions = cache.lastNodePositions;
    m_normalizationOffset = cache.normalizationOffset;
    
    // Добавляем узлы и связи обратно в сцену
    // ВАЖНО: элементы из кэша должны быть валидными, так как мы не вызываем clearScene()
    // при восстановлении из кэша. Но на всякий случай проверяем валидность через scene()
    for(auto* node : m_nodes)
    {
        if(node)
        {
            // Проверяем валидность указателя через проверку scene()
            // Если scene() возвращает nullptr, элемент был удален, пропускаем его
            QGraphicsScene* nodeScene = nullptr;
            try {
                nodeScene = node->scene();
            } catch (...) {
                // Указатель невалидный, пропускаем этот узел
                continue;
            }
            
            if(!nodeScene)
            {
                m_scene->addItem(node);
            }
        }
    }
    
    for(auto* link : m_links)
    {
        if(link)
        {
            // Проверяем валидность указателя
            QGraphicsScene* linkScene = nullptr;
            try {
                linkScene = link->scene();
            } catch (...) {
                // Указатель невалидный, пропускаем эту связь
                continue;
            }
            
            if(!linkScene)
            {
                m_scene->addItem(link);
                // Восстанавливаем кэш связей для узлов
                NodeItem* srcNode = link->getSourceNode();
                NodeItem* dstNode = link->getDestinationNode();
                if(srcNode && m_nodes.contains(srcNode))
                    srcNode->m_connectedLinks.append(link);
                if(dstNode && m_nodes.contains(dstNode))
                    dstNode->m_connectedLinks.append(link);
            }
        }
    }
    
    // Включаем обновления обратно
    setUpdatesEnabled(true);
    if(m_mainView)
        m_mainView->setUpdatesEnabled(true);
}

void UModernDiagramWidget::invalidateLevelCache(const QString& componentName)
{
    if(componentName.isEmpty())
    {
        // Инвалидируем все кэши
        m_levelCache.clear();
    }
    else
    {
        // Инвалидируем кэш конкретного уровня
        m_levelCache.remove(componentName);
    }
}

void UModernDiagramWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // Позиционируем кнопку сброса масштаба в правом верхнем углу
    if(m_resetZoomButton)
    {
        int margin = 10;
        m_resetZoomButton->move(width() - m_resetZoomButton->width() - margin, margin);
    }
}

void UModernDiagramWidget::keyPressEvent(QKeyEvent *event)
{
    // Обработка Enter для завершения соединения через окно выбора портов
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        // Проверяем, есть ли открытое окно выбора портов
        for(NodeItem* node : m_nodes)
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
                auto* node = dynamic_cast<NodeItem*>(item);
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
            for(NodeItem* node : m_nodes)
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
        for(NodeItem* node : m_nodes)
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
        QList<NodeItem*> nodesToDelete;
        for(QGraphicsItem* it : selected)
        {
            auto* node = dynamic_cast<NodeItem*>(it);
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

// --------------------------- Coord helpers ---------------------------

QPointF UModernDiagramWidget::scenePosFromKernel(const QPointF& kernel) const
{
    return kernel * m_coordScale;
}

QPointF UModernDiagramWidget::kernelPosFromScene(const QPointF& scene) const
{
    return scene / m_coordScale;
}

bool UModernDiagramWidget::loadCoord(const QString& fullName, QPointF& outPos) const
{
    // Удалено избыточное логирование - создавало спам в INFO логах
    const char* coordRaw = Model_GetComponentParameterValue(fullName.toStdString().c_str(), "Coord");
    if(!coordRaw)
    {
        // Удалено избыточное логирование - создавало спам в INFO логах
        return false;
    }
    std::string coordBuf(coordRaw);
    // fallback: если пришла строка "x y z" без XML
    {
        std::istringstream iss(coordBuf);
        double x,y,z;
        if(iss >> x >> y >> z)
        {
            QPointF kernel(x,y);
            // Удалено избыточное логирование - создавало спам в INFO логах
            outPos = kernel; // возвращаем ядровые координаты, сцену вычисляем выше
            Engine_FreeBufString(coordRaw);
            return true;
        }
    }
    RDK::USerStorageXML xml;
    bool ok = xml.Load(coordBuf, "Coord");
    Engine_FreeBufString(coordRaw);
    if(!ok)
    {
        // Удалено избыточное логирование - создавало спам в INFO логах
        return false;
    }
    RDK::MVector<double,3> pos;
    xml >> pos;
    QPointF kernel(pos[0], pos[1]);
    // Удалено избыточное логирование - создавало спам в INFO логах
    outPos = kernel; // возвращаем ядровые координаты, сцену вычисляем выше
    return true;
}

void UModernDiagramWidget::saveCoord(const QString& fullName, const QPointF& scenePos) const
{
    QPointF kernelPos = kernelPosFromScene(scenePos);
    // нормализация: гарантируем неотрицательные координаты в ядре
    if(kernelPos.x() < 0) kernelPos.setX(0);
    if(kernelPos.y() < 0) kernelPos.setY(0);
    // Удалено избыточное логирование - создавало спам в INFO логах
    RDK::USerStorageXML xml;
    xml.Create("Coord");
    RDK::MVector<double,3> posVec;
    posVec[0] = kernelPos.x();
    posVec[1] = kernelPos.y();
    posVec[2] = 0.0;
    xml << posVec;
    std::string buffer;
    xml.Save(buffer);
    Model_SetComponentParameterValue(fullName.toStdString().c_str(), "Coord", buffer.c_str());
}

QPointF UModernDiagramWidget::currentMinScenePos() const
{
    QPointF min(0,0);
    bool first=true;
    for(auto* n : m_nodes)
    {
        QPointF p = n->scenePos();
        if(first)
        {
            min = p;
            first=false;
        }
        else
        {
            if(p.x() < min.x()) min.setX(p.x());
            if(p.y() < min.y()) min.setY(p.y());
        }
    }
    if(first) return QPointF(0,0);
    return min;
}

// --------------------------- Scene events ---------------------------

ModernScene::ModernScene(UModernDiagramWidget* owner)
    : m_owner(owner), m_isRubberBandActive(false), m_isGroupSelected(false), m_isGroupMoving(false), m_lastHoverPos()
{
    m_hoverTimer = new QTimer(this);
    m_hoverTimer->setInterval(80); // ~12 fps, достаточно для отслеживания
    connect(m_hoverTimer, &QTimer::timeout, this, &ModernScene::pollHover);
    m_hoverTimer->start();
}

void ModernScene::pollHover()
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
        UModernDiagramWidget::NodeItem* portNode = nullptr;
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
            for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
        for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
    for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
    {
        // Проверяем, находится ли узел в видимой области или под курсором
        QRectF nodeRect = node->sceneBoundingRect();
        if(visibleRect.intersects(nodeRect) || nodeRect.contains(scenePos))
        {
            node->refreshHoverAtScenePos(scenePos);
        }
    }
}

void ModernScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
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
        for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
        UModernDiagramWidget::NodeItem* node = nullptr;
        
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
            m_owner->m_tempLink = new UModernDiagramWidget::LinkItem(node, event->scenePos());
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
            m_owner->m_scene->clearSelection();
            node->setSelected(true);
            m_owner->m_contextMenuNode = node;
            QString fullName = m_owner->m_componentName.isEmpty() ? node->nodeName
                                                                  : m_owner->m_componentName + "." + node->nodeName;
            emit m_owner->componentSelected(fullName);
            // Показываем контекстное меню
            QPoint globalPos = m_owner->m_mainView->mapToGlobal(m_owner->m_mainView->mapFromScene(event->scenePos()));
            m_owner->m_contextMenu->popup(globalPos);
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
            QList<QGraphicsItem*> selectedItems = m_owner->m_scene->selectedItems();
            int selectedNodeCount = 0;
            for(QGraphicsItem* item : selectedItems)
            {
                auto* selectedNode = dynamic_cast<UModernDiagramWidget::NodeItem*>(item);
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
                    auto* selectedNode = dynamic_cast<UModernDiagramWidget::NodeItem*>(item);
                    if(selectedNode && selectedNode->isSelected())
                    {
                        m_savedSelection.append(selectedNode);
                    }
                }
                m_isGroupMoving = true;
                m_isGroupSelected = true;
            }
        }
    }
    
    // Проверяем, попали ли в порт
    if(clickedOnBackground)
    {
        QPointF portPos;
        UModernDiagramWidget::NodeItem* portNode = nullptr;
        const UModernDiagramWidget::Port* port = m_owner->pickPortDetailed(event->scenePos(), false, portNode, portPos);
        if(port)
        {
            clickedOnBackground = false;
        }
    }
    
    // Проверяем, попали ли в окно со списком портов
    if(clickedOnBackground)
    {
        for(UModernDiagramWidget::NodeItem* inner_node : m_owner->m_nodes)
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
        for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
        {
            node->hidePortListWidget();
        }
        // Если было состояние ожидания выбора порта, сбрасываем его
        if(m_owner->m_isWaitingForPortSelection)
        {
            m_owner->m_isWaitingForPortSelection = false;
            m_owner->m_isLineFrozen = false;
        }
        
        // Сбрасываем выделение группы при клике на фоне (левой или правой кнопкой)
        if(m_isGroupSelected || m_isGroupMoving)
        {
            m_owner->m_scene->clearSelection();
            m_isGroupSelected = false;
            m_isGroupMoving = false;
            m_savedSelection.clear();
        }
        
        // Если клик на фоне и ЛКМ без Ctrl (не прокрутка), передаем событие для RubberBandDrag
        // Ctrl+ЛКМ обрабатывается в ModernGraphicsView для прокрутки
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

void ModernScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_owner->m_tempLink)
    {
        m_owner->m_tempLink->updateGeometry(event->scenePos());
        event->accept();
        return;
    }
    
    // Сначала проверяем, есть ли видимое окно выбора портов - если есть, не обновляем временную линию
    bool hasVisiblePortListWidget = false;
    for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
        UModernDiagramWidget::NodeItem* portNode = nullptr;
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
            for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
            for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
        if(!item || (!dynamic_cast<UModernDiagramWidget::NodeItem*>(item) && 
                     !dynamic_cast<UModernDiagramWidget::LinkItem*>(item)))
        {
            QString tooltip = m_owner->generateCanvasTooltip();
            m_owner->m_mainView->setToolTip(tooltip);
        }
        else
        {
            // Clear tooltip when hovering over items (they have their own tooltips)
            m_owner->m_mainView->setToolTip(QString());
        }
    }
}

void ModernScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Устанавливаем флаг обработки событий мыши для пропуска pollHover
    m_isProcessingMouseEvent = true;
    
    // Сначала обрабатываем drag & drop связь (m_tempLink), если она активна
    if(m_owner->m_tempLink && event->button() == Qt::LeftButton)
    {
        QPointF portPos;
        UModernDiagramWidget::NodeItem* targetNode = nullptr;
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
        for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
            for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
        UModernDiagramWidget::NodeItem* targetNode = nullptr;
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
            UModernDiagramWidget::NodeItem* activeSourceNode = m_owner->m_activeSourceNode;
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
            if(!srcPortComponentName.isEmpty() && 
               srcPortComponentName != srcName)
            {
                srcProp = srcPortComponentName + "." + srcProp;
            }
            if(!dstPortComponentName.isEmpty() && 
               dstPortComponentName != dstName)
            {
                dstProp = dstPortComponentName + "." + dstProp;
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
            for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
        UModernDiagramWidget::NodeItem* targetNode = nullptr;
        const UModernDiagramWidget::Port* targetPort = m_owner->pickPortDetailed(event->scenePos(), true, targetNode, portPos);
        if(targetNode && targetPort && m_owner->m_dragSourceNode && m_owner->m_dragSourcePort &&
           targetNode != m_owner->m_dragSourceNode)
        {
            m_owner->m_scene->removeItem(m_owner->m_tempLink);
            delete m_owner->m_tempLink;
            auto* finalLink = new UModernDiagramWidget::LinkItem(m_owner->m_dragSourceNode, targetNode, true, true);
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
    QList<UModernDiagramWidget::NodeItem*> savedSelectedNodes;
    if(wasGroupMoving)
    {
        // Сохраняем все выделенные узлы перед вызовом базового класса
        QList<QGraphicsItem*> selectedItems = m_owner->m_scene->selectedItems();
        for(QGraphicsItem* item : selectedItems)
        {
            auto* node = dynamic_cast<UModernDiagramWidget::NodeItem*>(item);
            if(node && node->isSelected())
            {
                savedSelectedNodes.append(node);
            }
        }
    }
    
    // Вызываем базовый класс для обработки событий
    // Базовый класс QGraphicsView::mouseReleaseEvent уже был вызван в ModernGraphicsView,
    // и он передал событие в сцену, поэтому здесь мы обрабатываем событие на уровне сцены
    QGraphicsScene::mouseReleaseEvent(event);
    
    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Если группа была перемещена, восстанавливаем выделение,
    // так как базовый класс мог его сбросить
    // Используем отложенный вызов, чтобы восстановление произошло после всех обработчиков событий
    if(wasGroupMoving && !savedSelectedNodes.isEmpty())
    {
        // Сохраняем список узлов для отложенного восстановления
        QList<UModernDiagramWidget::NodeItem*> nodesToRestore = savedSelectedNodes;
        
        // Восстанавливаем выделение немедленно
        m_owner->m_isBatchSelecting = true;
        m_owner->m_scene->blockSignals(true);
        
        for(UModernDiagramWidget::NodeItem* node : nodesToRestore)
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
            // Восстанавливаем выделение для всех сохраненных узлов
            m_owner->m_isBatchSelecting = true;
            m_owner->m_scene->blockSignals(true);
            
            for(UModernDiagramWidget::NodeItem* node : nodesToRestore)
            {
                if(node)
                {
                    node->setSelected(true);
                }
            }
            
            m_owner->m_isBatchSelecting = false;
            m_owner->m_scene->blockSignals(false);
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
            auto* node = dynamic_cast<UModernDiagramWidget::NodeItem*>(item);
            if(node && node->isSelected())
            {
                selectedNodeCount++;
                selectedNodeNames << node->nodeName;
            }
        }
        
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
}

void ModernScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        // Сначала проверяем, попал ли двойной клик на окно выбора портов
        for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
        UModernDiagramWidget::NodeItem* portNode = nullptr;
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
                m_owner->m_activeTempLink = new UModernDiagramWidget::LinkItem(portNode, portPos, portPos);
                m_owner->m_scene->addItem(m_owner->m_activeTempLink);
                
                // Сбрасываем состояние заморозки при создании новой связи
                m_owner->m_isLineFrozen = false;
                
                // Закрываем все открытые деревья портов
                for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
                if(!m_owner->m_activeSourcePortComponentName.isEmpty() && 
                   m_owner->m_activeSourcePortComponentName != srcName)
                {
                    srcProp = m_owner->m_activeSourcePortComponentName + "." + srcProp;
                }
                if(!dstPortComponentName.isEmpty() && dstPortComponentName != dstName)
                {
                    dstProp = dstPortComponentName + "." + dstProp;
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
                for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
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
            m_owner->saveCurrentViewState();
            
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
            m_owner->saveCurrentViewState();
            
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

// --------------------------- Slots ---------------------------

void UModernDiagramWidget::componentDoubleClick(QString name)
{
    // Сохраняем состояние текущего компонента перед переходом
    saveCurrentViewState();
    
    SetComponentName(name);
    Reload();
}

void UModernDiagramWidget::componentSingleClick(QString name)
{
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
        m_scene->update();
    }
    
    // Обновляем стили кнопки сброса масштаба
    updateResetZoomButtonStyle();
}

void UModernDiagramWidget::updateResetZoomButtonStyle()
{
    if(!m_resetZoomButton)
        return;
    
    UStyleManager* styleManager = UStyleManager::instance();
    QString themeName = styleManager->getThemeName();
    
    if(themeName == "Modern Dark" || themeName == "dark")
    {
        // Темная тема
        m_resetZoomButton->setStyleSheet(
            "QPushButton {"
            "    background-color: rgba(33, 37, 43, 220);"
            "    border: 1px solid #5C6370;"
            "    border-radius: 4px;"
            "    font-size: 18px;"
            "    color: #ABB2BF;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(44, 49, 58, 240);"
            "    border-color: #61AFEF;"
            "    color: #61AFEF;"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(30, 58, 95, 250);"
            "    border-color: #61AFEF;"
            "    color: #61AFEF;"
            "}"
        );
    }
    else
    {
        // Светлая тема (по умолчанию)
        m_resetZoomButton->setStyleSheet(
            "QPushButton {"
            "    background-color: rgba(255, 255, 255, 200);"
            "    border: 1px solid #ccc;"
            "    border-radius: 4px;"
            "    font-size: 18px;"
            "    color: #374151;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(240, 240, 240, 220);"
            "    border-color: #5B8DEF;"
            "    color: #1E40AF;"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(220, 220, 220, 240);"
            "    border-color: #3B82F6;"
            "    color: #1E40AF;"
            "}"
        );
    }
}

void UModernDiagramWidget::selectComponent(QString name)
{
    // Если имя пустое, ничего не делаем
    if(name.isEmpty())
        return;
    
    // Если имя совпадает с текущим путём, значит мы на верхнем уровне
    if(!m_componentName.isEmpty() && name == m_componentName)
    {
        // Выделяем все узлы на текущем уровне (или ничего, если это корневой уровень)
        return;
    }
    
    // Определяем относительное имя компонента в текущем контексте
    QString componentName = name;
    
    if(m_componentName.isEmpty())
    {
        // Мы на корневом уровне, компонент должен быть прямым потомком
        // Берем только первую часть пути
        componentName = name.split(".").first();
    }
    else if(name.startsWith(m_componentName + "."))
    {
        // Компонент находится внутри текущего контекста
        componentName = name.mid(m_componentName.size() + 1);
    }
    else
    {
        // Компонент находится вне текущего контекста
        // Нужно перейти на нужный уровень
        // Определяем родительский путь компонента
        QStringList pathParts = name.split(".");
        if(pathParts.size() > 1)
        {
            pathParts.removeLast();
            QString parentPath = pathParts.join(".");
            // Переходим на родительский уровень
            saveCurrentViewState();
            SetComponentName(parentPath);
            Reload();
            // После перезагрузки выбираем компонент
            QTimer::singleShot(100, [this, name]() {
                selectComponent(name);
            });
            return;
        }
        else
        {
            // Компонент на корневом уровне, переходим туда
            saveCurrentViewState();
            SetComponentName("");
            Reload();
            QTimer::singleShot(100, [this, name]() {
                selectComponent(name);
            });
            return;
        }
    }
    
    // Найти узел и выделить его
    if(auto it = m_nodeByName.find(componentName); it != m_nodeByName.end())
    {
        m_scene->clearSelection();
        it.value()->setSelected(true);
        m_contextMenuNode = it.value();
        // Прокручиваем к выбранному узлу
        if(m_mainView)
            m_mainView->centerOn(it.value());
    }
}

// --------------------------- Context Menu ---------------------------

void UModernDiagramWidget::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    // Separators
    QAction* actionSeparator1 = new QAction(this);
    actionSeparator1->setSeparator(true);
    QAction* actionSeparator2 = new QAction(this);
    actionSeparator2->setSeparator(true);
    QAction* actionSeparator3 = new QAction(this);
    actionSeparator3->setSeparator(true);
    QAction* actionSeparator4 = new QAction(this);
    actionSeparator4->setSeparator(true);
    QAction* actionSeparator5 = new QAction(this);
    actionSeparator5->setSeparator(true);
    QAction* actionSeparator6 = new QAction(this);
    actionSeparator6->setSeparator(true);
    QAction* actionSeparator7 = new QAction(this);
    actionSeparator7->setSeparator(true);
    QAction* actionSeparator8 = new QAction(this);
    actionSeparator8->setSeparator(true);
    
    // Actions
    m_actionViewOrBreakLink = new QAction(m_contextMenu);
    m_actionViewOrBreakLink->setText("View/Break link");
    
    m_actionCreateLink = new QAction(m_contextMenu);
    m_actionCreateLink->setText("Create link");
    
    m_actionFinishLink = new QAction(m_contextMenu);
    m_actionFinishLink->setText("Finish link");
    m_actionFinishLink->setEnabled(false);
    
    m_actionCancelLink = new QAction(m_contextMenu);
    m_actionCancelLink->setText("Cancel link");
    m_actionCancelLink->setEnabled(false);
    
    m_actionStartMoving = new QAction(m_contextMenu);
    m_actionStartMoving->setText("Start moving");
    
    m_actionFinishMoving = new QAction(m_contextMenu);
    m_actionFinishMoving->setText("Finish moving");
    m_actionFinishMoving->setEnabled(false);
    
    m_actionCancelMoving = new QAction(m_contextMenu);
    m_actionCancelMoving->setText("Cancel moving");
    m_actionCancelMoving->setEnabled(false);
    
    m_actionSwitchLink = new QAction(m_contextMenu);
    m_actionSwitchLink->setText("Switch link");
    
    m_actionFinishSwitching = new QAction(m_contextMenu);
    m_actionFinishSwitching->setText("Finish switching");
    m_actionFinishSwitching->setEnabled(false);
    
    m_actionCancelSwitching = new QAction(m_contextMenu);
    m_actionCancelSwitching->setText("Cancel switching");
    m_actionCancelSwitching->setEnabled(false);
    
    m_actionCloneComponent = new QAction(m_contextMenu);
    m_actionCloneComponent->setText("Clone");
    
    m_actionQuickLink = new QAction(m_contextMenu);
    m_actionQuickLink->setText("Quick Link...");
    
    QAction* actionRenameComponent = new QAction(m_contextMenu);
    actionRenameComponent->setText("Rename");
    
    QAction* actionClassDescription = new QAction(m_contextMenu);
    actionClassDescription->setText("Class Description");
    
    QAction* actionDeleteComponent = new QAction(m_contextMenu);
    actionDeleteComponent->setText("Delete");
    
    QAction* actionCopyNameToClipboard = new QAction(m_contextMenu);
    actionCopyNameToClipboard->setText("Copy name to Clipboard");
    
    QAction* actionCopyLongNameToClipboard = new QAction(m_contextMenu);
    actionCopyLongNameToClipboard->setText("Copy long name to Clipboard");
    
    QAction* actionCopyClassNameToClipboard = new QAction(m_contextMenu);
    actionCopyClassNameToClipboard->setText("Copy class to Clipboard");
    
    QAction* actionResetComponent = new QAction(m_contextMenu);
    actionResetComponent->setText("Reset");
    
    QAction* actionCalculateComponent = new QAction(m_contextMenu);
    actionCalculateComponent->setText("Calculate");
    
    QAction* actionDefaultComponent = new QAction(m_contextMenu);
    actionDefaultComponent->setText("Default");
    
    QAction* actionGUI = new QAction(m_contextMenu);
    actionGUI->setText("GUI (not implemented)");
    actionGUI->setEnabled(false);
    
    QAction* actionCopyComponentXMLDescription = new QAction(m_contextMenu);
    actionCopyComponentXMLDescription->setText("Copy component XML description");
    
    QAction* actionClearComponentCache = new QAction(m_contextMenu);
    actionClearComponentCache->setText("Clear component cache");
    
    // Add actions to menu
    m_contextMenu->addAction(m_actionViewOrBreakLink);
    m_contextMenu->addAction(actionSeparator1);
    m_contextMenu->addAction(m_actionCreateLink);
    m_contextMenu->addAction(m_actionFinishLink);
    m_contextMenu->addAction(m_actionCancelLink);
    m_contextMenu->addAction(actionSeparator7);
    m_contextMenu->addAction(m_actionSwitchLink);
    m_contextMenu->addAction(m_actionFinishSwitching);
    m_contextMenu->addAction(m_actionCancelSwitching);
    m_contextMenu->addAction(actionSeparator2);
    m_contextMenu->addAction(m_actionStartMoving);
    m_contextMenu->addAction(m_actionFinishMoving);
    m_contextMenu->addAction(m_actionCancelMoving);
    m_contextMenu->addAction(actionSeparator3);
    m_contextMenu->addAction(actionRenameComponent);
    m_contextMenu->addAction(actionClassDescription);
    m_contextMenu->addAction(actionDeleteComponent);
    m_contextMenu->addAction(actionSeparator4);
    m_contextMenu->addAction(actionCopyNameToClipboard);
    m_contextMenu->addAction(actionCopyLongNameToClipboard);
    m_contextMenu->addAction(actionCopyClassNameToClipboard);
    m_contextMenu->addAction(actionSeparator5);
    m_contextMenu->addAction(actionResetComponent);
    m_contextMenu->addAction(actionCalculateComponent);
    m_contextMenu->addAction(actionDefaultComponent);
    m_contextMenu->addAction(actionSeparator6);
    m_contextMenu->addAction(actionGUI);
    m_contextMenu->addAction(actionCopyComponentXMLDescription);
    m_contextMenu->addAction(actionSeparator8);
    m_contextMenu->addAction(m_actionCloneComponent);
    m_contextMenu->addAction(m_actionQuickLink);
    m_contextMenu->addAction(actionClearComponentCache);
    
    // Connect signals
    connect(m_actionViewOrBreakLink, SIGNAL(triggered(bool)), this, SLOT(componentViewOrBreakLink()));
    connect(m_actionCreateLink, SIGNAL(triggered(bool)), this, SLOT(componentCreateLink()));
    connect(m_actionFinishLink, SIGNAL(triggered(bool)), this, SLOT(componentFinishLink()));
    connect(m_actionCancelLink, SIGNAL(triggered(bool)), this, SLOT(componentCancelLink()));
    connect(m_actionStartMoving, SIGNAL(triggered(bool)), this, SLOT(componentStartMoving()));
    connect(m_actionFinishMoving, SIGNAL(triggered(bool)), this, SLOT(componentFinishMoving()));
    connect(m_actionCancelMoving, SIGNAL(triggered(bool)), this, SLOT(componentCancelMoving()));
    connect(m_actionSwitchLink, SIGNAL(triggered(bool)), this, SLOT(componentStartSwitching()));
    connect(m_actionFinishSwitching, SIGNAL(triggered(bool)), this, SLOT(componentFinishSwitching()));
    connect(m_actionCancelSwitching, SIGNAL(triggered(bool)), this, SLOT(componentCancelSwitching()));
    connect(actionRenameComponent, SIGNAL(triggered(bool)), this, SLOT(componentRename()));
    connect(actionClassDescription, SIGNAL(triggered(bool)), this, SLOT(actionClassDescriptionTriggered()));
    connect(actionDeleteComponent, SIGNAL(triggered(bool)), this, SLOT(componentDelete()));
    connect(actionCopyNameToClipboard, SIGNAL(triggered(bool)), this, SLOT(componentCopyNameToClipboard()));
    connect(actionCopyLongNameToClipboard, SIGNAL(triggered(bool)), this, SLOT(componentCopyLongNameToClipboard()));
    connect(actionCopyClassNameToClipboard, SIGNAL(triggered(bool)), this, SLOT(componentCopyClassNameToClipboard()));
    connect(actionResetComponent, SIGNAL(triggered(bool)), this, SLOT(componentReset()));
    connect(actionCalculateComponent, SIGNAL(triggered(bool)), this, SLOT(componentCalculate()));
    connect(actionDefaultComponent, SIGNAL(triggered(bool)), this, SLOT(componentDefault()));
    connect(actionGUI, SIGNAL(triggered(bool)), this, SLOT(componentGUI()));
    connect(actionCopyComponentXMLDescription, SIGNAL(triggered(bool)), this, SLOT(componentCopyXMLDescription()));
    connect(m_actionCloneComponent, SIGNAL(triggered(bool)), this, SLOT(componentCloneComponent()));
    connect(m_actionQuickLink, SIGNAL(triggered(bool)), this, SLOT(componentQuickLink()));
    connect(actionClearComponentCache, SIGNAL(triggered(bool)), this, SLOT(componentClearCache()));
}

QString UModernDiagramWidget::getSelectedComponentLongName() const
{
    if(!m_contextMenuNode)
        return QString();
    return m_componentName.isEmpty() ? m_contextMenuNode->nodeName
                                     : m_componentName + "." + m_contextMenuNode->nodeName;
}

// --------------------------- Context Menu Slots ---------------------------

void UModernDiagramWidget::componentViewOrBreakLink()
{
    emit viewLinks(getSelectedComponentLongName());
}

void UModernDiagramWidget::componentCreateLink()
{
    m_firstComponentToConnection = getSelectedComponentLongName();
    m_actionViewOrBreakLink->setEnabled(false);
    m_actionCreateLink->setEnabled(false);
    m_actionFinishLink->setEnabled(true);
    m_actionCancelLink->setEnabled(true);
    
    m_actionStartMoving->setEnabled(false);
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);
    
    m_actionSwitchLink->setEnabled(false);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramWidget::componentFinishLink()
{
    emit createLinks(m_firstComponentToConnection, getSelectedComponentLongName());
    componentCancelLink();
}

void UModernDiagramWidget::componentCancelLink()
{
    m_actionViewOrBreakLink->setEnabled(true);
    m_actionCreateLink->setEnabled(true);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);
    
    m_actionStartMoving->setEnabled(true);
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);
    
    m_actionSwitchLink->setEnabled(true);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramWidget::componentStartSwitching()
{
    m_startSwitchComponent = getSelectedComponentLongName();
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);
    m_actionStartMoving->setEnabled(false);
    
    m_actionCreateLink->setEnabled(false);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);
    
    m_actionSwitchLink->setEnabled(false);
    m_actionFinishSwitching->setEnabled(true);
    m_actionCancelSwitching->setEnabled(true);
}

void UModernDiagramWidget::componentFinishSwitching()
{
    emit switchLinks(m_startSwitchComponent, getSelectedComponentLongName());
    componentCancelSwitching();
}

void UModernDiagramWidget::componentCancelSwitching()
{
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);
    m_actionStartMoving->setEnabled(true);
    
    m_actionCreateLink->setEnabled(true);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);
    
    m_actionSwitchLink->setEnabled(true);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramWidget::componentStartMoving()
{
    m_startMoveComponent = getSelectedComponentLongName();
    m_actionFinishMoving->setEnabled(true);
    m_actionCancelMoving->setEnabled(true);
    m_actionStartMoving->setEnabled(false);
    
    m_actionCreateLink->setEnabled(false);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);
    
    m_actionSwitchLink->setEnabled(false);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramWidget::componentFinishMoving()
{
    QString endMoveComponent = getSelectedComponentLongName();
    
    if(Model_MoveComponent(m_startMoveComponent.toStdString().c_str(), endMoveComponent.toStdString().c_str()) != RDK_SUCCESS)
        QMessageBox::critical(this, "Error", "Component move error", QMessageBox::Ok);
    
    Reload();
    emit updateComponentsList();
    componentCancelMoving();
}

void UModernDiagramWidget::componentCancelMoving()
{
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);
    m_actionStartMoving->setEnabled(true);
    
    m_actionCreateLink->setEnabled(true);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);
    
    m_actionSwitchLink->setEnabled(true);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramWidget::componentRename()
{
    if(!m_contextMenuNode)
        return;
        
    bool ok;
    QString text = QInputDialog::getText(this, tr("Rename component"),
                                         tr("Enter new component name: "), QLineEdit::Normal,
                                         m_contextMenuNode->nodeName, &ok);
    if (ok && !text.isEmpty())
    {
        std::string new_name(text.toLocal8Bit().constData());
        Model_SetComponentPropertyData(getSelectedComponentLongName().toLocal8Bit().constData(), "Name", &new_name);
        
        emit updateComponentsList();
        Reload();
        // Выбрать компонент с новым именем
        QString newFullName = m_componentName.isEmpty() ? text : m_componentName + "." + text;
        selectComponent(newFullName);
        emit componentSelected(newFullName);
    }
}

void UModernDiagramWidget::actionClassDescriptionTriggered()
{
    if(!m_contextMenuNode)
        return;
        
    const char* class_name = Model_GetComponentClassName(getSelectedComponentLongName().toLocal8Bit().constData());
    if(class_name && strlen(class_name) > 0)
    {
        QMainWindow* classDescWindow = new QMainWindow(this);
        classDescWindow->setAttribute(Qt::WA_DeleteOnClose);
        UClassDescriptionDisplay* display = new UClassDescriptionDisplay(std::string(class_name));
        classDescWindow->setCentralWidget(display);
        classDescWindow->setWindowTitle("Class Description");
        
        classDescWindow->resize(display->size());
        display->show();
        classDescWindow->showNormal();
        classDescWindow->activateWindow();
    }
    Engine_FreeBufString(class_name);
}

void UModernDiagramWidget::deleteComponents(const QList<NodeItem*>& nodesToDelete)
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
        if(reply == QMessageBox::Cancel)
            return;
    }
    
    // Удаляем все компоненты
    for(NodeItem* node : nodesToDelete)
    {
        QString fullName = m_componentName.isEmpty() ? node->nodeName
                                                     : m_componentName + "." + node->nodeName;
        Model_DelComponent("", fullName.toLocal8Bit().constData());
    }
    
    // Очищаем сохраненные позиции удаленных узлов
    for(NodeItem* node : nodesToDelete)
    {
        m_lastNodePositions.remove(node);
    }
    
    Reload();
    emit updateComponentsList();
}

void UModernDiagramWidget::componentDelete()
{
    if(!m_contextMenuNode)
        return;
    
    QList<NodeItem*> nodesToDelete;
    nodesToDelete.append(m_contextMenuNode);
    deleteComponents(nodesToDelete);
}

void UModernDiagramWidget::componentCopyNameToClipboard()
{
    if(!m_contextMenuNode)
        return;
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_contextMenuNode->nodeName);
}

void UModernDiagramWidget::componentCopyLongNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(getSelectedComponentLongName());
}

void UModernDiagramWidget::componentCopyClassNameToClipboard()
{
    if(!m_contextMenuNode)
        return;
    const char *className = Model_GetComponentClassName(getSelectedComponentLongName().toLocal8Bit().constData());
    if(className)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString(className));
    }
    Engine_FreeBufString(className);
}

void UModernDiagramWidget::componentReset()
{
    Env_Reset(getSelectedComponentLongName().toLocal8Bit().constData());
    emit updateComponentsList();
}

void UModernDiagramWidget::componentCalculate()
{
    Env_Calculate(getSelectedComponentLongName().toLocal8Bit().constData());
    // TODO: обновить интерфейс если нужно
}

void UModernDiagramWidget::componentDefault()
{
    QString selectedComponentLongName = getSelectedComponentLongName();
    if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", 
            "Are you sure you want to reset all parameters for component " + selectedComponentLongName + " to default values?", 
            QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) 
            return;
    }
    
    RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
    std::string stringid = selectedComponentLongName.toLocal8Bit().constData();
    RDK::UEPtr<RDK::UNet> object;
    if(stringid.empty())
        object = RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetModel());
    else
        object = RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetEngine()->FindComponent(stringid.c_str()));
    
    RDK::UEPtr<RDK::UNet> owner = RDK::dynamic_pointer_cast<RDK::UNet>(object->GetOwner());
    RDK::UStringLinksList links_list;
    
    if(owner)
        object->GetLinks(links_list, owner, true, object);
    storage->DefaultObject(object);
    if(owner)
        object->CreateLinks(links_list, owner);
    
    Reload();
    emit updateComponentsList();
}

void UModernDiagramWidget::componentGUI()
{
    // Не реализовано
}

void UModernDiagramWidget::componentCopyXMLDescription()
{
    const char *xmlDescription = Model_SaveComponent(getSelectedComponentLongName().toLocal8Bit().constData());
    if(xmlDescription)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString(xmlDescription));
    }
    Engine_FreeBufString(xmlDescription);
}

void UModernDiagramWidget::componentCloneComponent()
{
    int res = Model_CloneComponent(getSelectedComponentLongName().toLocal8Bit().constData(), "");
    if(res == RDK_SUCCESS)
    {
        Reload();
        emit updateComponentsList();
    }
}

void UModernDiagramWidget::componentQuickLink()
{
    if(!m_contextMenuNode)
        return;
        
    UQuickLinkDialog dialog(this, m_componentName, m_application);
    
    if(dialog.exec() == QDialog::Accepted)
    {
        QString srcComp = dialog.getSourceComponent();
        QString srcProp = dialog.getSourceProperty();
        QString dstComp = dialog.getTargetComponent();
        QString dstProp = dialog.getTargetProperty();
        
        if(!srcProp.isEmpty() && !dstProp.isEmpty())
        {
            int result = Model_CreateLinkByName(srcComp.toStdString().c_str(),
                                               srcProp.toStdString().c_str(),
                                               dstComp.toStdString().c_str(),
                                               dstProp.toStdString().c_str());
            
            if(result == RDK_SUCCESS)
            {
                Reload();
                emit updateComponentsList();
            }
        }
    }
}

void UModernDiagramWidget::componentClearCache()
{
    int ret = QMessageBox::question(this, tr("Clear Component Cache"), 
                                    tr("Are you sure you want to clear the component cache? This will remove all cached component information."),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::Yes)
    {
        clearComponentCache();
        QMessageBox::information(this, tr("Cache Cleared"), tr("Component cache has been cleared successfully."));
    }
}

// --------------------------- Viewport State Management ---------------------------

void UModernDiagramWidget::saveCurrentViewState()
{
    if(m_componentName.isEmpty() || !m_mainView)
        return;
    
    ViewState state;
    QTransform transform = m_mainView->transform();
    state.scale = transform.m11();  // Масштаб по X (обычно равен масштабу по Y)
    state.center = m_mainView->mapToScene(m_mainView->viewport()->rect().center());
    state.isValid = true;
    
    m_viewStates[m_componentName] = state;
}

void UModernDiagramWidget::restoreViewState(const QString& componentName)
{
    if(componentName.isEmpty() || !m_mainView || m_scene->items().isEmpty())
        return;
    
    // Проверяем, есть ли сохраненное состояние для этого компонента
    if(m_viewStates.contains(componentName))
    {
        const ViewState& state = m_viewStates[componentName];
        if(state.isValid)
        {
            // Восстанавливаем масштаб
            m_mainView->resetTransform();
            m_mainView->scale(state.scale, state.scale);
            
            // Восстанавливаем центр
            m_mainView->centerOn(state.center);
            return;
        }
    }
    
    // Если сохраненного состояния нет, устанавливаем начальный масштаб 2.5x
    QRectF bounds = m_scene->itemsBoundingRect();
    if(!bounds.isNull())
    {
        QRectF padded = bounds.adjusted(-200, -200, 200, 200);
        m_scene->setSceneRect(padded);
        
        // Устанавливаем начальный масштаб 2.5x
        m_mainView->resetTransform();
        m_mainView->scale(DEFAULT_SCALE, DEFAULT_SCALE);
        
        // Центрируем на содержимом
        m_mainView->centerOn(bounds.center());
        
        // Сохраняем это состояние
        ViewState state;
        state.scale = DEFAULT_SCALE;
        state.center = bounds.center();
        state.isValid = true;
        m_viewStates[componentName] = state;
    }
}

void UModernDiagramWidget::resetZoom()
{
    if(!m_mainView || m_scene->items().isEmpty())
        return;
    
    QRectF bounds = m_scene->itemsBoundingRect();
    if(!bounds.isNull())
    {
        // Сбрасываем масштаб к начальному значению 2.5x
        m_mainView->resetTransform();
        m_mainView->scale(DEFAULT_SCALE, DEFAULT_SCALE);
        
        // Центрируем на содержимом
        m_mainView->centerOn(bounds.center());
        
        // Обновляем сохраненное состояние
        if(!m_componentName.isEmpty())
        {
            ViewState state;
            state.scale = DEFAULT_SCALE;
            state.center = bounds.center();
            state.isValid = true;
            m_viewStates[m_componentName] = state;
        }
    }
}

void UModernDiagramWidget::onResetZoomClicked()
{
    resetZoom();
}

void UModernDiagramWidget::SaveViewState()
{
    if(!m_application)
        return;
    
    // Сохраняем текущее состояние перед сохранением
    saveCurrentViewState();
    
    QSettings settings(QString::fromLocal8Bit(
                         m_application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup("UModernDiagramWidget_ViewStates");
    
    // Сохраняем количество состояний
    settings.setValue("count", m_viewStates.size());
    
    // Сохраняем каждое состояние
    int index = 0;
    for(auto it = m_viewStates.begin(); it != m_viewStates.end(); ++it, ++index)
    {
        QString key = QString("component_%1").arg(index);
        settings.setValue(key + "_name", it.key());
        settings.setValue(key + "_scale", it.value().scale);
        settings.setValue(key + "_center_x", it.value().center.x());
        settings.setValue(key + "_center_y", it.value().center.y());
        settings.setValue(key + "_valid", it.value().isValid);
    }
    
    settings.endGroup();
}

void UModernDiagramWidget::LoadViewState()
{
    if(!m_application)
        return;
    
    QSettings settings(QString::fromLocal8Bit(
                         m_application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup("UModernDiagramWidget_ViewStates");
    
    int count = settings.value("count", 0).toInt();
    m_viewStates.clear();
    
    // Загружаем каждое состояние
    for(int i = 0; i < count; ++i)
    {
        QString key = QString("component_%1").arg(i);
        QString name = settings.value(key + "_name").toString();
        if(!name.isEmpty())
        {
            ViewState state;
            state.scale = settings.value(key + "_scale", DEFAULT_SCALE).toDouble();
            state.center.setX(settings.value(key + "_center_x", 0.0).toDouble());
            state.center.setY(settings.value(key + "_center_y", 0.0).toDouble());
            state.isValid = settings.value(key + "_valid", false).toBool();
            
            if(state.isValid)
            {
                m_viewStates[name] = state;
            }
        }
    }
    
    settings.endGroup();
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
    QList<NodeItem*> nodesToSelect;
    
    for(NodeItem* node : m_nodes)
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
        m_isBatchSelecting = true;
        
        // ДИАГНОСТИКА: Проверяем выделение ПЕРЕД установкой для всех узлов
        QList<QGraphicsItem*> beforeBatchSelect = m_scene->selectedItems();
        int beforeBatchSelectCount = 0;
        QStringList beforeBatchSelectNames;
        for(QGraphicsItem* item : beforeBatchSelect)
        {
            auto* n = dynamic_cast<NodeItem*>(item);
            if(n && n->isSelected())
            {
                beforeBatchSelectCount++;
                beforeBatchSelectNames << n->nodeName;
            }
        }
        QString beforeBatchMsg = QString("selectNodesInRect: ПЕРЕД batch setSelected выделено %1 объектов: %2")
            .arg(beforeBatchSelectCount).arg(beforeBatchSelectNames.join(", "));
        
        // Устанавливаем выделение для всех узлов одновременно
        for(NodeItem* node : nodesToSelect)
        {
            node->setSelected(true);
        }
        
        // Сбрасываем флаг batch-выделения и разблокируем сигналы сцены
        m_isBatchSelecting = false;
        m_scene->blockSignals(false);
        
        // ДИАГНОСТИКА: Проверяем выделение ПОСЛЕ установки для всех узлов
        QList<QGraphicsItem*> afterBatchSelect = m_scene->selectedItems();
        int afterBatchSelectCount = 0;
        QStringList afterBatchSelectNames;
        for(QGraphicsItem* item : afterBatchSelect)
        {
            auto* n = dynamic_cast<NodeItem*>(item);
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
            for(NodeItem* n : nodesToSelect) expectedNames << n->nodeName;
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

// --------------------------- Tooltip generation methods ---------------------------

QString UModernDiagramWidget::getPortCategoryName(PortCategory category) const
{
    switch(category)
    {
    case PortCategory::Own:
        return tr("Own");
    case PortCategory::Child:
        return tr("Child");
    case PortCategory::Alias:
        return tr("Alias");
    default:
        return tr("Unknown");
    }
}

QString UModernDiagramWidget::generateNodeTooltip(NodeItem* node) const
{
    if(!node)
        return QString();
    
    QString tooltip = tr(
        "<b>%1</b><br/>"
        "<i>Class: %2</i><br/><br/>"
        "<b>Actions:</b><br/>"
        "• Left Click - Select<br/>"
        "• Double Left Click - Enter component<br/>"
        "• Right Click - Context menu<br/>"
        "• Drag - Move node<br/>"
        "• Drag selected - Move group<br/><br/>"
        "<b>Keys:</b><br/>"
        "• Delete - Delete (with confirmation)<br/>"
        "• Shift + Delete - Delete without confirmation<br/>"
        "• Esc - Cancel operation"
    ).arg(node->nodeName, node->className);
    
    return tooltip;
}

QString UModernDiagramWidget::generatePortTooltip(const Port& port) const
{
    QString portType = port.isInput ? tr("Input") : tr("Output");
    QString categoryName = getPortCategoryName(port.category);
    QString actionText = port.isInput 
        ? tr("• Left Click - Complete connection")
        : tr("• Left Click - Start connection");
    
    QString tooltip = tr(
        "<b>%1</b><br/>"
        "<i>%2</i><br/>"
        "Type: %3 | Category: %4<br/><br/>"
        "<b>Actions:</b><br/>"
        "%5<br/>"
        "• Shift + Left Click - Show nested ports<br/>"
        "• Enter - Select port (in selection window)"
    ).arg(port.displayName, port.fullPath, portType, categoryName, actionText);
    
    return tooltip;
}

QString UModernDiagramWidget::generateLinkTooltip(LinkItem* link) const
{
    if(!link || !link->getSourceNode())
        return QString();
    
    QString srcName = link->getSourceNode()->nodeName;
    NodeItem* dstNode = link->getDestinationNode();
    QString dstName = dstNode ? dstNode->nodeName : tr("(temporary)");
    
    QString tooltip = tr(
        "<b>Connection</b><br/>"
        "From: %1<br/>"
        "To: %2<br/><br/>"
        "<b>Actions:</b><br/>"
        "• Right Click - Context menu"
    ).arg(srcName, dstName);
    
    return tooltip;
}

QString UModernDiagramWidget::generateCanvasTooltip() const
{
    QString tooltip = tr(
        "<b>Diagram Canvas</b><br/><br/>"
        "<b>Selection:</b><br/>"
        "• Left Click + Drag - Select rectangle<br/>"
        "• Shift + Left Click + Drag - Add to selection<br/><br/>"
        "<b>Navigation:</b><br/>"
        "• Ctrl + Left Click + Drag - Pan canvas<br/>"
        "• Mouse Wheel - Zoom<br/>"
        "• Double Right Click - Go up level<br/><br/>"
        "<b>Connections:</b><br/>"
        "• Right Click - Cancel connection"
    );
    
    return tooltip;
}

// --------------------------- ComponentCache ---------------------------

UModernDiagramWidget::ComponentCacheEntry* UModernDiagramWidget::ComponentCache::getEntry(const QString& componentFullName)
{
    if(m_cache.contains(componentFullName))
        return &m_cache[componentFullName];
    return nullptr;
}

void UModernDiagramWidget::ComponentCache::setEntry(const QString& componentFullName, const ComponentCacheEntry& entry)
{
    m_cache[componentFullName] = entry;
}

bool UModernDiagramWidget::ComponentCache::hasEntry(const QString& componentFullName) const
{
    return m_cache.contains(componentFullName);
}

void UModernDiagramWidget::ComponentCache::invalidateEntry(const QString& componentFullName)
{
    m_cache.remove(componentFullName);
}

void UModernDiagramWidget::ComponentCache::clear()
{
    m_cache.clear();
}

// --------------------------- File Cache Methods ---------------------------

QString UModernDiagramWidget::getCacheFilePath(const QString& extension) const
{
    if(!m_application)
        return QString();
    
    QString cacheDirPath;
    
    // Пытаемся использовать путь к текущему открытому проекту
    QString projectPath = QString::fromLocal8Bit(m_application->GetProjectPath().c_str());
    if(!projectPath.isEmpty())
    {
        // Используем папку текущего проекта
        QDir projectDir(projectPath);
        cacheDirPath = projectDir.absoluteFilePath(".cache");
    }
    else
    {
        // Fallback: используем старую логику, если проект не открыт
        QString configsPath = QString::fromLocal8Bit(m_application->GetConfigsMainPath().c_str());
        QString workDir = QString::fromLocal8Bit(m_application->GetWorkDirectory().c_str());
        
        // Определяем полный путь к папке конфигураций
        QString fullConfigsPath;
        if(QDir::isAbsolutePath(configsPath))
        {
            fullConfigsPath = configsPath;
        }
        else
        {
            // Относительный путь - относительно рабочей директории
            QDir workDirObj(workDir);
            fullConfigsPath = workDirObj.absoluteFilePath(configsPath);
        }
        
        // Получаем имя проекта из имени файла приложения или используем "default"
        QString projectName = "default";
        QString appFileName = QString::fromLocal8Bit(m_application->GetApplicationFileName().c_str());
        if(!appFileName.isEmpty())
        {
            QFileInfo fileInfo(appFileName);
            projectName = fileInfo.baseName();
            if(projectName.isEmpty())
                projectName = "default";
        }
        
        // Создаем путь к папке кэша: {ConfigsMainPath}/{ProjectName}/.cache/
        QDir configsDir(fullConfigsPath);
        cacheDirPath = configsDir.absoluteFilePath(projectName + "/.cache");
    }
    
    // Создаем папку кэша, если она не существует
    QDir cacheDir(cacheDirPath);
    if(!cacheDir.exists())
    {
        cacheDir.mkpath(".");
    }
    
    // Возвращаем путь к файлу кэша
    return cacheDir.absoluteFilePath("component_cache." + extension);
}

bool UModernDiagramWidget::saveComponentCacheToFile(const QString& filePath, bool useBinary) const
{
    if(useBinary)
    {
        // Бинарный формат
        QFile file(filePath);
        if(!file.open(QIODevice::WriteOnly))
            return false;
        
        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_5_15);
        
        // Заголовок: магическое число и версия
        const quint32 MAGIC = 0x4E4D5344; // "NMSD" (NeuroModeler SDK)
        const quint32 VERSION = 1;
        stream << MAGIC << VERSION;
        
        const QHash<QString, ComponentCacheEntry>& entries = m_componentCache.getAllEntries();
        stream << static_cast<quint32>(entries.size());
        
        for(auto it = entries.begin(); it != entries.end(); ++it)
        {
            const QString& componentName = it.key();
            const ComponentCacheEntry& entry = it.value();
            
            // Имя компонента
            stream << componentName;
            
            // Метаданные
            stream << entry.timestamp;
            stream << entry.hash;
            stream << entry.className;
            stream << entry.hasKernelPos;
            if(entry.hasKernelPos)
            {
                stream << entry.kernelPos.x() << entry.kernelPos.y();
            }
            
            // Вспомогательная функция для сериализации портов
            auto serializePorts = [&stream](const QVector<Port>& ports)
            {
                stream << static_cast<quint32>(ports.size());
                for(const Port& port : ports)
                {
                    stream << port.pos.x() << port.pos.y();
                    stream << port.isInput;
                    stream << port.name;
                    stream << port.fullPath;
                    stream << port.componentName;
                    stream << port.displayName;
                    stream << static_cast<quint32>(port.category);
                }
            };
            
            // Сериализуем все массивы портов
            serializePorts(entry.ownInputPorts);
            serializePorts(entry.childInputPorts);
            serializePorts(entry.aliasInputPorts);
            serializePorts(entry.ownOutputPorts);
            serializePorts(entry.childOutputPorts);
            serializePorts(entry.aliasOutputPorts);
            
            // Сериализуем portCategoryCache
            stream << static_cast<quint32>(entry.portCategoryCache.size());
            for(auto cacheIt = entry.portCategoryCache.begin(); cacheIt != entry.portCategoryCache.end(); ++cacheIt)
            {
                stream << cacheIt.key().first;  // propertyName
                stream << cacheIt.key().second; // isInput
                stream << static_cast<quint32>(cacheIt.value()); // category
            }
        }
        
        file.close();
        return true;
    }
    
    // JSON формат
    QJsonObject root;
    root["version"] = 1;
    
    QJsonObject components;
    const QHash<QString, ComponentCacheEntry>& entries = m_componentCache.getAllEntries();
    for(auto it = entries.begin(); it != entries.end(); ++it)
    {
        const QString& componentName = it.key();
        const ComponentCacheEntry& entry = it.value();
        
        QJsonObject componentObj;
        componentObj["timestamp"] = entry.timestamp;
        componentObj["hash"] = entry.hash;
        
        // Сохраняем данные, используемые в buildScene
        componentObj["className"] = entry.className;
        componentObj["hasKernelPos"] = entry.hasKernelPos;
        if(entry.hasKernelPos)
        {
            QJsonObject kernelPosObj;
            kernelPosObj["x"] = entry.kernelPos.x();
            kernelPosObj["y"] = entry.kernelPos.y();
            componentObj["kernelPos"] = kernelPosObj;
        }
        
        QJsonObject portsObj;
        
        // Сериализуем порты
        QJsonArray ownInputArray;
        for(const Port& port : entry.ownInputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            ownInputArray.append(portObj);
        }
        portsObj["ownInput"] = ownInputArray;
        
        QJsonArray childInputArray;
        for(const Port& port : entry.childInputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            childInputArray.append(portObj);
        }
        portsObj["childInput"] = childInputArray;
        
        QJsonArray aliasInputArray;
        for(const Port& port : entry.aliasInputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            aliasInputArray.append(portObj);
        }
        portsObj["aliasInput"] = aliasInputArray;
        
        QJsonArray ownOutputArray;
        for(const Port& port : entry.ownOutputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            ownOutputArray.append(portObj);
        }
        portsObj["ownOutput"] = ownOutputArray;
        
        QJsonArray childOutputArray;
        for(const Port& port : entry.childOutputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            childOutputArray.append(portObj);
        }
        portsObj["childOutput"] = childOutputArray;
        
        QJsonArray aliasOutputArray;
        for(const Port& port : entry.aliasOutputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            aliasOutputArray.append(portObj);
        }
        portsObj["aliasOutput"] = aliasOutputArray;
        
        componentObj["ports"] = portsObj;
        components[componentName] = componentObj;
    }
    
    root["components"] = components;
    
    QJsonDocument doc(root);
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    
    file.write(doc.toJson());
    file.close();
    
    return true;
}

bool UModernDiagramWidget::loadComponentCacheFromFile(const QString& filePath, bool useBinary)
{
    if(useBinary)
    {
        // Бинарный формат
        QFile file(filePath);
        if(!file.exists() || !file.open(QIODevice::ReadOnly))
            return false;
        
        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_5_15);
        
        // Проверяем заголовок
        quint32 magic, version;
        stream >> magic >> version;
        if(magic != 0x4E4D5344 || version != 1) // "NMSD" и версия 1
        {
            file.close();
            return false;
        }
        
        quint32 componentCount;
        stream >> componentCount;
        
        QHash<QString, ComponentCacheEntry> entries;
        
        for(quint32 i = 0; i < componentCount; ++i)
        {
            ComponentCacheEntry entry;
            QString componentName;
            
            // Имя компонента
            stream >> componentName;
            
            // Метаданные
            stream >> entry.timestamp;
            stream >> entry.hash;
            stream >> entry.className;
            stream >> entry.hasKernelPos;
            if(entry.hasKernelPos)
            {
                qreal x, y;
                stream >> x >> y;
                entry.kernelPos.setX(x);
                entry.kernelPos.setY(y);
            }
            
            // Вспомогательная функция для десериализации портов
            auto deserializePorts = [&stream](QVector<Port>& ports)
            {
                quint32 count;
                stream >> count;
                ports.reserve(count);
                for(quint32 j = 0; j < count; ++j)
                {
                    Port port;
                    qreal x, y;
                    stream >> x >> y;
                    port.pos.setX(x);
                    port.pos.setY(y);
                    stream >> port.isInput;
                    stream >> port.name;
                    stream >> port.fullPath;
                    stream >> port.componentName;
                    stream >> port.displayName;
                    quint32 category;
                    stream >> category;
                    port.category = static_cast<PortCategory>(category);
                    ports.append(port);
                }
            };
            
            // Десериализуем все массивы портов
            deserializePorts(entry.ownInputPorts);
            deserializePorts(entry.childInputPorts);
            deserializePorts(entry.aliasInputPorts);
            deserializePorts(entry.ownOutputPorts);
            deserializePorts(entry.childOutputPorts);
            deserializePorts(entry.aliasOutputPorts);
            
            // Десериализуем portCategoryCache
            quint32 cacheSize;
            stream >> cacheSize;
            for(quint32 j = 0; j < cacheSize; ++j)
            {
                QString propertyName;
                bool isInput;
                quint32 category;
                stream >> propertyName >> isInput >> category;
                entry.portCategoryCache.insert(qMakePair(propertyName, isInput), static_cast<PortCategory>(category));
            }
            
            entries[componentName] = entry;
        }
        
        file.close();
        
        if(stream.status() != QDataStream::Ok)
        {
            QString logMsg = QString("[UModernDiagramWidget] Failed to load component cache from binary file: %1 (stream error)").arg(filePath);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            return false;
        }
        
        m_componentCache.setAllEntries(entries);
        QString logMsg = QString("[UModernDiagramWidget] Component cache loaded from binary file: %1 (%2 entries)")
            .arg(filePath).arg(entries.size());
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        return true;
    }
    
    // JSON формат
    QFile file(filePath);
    if(!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    
    QByteArray rawData = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(rawData, &error);
    if(error.error != QJsonParseError::NoError || !doc.isObject())
        return false;
    
    QJsonObject root = doc.object();
    int version = root["version"].toInt();
    if(version != 1)
        return false;  // Неподдерживаемая версия
    
    QJsonObject components = root["components"].toObject();
    QHash<QString, ComponentCacheEntry> entries;
    
    for(auto it = components.begin(); it != components.end(); ++it)
    {
        const QString& componentName = it.key();
        QJsonObject componentObj = it.value().toObject();
        
        ComponentCacheEntry entry;
        entry.timestamp = componentObj["timestamp"].toVariant().toLongLong();
        entry.hash = componentObj["hash"].toString();
        
        // Восстанавливаем данные, используемые в buildScene
        entry.className = componentObj["className"].toString();
        entry.hasKernelPos = componentObj["hasKernelPos"].toBool(false);
        if(entry.hasKernelPos && componentObj.contains("kernelPos"))
        {
            QJsonObject kernelPosObj = componentObj["kernelPos"].toObject();
            entry.kernelPos.setX(kernelPosObj["x"].toDouble());
            entry.kernelPos.setY(kernelPosObj["y"].toDouble());
        }
        
        QJsonObject portsObj = componentObj["ports"].toObject();
        
        // Десериализуем порты
        QJsonArray ownInputArray = portsObj["ownInput"].toArray();
        for(const QJsonValue& val : ownInputArray)
        {
            QJsonObject portObj = val.toObject();
            Port port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<PortCategory>(portObj["category"].toInt());
            entry.ownInputPorts.append(port);
        }
        
        QJsonArray childInputArray = portsObj["childInput"].toArray();
        for(const QJsonValue& val : childInputArray)
        {
            QJsonObject portObj = val.toObject();
            Port port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<PortCategory>(portObj["category"].toInt());
            entry.childInputPorts.append(port);
        }
        
        QJsonArray aliasInputArray = portsObj["aliasInput"].toArray();
        for(const QJsonValue& val : aliasInputArray)
        {
            QJsonObject portObj = val.toObject();
            Port port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<PortCategory>(portObj["category"].toInt());
            entry.aliasInputPorts.append(port);
        }
        
        QJsonArray ownOutputArray = portsObj["ownOutput"].toArray();
        for(const QJsonValue& val : ownOutputArray)
        {
            QJsonObject portObj = val.toObject();
            Port port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<PortCategory>(portObj["category"].toInt());
            entry.ownOutputPorts.append(port);
        }
        
        QJsonArray childOutputArray = portsObj["childOutput"].toArray();
        for(const QJsonValue& val : childOutputArray)
        {
            QJsonObject portObj = val.toObject();
            Port port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<PortCategory>(portObj["category"].toInt());
            entry.childOutputPorts.append(port);
        }
        
        QJsonArray aliasOutputArray = portsObj["aliasOutput"].toArray();
        for(const QJsonValue& val : aliasOutputArray)
        {
            QJsonObject portObj = val.toObject();
            Port port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<PortCategory>(portObj["category"].toInt());
            entry.aliasOutputPorts.append(port);
        }
        
        entries[componentName] = entry;
    }
    
    m_componentCache.setAllEntries(entries);
    QString logMsg = QString("[UModernDiagramWidget] Component cache loaded from JSON file: %1 (%2 entries)")
        .arg(filePath).arg(entries.size());
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    return true;
}

QString UModernDiagramWidget::computeComponentHash(const QString& componentFullName) const
{
    if(!m_application)
        return QString();
    
    QCryptographicHash hash(QCryptographicHash::Sha256);
    
    // Добавляем имя компонента
    hash.addData(componentFullName.toUtf8());
    
    // Добавляем список дочерних компонентов
    const char* compList = Model_GetComponentsNameList(componentFullName.toStdString().c_str());
    if(compList)
    {
        const int len = static_cast<int>(strlen(compList));
        hash.addData(compList, len);
        Engine_FreeBufString(compList);
    }
    
    // Добавляем список портов (входных и выходных)
    const char* inputProps = Model_GetComponentPropertiesLookupList(componentFullName.toStdString().c_str(), ptPubInput | ptInput);
    if(inputProps)
    {
        const int len = static_cast<int>(strlen(inputProps));
        hash.addData(inputProps, len);
        Engine_FreeBufString(inputProps);
    }
    
    const char* outputProps = Model_GetComponentPropertiesLookupList(componentFullName.toStdString().c_str(), ptPubOutput | ptOutput);
    if(outputProps)
    {
        const int len = static_cast<int>(strlen(outputProps));
        hash.addData(outputProps, len);
        Engine_FreeBufString(outputProps);
    }
    
    // Добавляем список связей
    const char* xmlRaw = Model_GetComponentInternalLinks(componentFullName.toStdString().c_str(), nullptr);
    if(xmlRaw)
    {
        const int len = static_cast<int>(strlen(xmlRaw));
        hash.addData(xmlRaw, len);
        Engine_FreeBufString(xmlRaw);
    }
    
    return QString::fromLatin1(hash.result().toHex());
}

void UModernDiagramWidget::invalidateComponentCache(const QString& componentFullName)
{
    if(componentFullName.isEmpty())
    {
        // Инвалидируем весь кэш
        m_componentCache.clear();
    }
    else
    {
        // Инвалидируем кэш конкретного компонента
        m_componentCache.invalidateEntry(componentFullName);
    }
}

void UModernDiagramWidget::clearComponentCache()
{
    m_componentCache.clear();
    
    // Также удаляем файлы кэша
    QString jsonPath = getCacheFilePath("json");
    QString binPath = getCacheFilePath("bin");
    
    if(QFile::exists(jsonPath))
        QFile::remove(jsonPath);
    if(QFile::exists(binPath))
        QFile::remove(binPath);
}

void UModernDiagramWidget::scheduleCacheSave()
{
    // Отложенное сохранение кэша через 2 секунды после последнего обновления
    // Это позволяет группировать множественные обновления и не замедлять работу
    QTimer::singleShot(2000, this, [this]()
    {
        if(m_application)
        {
            const QHash<QString, ComponentCacheEntry>& entries = m_componentCache.getAllEntries();
            if(!entries.isEmpty())
            {
                // Пробуем сохранить в бинарном формате (быстрее), если не получится - в JSON
                QString binPath = getCacheFilePath("bin");
                if(!saveComponentCacheToFile(binPath, true))
                {
                    QString jsonPath = getCacheFilePath("json");
                    saveComponentCacheToFile(jsonPath, false);
                }
            }
        }
    });
}

