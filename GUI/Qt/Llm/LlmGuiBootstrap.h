#ifndef RDK_LLM_GUI_BOOTSTRAP_H
#define RDK_LLM_GUI_BOOTSTRAP_H

class UGEngineControlWidget;
class ULlmGuiContextBridge;

namespace RDK {
class UApplication;
}

namespace LlmGui {

void RegisterLlmUi(UGEngineControlWidget* host, RDK::UApplication* app, ULlmGuiContextBridge* bridge);

}

#endif
