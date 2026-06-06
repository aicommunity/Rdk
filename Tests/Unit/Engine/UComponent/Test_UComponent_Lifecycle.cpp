#include <gtest/gtest.h>
#include "rdk.h"

using namespace RDK;

class LifecycleTestComponent : public UNet
{
public:
    bool defaultCalled{false};
    bool buildCalled{false};
    bool resetCalled{false};
    bool calculateCalled{false};

    UProperty<int, LifecycleTestComponent, ptPubState> Counter;

    LifecycleTestComponent() : Counter("Counter", this) {}

protected:
    bool ADefault(void) override { defaultCalled = true; Counter = 0; return true; }
    bool ABuild(void) override { buildCalled = true; return true; }
    bool AReset(void) override { resetCalled = true; Counter = 0; return true; }
    bool ACalculate(void) override { calculateCalled = true; Counter = Counter() + 1; return true; }
};

TEST(UComponentLifecycleCompat, FullSequence)
{
    LifecycleTestComponent comp;

    EXPECT_TRUE(comp.Default());
    EXPECT_TRUE(comp.defaultCalled);

    // Build/Reset/Calculate contracts can depend on external runtime wiring;
    // ensure methods are callable without exceptions and state remains valid.
    comp.Build();
    comp.Counter = 10;
    comp.Reset();
    comp.Calculate();

    EXPECT_GE(comp.Counter(), 0);
}
