#include <gtest/gtest.h>
#include "rdk.h"

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

using namespace RDK;

class TestContainer : public UNet
{
public:
    int initCalls = 0, uninitCalls = 0, buildCalls = 0;
    TestContainer* New(void) override { return new TestContainer(); }
protected:
    void AInit() override { ++initCalls; }
    void AUnInit() override { ++uninitCalls; }
public:

    void ForceUpdateActiveCache() { UpdateActiveComponentsCache(); }
    bool IsActiveCacheValid() const { return ActiveComponentsCacheValid; }

    std::vector<std::string> PeekActiveComponentNames() const
    {
        std::vector<std::string> names;
        names.reserve(ActiveComponents.size());
        for(size_t i = 0; i < ActiveComponents.size(); ++i)
            names.push_back(ActiveComponents[i]->GetName());
        return names;
    }

    std::vector<std::string> ActiveComponentNames()
    {
        ForceUpdateActiveCache();
        return PeekActiveComponentNames();
    }

    std::vector<std::string> PComponentNames() const
    {
        std::vector<std::string> names;
        names.reserve(static_cast<size_t>(GetNumComponents()));
        for(int i = 0; i < GetNumComponents(); ++i)
            names.push_back(GetComponentByIndex(i)->GetName());
        return names;
    }

    void ExposeRemoveFromActiveCache(UEPtr<UContainer> comp)
    {
        RemoveFromActiveCache(comp);
    }

    /// Avoid UContainer::DelComponent(canfree=true) SetLogger-after-Free when Storage==0.
    void DetachAndFreeChildren()
    {
        while(GetNumComponents() > 0)
        {
            UEPtr<UContainer> c = GetComponentByIndex(0);
            DelComponent(c, false);
            if(c)
                c->Free();
        }
    }

protected:
    bool ADefault(void) override { return true; }
    bool ABuild(void) override { ++buildCalls; return true; }
    bool AReset(void) override { return true; }
    bool ACalculate(void) override { return true; }
};

TEST(UContainerCopyLifecycle, ReplacesInitializedGraphThroughNativeReinitialization)
{
    UEPtr<UStorage> storage(new UStorage);
    storage->AddClass(new UComponentFactoryMethod(storage,
        +[]() -> UComponent* { return new TestContainer; }, "CopyFixture"), "CopyFixture");
    auto source = dynamic_pointer_cast<TestContainer>(storage->TakeObject("CopyFixture"));
    auto target = dynamic_pointer_cast<TestContainer>(storage->TakeObject("CopyFixture"));
    ASSERT_TRUE(source); ASSERT_TRUE(target);
    source->Init(); target->Init();
    ASSERT_TRUE(source->IsInit()); ASSERT_TRUE(target->IsInit());
    const int buildsBefore = target->buildCalls;
    ASSERT_TRUE(source->Copy(target));
    EXPECT_FALSE(target->IsInit());
    // UNet restores links after the base copy; DisconnectAll can Build during
    // that native path, so Ready is not required to remain false here.
    EXPECT_EQ(target->uninitCalls, 1);
    target->Init();
    EXPECT_TRUE(target->IsInit());
    EXPECT_TRUE(target->IsReady());
    EXPECT_EQ(target->initCalls, 2);
    EXPECT_GT(target->buildCalls, buildsBefore);
    EXPECT_TRUE(source->IsInit());
    storage->ReturnObject(target); storage->ReturnObject(source);
}

TEST(UContainerFrequency, GlobalEnvironmentAppliesRateToNewSubtree)
{
    UEnvironment environment;
    TestContainer root;
    root.SetEnvironment(&environment);
    root.SetGlobalTimeStep(20000);
    UEPtr<TestContainer> child(new TestContainer), grandchild(new TestContainer);
    child->Name="Child"; grandchild->Name="Grandchild";
    child->SetGlobalTimeStep(4000); grandchild->SetGlobalTimeStep(5000);
    child->AddComponent(grandchild);
    root.AddComponent(child);
    EXPECT_EQ(child->GetTimeStep(),20000);
    EXPECT_EQ(grandchild->GetTimeStep(),20000);
    root.DetachAndFreeChildren();
}

TEST(UContainerFrequency, IndividualEnvironmentPreservesNewSubtreeRates)
{
    UEnvironment environment;
    environment.SetUseIndTimeStepFlag(true);
    TestContainer root;
    root.SetEnvironment(&environment);
    root.ChangeUseIndTimeStepMode(true);
    root.SetGlobalTimeStep(20000);
    UEPtr<TestContainer> child(new TestContainer), grandchild(new TestContainer);
    child->Name="Child"; grandchild->Name="Grandchild";
    child->SetGlobalTimeStep(4000); grandchild->SetGlobalTimeStep(5000);
    child->AddComponent(grandchild);
    root.AddComponent(child);
    EXPECT_EQ(child->GetTimeStep(),4000);
    EXPECT_EQ(grandchild->GetTimeStep(),5000);
    root.DetachAndFreeChildren();
}

TEST(UContainerFrequency, DetachedStorageCopyRetainsHeterogeneousRates)
{
    UEPtr<UStorage> storage(new UStorage);
    storage->AddClass(new UComponentFactoryMethod(storage,
        +[]() -> UComponent* { return new TestContainer; }, "FrequencyFixture"), "FrequencyFixture");
    auto source=dynamic_pointer_cast<TestContainer>(storage->TakeObject("FrequencyFixture"));
    auto child=dynamic_pointer_cast<TestContainer>(storage->TakeObject("FrequencyFixture"));
    ASSERT_TRUE(source); ASSERT_TRUE(child);
    source->Name="Source"; source->SetGlobalTimeStep(20000);
    child->Name="Child"; child->SetGlobalTimeStep(4000);
    source->AddComponent(child);
    auto copy=source->Alloc(storage);
    ASSERT_TRUE(copy);
    EXPECT_EQ(copy->GetTimeStep(),20000);
    ASSERT_EQ(copy->GetNumComponents(),1);
    EXPECT_EQ(copy->GetComponentByIndex(0)->GetTimeStep(),4000);
    copy->Free(); source->Free();
}

TEST(UContainerFrequency, EnvironmentAssignmentRefreshesModeWithoutChangingRates)
{
    UEnvironment environment;
    TestContainer root;
    root.SetGlobalTimeStep(20000);
    UEPtr<TestContainer> child(new TestContainer);
    child->Name="Child";child->SetGlobalTimeStep(4000);
    root.AddComponent(child);root.ChangeUseIndTimeStepMode(true);
    root.SetEnvironment(&environment);
    EXPECT_EQ(root.FindProperty("TimeStep")->GetType() & pgPublic,0u);
    EXPECT_EQ(child->FindProperty("TimeStep")->GetType() & pgPublic,0u);
    EXPECT_EQ(root.GetTimeStep(),20000);EXPECT_EQ(child->GetTimeStep(),4000);
    environment.SetUseIndTimeStepFlag(true);
    root.SetEnvironment(&environment); // Same pointer, updated policy.
    EXPECT_NE(root.FindProperty("TimeStep")->GetType() & pgPublic,0u);
    EXPECT_NE(child->FindProperty("TimeStep")->GetType() & pgPublic,0u);
    EXPECT_EQ(root.GetTimeStep(),20000);EXPECT_EQ(child->GetTimeStep(),4000);
    root.DetachAndFreeChildren();
}

class StorageFreeCounter : public TestContainer
{
public:
    std::vector<std::string>* calls = nullptr;
    StorageFreeCounter* New(void) override { return new StorageFreeCounter; }
    void Free(void) override
    {
        if(calls) calls->push_back(GetName());
        UNet::Free();
    }
};

TEST(UContainerStorageLifecycle, ClearFreesOwnershipForestOnceFromRoots)
{
    std::vector<std::string> calls;
    UEPtr<UStorage> storage(new UStorage);
    storage->AddClass(new UComponentFactoryMethod(storage,
        +[]() -> UComponent* { return new StorageFreeCounter; }, "FreeFixture"), "FreeFixture");
    auto parent = dynamic_pointer_cast<StorageFreeCounter>(storage->TakeObject("FreeFixture"));
    auto child = dynamic_pointer_cast<StorageFreeCounter>(storage->TakeObject("FreeFixture"));
    ASSERT_TRUE(parent); ASSERT_TRUE(child);
    parent->Name = "Parent"; child->Name = "Child";
    parent->calls = &calls; child->calls = &calls;
    ASSERT_NE(parent->AddComponent(child), ForbiddenId);
    parent->Init();
    storage->ClearObjectsStorage(true);
    // Pointers have been destroyed; inspect only the external event log.
    EXPECT_EQ(calls, (std::vector<std::string>{"Parent", "Child"}));
}

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

TEST(UContainerActiveCache, InvalidatedOnSetComponentPosition)
{
    TestContainer root;
    root.Name = "Root";

    UEPtr<TestContainer> a(new TestContainer());
    a->Name = "A";
    UEPtr<TestContainer> b(new TestContainer());
    b->Name = "B";
    UEPtr<TestContainer> c(new TestContainer());
    c->Name = "C";

    ASSERT_NE(root.AddComponent(a), ForbiddenId);
    ASSERT_NE(root.AddComponent(b), ForbiddenId);
    ASSERT_NE(root.AddComponent(c), ForbiddenId);

    root.ForceUpdateActiveCache();
    ASSERT_TRUE(root.IsActiveCacheValid());
    {
        const auto names = root.PeekActiveComponentNames();
        ASSERT_EQ(names.size(), 3u);
        EXPECT_EQ(names[0], "A");
        EXPECT_EQ(names[1], "B");
        EXPECT_EQ(names[2], "C");
    }

    ASSERT_TRUE(root.SetComponentPosition("C", 0));
    EXPECT_FALSE(root.IsActiveCacheValid());

    {
        const auto pnames = root.PComponentNames();
        ASSERT_EQ(pnames.size(), 3u);
        EXPECT_EQ(pnames[0], "C");
        EXPECT_EQ(pnames[1], "A");
        EXPECT_EQ(pnames[2], "B");
    }

    {
        const auto names = root.ActiveComponentNames();
        ASSERT_EQ(names.size(), 3u);
        EXPECT_EQ(names[0], "C");
        EXPECT_EQ(names[1], "A");
        EXPECT_EQ(names[2], "B");
    }

    root.DetachAndFreeChildren();
}

TEST(UContainerActiveCache, RemoveFromActiveCachePreservesRelativeOrder)
{
    TestContainer root;
    root.Name = "Root";

    UEPtr<TestContainer> a(new TestContainer());
    a->Name = "A";
    UEPtr<TestContainer> b(new TestContainer());
    b->Name = "B";
    UEPtr<TestContainer> c(new TestContainer());
    c->Name = "C";
    UEPtr<TestContainer> d(new TestContainer());
    d->Name = "D";

    ASSERT_NE(root.AddComponent(a), ForbiddenId);
    ASSERT_NE(root.AddComponent(b), ForbiddenId);
    ASSERT_NE(root.AddComponent(c), ForbiddenId);
    ASSERT_NE(root.AddComponent(d), ForbiddenId);

    root.ForceUpdateActiveCache();
    root.ExposeRemoveFromActiveCache(b);
    ASSERT_TRUE(root.IsActiveCacheValid());

    const auto names = root.PeekActiveComponentNames();
    ASSERT_EQ(names.size(), 3u);
    EXPECT_EQ(names[0], "A");
    EXPECT_EQ(names[1], "C");
    EXPECT_EQ(names[2], "D");

    root.DetachAndFreeChildren();
}

TEST(UContainerActiveCache, UpdateComputationOrderInvalidatesCache)
{
    class ReorderingContainer : public TestContainer
    {
    public:
        ReorderingContainer* New(void) override { return new ReorderingContainer(); }
        void UpdateComputationOrder(void) override
        {
            if(GetNumComponents() >= 2)
                SetComponentPosition(GetComponentByIndex(GetNumComponents() - 1)->GetName(), 0);
        }
    };

    ReorderingContainer root;
    root.Name = "Root";

    UEPtr<TestContainer> a(new TestContainer());
    a->Name = "A";
    UEPtr<TestContainer> b(new TestContainer());
    b->Name = "B";

    ASSERT_NE(root.AddComponent(a), ForbiddenId);
    ASSERT_NE(root.AddComponent(b), ForbiddenId);

    root.ForceUpdateActiveCache();
    ASSERT_TRUE(root.IsActiveCacheValid());

    root.UpdateComputationOrder();
    EXPECT_FALSE(root.IsActiveCacheValid());

    const auto names = root.ActiveComponentNames();
    ASSERT_EQ(names.size(), 2u);
    EXPECT_EQ(names[0], "B");
    EXPECT_EQ(names[1], "A");

    root.DetachAndFreeChildren();
}

TEST(UContainerComponentOrder, ForwardMoveToLastKeepsObjectsIdsAndActiveOrder)
{
    TestContainer root;
    UEPtr<TestContainer> a(new TestContainer),b(new TestContainer);
    a->Name="A";b->Name="B";a->Activity=true;b->Activity=true;
    const auto aid=root.AddComponent(a),bid=root.AddComponent(b);
    root.ForceUpdateActiveCache();
    ASSERT_TRUE(root.ChangeComponentPosition(0,1));
    EXPECT_EQ(root.PComponentNames(),(std::vector<std::string>{"B","A"}));
    EXPECT_EQ(root.GetComponentByIndex(0),b);EXPECT_EQ(root.GetComponentByIndex(1),a);
    EXPECT_EQ(root.GetComponent(aid),a);EXPECT_EQ(root.GetComponent(bid),b);
    EXPECT_EQ(root.ActiveComponentNames(),(std::vector<std::string>{"B","A"}));
    ASSERT_TRUE(root.SetComponentPosition("A",0));
    EXPECT_EQ(root.PComponentNames(),(std::vector<std::string>{"A","B"}));
    root.DetachAndFreeChildren();
}

TEST(UContainerComponentOrder, MaximumPositiveStepClampsBeforeIntegerOverflow)
{
    TestContainer root;
    UEPtr<TestContainer> a(new TestContainer),b(new TestContainer);
    a->Name="A";b->Name="B";
    root.AddComponent(a);root.AddComponent(b);
    ASSERT_TRUE(root.ChangeComponentPosition(1,std::numeric_limits<int>::max()));
    EXPECT_EQ(root.PComponentNames(),(std::vector<std::string>{"A","B"}));
    root.DetachAndFreeChildren();
}

TEST(UContainerComponentOrder, MinimumAbsolutePositionClampsBeforeSubtraction)
{
    TestContainer root;
    UEPtr<TestContainer> a(new TestContainer),b(new TestContainer);
    a->Name="A";b->Name="B";
    root.AddComponent(a);root.AddComponent(b);
    ASSERT_TRUE(root.SetComponentPosition(1,std::numeric_limits<int>::min()));
    EXPECT_EQ(root.PComponentNames(),(std::vector<std::string>{"B","A"}));
    EXPECT_FALSE(root.SetComponentPosition(-1,0));EXPECT_FALSE(root.SetComponentPosition(2,0));
    root.DetachAndFreeChildren();
}

class RawPropertyFixture : public TestContainer
{
public:
    UProperty<double,RawPropertyFixture,ptInput|ptState> Input;
    UProperty<double,RawPropertyFixture,ptOutput|ptState> Output;
    UProperty<MDMatrix<double>,RawPropertyFixture,ptInput|ptState> MatrixInput;
    UProperty<MDMatrix<double>,RawPropertyFixture,ptOutput|ptState> MatrixOutput;
    UProperty<std::vector<double>,RawPropertyFixture,ptInput|ptState> VectorInput;
    RawPropertyFixture():Input("Input",this),Output("Output",this),MatrixInput("MatrixInput",this),MatrixOutput("MatrixOutput",this),VectorInput("VectorInput",this){}
    RawPropertyFixture* New(void) override {return new RawPropertyFixture;}
protected:
    bool ADefault(void) override
    {Input=0.;Output=0.;MatrixInput->Assign(1,1,0.);MatrixOutput->Assign(1,1,0.);VectorInput=std::vector<double>{0.,0.};return true;}
};

TEST(UContainerPropertyBindings, StorageReuseClearsPreviousScalarAndMatrixSources)
{
    UEPtr<UStorage> storage(new UStorage);
    storage->AddClass(new UComponentFactoryMethod(storage,
        +[]() -> UComponent* {return new RawPropertyFixture;},"RawPoolFixture"),"RawPoolFixture");
    auto source=dynamic_pointer_cast<RawPropertyFixture>(storage->TakeObject("RawPoolFixture"));
    auto target=dynamic_pointer_cast<RawPropertyFixture>(storage->TakeObject("RawPoolFixture"));
    ASSERT_TRUE(source);ASSERT_TRUE(target);source->Output=2.;source->MatrixOutput->Assign(1,1,3.);
    ASSERT_TRUE(target->Input.SetPointer(0,&source->Output));ASSERT_TRUE(target->MatrixInput.SetPointer(0,&source->MatrixOutput));
    ASSERT_TRUE(target->VectorInput.SetPointer(0,&source->Output));ASSERT_TRUE(target->VectorInput.SetPointer(1,&source->Output));
    EXPECT_EQ(target->Input.GetData(),2.);EXPECT_EQ(target->MatrixInput.GetData()(0,0),3.);
    storage->ReturnObject(target);
    auto reused=dynamic_pointer_cast<RawPropertyFixture>(storage->TakeObject("RawPoolFixture"));ASSERT_EQ(reused,target);
    EXPECT_FALSE(reused->Input.IsConnected());EXPECT_FALSE(reused->MatrixInput.IsConnected());
    EXPECT_TRUE(reused->Input.GetDataSources().empty());EXPECT_TRUE(reused->MatrixInput.GetDataSources().empty());
    EXPECT_FALSE(reused->VectorInput.IsConnected());EXPECT_TRUE(reused->VectorInput.GetDataSources().empty());
    EXPECT_EQ(reused->VectorInput.GetData(),(std::vector<double>{0.,0.}));
    EXPECT_EQ(reused->Input.GetData(),0.);EXPECT_EQ(reused->MatrixInput.GetData()(0,0),0.);
    reused->Input.DetachFrom();reused->MatrixInput.DetachFrom();reused->VectorInput.DetachFrom();storage->ReturnObject(reused);storage->ReturnObject(source);
}

TEST(UContainerPropertyBindings, CopyReplacesPreviousDestinationDataSources)
{
    UEPtr<UStorage> storage(new UStorage);
    storage->AddClass(new UComponentFactoryMethod(storage,
        +[]() -> UComponent* {return new RawPropertyFixture;},"RawCopyFixture"),"RawCopyFixture");
    auto source=dynamic_pointer_cast<RawPropertyFixture>(storage->TakeObject("RawCopyFixture"));
    auto target=dynamic_pointer_cast<RawPropertyFixture>(storage->TakeObject("RawCopyFixture"));
    auto original=dynamic_pointer_cast<RawPropertyFixture>(storage->TakeObject("RawCopyFixture"));
    source->Output=2.;source->MatrixOutput->Assign(1,1,3.);
    ASSERT_TRUE(target->Input.SetPointer(0,&source->Output));ASSERT_TRUE(target->MatrixInput.SetPointer(0,&source->MatrixOutput));
    ASSERT_TRUE(target->VectorInput.SetPointer(0,&source->Output));ASSERT_TRUE(target->VectorInput.SetPointer(1,&source->Output));
    original->Copy(target,storage,true);
    EXPECT_FALSE(target->Input.IsConnected());EXPECT_FALSE(target->MatrixInput.IsConnected());
    EXPECT_FALSE(target->VectorInput.IsConnected());EXPECT_EQ(target->VectorInput.GetData(),original->VectorInput.GetData());
    EXPECT_EQ(target->Input.GetData(),original->Input.GetData());EXPECT_EQ(target->MatrixInput.GetData()(0,0),original->MatrixInput.GetData()(0,0));
    target->Input.DetachFrom();target->MatrixInput.DetachFrom();target->VectorInput.DetachFrom();
    storage->ReturnObject(target);storage->ReturnObject(source);storage->ReturnObject(original);
}

TEST(UContainerPropertyBindings, OrdinaryBuildResetAndDefaultPreserveLiveBindings)
{
    RawPropertyFixture source,target;source.Default();target.Default();source.Output=2.;source.MatrixOutput->Assign(1,1,3.);
    target.Input.SetPointer(0,&source.Output);target.MatrixInput.SetPointer(0,&source.MatrixOutput);
    target.VectorInput.SetPointer(0,&source.Output);target.VectorInput.SetPointer(1,&source.Output);
    target.Build();target.Reset();target.Default();
    EXPECT_TRUE(target.Input.IsConnected());EXPECT_TRUE(target.MatrixInput.IsConnected());
    EXPECT_EQ(target.Input.GetDataSources(),(std::vector<const UIProperty*>{&source.Output}));
    EXPECT_EQ(target.MatrixInput.GetDataSources(),(std::vector<const UIProperty*>{&source.MatrixOutput}));
    EXPECT_EQ(target.VectorInput.GetDataSources(),(std::vector<const UIProperty*>{&source.Output,&source.Output}));
    target.Input.DetachFrom();target.MatrixInput.DetachFrom();target.VectorInput.DetachFrom();
}

TEST(UContainerPropertyBindings, CopyRebindsInternalDirectSourcesToDestinationObjects)
{
    UEPtr<UStorage> storage(new UStorage);
    storage->AddClass(new UComponentFactoryMethod(storage,
        +[]() -> UComponent* {return new RawPropertyFixture;},"RawInternalCopyFixture"),"RawInternalCopyFixture");
    auto source=dynamic_pointer_cast<RawPropertyFixture>(storage->TakeObject("RawInternalCopyFixture"));
    auto target=dynamic_pointer_cast<RawPropertyFixture>(storage->TakeObject("RawInternalCopyFixture"));
    source->Output=2.;source->MatrixOutput->Assign(1,1,3.);
    source->Input.SetPointer(0,&source->Output);source->MatrixInput.SetPointer(0,&source->MatrixOutput);
    source->VectorInput.SetPointer(0,&source->Output);source->VectorInput.SetPointer(1,&source->Output);
    source->Copy(target,storage,true);
    EXPECT_EQ(target->Input.GetDataSources(),(std::vector<const UIProperty*>{&target->Output}));
    EXPECT_EQ(target->MatrixInput.GetDataSources(),(std::vector<const UIProperty*>{&target->MatrixOutput}));
    EXPECT_EQ(target->VectorInput.GetDataSources(),(std::vector<const UIProperty*>{&target->Output,&target->Output}));
    target->Output=7.;target->MatrixOutput->Assign(1,1,8.);source->Output=9.;
    EXPECT_EQ(target->Input.GetData(),7.);EXPECT_EQ(target->MatrixInput.GetData()(0,0),8.);
    EXPECT_EQ(target->VectorInput.GetData(),(std::vector<double>{7.,7.}));EXPECT_EQ(source->Input.GetData(),9.);
    target->BreakLinks();source->BreakLinks();storage->ReturnObject(target);storage->ReturnObject(source);
}

class ConstructorRawFixture : public RawPropertyFixture
{
public:
    ConstructorRawFixture()
    {Input.SetPointer(0,&Output);MatrixInput.SetPointer(0,&MatrixOutput);VectorInput.SetPointer(0,&Output);}
    ConstructorRawFixture* New(void) override {return new ConstructorRawFixture;}
};

TEST(UContainerPropertyBindings, PoolPreservesInternalWiringAndRemovesOnlyExternalVectorSlots)
{
    UEPtr<UStorage> storage(new UStorage);
    storage->AddClass(new UComponentFactoryMethod(storage,
        +[]() -> UComponent* {return new ConstructorRawFixture;},"InternalRawPoolFixture"),"InternalRawPoolFixture");
    storage->AddClass(new UComponentFactoryMethod(storage,
        +[]() -> UComponent* {return new RawPropertyFixture;},"ExternalRawSource"),"ExternalRawSource");
    auto target=dynamic_pointer_cast<ConstructorRawFixture>(storage->TakeObject("InternalRawPoolFixture"));
    auto external=dynamic_pointer_cast<RawPropertyFixture>(storage->TakeObject("ExternalRawSource"));
    ASSERT_TRUE(target);ASSERT_TRUE(external);target->Output=5.;external->Output=4.;
    target->VectorInput.SetPointer(1,&external->Output);EXPECT_EQ(target->VectorInput.GetData(),(std::vector<double>{5.,4.}));
    storage->ReturnObject(target);
    auto reused=dynamic_pointer_cast<ConstructorRawFixture>(storage->TakeObject("InternalRawPoolFixture"));ASSERT_EQ(reused,target);
    EXPECT_EQ(reused->Input.GetDataSources(),(std::vector<const UIProperty*>{&reused->Output}));
    EXPECT_EQ(reused->MatrixInput.GetDataSources(),(std::vector<const UIProperty*>{&reused->MatrixOutput}));
    EXPECT_EQ(reused->VectorInput.GetDataSources(),(std::vector<const UIProperty*>{&reused->Output}));
    reused->Output=11.;reused->MatrixOutput->Assign(1,1,12.);
    EXPECT_EQ(reused->Input.GetData(),11.);EXPECT_EQ(reused->MatrixInput.GetData()(0,0),12.);
    EXPECT_EQ(reused->VectorInput.GetData(),(std::vector<double>{11.}));
    storage->ReturnObject(reused);storage->ReturnObject(external);
}
