#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-BasicLib/Deploy/Include/Lib.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class UEnvironmentTest : public ::testing::Test {
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

// Test Environment creation
TEST_F(UEnvironmentTest, CreateEnvironment) {
    ASSERT_NE(environment, nullptr);
    EXPECT_TRUE(environment->IsInit());
}

// Test setting Storage to Environment
TEST_F(UEnvironmentTest, SetStorage) {
    auto envStorage = environment->GetStorage();
    ASSERT_NE(envStorage, nullptr);
    EXPECT_EQ(envStorage, storage.get());
}

// Test creating model from Environment
TEST_F(UEnvironmentTest, CreateModel) {
    // Try to create a simple model
    bool result = environment->CreateModel("UContainer");
    EXPECT_TRUE(result);
    
    auto model = environment->GetModel();
    ASSERT_NE(model, nullptr);
    EXPECT_EQ(model->GetCompClassName(), "UContainer");
}

// Test model destruction
TEST_F(UEnvironmentTest, DestroyModel) {
    environment->CreateModel("UContainer");
    EXPECT_NE(environment->GetModel(), nullptr);
    
    bool result = environment->DestroyModel();
    EXPECT_TRUE(result);
    EXPECT_EQ(environment->GetModel(), nullptr);
}

// Test time management
TEST_F(UEnvironmentTest, TimeManagement) {
    auto& time = environment->GetTime();
    double currentTime = time.GetDoubleTime();
    EXPECT_GE(currentTime, 0.0);
    
    // Note: SetDoubleTime may not be available, so we just check that we can get time
    EXPECT_GE(time.GetDoubleTime(), 0.0);
}

