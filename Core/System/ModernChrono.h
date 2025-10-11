#ifndef MODERN_CHRONO_H
#define MODERN_CHRONO_H

#include "NamespaceIsolation.h"
#include <string>

namespace RDK {

// ============================================================================
// Modern Time Types - Using Isolated Namespace to Avoid Operator Conflicts
// ============================================================================

using TimePoint = Isolated::TimePoint;
using Duration = Isolated::Duration;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;
using Microseconds = std::chrono::microseconds;

// ============================================================================
// Time Utility Functions
// ============================================================================

// Get current time
inline TimePoint GetCurrentTime() {
    return Isolated::GetCurrentTime();
}

// Get current time in milliseconds since epoch
inline uint64_t GetCurrentTimeMs() {
    return std::chrono::duration_cast<Milliseconds>(
        GetCurrentTime().time_since_epoch()).count();
}

// Get current time in seconds since epoch
inline uint64_t GetCurrentTimeSec() {
    return std::chrono::duration_cast<Seconds>(
        GetCurrentTime().time_since_epoch()).count();
}

// Convert time_point to milliseconds since epoch
inline uint64_t ToMilliseconds(const TimePoint& time_point) {
    return std::chrono::duration_cast<Milliseconds>(
        time_point.time_since_epoch()).count();
}

// Convert time_point to seconds since epoch
inline uint64_t ToSeconds(const TimePoint& time_point) {
    return std::chrono::duration_cast<Seconds>(
        time_point.time_since_epoch()).count();
}

// Create time_point from milliseconds since epoch
inline TimePoint FromMilliseconds(uint64_t ms) {
    return std::chrono::system_clock::time_point(Milliseconds(ms));
}

// Create time_point from seconds since epoch
inline TimePoint FromSeconds(uint64_t sec) {
    return std::chrono::system_clock::time_point(Seconds(sec));
}

// ============================================================================
// Duration Utilities
// ============================================================================

// Sleep for specified duration
inline void SleepFor(const Duration& duration) {
    std::this_thread::sleep_for(duration);
}

// Sleep for milliseconds
inline void SleepForMs(uint64_t ms) {
    SleepFor(Milliseconds(ms));
}

// Sleep for seconds
inline void SleepForSec(uint64_t sec) {
    SleepFor(Seconds(sec));
}

// ============================================================================
// Epoch Constants
// ============================================================================

// Unix epoch (January 1, 1970)
inline constexpr TimePoint UnixEpoch() {
    return std::chrono::system_clock::time_point{};
}

// Custom epoch for RDK (if needed)
inline TimePoint RdkEpoch() {
    // Default to Unix epoch, can be customized if needed
    return UnixEpoch();
}

} // namespace RDK

#endif // MODERN_CHRONO_H
