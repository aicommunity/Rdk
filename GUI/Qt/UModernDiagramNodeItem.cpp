#include "UModernDiagramNodeItem.h"
#include "UModernDiagramWidget.h" // Для доступа к Port, PortCategory и UModernDiagramWidget
#include "UModernDiagramScene.h"
#include "UModernDiagramView.h"
#include "UModernDiagramLinkItem.h"
#include "UModernDiagramPortManager.h"
#include "UModernDiagramTooltipGenerator.h"
#include "UModernDiagramCoordinateManager.h"
#include "UModernDiagramCacheManager.h"
#include "UStyleManager.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QMap>
#include <QShortcut>
#include <QKeySequence>
#include <QDebug>
#include <QVector>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QLineF>
#include <QTimer>
#include <cmath>

// RDK includes
#include "../../Deploy/Include/rdk_init.h"
#include "../Core/Engine/UStorage.h"
#include "../Core/Engine/UEngine.h"
#include "../Core/Engine/UNet.h"
#include "../Core/Application/UIVisualController.h"
#include "UGuiTelemetry.h"

// Используем typedef для удобства (уже определены в UModernDiagramPort.h)

UModernDiagramNodeItem::UModernDiagramNodeItem(UModernDiagramWidget* owner, const QString& name, const QString& cls)
    : QGraphicsRectItem()
    , nodeName(name)
    , className(cls)
    , m_owner(owner)
    , m_hoveredPort(nullptr)
    , m_portListWidgetProxy(nullptr)
    , m_portListWidget(nullptr)
    , m_hideTimer(std::make_unique<QTimer>())
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
    m_portListWidget->setMaximumHeight(UModernDiagramConstants::PORT_LIST_MAX_HEIGHT);
    m_portListWidget->setMinimumWidth(UModernDiagramConstants::PORT_LIST_MIN_WIDTH);
    m_portListWidget->setMaximumWidth(UModernDiagramConstants::PORT_LIST_MAX_WIDTH);
    m_portListWidget->setStyleSheet(UStyleManager::instance()->getTreeWidgetStyleSheet());
    // Устанавливаем политику фокуса при создании
    m_portListWidget->setFocusPolicy(Qt::StrongFocus);

    // Таймер для отложенного скрытия списка портов
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(UModernDiagramConstants::PORT_LIST_HIDE_DELAY_MS);
    QObject::connect(m_hideTimer.get(), &QTimer::timeout, [this]()
    {
        if(m_portListWidget && (m_portListWidget->underMouse() || m_portListWidget->hasFocus()))
        {
            // Пока мышь или фокус внутри списка, откладываем скрытие
            m_hideTimer->start(UModernDiagramConstants::PORT_LIST_HIDE_RETRY_DELAY_MS);
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

UModernDiagramNodeItem::~UModernDiagramNodeItem()
{
    // Останавливаем таймер (std::unique_ptr автоматически удалит объект)
    if(m_hideTimer && m_hideTimer->isActive())
    {
        m_hideTimer->stop();
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

QRectF UModernDiagramNodeItem::boundingRect() const
{
    return rect().adjusted(-4, -4, 4, 4);
}

void UModernDiagramNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

QPointF UModernDiagramNodeItem::scenePortPos(bool output) const
{
    const QVector<Port>& ports = output ? outputs : inputs;
    if(ports.isEmpty())
        return scenePos();
    return mapToScene(ports.first().pos);
}

QPointF UModernDiagramNodeItem::scenePortPosByCategory(bool output, PortCategory category) const
{
    const QVector<Port>& ports = output ? outputs : inputs;
    const Port* fallbackPort = nullptr;

    // Сначала пытаемся найти «заголовочный» порт категории (Own/Children/Aliases),
    // у которого fullPath пустой. Если такого нет, используем любой первый порт
    // нужной категории (включая вложенные, где fullPath не пустой).
    for(const Port& port : ports)
    {
        if(port.category != category)
            continue;

        if(port.fullPath.isEmpty())
        {
            // Групповой порт категории — используем его сразу
            return mapToScene(port.pos);
        }

        if(!fallbackPort)
            fallbackPort = &port;
    }
    // Если нет заголовочного порта категории, но есть порты с нужной категорией —
    // используем первый найденный.
    if(fallbackPort)
        return mapToScene(fallbackPort->pos);

    // Fallback: возвращаем позицию первого порта
    return scenePortPos(output);
}

PortCategory UModernDiagramNodeItem::determinePortCategory(const QString& propertyName, bool isInput) const
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

    // Загружаем порты для проверки категории
    QVector<Port> ownPorts = isInput ? getOwnInputPorts() : getOwnOutputPorts();
    QVector<Port> childPorts = isInput ? getChildInputPorts() : getChildOutputPorts();
    QVector<Port> aliasPorts = isInput ? getAliasInputPorts() : getAliasOutputPorts();

    // Используем менеджер портов для определения категории
    return UModernDiagramPortManager::determinePortCategory(fullName, nodeName, propertyName, isInput, ownPorts, childPorts, aliasPorts);

    return PortCategory::Own;
}

QVariant UModernDiagramNodeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemPositionHasChanged && scene())
    {
        // Оптимизация: обновляем только связи, подключенные к этому узлу
        // Вместо обновления всех связей в сцене используем кэш m_connectedLinks
        for(UModernDiagramLinkItem* link : m_connectedLinks)
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
                    auto* node = dynamic_cast<UModernDiagramNodeItem*>(item);
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
                        auto* otherNode = dynamic_cast<UModernDiagramNodeItem*>(item);
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

        // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Отслеживаем компоненты с отрицательными позициями во время движения
        // Это должно происходить ВСЕГДА при изменении позиции, не только при сохранении координат
        // Offset будет обновлен только при завершении движения (в mouseReleaseEvent), чтобы предотвратить
        // каскадные обновления во время drag-операции
        // ВАЖНО: Эта проверка должна быть ВНЕ блока if(isSelected()), чтобы работать для всех компонентов
        if(m_owner && !m_owner->m_isUpdatingNormalizationOffset)
        {
            // CRITICAL FIX: Use pos() to get normalized coordinates for negative position check
            // pos() returns normalized coordinates (relative to m_normalizationOffset)
            // scenePos() returns absolute scene coordinates (pos() + m_normalizationOffset)
            QPointF normalizedPos = pos();

            // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Полностью отключаем проверку видимости во время движения
            // Никакие параметры канвы не должны меняться с момента клика до окончания перетаскивания
            // Проверку видимости выполняем только если компонент НЕ двигается
            if(!m_owner->m_isComponentMoving && m_owner->isComponentOutsideVisibleArea(this))
            {
                // Если компонент еще не в set, сохраняем его исходные абсолютные координаты
                // ВАЖНО: Делаем это ДО обновления m_lastNodePositions, чтобы сохранить исходную позицию
                if(!m_owner->m_componentsWithNegativePos.contains(this))
                {
                    // Сохраняем исходные абсолютные координаты ДО перемещения за видимую область
                    // Используем последнюю нормализованную позицию из m_lastNodePositions (ДО обновления)
                    QPointF originalNormalizedPos = m_owner->m_lastNodePositions.value(this, normalizedPos);
                    QPointF originalAbsolutePos = originalNormalizedPos + m_owner->m_coordinateManager->getNormalizationOffset();
                    m_owner->m_originalAbsolutePositions[this] = originalAbsolutePos;

                    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName
                                                                          : m_owner->m_componentName + "." + nodeName;
                    // DEBUG: Commented out to reduce log flood
                    // QString logMsg = QString("[UModernDiagramNodeItem::itemChange] Component '%1' moved outside visible area: originalNormalized=(%2, %3), originalAbsolute=(%4, %5), newNormalized=(%6, %7)")
                    //     .arg(fullName)
                    //     .arg(originalNormalizedPos.x()).arg(originalNormalizedPos.y())
                    //     .arg(originalAbsolutePos.x()).arg(originalAbsolutePos.y())
                    //     .arg(normalizedPos.x()).arg(normalizedPos.y());
                    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
                }

                // Добавляем компонент в set для последующего обновления offset при завершении движения
                m_owner->m_componentsWithNegativePos.insert(this);

                QString fullName = m_owner->m_componentName.isEmpty() ? nodeName
                                                                      : m_owner->m_componentName + "." + nodeName;
                // DEBUG: Commented out to reduce log flood - Logging for debugging visibility check
                // QString logMsg = QString("[UModernDiagramNodeItem::itemChange] Component '%1' is outside visible area: normalizedPos=(%2, %3), will update offset on mouse release")
                //     .arg(fullName)
                //     .arg(normalizedPos.x()).arg(normalizedPos.y());
                // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            }
        }

        // Сохранить координаты
        // Важно: сохраняем абсолютные координаты (с учетом визуальной нормализации)
        // Не сохраняем координаты во время инициализации сцены
        // Используем флаг m_isSavingCoordinates для предотвращения рекурсивных вызовов
        if(m_owner && !m_owner->m_isBuildingScene && !m_owner->m_isSavingCoordinates)
        {
            // Set flag to prevent recursive calls to itemChange when saving coordinates
            m_owner->m_isSavingCoordinates = true;

            QString fullName = m_owner->m_componentName.isEmpty() ? nodeName
                                                                  : m_owner->m_componentName + "." + nodeName;
            // CRITICAL FIX: Use pos() to get normalized coordinates for negative position check
            // pos() returns normalized coordinates (relative to m_normalizationOffset)
            // scenePos() returns absolute scene coordinates (pos() + m_normalizationOffset)
            QPointF normalizedPos = pos();

            // Денормализуем: scenePos() уже нормализован, добавляем offset для получения абсолютных координат
            QPointF absoluteScenePos = normalizedPos + m_owner->m_coordinateManager->getNormalizationOffset();

            // DEBUG: Commented out to reduce log flood - Logging for debugging component movement
            // QString logMsg = QString("[UModernDiagramNodeItem::itemChange] Component '%1' moved: normalizedPos=(%2, %3), offset=(%4, %5), absoluteScenePos=(%6, %7)")
            //     .arg(fullName)
            //     .arg(normalizedPos.x()).arg(normalizedPos.y())
            //     .arg(m_owner->m_coordinateManager->getNormalizationOffset().x()).arg(m_owner->m_coordinateManager->getNormalizationOffset().y())
            //     .arg(absoluteScenePos.x()).arg(absoluteScenePos.y());
            // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

            // Сохраняем координаты без пересчета offset
            // Offset пересчитывается только при загрузке сцены, чтобы избежать бесконечных циклов
            // CRITICAL: Don't save coordinates or update sceneRect during offset update
            // This prevents components from affecting each other's positions during offset adjustment
            if(!m_owner->m_isUpdatingNormalizationOffset && !m_owner->m_isBuildingScene)
            {
                m_owner->m_coordinateManager->saveCoord(fullName, absoluteScenePos);

                // CRITICAL FIX: Не обновляем sceneRect во время движения компонентов
                // Обновление sceneRect отложено до завершения движения (mouseReleaseEvent)
                // Это предотвращает прыжки компонентов при перемещении, особенно групп
            }

            // Clear flag after saving coordinates
            m_owner->m_isSavingCoordinates = false;
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

        // Don't emit componentSelected during programmatic selection to prevent recursion
        if(m_owner->m_isProgrammaticSelection)
        {
            return QGraphicsRectItem::itemChange(change, value);
        }

        // Отслеживаем изменение выбора компонента
        bool isNowSelected = value.toBool();
        if(isNowSelected)
        {
            QString fullName = m_owner->m_componentName.isEmpty() ? nodeName
                                                                  : m_owner->m_componentName + "." + nodeName;
            // Эмитируем сигнал только если имя не пустое
            // При снятии выделения (isNowSelected = false) сигнал не эмитируется - это предотвращает циклы
            if(!fullName.isEmpty())
            {
                // DEBUG: Commented out to reduce log flood
                // QString logMsg = QString("[SELECTION_DEBUG] UModernDiagramNodeItem::itemChange: emitting componentSelected('%1'), isProgrammatic=%2, isBatch=%3")
                //     .arg(fullName)
                //     .arg(m_owner->m_isProgrammaticSelection ? "true" : "false")
                //     .arg(m_owner->m_isBatchSelecting ? "true" : "false");
                // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
                emit m_owner->componentSelected(fullName);
            }
        }
        else
        {
            // DEBUG: Commented out to reduce log flood - Логируем снятие выделения для диагностики
            // QString fullName = m_owner->m_componentName.isEmpty() ? nodeName
            //                                                       : m_owner->m_componentName + "." + nodeName;
            // QString logMsg = QString("[SELECTION_DEBUG] UModernDiagramNodeItem::itemChange: deselected '%1', isProgrammatic=%2, isBatch=%3")
            //     .arg(fullName)
            //     .arg(m_owner->m_isProgrammaticSelection ? "true" : "false")
            //     .arg(m_owner->m_isBatchSelecting ? "true" : "false");
            // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

const Port* UModernDiagramNodeItem::getPortAtPosition(const QPointF& localPos) const
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

void UModernDiagramNodeItem::refreshHoverAtScenePos(const QPointF& scenePos)
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

QVector<Port> UModernDiagramNodeItem::getNestedPorts(bool isInput, bool includeNested) const
{
    if(!m_owner || !m_owner->m_application)
        return QVector<Port>();

    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;

    // Используем менеджер портов для загрузки
    return UModernDiagramPortManager::loadNestedPorts(fullName, nodeName, isInput, includeNested);
}

QVector<Port> UModernDiagramNodeItem::getOwnOutputPorts() const
{
    // ОПТИМИЗАЦИЯ: используем кэш для избежания повторных вызовов API ядра
    if(m_portsCacheValid && !m_cachedOwnOutputPorts.isEmpty())
    {
        return m_cachedOwnOutputPorts;
    }

    // ОПТИМИЗАЦИЯ: проверяем глобальный кэш компонентов
    QString fullName = m_owner ? (m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName) : nodeName;
    UModernDiagramComponentCacheEntry* cacheEntry = m_owner ? m_owner->m_cacheManager->getComponentCache().getEntry(fullName) : nullptr;
    if(cacheEntry && !cacheEntry->ownOutputPorts.isEmpty())
    {
        // Восстанавливаем из глобального кэша в локальный
        m_cachedOwnOutputPorts = cacheEntry->ownOutputPorts;
        m_portsCacheValid = true;
        return m_cachedOwnOutputPorts;
    }

    if(!m_owner || !m_owner->m_application)
        return QVector<Port>();

    // Используем менеджер портов для загрузки
    QVector<Port> result = UModernDiagramPortManager::loadOwnOutputPorts(fullName, nodeName);

    // Сохраняем в локальный кэш
    m_cachedOwnOutputPorts = result;
    m_portsCacheValid = true;

    // Сохраняем в глобальный кэш
    if(m_owner)
    {
        if(!cacheEntry)
        {
            UModernDiagramComponentCacheEntry newEntry;
            newEntry.ownOutputPorts = result;
            newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
            m_owner->m_cacheManager->getComponentCache().setEntry(fullName, newEntry);
        }
        else
        {
            cacheEntry->ownOutputPorts = result;
            cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
        }
    }

    return result;
}

QVector<Port> UModernDiagramNodeItem::getChildOutputPorts() const
{
    // ОПТИМИЗАЦИЯ: используем кэш для избежания повторных вызовов API ядра
    if(m_portsCacheValid && !m_cachedChildOutputPorts.isEmpty())
    {
        return m_cachedChildOutputPorts;
    }

    // ОПТИМИЗАЦИЯ: проверяем глобальный кэш компонентов
    QString fullName = m_owner ? (m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName) : nodeName;
    UModernDiagramComponentCacheEntry* cacheEntry = m_owner ? m_owner->m_cacheManager->getComponentCache().getEntry(fullName) : nullptr;
    if(cacheEntry && !cacheEntry->childOutputPorts.isEmpty())
    {
        // Восстанавливаем из глобального кэша в локальный
        m_cachedChildOutputPorts = cacheEntry->childOutputPorts;
        m_portsCacheValid = true;
        return m_cachedChildOutputPorts;
    }

    if(!m_owner || !m_owner->m_application)
        return QVector<Port>();

    // Используем менеджер портов для загрузки
    QVector<Port> result = UModernDiagramPortManager::loadChildOutputPorts(fullName, nodeName);

    // Сохраняем в локальный кэш
    m_cachedChildOutputPorts = result;
    m_portsCacheValid = true;

    // Сохраняем в глобальный кэш
    if(m_owner)
    {
        if(!cacheEntry)
        {
            UModernDiagramComponentCacheEntry newEntry;
            newEntry.childOutputPorts = result;
            newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
            m_owner->m_cacheManager->getComponentCache().setEntry(fullName, newEntry);
        }
        else
        {
            cacheEntry->childOutputPorts = result;
            cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
        }
    }

    return result;
}

QVector<Port> UModernDiagramNodeItem::getAliasOutputPorts() const
{
    // ОПТИМИЗАЦИЯ: используем кэш для избежания повторных вызовов API ядра
    if(m_portsCacheValid && !m_cachedAliasOutputPorts.isEmpty())
    {
        return m_cachedAliasOutputPorts;
    }

    // ОПТИМИЗАЦИЯ: проверяем глобальный кэш компонентов
    QString fullName = m_owner ? (m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName) : nodeName;
    UModernDiagramComponentCacheEntry* cacheEntry = m_owner ? m_owner->m_cacheManager->getComponentCache().getEntry(fullName) : nullptr;
    if(cacheEntry && !cacheEntry->aliasOutputPorts.isEmpty())
    {
        // Восстанавливаем из глобального кэша в локальный
        m_cachedAliasOutputPorts = cacheEntry->aliasOutputPorts;
        m_portsCacheValid = true;
        return m_cachedAliasOutputPorts;
    }

    if(!m_owner || !m_owner->m_application)
        return QVector<Port>();

    // Используем менеджер портов для загрузки
    QVector<Port> result = UModernDiagramPortManager::loadAliasOutputPorts(fullName, nodeName);

    // Сохраняем в локальный кэш
    m_cachedAliasOutputPorts = result;
    m_portsCacheValid = true;

    // Сохраняем в глобальный кэш
    if(m_owner)
    {
        if(!cacheEntry)
        {
            UModernDiagramComponentCacheEntry newEntry;
            newEntry.aliasOutputPorts = result;
            newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
            m_owner->m_cacheManager->getComponentCache().setEntry(fullName, newEntry);
        }
        else
        {
            cacheEntry->aliasOutputPorts = result;
            cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
        }
    }

    return result;
}

QVector<Port> UModernDiagramNodeItem::getOwnInputPorts() const
{
    // ОПТИМИЗАЦИЯ: сначала проверяем локальный кэш
    if(m_portsCacheValid && !m_cachedOwnInputPorts.isEmpty())
    {
        return m_cachedOwnInputPorts;
    }

    // ОПТИМИЗАЦИЯ: проверяем глобальный кэш компонентов
    QString fullName = m_owner ? (m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName) : nodeName;
    UModernDiagramComponentCacheEntry* cacheEntry = m_owner ? m_owner->m_cacheManager->getComponentCache().getEntry(fullName) : nullptr;
    if(cacheEntry && !cacheEntry->ownInputPorts.isEmpty())
    {
        // Восстанавливаем из глобального кэша в локальный
        m_cachedOwnInputPorts = cacheEntry->ownInputPorts;
        m_portsCacheValid = true;
        return m_cachedOwnInputPorts;
    }

    if(!m_owner || !m_owner->m_application)
        return QVector<Port>();

    // Используем менеджер портов для загрузки
    QVector<Port> result = UModernDiagramPortManager::loadOwnInputPorts(fullName, nodeName);

    // Сохраняем в локальный кэш
    m_cachedOwnInputPorts = result;
    m_portsCacheValid = true;

    // Сохраняем в глобальный кэш
    if(m_owner)
    {
        if(!cacheEntry)
        {
            UModernDiagramComponentCacheEntry newEntry;
            newEntry.ownInputPorts = result;
            newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
            m_owner->m_cacheManager->getComponentCache().setEntry(fullName, newEntry);
        }
        else
        {
            cacheEntry->ownInputPorts = result;
            cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
        }
    }

    return result;
}

QVector<Port> UModernDiagramNodeItem::getChildInputPorts() const
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

QVector<Port> UModernDiagramNodeItem::getAliasInputPorts() const
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

QVector<Port> UModernDiagramNodeItem::getChildInputPortsRecursive() const
{
    if(!m_owner || !m_owner->m_application)
        return QVector<Port>();

    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;

    // Используем менеджер портов для рекурсивной загрузки
    QVector<UModernDiagramPort> ports = UModernDiagramPortManager::loadChildInputPortsRecursive(fullName, nodeName);

    // Конвертируем UModernDiagramPort в Port
    QVector<Port> result;
    for(const UModernDiagramPort& p : ports)
    {
        Port port;
        port.isInput = p.isInput;
        port.name = p.name;
        port.componentName = p.componentName;
        port.fullPath = p.fullPath;
        port.displayName = p.displayName;
        port.category = static_cast<PortCategory>(p.category);
        result.append(port);
    }

    return result;
}

QVector<Port> UModernDiagramNodeItem::getChildOutputPortsRecursive() const
{
    if(!m_owner || !m_owner->m_application)
        return QVector<Port>();

    QString fullName = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;

    // Используем менеджер портов для рекурсивной загрузки
    QVector<UModernDiagramPort> ports = UModernDiagramPortManager::loadChildOutputPortsRecursive(fullName, nodeName);

    // Конвертируем UModernDiagramPort в Port
    QVector<Port> result;
    for(const UModernDiagramPort& p : ports)
    {
        Port port;
        port.isInput = p.isInput;
        port.name = p.name;
        port.componentName = p.componentName;
        port.fullPath = p.fullPath;
        port.displayName = p.displayName;
        port.category = static_cast<PortCategory>(p.category);
        result.append(port);
    }

    return result;
}

bool UModernDiagramNodeItem::hasConnectionsToInputCategory(PortCategory category) const
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

    for(UModernDiagramLinkItem* link : m_connectedLinks)
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

bool UModernDiagramNodeItem::hasConnectionsToOutputCategory(PortCategory category) const
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

    for(UModernDiagramLinkItem* link : m_connectedLinks)
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

void UModernDiagramNodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverEnterEvent(event);
    m_hoveredPort = getPortAtPosition(event->pos());
    update(); // Только визуальная подсветка
    // Убрано: showPortListWidget - окно открывается только при клике

    // Set initial tooltip
    if(m_hoveredPort && m_owner)
    {
        QString tooltip = UModernDiagramTooltipGenerator::generatePortTooltip(*m_hoveredPort);
        setToolTip(tooltip);
    }
    else if(m_owner)
    {
        QString tooltip = UModernDiagramTooltipGenerator::generateNodeTooltip(this);
        setToolTip(tooltip);
    }
}

void UModernDiagramNodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
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
            QString tooltip = UModernDiagramTooltipGenerator::generatePortTooltip(*m_hoveredPort);
            setToolTip(tooltip);
        }
        else if(m_owner)
        {
            QString tooltip = UModernDiagramTooltipGenerator::generateNodeTooltip(this);
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
        QString tooltip = UModernDiagramTooltipGenerator::generatePortTooltip(*m_hoveredPort);
        setToolTip(tooltip);
    }
    else if(m_owner)
    {
        QString tooltip = UModernDiagramTooltipGenerator::generateNodeTooltip(this);
        setToolTip(tooltip);
    }
}

void UModernDiagramNodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
    m_hoveredPort = nullptr;
    m_lastHoverMovePos = QPointF(); // Сбрасываем позицию при выходе
    if(m_hideTimer)
        m_hideTimer->start();
    setToolTip(QString()); // Clear tooltip on leave
    update();
}

void UModernDiagramNodeItem::showPortListWidget(const QPointF& scenePos)
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
    for(UModernDiagramNodeItem* node : m_owner->m_nodes)
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

void UModernDiagramNodeItem::hidePortListWidget()
{
    if(m_portListWidgetProxy)
    {
        m_portListWidgetProxy->setVisible(false);
    }
}

void UModernDiagramNodeItem::onPortItemActivated(QTreeWidgetItem* item, int column)
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

            // Логируем начало обработки с детальной информацией
            QString logMsg = QString("onPortItemActivated: Processing %1 port '%2' in component '%3', fullPath='%4', activeTempLink=%5")
                .arg(isInput ? "input" : "output")
                .arg(portName)
                .arg(componentName)
                .arg(fullPath)
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
            // Важно: пути должны быть полными от корня модели, независимо от текущего уровня вложенности
            QString srcName = m_owner->m_activeSourceNode->nodeName;
            QString fullSrc = m_owner->m_componentName.isEmpty() ? srcName : m_owner->m_componentName + "." + srcName;

            // Если источник имеет componentName, добавляем его к fullSrc
            if(!m_owner->m_activeSourcePortComponentName.isEmpty() &&
               m_owner->m_activeSourcePortComponentName != srcName)
            {
                // Проверяем, начинается ли m_activeSourcePortComponentName с fullSrc
                if(!m_owner->m_activeSourcePortComponentName.startsWith(fullSrc + "."))
                {
                    // m_activeSourcePortComponentName - это путь относительно srcName
                    // Добавляем его к fullSrc
                    fullSrc = fullSrc + "." + m_owner->m_activeSourcePortComponentName;
                }
                else
                {
                    // m_activeSourcePortComponentName уже содержит полный путь
                    fullSrc = m_owner->m_activeSourcePortComponentName;
                }
            }

            // Формируем fullDst с учетом того, что componentName может содержать полный путь
            // fullDst должен быть полным путем к компоненту от корня модели
            QString fullDst;
            if(componentName.isEmpty() || componentName == nodeName)
            {
                // Собственный порт компонента
                if(m_owner->m_componentName.isEmpty())
                {
                    fullDst = nodeName;
                }
                else
                {
                    // Проверяем, не содержит ли m_componentName уже nodeName
                    if(m_owner->m_componentName == nodeName || m_owner->m_componentName.endsWith("." + nodeName))
                    {
                        fullDst = m_owner->m_componentName;
                    }
                    else
                    {
                        fullDst = m_owner->m_componentName + "." + nodeName;
                    }
                }
            }
            else
            {
                // Порты дочерних компонентов - componentName содержит путь относительно nodeName
                // Например: componentName = "Dendrite1_1.InhSynapse1" относительно "Neuron"
                // fullPath формируется относительно fullName = "NeuronTrainer.Neuron"
                // Поэтому fullDst должен быть "NeuronTrainer.Neuron.Dendrite1_1.InhSynapse1"
                if(m_owner->m_componentName.isEmpty())
                {
                    // Мы на корневом уровне модели
                    // componentName - это путь относительно nodeName
                    // fullDst должен быть nodeName + "." + componentName
                    // Например: nodeName = "NeuronTrainer", componentName = "Source1"
                    // Результат: fullDst = "NeuronTrainer.Source1"
                    fullDst = nodeName + "." + componentName;
                }
                else
                {
                    // Проверяем, начинается ли componentName с m_componentName (уже полный путь)
                    if(componentName.startsWith(m_owner->m_componentName + "."))
                    {
                        // componentName уже содержит полный путь от корня
                        fullDst = componentName;
                    }
                    else if(componentName.startsWith(nodeName + "."))
                    {
                        // componentName начинается с nodeName, добавляем m_componentName
                        // Например: componentName = "Neuron.Source1", nodeName = "Neuron"
                        // Результат: fullDst = "NeuronTrainer.Neuron.Source1"
                        fullDst = m_owner->m_componentName + "." + componentName;
                    }
                    else
                    {
                        // componentName - это путь относительно nodeName (не m_componentName!)
                        // Например: componentName = "Source1", nodeName = "Neuron"
                        // fullPath формируется относительно fullName = m_componentName + "." + nodeName
                        // Поэтому fullDst должен быть m_componentName + "." + nodeName + "." + componentName
                        // Например: "NeuronTrainer.Neuron.Source1"
                        QString basePath = m_owner->m_componentName.isEmpty() ? nodeName : m_owner->m_componentName + "." + nodeName;
                        fullDst = basePath + "." + componentName;
                    }
                }
            }

            // Диагностическое логирование для отладки
            QString fullDstDebugMsg = QString("onPortItemActivated: fullDst formation - m_componentName='%1', nodeName='%2', componentName='%3', fullPath='%4', portName='%5', result fullDst='%6'")
                .arg(m_owner->m_componentName).arg(nodeName).arg(componentName).arg(fullPath).arg(portName).arg(fullDst);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, fullDstDebugMsg.toStdString().c_str(), 0);

            // Логируем промежуточные значения для диагностики

            // Формируем пути свойств, используя сохраненные копии вместо указателя
            // srcProp должен быть путем к свойству ОТНОСИТЕЛЬНО компонента fullSrc
            QString srcProp;
            if(m_owner->m_activeSourcePortFullPath.isEmpty())
            {
                srcProp = m_owner->m_activeSourcePortName;
            }
            else
            {
                // fullPath может содержать путь относительно текущего компонента
                // Нужно извлечь только путь к свойству относительно fullSrc
                QString fullPath = m_owner->m_activeSourcePortFullPath;

                // Если fullPath начинается с componentName, извлекаем только имя свойства
                if(!m_owner->m_activeSourcePortComponentName.isEmpty() &&
                   fullPath.startsWith(m_owner->m_activeSourcePortComponentName + "."))
                {
                    // fullPath = "LTZone.Output", componentName = "LTZone"
                    // Извлекаем только "Output"
                    srcProp = fullPath.mid(m_owner->m_activeSourcePortComponentName.length() + 1);
                }
                else
                {
                    // fullPath не начинается с componentName, используем его как есть
                    srcProp = fullPath;
                }
            }

            // Формируем dstProp - путь к свойству ОТНОСИТЕЛЬНО компонента fullDst
            // fullDst уже содержит полный путь к компоненту, поэтому dstProp должен быть только путем к свойству
            QString dstProp;
            if(!fullPath.isEmpty())
            {
                if(!componentName.isEmpty() && componentName != nodeName)
                {
                    // Порты дочерних компонентов
                    if(fullPath.startsWith(componentName + "."))
                    {
                        // fullPath начинается с componentName + ".", извлекаем только часть после componentName
                        // Например: fullPath = "Dendrite1_1.ExcSynapse1.Input", componentName = "Dendrite1_1.ExcSynapse1"
                        // Результат: dstProp = "Input"
                        dstProp = fullPath.mid(componentName.length() + 1);
                    }
                    else if(fullPath.startsWith(componentName))
                    {
                        // fullPath начинается с componentName (возможно без точки)
                        // Извлекаем часть после componentName
                        int skipLength = componentName.length();
                        if(fullPath.length() > skipLength && fullPath[skipLength] == '.')
                            skipLength++;
                        dstProp = fullPath.mid(skipLength);
                    }
                    else
                    {
                        // fullPath не начинается с componentName
                        // Проверяем, является ли fullPath просто именем свойства
                        if(fullPath == portName || (!fullPath.contains('.') && fullPath == portName))
                        {
                            // fullPath - это просто имя свойства, используем его как есть
                            dstProp = portName;
                        }
                        else
                        {
                            // fullPath содержит путь, но не начинается с componentName
                            // Это может быть путь к более глубоко вложенному компоненту
                            // Используем fullPath как есть (он должен быть относительным к fullDst)
                            dstProp = fullPath;
                        }
                    }
                }
                else
                {
                    // componentName пуст или равен nodeName (собственный порт компонента)
                    // Используем fullPath или portName
                    if(fullPath == portName || !fullPath.contains('.'))
                    {
                        dstProp = portName;
                    }
                    else
                    {
                        dstProp = fullPath;
                    }
                }
            }
            else
            {
                // fullPath пуст - это может быть порт первого уровня дочернего компонента
                // В этом случае componentName содержит путь к компоненту, а portName - имя свойства
                // Но fullDst уже содержит полный путь к компоненту, поэтому dstProp должен быть просто portName
                if(!componentName.isEmpty() && componentName != nodeName)
                {
                    // Для дочерних компонентов без fullPath используем только portName
                    // потому что fullDst уже содержит путь к компоненту
                    dstProp = portName;
                }
                else
                {
                    dstProp = portName;
                }
            }

            // Диагностическое логирование для отладки
            QString debugMsg = QString("onPortItemActivated: dstProp formation - componentName='%1', fullPath='%2', portName='%3', nodeName='%4', result dstProp='%5'")
                .arg(componentName).arg(fullPath).arg(portName).arg(nodeName).arg(dstProp);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, debugMsg.toStdString().c_str(), 0);

            // Учитываем вложенные компоненты для источника
            // fullSrc уже содержит полный путь к компоненту порта (например, "PNeuronS1D1Syn1.LTZone")
            // srcProp должен быть только путем к свойству относительно этого компонента
            // НЕ добавляем componentName к srcProp, так как fullSrc уже содержит его
            // Например: fullSrc = "PNeuronS1D1Syn1.LTZone", srcProp = "Output"

            // Диагностическое логирование для источника
            QString srcDebugMsg = QString("onPortItemActivated: srcProp formation - activeSourcePortComponentName='%1', activeSourcePortFullPath='%2', activeSourcePortName='%3', srcName='%4', result srcProp='%5'")
                .arg(m_owner->m_activeSourcePortComponentName)
                .arg(m_owner->m_activeSourcePortFullPath)
                .arg(m_owner->m_activeSourcePortName)
                .arg(srcName)
                .arg(srcProp);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, srcDebugMsg.toStdString().c_str(), 0);

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
            m_owner->m_activeTempLink = new UModernDiagramLinkItem(this, cursorScenePos, m_owner->m_activeSourcePortPos);
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

void UModernDiagramNodeItem::updatePortListWidget(bool isInput, bool includeNested)
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
            // При Shift используем рекурсивные методы для Child портов (на всю глубину)
            if(isInput)
            {
                availablePorts.append(getOwnInputPorts());
                availablePorts.append(getChildInputPortsRecursive());
                availablePorts.append(getAliasInputPorts());
            }
            else
            {
                availablePorts.append(getOwnOutputPorts());
                availablePorts.append(getChildOutputPortsRecursive());
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
                    // При Shift используем рекурсивный метод для показа портов на всю глубину
                    bool shiftPressed = QApplication::keyboardModifiers() & Qt::ShiftModifier;
                    availablePorts = shiftPressed ? getChildInputPortsRecursive() : getChildInputPorts();
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
                    // При Shift используем рекурсивный метод для показа портов на всю глубину
                    bool shiftPressed = QApplication::keyboardModifiers() & Qt::ShiftModifier;
                    availablePorts = shiftPressed ? getChildOutputPortsRecursive() : getChildOutputPorts();
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
        // Группируем порты по компонентам с иерархической структурой
        // Используем полный путь компонента для создания иерархии
        QMap<QString, QTreeWidgetItem*> componentItems;  // Ключ - полный путь компонента
        QTreeWidgetItem* currentPortItem = nullptr;

        for(const Port& p : availablePorts)
        {
            // Определяем родительский элемент для порта
            QTreeWidgetItem* parentItem = m_portListWidget->invisibleRootItem();

            // Если componentName содержит точки, создаем иерархическую структуру
            if(!p.componentName.isEmpty() && p.componentName.contains('.'))
            {
                QStringList pathParts = p.componentName.split('.');
                QString currentPath;

                // Создаем иерархию компонентов
                for(int i = 0; i < pathParts.size(); ++i)
                {
                    if(i > 0)
                        currentPath += ".";
                    currentPath += pathParts[i];

                    if(!componentItems.contains(currentPath))
                    {
                        QTreeWidgetItem* compItem = new QTreeWidgetItem(parentItem);
                        compItem->setText(0, pathParts[i]);
                        componentItems[currentPath] = compItem;
                        parentItem = compItem;
                    }
                    else
                    {
                        parentItem = componentItems[currentPath];
                    }
                }
            }
            else
            {
                // Простой случай - один уровень компонента
                if(!componentItems.contains(p.componentName))
                {
                    QTreeWidgetItem* compItem = new QTreeWidgetItem(parentItem);
                    compItem->setText(0, p.componentName.isEmpty() ? nodeName : p.componentName);
                    componentItems[p.componentName] = compItem;
                    parentItem = compItem;
                }
                else
                {
                    parentItem = componentItems[p.componentName];
                }
            }

            // Создаем элемент порта
            QTreeWidgetItem* portItem = new QTreeWidgetItem(parentItem);
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

        // Разворачиваем все элементы для показа иерархии
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
