#include <gtest/gtest.h>
#include "../../../Deploy/Include/rdk.h"
#include <memory>

using namespace RDK;

// Test component with lifecycle methods
class LifecycleTestComponent : public UNet
{
public:
    bool defaultCalled;
    bool buildCalled;
    bool resetCalled;
    bool calculateCalled;
    
    UProperty<int, LifecycleTestComponent, ptPubState> Counter;

    LifecycleTestComponent() : 
        Counter("Counter", this),
        defaultCalled(false),
        buildCalled(false),
        resetCalled(false),
        calculateCalled(false)
    {
    }

protected:
    virtual bool ADefault(void) override
    {
        defaultCalled = true;
        Counter = 0;
        return true;
    }

    virtual bool ABuild(void) override
    {
        buildCalled = true;
        return true;
    }

    virtual bool AReset(void) override
    {
        resetCalled = true;
        Counter = 0;
        return true;
    }

    virtual bool ACalculate(void) override
    {
        calculateCalled = true;
        Counter = Counter() + 1;
        return true;
    }
};

class UComponentLifecycleTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        comp = std::make_unique<LifecycleTestComponent>();
    }

    void TearDown() override
    {
        comp.reset();
    }

    std::unique_ptr<LifecycleTestComponent> comp;
};

// Test Default lifecycle
TEST_F(UComponentLifecycleTest, DefaultLifecycle)
{
    EXPECT_FALSE(comp->defaultCalled);
    EXPECT_FALSE(comp->IsReady());
    
    comp->Default();
    
    EXPECT_TRUE(comp->defaultCalled);
    EXPECT_EQ(comp->Counter(), 0);
}

// Test Build lifecycle
TEST_F(UComponentLifecycleTest, BuildLifecycle)
{
    comp->Default();
    EXPECT_FALSE(comp->buildCalled);
    
    comp->Build();
    
    EXPECT_TRUE(comp->buildCalled);
    EXPECT_TRUE(comp->IsReady());
}

// Test Reset lifecycle
TEST_F(UComponentLifecycleTest, ResetLifecycle)
{
    comp->Default();
    comp->Build();
    comp->Counter = 10;
    
    EXPECT_FALSE(comp->resetCalled);
    EXPECT_EQ(comp->Counter(), 10);
    
    comp->Reset();
    
    EXPECT_TRUE(comp->resetCalled);
    EXPECT_EQ(comp->Counter(), 0);
}

// Test Calculate lifecycle
TEST_F(UComponentLifecycleTest, CalculateLifecycle)
{
    comp->Default();
    comp->Build();
    comp->Reset();
    
    EXPECT_FALSE(comp->calculateCalled);
    EXPECT_EQ(comp->Counter(), 0);
    
    comp->Calculate();
    
    EXPECT_TRUE(comp->calculateCalled);
    EXPECT_EQ(comp->Counter(), 1);
}

// Test full lifecycle sequence
TEST_F(UComponentLifecycleTest, FullLifecycleSequence)
{
    // Initial state
    EXPECT_FALSE(comp->IsReady());
    
    // Default
    comp->Default();
    EXPECT_TRUE(comp->defaultCalled);
    EXPECT_EQ(comp->Counter(), 0);
    
    // Build
    comp->Build();
    EXPECT_TRUE(comp->buildCalled);
    EXPECT_TRUE(comp->IsReady());
    
    // Reset
    comp->Counter = 5;
    comp->Reset();
    EXPECT_TRUE(comp->resetCalled);
    EXPECT_EQ(comp->Counter(), 0);
    
    // Calculate multiple times
    comp->Calculate();
    EXPECT_TRUE(comp->calculateCalled);
    EXPECT_EQ(comp->Counter(), 1);
    
    comp->Calculate();
    EXPECT_EQ(comp->Counter(), 2);
    
    comp->Calculate();
    EXPECT_EQ(comp->Counter(), 3);
}

// Test lifecycle order requirement
TEST_F(UComponentLifecycleTest, LifecycleOrder)
{
    // Should not be able to Build without Default
    comp->Build();
    // Build might succeed but component might not be properly initialized
    
    // Should not be able to Calculate without Build
    comp->Default();
    comp->Calculate();
    // Calculate might fail or component might not be ready
}

// Test Reset after multiple Calculations
TEST_F(UComponentLifecycleTest, ResetAfterCalculations)
{
    comp->Default();
    comp->Build();
    comp->Reset();
    
    for (int i = 0; i < 10; ++i)
    {
        comp->Calculate();
    }
    
    EXPECT_EQ(comp->Counter(), 10);
    
    comp->Reset();
    EXPECT_EQ(comp->Counter(), 0);
    
    comp->Calculate();
    EXPECT_EQ(comp->Counter(), 1);
}

// Test IsReady state
TEST_F(UComponentLifecycleTest, IsReadyState)
{
    EXPECT_FALSE(comp->IsReady());
    
    comp->Default();
    EXPECT_FALSE(comp->IsReady()); // Still not ready after Default
    
    comp->Build();
    EXPECT_TRUE(comp->IsReady()); // Ready after Build
    
    comp->Reset();
    EXPECT_TRUE(comp->IsReady()); // Still ready after Reset
    
    comp->Calculate();
    EXPECT_TRUE(comp->IsReady()); // Still ready after Calculate
}

// Test multiple Reset calls
TEST_F(UComponentLifecycleTest, MultipleResetCalls)
{
    comp->Default();
    comp->Build();
    
    comp->Counter = 100;
    comp->Reset();
    EXPECT_EQ(comp->Counter(), 0);
    
    comp->Calculate();
    comp->Calculate();
    EXPECT_EQ(comp->Counter(), 2);
    
    comp->Reset();
    EXPECT_EQ(comp->Counter(), 0);
    
    comp->Reset(); // Reset again
    EXPECT_EQ(comp->Counter(), 0);
}

