#ifndef UMODERNDIAGRAMTOOLTIPGENERATOR_H
#define UMODERNDIAGRAMTOOLTIPGENERATOR_H

#include <QString>
#include "UModernDiagramPort.h"

// Forward declarations
class UModernDiagramNodeItem;
class UModernDiagramLinkItem;

/// Генератор tooltips для элементов диаграммы
/// Инкапсулирует логику генерации подсказок для узлов, портов, связей и канвы
class UModernDiagramTooltipGenerator
{
public:
    /// Генерирует tooltip для узла
    static QString generateNodeTooltip(const UModernDiagramNodeItem* node);

    /// Генерирует tooltip для порта
    static QString generatePortTooltip(const UModernDiagramPort& port);

    /// Генерирует tooltip для связи
    static QString generateLinkTooltip(const UModernDiagramLinkItem* link);

    /// Генерирует tooltip для канвы
    static QString generateCanvasTooltip();

    /// Получает имя категории порта
    static QString getPortCategoryName(UModernDiagramPortCategory category);
};

#endif // UMODERNDIAGRAMTOOLTIPGENERATOR_H
