#ifndef MODERN_BOOST_REPLACEMENT_H
#define MODERN_BOOST_REPLACEMENT_H

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <chrono>
#include <functional>

namespace RDK {
namespace boost_compat {

// ============================================================================
// Thread and Synchronization Replacements
// ============================================================================

// Replace boost::thread with std::jthread (auto-join)
using thread = std::jthread;

// Replace boost::mutex with std::mutex
using mutex = std::mutex;

// Replace boost::shared_mutex with std::shared_mutex
using shared_mutex = std::shared_mutex;

// Replace boost::condition_variable with std::condition_variable
using condition_variable = std::condition_variable;

// Lock types
template<typename T>
using unique_lock = std::unique_lock<T>;

template<typename T>
using shared_lock = std::shared_lock<T>;

template<typename T>
using lock_guard = std::lock_guard<T>;

// ============================================================================
// Time and Date Replacements
// ============================================================================

// Replace boost::posix_time::ptime with std::chrono::time_point
using ptime = std::chrono::system_clock::time_point;

// Replace boost::gregorian::date with std::chrono::year_month_day
using date = std::chrono::year_month_day;

// Duration types
using time_duration = std::chrono::system_clock::duration;
using milliseconds = std::chrono::milliseconds;
using seconds = std::chrono::seconds;

// ============================================================================
// Utility Functions
// ============================================================================

// Get current time (replaces boost::posix_time::microsec_clock::universal_time())
inline ptime microsec_clock_universal_time() {
    return std::chrono::system_clock::now();
}

// Get current time (replaces boost::posix_time::second_clock::universal_time())
inline ptime second_clock_universal_time() {
    return std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now());
}

// Convert time_point to milliseconds since epoch
inline uint64_t to_milliseconds(const ptime& time_point) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        time_point.time_since_epoch()).count();
}

// Convert time_point to seconds since epoch
inline uint64_t to_seconds(const ptime& time_point) {
    return std::chrono::duration_cast<std::chrono::seconds>(
        time_point.time_since_epoch()).count();
}

// Create time_point from milliseconds since epoch
inline ptime from_milliseconds(uint64_t ms) {
    return std::chrono::system_clock::time_point(
        std::chrono::milliseconds(ms));
}

// Create time_point from seconds since epoch
inline ptime from_seconds(uint64_t sec) {
    return std::chrono::system_clock::time_point(
        std::chrono::seconds(sec));
}

// ============================================================================
// Exception Replacements
// ============================================================================

// Replace boost::exception with std::exception
class exception : public std::exception {
public:
    explicit exception(const std::string& message) : message_(message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

// ============================================================================
// Bind Replacement (use lambdas instead)
// ============================================================================

// Helper for creating lambda functions to replace boost::bind
template<typename F, typename... Args>
auto make_lambda(F&& f, Args&&... args) {
    return [f = std::forward<F>(f), args...](auto&&... params) {
        return f(args..., std::forward<decltype(params)>(params)...);
    };
}

} // namespace boost_compat

// ============================================================================
// Migration helpers (use boost_compat instead of boost alias)
// ============================================================================

// Note: We don't create a global boost alias to avoid conflicts with real Boost
// Use boost_compat:: namespace directly in migrated code

} // namespace RDK

#endif // MODERN_BOOST_REPLACEMENT_H
