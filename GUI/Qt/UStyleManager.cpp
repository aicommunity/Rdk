#include "UStyleManager.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QDockWidget>
#include <QMdiSubWindow>
#include <QEvent>
#include <QWidget>
#include <QScreen>
#include <QGuiApplication>

namespace {

class UTitleFontPolisher : public QObject
{
public:
    explicit UTitleFontPolisher(UStyleManager* styles, QObject* parent = nullptr)
        : QObject(parent)
        , m_styles(styles)
    {
    }

    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if(!m_styles)
            return QObject::eventFilter(watched, event);

        const QEvent::Type type = event->type();
        if(type != QEvent::Polish && type != QEvent::Show)
            return QObject::eventFilter(watched, event);
        if(!watched->isWidgetType())
            return QObject::eventFilter(watched, event);

        if(auto* dock = qobject_cast<QDockWidget*>(watched))
            m_styles->applyTitleBarFont(dock);
        else if(auto* mdi = qobject_cast<QMdiSubWindow*>(watched))
            m_styles->applyTitleBarFont(mdi);

        return QObject::eventFilter(watched, event);
    }

private:
    UStyleManager* m_styles;
};

} // namespace

UStyleManager* UStyleManager::s_instance = nullptr;

UStyleManager* UStyleManager::instance()
{
    if (!s_instance)
    {
        s_instance = new UStyleManager();
    }
    return s_instance;
}

UStyleManager::UStyleManager()
    : QObject(nullptr)
{
    setDefaults();
}

void UStyleManager::setDefaults()
{
    // Цвета узлов (из UModernDiagramWidget)
    m_nodeFill = QColor(245, 245, 245);           // #F5F5F5
    m_nodeFillSelected = QColor(220, 240, 255);   // #DCF0FF
    m_nodeBorder = QColor(70, 120, 200);          // #4678C8
    m_nodeBorderWidth = 1.5;
    m_nodeCornerRadius = 6.0;
    
    // Цвета портов
    m_portInputNormal = QColor(50, 150, 50);      // #329632
    m_portInputHover = QColor(100, 200, 100);     // #64C864
    m_portOutputNormal = QColor(50, 100, 200);    // #3264C8
    m_portOutputHover = QColor(100, 150, 255);    // #6496FF
    m_portRadius = 4.0;
    m_portHoverRadius = 5.0;
    
    // Цвета связей
    m_linkSolid = QColor(80, 120, 200);           // #5078C8
    m_linkDashed = QColor(120, 160, 220);         // #78A0DC
    m_linkWidth = 2.0;
    
    // Цвета drag
    m_dragLineColor = QColor(100, 100, 255, 200);
    m_portHighlightColor = QColor(0, 200, 0, 150);
    
    // Текстовые цвета
    m_textColor = QColor(0, 0, 0);                // черный
    m_textSecondary = QColor(100, 100, 100);      // серый
    
    // Общие цвета
    m_backgroundColor = QColor(255, 255, 255);    // белый
    m_backgroundAlt = QColor(245, 245, 245);      // светло-серый
    m_accentColor = QColor(70, 120, 200);         // #4678C8
    m_selectionColor = QColor(224, 224, 224);     // #E0E0E0
    
    // Цвета графиков
    m_chartBackground = QColor(255, 255, 255);    // белый
    m_chartAxis = QColor(51, 51, 51);             // #333333
    m_chartGrid = QColor(224, 224, 224);          // #E0E0E0
    m_chartTitle = QColor(51, 51, 51);            // #333333
    m_chartLabel = QColor(102, 102, 102);         // #666666
    
    // Цвета серий графиков (15 цветов, как в оригинале)
    m_chartSeriesColors = {
        QColor(Qt::red),         // 0
        QColor(Qt::darkRed),     // 1
        QColor(Qt::yellow),      // 2
        QColor(Qt::darkYellow),  // 3
        QColor(Qt::green),       // 4
        QColor(Qt::darkGreen),   // 5
        QColor(Qt::cyan),        // 6
        QColor(Qt::darkCyan),    // 7
        QColor(Qt::blue),        // 8
        QColor(Qt::darkBlue),    // 9
        QColor(Qt::magenta),     // 10
        QColor(Qt::darkMagenta), // 11
        QColor(Qt::gray),        // 12
        QColor(Qt::darkGray),    // 13
        QColor(Qt::black)        // 14
    };
    
    // Цвет для отключенных элементов
    m_disabledText = QColor(153, 153, 153);       // #999999
    
    // Градиенты (современная светлая тема)
    m_nodeGradientTop = QColor(255, 255, 255);    // #FFFFFF
    m_nodeGradientBottom = QColor(248, 250, 252); // #F8FAFC
    m_buttonGradientTop = QColor(91, 141, 239);   // #5B8DEF
    m_buttonGradientBottom = QColor(79, 123, 232);// #4F7BE8
    m_dockTitleGradientTop = QColor(91, 141, 239);// #5B8DEF
    m_dockTitleGradientBottom = QColor(79, 123, 232); // #4F7BE8
    m_headerGradientTop = QColor(248, 250, 252);  // #F8FAFC
    m_headerGradientBottom = QColor(241, 245, 249); // #F1F5F9
    
    // Эффекты теней
    m_shadowColor = QColor(0, 0, 0, 26);          // #0000001A
    m_shadowBlur = 12.0;
    m_shadowOffsetX = 0.0;
    m_shadowOffsetY = 4.0;
    
    // Эффекты свечения
    m_glowColor = QColor(91, 141, 239, 64);       // #5B8DEF40
    m_glowRadius = 6.0;
    
    // Цвета статусов
    m_successColor = QColor(16, 185, 129);        // #10B981
    m_warningColor = QColor(245, 158, 11);        // #F59E0B
    m_errorColor = QColor(239, 68, 68);           // #EF4444
    m_infoColor = QColor(91, 141, 239);           // #5B8DEF
    
    // Имя темы
    m_themeName = "Modern Light";
}

bool UStyleManager::loadTheme(const QString& themeJsonPath)
{
    QFile file(themeJsonPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "UStyleManager: Cannot open theme file:" << themeJsonPath;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning() << "UStyleManager: JSON parse error:" << parseError.errorString();
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Parse node colors
    if (root.contains("node"))
    {
        QJsonObject node = root["node"].toObject();
        m_nodeFill = parseColor(node, "fill", m_nodeFill);
        m_nodeFillSelected = parseColor(node, "fillSelected", m_nodeFillSelected);
        m_nodeBorder = parseColor(node, "border", m_nodeBorder);
        m_nodeBorderWidth = parseDouble(node, "borderWidth", m_nodeBorderWidth);
        m_nodeCornerRadius = parseDouble(node, "cornerRadius", m_nodeCornerRadius);
    }
    
    // Parse port colors
    if (root.contains("port"))
    {
        QJsonObject port = root["port"].toObject();
        m_portInputNormal = parseColor(port, "inputNormal", m_portInputNormal);
        m_portInputHover = parseColor(port, "inputHover", m_portInputHover);
        m_portOutputNormal = parseColor(port, "outputNormal", m_portOutputNormal);
        m_portOutputHover = parseColor(port, "outputHover", m_portOutputHover);
        m_portRadius = parseDouble(port, "radius", m_portRadius);
        m_portHoverRadius = parseDouble(port, "hoverRadius", m_portHoverRadius);
    }
    
    // Parse link colors
    if (root.contains("link"))
    {
        QJsonObject link = root["link"].toObject();
        m_linkSolid = parseColor(link, "solid", m_linkSolid);
        m_linkDashed = parseColor(link, "dashed", m_linkDashed);
        m_linkWidth = parseDouble(link, "width", m_linkWidth);
    }
    
    // Parse drag colors
    if (root.contains("drag"))
    {
        QJsonObject drag = root["drag"].toObject();
        m_dragLineColor = parseColor(drag, "lineColor", m_dragLineColor);
        m_portHighlightColor = parseColor(drag, "portHighlight", m_portHighlightColor);
    }
    
    // Parse text colors
    if (root.contains("text"))
    {
        QJsonObject text = root["text"].toObject();
        m_textColor = parseColor(text, "primary", m_textColor);
        m_textSecondary = parseColor(text, "secondary", m_textSecondary);
    }
    
    // Parse general colors
    if (root.contains("general"))
    {
        QJsonObject general = root["general"].toObject();
        m_backgroundColor = parseColor(general, "background", m_backgroundColor);
        m_backgroundAlt = parseColor(general, "backgroundAlt", m_backgroundAlt);
        m_accentColor = parseColor(general, "accent", m_accentColor);
        m_selectionColor = parseColor(general, "selection", m_selectionColor);
    }
    
    // Parse chart colors
    if (root.contains("chart"))
    {
        QJsonObject chart = root["chart"].toObject();
        m_chartBackground = parseColor(chart, "background", m_chartBackground);
        m_chartAxis = parseColor(chart, "axis", m_chartAxis);
        m_chartGrid = parseColor(chart, "grid", m_chartGrid);
        m_chartTitle = parseColor(chart, "title", m_chartTitle);
        m_chartLabel = parseColor(chart, "label", m_chartLabel);
        m_chartSeriesColors = parseColorArray(chart, "seriesColors", m_chartSeriesColors);
    }
    
    // Parse disabled colors
    if (root.contains("disabled"))
    {
        QJsonObject disabled = root["disabled"].toObject();
        m_disabledText = parseColor(disabled, "text", m_disabledText);
    }
    
    // Parse gradients
    if (root.contains("gradients"))
    {
        QJsonObject gradients = root["gradients"].toObject();
        m_nodeGradientTop = parseColor(gradients, "nodeTop", m_nodeGradientTop);
        m_nodeGradientBottom = parseColor(gradients, "nodeBottom", m_nodeGradientBottom);
        m_buttonGradientTop = parseColor(gradients, "buttonTop", m_buttonGradientTop);
        m_buttonGradientBottom = parseColor(gradients, "buttonBottom", m_buttonGradientBottom);
        m_dockTitleGradientTop = parseColor(gradients, "dockTitleTop", m_dockTitleGradientTop);
        m_dockTitleGradientBottom = parseColor(gradients, "dockTitleBottom", m_dockTitleGradientBottom);
        m_headerGradientTop = parseColor(gradients, "headerTop", m_headerGradientTop);
        m_headerGradientBottom = parseColor(gradients, "headerBottom", m_headerGradientBottom);
    }
    
    // Parse effects
    if (root.contains("effects"))
    {
        QJsonObject effects = root["effects"].toObject();
        m_shadowColor = parseColor(effects, "shadowColor", m_shadowColor);
        m_shadowBlur = parseDouble(effects, "shadowBlur", m_shadowBlur);
        m_shadowOffsetX = parseDouble(effects, "shadowOffsetX", m_shadowOffsetX);
        m_shadowOffsetY = parseDouble(effects, "shadowOffsetY", m_shadowOffsetY);
        m_glowColor = parseColor(effects, "glowColor", m_glowColor);
        m_glowRadius = parseDouble(effects, "glowRadius", m_glowRadius);
    }
    
    // Parse status colors
    m_successColor = parseColor(root, "success", m_successColor);
    m_warningColor = parseColor(root, "warning", m_warningColor);
    m_errorColor = parseColor(root, "error", m_errorColor);
    m_infoColor = parseColor(root, "info", m_infoColor);
    
    // Parse theme name
    m_themeName = root["name"].toString(m_themeName);
    
    qDebug() << "UStyleManager: Theme loaded from" << themeJsonPath << "(" << m_themeName << ")";
    return true;
}

bool UStyleManager::loadStyleSheet(const QString& qssPath)
{
    QFile file(qssPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "UStyleManager: Cannot open stylesheet:" << qssPath;
        return false;
    }
    
    m_styleSheet = QString::fromUtf8(file.readAll());
    file.close();

    // setStyleSheet(string) loses the QSS file base path — resolve icon urls.
    const QString stylesDir = QFileInfo(qssPath).absolutePath();
    const QString iconsDir = QDir(stylesDir).filePath(QStringLiteral("icons"));
    m_styleSheet.replace(QStringLiteral("url(icons/"),
                         QStringLiteral("url(%1/").arg(QDir::fromNativeSeparators(iconsDir)));
    
    qDebug() << "UStyleManager: Stylesheet loaded from" << qssPath;
    return true;
}

void UStyleManager::applyGlobalStyleSheet(QApplication* app)
{
    if (app && !m_styleSheet.isEmpty())
    {
        app->setStyleSheet(m_styleSheet);
        qDebug() << "UStyleManager: Global stylesheet applied";
    }
    applySystemUiFonts(app);
}

void UStyleManager::applySystemUiFonts(QApplication* app)
{
    if(!app)
        return;

    QFont uiFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    if(uiFont.pointSizeF() <= 0 && uiFont.pixelSize() <= 0)
        uiFont = app->font();

#ifdef Q_OS_WIN
    // Soft-cap oversized Windows UI fonts so compact chrome stays usable.
    qreal scale = 1.0;
    if(QScreen* screen = QGuiApplication::primaryScreen())
    {
        scale = qMax(screen->logicalDotsPerInchX() / 96.0, screen->devicePixelRatio());
    }
    constexpr qreal kScaleCapThreshold = 1.25;
    constexpr qreal kMaxPointSize = 12.0;
    if(scale >= kScaleCapThreshold && uiFont.pointSizeF() > kMaxPointSize)
    {
        static bool logged = false;
        if(!logged)
        {
            qDebug() << "UStyleManager: capping UI font from" << uiFont.pointSizeF()
                     << "pt to" << kMaxPointSize << "pt (scale" << scale << ")";
            logged = true;
        }
        uiFont.setPointSizeF(kMaxPointSize);
    }
#endif

    app->setFont(uiFont);

    if(!m_titleFontPolisher)
    {
        auto* polisher = new UTitleFontPolisher(this, app);
        app->installEventFilter(polisher);
        m_titleFontPolisher = polisher;
    }

    const auto widgets = app->allWidgets();
    for(QWidget* widget : widgets)
    {
        if(qobject_cast<QDockWidget*>(widget) || qobject_cast<QMdiSubWindow*>(widget))
            applyTitleBarFont(widget);
    }
}

int UStyleManager::densitySpace(int units)
{
    if(units <= 0)
        return 0;
    QFontMetrics fm(QApplication::font());
    const int step = qMax(1, fm.height() / 4);
    return units * step;
}

QFont UStyleManager::titleBarFont() const
{
    // Use the desktop UI font (not GNOME titlebar-font): users typically enlarge
    // Interface Text, while Window Title stays at the default 11pt.
    QFont font = QApplication::font();
    if(font.pointSizeF() <= 0 && font.pixelSize() <= 0)
        font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    font.setWeight(QFont::DemiBold);
    return font;
}

void UStyleManager::applyTitleBarFont(QWidget* titleHost) const
{
    if(!titleHost)
        return;

    titleHost->setFont(titleBarFont());

    if(auto* dock = qobject_cast<QDockWidget*>(titleHost))
    {
        if(QWidget* content = dock->widget())
            content->setFont(QApplication::font());
    }
    else if(auto* mdi = qobject_cast<QMdiSubWindow*>(titleHost))
    {
        if(QWidget* content = mdi->widget())
            content->setFont(QApplication::font());
    }
}

QString UStyleManager::getTreeWidgetStyleSheet() const
{
    // Stylesheet for QTreeWidget used in port list popup
    return QString(
        "QTreeWidget {"
        "  background-color: %1;"
        "  border: 1px solid %2;"
        "  border-radius: 4px;"
        "}"
        "QTreeWidget::item {"
        "  padding: 2px;"
        "  color: %3;"
        "}"
        "QTreeWidget::item:selected {"
        "  background-color: %4;"
        "  color: %3;"
        "}"
    ).arg(m_backgroundColor.name())
     .arg(m_textSecondary.name())
     .arg(m_textColor.name())
     .arg(m_selectionColor.name());
}

QColor UStyleManager::parseColor(const QJsonObject& obj, const QString& key, const QColor& defaultColor)
{
    if (!obj.contains(key))
        return defaultColor;
    
    QString colorStr = obj[key].toString();
    if (colorStr.isEmpty())
        return defaultColor;
    
    QColor color(colorStr);
    if (!color.isValid())
    {
        qWarning() << "UStyleManager: Invalid color value for" << key << ":" << colorStr;
        return defaultColor;
    }
    
    return color;
}

double UStyleManager::parseDouble(const QJsonObject& obj, const QString& key, double defaultValue)
{
    if (!obj.contains(key))
        return defaultValue;
    
    return obj[key].toDouble(defaultValue);
}

QVector<QColor> UStyleManager::parseColorArray(const QJsonObject& obj, const QString& key, const QVector<QColor>& defaultColors)
{
    if (!obj.contains(key))
        return defaultColors;
    
    QJsonArray arr = obj[key].toArray();
    if (arr.isEmpty())
        return defaultColors;
    
    QVector<QColor> colors;
    colors.reserve(arr.size());
    
    for (const QJsonValue& val : arr)
    {
        QString colorStr = val.toString();
        QColor color(colorStr);
        if (color.isValid())
        {
            colors.append(color);
        }
        else
        {
            qWarning() << "UStyleManager: Invalid color in array:" << colorStr;
        }
    }
    
    return colors.isEmpty() ? defaultColors : colors;
}

QColor UStyleManager::getChartSeriesColor(int index) const
{
    if (m_chartSeriesColors.isEmpty())
        return Qt::black;
    
    // Циклический доступ к цветам
    return m_chartSeriesColors[index % m_chartSeriesColors.size()];
}

QLinearGradient UStyleManager::getNodeGradient(const QRectF& rect) const
{
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    gradient.setColorAt(0, m_nodeGradientTop);
    gradient.setColorAt(1, m_nodeGradientBottom);
    return gradient;
}

QLinearGradient UStyleManager::getButtonGradient(const QRectF& rect) const
{
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    gradient.setColorAt(0, m_buttonGradientTop);
    gradient.setColorAt(1, m_buttonGradientBottom);
    return gradient;
}

QLinearGradient UStyleManager::getDockTitleGradient(const QRectF& rect) const
{
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    gradient.setColorAt(0, m_dockTitleGradientTop);
    gradient.setColorAt(1, m_dockTitleGradientBottom);
    return gradient;
}

QLinearGradient UStyleManager::getHeaderGradient(const QRectF& rect) const
{
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    gradient.setColorAt(0, m_headerGradientTop);
    gradient.setColorAt(1, m_headerGradientBottom);
    return gradient;
}

QString UStyleManager::getStylesPath() const
{
    if (!m_stylesPath.isEmpty())
        return m_stylesPath;
    
    // Find Styles directory relative to Bin/ (not Bin/Platform/Linux/)
    QString appDir = QCoreApplication::applicationDirPath();
    
    // If we're in Bin/Platform/Linux/, go up to Bin/Styles/
    if (appDir.contains("/Platform/"))
    {
        QDir dir(appDir);
        dir.cdUp(); // Bin/Platform/
        dir.cdUp(); // Bin/
        return dir.absolutePath() + "/Styles/";
    }
    else
    {
        // Fallback: try relative to application directory
        return appDir + "/Styles/";
    }
}

bool UStyleManager::switchTheme(const QString& themeName, QApplication* app)
{
    QString stylesPath = getStylesPath();
    
    QString qssFile, jsonFile;
    
    if (themeName == "dark" || themeName == "Modern Dark")
    {
        qssFile = stylesPath + "dark.qss";
        jsonFile = stylesPath + "dark-theme.json";
    }
    else // default to light
    {
        qssFile = stylesPath + "default.qss";
        jsonFile = stylesPath + "theme.json";
    }
    
    // Reset to defaults first
    setDefaults();
    
    // Load new theme
    bool themeLoaded = loadTheme(jsonFile);
    bool qssLoaded = loadStyleSheet(qssFile);
    
    if (themeLoaded && qssLoaded && app)
    {
        applyGlobalStyleSheet(app);
        m_stylesPath = stylesPath; // Cache the path
        qDebug() << "UStyleManager: Switched to theme:" << m_themeName << "from" << stylesPath;
        emit themeChanged();
        return true;
    }
    
    qWarning() << "UStyleManager: Failed to switch theme to:" << themeName;
    qWarning() << "  QSS file:" << qssFile << (QFile::exists(qssFile) ? "exists" : "NOT FOUND");
    qWarning() << "  JSON file:" << jsonFile << (QFile::exists(jsonFile) ? "exists" : "NOT FOUND");
    return false;
}

QStringList UStyleManager::getAvailableThemes() const
{
    return QStringList() << "Modern Light" << "Modern Dark";
}

