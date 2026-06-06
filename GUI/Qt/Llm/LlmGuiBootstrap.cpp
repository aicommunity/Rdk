#include "LlmGuiBootstrap.h"

#include <QPointer>
#include <QShortcut>

#ifdef RDK_LLM_EMBEDDED
#include "EmbeddedLlamaProviderApi.h"
#endif

#include "../UGEngineControlWidget.h"
#include "ULlmAssistantDockWidget.h"
#include "ULlmChangePreviewWidget.h"
#include "ULlmProviderSettingsWidget.h"

namespace LlmGui {

namespace {

QPointer<ULlmChangePreviewWidget> g_plan_preview;

} // namespace

void showPlanPreview(UGEngineControlWidget* host, const QString& summary)
{
    if(host)
        host->showCustomWidgetById(QStringLiteral("llm.preview"));
    if(g_plan_preview)
        g_plan_preview->showPlan(summary);
}

void OpenProviderSettingsDialog(QWidget* parent, RDK::UApplication* app)
{
    if(!app)
        return;
    ULlmProviderSettingsWidget dlg(parent, app);
    dlg.exec();
}

void RegisterLlmUi(UGEngineControlWidget* host, RDK::UApplication* app, ULlmGuiContextBridge* bridge,
                   bool showMenu)
{
    if(!host || !app || !bridge)
        return;

#ifdef RDK_LLM_EMBEDDED
    RDK::LLM::EnsureEmbeddedLlamaProviderRegistered();
#endif

    const QString aiAssistantMenu = QObject::tr("AI Assistant");

    UCustomWidgetDescriptor assistant;
    assistant.id = QStringLiteral("llm.assistant");
    assistant.title = aiAssistantMenu;
    if(showMenu)
    {
        assistant.menuPath = aiAssistantMenu;
        assistant.shortcut = QKeySequence(QStringLiteral("Ctrl+Shift+A"));
    }
    assistant.placement = UCustomWidgetPlacement::Dock;
    assistant.defaultDockArea = Qt::RightDockWidgetArea;
    assistant.factory = [bridge](RDK::UApplication* application) -> UVisualControllerWidget* {
        return new ULlmAssistantDockWidget(nullptr, application, bridge);
    };
    host->registerCustomWidget(assistant);

    UCustomWidgetDescriptor preview;
    preview.id = QStringLiteral("llm.preview");
    preview.title = QObject::tr("AI Change Preview");
    if(showMenu)
    {
        preview.menuPath =
            aiAssistantMenu + QStringLiteral("/") + QObject::tr("AI Change Preview");
    }
    preview.placement = UCustomWidgetPlacement::Dock;
    preview.defaultDockArea = Qt::BottomDockWidgetArea;
    preview.factory = [](RDK::UApplication* application) -> UVisualControllerWidget* {
        auto* widget = new ULlmChangePreviewWidget(nullptr, application);
        g_plan_preview = widget;
        return widget;
    };
    host->registerCustomWidget(preview);

    if(showMenu)
    {
        host->appendMenuSeparator(aiAssistantMenu);

        auto* settings_action = new QAction(QObject::tr("AI Assistant Settings"), host);
        settings_action->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+L")));
        QObject::connect(settings_action, &QAction::triggered, host,
                         [host, app]() { OpenProviderSettingsDialog(host, app); });
        host->appendMenuAction(aiAssistantMenu, settings_action);

        auto* shortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+A")), host);
        QObject::connect(shortcut, &QShortcut::activated, host, [host]() {
            host->showCustomWidgetById(QStringLiteral("llm.assistant"));
        });
    }

    QObject::connect(host, &UGEngineControlWidget::openComponentGuiFromScheme, bridge,
                     &ULlmGuiContextBridge::onDiagramSelectionChanged);
    if(UModernDiagramContainerWidget* diagram_container = host->modernDiagramContainer())
    {
        if(UModernDiagramWidget* diagram = diagram_container->modernDiagramWidget())
        {
            QObject::connect(diagram, &UModernDiagramWidget::diagramScopeChanged, bridge,
                             &ULlmGuiContextBridge::onDiagramScopeChanged);
            bridge->onDiagramScopeChanged(diagram->diagramScopeLongName());
        }
    }
}

} // namespace LlmGui
