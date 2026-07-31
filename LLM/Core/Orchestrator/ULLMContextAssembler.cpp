#include "ULLMContextAssembler.h"

#include "../Context/URdkContextRetriever.h"
#include "ULLMAgentManifestBuilder.h"
#include "ULLMConnectPlanParsing.h"

#include <algorithm>
#include <filesystem>
#include <sstream>

namespace RDK::LLM {

namespace {

constexpr std::size_t kGuiFocusMaxChars = 1200;
constexpr std::size_t kRetrieverSummaryMaxChars = 4096;
constexpr std::size_t kAgentNotesMaxChars = 4096;
constexpr std::size_t kLongTermMemoryMaxChars = 2048;

void truncateInPlace(std::string& s, const std::size_t max_chars)
{
    if(s.size() <= max_chars)
        return;
    s.resize(max_chars);
    s += "\n...(truncated)";
}

LLMMessage makeSystemMessage(std::string content)
{
    LLMMessage msg;
    msg.role = LLMMessage::Role::System;
    msg.content = std::move(content);
    return msg;
}

void prependSystem(std::vector<LLMMessage>& messages, std::string content)
{
    if(content.empty())
        return;
    messages.insert(messages.begin(), makeSystemMessage(std::move(content)));
}

bool guiSnapshotHasData(const LLMGuiContextSnapshot& gui)
{
    return !gui.current_component_long_name.empty() || !gui.focused_component_long_name.empty()
           || !gui.diagram_scope_long_name.empty() || !gui.focused_class_name.empty()
           || !gui.project_xml_path.empty() || gui.snapshot_fingerprint != 0
           || gui.channel_index != 0;
}

} // namespace

std::size_t sumMessagePayloadChars(const std::vector<LLMMessage>& messages)
{
    std::size_t total = 0;
    for(const LLMMessage& msg : messages)
        total += msg.content.size();
    return total;
}

std::string buildGuiFocusSystemHint(const LLMGuiContextSnapshot& gui,
                                    const LLMSessionContext& session)
{
    if(!guiSnapshotHasData(gui) && !session.project_loaded)
        return {};

    auto noneOr = [](const std::string& v) { return v.empty() ? "(none)" : v; };

    std::string config_display = "(none)";
    if(!gui.project_xml_path.empty())
    {
        if(session.allow_cloud_llm)
            config_display = gui.project_xml_path;
        else
            config_display = std::filesystem::path(gui.project_xml_path).filename().string();
    }

    const std::string& kernel_current = gui.current_component_long_name.empty()
                                            ? gui.focused_component_long_name
                                            : gui.current_component_long_name;

    std::ostringstream oss;
    oss << "## Current component (kernel scope)\n"
        << "- current_component_long_name: " << noneOr(kernel_current) << "\n"
        << "- current_component_id: " << noneOr(gui.current_component_id) << "\n"
        << "- channel_index: " << gui.channel_index << "\n"
        << "- project_loaded: " << (session.project_loaded ? "true" : "false") << "\n"
        << "## GUI focus\n"
        << "- focused_component_long_name: " << noneOr(gui.focused_component_long_name) << "\n"
        << "- focused_class_name: " << noneOr(gui.focused_class_name) << "\n"
        << "- diagram_scope_long_name: " << noneOr(gui.diagram_scope_long_name) << "\n"
        << "- project_config_path: " << config_display << "\n"
        << "- diagram_snapshot_fingerprint: " << gui.snapshot_fingerprint << "\n"
        << "All model mutations without explicit paths use engine CurrentComponent (same as "
           "NeuroModeler UI). add_component: default parent = diagram_scope_long_name when "
           "drilled into a container, else current_component_long_name. "
           "connect/remove/set_property: resolve short names under current subtree first.\n";

    std::string out = oss.str();
    truncateInPlace(out, kGuiFocusMaxChars);
    return out;
}

static std::string buildRetrieverSummaryBlock(const EphemeralContextInput& input)
{
    if(!input.context_retriever || !input.session.project_loaded)
        return {};
    // Open project is enough: inject a live net snapshot so Query about "current model"
    // does not require the model to guess get_net_snapshot.
    const std::string& root_scope = input.gui.diagram_scope_long_name;
    const nlohmann::json summary = input.context_retriever->buildSummary(
        input.session.active_channel_index, input.gui.focused_class_name, root_scope);
    if(summary.empty())
        return {};

    std::string block =
        "## Project context snapshot (live net; prefer this over guessing tools)\n";
    if(!root_scope.empty())
        block += "- root_long_name (diagram drill): " + root_scope + "\n";
    block += summary.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
    truncateInPlace(block, kRetrieverSummaryMaxChars);
    return block;
}

void prependEphemeralSystemMessages(std::vector<LLMMessage>& provider_messages,
                                    const EphemeralContextInput& input,
                                    LLMContextBudget* out_budget)
{
    const std::size_t messages_chars = sumMessagePayloadChars(provider_messages);

    if(!input.provider_tools)
    {
        if(out_budget)
        {
            out_budget->messages_chars = messages_chars;
            out_budget->ephemeral_chars = 0;
            out_budget->manifest_chars = 0;
        }
        return;
    }

    // Insert order 1..8 per plan: each insert(begin) — last insert ends up closest to user messages.
    std::string manifest_content;
    std::size_t manifest_chars = 0;
    if(input.registry)
    {
        manifest_content = buildAgentManifest(*input.registry, input.tool_filter, 6000,
                                              input.planning_text, input.system_log_summary,
                                              input.response_language);
        manifest_chars = manifest_content.size();
    }

    // Prepend chain: first call ends up nearest user messages; last call is outermost system block.
    prependSystem(provider_messages, std::move(manifest_content));

    if(!input.state.agent_notes.empty())
    {
        std::string notes = "## Agent notes\n";
        notes += input.state.agent_notes;
        truncateInPlace(notes, kAgentNotesMaxChars);
        prependSystem(provider_messages, std::move(notes));
    }

    if(input.state.last_quantity.valid)
    {
        prependSystem(provider_messages,
                      "## Resolved quantity (this turn)\nprimary=" + std::to_string(input.state.last_quantity.primary));
    }
    if(!input.state.session_graph.added_long_names.empty() || input.state.session_graph.last_add)
    {
        std::ostringstream sg;
        sg << "## Session graph memory\n"
           << "- added_count: " << input.state.session_graph.added_long_names.size() << "\n"
           << "- linked_count: " << input.state.session_graph.linked_records.size() << "\n";
        if(input.state.session_graph.last_add
           && !input.state.session_graph.last_add->class_name.empty())
        {
            sg << "- last_added_class: " << input.state.session_graph.last_add->class_name << "\n";
            if(!input.state.session_graph.last_add->parent_long_name.empty())
                sg << "- last_added_parent: " << input.state.session_graph.last_add->parent_long_name
                   << "\n";
            if(!input.state.session_graph.last_add->short_name_base.empty())
                sg << "- last_added_short_name: "
                   << input.state.session_graph.last_add->short_name_base << "\n";
            sg << "- continuity_rule: If the user asks for more of the same "
                  "(ещё/таких же/same/another/more) or uses a continuer noun "
                  "(нейрон/neuron/компонент/component/блок/block/модуль/module) and "
                  "last_added_class is set, use that class_name for add_component; "
                  "do not ask which class.\n";
        }
        const auto& added = input.state.session_graph.added_long_names;
        if(!added.empty())
        {
            sg << "- recent_added:";
            const std::size_t n = std::min<std::size_t>(added.size(), 5);
            for(std::size_t i = added.size() - n; i < added.size(); ++i)
                sg << " `" << added[i] << "`";
            sg << "\n";
        }
        prependSystem(provider_messages, sg.str());
    }

    if(!input.connect_semantics_block.empty())
        prependSystem(provider_messages, input.connect_semantics_block);
    if(!input.link_patterns_block.empty())
        prependSystem(provider_messages, input.link_patterns_block);

    prependSystem(provider_messages, buildRetrieverSummaryBlock(input));
    prependSystem(provider_messages, buildGuiFocusSystemHint(input.gui, input.session));

    if(input.state.session_summary && !input.state.session_summary->empty())
        prependSystem(provider_messages, "## Session summary\n" + *input.state.session_summary);

    if(!input.long_term_memory_block.empty())
    {
        std::string mem = input.long_term_memory_block;
        truncateInPlace(mem, kLongTermMemoryMaxChars);
        prependSystem(provider_messages, std::move(mem));
    }

    if(!input.prefetched_docs_block.empty())
        prependSystem(provider_messages, input.prefetched_docs_block);

    if(input.lifecycle_action != ConfigurationLifecycleAction::None)
    {
        prependSystem(provider_messages,
                      configurationLifecycleSystemHint(input.lifecycle_action,
                                                       input.session.project_loaded));
    }

    if(input.intent == LLMIntentKind::Query || input.intent == LLMIntentKind::Explain)
    {
        std::string query_hint =
            "## Inspect live project vs docs\n"
            "- Live graph / current model / components / diagram / схема: use "
            "get_net_snapshot (channel_index from GUI focus; if diagram_scope_long_name is set, "
            "pass root_long_name=that scope). Prefer the Project context snapshot block when "
            "present instead of re-calling the tool.\n"
            "- Selected component: find_component / get_component_properties.\n"
            "- Class metadata / ClDesc: describe_class / list_registered_classes.\n"
            "- How-to / product docs: search_project_docs(scope=docs). Cite source_id and path.\n"
            "- Recent or disk configurations: list_recent_configurations — not for live graph.\n"
            "- Do not confuse \"model\" (net graph) with a configuration file or channel.\n"
            "- Do not call write tools.";
        if(!input.prefetched_docs_block.empty())
            query_hint +=
                " Prefetched doc excerpts may appear below; call search_project_docs for more.";
        prependSystem(provider_messages, std::move(query_hint));
    }

    if(input.intent == LLMIntentKind::Plan)
    {
        const std::string plan_hint =
            input.strict_plan_schema
                ? "Plan-only mode: use read tools to inspect state, then respond with JSON "
                  "matching the execution_plan schema (no markdown)."
                : "Plan-only mode: use read tools to inspect state, then reply with a JSON "
                  "execution plan in a ```json code block. Do not mutate until the user confirms.";
        prependSystem(provider_messages, plan_hint);
    }

    if(out_budget)
    {
        const std::size_t total_chars = sumMessagePayloadChars(provider_messages);
        out_budget->messages_chars = messages_chars;
        out_budget->ephemeral_chars =
            total_chars > messages_chars ? total_chars - messages_chars : 0;
        out_budget->manifest_chars = manifest_chars;
    }
}

} // namespace RDK::LLM
