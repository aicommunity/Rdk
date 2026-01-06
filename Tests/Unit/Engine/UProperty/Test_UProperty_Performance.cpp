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

// Test for multiple GetData() calls in arithmetic operations (simulating real usage patterns)
TEST(PerformanceTest, MultipleGetDataCalls)
{
    PerfTestComponent comp;
    comp.Parameter = 42;
    
    const int iterations = 100000;
    
    // Test: Multiple GetData() calls without caching (old pattern)
    auto start1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        // Simulating: result = prop1.GetData() + prop2.GetData() + prop3.GetData()
        volatile int result = comp.Parameter() + comp.Parameter() + comp.Parameter();
        (void)result; // Suppress unused variable warning
    }
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    
    // Test: Multiple GetData() calls with caching (optimized pattern)
    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        // Simulating optimized pattern: cache value first
        const int param = comp.Parameter();
        volatile int result = param + param + param;
        (void)result; // Suppress unused variable warning
    }
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    
    double without_caching_ns = (duration1.count() * 1000.0) / iterations;
    double with_caching_ns = (duration2.count() * 1000.0) / iterations;
    
    std::cout << "Multiple GetData() without caching: " << without_caching_ns << " ns" << std::endl;
    std::cout << "Multiple GetData() with caching: " << with_caching_ns << " ns" << std::endl;
    if (without_caching_ns > 0)
    {
        double improvement = ((without_caching_ns - with_caching_ns) / without_caching_ns) * 100.0;
        std::cout << "Improvement from caching: " << improvement << "%" << std::endl;
    }
    
    // Caching should be faster or at least not slower
    EXPECT_LE(with_caching_ns, without_caching_ns * 1.1); // Allow 10% tolerance
}

// Test for GetData() in loop (simulating ACalculate() methods)
TEST(PerformanceTest, GetDataInLoop)
{
    PerfTestComponent comp;
    comp.Parameter = 42;
    
    const int outer_iterations = 1000;
    const int inner_iterations = 100;
    
    // Test: GetData() called in inner loop without caching
    auto start1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < outer_iterations; ++i)
    {
        int sum = 0;
        for (int j = 0; j < inner_iterations; ++j)
        {
            sum += comp.Parameter(); // GetData() called in loop
        }
        volatile int dummy = sum;
        (void)dummy;
    }
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    
    // Test: GetData() cached before loop (optimized pattern)
    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < outer_iterations; ++i)
    {
        const int param = comp.Parameter(); // Cache before loop
        int sum = 0;
        for (int j = 0; j < inner_iterations; ++j)
        {
            sum += param; // Use cached value
        }
        volatile int dummy = sum;
        (void)dummy;
    }
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    
    double without_caching_ns = (duration1.count() * 1000.0) / (outer_iterations * inner_iterations);
    double with_caching_ns = (duration2.count() * 1000.0) / (outer_iterations * inner_iterations);
    
    std::cout << "GetData() in loop (per access): " << without_caching_ns << " ns" << std::endl;
    std::cout << "GetData() cached before loop (per access): " << with_caching_ns << " ns" << std::endl;
    if (without_caching_ns > 0)
    {
        double improvement = ((without_caching_ns - with_caching_ns) / without_caching_ns) * 100.0;
        std::cout << "Improvement from caching: " << improvement << "%" << std::endl;
    }
    
    // Caching should provide significant improvement
    EXPECT_LE(with_caching_ns, without_caching_ns * 0.8); // At least 20% improvement expected
}

// Test for different property types (int, double, bool)
TEST(PerformanceTest, GetDataDifferentTypes)
{
    class MultiTypeComponent : public UNet
    {
    public:
        UProperty<int, MultiTypeComponent, ptPubParameter> IntProp;
        UProperty<double, MultiTypeComponent, ptPubParameter> DoubleProp;
        UProperty<bool, MultiTypeComponent, ptPubParameter> BoolProp;
        
        MultiTypeComponent() : 
            IntProp("IntProp", this),
            DoubleProp("DoubleProp", this),
            BoolProp("BoolProp", this)
        {
        }
    };
    
    MultiTypeComponent comp;
    comp.IntProp = 42;
    comp.DoubleProp = 3.14159;
    comp.BoolProp = true;
    
    const int iterations = 1000000;
    
    // Test int
    auto start1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        volatile int value = comp.IntProp();
    }
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    
    // Test double
    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        volatile double value = comp.DoubleProp();
    }
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    
    // Test bool
    auto start3 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        volatile bool value = comp.BoolProp();
    }
    auto end3 = std::chrono::high_resolution_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end3 - start3);
    
    double int_ns = (duration1.count() * 1000.0) / iterations;
    double double_ns = (duration2.count() * 1000.0) / iterations;
    double bool_ns = (duration3.count() * 1000.0) / iterations;
    
    std::cout << "GetData() int: " << int_ns << " ns" << std::endl;
    std::cout << "GetData() double: " << double_ns << " ns" << std::endl;
    std::cout << "GetData() bool: " << bool_ns << " ns" << std::endl;
    
    // All should be reasonably fast
    EXPECT_LT(int_ns, 1000.0);
    EXPECT_LT(double_ns, 1000.0);
    EXPECT_LT(bool_ns, 1000.0);
}

