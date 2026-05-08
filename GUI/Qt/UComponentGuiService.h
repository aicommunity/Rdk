#ifndef UCOMPONENTGUISERVICE_H
#define UCOMPONENTGUISERVICE_H

#include <QString>
#include <QHash>
#include <QPointer>
#include <QByteArray>

#include "UComponentGuiContext.h"
#include "UComponentFormRegistry.h"
#include "IComponentControllerWidget.h"

class QWidget;
class UVisualControllerWidget;
class QMdiArea;
class QMainWindow;
class QDockWidget;

enum class UComponentGuiHostMode
{
    Mdi,
    Floating,
    TabHost,
    SecondaryDock,
    Grid // legacy compatibility mode
};

struct UComponentGuiSessionSnapshot
{
    QString sessionId;
    QString formId;
    QString componentClassName;
    QString componentLongName;
    int channelIndex = -1;
    UComponentGuiHostMode hostMode = UComponentGuiHostMode::Mdi;
    QString containerId;
    int cellRow = -1;
    int cellCol = -1;
    int orderIndex = 0;
    bool isActive = false;
};

class UComponentGuiService
{
public:
    explicit UComponentGuiService(RDK::UApplication* app = nullptr);

    void setApplication(RDK::UApplication* app);

    bool canOpen(const UComponentGuiContext& context) const;
    UVisualControllerWidget* createOrActivate(QWidget* parentWindow, const UComponentGuiContext& context);
    bool detachToFloating(const UComponentGuiContext& context);
    bool attachToMdi(const UComponentGuiContext& context, QMdiArea* mdiArea);
    bool attachToSecondaryDock(const UComponentGuiContext& context);
    bool moveToTabHost(const UComponentGuiContext& context, const QString& hostId, QWidget* hostWidget);
    void setHostMainWindow(QMainWindow* mainWindow);
    void setSecondaryHostMainWindow(QMainWindow* mainWindow);
    QList<UComponentGuiSessionSnapshot> snapshotOpenSessions() const;
    void clearClosedInstances();
    void clearAllInstances();
    bool applyFloatingState(const QString& sessionId, const QByteArray& geometry, const QByteArray& state);
    QByteArray floatingGeometry(const QString& sessionId) const;
    QByteArray floatingWindowState(const QString& sessionId) const;
    bool captureFloatingState(const QString& sessionId);
    bool tryGetContextByWidget(const UVisualControllerWidget* widget, UComponentGuiContext& outContext) const;
    bool tryGetHostModeByWidget(const UVisualControllerWidget* widget, UComponentGuiHostMode& outMode) const;
    bool tryGetWidgetByContext(const UComponentGuiContext& context, UVisualControllerWidget*& outWidget) const;
    bool tryGetHostModeByContext(const UComponentGuiContext& context, UComponentGuiHostMode& outMode) const;

private:
    QString makeInstanceKey(const UComponentGuiContext& context, const UComponentFormDescriptor& descriptor) const;
    void applyContext(UVisualControllerWidget* widget, const UComponentGuiContext& context) const;
    QString makeSessionKeyFromContext(const UComponentGuiContext& context) const;
    bool resolveDescriptor(const UComponentGuiContext& context, UComponentFormDescriptor& descriptor) const;
    UVisualControllerWidget* resolveInstance(const UComponentGuiContext& context, QString* outSessionKey = nullptr) const;
    void assignHostMode(const QString& key, UComponentGuiHostMode mode, const QString& containerId = QString(), int row = -1, int col = -1);
    void clearSessionState(const QString& key);
    QString findSessionKeyByWidget(const UVisualControllerWidget* widget) const;

private:
    struct UInstanceHostInfo
    {
        UComponentGuiHostMode mode = UComponentGuiHostMode::Mdi;
        QString containerId;
        int row = -1;
        int col = -1;
        QByteArray floatingGeometry;
        QByteArray floatingWindowState;
    };

    RDK::UApplication* m_application;
    QPointer<QMainWindow> m_hostMainWindow;
    QPointer<QMainWindow> m_secondaryHostMainWindow;
    QHash<QString, QPointer<UVisualControllerWidget>> m_instances;
    QHash<QString, QPointer<QDockWidget>> m_dockHosts;
    QHash<QString, UComponentGuiContext> m_instanceContexts;
    QHash<QString, QString> m_instanceFormIds;
    QHash<QString, UInstanceHostInfo> m_instanceHostInfo;
    mutable int m_activationCounter = 0;
    mutable QString m_lastActiveSession;
};

#endif // UCOMPONENTGUISERVICE_H
