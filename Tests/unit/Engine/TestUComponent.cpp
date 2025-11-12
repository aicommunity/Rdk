#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-BasicLib/Deploy/Include/Lib.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class UComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateTestStorage();
    }

    void TearDown() override {
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
};

// Test Component creation
TEST_F(UComponentTest, CreateComponent) {
    auto component = storage->TakeObject("UComponent");
    ASSERT_NE(component, nullptr);
    EXPECT_EQ(component->GetCompClassName(), "UComponent");
}

// Test Component properties
TEST_F(UComponentTest, ComponentProperties) {
    auto component = storage->TakeObject("UComponent");
    ASSERT_NE(component, nullptr);
    
    // Test setting and getting name
    component->SetName("TestComponent");
    EXPECT_EQ(component->GetName(), "TestComponent");
}

// Test Component-Storage connection
TEST_F(UComponentTest, ComponentStorageConnection) {
    auto component = storage->TakeObject("UComponent");
    ASSERT_NE(component, nullptr);
    
    component->SetStorage(storage.get());
    // UContainer::GetStorage returns shared_ptr (overrides UComponent::GetStorage)
    auto compStorage = component->GetStorage();
    EXPECT_EQ(compStorage, storage);
}

// Test Component-Environment connection
TEST_F(UComponentTest, ComponentEnvironmentConnection) {
    auto component = storage->TakeObject("UComponent");
    ASSERT_NE(component, nullptr);
    
    auto environment = CreateTestEnvironment(storage);
    bool result = component->SetEnvironment(environment.get());
    EXPECT_TRUE(result);
    // UComponent::GetEnvironment returns UEnvironment* (not overridden by UContainer)
    UEnvironment* compEnv = component->GetEnvironment();
    EXPECT_EQ(compEnv, environment.get());
}

