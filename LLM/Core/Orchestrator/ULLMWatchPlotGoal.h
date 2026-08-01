#ifndef RDK_ULLM_WATCH_PLOT_GOAL_H
#define RDK_ULLM_WATCH_PLOT_GOAL_H

#include <string>
#include <vector>

namespace RDK::LLM {

struct ParsedWatchPlotGoal {
    bool ok = false;
    bool want_new_mdi = false;
    /// Component short/long tokens to plot (e.g. generator, neuron names).
    std::vector<std::string> component_tokens;
    /// Preferred property (default Output when empty).
    std::string property_name = "Output";
};

bool isWatchPlotGoal(const std::string& text);

std::string watchPlotEphemeralHint();

ParsedWatchPlotGoal parseWatchPlotGoal(const std::string& text);

} // namespace RDK::LLM

#endif
