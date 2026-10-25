#include "UPackProjectDescription.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {

namespace {

std::string toLowerAscii(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool mentionsProjectOrConfig(const std::string& lower)
{
    return lower.find("проект") != std::string::npos || lower.find("конфиг") != std::string::npos
           || lower.find("конфигурац") != std::string::npos || lower.find("project") != std::string::npos
           || lower.find("config") != std::string::npos || lower.find("configuration") != std::string::npos;
}

bool looksLikeProjectDescriptionGoal(const std::string& text)
{
    const std::string lower = toLowerAscii(text);
    if(lower.find("project description") != std::string::npos
       || lower.find("project_description") != std::string::npos
       || lower.find("configuration description") != std::string::npos
       || lower.find("описание конфигурации") != std::string::npos
       || lower.find("описание проекта") != std::string::npos
       || lower.find("описание конфиг") != std::string::npos)
        return true;
    if(lower.find("описани") != std::string::npos && mentionsProjectOrConfig(lower))
        return true;
    if((lower.find("запиш") != std::string::npos || lower.find("write ") != std::string::npos
        || lower.find("обнови") != std::string::npos)
       && (lower.find("description") != std::string::npos || lower.find("описан") != std::string::npos)
       && mentionsProjectOrConfig(lower))
        return true;
    if((lower.find("модул") != std::string::npos || lower.find("module") != std::string::npos)
       && (lower.find("описан") != std::string::npos || lower.find("description") != std::string::npos
           || lower.find("состоит") != std::string::npos))
        return true;
    return false;
}

} // namespace

PackMatch UPackProjectDescription::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    if(!snap.req || snap.skip_pre_llm_funnel)
        return m;
    if(!looksLikeProjectDescriptionGoal(snap.req->user_text)
       && !looksLikeProjectDescriptionGoal(snap.planning_text))
        return m;
    m.score = 0.75f;
    m.reason = "project_description";
    return m;
}

PackHintContribution UPackProjectDescription::hints(const PackTurnSnapshot& snap) const
{
    (void)snap;
    PackHintContribution h;
    h.ephemeral_markdown =
        "## Project / configuration description\n"
        "In this product, **project**, **configuration**, and **конфигурация** mean the currently "
        "open configuration.\n"
        "When writing or updating project description / modules:\n"
        "1. Call `get_net_snapshot` **omitting** `root_long_name` and/or `inspect_configuration` "
        "**omitting** `configuration_path` (empty = open configuration). "
        "Never pass `configuration_path` as the words 'open project' or `root_path` as '.'.\n"
        "2. Synthesize a concrete description from those results (user language).\n"
        "3. You MUST call `update_configuration` with `project_description` set to that text.\n"
        "Never write placeholders like «указать здесь», «которое вы хотите добавить», "
        "or `[list modules here]`. Do not finish with prose alone — the write is required.\n"
        "Never greet the user or invent product names; stay on NeuroModeler / the open configuration.";
    h.extra_tool_names = {"get_net_snapshot", "inspect_configuration", "list_project_files",
                          "read_text_artifact", "update_configuration", "ask_user"};
    h.act_or_clarify_recovery_tools = {"get_net_snapshot", "inspect_configuration",
                                       "update_configuration", "ask_user"};
    h.force_include_write = true;
    return h;
}

} // namespace RDK::LLM
