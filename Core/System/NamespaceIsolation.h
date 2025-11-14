//---------------------------------------------------------------------------
// Namespace Isolation for RDK - GCC 11 Compatible
// Prevents conflicts between RDK operators and std::chrono
//---------------------------------------------------------------------------

#ifndef NamespaceIsolationH
#define NamespaceIsolationH

#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
// UPropertyEndpoints.h может быть включен условно после namespace, но лучше включить здесь если нужен
#ifdef RDK_ENABLE_OPERATOR_CONFLICTS
#include "../Engine/UPropertyEndpoints.h"
#endif

namespace RDK {
    namespace Isolated {
        // Isolated chrono types to avoid operator conflicts
        using TimePoint = std::chrono::system_clock::time_point;
        using Duration = std::chrono::system_clock::duration;
        using SteadyTimePoint = std::chrono::steady_clock::time_point;
        
        // Isolated thread types
        using Thread = std::thread;
        using JThread = std::jthread;
        using Mutex = std::mutex;
        using SharedMutex = std::shared_mutex;
        using ConditionVariable = std::condition_variable;
        
        // Template aliases for locks
        template<typename T>
        using UniqueLock = std::unique_lock<T>;
        
        template<typename T>
        using SharedLock = std::shared_lock<T>;
        
        template<typename T>
        using LockGuard = std::lock_guard<T>;
        
        // Time utilities
        inline TimePoint GetCurrentTime() {
            return std::chrono::system_clock::now();
        }
        
        inline SteadyTimePoint GetSteadyTime() {
            return std::chrono::steady_clock::now();
        }
        
        // Duration conversions
        template<typename Rep, typename Period>
        inline Duration ToSystemDuration(const std::chrono::duration<Rep, Period>& d) {
            return std::chrono::duration_cast<Duration>(d);
        }
        
        // Time point conversions
        template<typename Clock, typename Duration>
        inline TimePoint ToSystemTimePoint(const std::chrono::time_point<Clock, Duration>& tp) {
            return std::chrono::time_point_cast<TimePoint::duration>(tp);
        }
    }
}

// Macro to enable/disable RDK operator conflicts
// UPropertyEndpoints.h уже включен в начале файла если RDK_ENABLE_OPERATOR_CONFLICTS определен
#ifndef RDK_ENABLE_OPERATOR_CONFLICTS
    // Define minimal RDK types without conflicting operators
    namespace RDK {
        namespace Minimal {
            // Minimal types without operator conflicts
            template<typename T>
            class SafeProperty {
            public:
                T value;
                SafeProperty() = default;
                SafeProperty(const T& v) : value(v) {}
                SafeProperty(T&& v) : value(std::move(v)) {}
                
                // Safe assignment without operator conflicts
                SafeProperty& assign(const T& v) { value = v; return *this; }
                SafeProperty& assign(T&& v) { value = std::move(v); return *this; }
                
                // Safe access
                T& get() { return value; }
                const T& get() const { return value; }
                
                // Safe comparison
                bool equals(const SafeProperty& other) const { return value == other.value; }
            };
        }
    }
#endif

// Conditional compilation for C++20 features in GCC 11
#if __cpp_lib_ranges >= 201911L
    #define RDK_HAS_RANGES 1
#else
    #define RDK_HAS_RANGES 0
#endif

#if __cpp_concepts >= 202002L
    #define RDK_HAS_CONCEPTS 1
#else
    #define RDK_HAS_CONCEPTS 0
#endif

#if __cpp_lib_jthread >= 201911L
    #define RDK_HAS_JTHREAD 1
#else
    #define RDK_HAS_JTHREAD 0
#endif

// GCC 11 already has jthread, so no fallback needed

#endif // NamespaceIsolationH
