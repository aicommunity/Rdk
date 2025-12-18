#include "UStyleManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

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
    
    qDebug() << "UStyleManager: Theme loaded from" << themeJsonPath;
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
}

QString UStyleManager::getTreeWidgetStyleSheet() const
{
    // Stylesheet for QTreeWidget used in port list popup
    return QString(
        "QTreeWidget {"
        "  font-size: 10pt;"
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

