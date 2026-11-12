#include <gtest/gtest.h>
#include "rdk.h"

using namespace RDK;

class TestComponent : public UNet
{
public:
    UProperty<double, TestComponent, ptPubParameter> Param;
    UProperty<int, TestComponent, ptPubState> State;

    TestComponent() : Param("Param", this), State("State", this) {}
};

TEST(UPropertyBasicCompat, CreateAndGetSet)
{
    TestComponent comp;
    comp.Param = 42.5;
    comp.State = 100;

    EXPECT_DOUBLE_EQ(comp.Param.GetData(), 42.5);
    EXPECT_DOUBLE_EQ(comp.Param(), 42.5);
    EXPECT_EQ(comp.State.GetData(), 100);
    EXPECT_EQ(comp.State(), 100);
}

TEST(UPropertyBasicCompat, FindProperty)
{
    TestComponent comp;
    EXPECT_NE(comp.FindProperty("Param"), nullptr);
    EXPECT_NE(comp.FindProperty("State"), nullptr);
    EXPECT_EQ(comp.FindProperty("Unknown"), nullptr);
}
