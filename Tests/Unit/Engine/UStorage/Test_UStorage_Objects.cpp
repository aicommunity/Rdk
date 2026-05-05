#include <gtest/gtest.h>
#include "rdk.h"

using namespace RDK;

class TestStorageContainer : public UContainer
{
public:
    TestStorageContainer* New(void) override { return new TestStorageContainer(); }
protected:
    bool ADefault(void) override { return true; }
    bool ABuild(void) override { return true; }
    bool AReset(void) override { return true; }
    bool ACalculate(void) override { return true; }
};

class TestStorage : public UStorage
{
public:
    using UStorage::PopObject;
    using UStorage::PushObject;
    using UStorage::ReturnObject;
};

TEST(UStorageCompat, PushPopReturnObject)
{
    UEPtr<TestStorage> storage(new TestStorage());
    UEPtr<TestStorageContainer> obj(new TestStorageContainer());

    const UId classId = 1;
    storage->PushObject(classId, obj);
    EXPECT_EQ(obj->GetClass(), classId);
    EXPECT_EQ(obj->GetStorage(), storage.Get());

    UId poppedClass = storage->PopObject(obj);
    EXPECT_EQ(poppedClass, classId);
    EXPECT_EQ(obj->GetStorage(), nullptr);

    storage->ReturnObject(obj);
    // ReturnObject may keep the object detached when it is not in active-use mode.
    EXPECT_TRUE(obj->GetStorage() == storage.Get() || obj->GetStorage() == nullptr);
}
