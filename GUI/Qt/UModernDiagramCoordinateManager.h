#ifndef UMODERNDIAGRAMCOORDINATEMANAGER_H
#define UMODERNDIAGRAMCOORDINATEMANAGER_H

#include <QPointF>
#include <QString>
#include <QSet>

// Forward declarations
class UModernDiagramWidget;
class UModernDiagramNodeItem;

/// Менеджер управления координатами для диаграммы
/// Управляет преобразованием координат между kernel и scene, нормализацией и сохранением/загрузкой координат
class UModernDiagramCoordinateManager
{
public:
    explicit UModernDiagramCoordinateManager(UModernDiagramWidget* owner);
    ~UModernDiagramCoordinateManager() = default;

    /// Преобразует kernel координаты в scene координаты
    QPointF scenePosFromKernel(const QPointF& kernel) const;

    /// Преобразует scene координаты в kernel координаты
    QPointF kernelPosFromScene(const QPointF& scene) const;

    /// Загружает координаты компонента из ядра
    bool loadCoord(const QString& fullName, QPointF& outPos) const;

    /// Сохраняет координаты компонента в ядро
    void saveCoord(const QString& fullName, const QPointF& scenePos) const;

    /// Вычисляет минимальную позицию всех узлов на сцене
    QPointF currentMinScenePos() const;

    /// Пересчитывает m_normalizationOffset на основе текущих kernel координат всех компонентов на сцене
    /// @param pendingComponentPos Опциональная позиция компонента, который еще не сохранен (в абсолютных scene координатах)
    /// @param pendingComponentName Опциональное имя компонента, который еще не сохранен
    void recalculateNormalizationOffset(const QPointF& pendingComponentPos = QPointF(), const QString& pendingComponentName = QString());

    /// Обновляет m_normalizationOffset во время перемещения компонента, если он стал новым минимумом
    /// @param newMinNormalizedPos Новая минимальная нормализованная позиция компонента
    /// @param componentsToAdjust Множество компонентов для корректировки позиций
    void updateNormalizationOffsetForMovement(const QPointF& newMinNormalizedPos, const QSet<UModernDiagramNodeItem*>& componentsToAdjust = QSet<UModernDiagramNodeItem*>());

    // Геттеры и сеттеры для данных координат
    double getCoordScale() const { return m_coordScale; }
    void setCoordScale(double scale) { m_coordScale = scale; }

    QPointF getNormalizationOffset() const { return m_normalizationOffset; }
    void setNormalizationOffset(const QPointF& offset) { m_normalizationOffset = offset; }

private:
    UModernDiagramWidget* m_owner;

    // Coord scaling (scene units per kernel unit)
    // Set to DEFAULT_COORD_SCALE to match UDrawEngine's ZoomCoeff for 1:1 scale with classic diagram
    static constexpr double DEFAULT_COORD_SCALE = 30.0;
    double m_coordScale = DEFAULT_COORD_SCALE;

    // Минимальная позиция, использованная для нормализации при загрузке
    // Нужна для правильной денормализации координат при сохранении
    QPointF m_normalizationOffset;
};

#endif // UMODERNDIAGRAMCOORDINATEMANAGER_H

