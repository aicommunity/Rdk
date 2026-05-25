#include "ULlmGuiContextBridge.h"

#include <rdk_application.h>

ULlmGuiContextBridge::ULlmGuiContextBridge(RDK::UApplication* app, QObject* parent)
    : QObject(parent)
    , m_app(app)
{
    m_ctx.application = app;
}

LLMGuiContext ULlmGuiContextBridge::currentContext() const
{
    return m_ctx;
}

void ULlmGuiContextBridge::onDiagramSelectionChanged(const UComponentGuiContext& ctx)
{
    m_ctx.focused_component_long_name = ctx.componentLongName;
    m_ctx.focused_class_name = ctx.componentClassName;
    if(ctx.channelIndex >= 0)
        m_ctx.channel_index = ctx.channelIndex;
    emitIfChanged();
}

void ULlmGuiContextBridge::onProjectLoaded(const QString& path)
{
    m_ctx.project_xml_path = path;
    emitIfChanged();
}

void ULlmGuiContextBridge::onProjectClosed()
{
    m_ctx.project_xml_path.clear();
    emitIfChanged();
}

void ULlmGuiContextBridge::onChannelChanged(int index)
{
    m_ctx.channel_index = index;
    emitIfChanged();
}

void ULlmGuiContextBridge::emitIfChanged()
{
    emit contextChanged(m_ctx);
}
