#include "ULLMContextAssembler.h"

#include "../Context/URdkContextRetriever.h"
#include "ULLMAgentManifestBuilder.h"
#include "ULLMConnectPlanParsing.h"

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
           "NeuroModeler UI). add_component: default parent = current long_name (child). "
           "connect/remove/set_property: resolve short names under current subtree first.\n";

    std::string out = oss.str();
    truncateInPlace(out, kGuiFocusMaxChars);
    return out;
}

static std::string buildRetrieverSummaryBlock(const EphemeralContextInput& input)
{
    if(!input.context_retriever || !input.session.project_loaded)
        return {};
    const bool has_focus = !input.gui.focused_class_name.empty()
                           || !input.gui.focused_component_long_name.empty();
    const bool has_diagram_scope = !input.gui.diagram_scope_long_name.empty();
    if(!has_focus && !has_diagram_scope && !input.allow_retriever_without_list_focus)
        return {};

    const nlohmann::json summary =
        input.context_retriever->buildSummary(input.session.active_channel_index,
                                              input.gui.focused_class_name);
    if(summary.empty())
        return {};

    std::string block = "## Project context snapshot\n";
    block += summary.dump(2);
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
    if(!input.state.session_graph.added_long_names.empty())
    {
        std::ostringstream sg;
        sg << "## Session graph memory\n"
           << "- added_count: " << input.state.session_graph.added_long_names.size() << "\n"
           << "- linked_count: " << input.state.session_graph.linked_records.size() << "\n";
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

    if(input.intent == LLMIntentKind::Query)
    {
        std::string query_hint =
            "Use search_project_docs(scope=docs) and describe_class. Cite source_id and path. "
            "Do not call write tools.";
        if(!input.prefetched_docs_block.empty())
            query_hint +=
                " Prefetched excerpts may appear below; you may still call search_project_docs for "
                "more.";
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
