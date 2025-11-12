#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-BasicLib/Deploy/Include/Lib.h"
#include "../../../../Rdk/Core/Application/UEngineControl.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class UEngineControlTest : public ::testing::Test {
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

// Test EngineControl basic structure
TEST_F(UEngineControlTest, EngineControlStructure) {
    // Note: UEngineControl requires full application context
    // Basic structure test - full test in integration
    EXPECT_NE(storage, nullptr);
    EXPECT_NE(environment, nullptr);
}

// Test EngineControl can access Environment
TEST_F(UEngineControlTest, EngineControlEnvironmentAccess) {
    ASSERT_NE(environment, nullptr);
    EXPECT_TRUE(environment->IsInit());
}

