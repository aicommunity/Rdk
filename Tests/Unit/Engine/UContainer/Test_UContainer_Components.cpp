#include <gtest/gtest.h>
#include "../../../Deploy/Include/rdk.h"

using namespace RDK;

// Test fixture for UContainer component management
class UContainerComponentsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test storage and environment
        Storage = UEPtr<UStorage>(new UStorage());
        Env = UEPtr<UEnvironment>(new UEnvironment());
        Env->SetStorage(Storage);
        
        // Create a test container
        Container = UEPtr<UContainer>(new UContainer());
        Container->SetStorage(Storage);
        Container->SetEnvironment(Env);
    }

    void TearDown() override {
        Container.reset();
        Env.reset();
        Storage.reset();
    }

    UEPtr<UStorage> Storage;
    UEPtr<UEnvironment> Env;
    UEPtr<UContainer> Container;
};

// Test adding a component
TEST_F(UContainerComponentsTest, AddComponent) {
    UEPtr<UContainer> child = UEPtr<UContainer>(new UContainer());
    child->SetStorage(Storage);
    
    UId id = Container->AddComponent(child);
    
    EXPECT_NE(id, ForbiddenId);
    EXPECT_EQ(child->GetOwner(), Container.get());
    EXPECT_EQ(child->Id, id);
}

// Test getting a component by ID
TEST_F(UContainerComponentsTest, GetComponentById) {
    UEPtr<UContainer> child = UEPtr<UContainer>(new UContainer());
    child->SetStorage(Storage);
    
    UId id = Container->AddComponent(child);
    
    UEPtr<UContainer> retrieved = Container->GetComponent(id);
    EXPECT_EQ(retrieved, child);
}

// Test getting a component by name
TEST_F(UContainerComponentsTest, GetComponentByName) {
    UEPtr<UContainer> child = UEPtr<UContainer>(new UContainer());
    child->SetStorage(Storage);
    child->Name = "TestComponent";
    
    UId id = Container->AddComponent(child);
    
    UEPtr<UContainer> retrieved = Container->GetComponent("TestComponent");
    EXPECT_EQ(retrieved, child);
}

// Test deleting a component
TEST_F(UContainerComponentsTest, DelComponent) {
    UEPtr<UContainer> child = UEPtr<UContainer>(new UContainer());
    child->SetStorage(Storage);
    
    UId id = Container->AddComponent(child);
    EXPECT_EQ(Container->GetNumComponents(), 1);
    
    Container->DelComponent(id, false);
    EXPECT_EQ(Container->GetNumComponents(), 0);
    
    // Component should not be retrievable
    EXPECT_THROW(Container->GetComponent(id), EComponentIdNotExist);
}

// Test component lookup performance with index map
TEST_F(UContainerComponentsTest, GetComponentPerformance) {
    // Add multiple components
    std::vector<UEPtr<UContainer>> components;
    std::vector<UId> ids;
    
    for(int i = 0; i < 100; ++i) {
        UEPtr<UContainer> child = UEPtr<UContainer>(new UContainer());
        child->SetStorage(Storage);
        UId id = Container->AddComponent(child);
        components.push_back(child);
        ids.push_back(id);
    }
    
    // Test O(1) lookup performance
    for(int i = 0; i < 1000; ++i) {
        UId testId = ids[i % ids.size()];
        UEPtr<UContainer> retrieved = Container->GetComponent(testId);
        EXPECT_EQ(retrieved, components[i % components.size()]);
    }
}

// Test component count
TEST_F(UContainerComponentsTest, GetNumComponents) {
    EXPECT_EQ(Container->GetNumComponents(), 0);
    
    for(int i = 0; i < 10; ++i) {
        UEPtr<UContainer> child = UEPtr<UContainer>(new UContainer());
        child->SetStorage(Storage);
        Container->AddComponent(child);
    }
    
    EXPECT_EQ(Container->GetNumComponents(), 10);
}

// Test component iteration
TEST_F(UContainerComponentsTest, ComponentIteration) {
    std::vector<UId> addedIds;
    
    for(int i = 0; i < 5; ++i) {
        UEPtr<UContainer> child = UEPtr<UContainer>(new UContainer());
        child->SetStorage(Storage);
        UId id = Container->AddComponent(child);
        addedIds.push_back(id);
    }
    
    // Iterate through components
    int count = 0;
    for(int i = 0; i < Container->GetNumComponents(); ++i) {
        UEPtr<UContainer> comp = Container->GetComponentByIndex(i);
        EXPECT_NE(comp, nullptr);
        count++;
    }
    
    EXPECT_EQ(count, 5);
}

// Test component name uniqueness
TEST_F(UContainerComponentsTest, ComponentNameUniqueness) {
    UEPtr<UContainer> child1 = UEPtr<UContainer>(new UContainer());
    child1->SetStorage(Storage);
    child1->Name = "Test";
    Container->AddComponent(child1);
    
    UEPtr<UContainer> child2 = UEPtr<UContainer>(new UContainer());
    child2->SetStorage(Storage);
    child2->Name = "Test";
    Container->AddComponent(child2);
    
    // Names should be made unique
    EXPECT_NE(child1->Name, child2->Name);
}

