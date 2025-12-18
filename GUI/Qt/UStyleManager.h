#ifndef USTYLEMANAGER_H
#define USTYLEMANAGER_H

#include <QColor>
#include <QString>
#include <QApplication>
#include <QJsonObject>
#include <QVector>

/**
 * @brief Синглтон для централизованного управления стилями приложения.
 * 
 * Загружает цвета из theme.json для кастомной отрисовки (QPainter)
 * и применяет .qss стили к Qt виджетам.
 */
class UStyleManager
{
public:
    static UStyleManager* instance();
    
    // Загрузка конфигурации
    bool loadTheme(const QString& themeJsonPath);
    bool loadStyleSheet(const QString& qssPath);
    void applyGlobalStyleSheet(QApplication* app);
    
    // Получение stylesheet для виджетов
    QString getStyleSheet() const { return m_styleSheet; }
    QString getTreeWidgetStyleSheet() const;
    
    // Цвета узлов (NodeItem)
    QColor getNodeFillColor() const { return m_nodeFill; }
    QColor getNodeFillSelectedColor() const { return m_nodeFillSelected; }
    QColor getNodeBorderColor() const { return m_nodeBorder; }
    double getNodeBorderWidth() const { return m_nodeBorderWidth; }
    double getNodeCornerRadius() const { return m_nodeCornerRadius; }
    
    // Цвета портов
    QColor getPortInputColor() const { return m_portInputNormal; }
    QColor getPortInputHoverColor() const { return m_portInputHover; }
    QColor getPortOutputColor() const { return m_portOutputNormal; }
    QColor getPortOutputHoverColor() const { return m_portOutputHover; }
    double getPortRadius() const { return m_portRadius; }
    double getPortHoverRadius() const { return m_portHoverRadius; }
    
    // Цвета связей (LinkItem)
    QColor getLinkColor() const { return m_linkSolid; }
    QColor getLinkTempColor() const { return m_linkDashed; }
    double getLinkWidth() const { return m_linkWidth; }
    
    // Цвета для старого виджета диаграмм (UDrawEngineImageWidget)
    QColor getDragLineColor() const { return m_dragLineColor; }
    QColor getPortHighlightColor() const { return m_portHighlightColor; }
    
    // Текстовые цвета
    QColor getTextColor() const { return m_textColor; }
    QColor getTextSecondaryColor() const { return m_textSecondary; }
    
    // Общие цвета фона
    QColor getBackgroundColor() const { return m_backgroundColor; }
    QColor getBackgroundAltColor() const { return m_backgroundAlt; }
    QColor getAccentColor() const { return m_accentColor; }
    QColor getSelectionColor() const { return m_selectionColor; }
    
    // Цвета графиков (QChart)
    QColor getChartBackgroundColor() const { return m_chartBackground; }
    QColor getChartAxisColor() const { return m_chartAxis; }
    QColor getChartGridColor() const { return m_chartGrid; }
    QColor getChartTitleColor() const { return m_chartTitle; }
    QColor getChartLabelColor() const { return m_chartLabel; }
    const QVector<QColor>& getChartSeriesColors() const { return m_chartSeriesColors; }
    QColor getChartSeriesColor(int index) const;
    int getChartSeriesColorCount() const { return m_chartSeriesColors.size(); }
    
    // Цвет для отключенных/неактивных элементов
    QColor getDisabledTextColor() const { return m_disabledText; }
    
private:
    UStyleManager();
    ~UStyleManager() = default;
    UStyleManager(const UStyleManager&) = delete;
    UStyleManager& operator=(const UStyleManager&) = delete;
    
    void setDefaults();
    QColor parseColor(const QJsonObject& obj, const QString& key, const QColor& defaultColor);
    double parseDouble(const QJsonObject& obj, const QString& key, double defaultValue);
    QVector<QColor> parseColorArray(const QJsonObject& obj, const QString& key, const QVector<QColor>& defaultColors);
    
    static UStyleManager* s_instance;
    
    // Stylesheet
    QString m_styleSheet;
    
    // Node colors
    QColor m_nodeFill;
    QColor m_nodeFillSelected;
    QColor m_nodeBorder;
    double m_nodeBorderWidth;
    double m_nodeCornerRadius;
    
    // Port colors
    QColor m_portInputNormal;
    QColor m_portInputHover;
    QColor m_portOutputNormal;
    QColor m_portOutputHover;
    double m_portRadius;
    double m_portHoverRadius;
    
    // Link colors
    QColor m_linkSolid;
    QColor m_linkDashed;
    double m_linkWidth;
    
    // Drag colors
    QColor m_dragLineColor;
    QColor m_portHighlightColor;
    
    // Text colors
    QColor m_textColor;
    QColor m_textSecondary;
    
    // General colors
    QColor m_backgroundColor;
    QColor m_backgroundAlt;
    QColor m_accentColor;
    QColor m_selectionColor;
    
    // Chart colors
    QColor m_chartBackground;
    QColor m_chartAxis;
    QColor m_chartGrid;
    QColor m_chartTitle;
    QColor m_chartLabel;
    QVector<QColor> m_chartSeriesColors;
    
    // Disabled colors
    QColor m_disabledText;
};

#endif // USTYLEMANAGER_H

