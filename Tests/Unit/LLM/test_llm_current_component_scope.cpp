#include <gtest/gtest.h>

#include "Domain/ULLMCurrentComponentScope.h"
#include "LlmTypes.h"

using namespace RDK::LLM;

TEST(LLMCurrentComponentScope, ReadDiagramScopeFromSnapshot)
{
    LLMGuiContextSnapshot gui;
    gui.diagram_scope_long_name = "Hardware/DrillLevel";
    EXPECT_EQ(readDiagramScopeLongName(&gui), "Hardware/DrillLevel");
}

TEST(LLMCurrentComponentScope, DiagramScopeEmptyAtRoot)
{
    LLMGuiContextSnapshot gui;
    gui.current_component_long_name = "Hardware";
    EXPECT_TRUE(readDiagramScopeLongName(&gui).empty());
}
