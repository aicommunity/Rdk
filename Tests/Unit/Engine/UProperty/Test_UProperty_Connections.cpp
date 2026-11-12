#include <gtest/gtest.h>
#include "rdk.h"

using namespace RDK;

class ConnectionCompatComponent : public UNet
{
public:
    UProperty<int, ConnectionCompatComponent, ptPubOutput> Output;
    UProperty<int, ConnectionCompatComponent, ptPubInput> Input;
    UProperty<int, ConnectionCompatComponent, ptPubParameter> Parameter;

    ConnectionCompatComponent()
        : Output("Output", this), Input("Input", this), Parameter("Parameter", this)
    {
    }
};

TEST(UPropertyConnectionsCompat, BasicPropertySmoke)
{
    ConnectionCompatComponent comp;
    comp.Output = 42;
    comp.Input = 7;
    comp.Parameter = 100;

    EXPECT_EQ(comp.Output(), 42);
    EXPECT_EQ(comp.Input(), 7);
    EXPECT_EQ(comp.Parameter(), 100);
}

TEST(UPropertyConnectionsCompat, FindPropertyWorks)
{
    ConnectionCompatComponent comp;
    EXPECT_NE(comp.FindProperty("Output"), nullptr);
    EXPECT_NE(comp.FindProperty("Input"), nullptr);
    EXPECT_NE(comp.FindProperty("Parameter"), nullptr);
    EXPECT_EQ(comp.FindProperty("Missing"), nullptr);
}
