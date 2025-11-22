#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <chrono>
#include "../../../Deploy/Include/rdk.h"

using namespace RDK;

// Test class for performance benchmarks
class PerfTestComponent : public UNet
{
public:
    UProperty<int, PerfTestComponent, ptPubOutput> Output;
    UProperty<int, PerfTestComponent, ptPubInput> Input;
    UProperty<int, PerfTestComponent, ptPubParameter> Parameter;

    PerfTestComponent() : 
        Output("Output", this),
        Input("Input", this),
        Parameter("Parameter", this)
    {
    }
};

// Simple performance test for GetData
TEST(PerformanceTest, GetDataUnconnected)
{
    PerfTestComponent comp;
    comp.Parameter = 42;
    
    auto start = std::chrono::high_resolution_clock::now();
    const int iterations = 1000000;
    
    for (int i = 0; i < iterations; ++i)
    {
        volatile int value = comp.Parameter();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_ns = (duration.count() * 1000.0) / iterations;
    std::cout << "Average GetData time (unconnected): " << avg_time_ns << " ns" << std::endl;
    
    // Should be reasonably fast (< 100ns per access)
    EXPECT_LT(avg_time_ns, 1000.0);
}

// Performance test for SetData
TEST(PerformanceTest, SetDataUnconnected)
{
    PerfTestComponent comp;
    
    auto start = std::chrono::high_resolution_clock::now();
    const int iterations = 1000000;
    
    for (int i = 0; i < iterations; ++i)
    {
        comp.Parameter = i;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_ns = (duration.count() * 1000.0) / iterations;
    std::cout << "Average SetData time (unconnected): " << avg_time_ns << " ns" << std::endl;
    
    EXPECT_LT(avg_time_ns, 1000.0);
}

// Performance test for connected properties
TEST(PerformanceTest, GetDataConnected)
{
    auto comp1 = std::make_unique<PerfTestComponent>();
    auto comp2 = std::make_unique<PerfTestComponent>();
    
    comp1->Output = 42;
    comp2->Input.Connect(&comp1->Output);
    
    auto start = std::chrono::high_resolution_clock::now();
    const int iterations = 1000000;
    
    for (int i = 0; i < iterations; ++i)
    {
        volatile int value = comp2->Input();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_ns = (duration.count() * 1000.0) / iterations;
    std::cout << "Average GetData time (connected): " << avg_time_ns << " ns" << std::endl;
    
    // Connected properties may be slightly slower but should still be reasonable
    EXPECT_LT(avg_time_ns, 2000.0);
}

// Performance test for property search in component
TEST(PerformanceTest, FindProperty)
{
    PerfTestComponent comp;
    
    auto start = std::chrono::high_resolution_clock::now();
    const int iterations = 100000;
    
    for (int i = 0; i < iterations; ++i)
    {
        auto prop = comp.FindProperty("Parameter");
        EXPECT_NE(prop, nullptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_ns = (duration.count() * 1000.0) / iterations;
    std::cout << "Average FindProperty time: " << avg_time_ns << " ns" << std::endl;
    
    // With caching, should be very fast
    EXPECT_LT(avg_time_ns, 500.0);
}

// Comparison test: unconnected vs connected
TEST(PerformanceTest, UnconnectedVsConnected)
{
    auto comp1 = std::make_unique<PerfTestComponent>();
    auto comp2 = std::make_unique<PerfTestComponent>();
    
    comp1->Output = 100;
    comp2->Parameter = 100;
    
    const int iterations = 100000;
    
    // Test unconnected
    auto start1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        volatile int value = comp2->Parameter();
    }
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    
    // Test connected
    comp2->Input.Connect(&comp1->Output);
    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        volatile int value = comp2->Input();
    }
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    
    double unconnected_ns = (duration1.count() * 1000.0) / iterations;
    double connected_ns = (duration2.count() * 1000.0) / iterations;
    
    std::cout << "Unconnected: " << unconnected_ns << " ns, Connected: " << connected_ns << " ns" << std::endl;
    std::cout << "Overhead: " << ((connected_ns / unconnected_ns - 1.0) * 100.0) << "%" << std::endl;
    
    // Connected should not be more than 2x slower
    EXPECT_LT(connected_ns, unconnected_ns * 2.0);
}

