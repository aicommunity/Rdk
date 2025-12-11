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
#include "UClassDescriptionDisplay.h"
#include "UQuickLinkDialog.h"
#include "../Core/Engine/UStorage.h"
#include "../Core/Engine/UEngine.h"
#include "../Core/Engine/UNet.h"
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
        : QGraphicsView(scene), m_owner(owner) {}
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
private:
    UModernDiagramWidget* m_owner;
};

// --------------------------- Helpers ---------------------------

UModernDiagramWidget::NodeItem::NodeItem(UModernDiagramWidget* owner, const QString& name, const QString& cls)
    : QGraphicsRectItem()
    , nodeName(name)
    , className(cls)
    , m_owner(owner)
{
    setRect(-60, -30, 120, 60);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    inputs.append({QPointF(rect().left(), 0), true, QStringLiteral("In")});
    outputs.append({QPointF(rect().right(), 0), false, QStringLiteral("Out")});
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
        painter->setPen(QPen(QColor(50, 150, 50), 1.0));
        painter->drawEllipse(p.pos, 4, 4);
        painter->drawText(p.pos + QPointF(6, -2), p.name);
    }
    for (const Port& p : outputs) {
        painter->setPen(QPen(QColor(50, 100, 200), 1.0));
        painter->drawEllipse(p.pos, 4, 4);
        painter->drawText(p.pos + QPointF(-28, -2), p.name);
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
        if(m_owner)
        {
            QString fullName = m_owner->m_componentName.isEmpty() ? nodeName
                                                                  : m_owner->m_componentName + "." + nodeName;
            QPointF minPos = m_owner->currentMinScenePos();
            m_owner->saveCoord(fullName, scenePos() - minPos);
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
}

void UModernDiagramWidget::buildScene()
{
    if(!m_application)
        return;

    const char* compRaw = Model_GetComponentsNameList(m_componentName.toStdString().c_str());
    QStringList components = QString::fromUtf8(compRaw ? compRaw : "").split(",", Qt::SkipEmptyParts);
    Engine_FreeBufString(compRaw);

    int idx = 0;
    bool coordsLoaded = false;
    QPointF minKernel(0,0);
    bool minSet=false;
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
            loaded = scenePosFromKernel(kernelPos); // позже нормализуем
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

    if(coordsLoaded)
    {
        // нормализуем сцену, чтобы левый верхний был в (0,0)
        QPointF minPos = scenePosFromKernel(minKernel);
        for(auto* n : m_nodes)
            n->setPos(n->scenePos() - minPos);
    }
    else
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
    for(auto* node : m_nodes)
    {
        auto test = [&](const QVector<Port>& ports)->bool{
            for(const Port& p: ports)
            {
                QPointF pScene = node->mapToScene(p.pos);
                if(QLineF(pScene, scenePos).length() < 8.0)
                {
                    portPos = pScene;
                    return true;
                }
            }
            return false;
        };
        if(requireInput)
        {
            if(test(node->inputs)) return node;
        }
        else
        {
            if(test(node->outputs)) return node;
        }
    }
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
        auto* node = m_owner->pickPort(event->scenePos(), false, portPos);
        if(node)
        {
            m_owner->m_dragSourceNode = node;
            m_owner->m_dragSourcePort = portPos;
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
        auto* target = m_owner->pickPort(event->scenePos(), true, portPos);
        if(target && target != m_owner->m_dragSourceNode)
        {
            m_owner->m_scene->removeItem(m_owner->m_tempLink);
            delete m_owner->m_tempLink;
            auto* finalLink = new UModernDiagramWidget::LinkItem(m_owner->m_dragSourceNode, target, true, true);
            m_owner->m_scene->addItem(finalLink);
            m_owner->m_links.append(finalLink);
            m_owner->m_tempLink = nullptr;
            // Создание связи в ядре через имена (упрощённо: берём первые порты)
            QString srcName = m_owner->m_dragSourceNode->nodeName;
            QString dstName = target->nodeName;
            QString fullSrc = m_owner->m_componentName.isEmpty() ? srcName : m_owner->m_componentName + "." + srcName;
            QString fullDst = m_owner->m_componentName.isEmpty() ? dstName : m_owner->m_componentName + "." + dstName;
            Model_CreateLinkByName(fullSrc.toStdString().c_str(), "Out",
                                   fullDst.toStdString().c_str(), "In");
        }
        else
        {
            m_owner->m_scene->removeItem(m_owner->m_tempLink);
            delete m_owner->m_tempLink;
            m_owner->m_tempLink = nullptr;
        }
        m_owner->m_dragSourceNode = nullptr;
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

