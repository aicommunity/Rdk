#include <gtest/gtest.h>

#include "Domain/ULLMAddParentResolution.h"

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
    EXPECT_EQ(res.parent_long_name, "Model.ZoneA");
}
