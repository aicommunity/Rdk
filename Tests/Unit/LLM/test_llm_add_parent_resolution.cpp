#include <gtest/gtest.h>

#include "Domain/ULLMAddParentResolution.h"
#include "Domain/ULLMCurrentComponentScope.h"

using namespace RDK::LLM;

TEST(LLMAddParentResolution, PrefersDiagramScopeOverRootModel)
{
    URdkDomainAccess domain(nullptr);
    LLMGuiContextSnapshot pin;
    pin.diagram_scope_long_name = "Model.ZoneA";
    pin.current_component_long_name = "Model";

    const AddParentResolution res =
        resolveValidAddParent(domain, "Model", "NSPNeuron", 0, pin);
    EXPECT_TRUE(res.ok);
    EXPECT_FALSE(res.needs_clarification);
    EXPECT_EQ(res.parent_long_name, "Model.ZoneA");
}

TEST(LLMAddParentResolution, DrilledContainerNamedModelIsNotRoot)
{
    URdkDomainAccess domain(nullptr);
    LLMGuiContextSnapshot pin;
    pin.diagram_scope_long_name = "Model";
    pin.current_component_long_name = "Model";

    const AddParentResolution res = resolveValidAddParent(domain, "", "NSPNeuron", 0, pin);
    EXPECT_TRUE(res.ok);
    EXPECT_FALSE(res.needs_clarification);
    EXPECT_EQ(res.parent_long_name, "Model");
}

TEST(LLMAddParentResolution, EmptyParentAtRootDefaultsToEmptyWithoutClarification)
{
    URdkDomainAccess domain(nullptr);
    LLMGuiContextSnapshot pin;
    // Root view: empty diagram scope → engine root (empty parent_long_name).
    pin.current_component_long_name = "Model";

    const AddParentResolution res = resolveValidAddParent(domain, "", "NSPNeuronGen", 0, pin);
    EXPECT_TRUE(res.ok);
    EXPECT_FALSE(res.needs_clarification);
    EXPECT_TRUE(res.candidates.empty());
    EXPECT_EQ(res.parent_long_name, "");
}

TEST(LLMAddParentResolution, ModelHintAtRootDefaultsToEmptyWithoutClarification)
{
    URdkDomainAccess domain(nullptr);
    LLMGuiContextSnapshot pin;

    const AddParentResolution res =
        resolveValidAddParent(domain, "Model", "NSPNeuronGen", 0, pin);
    EXPECT_TRUE(res.ok);
    EXPECT_FALSE(res.needs_clarification);
    EXPECT_EQ(res.parent_long_name, "");
}

TEST(LLMAddParentResolution, EmptyParentUsesCurrentNonRootComponent)
{
    URdkDomainAccess domain(nullptr);
    LLMGuiContextSnapshot pin;
    pin.current_component_long_name = "PNeuron";

    const AddParentResolution res = resolveValidAddParent(domain, "", "NSPNeuronGen", 0, pin);
    EXPECT_TRUE(res.ok);
    EXPECT_FALSE(res.needs_clarification);
    EXPECT_EQ(res.parent_long_name, "PNeuron");
}

TEST(LLMAddParentResolution, ExplicitUnresolvedHintKeptAsIs)
{
    URdkDomainAccess domain(nullptr);
    LLMGuiContextSnapshot pin;

    const AddParentResolution res =
        resolveValidAddParent(domain, "Some.Explicit.Parent", "NSPNeuron", 0, pin);
    EXPECT_TRUE(res.ok);
    EXPECT_FALSE(res.needs_clarification);
    EXPECT_EQ(res.parent_long_name, "Some.Explicit.Parent");
}

TEST(LLMAddParentResolution, EngineParentPathIsCanonicalFullPath)
{
    LLMGuiContextSnapshot pin;
    pin.diagram_scope_long_name = "PGenerator";
    EXPECT_EQ(engineContainerStringId("PGenerator", &pin), "PGenerator");
    EXPECT_EQ(engineContainerStringId("PGenerator.ZoneA", &pin), "PGenerator.ZoneA");
}

TEST(LLMCurrentComponentScope, ModelTokenAtRootVsDrill)
{
    LLMGuiContextSnapshot root;
    EXPECT_TRUE(isAtRootDiagramView(&root));
    EXPECT_TRUE(isModelRootContainerToken("Model", &root));

    LLMGuiContextSnapshot drilled;
    drilled.diagram_scope_long_name = "Model";
    EXPECT_FALSE(isAtRootDiagramView(&drilled));
    EXPECT_FALSE(isModelRootContainerToken("Model", &drilled));
}
