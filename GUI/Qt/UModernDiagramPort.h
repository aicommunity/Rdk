#ifndef UMODERNDIAGRAMPORT_H
#define UMODERNDIAGRAMPORT_H

#include <QPointF>
#include <QString>

/// Категория порта для категоризации выходных портов
enum class UModernDiagramPortCategory {
    Own,        // Собственные свойства компонента
    Child,      // Свойства дочерних компонентов
    Alias       // Алиасы свойств
};

/// Структура, представляющая порт узла диаграммы
struct UModernDiagramPort
{
    QPointF pos;
    bool isInput;
    QString name;           // Имя порта (например, "Output", "ChannelInputs")
    QString fullPath;       // Полный путь для вложенных портов (например, "SubComp.Output")
    QString componentName;  // Имя компонента-владельца
    QString displayName;    // Отображаемое имя (для tooltip)
    UModernDiagramPortCategory category;  // Категория порта (для выходных портов)

    UModernDiagramPort() : isInput(false), category(UModernDiagramPortCategory::Own) {}
    UModernDiagramPort(const QPointF& p, bool input, const QString& n)
        : pos(p), isInput(input), name(n), componentName(n), displayName(n), category(UModernDiagramPortCategory::Own) {}
};

// Typedef для обратной совместимости
typedef UModernDiagramPortCategory PortCategory;
typedef UModernDiagramPort Port;

#endif // UMODERNDIAGRAMPORT_H

