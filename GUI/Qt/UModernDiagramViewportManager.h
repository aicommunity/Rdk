#ifndef UMODERNDIAGRAMVIEWPORTMANAGER_H
#define UMODERNDIAGRAMVIEWPORTMANAGER_H

#include <QHash>
#include <QString>
#include <QPointF>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>

// Forward declarations
class UModernDiagramWidget;

/// Состояние viewport для компонента
struct UModernDiagramViewState {
    double scale = 1.0;
    QPointF center;
    bool isValid = false;
};

/// Менеджер управления viewport для диаграммы
/// Управляет сохранением и восстановлением состояния viewport для каждого компонента
class UModernDiagramViewportManager
{
public:
    explicit UModernDiagramViewportManager(UModernDiagramWidget* owner);
    ~UModernDiagramViewportManager() = default;

    /// Сохраняет текущее состояние viewport для указанного компонента
    void saveCurrentViewState(const QString& componentName);

    /// Восстанавливает состояние viewport для указанного компонента
    void restoreViewState(const QString& componentName);

    /// Сбрасывает масштаб к начальному значению
    void resetZoom();

    /// Сохраняет состояние viewport в QSettings
    void saveToSettings();

    /// Загружает состояние viewport из QSettings
    void loadFromSettings();

    /// Создает и настраивает кнопку «Описание проекта»
    QPushButton* createProjectDescriptionButton(QWidget* parent);

    /// Создает и настраивает кнопку сброса масштаба
    QPushButton* createResetZoomButton(QWidget* parent);

    /// Создает кнопку показа/скрытия палитры классов
    QPushButton* createClassesListButton(QWidget* parent);

    /// Обновляет позиции кнопок поверх диаграммы (Reset Zoom и Project Description)
    void updateOverlayButtonsPosition(int width, int height);

    /// Обновляет стиль кнопки сброса масштаба (устаревшее имя, вызывает updateOverlayButtonsStyle)
    void updateResetZoomButtonPosition(int width, int height);

    /// Обновляет стили кнопок поверх диаграммы
    void updateOverlayButtonsStyle();

    /// Обновляет стиль кнопки сброса масштаба (устаревшее имя, вызывает updateOverlayButtonsStyle)
    void updateResetZoomButtonStyle();

    /// Подсветить кнопку палитры классов (открыта / закрыта)
    void setClassesListButtonChecked(bool checked);

private:
    UModernDiagramWidget* m_owner;
    QHash<QString, UModernDiagramViewState> m_viewStates;  // Состояние viewport для каждого компонента
    QPushButton* m_projectDescriptionButton;
    QPushButton* m_resetZoomButton;
    QPushButton* m_classesListButton;

    static constexpr double DEFAULT_SCALE = 1.0;  // Начальный масштаб по умолчанию
};

#endif // UMODERNDIAGRAMVIEWPORTMANAGER_H

