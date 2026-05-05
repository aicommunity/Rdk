#include <gtest/gtest.h>
#include "rdk.h"

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
