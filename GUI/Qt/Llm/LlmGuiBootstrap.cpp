#include "LlmGuiBootstrap.h"

#include <QShortcut>

#include "../UGEngineControlWidget.h"
#include "ULlmAssistantDockWidget.h"
#include "ULlmChangePreviewWidget.h"

namespace LlmGui {

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
        return new ULlmChangePreviewWidget(nullptr, application);
    };
    host->registerCustomWidget(preview);

    auto* shortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+A")), host);
    QObject::connect(shortcut, &QShortcut::activated, host, [host]() {
        host->showCustomWidgetById(QStringLiteral("llm.assistant"));
    });

    QObject::connect(host, &UGEngineControlWidget::openComponentGuiFromScheme, bridge,
                     &ULlmGuiContextBridge::onDiagramSelectionChanged);
}

} // namespace LlmGui
