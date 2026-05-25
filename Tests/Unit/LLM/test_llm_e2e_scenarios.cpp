#include <gtest/gtest.h>

#include <cstdlib>
#include <string>

#include "llm_e2e_lab_harness.h"
#include "llm_e2e_scenarios.h"

using namespace RDK::LLM;
using namespace RDK::LLM::E2eLab;

namespace {

class E2eNaturalLanguage : public ::testing::TestWithParam<E2eScenarioDef> {
protected:
    void SetUp() override
    {
        skipIfLabOllamaUnreachable();
        setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);
    }
};

std::string formatTools(const E2eConversationDigest& d)
{
    std::string s;
    for(size_t i = 0; i < d.tools_invoked.size(); ++i)
    {
        if(i)
            s += ", ";
        s += d.tools_invoked[i];
    }
    return s.empty() ? "(none)" : s;
}

} // namespace

TEST_P(E2eNaturalLanguage, ScenarioReachesGoal)
{
    const E2eScenarioDef scenario = GetParam();
    Harness h;
    LLMSessionContext session = defaultSession(scenario.project_loaded);

    const E2eScenarioRun run =
        runNaturalLanguageScenario(h, scenario.id, scenario.user_message, session,
                                 scenario.auto_confirm_pending);

    std::string skip_reason;
    if(shouldSkipForProviderError(run.final_response, &skip_reason))
        GTEST_SKIP() << scenario.id << ": " << skip_reason;

    const E2eScenarioEvaluation eval = evaluateScenario(run.digest, scenario.expectation);

    EXPECT_TRUE(eval.passed) << "[" << scenario.id << "] " << eval.diagnosis << "\nMessage: "
                             << scenario.user_message << "\nTools: " << formatTools(run.digest)
                             << "\nAssistant: " << run.digest.assistant_text.substr(0, 400);
}

INSTANTIATE_TEST_SUITE_P(
    LabOllama,
    E2eNaturalLanguage,
    ::testing::ValuesIn(allNaturalLanguageScenarios()),
    [](const ::testing::TestParamInfo<E2eScenarioDef>& info) { return info.param.id; });

TEST(E2eAnalyzerUnit, DetectsValidateDiagnostics)
{
    E2eConversationDigest d;
    d.orchestrator_ok = true;
    d.tools_invoked = {"validate_configuration"};
    d.tool_results_by_name["validate_configuration"] = {{"is_valid", true}, {"errors", nlohmann::json::array()}};

    E2eScenarioExpectation exp;
    exp.expected_tools_any = {"validate_configuration"};
    exp.goal = E2eGoalKind::ValidateRan;

    const E2eScenarioEvaluation ev = evaluateScenario(d, exp);
    EXPECT_TRUE(ev.passed) << ev.diagnosis;
}

TEST(E2eAnalyzerUnit, RejectsWrongToolForCreate)
{
    E2eConversationDigest d;
    d.orchestrator_ok = true;
    d.tools_invoked = {"add_component"};

    E2eScenarioExpectation exp;
    exp.expected_tools_any = {"create_configuration"};
    exp.forbidden_tools = {"add_component"};
    exp.goal = E2eGoalKind::CreateRequested;

    const E2eScenarioEvaluation ev = evaluateScenario(d, exp);
    EXPECT_FALSE(ev.passed);
}
