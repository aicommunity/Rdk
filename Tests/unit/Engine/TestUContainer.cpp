#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-BasicLib/Deploy/Include/Lib.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class UContainerTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateTestStorage();
        environment = CreateTestEnvironment(storage);
    }

    void TearDown() override {
        environment.reset();
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
    std::shared_ptr<UEnvironment> environment;
};

// Test Container creation
TEST_F(UContainerTest, CreateContainer) {
    // Use UModel instead of abstract UContainer
    auto model = storage->TakeObject("UModel");
    if (model) {
        EXPECT_EQ(model->GetCompClassName(), "UModel");
    } else {
        GTEST_SKIP() << "UModel class not available";
    }
}

// Test adding components to container
TEST_F(UContainerTest, AddComponent) {
    auto container = storage->TakeObject("UModel");
    if (!container) {
        GTEST_SKIP() << "UModel class not available";
        return;
    }
    
    // Create another model to add as component
    auto child = storage->TakeObject("UModel");
    if (!child) {
        GTEST_SKIP() << "Cannot create child component";
        return;
    }
    
    bool result = container->AddComponent(child);
    EXPECT_TRUE(result);
    EXPECT_GT(container->GetNumComponents(), 0);
}

// Test setting Environment to container
TEST_F(UContainerTest, SetEnvironment) {
    auto container = storage->TakeObject("UModel");
    if (!container) {
        GTEST_SKIP() << "UModel class not available";
        return;
    }
    
    bool result = container->SetEnvironment(environment.get());
    EXPECT_TRUE(result);
}

// Test container hierarchy
TEST_F(UContainerTest, ContainerHierarchy) {
    auto parent = storage->TakeObject("UModel");
    if (!parent) {
        GTEST_SKIP() << "UModel class not available";
        return;
    }
    
    auto child = storage->TakeObject("UModel");
    if (!child) {
        GTEST_SKIP() << "Cannot create child component";
        return;
    }
    
    parent->AddComponent(child);
    auto childOwner = child->GetMainOwner();
    EXPECT_EQ(childOwner, parent) << "Child's main owner should be parent";
}

// Test finding components in container
TEST_F(UContainerTest, FindComponent) {
    auto container = storage->TakeObject("UModel");
    if (!container) {
        GTEST_SKIP() << "UModel class not available";
        return;
    }
    
    auto child = storage->TakeObject("UModel");
    if (!child) {
        GTEST_SKIP() << "Cannot create child component";
        return;
    }
    child->SetName("TestComponent");
    
    container->AddComponent(child);
    
    auto found = container->GetComponent("TestComponent", true);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->GetName(), "TestComponent");
}

