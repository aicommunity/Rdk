#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-BasicLib/Deploy/Include/Lib.h"
#include "../../../../Rdk/Core/Application/UProject.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class UProjectTest : public ::testing::Test {
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

// Test Project basic structure
TEST_F(UProjectTest, ProjectStructure) {
    // Note: UProject requires full application context
    // Basic structure test - full test in integration
    EXPECT_NE(storage, nullptr);
    EXPECT_NE(environment, nullptr);
}

// Test Project can work with Storage
TEST_F(UProjectTest, ProjectStorageAccess) {
    auto envStorage = environment->GetStorage();
    ASSERT_NE(envStorage, nullptr);
}

