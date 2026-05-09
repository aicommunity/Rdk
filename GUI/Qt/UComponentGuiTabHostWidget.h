#ifndef UCOMPONENTGUITABHOSTWIDGET_H
#define UCOMPONENTGUITABHOSTWIDGET_H

#include <QHash>
#include <QPointer>
#include <QTabWidget>

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

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    bool handleDrop(const QMimeData* mimeData);
    void startDragFromTab(int index);
    int tabIndexForContext(const UComponentGuiContext& context) const;
    QWidget* hostWidgetForIndex(int index) const;
    void syncCurrentTabState();
    static QString contextKey(const UComponentGuiContext& context);

private:
    QString m_hostId;
    UComponentGuiService* m_service = nullptr;
    QTabWidget* m_tabWidget = nullptr;
    QHash<QString, QPointer<QWidget>> m_tabHosts;
    QPoint m_dragStartPos;
    int m_dragStartIndex = -1;
};

#endif // UCOMPONENTGUITABHOSTWIDGET_H
