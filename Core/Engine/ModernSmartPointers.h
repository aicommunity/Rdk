#ifndef MODERN_SMART_POINTERS_H
#define MODERN_SMART_POINTERS_H

#include <memory> // For std::shared_ptr, std::unique_ptr, std::make_shared, std::make_unique

namespace RDK {

// Type aliases for smooth migration (using different names to avoid conflicts)
template<typename T>
using ModernUEPtr = std::shared_ptr<T>;

template<typename T>
using ModernUESharedPtr = std::shared_ptr<T>;

template<typename T>
using ModernUUniquePtr = std::unique_ptr<T>;

// Helper functions for creating smart pointers
template<typename T, typename... Args>
ModernUEPtr<T> make_modern_ueptr(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
ModernUUniquePtr<T> make_uuniqueptr(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// Legacy compatibility function
template<typename T, typename... Args>
ModernUEPtr<T> make_ueptr(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// Non-owning shared_ptr deleter - does nothing
struct NonOwningDeleter {
    template<typename T>
    void operator()(T*) const {}
};

} // namespace RDK

#endif // MODERN_SMART_POINTERS_H