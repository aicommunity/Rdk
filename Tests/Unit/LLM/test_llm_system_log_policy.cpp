#include <gtest/gtest.h>

#include "Observability/ULLMSystemLogPolicy.h"

#include "../../../Core/Application/UProject.h"
#include "../../../Deploy/Include/rdk_init.h"

using namespace RDK::LLM;

TEST(LLMSystemLogPolicy, DebugModeMapsToDebug)
{
    RDK::TProjectConfig cfg;
    cfg.DebugMode = true;
    const SystemLogPolicySnapshot snap = buildSystemLogPolicySnapshotFromConfig(cfg, 0);
    EXPECT_EQ(snap.project_effective_min_severity, RDK_EX_DEBUG);
    EXPECT_EQ(snap.project_effective_min_severity_name, "DEBUG");
}

TEST(LLMSystemLogPolicy, EventsOnlyMapsToInfo)
{
    RDK::TProjectConfig cfg;
    cfg.EventsLogMode = true;
    const SystemLogPolicySnapshot snap = buildSystemLogPolicySnapshotFromConfig(cfg, 0);
    EXPECT_EQ(snap.project_effective_min_severity, RDK_EX_INFO);
}

TEST(LLMSystemLogPolicy, BothOffMapsToWarning)
{
    RDK::TProjectConfig cfg;
    const SystemLogPolicySnapshot snap = buildSystemLogPolicySnapshotFromConfig(cfg, 0);
    EXPECT_EQ(snap.project_effective_min_severity, RDK_EX_WARNING);
}

TEST(LLMSystemLogPolicy, SummaryMentionsDebugDisclaimer)
{
    RDK::TProjectConfig cfg;
    cfg.DebugMode = false;
    const SystemLogPolicySnapshot snap = buildSystemLogPolicySnapshotFromConfig(cfg, 0);
    EXPECT_NE(snap.summary_for_model.find("DEBUG"), std::string::npos);
}
