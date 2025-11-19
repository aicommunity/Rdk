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
    auto childOwner_weak = child->GetMainOwner();
    ASSERT_FALSE(childOwner_weak.expired()) << "Child's main owner should exist";
    auto childOwner = childOwner_weak.lock();
    EXPECT_EQ(childOwner, parent) << "Child's main owner should be parent";
}

// Test finding components in container
TEST_F(UContainerTest, FindComponent) {
    LOG(INFO) << "=== FindComponent test START ===";
    
    auto container = storage->TakeObject("UModel");
    if (!container) {
        GTEST_SKIP() << "UModel class not available";
        return;
    }
    LOG(INFO) << "Created container: " << container->GetName() << " use_count=" << container.use_count() 
              << " address=" << container.get();
    
    auto child = storage->TakeObject("UModel");
    if (!child) {
        GTEST_SKIP() << "Cannot create child component";
        return;
    }
    LOG(INFO) << "Created child: " << child->GetName() << " use_count=" << child.use_count() 
              << " address=" << child.get();
    
    child->SetName("TestComponent");
    
    container->AddComponent(child);
    LOG(INFO) << "Added child to container, child use_count=" << child.use_count();
    
    auto found_weak = container->GetComponent("TestComponent", true);
    ASSERT_FALSE(found_weak.expired());
    auto found = found_weak.lock();
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->GetName(), "TestComponent");
    
    LOG(INFO) << "=== FindComponent test END - objects will be destroyed ===";
    LOG(INFO) << "Before test end: container use_count=" << container.use_count() 
              << " child use_count=" << child.use_count();
    
    // Note: With shared_ptr, objects will be automatically destroyed
    // when the last reference is released. The order of destruction
    // should be handled automatically by shared_ptr reference counting.
    // Objects in container->Components will be destroyed when container
    // is destroyed, and objects in Storage will be destroyed when Storage
    // is destroyed. This should work correctly with shared_ptr.
}

// Minimal test to reproduce segfault
TEST_F(UContainerTest, MinimalSegfaultTest) {
    LOG(INFO) << "=== MinimalSegfaultTest START ===";
    
    // Create Storage and container
    auto container = storage->TakeObject("UModel");
    ASSERT_NE(container, nullptr);
    LOG(INFO) << "Created container: " << container->GetName() << " use_count=" << container.use_count() 
              << " address=" << container.get();
    
    // Create child
    auto child = storage->TakeObject("UModel");
    ASSERT_NE(child, nullptr);
    LOG(INFO) << "Created child: " << child->GetName() << " use_count=" << child.use_count() 
              << " address=" << child.get();
    
    // Add child to container
    container->AddComponent(child);
    LOG(INFO) << "Added child to container, child use_count=" << child.use_count();
    
    // At this point:
    // - container: use_count should be 2 (test + Storage)
    // - child: use_count should be 3 (test + container->Components + Storage)
    
    LOG(INFO) << "Before test end: container use_count=" << container.use_count() 
              << " child use_count=" << child.use_count();
    
    // Test will end here, objects will be destroyed
    // Expected order:
    // 1. child goes out of scope (use_count: 3 -> 2)
    // 2. container goes out of scope (use_count: 2 -> 1)
    // 3. Storage is destroyed in TearDown
    // 4. ObjectsStorage.clear() destroys remaining objects
    
    LOG(INFO) << "=== MinimalSegfaultTest END ===";
}

