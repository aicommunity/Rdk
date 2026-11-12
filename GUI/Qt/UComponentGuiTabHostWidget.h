#ifndef UCOMPONENTGUITABHOSTWIDGET_H
#define UCOMPONENTGUITABHOSTWIDGET_H

#include <QHash>
#include <QPointer>
#include <QTabWidget>

#include <functional>

#include "UComponentGuiContext.h"
#include "UVisualControllerWidget.h"

class QMimeData;
class UComponentGuiService;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;

class UComponentGuiTabHostWidget : public UVisualControllerWidget
{
    Q_OBJECT
public:
    explicit UComponentGuiTabHostWidget(const QString& hostId,
                                        UComponentGuiService* service,
                                        QWidget* parent = nullptr,
                                        RDK::UApplication* app = nullptr);

    QString hostId() const;
    bool assignContext(const UComponentGuiContext& context);
    bool removeContext(const UComponentGuiContext& context);
    bool hasContext(const UComponentGuiContext& context) const;
    QList<UComponentGuiContext> contexts() const;
    int tabCount() const;

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

    /// Remove a tab whose cell is empty for this context (after the widget moved to another host).
    void pruneStaleTabForContext(const UComponentGuiContext& context);

    void setAfterAssignContextHook(std::function<void(const UComponentGuiContext&)> hook);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    bool closeTabAtIndex(int index);
    void discardStaleTabCellForKey(const QString& key);
    bool handleDrop(const QMimeData* mimeData);
    void startDragFromTab(int index);
    int tabIndexForContext(const UComponentGuiContext& context) const;
    QWidget* hostWidgetForIndex(int index) const;
    void syncCurrentTabState();
    static void embedWidgetInTabCell(QWidget* cell, UVisualControllerWidget* widget);
    static QString contextKey(const UComponentGuiContext& context);

private:
    QString m_hostId;
    UComponentGuiService* m_service = nullptr;
    QTabWidget* m_tabWidget = nullptr;
    QHash<QString, QPointer<QWidget>> m_tabHosts;
    QPoint m_dragStartPos;
    int m_dragStartIndex = -1;
    std::function<void(const UComponentGuiContext&)> m_afterAssignContextHook;
};

#endif // UCOMPONENTGUITABHOSTWIDGET_H
