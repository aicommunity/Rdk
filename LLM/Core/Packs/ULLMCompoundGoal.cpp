#include "ULLMCompoundGoal.h"

#include <QString>

#include "../Orchestrator/ULLMChannelCalcCommand.h"
#include "../Orchestrator/ULLMComponentStructureGoal.h"
#include "../Orchestrator/ULLMConfigurationLifecycle.h"
#include "../Orchestrator/ULLMConnectPlanParsing.h"
#include "../Orchestrator/ULLMWatchPlotGoal.h"

namespace RDK::LLM {

namespace {

std::string toLowerUtf8(const std::string& s)
{
    QString q = QString::fromUtf8(s.data(), static_cast<int>(s.size())).toLower();
    q.replace(QString::fromUtf8("ё"), QString::fromUtf8("е"));
    return q.toUtf8().toStdString();
}

bool hasConjunction(const std::string& lower)
{
    return lower.find(" и ") != std::string::npos || lower.find(" and ") != std::string::npos
           || lower.find(" затем ") != std::string::npos || lower.find(" then ") != std::string::npos
           || lower.find(", и ") != std::string::npos || lower.find(", and ") != std::string::npos
           || lower.find(" а также ") != std::string::npos;
}

bool looksLikeAddFamily(const std::string& lower)
{
    return lower.find("добав") != std::string::npos || lower.find("add ") != std::string::npos
           || lower.find("create ") != std::string::npos || lower.find("создай") != std::string::npos
           || lower.find("создать") != std::string::npos;
}

} // namespace

bool isCompoundActionableGoal(const std::string& user_text)
{
    if(user_text.empty())
        return false;
    const std::string lower = toLowerUtf8(user_text);
    if(!hasConjunction(lower))
        return false;

    int families = 0;
    if(looksLikeAddFamily(lower))
        ++families;
    if(detectChannelCalcAction(user_text) != ChannelCalcAction::None)
        ++families;
    if(isConnectGoalText(user_text))
        ++families;
    if(parseWatchPlotGoal(user_text).ok)
        ++families;
    if(isComponentStructureGoal(user_text) || parseDendriteStructureGoal(user_text).ok)
        ++families;
    if(detectConfigurationLifecycleAction(user_text) != ConfigurationLifecycleAction::None)
        ++families;
    return families >= 2;
}

const std::vector<std::string>& packRecordedDependencyOrder()
{
    static const std::vector<std::string> kOrder = {
        "add_component_direct", "component_structure", "connect",
        "watch_plot",           "channel_calc",        "lifecycle_soft",
    };
    return kOrder;
}

int packDependencyRank(const std::string& pack_id)
{
    const auto& order = packRecordedDependencyOrder();
    for(size_t i = 0; i < order.size(); ++i)
    {
        if(order[i] == pack_id)
            return static_cast<int>(i);
    }
    return 1000;
}

const char* workingGoalIdForPack(const std::string& pack_id)
{
    if(pack_id == "add_component_direct")
        return "goal_add";
    if(pack_id == "component_structure")
        return "goal_structure";
    if(pack_id == "connect")
        return "goal_connect";
    if(pack_id == "watch_plot")
        return "goal_watch";
    if(pack_id == "channel_calc")
        return "goal_calc";
    if(pack_id == "lifecycle_soft")
        return "goal_lifecycle";
    return "turn_goal";
}

const char* workingGoalTitleForPack(const std::string& pack_id)
{
    if(pack_id == "add_component_direct")
        return "Add component(s)";
    if(pack_id == "component_structure")
        return "Configure component structure";
    if(pack_id == "connect")
        return "Connect components";
    if(pack_id == "watch_plot")
        return "Add watch / plot";
    if(pack_id == "channel_calc")
        return "Channel calculation";
    if(pack_id == "lifecycle_soft")
        return "Configuration lifecycle";
    if(pack_id == "project_description")
        return "Update project description";
    return "User goal";
}

std::string workingGoalIdForToolName(const std::string& tool_name)
{
    if(tool_name == "add_component")
        return "goal_add";
    if(tool_name == "set_property" || tool_name == "calculate_component")
        return "goal_structure";
    if(tool_name == "connect_components" || tool_name == "list_model_links"
       || tool_name == "get_component_ports")
        return "goal_connect";
    if(tool_name == "add_watch_series" || tool_name == "create_watch_mdi")
        return "goal_watch";
    if(tool_name == "start_channel_calculation" || tool_name == "pause_channel_calculation"
       || tool_name == "reset_channel_calculation" || tool_name == "step_channel_calculation"
       || tool_name == "run_n_steps")
        return "goal_calc";
    if(tool_name == "update_configuration")
        return "goal_project_description";
    if(tool_name.find("configuration") != std::string::npos)
        return "goal_lifecycle";
    return {};
}

} // namespace RDK::LLM
