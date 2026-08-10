#include "ULLMSessionGraphMemory.h"

#include "../Domain/URdkDomainAccess.h"
#include "ULLMConversationStore.h"

#include <algorithm>
#include <ctime>

namespace RDK::LLM {

namespace {

int64_t nowUnixSec()
{
    return static_cast<int64_t>(std::time(nullptr));
}

} // namespace

nlohmann::json sessionGraphMemoryToJson(const SessionGraphMemory& graph)
{
    nlohmann::json out = nlohmann::json::object();
    if(!graph.added_long_names.empty())
        out["added_long_names"] = graph.added_long_names;
    if(!graph.linked_records.empty())
    {
        out["linked_records"] = nlohmann::json::array();
        for(const ConnectRecord& rec : graph.linked_records)
        {
            out["linked_records"].push_back(
                {{"from_long_name", rec.from_long_name},
                 {"to_long_name", rec.to_long_name},
                 {"from_property", rec.from_property},
                 {"to_property", rec.to_property},
                 {"created_at_unix_sec", rec.created_at_unix_sec}});
        }
    }
    if(graph.last_template)
    {
        out["last_template"] = {{"from_class", graph.last_template->from_class},
                                {"to_class", graph.last_template->to_class},
                                {"from_port", graph.last_template->from_port},
                                {"to_port", graph.last_template->to_port}};
    }
    if(graph.last_add && !graph.last_add->class_name.empty())
    {
        out["last_add"] = {{"class_name", graph.last_add->class_name},
                           {"parent_long_name", graph.last_add->parent_long_name},
                           {"short_name_base", graph.last_add->short_name_base}};
    }
    return out;
}

SessionGraphMemory sessionGraphMemoryFromJson(const nlohmann::json& j)
{
    SessionGraphMemory out;
    if(!j.is_object())
        return out;

    if(j.contains("added_long_names") && j["added_long_names"].is_array())
    {
        for(const auto& item : j["added_long_names"])
        {
            if(item.is_string())
                out.added_long_names.push_back(item.get<std::string>());
        }
    }
    if(j.contains("linked_records") && j["linked_records"].is_array())
    {
        for(const auto& item : j["linked_records"])
        {
            ConnectRecord rec;
            rec.from_long_name = item.value("from_long_name", "");
            rec.to_long_name = item.value("to_long_name", "");
            rec.from_property = item.value("from_property", "");
            rec.to_property = item.value("to_property", "");
            rec.created_at_unix_sec = item.value("created_at_unix_sec", int64_t{0});
            if(!rec.from_long_name.empty() && !rec.to_long_name.empty())
                out.linked_records.push_back(std::move(rec));
        }
    }
    if(j.contains("last_template") && j["last_template"].is_object())
    {
        ConnectTemplate t;
        t.from_class = j["last_template"].value("from_class", "");
        t.to_class = j["last_template"].value("to_class", "");
        t.from_port = j["last_template"].value("from_port", "");
        t.to_port = j["last_template"].value("to_port", "");
        if(!t.from_class.empty() && !t.to_class.empty())
            out.last_template = std::move(t);
    }
    if(j.contains("last_add") && j["last_add"].is_object())
    {
        LastAddComponentMemory add;
        add.class_name = j["last_add"].value("class_name", "");
        add.parent_long_name = j["last_add"].value("parent_long_name", "");
        add.short_name_base = j["last_add"].value("short_name_base", "");
        if(!add.class_name.empty())
            out.last_add = std::move(add);
    }
    return out;
}

void resetSessionGraphMemory(SessionGraphMemory& graph)
{
    graph.added_long_names.clear();
    graph.linked_records.clear();
    graph.last_template.reset();
    graph.last_add.reset();
}

void syncSessionGraphOnSessionChange(ConversationState& state, const LLMSessionContext& session)
{
    if(!state.last_session_context)
        return;
    if(state.last_session_context->project_loaded == session.project_loaded)
        return;
    resetSessionGraphMemory(state.session_graph);
}

void recordSessionAdd(ConversationState& state, const std::string& long_name)
{
    if(long_name.empty())
        return;
    auto& added = state.session_graph.added_long_names;
    if(std::find(added.begin(), added.end(), long_name) == added.end())
        added.push_back(long_name);
}

std::string formatConnectKnownFact(const ConnectRecord& rec)
{
    std::string fact = "Connected " + rec.from_long_name + " to " + rec.to_long_name;
    if(!rec.from_property.empty() || !rec.to_property.empty())
    {
        fact += " via ";
        if(!rec.from_property.empty())
            fact += rec.from_property;
        if(!rec.to_property.empty())
            fact += " -> " + rec.to_property;
    }
    return fact;
}

void appendConnectKnownFact(ConversationState& state, const ConnectRecord& rec)
{
    const std::string fact = formatConnectKnownFact(rec);
    if(fact.empty())
        return;
    if(std::find(state.known_facts.begin(), state.known_facts.end(), fact)
       == state.known_facts.end())
        state.known_facts.push_back(fact);
}

void recordSessionConnect(ConversationState& state, URdkDomainAccess& domain,
                          const nlohmann::json& connect_result, int channel_index)
{
    if(connect_result.value("already_existed", false))
        return;

    ConnectRecord rec;
    rec.from_long_name = connect_result.value("from_long_name", "");
    rec.to_long_name = connect_result.value("to_long_name", "");
    rec.from_property = connect_result.value("from_property", "");
    rec.to_property = connect_result.value("to_property", "");
    rec.created_at_unix_sec = nowUnixSec();
    if(rec.from_long_name.empty() || rec.to_long_name.empty())
        return;
    state.session_graph.linked_records.push_back(rec);
    appendConnectKnownFact(state, rec);

    std::string from_class;
    std::string to_class;
    if(domain.getComponentClassName(rec.from_long_name, channel_index, from_class).ok()
       && domain.getComponentClassName(rec.to_long_name, channel_index, to_class).ok())
    {
        ConnectTemplate t;
        t.from_class = from_class;
        t.to_class = to_class;
        t.from_port = rec.from_property;
        t.to_port = rec.to_property;
        state.session_graph.last_template = std::move(t);
    }
}

bool isEndpointLinkedInSession(const SessionGraphMemory& mem, const std::string& long_name)
{
    if(long_name.empty())
        return false;
    return std::any_of(mem.linked_records.begin(), mem.linked_records.end(),
                       [&](const ConnectRecord& rec) {
                           return rec.from_long_name == long_name || rec.to_long_name == long_name;
                       });
}

void recordWriteToolOutcome(ConversationState& state, URdkDomainAccess& domain,
                          const std::string& tool_name, const nlohmann::json& result,
                          int channel_index, const nlohmann::json* arguments)
{
    if(tool_name == "add_component")
    {
        const std::string long_name =
            result.is_object() ? result.value("long_name", "") : std::string{};
        if(!long_name.empty())
            recordSessionAdd(state, long_name);

        LastAddComponentMemory add;
        if(arguments && arguments->is_object())
        {
            add.class_name = arguments->value("class_name", "");
            add.parent_long_name = arguments->value("parent_long_name", "");
            add.short_name_base = arguments->value("short_name", "");
        }
        if(add.class_name.empty() && result.is_object())
            add.class_name = result.value("class_name", "");
        if(add.parent_long_name.empty() && result.is_object())
            add.parent_long_name = result.value("parent_long_name", "");
        if(add.short_name_base.empty() && result.is_object())
            add.short_name_base = result.value("short_name", "");
        if(!add.class_name.empty())
            state.session_graph.last_add = std::move(add);
        return;
    }
    if(tool_name == "connect_components")
        recordSessionConnect(state, domain, result, channel_index);
}

std::vector<std::string> sessionRemainingLongNames(const ConversationState& state)
{
    std::vector<std::string> out;
    for(const std::string& long_name : state.session_graph.added_long_names)
    {
        if(!isEndpointLinkedInSession(state.session_graph, long_name))
            out.push_back(long_name);
    }
    return out;
}

} // namespace RDK::LLM
