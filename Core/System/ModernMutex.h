#ifndef MODERN_MUTEX_H
#define MODERN_MUTEX_H

#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <chrono>

namespace RDK {

// Type aliases for modern mutexes and locks
using UMutex = std::mutex;
using USharedMutex = std::shared_mutex;
using UConditionVariable = std::condition_variable;

// RAII Lock Guards
using ULockGuard = std::lock_guard<UMutex>;
using UUniqueLock = std::unique_lock<UMutex>;
using USharedLock = std::shared_lock<USharedMutex>;
using UUpgradeLock = std::unique_lock<USharedMutex>; // For upgrading shared to exclusive

// Helper for condition variable wait with timeout
template<typename Predicate>
bool UWaitWithTimeout(UConditionVariable& cv, UUniqueLock& lock, std::chrono::milliseconds timeout, Predicate pred) {
    return cv.wait_for(lock, timeout, pred);
}

} // namespace RDK

#endif // MODERN_MUTEX_H