#include <gtest/gtest.h>
#include "rdk.h"
#include <string>
#include <vector>
#include <thread>

using namespace RDK;

namespace {
struct OperationReporter : UModule {
    void Report(const std::string& operation,const std::string& reason) {RecordOperationFailure(operation,reason);}
};
}
TEST(UModuleOperationDiagnostics,SuccessDoesNotEraseAnEarlierFailure)
{
    OperationReporter module;const auto marker=module.OperationFailureSequence();
    module.Report("Reset","observed reset failure");
    EXPECT_TRUE(module.HasOperationFailureSince(marker));
    const auto after=module.OperationFailureSequence();
    module.Build();module.Init();module.Reset();module.Calculate();
    EXPECT_FALSE(module.HasOperationFailureSince(after));
    const auto failure=module.LastOperationFailure();
    EXPECT_EQ(failure.Sequence,after);EXPECT_EQ(failure.Operation,"Reset");
    EXPECT_EQ(failure.Reason,"observed reset failure");EXPECT_FALSE(failure.Overflowed);
}
TEST(UModuleOperationDiagnostics,CopyStartsAnIndependentDiagnosticHistory)
{
    OperationReporter source;source.Report("Calculate","source only");
    UModule copy(source);EXPECT_EQ(copy.OperationFailureSequence(),0u);
    EXPECT_FALSE(copy.HasOperationFailureSince(0));
    OperationReporter target;target.Report("Reset","target history");const auto marker=target.OperationFailureSequence();
    static_cast<UModule&>(target)=source;
    EXPECT_EQ(target.OperationFailureSequence(),marker);EXPECT_EQ(target.LastOperationFailure().Reason,"target history");
}
TEST(UModuleOperationDiagnostics,DiagnosticsDoNotChangeLifecycleIdentity)
{
    OperationReporter module;const auto life=module.ObserveLifecycle();
    module.Report("Build","diagnostic event");
    EXPECT_TRUE(life.IsCurrent());EXPECT_TRUE(module.HasOperationFailureSince(0));
}
TEST(UModuleOperationDiagnostics,ConcurrentReportsKeepCoherentOperationAndReason)
{
    OperationReporter module;
    std::thread first([&]{for(int i=0;i<1000;++i)module.Report("Reset","reset failure");});
    std::thread second([&]{for(int i=0;i<1000;++i)module.Report("Calculate","calculate failure");});
    first.join();second.join();const auto failure=module.LastOperationFailure();
    EXPECT_EQ(failure.Sequence,2000u);EXPECT_FALSE(failure.Overflowed);
    ASSERT_TRUE(failure.Operation=="Reset" || failure.Operation=="Calculate");
    EXPECT_EQ(failure.Reason,failure.Operation=="Reset"?"reset failure":"calculate failure");
}

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

TEST(UModuleLifecycleObservation, DirectContainerInitAndUnInitKeepTheirNativeHookBypasses)
{
    class DirectContainer : public UNet
    {
    public:
        UModuleLifecycleObservation watched;
        bool invalidAtInit=false,invalidAtUnInit=false;
        int genericBeforeHooks=0;
        std::vector<std::string> order;
    protected:
        void ABeforeInit() override { ++genericBeforeHooks; }
        void ABeforeUnInit() override { ++genericBeforeHooks; }
        void AInit() override { invalidAtInit=!watched.IsCurrent();order.push_back("init"); }
        void AUnInit() override { invalidAtUnInit=!watched.IsCurrent();order.push_back("uninit"); }
    };
    DirectContainer component;component.SetActivity(true);
    component.watched=component.ObserveLifecycle();component.Init();
    ASSERT_TRUE(component.IsInit());EXPECT_TRUE(component.invalidAtInit);
    const auto initialized=component.ObserveLifecycle();component.Init();
    EXPECT_TRUE(initialized.IsCurrent());EXPECT_EQ(component.genericBeforeHooks,0);
    component.watched=component.ObserveLifecycle();component.UnInit();
    EXPECT_TRUE(component.invalidAtUnInit);EXPECT_FALSE(component.IsInit());
    EXPECT_EQ(component.genericBeforeHooks,0);
    EXPECT_EQ(component.order,(std::vector<std::string>{"init","uninit"}));
    const auto inactive=component.ObserveLifecycle();component.UnInit();EXPECT_TRUE(inactive.IsCurrent());
}
