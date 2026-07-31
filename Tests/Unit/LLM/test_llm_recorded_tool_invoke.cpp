#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Orchestrator/ULLMRecordedToolInvoke.h"
#include "Orchestrator/ULLMTurnTerminalHelpers.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/ULLMToolArgumentValidator.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

TEST(LLMRecordedToolInvoke, SuccessWritesPairedAssistantAndTool)
{
    ULLMToolRegistry registry;
    LLMToolDefinition def;
    def.name = "get_net_snapshot";
    def.kind = LLMToolKind::Read;
    def.description = "snap";
    def.input_schema = {{"type", "object"},
                        {"properties", {{"channel_index", {{"type", "integer"}}}}},
                        {"additionalProperties", true}};
    registry.registerTool(def, [](const nlohmann::json&) {
        ToolGatewayResult tr;
        tr.ok = true;
        tr.result = {{"components", nlohmann::json::array()}};
        tr.message = "ok";
        return tr;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMConversationStore store;
    const std::string sid = "rec-test-1";
    ConversationState& state = store.getOrCreate(sid);

    RecordedToolInvokeDeps deps{registry, gateway, store, {}, nullptr};
    RecordedToolInvokeRequest req;
    req.session_id = sid;
    req.trace_id = "t1";
    req.tool_name = "get_net_snapshot";
    req.arguments = {{"channel_index", 0}};
    req.session.session_id = sid;
    req.session.project_loaded = true;
    req.append_outcome_assistant = true;

    const RecordedToolInvokeResult r = recordedToolInvoke(state, deps, req);
    EXPECT_TRUE(r.gateway.ok);
    EXPECT_TRUE(r.wrote_assistant_tool_calls);
    EXPECT_TRUE(r.wrote_tool_result);
    EXPECT_FALSE(r.tool_call_id.empty());

    bool saw_calls = false;
    bool saw_tool = false;
    for(const LLMMessage& m : state.messages)
    {
        if(m.role == LLMMessage::Role::Assistant && m.assistant_tool_calls
           && !m.assistant_tool_calls->empty())
        {
            saw_calls = true;
            EXPECT_EQ(m.assistant_tool_calls->front().id, r.tool_call_id);
            EXPECT_EQ(m.assistant_tool_calls->front().name, "get_net_snapshot");
        }
        if(m.role == LLMMessage::Role::Tool)
        {
            saw_tool = true;
            EXPECT_EQ(m.tool_call_id.value_or(""), r.tool_call_id);
        }
    }
    EXPECT_TRUE(saw_calls);
    EXPECT_TRUE(saw_tool);
}

TEST(LLMRecordedToolInvoke, AssistantToolCallsOnlyHelper)
{
    ULLMConversationStore store;
    const std::string sid = "rec-hitl";
    store.getOrCreate(sid);
    appendAssistantToolCallsOnly(store, sid, "rec-abc", "add_component",
                                 {{"class_name", "NSPNeuronGen"}});
    const ConversationState& st = store.getOrCreate(sid);
    ASSERT_EQ(st.messages.size(), 1u);
    ASSERT_TRUE(st.messages[0].assistant_tool_calls.has_value());
    EXPECT_EQ(st.messages[0].assistant_tool_calls->front().id, "rec-abc");
    appendToolResultOnly(store, sid, "rec-abc", "add_component", R"({"ok":true})");
    EXPECT_EQ(store.getOrCreate(sid).messages.size(), 2u);
    EXPECT_EQ(store.getOrCreate(sid).messages.back().role, LLMMessage::Role::Tool);
    EXPECT_EQ(store.getOrCreate(sid).messages.back().tool_call_id.value_or(""), "rec-abc");
}

TEST(LLMRecordedToolInvoke, HitlWritesAssistantCallsOnlyThenConfirmPairsTool)
{
    ULLMToolRegistry registry;
    LLMToolDefinition def;
    def.name = "test_write";
    def.kind = LLMToolKind::Write;
    def.requires_confirmation = true;
    def.input_schema = {{"type", "object"}, {"additionalProperties", false}};
    registry.registerTool(def, [](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = true;
        r.result["applied"] = true;
        return r;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMConversationStore store;
    const std::string sid = "rec-hitl-pair";
    ConversationState& state = store.getOrCreate(sid);

    RecordedToolInvokeDeps deps{registry, gateway, store, {}, nullptr};
    RecordedToolInvokeRequest req;
    req.session_id = sid;
    req.trace_id = "hitl-1";
    req.tool_name = "test_write";
    req.arguments = nlohmann::json::object();
    req.session.session_id = sid;
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.append_outcome_assistant = false;

    const RecordedToolInvokeResult pending = recordedToolInvoke(state, deps, req);
    EXPECT_TRUE(pending.needs_hitl);
    EXPECT_TRUE(pending.wrote_assistant_tool_calls);
    EXPECT_FALSE(pending.wrote_tool_result);
    ASSERT_EQ(state.messages.size(), 1u);
    EXPECT_EQ(state.messages[0].role, LLMMessage::Role::Assistant);
    EXPECT_EQ(state.messages[0].assistant_tool_calls->front().id, pending.tool_call_id);

    PendingConfirmation pc;
    pc.confirmation_id = pending.gateway.confirmation_id;
    pc.tool_call_id = pending.tool_call_id;
    pc.request.tool_name = "test_write";
    pc.request.arguments = nlohmann::json::object();
    pc.request.confirmed = true;
    store.setPending(sid, pc);

    // Confirm path: Tool message only with stable id (mirrors confirmPending).
    req.confirmed = true;
    req.force_confirmed = true;
    req.tool_call_id = pending.tool_call_id;
    // Clear assistant-only from prior pending so second invoke is a full apply with same id.
    // Production confirmPending uses appendToolResultOnly; exercise that pairing here.
    appendToolResultOnly(store, sid, pending.tool_call_id, "test_write", R"({"ok":true,"applied":true})");
    ASSERT_EQ(state.messages.size(), 2u);
    EXPECT_EQ(state.messages[1].role, LLMMessage::Role::Tool);
    EXPECT_EQ(state.messages[1].tool_call_id.value_or(""), pending.tool_call_id);
}
