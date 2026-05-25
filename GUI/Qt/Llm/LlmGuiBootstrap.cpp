#include "LlmGuiBootstrap.h"

#include <QPointer>
#include <QShortcut>

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

void RegisterLlmUi(UGEngineControlWidget* host, RDK::UApplication* app, ULlmGuiContextBridge* bridge)
{
    if(!host || !app || !bridge)
        return;

    UCustomWidgetDescriptor assistant;
    assistant.id = QStringLiteral("llm.assistant");
    assistant.title = QObject::tr("AI Assistant");
    assistant.menuPath = QStringLiteral("View/AI Assistant");
    assistant.placement = UCustomWidgetPlacement::Dock;
    assistant.defaultDockArea = Qt::RightDockWidgetArea;
    assistant.shortcut = QKeySequence(QStringLiteral("Ctrl+Shift+A"));
    assistant.factory = [bridge](RDK::UApplication* application) -> UVisualControllerWidget* {
        return new ULlmAssistantDockWidget(nullptr, application, bridge);
    };
    host->registerCustomWidget(assistant);

    UCustomWidgetDescriptor preview;
    preview.id = QStringLiteral("llm.preview");
    preview.title = QObject::tr("AI Change Preview");
    preview.menuPath = QStringLiteral("View/AI Change Preview");
    preview.placement = UCustomWidgetPlacement::Dock;
    preview.defaultDockArea = Qt::BottomDockWidgetArea;
    preview.factory = [](RDK::UApplication* application) -> UVisualControllerWidget* {
        auto* widget = new ULlmChangePreviewWidget(nullptr, application);
        g_plan_preview = widget;
        return widget;
    };
    host->registerCustomWidget(preview);

    auto* settings_action = new QAction(QObject::tr("AI Assistant Settings"), host);
    settings_action->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+L")));
    QObject::connect(settings_action, &QAction::triggered, host,
                     [host, app]() { OpenProviderSettingsDialog(host, app); });
    host->appendMenuAction(QStringLiteral("View"), settings_action);

    auto* shortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+A")), host);
    QObject::connect(shortcut, &QShortcut::activated, host, [host]() {
        host->showCustomWidgetById(QStringLiteral("llm.assistant"));
    });

    QObject::connect(host, &UGEngineControlWidget::openComponentGuiFromScheme, bridge,
                     &ULlmGuiContextBridge::onDiagramSelectionChanged);
}

} // namespace LlmGui
