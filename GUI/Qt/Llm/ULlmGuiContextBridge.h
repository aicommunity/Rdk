#ifndef RDK_ULLM_GUI_CONTEXT_BRIDGE_H
#define RDK_ULLM_GUI_CONTEXT_BRIDGE_H

#include <QObject>

#include "LLMGuiTypes.h"
#include "../UComponentGuiContext.h"

namespace RDK {
class UApplication;
}

class ULlmGuiContextBridge : public QObject {
    Q_OBJECT
public:
    explicit ULlmGuiContextBridge(RDK::UApplication* app, QObject* parent = nullptr);

    LLMGuiContext currentContext() const;

public slots:
    void onDiagramSelectionChanged(const UComponentGuiContext& ctx);
    void onDiagramScopeChanged(const QString& scope_long_name);
    void onProjectLoaded(const QString& path);
    void onProjectClosed();
    void onChannelChanged(int index);

signals:
    void contextChanged(const LLMGuiContext& ctx);
    void projectOpened(const QString& configuration_ini_path);
    void projectClosed();

private:
    void emitIfChanged();

    RDK::UApplication* m_app = nullptr;
    LLMGuiContext m_ctx;
};

#endif
