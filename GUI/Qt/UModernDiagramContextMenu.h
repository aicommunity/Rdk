#ifndef UMODERNDIAGRAMCONTEXTMENU_H
#define UMODERNDIAGRAMCONTEXTMENU_H

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QString>
#include "UComponentGuiContext.h"

// Forward declarations
class UModernDiagramWidget;
class UModernDiagramNodeItem;

/// @brief Менеджер контекстного меню для UModernDiagramWidget.
/// Отвечает за создание и управление контекстным меню компонентов диаграммы.
class UModernDiagramContextMenu : public QObject
{
    Q_OBJECT
public:
    explicit UModernDiagramContextMenu(UModernDiagramWidget* owner, QObject* parent = nullptr);
    ~UModernDiagramContextMenu() override;

    /// @brief Создает и настраивает контекстное меню.
    void createContextMenu();

    /// @brief Возвращает указатель на меню.
    QMenu* menu() const { return m_contextMenu; }

    /// @brief Показывает меню в указанной позиции.
    void showMenu(const QPoint& globalPos, UModernDiagramNodeItem* node);

    /// @brief Получает полное имя выбранного компонента.
    QString getSelectedComponentLongName() const;
    QString getSelectedComponentClassName() const;
    UComponentGuiContext buildComponentGuiContext() const;

    /// @brief Обновляет состояние действий меню в зависимости от текущего состояния виджета.
    void updateActionsState();

private slots:
    // Context menu action handlers
    void componentViewOrBreakLink();
    void componentCreateLink();
    void componentFinishLink();
    void componentCancelLink();
    void componentStartMoving();
    void componentFinishMoving();
    void componentCancelMoving();
    void componentStartSwitching();
    void componentFinishSwitching();
    void componentCancelSwitching();
    void componentRename();
    void actionClassDescriptionTriggered();
    void componentDelete();
    void componentCopyNameToClipboard();
    void componentCopyLongNameToClipboard();
    void componentCopyClassNameToClipboard();
    void componentReset();
    void componentCalculate();
    void componentDefault();
    void componentGUI();
    void componentCopyXMLDescription();
    void componentCloneComponent();
    void componentQuickLink();
    void componentClearCache();

private:
    UModernDiagramWidget* m_owner;
    QMenu* m_contextMenu;

    // Actions
    QAction* m_actionViewOrBreakLink;
    QAction* m_actionCreateLink;
    QAction* m_actionFinishLink;
    QAction* m_actionCancelLink;
    QAction* m_actionStartMoving;
    QAction* m_actionFinishMoving;
    QAction* m_actionCancelMoving;
    QAction* m_actionSwitchLink;
    QAction* m_actionFinishSwitching;
    QAction* m_actionCancelSwitching;
    QAction* m_actionCloneComponent;
    QAction* m_actionQuickLink;
    QAction* m_actionComponentGui;

    // Context menu state
    QString m_firstComponentToConnection;
    QString m_startMoveComponent;
    QString m_startSwitchComponent;
    UModernDiagramNodeItem* m_contextMenuNode;
};

#endif // UMODERNDIAGRAMCONTEXTMENU_H

