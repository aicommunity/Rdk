#include <gtest/gtest.h>
#include "rdk.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace RDK;

class TestContainer : public UNet
{
public:
    TestContainer* New(void) override { return new TestContainer(); }

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
