#ifndef UMODERNDIAGRAMWIDGET_H
#define UMODERNDIAGRAMWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QPointer>
#include <QPoint>
#include <QTreeWidget>
#include <QHash>
#include <QMap>
#include <QSet>
#include <QPushButton>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QCryptographicHash>
#include <rdk_init.h>
#include <rdk_application.h>
#include "../Core/Engine/UXMLEnvSerialize.h"
#include "../Core/Engine/UEnvSupport.h"
#include "UModernDiagramPort.h"  // Для Port и PortCategory
#include "UComponentGuiContext.h"

// Константы для диаграммы
namespace UModernDiagramConstants {
    // Размеры и отступы
    constexpr int MINIMAP_HEIGHT = 140;                    // Высота миникарты
    constexpr double MINIMAP_SCALE = 0.2;                  // Масштаб миникарты
    constexpr int SCENE_RECT_PADDING = 200;                // Отступ для sceneRect

    // Сетка размещения компонентов
    constexpr int GRID_COLUMNS = 4;                        // Количество колонок в сетке
    constexpr int GRID_CELL_WIDTH = 180;                   // Ширина ячейки сетки
    constexpr int GRID_CELL_HEIGHT = 140;                  // Высота ячейки сетки

    // Размеры виджета списка портов
    constexpr int PORT_LIST_MAX_HEIGHT = 300;              // Максимальная высота списка портов
    constexpr int PORT_LIST_MIN_WIDTH = 250;               // Минимальная ширина списка портов
    constexpr int PORT_LIST_MAX_WIDTH = 350;               // Максимальная ширина списка портов

    // Таймеры
    constexpr int PORT_LIST_HIDE_DELAY_MS = 1000;          // Задержка скрытия списка портов (мс)
    constexpr int PORT_LIST_HIDE_RETRY_DELAY_MS = 300;     // Повторная задержка скрытия списка портов (мс)
}

// Forward declarations
class UModernDiagramWidgetMovementTest;
class UModernDiagramLinkItem;
class UModernDiagramNodeItem;
class UModernDiagramViewportManager;
class UModernDiagramCoordinateManager;
class UModernDiagramCacheManager;
class UModernDiagramContextMenu;
// Forward declarations for extracted classes
class UModernDiagramScene;
class UModernDiagramView;

/// Простой современный виджет диаграммы на основе QGraphicsView/QGraphicsScene.
/// Отображает компоненты как узлы с портами, поддерживает drag&drop связей и миникарту.
class UModernDiagramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UModernDiagramWidget(QWidget *parent = nullptr);
    ~UModernDiagramWidget() override;

    /// Устанавливает приложение и имя компонента (модели) для отображения
    void SetApplication(RDK::UApplication* app);
    void SetComponentName(const QString& name);

public slots:
    /// Перезагрузка диаграммы из текущей модели
    void Reload();
    /// Установить масштаб по содержимому
    void FitToView();
    /// Обработка двойного клика из списка компонентов
    void componentDoubleClick(QString name);
    /// Обработка одиночного клика из списка компонентов
    void componentSingleClick(QString name);
    /// Обновление схемы
    void updateScheme(bool reloadXml);
    /// Выбор компонента по имени
    void selectComponent(QString name);
    /// Сохранение состояния viewport в QSettings
    void SaveViewState();
    /// Загрузка состояния viewport из QSettings
    void LoadViewState();
    /// Выделение всех NodeItem внутри указанного прямоугольника
    /// @param selectionRect Прямоугольник выделения в координатах scene
    /// @param addToSelection Если true, добавляет к текущему выделению, иначе очищает перед выделением
    /// @return Количество выделенных объектов
    int selectNodesInRect(const QRectF& selectionRect, bool addToSelection = false);
    /// Обновление темы - инвалидирует кэш всех узлов и обновляет сцену
    void updateTheme();
    /// Вызвать открытие окна описания проекта (используется кнопкой на диаграмме)
    void requestOpenProjectDescription();
    /// Обновление стилей кнопки сброса масштаба в соответствии с текущей темой
    // updateResetZoomButtonStyle теперь в UModernDiagramViewportManager

signals:
    /// Компонент выбран (одиночный клик)
    void componentSelected(QString name);
    /// Двойной клик по компоненту (сигнал для внешних обработчиков)
    void componentDoubleClicked(QString name);
    /// Выход на уровень выше
    void componentStapBack();
    /// Обновление списка компонентов
    void updateComponentsList();
    /// Просмотр/разрыв связи
    void viewLinks(QString componentName);
    /// Создание связи между двумя компонентами
    void createLinks(QString firstComponentName, QString secondComponentName);
    /// Переключение связи между двумя компонентами
    void switchLinks(QString firstComponentName, QString secondComponentName);
    /// Запрос на открытие окна описания проекта (кнопка на диаграмме)
    void openProjectDescriptionRequested();
    /// Запрос на открытие специализированной GUI-формы компонента.
    void openComponentGuiRequested(const UComponentGuiContext& context);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    friend class UModernDiagramScene;
    friend class UModernDiagramView;
    friend class UModernDiagramLinkItem;  // Для доступа к NodeItem и PortCategory
    friend class UModernDiagramNodeItem;  // Для доступа к Port и PortCategory
    friend class UModernDiagramCacheManager;  // Для доступа к данным кэша
    friend class UModernDiagramCoordinateManager;  // Для доступа к данным координат
    friend class UModernDiagramViewportManager;  // Для доступа к m_mainView и m_scene
    friend class UModernDiagramContextMenu;  // Для доступа к данным виджета
    // Friend class for unit tests
    friend class UModernDiagramWidgetMovementTest;

    // Port и PortCategory теперь выделены в отдельный файл UModernDiagramPort.h
    // Используем typedef для обратной совместимости
    using Port = UModernDiagramPort;
    using PortCategory = UModernDiagramPortCategory;

    // Forward declarations для извлеченных классов
    class LinkItem;  // Теперь это UModernDiagramLinkItem
    class NodeItem;  // Теперь это UModernDiagramNodeItem

    // NodeItem теперь выделен в отдельный класс UModernDiagramNodeItem
    // Forward declaration остается для обратной совместимости

    // LinkItem теперь выделен в отдельный класс UModernDiagramLinkItem
    // Forward declaration остается для обратной совместимости

    void buildScene();
    void clearScene();
    void layoutGrid();

private:
    // Вспомогательные методы для buildScene()
    /// Загружает список компонентов из ядра
    QStringList loadComponentList() const;
    /// Загружает и кэширует координаты всех компонентов, возвращает минимальную позицию сцены
    /// @param components Список компонентов для загрузки
    /// @param coordsLoaded Выходной параметр: true, если хотя бы одна координата загружена
    /// @param coordCache Выходной параметр: кэш координат компонентов
    /// @param coordLoadedCache Выходной параметр: кэш флагов загрузки координат
    /// @return Минимальная позиция сцены для нормализации
    QPointF loadAndCacheCoordinates(const QStringList& components, bool& coordsLoaded,
                                     QHash<QString, QPointF>& coordCache, QHash<QString, bool>& coordLoadedCache);
    /// Создает узлы для всех компонентов
    void createNodes(const QStringList& components, const QHash<QString, QPointF>& coordCache,
                     const QHash<QString, bool>& coordLoadedCache, const QPointF& minScenePos,
                     QList<UModernDiagramNodeItem*>& nodesToAdd, QHash<QString, QPointF>& nodePositions);
    /// Добавляет узлы в сцену и устанавливает их позиции
    void addNodesToScene(const QList<UModernDiagramNodeItem*>& nodesToAdd, const QHash<QString, QPointF>& nodePositions);
    /// Обновляет sceneRect на основе текущих границ всех элементов сцены
    /// Вызывается после завершения движения компонентов для предотвращения прыжков
    void updateSceneRect();
    /// Добавляет один компонент в сцену без полного перестроения
    /// Использует текущий m_normalizationOffset, чтобы не сдвигать существующие компоненты
    UModernDiagramNodeItem* addSingleComponent(const QString& fullName);
    UModernDiagramNodeItem* pickPort(const QPointF& scenePos, bool requireInput, QPointF& portPos);
    const Port* pickPortDetailed(const QPointF& scenePos, bool requireInput, UModernDiagramNodeItem*& node, QPointF& portPos);
    UModernDiagramNodeItem* pickNode(const QPointF& scenePos) const;
    /// Проверяет, выходит ли компонент за видимую область viewport
    /// Возвращает true, если компонент находится вне видимой области (с учетом масштаба)
    bool isComponentOutsideVisibleArea(UModernDiagramNodeItem* node) const;
    void buildLinks();
    void rebuildLinks(); // Перестраивает только связи без перезагрузки всей сцены
    // Cache management methods теперь в UModernDiagramCacheManager
    // Используйте m_cacheManager для доступа к этим методам
    const Port* m_dragSourcePort;
    QPointF   m_dragSourcePortPos;

    // Active connection state (for tree widget selection)
    UModernDiagramNodeItem* m_activeSourceNode;
    const Port* m_activeSourcePort;  // Указатель для быстрого доступа (может стать невалидным)
    // Копии данных порта для безопасного использования (избегаем проблем с невалидными указателями)
    QString m_activeSourcePortName;
    QString m_activeSourcePortFullPath;
    QString m_activeSourcePortComponentName;
    QPointF m_activeSourcePortPos;
    UModernDiagramLinkItem* m_activeTempLink;
    bool m_isLineFrozen;  // Флаг, указывающий, что соединение "заморожено" на порту
    QPointF m_frozenTargetPortPos;  // Позиция порта, к которому "прилипло" соединение
    bool m_isWaitingForPortSelection;  // Флаг, указывающий, что ожидается выбор порта из окна выбора

    // Scene and view
    UModernDiagramScene* m_scene;
    UModernDiagramView* m_mainView;
    QGraphicsView* m_miniMap;

    // Temporary link during drag
    UModernDiagramLinkItem* m_tempLink;
    UModernDiagramNodeItem* m_dragSourceNode;

    // Viewport manager
    UModernDiagramViewportManager* m_viewportManager;

    // Coordinate manager
    UModernDiagramCoordinateManager* m_coordinateManager;

    // Data
    RDK::UApplication* m_application;
    QString m_componentName;
    QList<UModernDiagramNodeItem*> m_nodes;
    QHash<QString, UModernDiagramNodeItem*> m_nodeByName;
    QList<UModernDiagramLinkItem*> m_links;

    // Защита от бесконечной рекурсии при выборе компонента
    // Cache manager
    UModernDiagramCacheManager* m_cacheManager;

    // Context menu manager
    UModernDiagramContextMenu* m_contextMenuManager;

    int m_selectComponentRetryCount;

    // Для перемещения группы объектов - храним предыдущие позиции
    QHash<UModernDiagramNodeItem*, QPointF> m_lastNodePositions;

    // Coord scaling и normalization offset теперь в UModernDiagramCoordinateManager

    // Viewport state management теперь в UModernDiagramViewportManager

    // Кэш загруженных уровней и компонентов теперь в UModernDiagramCacheManager
    bool m_isMovingGroup = false;
    bool m_isBatchSelecting = false;  // Флаг для отслеживания выделения прямоугольником

    // Флаг для отслеживания движения компонентов (для отложенного обновления sceneRect)
    // Устанавливается в mousePressEvent, сбрасывается в mouseReleaseEvent
    bool m_isComponentMoving = false;

    // Флаг для предотвращения сохранения координат во время инициализации сцены
    bool m_isBuildingScene = false;

    // Флаг для предотвращения эмиссии сигнала componentSelected при программном выделении
    mutable bool m_isProgrammaticSelection = false;

    // Флаг для предотвращения рекурсивных вызовов itemChange при сохранении координат
    mutable bool m_isSavingCoordinates = false;

    // Флаг для предотвращения повторных вызовов updateNormalizationOffsetForMovement во время обновления позиций
    mutable bool m_isUpdatingNormalizationOffset = false;

    // Компоненты с отрицательными позициями во время движения (для обновления offset при завершении движения)
    QSet<UModernDiagramNodeItem*> m_componentsWithNegativePos;

    // Исходные абсолютные координаты компонентов ДО перемещения в отрицательную область
    // Ключ: UModernDiagramNodeItem*, значение: исходные абсолютные координаты (normalizedPos + offset)
    QMap<UModernDiagramNodeItem*, QPointF> m_originalAbsolutePositions;

    // Кнопка сброса масштаба теперь в UModernDiagramViewportManager

    // Context menu теперь в UModernDiagramContextMenu

    /// Удаляет указанные компоненты с запросом подтверждения
    /// @param nodesToDelete Список NodeItem для удаления
    void deleteComponents(const QList<UModernDiagramNodeItem*>& nodesToDelete);

    // Public methods for context menu manager to emit signals
    void emitViewLinks(const QString& componentName);
    void emitCreateLinks(const QString& firstComponentName, const QString& secondComponentName);
    void emitSwitchLinks(const QString& firstComponentName, const QString& secondComponentName);
    void emitOpenComponentGui(const UComponentGuiContext& context);

    // Context menu slots
private slots:
    // Context menu slots теперь в UModernDiagramContextMenu
    // onResetZoomClicked теперь в UModernDiagramViewportManager

private:
    // Coordinate management methods теперь в UModernDiagramCoordinateManager
    // (делегируются в менеджер для обратной совместимости API)
    QPointF scenePosFromKernel(const QPointF& kernel) const;
    QPointF kernelPosFromScene(const QPointF& scene) const;
    bool loadCoord(const QString& fullName, QPointF& outPos) const;
    void saveCoord(const QString& fullName, const QPointF& scenePos) const;
    QPointF currentMinScenePos() const;
    void recalculateNormalizationOffset(const QPointF& pendingComponentPos = QPointF(), const QString& pendingComponentName = QString());
    void updateNormalizationOffsetForMovement(const QPointF& newMinNormalizedPos, const QSet<UModernDiagramNodeItem*>& componentsToAdjust = QSet<UModernDiagramNodeItem*>());

    // Viewport state management methods теперь в UModernDiagramViewportManager

    // Tooltip generation methods теперь в UModernDiagramTooltipGenerator

public:
    // Test accessors for unit tests (always available for testing)
    // Implementation in .cpp file to avoid issues with incomplete type NodeItem
    // These methods are placed at the end of the class after NodeItem is fully defined
    QPointF testGetNormalizationOffset() const;
    const QSet<UModernDiagramNodeItem*>& testGetComponentsWithNegativePos() const;
};

// Forward declarations для извлеченных классов (включаем только в .cpp для избежания циклических зависимостей)
class UModernDiagramLinkItem;
class UModernDiagramNodeItem;

// Typedef для обратной совместимости
typedef UModernDiagramLinkItem LinkItem;
typedef UModernDiagramNodeItem NodeItem;

#endif // UMODERNDIAGRAMWIDGET_H

