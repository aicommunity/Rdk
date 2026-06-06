#include <gtest/gtest.h>
#include <chrono>
#include "rdk.h"

using namespace RDK;

class PerfTestComponent : public UNet
{
public:
    UProperty<int, PerfTestComponent, ptPubParameter> Parameter;

    PerfTestComponent() : Parameter("Parameter", this) {}
};

TEST(UPropertyPerformanceCompat, GetSetAndFindProperty)
{
    PerfTestComponent comp;
    comp.Parameter = 1;

    const int iterations = 200000;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        comp.Parameter = i;
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    volatile int sink = 0;
    for (int i = 0; i < iterations; ++i)
    {
        sink += comp.Parameter();
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i)
    {
        EXPECT_NE(comp.FindProperty("Parameter"), nullptr);
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    auto set_us  = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    auto get_us  = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    auto find_us = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();

    EXPECT_GT(set_us, 0);
    EXPECT_GT(get_us, 0);
    EXPECT_GT(find_us, 0);
    EXPECT_NE(sink, -1);
}
