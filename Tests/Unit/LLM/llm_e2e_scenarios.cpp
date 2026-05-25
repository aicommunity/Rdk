#include "llm_e2e_scenarios.h"

#include <cstdio>
#include <sstream>
#include <unistd.h>

#include "llm_e2e_lab_harness.h"

namespace RDK::LLM::E2eLab {

namespace {

std::string e2eTempParentDir()
{
    std::ostringstream oss;
    oss << "/tmp/nmsdk_llm_e2e_" << static_cast<unsigned long>(getpid());
    return oss.str();
}

E2eScenarioDef make(const std::string& id, const std::string& msg, E2eScenarioExpectation exp,
                     bool project_loaded = false, bool auto_confirm = true)
{
    E2eScenarioDef s;
    s.id = id;
    s.user_message = msg;
    s.project_loaded = project_loaded;
    s.auto_confirm_pending = auto_confirm;
    s.expectation = std::move(exp);
    s.expectation.id = id;
    return s;
}

} // namespace

std::vector<E2eScenarioDef> allNaturalLanguageScenarios()
{
    const std::string valid_ini = sampleValidConfigurationIni();
    const std::string valid_dir = valid_ini.substr(0, valid_ini.rfind('/'));
    const std::string temp_parent = e2eTempParentDir();

    std::vector<E2eScenarioDef> scenarios;

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"list_registered_classes"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::ClassesListed;
        scenarios.push_back(make(
            "nl_list_classes_en",
            "What component classes are registered in RDK? Use tools if needed, keep the answer short.",
            exp));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"list_registered_classes"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::ClassesListed;
        scenarios.push_back(make("nl_list_classes_ru",
                                 "какие классы компонентов зарегистрированы? можно кратко",
                                 exp));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"validate_configuration"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::ValidateRan;
        scenarios.push_back(make(
            "nl_validate_fuzzy_en",
            "Could you check whether the configuration at " + valid_ini + " is valid?",
            exp));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"validate_configuration"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::ValidateRan;
        scenarios.push_back(
            make("nl_validate_fuzzy_ru", "проверь пожалуйста конфиг по пути " + valid_ini, exp));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"load_configuration", "load_project"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::LoadRequested;
        scenarios.push_back(make(
            "nl_load_fuzzy_en",
            "Open the existing sample configuration located at " + valid_dir,
            exp));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"load_configuration", "load_project"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::LoadRequested;
        scenarios.push_back(make("nl_load_fuzzy_ru",
                                 "открой конфигурацию из папки " + valid_dir,
                                 exp));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"create_configuration"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::CreateRequested;
        scenarios.push_back(make(
            "nl_create_fuzzy_en",
            std::string("I want a brand new configuration. Create it under parent directory ")
                + temp_parent
                + " with project name E2eScenarioTest and one channel class Model.",
            exp));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"create_configuration"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::CreateRequested;
        scenarios.push_back(make(
            "nl_create_fuzzy_ru",
            std::string("создай новую конфигурацию в каталоге ") + temp_parent
                + " имя проекта E2eScenarioTest один канал Model",
            exp));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"create_configuration"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::CreateRequested;
        scenarios.push_back(make("nl_create_short",
                                 "create new config in " + temp_parent + " name DemoCfg",
                                 exp));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"save_configuration", "save_project"};
        exp.forbidden_tools = {"add_component", "create_configuration"};
        exp.goal = E2eGoalKind::InformativeAnswer;
        exp.require_orchestrator_ok = true;
        scenarios.push_back(
            make("nl_save_howto_en",
                 "How do I save my open NeuroModeler configuration to disk?", exp, true, false));
    }

    {
        E2eScenarioExpectation exp;
        exp.expected_tools_any = {"describe_class", "list_registered_classes"};
        exp.forbidden_tools = {"add_component"};
        exp.goal = E2eGoalKind::ToolInvoked;
        scenarios.push_back(make(
            "nl_describe_class_fuzzy",
            "Tell me something about the Model component class — use describe_class if you can.",
            exp));
    }

    return scenarios;
}

} // namespace RDK::LLM::E2eLab
