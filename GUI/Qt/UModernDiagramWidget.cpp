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
#include <sstream>


// --------------------------- ModernScene ---------------------------

class ModernScene : public QGraphicsScene
{
public:
    explicit ModernScene(class UModernDiagramWidget* owner);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
private:
    UModernDiagramWidget* m_owner;
    QTimer* m_hoverTimer{nullptr};
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
        if(m_isPanning && event->button() == Qt::LeftButton)
        {
            // Завершаем прокрутку
            m_isPanning = false;
            unsetCursor();
            event->accept();
            return;
        }
        
        // Иначе передаем событие в базовый класс
        QGraphicsView::mouseReleaseEvent(event);
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
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setAcceptHoverEvents(true);
    
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
                            qDebug() << "itemDoubleClicked signal received";
                            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                                "itemDoubleClicked signal received", 0);
                            // Двойной клик всегда активирует порт
                            onPortItemActivated(item, column);
                        });
        
        // Подключаем обработку активации элемента (двойной клик или Enter)
        QObject::connect(m_portListWidget, &QTreeWidget::itemActivated,
                        [this](QTreeWidgetItem* item, int column) {
                            // Логируем событие активации (используем qDebug для гарантированного вывода)
                            qDebug() << "itemActivated signal received";
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
            qDebug() << "Enter shortcut activated";
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
            qDebug() << "Enter (numpad) shortcut activated";
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
                            // Логируем событие клика
                            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
                                "itemClicked signal received", 0);
                            
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
                                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, logMsg.toStdString().c_str(), 0);
                                    
                                    // Для выходных портов начинаем соединение
                                    if(!isInput && !portName.isEmpty())
                                    {
                                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
                                            "itemClicked: Calling onPortItemActivated for output port", 0);
                                        onPortItemActivated(item, column);
                                    }
                                    // Для входных портов завершаем соединение, если оно активно
                                    else if(isInput && !portName.isEmpty() && m_owner && m_owner->m_activeTempLink)
                                    {
                                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
                                            "itemClicked: Calling onPortItemActivated for input port", 0);
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
                            else
                            {
                                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
                                    "itemClicked: Item is null or is a component (not a port)", 0);
                            }
                        });
    }

    // Константы для размеров
    const double minWidth = 180.0;  // Увеличено в 1.5 раза (120 * 1.5)
    const double minHeight = 40.0;  // Минимальная высота для текста
    const double portSpacing = 20.0;
    const double topMargin = 30.0;  // Отступ сверху для текста
    const double bottomMargin = 10.0;  // Отступ снизу
    const double sideMargin = 10.0;  // Отступ для портов от края

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
        
        // Многослойная тень для эффекта размытия
        for (int i = 3; i >= 0; --i)
        {
            QColor layerColor = shadowColor;
            layerColor.setAlpha(shadowColor.alpha() * (4 - i) / 8);
            painter->setBrush(layerColor);
            QRectF layerRect = shadowRect.adjusted(-i*2, -i*2, i*2, i*2);
            painter->drawRoundedRect(layerRect, cornerRadius + i, cornerRadius + i);
        }
    }
    
    // Градиентный фон узла
    QLinearGradient gradient = style->getNodeGradient(rect());
    if (isSelected())
    {
        // Для выделенного узла используем специальный цвет
        gradient.setColorAt(0, style->getNodeFillSelectedColor());
        gradient.setColorAt(1, style->getNodeFillSelectedColor().darker(105));
    }
    
    // Рисуем основной прямоугольник узла
    QColor border = isSelected() ? style->getAccentColor() : style->getNodeBorderColor();
    painter->setPen(QPen(border, style->getNodeBorderWidth()));
    painter->setBrush(gradient);
    painter->drawRoundedRect(rect(), cornerRadius, cornerRadius);
    
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
        
        // Проверяем, есть ли порты для этой категории
        bool hasPorts = false;
        if (p.category == PortCategory::Own)
        {
            hasPorts = !getOwnInputPorts().isEmpty();
        }
        else if (p.category == PortCategory::Child)
        {
            hasPorts = !getChildInputPorts().isEmpty();
        }
        else if (p.category == PortCategory::Alias)
        {
            hasPorts = !getAliasInputPorts().isEmpty();
        }
        
        // Проверяем, есть ли соединения к портам этой категории
        // Проверяем соединения для всех категоризированных портов (с пустым fullPath)
        bool hasConnections = false;
        if (p.fullPath.isEmpty())  // Только для категоризированных портов
        {
            hasConnections = hasConnectionsToInputCategory(p.category);
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
        
        // Проверяем, есть ли порты для этой категории
        bool hasPorts = false;
        if (p.category == PortCategory::Own)
        {
            hasPorts = !getOwnOutputPorts().isEmpty();
        }
        else if (p.category == PortCategory::Child)
        {
            hasPorts = !getChildOutputPorts().isEmpty();
        }
        else if (p.category == PortCategory::Alias)
        {
            hasPorts = !getAliasOutputPorts().isEmpty();
        }
        
        // Проверяем, есть ли соединения от портов этой категории
        // Проверяем соединения для всех категоризированных портов (с пустым fullPath)
        bool hasConnections = false;
        if (p.fullPath.isEmpty())  // Только для категоризированных портов
        {
            hasConnections = hasConnectionsToOutputCategory(p.category);
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
        // Обновить линии
        for(QGraphicsItem* it : scene()->items())
        {
            auto* link = dynamic_cast<UModernDiagramWidget::LinkItem*>(it);
            if(link)
                link->updateGeometry();
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
        // Отслеживаем изменение выбора компонента
        if(value.toBool())
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
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getChildOutputPorts() const
{
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
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getAliasOutputPorts() const
{
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
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getOwnInputPorts() const
{
    QVector<Port> result;
    if(!m_owner || !m_owner->m_application)
        return result;
    
    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
    
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
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getChildInputPorts() const
{
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
    
    return result;
}

QVector<UModernDiagramWidget::Port> UModernDiagramWidget::NodeItem::getAliasInputPorts() const
{
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
    
    return result;
}

bool UModernDiagramWidget::NodeItem::hasConnectionsToInputCategory(PortCategory category) const
{
    if(!m_owner || !m_owner->m_application)
        return false;
    
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
                            QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
                            const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
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
                            QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
                            const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
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
}

void UModernDiagramWidget::NodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverMoveEvent(event);
    const Port* port = getPortAtPosition(event->pos());
    bool portChanged = (port != m_hoveredPort);
    if(portChanged)
    {
        m_hoveredPort = port;
        update(); // Только визуальная подсветка
        // Убрано: showPortListWidget - окно открывается только при клике
    }
}

void UModernDiagramWidget::NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
    m_hoveredPort = nullptr;
    if(m_hideTimer)
        m_hideTimer->start();
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
    qDebug() << "onPortItemActivated: Function called";
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
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
            "onPortItemActivated: Item is a component, not a port", 0);
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
        qDebug() << "onPortItemActivated: Input port selected, portName:" << portName << "componentName:" << componentName;
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
            "onPortItemActivated: Input port selected", 0);
        
        // Если выбран входной порт и есть активное соединение - завершаем связь
        // Проверяем наличие активного соединения через m_activeTempLink
        if(m_owner->m_activeTempLink)
        {
            qDebug() << "onPortItemActivated: Active connection exists";
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                "onPortItemActivated: Active connection exists", 0);
            
            // Дополнительная проверка: должны быть установлены исходные данные
            if(!m_owner->m_activeSourceNode || 
               m_owner->m_activeSourcePortName.isEmpty())
            {
                // Логируем ошибку: отсутствуют исходные данные
                qDebug() << "onPortItemActivated: ERROR - source port data is missing, activeSourceNode:" 
                         << (m_owner->m_activeSourceNode ? "exists" : "null") 
                         << "activeSourcePortName:" << m_owner->m_activeSourcePortName;
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
                    "onPortItemActivated: Cannot complete connection - source port data is missing", 0);
                return;
            }
            
            qDebug() << "onPortItemActivated: Source port data OK, sourceNode:" << m_owner->m_activeSourceNode->nodeName
                     << "sourcePortName:" << m_owner->m_activeSourcePortName;
            
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
            qDebug() << "onPortItemActivated: Component path formation:";
            qDebug() << "  nodeName:" << nodeName;
            qDebug() << "  componentName:" << componentName;
            qDebug() << "  dstName:" << dstName;
            qDebug() << "  m_componentName:" << m_owner->m_componentName;
            qDebug() << "  fullDst:" << fullDst;
            
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
            qDebug() << "onPortItemActivated: Creating connection:";
            qDebug() << "  fullSrc:" << fullSrc;
            qDebug() << "  srcProp:" << srcProp;
            qDebug() << "  fullDst:" << fullDst;
            qDebug() << "  dstProp:" << dstProp;
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
            
            qDebug() << "onPortItemActivated: Model_CreateLinkByName returned:" << result;
            
            // Логируем результат создания соединения
            if(result == 0) // Предполагаем, что 0 означает успех
            {
                QString successMsg = QString("Connection created successfully: %1.%2 -> %3.%4")
                    .arg(fullSrc).arg(srcProp).arg(fullDst).arg(dstProp);
                qDebug() << "onPortItemActivated: SUCCESS -" << successMsg;
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, successMsg.toStdString().c_str(), 0);
            }
            else
            {
                QString errorMsg = QString("Failed to create connection: %1.%2 -> %3.%4 (error code: %5)")
                    .arg(fullSrc).arg(srcProp).arg(fullDst).arg(dstProp).arg(result);
                qDebug() << "onPortItemActivated: ERROR -" << errorMsg;
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
                qDebug() << "onPortItemActivated: Calling rebuildLinks() to update links";
                m_owner->rebuildLinks();
            }
            else
            {
                qDebug() << "onPortItemActivated: Not rebuilding links, result =" << result;
            }
        }
        else
        {
            // Логируем предупреждение: попытка завершить соединение без активного соединения
            qDebug() << "onPortItemActivated: WARNING - no active connection exists (m_activeTempLink is null)";
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
                "onPortItemActivated: Attempted to complete connection but no active connection exists", 0);
            return;
        }
    }
    else
    {
        // Если выбран выходной порт - начинаем pull-режим
        QString logMsg = QString("onPortItemActivated: Starting connection from output port '%1'")
            .arg(portName);
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, logMsg.toStdString().c_str(), 0);
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
            qDebug() << "onPortItemActivated: Starting connection from output port:" << portName 
                     << "in component:" << nodeName;
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
    updateGeometry();
}

UModernDiagramWidget::LinkItem::LinkItem(NodeItem* src, NodeItem* dst, PortCategory srcCategory, PortCategory dstCategory)
    : QGraphicsPathItem()
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
    updateGeometry();
}

UModernDiagramWidget::LinkItem::LinkItem(NodeItem* src, const QPointF& tempEnd, const QPointF& startPos)
    : QGraphicsPathItem()
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
    m_resetZoomButton->setToolTip("Сбросить масштаб");
    m_resetZoomButton->setFixedSize(32, 32);
    m_resetZoomButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(255, 255, 255, 200);"
        "    border: 1px solid #ccc;"
        "    border-radius: 4px;"
        "    font-size: 18px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(240, 240, 240, 220);"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(220, 220, 220, 240);"
        "}"
    );
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

UModernDiagramWidget::~UModernDiagramWidget() = default;

void UModernDiagramWidget::SetApplication(RDK::UApplication* app)
{
    m_application = app;
}

void UModernDiagramWidget::SetComponentName(const QString& name)
{
    m_componentName = name;
}

void UModernDiagramWidget::Reload()
{
    // Сохраняем текущее состояние viewport перед перезагрузкой
    // Сохраняем только если сцена уже содержит элементы (компонент был загружен ранее)
    if(!m_componentName.isEmpty() && !m_scene->items().isEmpty())
    {
        saveCurrentViewState();
    }
    
    // Удалено избыточное логирование - создавало спам в INFO логах
    clearScene();
    buildScene();
    
    // Восстанавливаем состояние viewport для текущего компонента
    restoreViewState(m_componentName);
}

void UModernDiagramWidget::FitToView()
{
    if(m_scene->items().isEmpty())
        return;
    m_mainView->fitInView(m_scene->itemsBoundingRect().adjusted(-40,-40,40,40), Qt::KeepAspectRatio);
}

void UModernDiagramWidget::clearScene()
{
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
        }
    }
    
    m_nodes.clear();
    m_nodeByName.clear();
    m_links.clear();
    m_scene->clear();  // Удаляет все элементы, включая NodeItem и прокси-виджеты
    m_tempLink = nullptr;
    m_dragSourceNode = nullptr;
    m_activeTempLink = nullptr;
    m_activeSourceNode = nullptr;
    m_activeSourcePort = nullptr;
    m_isLineFrozen = false;
    m_frozenTargetPortPos = QPointF(0, 0);
    m_normalizationOffset = QPointF(0, 0);
}

void UModernDiagramWidget::buildScene()
{
    // Удалено избыточное логирование - создавало спам в INFO логах
    if(!m_application)
    {
        // Удалено избыточное логирование - создавало спам в INFO логах
        return;
    }

    const char* compRaw = Model_GetComponentsNameList(m_componentName.toStdString().c_str());
    QString compListStr = QString::fromUtf8(compRaw ? compRaw : "");
    QStringList components = compListStr.split(",", Qt::SkipEmptyParts);
    Engine_FreeBufString(compRaw);
    // Удалено избыточное логирование - создавало спам в INFO логах

    int idx = 0;
    bool coordsLoaded = false;
    QPointF minKernel(0,0);
    bool minSet=false;
    // Сначала загружаем все координаты и находим минимальную для визуальной нормализации
    for(const QString& comp : components)
    {
        QString fullName = m_componentName.isEmpty() ? comp : m_componentName + "." + comp;
        QPointF kernelPos;
        if(loadCoord(fullName, kernelPos))
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
    
    for(const QString& comp : components)
    {
        QString fullName = m_componentName.isEmpty() ? comp : m_componentName + "." + comp;
        const char* clsRaw = Model_GetComponentClassName(fullName.toStdString().c_str());
        QString cls = QString::fromUtf8(clsRaw ? clsRaw : "");
        Engine_FreeBufString(clsRaw);

        auto* node = new NodeItem(this, comp, cls);
        QPointF loaded;
        QPointF kernelPos;
        if(loadCoord(fullName, kernelPos))
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
        m_scene->addItem(node);
        m_nodes.append(node);
        m_nodeByName.insert(comp, node);
        node->setPos(loaded);
        idx++;
    }
    
    if(!coordsLoaded)
    {
        // нет координат из ядра — оставляем как есть и не перезаписываем в ядро,
        // чтобы при первом отображении не было автосжатия старого вида
        layoutGrid();
    }

    buildLinks();
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
    const Port* port = pickPortDetailed(scenePos, requireInput, node, portPos);
    return node;
}

const UModernDiagramWidget::Port* UModernDiagramWidget::pickPortDetailed(
    const QPointF& scenePos, bool requireInput, NodeItem*& node, QPointF& portPos)
{
    const double portRadius = 8.0;
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
    const char* xmlRaw = Model_GetComponentInternalLinks(m_componentName.toStdString().c_str(), nullptr);
    if(!xmlRaw)
        return;
    std::string raw(xmlRaw ? xmlRaw : "");
    // Удалено избыточное логирование - создавало спам в INFO логах

    RDK::USerStorageXML xml;
    if(!xml.Load(raw, "Links"))
    {
        Engine_FreeBufString(xmlRaw);
        // Удалено избыточное логирование - создавало спам в INFO логах
        return;
    }

    RDK::UStringLinksList linkslist;
    xml >> linkslist;
    Engine_FreeBufString(xmlRaw);
    // Удалено избыточное логирование - создавало спам в INFO логах

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

    int added = 0;
    int skipped = 0;
    int loggedSkip = 0;
    int loggedPairs = 0;
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
            if(!srcNode || !dstNode)
            {
                skipped++;
                // Удалено избыточное логирование - создавало спам в INFO логах
                continue;
            }
            
            // Определяем категории портов по именам свойств
            PortCategory srcCategory = srcNode->determinePortCategory(itemName, false);
            
            // Для определения категории входного порта нужно нормализовать connName относительно dstNode
            // connName может быть в любом формате, нужно извлечь часть, относящуюся к dstNode
            QString normalizedConnName = connName;
            QString dstNodeName = dstNode->nodeName;
            QString connIdStr = QString::fromStdString(connId);
            
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
            
            PortCategory dstCategory = dstNode->determinePortCategory(normalizedConnName, true);
            
            // Создаем LinkItem с категориями портов
            auto* l = new LinkItem(srcNode, dstNode, srcCategory, dstCategory);
            m_scene->addItem(l);
            m_links.append(l);
            added++;
            // Удалено избыточное логирование - создавало спам в INFO логах
        }
    }
    // Удалено избыточное логирование - создавало спам в INFO логах
}

void UModernDiagramWidget::rebuildLinks()
{
    // Удаляем все существующие связи
    for(auto* link : m_links)
    {
        m_scene->removeItem(link);
        delete link;
    }
    m_links.clear();
    
    // Перестраиваем связи
    buildLinks();
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
        qDebug() << "keyPressEvent: Enter/Return key pressed";
        // Проверяем, есть ли открытое окно выбора портов
        for(NodeItem* node : m_nodes)
        {
            if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
            {
                qDebug() << "keyPressEvent: Found visible port list widget";
                if(node->m_portListWidget)
                {
                    QTreeWidgetItem* currentItem = node->m_portListWidget->currentItem();
                    if(currentItem && currentItem->parent() != nullptr) // Проверяем, что это элемент порта (не категория)
                    {
                        // Логируем через qDebug для гарантированного вывода
                        qDebug() << "keyPressEvent: Enter pressed, currentItem found, calling onPortItemActivated";
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                            "keyPressEvent: Enter pressed, processing current item from port list", 0);
                        node->onPortItemActivated(currentItem, 0);
                        event->accept();
                        return;
                    }
                    else
                    {
                        qDebug() << "keyPressEvent: Enter pressed but currentItem is null or is a category";
                    }
                }
            }
        }
        qDebug() << "keyPressEvent: Enter pressed but no visible port list widget found";
        // Если окно выбора портов не открыто, передаем событие дальше (не отменяем соединение!)
    }
    
    // Обработка Esc для отмены активной связи и закрытия окон со списком портов
    if(event->key() == Qt::Key_Escape)
    {
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
        bool deleted = false;
        for(QGraphicsItem* it : selected)
        {
            auto* node = dynamic_cast<NodeItem*>(it);
            if(!node) continue;
            QString fullName = m_componentName.isEmpty() ? node->nodeName
                                                         : m_componentName + "." + node->nodeName;
            Model_DelComponent(fullName.toStdString().c_str(), node->nodeName.toStdString().c_str());
            deleted = true;
        }
        if(deleted)
        {
            Reload();
            emit updateComponentsList();
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
    : m_owner(owner)
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

    // Обновляем hover для всех узлов
    for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
    {
        node->refreshHoverAtScenePos(scenePos);
    }
}

void ModernScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
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
                        qDebug() << "mousePressEvent: Opening input port list for category" << (int)inputPort->category;
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                            "mousePressEvent: Opening input port list for category", 0);
                        
                        // Если есть активная связь, активируем состояние ожидания выбора порта
                        if(m_owner->m_activeTempLink)
                        {
                            qDebug() << "mousePressEvent: Active connection exists, setting waiting state";
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
                            
                            qDebug() << "mousePressEvent: Connection frozen at position:" << freezePos;
                            
                            // Принудительно обновляем сцену, чтобы изменения вступили в силу немедленно
                            if(m_owner->m_scene)
                            {
                                m_owner->m_scene->update();
                            }
                        }
                        else
                        {
                            qDebug() << "mousePressEvent: No active connection, just opening port list";
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
    auto* node = m_owner->pickNode(event->scenePos());
    if(node)
    {
        clickedOnBackground = false;
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
        for(UModernDiagramWidget::NodeItem* node : m_owner->m_nodes)
        {
            if(node->m_portListWidgetProxy && node->m_portListWidgetProxy->isVisible())
            {
                QRectF widgetRect = node->m_portListWidgetProxy->mapToScene(
                    node->m_portListWidgetProxy->boundingRect()).boundingRect();
                if(widgetRect.contains(event->scenePos()))
                {
                    clickedOnBackground = false;
                    // Логируем, что клик попал в окно выбора портов
                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
                        "mousePressEvent: Click detected in port list widget, forwarding to widget", 0);
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
    }
    
    QGraphicsScene::mousePressEvent(event);
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
}

void ModernScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
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
                qDebug() << "onPortItemActivated: Calling Reload() to rebuild links";
                m_owner->Reload();
            }
            else
            {
                qDebug() << "onPortItemActivated: Not reloading, result =" << result;
            }
            emit m_owner->updateComponentsList();
            
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
            qDebug() << "mouseReleaseEvent: Click detected on port list widget";
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
                        qDebug() << "mouseReleaseEvent: Found visible port list widget, scenePos:" << event->scenePos();
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                            "mouseReleaseEvent: Found visible port list widget", 0);
                        
                        // Преобразуем позицию клика в локальные координаты виджета
                        QPointF widgetLocalPos = node->m_portListWidgetProxy->mapFromScene(event->scenePos());
                        QPoint widgetPoint = widgetLocalPos.toPoint();
                        
                        qDebug() << "mouseReleaseEvent: widgetLocalPos:" << widgetLocalPos << "widgetPoint:" << widgetPoint;
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
                            qDebug() << "mouseReleaseEvent: Trying mapFromGlobal, globalPos:" << globalPos 
                                     << "widgetGlobalPoint:" << widgetGlobalPoint;
                        }
                        
                        // 3. Если все еще не нашли, используем currentItem
                        if(!itemUnderCursor)
                        {
                            itemUnderCursor = node->m_portListWidget->currentItem();
                            qDebug() << "mouseReleaseEvent: Using currentItem as fallback";
                        }
                        
                        if(itemUnderCursor)
                        {
                            qDebug() << "mouseReleaseEvent: itemUnderCursor found, has parent:" << (itemUnderCursor->parent() != nullptr);
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
                                    
                                    qDebug() << "mouseReleaseEvent: Clicked on port item, isInput:" << isInput << "portName:" << portName;
                                    QString logMsg = QString("mouseReleaseEvent: Clicked on %1 port '%2' in list, completing connection")
                                        .arg(isInput ? "input" : "output").arg(portName);
                                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
                                    
                                    node->onPortItemActivated(itemUnderCursor, 0);
                                    event->accept();
                                    return;
                                }
                                else
                                {
                                    qDebug() << "mouseReleaseEvent: Item data is invalid";
                                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
                                        "mouseReleaseEvent: Item data is invalid", 0);
                                }
                            }
                            else
                            {
                                qDebug() << "mouseReleaseEvent: Clicked on category item (no parent)";
                                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
                                    "mouseReleaseEvent: Clicked on category item", 0);
                            }
                        }
                        else
                        {
                            qDebug() << "mouseReleaseEvent: No item found at widgetPoint:" << widgetPoint;
                            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_WARNING, 
                                "mouseReleaseEvent: No item found at cursor position", 0);
                        }
                        
                        // Если не удалось найти элемент напрямую, передаем событие виджету
                        // чтобы он мог обработать клик через свой механизм
                        qDebug() << "mouseReleaseEvent: Forwarding event to QTreeWidget for processing";
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                            "mouseReleaseEvent: Forwarding event to QTreeWidget", 0);
                    }
                }
            }
            
            // Если не попали на элемент порта, передаем событие дальше
            qDebug() << "mouseReleaseEvent: No port item found, forwarding event";
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
                qDebug() << "mouseReleaseEvent (port tree): Calling rebuildLinks() to update links";
                m_owner->rebuildLinks();
            }
            else
            {
                qDebug() << "mouseReleaseEvent (port tree): Not rebuilding links, result =" << result;
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
    QGraphicsScene::mouseReleaseEvent(event);
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
                        qDebug() << "mouseDoubleClickEvent: Double-clicked on port item, calling onPortItemActivated";
                        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, 
                            "mouseDoubleClickEvent: Double-clicked on port item in list, completing connection", 0);
                        node->onPortItemActivated(itemUnderCursor, 0);
                        event->accept();
                        return;
                    }
                    else
                    {
                        qDebug() << "mouseDoubleClickEvent: Double-clicked on category item or no item found";
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
                qDebug() << "mouseReleaseEvent (drag&drop): Calling rebuildLinks() to update links";
                m_owner->rebuildLinks();
            }
            else
            {
                qDebug() << "mouseReleaseEvent (drag&drop): Not rebuilding links, result =" << result;
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

void UModernDiagramWidget::selectComponent(QString name)
{
    // Найти компонент в текущем пути
    QString componentName = name;
    if(!m_componentName.isEmpty() && name.startsWith(m_componentName + "."))
    {
        componentName = name.mid(m_componentName.size() + 1);
    }
    else if(!m_componentName.isEmpty() && name == m_componentName)
    {
        // Если имя совпадает с текущим путём, значит мы на верхнем уровне
        return;
    }
    
    // Найти узел и выделить его
    if(auto it = m_nodeByName.find(componentName); it != m_nodeByName.end())
    {
        m_scene->clearSelection();
        it.value()->setSelected(true);
        m_contextMenuNode = it.value();
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

void UModernDiagramWidget::componentDelete()
{
    if(!m_contextMenuNode)
        return;
        
    QString selectedComponentLongName = getSelectedComponentLongName();
    if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", 
            "Are you sure you want to delete component " + selectedComponentLongName + "?", 
            QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) 
            return;
    }
    
    Model_DelComponent("", selectedComponentLongName.toLocal8Bit().constData());
    Reload();
    emit updateComponentsList();
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

