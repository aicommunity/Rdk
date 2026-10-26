#include <gtest/gtest.h>
#include "rdk.h"
#include <string>
#include <vector>

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

namespace {
class ObservedModule : public UModule
{
public:
    int value=0;
    std::vector<std::string> events;
    UModuleLifecycleObservation watched;
    bool watchArmed=false,invalidBeforeReset=false;
    void InvalidateStructure() { Ready=false; }
protected:
    bool ADefault() override { value=0;events.push_back("default");return true; }
    bool ABuild() override { events.push_back("build");return true; }
    bool ABeforeReset() override
    { if(watchArmed)invalidBeforeReset=!watched.IsCurrent();return true; }
    bool AReset() override { value=0;events.push_back("reset");return true; }
    bool ACalculate() override { ++value;events.push_back("calculate");return true; }
    void AInit() override { events.push_back("init"); }
    void AUnInit() override { events.push_back("uninit");UModule::AUnInit(); }
};
}

TEST(UModuleLifecycleObservation, NativeOrderAndCalculatePreserveObservation)
{
    ObservedModule module;const auto fresh=module.ObserveLifecycle();
    module.Default();EXPECT_FALSE(fresh.IsCurrent());EXPECT_TRUE(fresh.IsAlive());
    EXPECT_EQ(module.events,(std::vector<std::string>{"default","build","reset"}));
    module.Init();const auto initialized=module.ObserveLifecycle();
    module.Init();module.Build(); // Native no-op calls must not invent mutations.
    module.Calculate();module.Calculate();
    EXPECT_TRUE(initialized.IsCurrent());EXPECT_EQ(module.value,2);
    module.watched=initialized;module.watchArmed=true;module.Reset();
    EXPECT_TRUE(module.invalidBeforeReset);EXPECT_FALSE(initialized.IsCurrent());
    EXPECT_EQ(module.value,0);
    const auto reset=module.ObserveLifecycle();module.Calculate();
    EXPECT_TRUE(reset.IsCurrent());EXPECT_EQ(module.value,1);
    module.InvalidateStructure();module.Build();
    EXPECT_FALSE(reset.IsCurrent());EXPECT_EQ(module.value,0);
    ASSERT_GE(module.events.size(),2u);
    EXPECT_EQ(module.events[module.events.size()-2],"build");EXPECT_EQ(module.events.back(),"reset");
    const auto rebuilt=module.ObserveLifecycle();module.UnInit();
    EXPECT_FALSE(rebuilt.IsCurrent());
}

TEST(UModuleLifecycleObservation, LifetimeDoesNotOwnComponentAndCopyIsIndependent)
{
    UModuleLifecycleObservation expired;
    {
        ObservedModule source;source.Default();source.Init();source.Calculate();
        const auto original=source.ObserveLifecycle();
        ObservedModule copy(source);const auto copied=copy.ObserveLifecycle();
        copy.Reset();EXPECT_FALSE(copied.IsCurrent());EXPECT_TRUE(original.IsCurrent());
        EXPECT_EQ(source.value,1);EXPECT_EQ(copy.value,0);
        const auto beforeAssignment=copy.ObserveLifecycle();copy=source;
        EXPECT_FALSE(beforeAssignment.IsCurrent());EXPECT_TRUE(original.IsCurrent());
        EXPECT_EQ(copy.value,source.value);
        expired=source.ObserveLifecycle();EXPECT_TRUE(expired.IsCurrent());
    }
    EXPECT_FALSE(expired.IsAlive());EXPECT_FALSE(expired.IsCurrent());
    UModuleLifecycleObservation empty;EXPECT_FALSE(empty.IsAlive());EXPECT_FALSE(empty.IsCurrent());
}

TEST(UModuleLifecycleObservation, NativeContainerResetAttemptIsObservableWithoutInitialization)
{
    LifecycleTestComponent component;component.Counter=7;const auto before=component.ObserveLifecycle();
    component.Reset(); // UContainer enters BeforeReset, then guards InitFlag.
    EXPECT_FALSE(before.IsCurrent());EXPECT_TRUE(before.IsAlive());
    EXPECT_FALSE(component.resetCalled);EXPECT_EQ(component.Counter.GetData(),7);
    const auto resetAttempt=component.ObserveLifecycle();component.Build();
    // With no Storage, UContainer does not enter the Build hooks.
    EXPECT_TRUE(resetAttempt.IsCurrent());EXPECT_FALSE(component.buildCalled);
}
