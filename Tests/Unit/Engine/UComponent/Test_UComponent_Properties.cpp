#include <gtest/gtest.h>
#include "rdk.h"

#include <algorithm>

using namespace RDK;

class TestComponentWithProps : public UNet
{
public:
    UProperty<int, TestComponentWithProps, ptPubParameter> Param1;
    UProperty<double, TestComponentWithProps, ptPubParameter> Param2;
    UProperty<int, TestComponentWithProps, ptPubState> State1;
    UProperty<int, TestComponentWithProps, ptPubOutput> Output1;
    UProperty<int, TestComponentWithProps, ptPubInput> Input1;

    TestComponentWithProps()
        : Param1("Param1", this), Param2("Param2", this), State1("State1", this),
          Output1("Output1", this), Input1("Input1", this)
    {
    }
};

TEST(UComponentPropertiesCompat, FindPropertyAndType)
{
    TestComponentWithProps comp;

    UIProperty* param = comp.FindProperty("Param1");
    UIProperty* state = comp.FindProperty("State1");
    UIProperty* outp = comp.FindProperty("Output1");
    UIProperty* inp = comp.FindProperty("Input1");

    ASSERT_NE(param, nullptr);
    ASSERT_NE(state, nullptr);
    ASSERT_NE(outp, nullptr);
    ASSERT_NE(inp, nullptr);

    EXPECT_EQ(param->GetType(), ptPubParameter);
    EXPECT_EQ(state->GetType(), ptPubState);
    EXPECT_EQ(outp->GetType(), ptPubOutput);
    EXPECT_EQ(inp->GetType(), ptPubInput);
}

TEST(UComponentPropertiesCompat, PropertiesOrderMatchesRegistration)
{
    TestComponentWithProps comp;
    const auto& order = comp.GetPropertiesOrder();
    ASSERT_EQ(order.size(), comp.GetPropertiesList().size());

    auto index_of = [&](const char* name) -> int {
        auto it = std::find(order.begin(), order.end(), NameT(name));
        if(it == order.end())
            return -1;
        return static_cast<int>(it - order.begin());
    };

    const int iParam1 = index_of("Param1");
    const int iParam2 = index_of("Param2");
    const int iState1 = index_of("State1");
    const int iOutput1 = index_of("Output1");
    const int iInput1 = index_of("Input1");

    ASSERT_GE(iParam1, 0);
    ASSERT_GE(iParam2, 0);
    ASSERT_GE(iState1, 0);
    ASSERT_GE(iOutput1, 0);
    ASSERT_GE(iInput1, 0);

    // Derived member-init order relative to each other
    EXPECT_LT(iParam1, iParam2);
    EXPECT_LT(iParam2, iState1);
    EXPECT_LT(iState1, iOutput1);
    EXPECT_LT(iOutput1, iInput1);
}
