#include <gtest/gtest.h>
#include "../../../Deploy/Include/rdk.h"
#include <memory>

using namespace RDK;

// Test class for property connections
class TestComponent : public UNet
{
public:
    UProperty<int, TestComponent, ptPubOutput> Output;
    UProperty<int, TestComponent, ptPubInput> Input;
    UProperty<int, TestComponent, ptPubParameter> Parameter;

    TestComponent() : 
        Output("Output", this),
        Input("Input", this),
        Parameter("Parameter", this)
    {
    }
};

class UPropertyConnectionsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        comp1 = std::make_unique<TestComponent>();
        comp2 = std::make_unique<TestComponent>();
    }

    void TearDown() override
    {
        comp1.reset();
        comp2.reset();
    }

    std::unique_ptr<TestComponent> comp1;
    std::unique_ptr<TestComponent> comp2;
};

// Test connecting output to input
TEST_F(UPropertyConnectionsTest, ConnectOutputToInput)
{
    comp1->Output = 42;
    comp2->Input.Connect(&comp1->Output);
    
    EXPECT_EQ(comp2->Input(), 42);
    EXPECT_TRUE(comp2->Input.IsConnected());
}

// Test disconnecting properties
TEST_F(UPropertyConnectionsTest, DisconnectProperties)
{
    comp1->Output = 100;
    comp2->Input.Connect(&comp1->Output);
    EXPECT_TRUE(comp2->Input.IsConnected());
    
    comp2->Input.DetachFrom(&comp1->Output);
    EXPECT_FALSE(comp2->Input.IsConnected());
}

// Test multiple connections to same output
TEST_F(UPropertyConnectionsTest, MultipleConnectionsToOutput)
{
    auto comp3 = std::make_unique<TestComponent>();
    
    comp1->Output = 50;
    comp2->Input.Connect(&comp1->Output);
    comp3->Input.Connect(&comp1->Output);
    
    EXPECT_EQ(comp2->Input(), 50);
    EXPECT_EQ(comp3->Input(), 50);
    EXPECT_TRUE(comp2->Input.IsConnected());
    EXPECT_TRUE(comp3->Input.IsConnected());
}

// Test data propagation through connection
TEST_F(UPropertyConnectionsTest, DataPropagation)
{
    comp1->Output = 10;
    comp2->Input.Connect(&comp1->Output);
    
    EXPECT_EQ(comp2->Input(), 10);
    
    comp1->Output = 20;
    EXPECT_EQ(comp2->Input(), 20);
}

// Test connection chain
TEST_F(UPropertyConnectionsTest, ConnectionChain)
{
    auto comp3 = std::make_unique<TestComponent>();
    
    comp1->Output = 30;
    comp2->Input.Connect(&comp1->Output);
    comp3->Input.Connect(&comp2->Output);
    
    // Note: comp2->Output needs to be set explicitly or connected
    comp2->Output = comp2->Input();
    EXPECT_EQ(comp3->Input(), 30);
}

// Test that disconnected property uses local value
TEST_F(UPropertyConnectionsTest, DisconnectedUsesLocalValue)
{
    comp2->Input = 99;
    EXPECT_FALSE(comp2->Input.IsConnected());
    EXPECT_EQ(comp2->Input(), 99);
    
    comp1->Output = 77;
    comp2->Input.Connect(&comp1->Output);
    EXPECT_EQ(comp2->Input(), 77);
    
    comp2->Input.DetachFrom(&comp1->Output);
    comp2->Input = 88;
    EXPECT_EQ(comp2->Input(), 88);
}

