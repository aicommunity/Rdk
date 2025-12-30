#include <gtest/gtest.h>
#include "../../../Deploy/Include/rdk.h"
#include <memory>

using namespace RDK;

// Test component with multiple properties
class TestComponentWithProps : public UNet
{
public:
    UProperty<int, TestComponentWithProps, ptPubParameter> Param1;
    UProperty<double, TestComponentWithProps, ptPubParameter> Param2;
    UProperty<std::string, TestComponentWithProps, ptPubParameter> Param3;
    UProperty<int, TestComponentWithProps, ptPubState> State1;
    UProperty<int, TestComponentWithProps, ptPubOutput> Output1;
    UProperty<int, TestComponentWithProps, ptPubInput> Input1;

    TestComponentWithProps() : 
        Param1("Param1", this),
        Param2("Param2", this),
        Param3("Param3", this),
        State1("State1", this),
        Output1("Output1", this),
        Input1("Input1", this)
    {
    }
};

class UComponentPropertiesTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        comp = std::make_unique<TestComponentWithProps>();
    }

    void TearDown() override
    {
        comp.reset();
    }

    std::unique_ptr<TestComponentWithProps> comp;
};

// Test FindProperty by name
TEST_F(UComponentPropertiesTest, FindPropertyByName)
{
    auto prop1 = comp->FindProperty("Param1");
    EXPECT_NE(prop1, nullptr);
    
    auto prop2 = comp->FindProperty("Param2");
    EXPECT_NE(prop2, nullptr);
    
    auto prop3 = comp->FindProperty("NonExistent");
    EXPECT_EQ(prop3, nullptr);
}

// Test FindProperty caching
TEST_F(UComponentPropertiesTest, FindPropertyCaching)
{
    // First call should populate cache
    auto prop1 = comp->FindProperty("Param1");
    EXPECT_NE(prop1, nullptr);
    
    // Second call should use cache (same pointer)
    auto prop2 = comp->FindProperty("Param1");
    EXPECT_EQ(prop1, prop2);
}

// Test FindProperty with different property types
TEST_F(UComponentPropertiesTest, FindPropertyDifferentTypes)
{
    auto param = comp->FindProperty("Param1");
    auto state = comp->FindProperty("State1");
    auto output = comp->FindProperty("Output1");
    auto input = comp->FindProperty("Input1");
    
    EXPECT_NE(param, nullptr);
    EXPECT_NE(state, nullptr);
    EXPECT_NE(output, nullptr);
    EXPECT_NE(input, nullptr);
}

// Test GetPropertyType
TEST_F(UComponentPropertiesTest, GetPropertyType)
{
    auto param = comp->FindProperty("Param1");
    auto state = comp->FindProperty("State1");
    auto output = comp->FindProperty("Output1");
    auto input = comp->FindProperty("Input1");
    
    EXPECT_EQ(param->GetPropertyType(), ptPubParameter);
    EXPECT_EQ(state->GetPropertyType(), ptPubState);
    EXPECT_EQ(output->GetPropertyType(), ptPubOutput);
    EXPECT_EQ(input->GetPropertyType(), ptPubInput);
}

// Test PropertiesLookupTable
TEST_F(UComponentPropertiesTest, PropertiesLookupTable)
{
    // All properties should be in lookup table
    EXPECT_NE(comp->FindProperty("Param1"), nullptr);
    EXPECT_NE(comp->FindProperty("Param2"), nullptr);
    EXPECT_NE(comp->FindProperty("Param3"), nullptr);
    EXPECT_NE(comp->FindProperty("State1"), nullptr);
    EXPECT_NE(comp->FindProperty("Output1"), nullptr);
    EXPECT_NE(comp->FindProperty("Input1"), nullptr);
}

// Test property access through component
TEST_F(UComponentPropertiesTest, PropertyAccess)
{
    comp->Param1 = 42;
    comp->Param2 = 3.14;
    comp->Param3 = "test";
    
    EXPECT_EQ(comp->Param1(), 42);
    EXPECT_DOUBLE_EQ(comp->Param2(), 3.14);
    EXPECT_EQ(comp->Param3(), "test");
}

// Test multiple FindProperty calls performance
TEST_F(UComponentPropertiesTest, FindPropertyPerformance)
{
    const int iterations = 10000;
    
    for (int i = 0; i < iterations; ++i)
    {
        auto prop = comp->FindProperty("Param1");
        EXPECT_NE(prop, nullptr);
    }
    
    // With caching, this should be fast
    // Actual performance test is in Performance test file
}

// Test FindProperty after property modification
TEST_F(UComponentPropertiesTest, FindPropertyAfterModification)
{
    comp->Param1 = 100;
    auto prop1 = comp->FindProperty("Param1");
    EXPECT_NE(prop1, nullptr);
    
    comp->Param1 = 200;
    auto prop2 = comp->FindProperty("Param1");
    EXPECT_EQ(prop1, prop2); // Same property object
    EXPECT_EQ(comp->Param1(), 200);
}

// Test property names
TEST_F(UComponentPropertiesTest, PropertyNames)
{
    auto prop1 = comp->FindProperty("Param1");
    auto prop2 = comp->FindProperty("Param2");
    
    EXPECT_NE(prop1, nullptr);
    EXPECT_NE(prop2, nullptr);
    EXPECT_NE(prop1, prop2);
}

// Test invalid property name
TEST_F(UComponentPropertiesTest, InvalidPropertyName)
{
    auto prop = comp->FindProperty("InvalidPropertyName");
    EXPECT_EQ(prop, nullptr);
    
    auto prop2 = comp->FindProperty("");
    EXPECT_EQ(prop2, nullptr);
}

