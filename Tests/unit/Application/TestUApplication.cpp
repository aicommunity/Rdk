#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-BasicLib/Deploy/Include/Lib.h"
#include "../../../../Rdk/Core/Application/UApplication.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class UApplicationTest : public ::testing::Test {
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

// Test Application initialization
TEST_F(UApplicationTest, ApplicationInitialization) {
    // Note: UApplication requires UAppCore, so this is a basic structure test
    // Full initialization test is in integration tests
    EXPECT_NE(storage, nullptr);
    EXPECT_NE(environment, nullptr);
}

// Test Application can access Storage
TEST_F(UApplicationTest, ApplicationStorageAccess) {
    UStorage* envStorage = environment->GetStorage();
    ASSERT_NE(envStorage, nullptr);
    EXPECT_EQ(envStorage, storage.get());
}

// Test Application can access Environment
TEST_F(UApplicationTest, ApplicationEnvironmentAccess) {
    ASSERT_NE(environment, nullptr);
    EXPECT_TRUE(environment->IsInit());
}

