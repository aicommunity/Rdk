#include <gtest/gtest.h>

#include "Gui/ILLMPresentationSink.h"
#include "Gui/ULLMNoopPresentationSink.h"
#include "Tools/ApplicationToolAudit.h"
#include "Tools/ApplicationToolHelpers.h"

using namespace RDK::LLM;

TEST(LLMPresentationSink, NoopApplyIsSafe)
{
    ULLMNoopPresentationSink sink;
    LLMPresentationEvent ev;
    ev.effect = LLMPresentationEffect::FullShellRefresh;
    sink.apply(ev);
    SUCCEED();
}

TEST(LLMPresentationSink, CommandMapsToGateway)
{
    ApplicationCommandResult cmd;
    cmd.status = {};
    cmd.payload = {{"ok", true}};
    ToolGatewayResult gw = commandResultToGateway(cmd);
    EXPECT_TRUE(gw.ok);
}

TEST(LLMPresentationSink, LoadLikeCommandTriggersFullShellSink)
{
    struct CountingSink : ILLMPresentationSink {
        int count = 0;
        LLMPresentationEffect last = LLMPresentationEffect::None;
        void apply(const LLMPresentationEvent& ev) override
        {
            ++count;
            last = ev.effect;
        }
    } sink;

    ApplicationCommandResult cmd;
    cmd.status = {};
    cmd.presentation = LLMPresentationEffect::FullShellRefresh;
    cmd.update_context = true;
    cmd.resolved_configuration_path = "/tmp/demo/project.ini";
    invokeApplicationTool(&sink, [&]() { return cmd; });
    EXPECT_EQ(sink.count, 1);
    EXPECT_EQ(sink.last, LLMPresentationEffect::FullShellRefresh);
}

TEST(LLMPresentationSink, ValidateLikeCommandSkipsSink)
{
    struct CountingSink : ILLMPresentationSink {
        int count = 0;
        void apply(const LLMPresentationEvent& ev) override
        {
            ++count;
            (void)ev;
        }
    } sink;

    ApplicationCommandResult cmd;
    cmd.status = {};
    cmd.presentation = LLMPresentationEffect::None;
    ToolGatewayResult gw = invokeApplicationTool(&sink, [&]() { return cmd; });
    EXPECT_TRUE(gw.ok);
    EXPECT_EQ(sink.count, 0);
}

TEST(LLMPresentationSink, ShowPanelTriggersSinkEvenWithoutEffect)
{
    struct CountingSink : ILLMPresentationSink {
        int count = 0;
        LLMPresentationEvent last_event;
        void apply(const LLMPresentationEvent& ev) override
        {
            ++count;
            last_event = ev;
        }
    } sink;

    ApplicationCommandResult cmd;
    cmd.status = {};
    cmd.presentation = LLMPresentationEffect::None;
    cmd.show_panel = LLMUiPanel::Logger;
    cmd.show_panel_visible = true;

    invokeApplicationTool(&sink, [&]() { return cmd; });
    EXPECT_EQ(sink.count, 1);
    EXPECT_EQ(sink.last_event.show_panel, LLMUiPanel::Logger);
}

TEST(LLMPresentationSink, AuditFieldsAttached)
{
    ApplicationCommandResult cmd;
    cmd.status = {};
    cmd.resolved_configuration_path = "/data/cfg/project.ini";
    cmd.presentation = LLMPresentationEffect::DiagramRefresh;
    ToolGatewayResult gw = commandResultToGateway(cmd);
    attachApplicationToolAuditFields(gw, cmd);
    EXPECT_EQ(gw.result[kAuditConfigurationPathKey], "/data/cfg/project.ini");
    EXPECT_EQ(gw.result[kAuditPresentationEffectKey], "DiagramRefresh");
}
