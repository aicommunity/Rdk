#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-BasicLib/Deploy/Include/Lib.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class EngineIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateTestStorage();
        environment = CreateTestEnvironment(storage);
        engine = CreateTestEngine(storage, environment);
    }

    void TearDown() override {
        engine.reset();
        environment.reset();
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
    std::shared_ptr<UEnvironment> environment;
    std::unique_ptr<UEngine> engine;
};

// Test full Engine-Environment-Storage integration
TEST_F(EngineIntegrationTest, FullIntegration) {
    ASSERT_NE(engine, nullptr);
    ASSERT_NE(environment, nullptr);
    ASSERT_NE(storage, nullptr);
    
    // Check connections through environment
    UStorage* envStorage = environment->GetStorage();
    EXPECT_EQ(envStorage, storage.get());
}

// Test creating and using model through full stack
TEST_F(EngineIntegrationTest, ModelCreationThroughStack) {
    // Create model through Environment
    bool result = environment->CreateModel("UContainer");
    EXPECT_TRUE(result);
    
    // Access model through Engine
    auto model = engine->GetModel();
    ASSERT_NE(model, nullptr);
    
    // Verify model is correct type
    EXPECT_EQ(model->GetCompClassName(), "UContainer");
}

// Test component creation through full stack
TEST_F(EngineIntegrationTest, ComponentCreationThroughStack) {
    // Create model
    environment->CreateModel("UContainer");
    auto model = engine->GetModel();
    ASSERT_NE(model, nullptr);
    
    // Add component to model
    auto item = storage->TakeObject("UItem");
    ASSERT_NE(item, nullptr);
    
    bool result = model->AddComponent(item);
    EXPECT_TRUE(result);
    EXPECT_GT(model->GetNumComponents(), 0);
}

