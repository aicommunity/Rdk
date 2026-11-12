#include <gtest/gtest.h>
#include "rdk.h"

using namespace RDK;

class TestDirectUpdateComponent : public UNet
{
public:
    UProperty<double, TestDirectUpdateComponent, ptPubParameter> Threshold;

    TestDirectUpdateComponent()
        : Threshold("Threshold", this)
    {
    }

    TestDirectUpdateComponent* New(void) override { return new TestDirectUpdateComponent(); }

protected:
    virtual bool ADefault(void)
    {
        Threshold = 0.0;
        return true;
    }

    virtual bool ABuild(void) { return true; }
    virtual bool AReset(void) { return true; }
    virtual bool ACalculate(void) { return true; }
};

TEST(UPropertyDirectUpdate, SetDataDirectIsImmediatelyVisible)
{
    TestDirectUpdateComponent comp;
    comp.Threshold.SetDataDirect(0.123456);
    EXPECT_DOUBLE_EQ(comp.Threshold.GetData(), 0.123456);
    EXPECT_DOUBLE_EQ(comp.Threshold(), 0.123456);
}

TEST(UPropertyDirectUpdate, LastDirectUpdateWins)
{
    TestDirectUpdateComponent comp;
    comp.Threshold.SetDataDirect(0.1);
    comp.Threshold.SetDataDirect(0.2);
    comp.Threshold.SetDataDirect(0.3);
    EXPECT_DOUBLE_EQ(comp.Threshold.GetData(), 0.3);
}

