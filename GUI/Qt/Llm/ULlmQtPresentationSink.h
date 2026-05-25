#ifndef RDK_ULLM_QT_PRESENTATION_SINK_H
#define RDK_ULLM_QT_PRESENTATION_SINK_H

#include <QObject>

#include "../../../LLM/Core/Gui/ILLMPresentationSink.h"

class UGEngineControlWidget;
class ULlmGuiContextBridge;

class ULlmQtPresentationSink : public QObject, public RDK::LLM::ILLMPresentationSink {
    Q_OBJECT
public:
    ULlmQtPresentationSink(UGEngineControlWidget* host, ULlmGuiContextBridge* bridge,
                           QObject* parent = nullptr);

    void apply(const RDK::LLM::LLMPresentationEvent& event) override;

    static int defaultInvokeTimeoutMs();

private slots:
    void applyOnGuiThread();

private:
    UGEngineControlWidget* m_host = nullptr;
    ULlmGuiContextBridge* m_bridge = nullptr;
    RDK::LLM::LLMPresentationEvent m_pending;
};

#endif
