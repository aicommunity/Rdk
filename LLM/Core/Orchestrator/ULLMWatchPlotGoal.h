#ifndef RDK_ULLM_WATCH_PLOT_GOAL_H
#define RDK_ULLM_WATCH_PLOT_GOAL_H

#include <string>
#include <vector>

namespace RDK::LLM {

struct ParsedWatchPlotGoal {
    bool ok = false;
    bool want_new_mdi = false;
    /// Root / parent component tokens to plot (e.g. PNeuron, PGenerator).
    std::vector<std::string> component_tokens;
    /// Same as component_tokens when nested_hint is set (explicit parent anchors).
    std::vector<std::string> anchor_tokens;
    /// Nested child short name / role under parent (e.g. LTZone). Empty = plot parent itself.
    std::string nested_hint;
    /// Preferred property (default Output when empty).
    std::string property_name = "Output";
};

bool isWatchPlotGoal(const std::string& text);

std::string watchPlotEphemeralHint();

ParsedWatchPlotGoal parseWatchPlotGoal(const std::string& text);

} // namespace RDK::LLM

#endif
