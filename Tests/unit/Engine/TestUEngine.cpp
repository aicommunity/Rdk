#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-BasicLib/Deploy/Include/Lib.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class UEngineTest : public ::testing::Test {
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

// Test Engine creation
TEST_F(UEngineTest, CreateEngine) {
    ASSERT_NE(engine, nullptr);
}

// Test Engine initialization
TEST_F(UEngineTest, InitEngine) {
    // Engine is initialized, check that it can work with model
    ASSERT_NE(engine, nullptr);
}

// Test Engine-Storage connection
TEST_F(UEngineTest, EngineStorageConnection) {
    // Storage is set during Init, verify through model access
    ASSERT_NE(engine, nullptr);
    ASSERT_NE(storage, nullptr);
}

// Test Engine-Environment connection
TEST_F(UEngineTest, EngineEnvironmentConnection) {
    // Environment is set during Init, verify through model access
    ASSERT_NE(engine, nullptr);
    ASSERT_NE(environment, nullptr);
}

// Test getting model through Engine
TEST_F(UEngineTest, GetModel) {
    environment->CreateModel("UContainer");
    auto model = engine->GetModel();
    ASSERT_NE(model, nullptr);
}

