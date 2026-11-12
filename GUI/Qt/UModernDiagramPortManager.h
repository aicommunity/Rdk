#ifndef UMODERNDIAGRAMPORTMANAGER_H
#define UMODERNDIAGRAMPORTMANAGER_H

#include <QVector>
#include <QString>
#include "UModernDiagramPort.h"

// Forward declarations
class UModernDiagramNodeItem;
class UModernDiagramWidget;

/// @brief Менеджер для работы с портами компонентов диаграммы.
/// Отвечает за загрузку портов из ядра, определение категории порта и кэширование.
class UModernDiagramPortManager
{
public:
    /// @brief Загружает собственные выходные порты компонента.
    /// @param fullName Полное имя компонента (например, "Root.Comp1")
    /// @param nodeName Короткое имя узла (например, "Comp1")
    /// @return Вектор портов категории Own
    static QVector<UModernDiagramPort> loadOwnOutputPorts(const QString& fullName, const QString& nodeName);

    /// @brief Загружает выходные порты дочерних компонентов.
    /// @param fullName Полное имя компонента
    /// @param nodeName Короткое имя узла
    /// @return Вектор портов категории Child
    static QVector<UModernDiagramPort> loadChildOutputPorts(const QString& fullName, const QString& nodeName);

    /// @brief Загружает выходные порты алиасов.
    /// @param fullName Полное имя компонента
    /// @param nodeName Короткое имя узла
    /// @return Вектор портов категории Alias
    static QVector<UModernDiagramPort> loadAliasOutputPorts(const QString& fullName, const QString& nodeName);

    /// @brief Загружает собственные входные порты компонента.
    /// @param fullName Полное имя компонента
    /// @param nodeName Короткое имя узла
    /// @return Вектор портов категории Own
    static QVector<UModernDiagramPort> loadOwnInputPorts(const QString& fullName, const QString& nodeName);

    /// @brief Загружает входные порты дочерних компонентов.
    /// @param fullName Полное имя компонента
    /// @param nodeName Короткое имя узла
    /// @return Вектор портов категории Child
    static QVector<UModernDiagramPort> loadChildInputPorts(const QString& fullName, const QString& nodeName);

    /// @brief Загружает входные порты алиасов.
    /// @param fullName Полное имя компонента
    /// @param nodeName Короткое имя узла
    /// @return Вектор портов категории Alias
    static QVector<UModernDiagramPort> loadAliasInputPorts(const QString& fullName, const QString& nodeName);

    /// @brief Загружает входные порты дочерних компонентов рекурсивно на всю глубину.
    /// @param fullName Полное имя компонента
    /// @param nodeName Короткое имя узла
    /// @param prefixPath Префикс пути для отображения (используется при рекурсии)
    /// @return Вектор портов категории Child со всех уровней вложенности
    static QVector<UModernDiagramPort> loadChildInputPortsRecursive(const QString& fullName, const QString& nodeName, const QString& prefixPath = QString());

    /// @brief Загружает выходные порты дочерних компонентов рекурсивно на всю глубину.
    /// @param fullName Полное имя компонента
    /// @param nodeName Короткое имя узла
    /// @param prefixPath Префикс пути для отображения (используется при рекурсии)
    /// @return Вектор портов категории Child со всех уровней вложенности
    static QVector<UModernDiagramPort> loadChildOutputPortsRecursive(const QString& fullName, const QString& nodeName, const QString& prefixPath = QString());

    /// @brief Загружает вложенные порты компонента.
    /// @param fullName Полное имя компонента
    /// @param nodeName Короткое имя узла
    /// @param isInput Если true, загружает входные порты, иначе выходные
    /// @param includeNested Если true, включает вложенные порты
    /// @return Вектор портов
    static QVector<UModernDiagramPort> loadNestedPorts(const QString& fullName, const QString& nodeName, bool isInput, bool includeNested);

    /// @brief Определяет категорию порта по имени свойства.
    /// @param fullName Полное имя компонента
    /// @param nodeName Короткое имя узла
    /// @param propertyName Имя свойства (может быть полным путем)
    /// @param isInput Если true, проверяет входные порты, иначе выходные
    /// @param ownPorts Собственные порты (для проверки)
    /// @param childPorts Порты дочерних компонентов (для проверки)
    /// @param aliasPorts Порты алиасов (для проверки)
    /// @return Категория порта
    static UModernDiagramPortCategory determinePortCategory(
        const QString& fullName,
        const QString& nodeName,
        const QString& propertyName,
        bool isInput,
        const QVector<UModernDiagramPort>& ownPorts,
        const QVector<UModernDiagramPort>& childPorts,
        const QVector<UModernDiagramPort>& aliasPorts);

private:
    // Вспомогательные методы
    static QVector<UModernDiagramPort> loadPortsFromProperties(
        const QString& fullName,
        const QString& nodeName,
        unsigned int propertyTypeMask,
        bool isInput,
        UModernDiagramPortCategory category,
        bool filterOwn = false);

    static QVector<UModernDiagramPort> loadAliasPortsFromNet(
        const QString& fullName,
        const QString& nodeName,
        unsigned int propertyTypeMask,
        bool isInput);
};

#endif // UMODERNDIAGRAMPORTMANAGER_H

