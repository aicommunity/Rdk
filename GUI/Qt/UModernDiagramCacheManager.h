#ifndef UMODERNDIAGRAMCACHEMANAGER_H
#define UMODERNDIAGRAMCACHEMANAGER_H

#include <QHash>
#include <QString>
#include <QStringList>
#include <QPointF>
#include <QVector>
#include <QPair>
#include "UModernDiagramPort.h"

// Forward declarations
class UModernDiagramWidget;
class UModernDiagramNodeItem;
class UModernDiagramLinkItem;

/// Кэш загруженных уровней для быстрого повторного перехода
struct UModernDiagramSceneCache {
    QList<UModernDiagramNodeItem*> nodes;
    QList<UModernDiagramLinkItem*> links;
    QHash<QString, UModernDiagramNodeItem*> nodeByName;
    QHash<UModernDiagramNodeItem*, QPointF> lastNodePositions;
    QPointF normalizationOffset;
    QStringList componentNames;  // Список компонентов для проверки изменений структуры
    bool isValid = false;
};

/// Кэш информации о компонентах для ускорения отрисовки
struct UModernDiagramComponentCacheEntry {
    // Кэш портов (для NodeItem::paint и determinePortCategory)
    QVector<UModernDiagramPort> ownInputPorts;
    QVector<UModernDiagramPort> childInputPorts;
    QVector<UModernDiagramPort> aliasInputPorts;
    QVector<UModernDiagramPort> ownOutputPorts;
    QVector<UModernDiagramPort> childOutputPorts;
    QVector<UModernDiagramPort> aliasOutputPorts;
    QHash<QPair<QString, bool>, UModernDiagramPortCategory> portCategoryCache;  // Кэш для determinePortCategory: (propertyName, isInput) -> PortCategory

    // Данные, используемые в buildScene (для ускорения Reload)
    QString className;      // Имя класса компонента
    QPointF kernelPos;      // Координаты компонента в ядре
    bool hasKernelPos = false; // Флаг, что kernelPos загружен и валиден

    qint64 timestamp;  // Временная метка последнего обновления
    QString hash;      // Хеш содержимого компонента для инвалидации

    UModernDiagramComponentCacheEntry() : timestamp(0) {}
};

/// Класс для управления кэшем компонентов
class UModernDiagramComponentCache {
public:
    UModernDiagramComponentCache() {}

    // Получить запись кэша для компонента
    UModernDiagramComponentCacheEntry* getEntry(const QString& componentFullName);

    // Создать или обновить запись кэша
    void setEntry(const QString& componentFullName, const UModernDiagramComponentCacheEntry& entry);

    // Проверить, есть ли запись в кэше
    bool hasEntry(const QString& componentFullName) const;

    // Инвалидировать запись кэша
    void invalidateEntry(const QString& componentFullName);

    // Очистить весь кэш
    void clear();

    // Получить все записи кэша (для сохранения в файл)
    const QHash<QString, UModernDiagramComponentCacheEntry>& getAllEntries() const { return m_cache; }

    // Установить все записи кэша (для загрузки из файла)
    void setAllEntries(const QHash<QString, UModernDiagramComponentCacheEntry>& entries) { m_cache = entries; }

private:
    QHash<QString, UModernDiagramComponentCacheEntry> m_cache;
};

/// Менеджер управления кэшем для диаграммы
/// Управляет кэшем уровней (SceneCache) и кэшем компонентов (ComponentCache)
class UModernDiagramCacheManager
{
public:
    explicit UModernDiagramCacheManager(UModernDiagramWidget* owner);
    ~UModernDiagramCacheManager() = default;

    // Методы для работы с кэшем уровней
    void saveSceneToCache(const QString& componentName);
    void restoreSceneFromCache(const QString& componentName);
    void invalidateLevelCache(const QString& componentName = QString());
    bool hasLevelCache(const QString& componentName) const;
    const UModernDiagramSceneCache& getLevelCache(const QString& componentName) const;

    // Методы для работы с кэшем компонентов
    QString computeComponentHash(const QString& componentFullName) const;
    void invalidateComponentCache(const QString& componentFullName = QString());
    void clearComponentCache();
    QString getCacheFilePath(const QString& extension) const;
    bool saveComponentCacheToFile(const QString& filePath, bool useBinary = false) const;
    bool loadComponentCacheFromFile(const QString& filePath, bool useBinary = false);
    void scheduleCacheSave();

    // Доступ к кэшу компонентов (для использования в buildScene и других местах)
    UModernDiagramComponentCache& getComponentCache() { return m_componentCache; }
    const UModernDiagramComponentCache& getComponentCache() const { return m_componentCache; }

private:
    UModernDiagramWidget* m_owner;
    QHash<QString, UModernDiagramSceneCache> m_levelCache;  // Кэш уровней по имени компонента
    mutable UModernDiagramComponentCache m_componentCache;  // Сессионный кэш компонентов (mutable для использования в const методах)
};

#endif // UMODERNDIAGRAMCACHEMANAGER_H

