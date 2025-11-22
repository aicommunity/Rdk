#include <gtest/gtest.h>
#include "../../../Deploy/Include/rdk.h"

using namespace RDK;

// Test fixture for UStorage object management
class UStorageObjectsTest : public ::testing::Test {
protected:
    void SetUp() override {
        Storage = UEPtr<UStorage>(new UStorage());
    }

    void TearDown() override {
        Storage.reset();
    }

    UEPtr<UStorage> Storage;
};

// Test pushing an object to storage
TEST_F(UStorageObjectsTest, PushObject) {
    UEPtr<UContainer> obj = UEPtr<UContainer>(new UContainer());
    UId classId = 1;
    
    Storage->PushObject(classId, obj);
    
    EXPECT_EQ(obj->GetClass(), classId);
    EXPECT_EQ(obj->GetStorage(), Storage.get());
}

// Test popping an object from storage
TEST_F(UStorageObjectsTest, PopObject) {
    UEPtr<UContainer> obj = UEPtr<UContainer>(new UContainer());
    UId classId = 1;
    
    Storage->PushObject(classId, obj);
    UId retrievedId = Storage->PopObject(obj);
    
    EXPECT_EQ(retrievedId, classId);
    EXPECT_EQ(obj->GetStorage(), nullptr);
    EXPECT_EQ(obj->GetClass(), ForbiddenId);
}

// Test returning an object to storage
TEST_F(UStorageObjectsTest, ReturnObject) {
    UEPtr<UContainer> obj = UEPtr<UContainer>(new UContainer());
    UId classId = 1;
    
    Storage->PushObject(classId, obj);
    Storage->PopObject(obj);
    
    // Return object back
    Storage->ReturnObject(obj);
    
    // Object should be back in storage
    EXPECT_EQ(obj->GetStorage(), Storage.get());
}

// Test object lookup performance with index map
TEST_F(UStorageObjectsTest, PopObjectPerformance) {
    UId classId = 1;
    std::vector<UEPtr<UContainer>> objects;
    
    // Push multiple objects
    for(int i = 0; i < 100; ++i) {
        UEPtr<UContainer> obj = UEPtr<UContainer>(new UContainer());
        Storage->PushObject(classId, obj);
        objects.push_back(obj);
    }
    
    // Test O(log n) lookup performance
    for(int i = 0; i < 1000; ++i) {
        UEPtr<UContainer> testObj = objects[i % objects.size()];
        UId retrievedId = Storage->PopObject(testObj);
        EXPECT_EQ(retrievedId, classId);
        
        // Push back for next iteration
        Storage->PushObject(classId, testObj);
    }
}

// Test moving object between storages
TEST_F(UStorageObjectsTest, MoveObject) {
    UEPtr<UStorage> storage2 = UEPtr<UStorage>(new UStorage());
    UEPtr<UContainer> obj = UEPtr<UContainer>(new UContainer());
    UId classId = 1;
    
    Storage->PushObject(classId, obj);
    Storage->MoveObject(obj, storage2);
    
    EXPECT_EQ(obj->GetStorage(), storage2.get());
    EXPECT_NE(obj->GetStorage(), Storage.get());
}

// Test multiple objects of same class
TEST_F(UStorageObjectsTest, MultipleObjectsSameClass) {
    UId classId = 1;
    std::vector<UEPtr<UContainer>> objects;
    
    for(int i = 0; i < 10; ++i) {
        UEPtr<UContainer> obj = UEPtr<UContainer>(new UContainer());
        Storage->PushObject(classId, obj);
        objects.push_back(obj);
    }
    
    // Pop all objects
    for(auto& obj : objects) {
        UId id = Storage->PopObject(obj);
        EXPECT_EQ(id, classId);
    }
}

