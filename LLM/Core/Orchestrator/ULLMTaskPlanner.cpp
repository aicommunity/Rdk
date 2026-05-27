#include "ULLMTaskPlanner.h"

#include "ULLMQuantityParser.h"

#include <algorithm>
#include <filesystem>
#include <mutex>
#include <regex>
#include <set>

namespace RDK::LLM {

namespace {

bool containsWord(const std::string& text, const std::string& word)
{
    const std::regex re("\\b" + word + "\\b", std::regex::icase);
    return std::regex_search(text, re);
}

std::string extractClassName(const std::string& text)
{
    const std::regex klass(R"(\b([A-Z][A-Za-z0-9_]{2,})\b)");
    for(std::sregex_iterator it(text.begin(), text.end(), klass), end; it != end; ++it)
    {
        const std::string k = it->str(1);
        if(k.find("NSP") == 0 || k.find('N') == 0)
            return k;
    }
    return "NSPNeuron";
}

const ULinkPatternCatalog& defaultLinkCatalog()
{
    static std::once_flag once;
    static ULinkPatternCatalog catalog;
    std::call_once(once, []() {
        std::filesystem::path root = std::filesystem::current_path();
        for(int i = 0; i < 8 && root.has_parent_path(); ++i)
        {
            if(std::filesystem::exists(root / "CMakeLists.txt"))
                break;
            root = root.parent_path();
        }
        if(!root.empty())
            catalog.loadFromFile(root / "Bin/LLM/index/link-patterns.json");
    });
    return catalog;
}

void appendIssue(TaskPlanResult& out, const std::string& issue)
{
    out.issues.push_back(issue);
    out.plan.confidence = std::max(0.0f, out.plan.confidence - 0.2f);
}

void postValidatePlan(TaskPlanResult& out, URdkDomainAccess& domain, const TaskPlanRequest& req)
{
    std::vector<std::string> registered;
    const DomainStatus classes_st = domain.listRegisteredClassNames(registered);
    std::set<std::string> class_set(registered.begin(), registered.end());

    for(const ExecutionPlanStep& step : out.plan.steps)
    {
        if(step.tool_name == "add_component")
        {
            const std::string class_name = step.arguments.value("class_name", "");
            if(!class_name.empty() && classes_st.ok() && !class_set.count(class_name))
                appendIssue(out, "unknown_class:" + class_name);
        }
        else if(step.tool_name == "connect_components")
        {
            const std::string from_ln = step.arguments.value("from_long_name", "");
            const std::string to_ln = step.arguments.value("to_long_name", "");
            nlohmann::json from_comp;
            nlohmann::json to_comp;
            if(!from_ln.empty() && !domain.findComponentByLongName(from_ln, from_comp,
                                                                   req.session.active_channel_index)
                                      .ok())
                appendIssue(out, "connect_from_not_found:" + from_ln);
            if(!to_ln.empty() && !domain.findComponentByLongName(to_ln, to_comp,
                                                                 req.session.active_channel_index)
                                    .ok())
                appendIssue(out, "connect_to_not_found:" + to_ln);

            const std::string from_prop = step.arguments.value("from_property", "");
            const std::string to_prop = step.arguments.value("to_property", "");
            if(from_prop.empty() || to_prop.empty())
            {
                const ULinkPatternCatalog* catalog = req.link_catalog ? req.link_catalog : &defaultLinkCatalog();
                std::string from_class;
                std::string to_class;
                if(catalog && domain.getComponentClassName(from_ln, req.session.active_channel_index, from_class).ok()
                   && domain.getComponentClassName(to_ln, req.session.active_channel_index, to_class).ok())
                {
                    if(catalog->suggest(from_class, to_class, 1).empty())
                        appendIssue(out, "no_link_pattern_for_class_pair");
                }
            }
        }
    }
}

} // namespace

TaskPlanResult buildTaskPlan(ILLMProvider& provider,
                             ULLMToolRegistry& registry,
                             URdkDomainAccess& domain,
                             const TaskPlanRequest& req)
{
    (void)provider;
    (void)registry;

    TaskPlanResult out;
    out.ok = true;
    out.plan.plan_id = "task_" + std::to_string(std::hash<std::string>{}(req.goal_en));
    out.plan.goal_en = req.goal_en;
    out.plan.requires_user_confirmation = true;
    out.plan.confidence = 0.7f;

    ExecutionPlanStep snap;
    snap.step_id = 1;
    snap.tool_name = "get_net_snapshot";
    snap.arguments = {{"channel_index", req.session.active_channel_index}};
    snap.success = SuccessCriteria{"tool_ok", nlohmann::json::object()};
    out.plan.steps.push_back(snap);

    const ParsedQuantity quantity = extractQuantity(req.goal_en);
    int step_id = 2;
    if(containsWord(req.goal_en, "add"))
    {
        ExecutionPlanStep add;
        add.step_id = step_id++;
        add.tool_name = "add_component";
        const std::string class_name = extractClassName(req.goal_en);
        add.arguments = {{"class_name", class_name},
                         {"parent_long_name", ""},
                         {"short_name", "Neuron"},
                         {"channel_index", req.session.active_channel_index}};
        add.depends_on = {1};
        const int min_count = quantity.count > 1 ? quantity.count : 1;
        add.success = SuccessCriteria{
            "component_count", {{"class_name", class_name}, {"min_count", min_count}}};
        add.repeat_count = min_count;
        out.plan.steps.push_back(add);
        out.plan.goal_success = add.success;
        out.plan.confidence = min_count > 1 ? 0.82f : 0.75f;
    }

    if(containsWord(req.goal_en, "connect") || containsWord(req.goal_en, "link"))
    {
        ExecutionPlanStep connect;
        connect.step_id = step_id++;
        connect.tool_name = "connect_components";
        connect.arguments = {{"from_long_name", "PNeuron"},
                             {"to_long_name", "PNeuron2"},
                             {"from_property", ""},
                             {"to_property", ""},
                             {"channel_index", req.session.active_channel_index}};
        connect.depends_on = {step_id > 3 ? 2 : 1};
        connect.success = SuccessCriteria{"link_exists",
                                          {{"from_long_name", "PNeuron"},
                                           {"to_long_name", "PNeuron2"}}};
        out.plan.steps.push_back(connect);
        out.plan.goal_success = connect.success;
        out.plan.confidence = std::max(out.plan.confidence, 0.8f);
    }

    if(out.plan.steps.size() == 1)
    {
        out.ok = false;
        out.issues.push_back("no_task_steps");
    }

    postValidatePlan(out, domain, req);
    if(!out.issues.empty())
        out.ok = false;
    out.confidence = out.plan.confidence;
    return out;
}

} // namespace RDK::LLM
