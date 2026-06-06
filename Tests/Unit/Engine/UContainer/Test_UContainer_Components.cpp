#include <gtest/gtest.h>
#include "rdk.h"

using namespace RDK;

class TestContainer : public UContainer
{
public:
    TestContainer* New(void) override { return new TestContainer(); }
protected:
    bool ADefault(void) override { return true; }
    bool ABuild(void) override { return true; }
    bool AReset(void) override { return true; }
    bool ACalculate(void) override { return true; }
};

TEST(UContainerCompat, BasicLifecycleAndName)
{
    TestContainer root;
    root.Name = "Root";

    EXPECT_TRUE(root.Default());
    EXPECT_TRUE(root.Build());
    EXPECT_TRUE(root.Reset());
    EXPECT_TRUE(root.Calculate());
    EXPECT_EQ(root.Name.GetData(), "Root");
}
