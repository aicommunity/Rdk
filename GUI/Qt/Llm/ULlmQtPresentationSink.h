#ifndef RDK_ULLM_QT_PRESENTATION_SINK_H
#define RDK_ULLM_QT_PRESENTATION_SINK_H

#include <QObject>

#include <functional>
#include <mutex>

#include "../../../LLM/Core/Gui/ILLMPresentationSink.h"

class UGEngineControlWidget;
class ULlmGuiContextBridge;

class ULlmQtPresentationSink : public QObject, public RDK::LLM::ILLMPresentationSink {
    Q_OBJECT
public:
    ULlmQtPresentationSink(UGEngineControlWidget* host, ULlmGuiContextBridge* bridge,
                           QObject* parent = nullptr);

    void apply(const RDK::LLM::LLMPresentationEvent& event) override;

    std::vector<std::string> recentConfigurationPaths() const override;

    nlohmann::json listLlmUiPanelsState() const override;

    RDK::LLM::ApplicationCommandResult invokeHostSynchronized(
        const std::function<RDK::LLM::ApplicationCommandResult()>& run) override;

    static int defaultInvokeTimeoutMs();

private slots:
    void applyOnGuiThread();
    void runHostCommandOnGuiThread();
    void runHostListUiPanelsOnGuiThread();

private:
    UGEngineControlWidget* m_host = nullptr;
    ULlmGuiContextBridge* m_bridge = nullptr;
    RDK::LLM::LLMPresentationEvent m_pending;
    mutable std::mutex m_host_mu;
    std::function<RDK::LLM::ApplicationCommandResult()> m_pending_host_run;
    RDK::LLM::ApplicationCommandResult m_pending_host_result{};
    mutable std::function<nlohmann::json()> m_pending_host_list_run;
    mutable nlohmann::json m_pending_host_list_result = nlohmann::json::object();
};

#endif
