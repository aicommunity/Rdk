#ifndef USTYLEMANAGER_H
#define USTYLEMANAGER_H

#include <QObject>
#include <QColor>
#include <QString>
#include <QApplication>
#include <QFont>
#include <QJsonObject>
#include <QVector>
#include <QLinearGradient>
#include <QPointer>

class QWidget;

/**
 * @brief Синглтон для централизованного управления стилями приложения.
 * 
 * Загружает цвета из theme.json для кастомной отрисовки (QPainter)
 * и применяет .qss стили к Qt виджетам.
 */
class UStyleManager : public QObject
{
    Q_OBJECT
public:
    static UStyleManager* instance();
    
    // Загрузка конфигурации
    bool loadTheme(const QString& themeJsonPath);
    bool loadStyleSheet(const QString& qssPath);
    void applyGlobalStyleSheet(QApplication* app);

    /// Sync QApplication font with the desktop UI font and polish dock/MDI titles.
    void applySystemUiFonts(QApplication* app);
    QFont titleBarFont() const;
    void applyTitleBarFont(QWidget* titleHost) const;

    /// Density spacing from current UI font metrics (n * height/4, min 1).
    static int densitySpace(int units);
    
    // Переключение темы
    bool switchTheme(const QString& themeName, QApplication* app);
    QStringList getAvailableThemes() const;
    QString getStylesPath() const;
    
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
    QColor getLinkReverseColor() const { return m_linkReverse; }
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
    
    // Градиенты
    QLinearGradient getNodeGradient(const QRectF& rect) const;
    QLinearGradient getButtonGradient(const QRectF& rect) const;
    QLinearGradient getDockTitleGradient(const QRectF& rect) const;
    QLinearGradient getHeaderGradient(const QRectF& rect) const;
    QColor getNodeGradientTop() const { return m_nodeGradientTop; }
    QColor getNodeGradientBottom() const { return m_nodeGradientBottom; }
    
    // Эффекты теней
    QColor getShadowColor() const { return m_shadowColor; }
    double getShadowBlur() const { return m_shadowBlur; }
    double getShadowOffsetX() const { return m_shadowOffsetX; }
    double getShadowOffsetY() const { return m_shadowOffsetY; }
    
    // Эффекты свечения
    QColor getGlowColor() const { return m_glowColor; }
    double getGlowRadius() const { return m_glowRadius; }
    
    // Цвета статусов
    QColor getSuccessColor() const { return m_successColor; }
    QColor getWarningColor() const { return m_warningColor; }
    QColor getErrorColor() const { return m_errorColor; }
    QColor getInfoColor() const { return m_infoColor; }
    
    // Имя текущей темы
    QString getThemeName() const { return m_themeName; }

signals:
    void themeChanged();
    
private:
    UStyleManager();
    ~UStyleManager() override = default;
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
    QColor m_linkReverse;
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
    
    // Gradient colors
    QColor m_nodeGradientTop;
    QColor m_nodeGradientBottom;
    QColor m_buttonGradientTop;
    QColor m_buttonGradientBottom;
    QColor m_dockTitleGradientTop;
    QColor m_dockTitleGradientBottom;
    QColor m_headerGradientTop;
    QColor m_headerGradientBottom;
    
    // Shadow effects
    QColor m_shadowColor;
    double m_shadowBlur;
    double m_shadowOffsetX;
    double m_shadowOffsetY;
    
    // Glow effects
    QColor m_glowColor;
    double m_glowRadius;
    
    // Status colors
    QColor m_successColor;
    QColor m_warningColor;
    QColor m_errorColor;
    QColor m_infoColor;
    
    // Theme name and paths
    QString m_themeName;
    QString m_stylesPath;

    QPointer<QObject> m_titleFontPolisher;
};

#endif // USTYLEMANAGER_H

