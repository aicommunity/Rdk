#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Gui/ILLMPresentationSink.h"
#include "LlmPublicApi.h"
#include "Tools/RegisterDocumentationTools.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

namespace {

struct DocOpenSink : ILLMPresentationSink {
    int help_count = 0;
    int class_count = 0;
    int md_count = 0;
    std::string last_topic;
    std::string last_class;
    std::string last_path;

    void apply(const LLMPresentationEvent& ev) override { (void)ev; }

    nlohmann::json openHelpTopic(const std::string& topic) override
    {
        ++help_count;
        last_topic = topic.empty() ? "index.html" : topic;
        return {{"ok", true}, {"topic", last_topic}};
    }

    nlohmann::json openClassDescription(const std::string& class_name) override
    {
        ++class_count;
        last_class = class_name;
        return {{"ok", true}, {"class_name", class_name}};
    }

    nlohmann::json openMarkdownDocument(const std::string& abs_path,
                                        const std::string& title) override
    {
        (void)title;
        ++md_count;
        last_path = abs_path;
        return {{"ok", true}, {"path", abs_path}};
    }
};

} // namespace

TEST(LLMDocumentationTools, RegistersOpenTools)
{
    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
    RegisterDocumentationTools(registry, domain, nullptr);
    EXPECT_NE(registry.find("open_help"), nullptr);
    EXPECT_NE(registry.find("open_class_docs"), nullptr);
    EXPECT_NE(registry.find("open_documentation"), nullptr);
    EXPECT_NE(registry.find("list_help_topics"), nullptr);
}
