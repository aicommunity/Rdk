#ifndef RDK_LLM_GUI_BOOTSTRAP_H
#define RDK_LLM_GUI_BOOTSTRAP_H

class QString;
class QWidget;
class UGEngineControlWidget;
class ULlmGuiContextBridge;

namespace RDK {
class UApplication;
}

namespace LlmGui {

void RegisterLlmUi(UGEngineControlWidget* host, RDK::UApplication* app, ULlmGuiContextBridge* bridge,
                   bool showMenu = false);
void OpenProviderSettingsDialog(QWidget* parent, RDK::UApplication* app);
void showPlanPreview(UGEngineControlWidget* host, const QString& summary);

}

#endif
