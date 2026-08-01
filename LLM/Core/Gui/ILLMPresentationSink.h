#ifndef RDK_ILLM_PRESENTATION_SINK_H
#define RDK_ILLM_PRESENTATION_SINK_H

#include "../LlmTypes.h"

#include <functional>

namespace RDK::LLM {

/// Args for Watch series tools (surface window|mdi).
struct LLMWatchSeriesArgs {
    std::string surface = "window"; // "window" | "mdi"
    int mdi_id = -1;
    int tab_index = 0;
    int chart_index = 0;
    int channel_index = 0;
    std::string long_name;
    std::string property_name;
    int jx = 0;
    int jy = 0;
    int serie_index = -1; // for remove by index
};

class ILLMPresentationSink {
public:
    virtual ~ILLMPresentationSink() = default;
    virtual void apply(const LLMPresentationEvent& event) = 0;

    /// Read host-level recent configuration paths (most recent first).
    /// Used by LLM tools to implement `list_recent_configurations` and lifecycle "last".
    virtual std::vector<std::string> recentConfigurationPaths() const { return {}; }

    /// Read host UI panels state (id, title, visible).
    /// Used by LLM tools to implement `list_ui_panels`.
    virtual nlohmann::json listLlmUiPanelsState() const { return nlohmann::json::object(); }

    /// Run UApplication-backed commands on the host UI thread when a GUI sink is present.
    virtual ApplicationCommandResult invokeHostSynchronized(
        const std::function<ApplicationCommandResult()>& run)
    {
        return run();
    }

    /// Opaque host navigation state for diagram drill level (GUI sinks only).
    virtual std::string captureNavigationToken() const { return {}; }

    virtual void restoreNavigationToken(const std::string& token) { (void)token; }

    virtual void navigateToDiagramScope(const std::string& scope_long_name, int channel_index)
    {
        (void)scope_long_name;
        (void)channel_index;
    }

    /// Watch series / MDI — default: unavailable (headless / noop sink).
    virtual nlohmann::json watchAddSeries(const LLMWatchSeriesArgs& args)
    {
        (void)args;
        return {{"ok", false}, {"error", "Watch host unavailable"}};
    }
    virtual nlohmann::json watchListSeries(const LLMWatchSeriesArgs& args)
    {
        (void)args;
        return {{"ok", false},
                {"error", "Watch host unavailable"},
                {"items", nlohmann::json::array()}};
    }
    virtual nlohmann::json watchRemoveSeries(const LLMWatchSeriesArgs& args)
    {
        (void)args;
        return {{"ok", false}, {"error", "Watch host unavailable"}};
    }
    virtual nlohmann::json watchClearSeries(const LLMWatchSeriesArgs& args)
    {
        (void)args;
        return {{"ok", false}, {"error", "Watch host unavailable"}};
    }
    virtual nlohmann::json watchMdiList()
    {
        return {{"ok", true}, {"items", nlohmann::json::array()}};
    }
    virtual nlohmann::json watchMdiCreate(int grid_rows, int grid_cols, const std::string& title)
    {
        (void)grid_rows;
        (void)grid_cols;
        (void)title;
        return {{"ok", false}, {"error", "Watch host unavailable"}};
    }
    virtual nlohmann::json watchMdiFocus(int mdi_id)
    {
        (void)mdi_id;
        return {{"ok", false}, {"error", "Watch host unavailable"}};
    }
    virtual nlohmann::json watchMdiClose(int mdi_id)
    {
        (void)mdi_id;
        return {{"ok", false}, {"error", "Watch host unavailable"}};
    }
};

} // namespace RDK::LLM

#endif
