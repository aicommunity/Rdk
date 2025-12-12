#include "UModernDiagramWidget.h"

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
#include <QToolTip>
#include <QGraphicsSceneHoverEvent>
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
    explicit ModernScene(class UModernDiagramWidget* owner) : m_owner(owner) {}
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
private:
    UModernDiagramWidget* m_owner;
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
    bool viewportEvent(QEvent *event) override
    {
        if(event->type() == QEvent::Wheel)
        {
            auto* wheel = static_cast<QWheelEvent*>(event);
            const double factor = wheel->angleDelta().y() > 0 ? 1.15 : 0.87;
            scale(factor, factor);
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
};

// --------------------------- Helpers ---------------------------

UModernDiagramWidget::NodeItem::NodeItem(UModernDiagramWidget* owner, const QString& name, const QString& cls)
    : QGraphicsRectItem()
    , nodeName(name)
    , className(cls)
    , m_owner(owner)
    , m_hoveredPort(nullptr)
    , m_tooltipTimer(new QTimer())
{
    m_tooltipTimer->setSingleShot(false);
    m_tooltipTimer->setInterval(300);  // Обновляем tooltip каждые 300 мс, чтобы он не скрывался автоматически
    QObject::connect(m_tooltipTimer, &QTimer::timeout, [this]() {
        if(m_hoveredPort && m_owner)
        {
            updateTooltip();
        }
    });
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setAcceptHoverEvents(true);

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
        
        // Загружаем входные порты
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
                    Port port;
                    port.isInput = true;
                    port.name = propName;
                    port.componentName = name;
                    port.fullPath = propName;
                    port.displayName = propName;
                    inputs.append(port);
                }
            }
            Engine_FreeBufString(inputProps);
        }
        
        // Если нет входных портов, добавляем дефолтный
        if(inputs.isEmpty())
        {
            Port defaultInput;
            defaultInput.isInput = true;
            defaultInput.name = QStringLiteral("In");
            defaultInput.componentName = name;
            defaultInput.fullPath = QStringLiteral("In");
            defaultInput.displayName = QStringLiteral("In");
            inputs.append(defaultInput);
        }
        
        // Загружаем выходные порты
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
                    Port port;
                    port.isInput = false;
                    port.name = propName;
                    port.componentName = name;
                    port.fullPath = propName;
                    port.displayName = propName;
                    outputs.append(port);
                }
            }
            Engine_FreeBufString(outputProps);
        }
        
        // Если нет выходных портов, добавляем дефолтный
        if(outputs.isEmpty())
        {
            Port defaultOutput;
            defaultOutput.isInput = false;
            defaultOutput.name = QStringLiteral("Out");
            defaultOutput.componentName = name;
            defaultOutput.fullPath = QStringLiteral("Out");
            defaultOutput.displayName = QStringLiteral("Out");
            outputs.append(defaultOutput);
        }
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

QRectF UModernDiagramWidget::NodeItem::boundingRect() const
{
    return rect().adjusted(-4, -4, 4, 4);
}

void UModernDiagramWidget::NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QColor fill = isSelected() ? QColor(220, 240, 255) : QColor(245, 245, 245);
    QColor border = QColor(70, 120, 200);
    painter->setPen(QPen(border, 1.5));
    painter->setBrush(fill);
    painter->drawRoundedRect(rect(), 6, 6);

    painter->setPen(Qt::black);
    painter->drawText(rect().adjusted(4, 4, -4, -4),
                     Qt::AlignTop | Qt::AlignLeft,
                     nodeName + "\n" + className);

    painter->setBrush(Qt::white);
    for (const Port& p : inputs) {
        bool isHovered = (m_hoveredPort == &p);
        QColor portColor = isHovered ? QColor(100, 200, 100) : QColor(50, 150, 50);
        double portSize = isHovered ? 5.0 : 4.0;
        painter->setPen(QPen(portColor, isHovered ? 2.0 : 1.0));
        painter->drawEllipse(p.pos, portSize, portSize);
        // Имя порта показывается только в tooltip при наведении
    }
    for (const Port& p : outputs) {
        bool isHovered = (m_hoveredPort == &p);
        QColor portColor = isHovered ? QColor(100, 150, 255) : QColor(50, 100, 200);
        double portSize = isHovered ? 5.0 : 4.0;
        painter->setPen(QPen(portColor, isHovered ? 2.0 : 1.0));
        painter->drawEllipse(p.pos, portSize, portSize);
        // Имя порта показывается только в tooltip при наведении
    }
}

QPointF UModernDiagramWidget::NodeItem::scenePortPos(bool output) const
{
    const QVector<Port>& ports = output ? outputs : inputs;
    if(ports.isEmpty())
        return scenePos();
    return mapToScene(ports.first().pos);
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
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
                (std::string("[ModernDiagram] item move ") + fullName.toStdString() + 
                 " scenePos=(" + std::to_string(normalizedPos.x()) + "," + std::to_string(normalizedPos.y()) + ")" +
                 " offset=(" + std::to_string(m_owner->m_normalizationOffset.x()) + "," + std::to_string(m_owner->m_normalizationOffset.y()) + ")" +
                 " absoluteScene=(" + std::to_string(absoluteScenePos.x()) + "," + std::to_string(absoluteScenePos.y()) + ")").c_str(), 0);
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

void UModernDiagramWidget::NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverEnterEvent(event);
    m_hoveredPort = getPortAtPosition(event->pos());
    update();
    if(m_hoveredPort)
    {
        m_lastTooltipPos = event->pos();
        m_tooltipTimer->start();
        updateTooltip();
    }
}

void UModernDiagramWidget::NodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverMoveEvent(event);
    const Port* port = getPortAtPosition(event->pos());
    bool portChanged = (port != m_hoveredPort);
    if(portChanged)
    {
        m_hoveredPort = port;
        update();
        if(port)
        {
            m_tooltipTimer->start();
            updateTooltip();
        }
        else
        {
            m_tooltipTimer->stop();
            QToolTip::hideText();
        }
    }
    else if(port)
    {
        // Обновляем позицию tooltip при движении мыши
        m_lastTooltipPos = event->pos();
        updateTooltip();
    }
}

void UModernDiagramWidget::NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
    m_hoveredPort = nullptr;
    m_tooltipTimer->stop();
    QToolTip::hideText();
    update();
}

void UModernDiagramWidget::NodeItem::updateTooltip()
{
    if(!m_hoveredPort || !m_owner)
    {
        QToolTip::hideText();
        return;
    }
    
    bool shiftPressed = QApplication::keyboardModifiers() & Qt::ShiftModifier;
    
    QString tooltipText;
    
    if(!shiftPressed)
    {
        // Без Shift: показываем только имя текущего порта
        tooltipText = nodeName + ":\n";
        tooltipText += "  - " + m_hoveredPort->name + "\n";
        tooltipText += "\n(Удерживайте Shift для просмотра всех доступных портов)";
    }
    else
    {
        // С Shift: показываем все доступные порты того же типа (включая вложенные)
        QVector<Port> availablePorts = getNestedPorts(m_hoveredPort->isInput, true);
        
        tooltipText = nodeName + " - доступные " + (m_hoveredPort->isInput ? "входные" : "выходные") + " порты:\n";
        
        if(availablePorts.isEmpty())
        {
            tooltipText += "  - " + m_hoveredPort->name + "\n";
        }
        else
        {
            QString currentComponent;
            bool firstGroup = true;
            for(const Port& p : availablePorts)
            {
                if(p.componentName != currentComponent)
                {
                    if(!firstGroup)
                        tooltipText += "\n";
                    tooltipText += p.componentName + ":\n";
                    currentComponent = p.componentName;
                    firstGroup = false;
                }
                // Выделяем текущий порт
                if(p.name == m_hoveredPort->name && p.componentName == nodeName)
                    tooltipText += "  > " + p.name + " (текущий)\n";
                else
                    tooltipText += "  - " + p.name + "\n";
            }
        }
    }
    
    // Используем последнюю позицию мыши или текущую позицию порта
    QPointF scenePos = m_lastTooltipPos.isNull() ? 
                       mapToScene(m_hoveredPort->pos) : 
                       mapToScene(m_lastTooltipPos);
    QPoint globalPos = m_owner->m_mainView->mapToGlobal(
        m_owner->m_mainView->mapFromScene(scenePos));
    
    // Периодически обновляем tooltip, чтобы он не скрывался автоматически
    QToolTip::showText(globalPos, tooltipText, m_owner->m_mainView);
    m_lastTooltipText = tooltipText;
}

UModernDiagramWidget::LinkItem::LinkItem(NodeItem* src, NodeItem* dst, bool useOutput, bool useInput)
    : QGraphicsPathItem()
    , m_src(src)
    , m_dst(dst)
    , m_useOutput(useOutput)
    , m_useInput(useInput)
    , m_isTemp(false)
{
    setPen(QPen(QColor(80, 120, 200), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    updateGeometry();
}

UModernDiagramWidget::LinkItem::LinkItem(NodeItem* src, const QPointF& tempEnd)
    : QGraphicsPathItem()
    , m_src(src)
    , m_dst(nullptr)
    , m_useOutput(true)
    , m_useInput(true)
    , m_isTemp(true)
    , m_tempEnd(tempEnd)
{
    setPen(QPen(QColor(120, 160, 220), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    updateGeometry(tempEnd);
}

void UModernDiagramWidget::LinkItem::updateGeometry(const QPointF& cursorOverride)
{
    if(!m_src)
        return;
    QPointF start = m_src->scenePortPos(true);
    QPointF end;
    if(m_dst)
        end = m_dst->scenePortPos(false);
    else
        end = cursorOverride.isNull() ? m_tempEnd : cursorOverride;

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
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
        (std::string("[ModernDiagram] Reload START componentName=") + m_componentName.toStdString()).c_str(), 0);
    clearScene();
    buildScene();
    QRectF bounds = m_scene->itemsBoundingRect();
    if(!bounds.isNull())
    {
        QRectF padded = bounds.adjusted(-200, -200, 200, 200);
        m_scene->setSceneRect(padded);
        m_mainView->fitInView(padded, Qt::KeepAspectRatio);
    }
}

void UModernDiagramWidget::FitToView()
{
    if(m_scene->items().isEmpty())
        return;
    m_mainView->fitInView(m_scene->itemsBoundingRect().adjusted(-40,-40,40,40), Qt::KeepAspectRatio);
}

void UModernDiagramWidget::clearScene()
{
    m_nodes.clear();
    m_nodeByName.clear();
    m_links.clear();
    m_scene->clear();
    m_tempLink = nullptr;
    m_dragSourceNode = nullptr;
    m_normalizationOffset = QPointF(0, 0);
}

void UModernDiagramWidget::buildScene()
{
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
        (std::string("[ModernDiagram] buildScene START componentName=") + m_componentName.toStdString()).c_str(), 0);
    if(!m_application)
    {
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
            "[ModernDiagram] buildScene SKIP - no application", 0);
        return;
    }

    const char* compRaw = Model_GetComponentsNameList(m_componentName.toStdString().c_str());
    QString compListStr = QString::fromUtf8(compRaw ? compRaw : "");
    QStringList components = compListStr.split(",", Qt::SkipEmptyParts);
    Engine_FreeBufString(compRaw);
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
        (std::string("[ModernDiagram] buildScene components count=") + std::to_string(components.size()) +
         " list=" + compListStr.toStdString()).c_str(), 0);

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
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
        (std::string("[ModernDiagram] buildScene componentName=") + m_componentName.toStdString() +
         " minKernel=(" + std::to_string(minKernel.x()) + "," + std::to_string(minKernel.y()) + ")" +
         " minScenePos=(" + std::to_string(minScenePos.x()) + "," + std::to_string(minScenePos.y()) + ")" +
         " normalizationOffset=(" + std::to_string(m_normalizationOffset.x()) + "," + std::to_string(m_normalizationOffset.y()) + ")").c_str(), 0);
    
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
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, 
                (std::string("[ModernDiagram] buildScene node ") + comp.toStdString() +
                 " kernel=(" + std::to_string(kernelPos.x()) + "," + std::to_string(kernelPos.y()) + ")" +
                 " absoluteScene=(" + std::to_string(absoluteScenePos.x()) + "," + std::to_string(absoluteScenePos.y()) + ")" +
                 " normalized=(" + std::to_string(loaded.x()) + "," + std::to_string(loaded.y()) + ")").c_str(), 0);
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
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
        (std::string("[ModernDiagram] links xml bytes=") + std::to_string(raw.size())).c_str(), 0);
    if(!raw.empty())
    {
        std::string sample = raw.substr(0, 2000);
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
            (std::string("[ModernDiagram] links xml sample=") + sample).c_str(), 0);
    }

    RDK::USerStorageXML xml;
    if(!xml.Load(raw, "Links"))
    {
        Engine_FreeBufString(xmlRaw);
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
            "[ModernDiagram] links parse failed", 0);
        return;
    }

    RDK::UStringLinksList linkslist;
    xml >> linkslist;
    Engine_FreeBufString(xmlRaw);
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
        (std::string("[ModernDiagram] links parsed count=") + std::to_string(linkslist.GetSize())).c_str(), 0);

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
            if(loggedPairs < 50)
            {
                MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
                    (std::string("[ModernDiagram] link raw srcName=") + itemName.toStdString() +
                     " srcId=" + itemId +
                     " dstName=" + connName.toStdString() +
                     " dstId=" + connId).c_str(), 0);
                loggedPairs++;
            }
            NodeItem* srcNode = resolveNode(itemName);
            NodeItem* dstNode = resolveNode(connName);
            if(!srcNode)
                srcNode = resolveById(QString::fromStdString(itemId));
            if(!dstNode)
                dstNode = resolveById(QString::fromStdString(connId));
            if(!srcNode || !dstNode)
            {
                skipped++;
                if(loggedSkip < 30) // ограничим шум
                {
                    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
                        (std::string("[ModernDiagram] link skip src=") + itemName.toStdString() +
                         " dst=" + connName.toStdString() + " srcOk=" + (srcNode?"1":"0") +
                         " dstOk=" + (dstNode?"1":"0")).c_str(), 0);
                    loggedSkip++;
                }
                continue;
            }
            auto* l = new LinkItem(srcNode, dstNode, true, true);
            m_scene->addItem(l);
            m_links.append(l);
            added++;
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
                (std::string("[ModernDiagram] link add ") + itemName.toStdString() + " -> " + connName.toStdString()).c_str(), 0);
        }
    }
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
        (std::string("[ModernDiagram] links summary added=") + std::to_string(added) +
         " skipped=" + std::to_string(skipped)).c_str(), 0);
}

void UModernDiagramWidget::keyPressEvent(QKeyEvent *event)
{
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
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
        (std::string("[ModernDiagram] loadCoord called for ") + fullName.toStdString()).c_str(), 0);
    const char* coordRaw = Model_GetComponentParameterValue(fullName.toStdString().c_str(), "Coord");
    if(!coordRaw)
    {
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
            (std::string("[ModernDiagram] coord missing for ") + fullName.toStdString()).c_str(), 0);
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
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
                (std::string("[ModernDiagram] load coord (plain) ") + fullName.toStdString() +
                 " kernel=(" + std::to_string(kernel.x()) + "," +
                 std::to_string(kernel.y()) + ")").c_str(), 0);
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
        std::string sample = coordBuf.substr(0, 120);
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG,
            (std::string("[ModernDiagram] coord parse fail for ") + fullName.toStdString() +
             " raw=" + sample).c_str(), 0);
        return false;
    }
    RDK::MVector<double,3> pos;
    xml >> pos;
    QPointF kernel(pos[0], pos[1]);
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, (std::string("[ModernDiagram] load coord ") +
        fullName.toStdString() + " kernel=(" + std::to_string(kernel.x()) + "," +
        std::to_string(kernel.y()) + ")").c_str(), 0);
    outPos = kernel; // возвращаем ядровые координаты, сцену вычисляем выше
    return true;
}

void UModernDiagramWidget::saveCoord(const QString& fullName, const QPointF& scenePos) const
{
    QPointF kernelPos = kernelPosFromScene(scenePos);
    // нормализация: гарантируем неотрицательные координаты в ядре
    if(kernelPos.x() < 0) kernelPos.setX(0);
    if(kernelPos.y() < 0) kernelPos.setY(0);
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_DEBUG, (std::string("[ModernDiagram] save coord ") +
        fullName.toStdString() + " kernel=(" + std::to_string(kernelPos.x()) + "," +
        std::to_string(kernelPos.y()) + ")").c_str(), 0);
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

void ModernScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QPointF portPos;
        UModernDiagramWidget::NodeItem* node = nullptr;
        const UModernDiagramWidget::Port* port = m_owner->pickPortDetailed(event->scenePos(), false, node, portPos);
        if(node && port)
        {
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
    QGraphicsScene::mouseMoveEvent(event);
}

void ModernScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
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
            
            int result = Model_CreateLinkByName(fullSrc.toStdString().c_str(), srcProp.toStdString().c_str(),
                                               fullDst.toStdString().c_str(), dstProp.toStdString().c_str());
            if(result != RDK_SUCCESS)
            {
                QMessageBox::warning(m_owner, "Error", 
                    QString("Failed to create link: %1.%2 -> %3.%4")
                    .arg(fullSrc, srcProp, fullDst, dstProp));
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
        // Проверяем, попали ли мы в узел
        auto* node = m_owner->pickNode(event->scenePos());
        if(node)
        {
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

