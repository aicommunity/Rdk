#ifndef MODERN_CONTAINERS_H
#define MODERN_CONTAINERS_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include "../System/ConditionalCompilation.h"

namespace RDK {

// Type aliases for common containers
template<typename T>
using UVector = std::vector<T>;

template<typename K, typename V>
using UHashMap = std::unordered_map<K, V>;

template<typename K>
using UHashSet = std::unordered_set<K>;

template<typename K, typename V>
using UOrderedMap = std::map<K, V>;

template<typename K>
using UOrderedSet = std::set<K>;

// ============================================================================
// Container Utilities with C++20 Ranges (fallback to traditional algorithms)
// ============================================================================

// Filter container using ranges (if available) or traditional algorithms
template<typename Container, typename Predicate>
void FilterContainer(Container& container, Predicate&& pred) {
#if RDK_HAS_RANGES
    auto filtered = container | std::views::filter(std::forward<Predicate>(pred));
    container.assign(filtered.begin(), filtered.end());
#else
    // Fallback to traditional algorithm
    container.erase(
        std::remove_if(container.begin(), container.end(), 
                      [&pred](const auto& item) { return !pred(item); }),
        container.end()
    );
#endif
}

// Transform container using ranges (if available) or traditional algorithms
template<typename Container, typename Transform>
void TransformContainer(Container& container, Transform&& transform) {
#if RDK_HAS_RANGES
    auto transformed = container | std::views::transform(std::forward<Transform>(transform));
    container.assign(transformed.begin(), transformed.end());
#else
    // Fallback to traditional algorithm
    std::transform(container.begin(), container.end(), 
                  container.begin(), std::forward<Transform>(transform));
#endif
}

// ============================================================================
// Optimized Container Operations
// ============================================================================

// Reserve capacity for vector
template<typename T>
void ReserveVector(std::vector<T>& vec, size_t size) {
    vec.reserve(size);
}

// Reserve capacity for unordered_map
template<typename K, typename V>
void ReserveHashMap(std::unordered_map<K, V>& map, size_t size) {
    if (size > map.bucket_count()) {
        map.reserve(size);
    }
}

// Reserve capacity for unordered_set
template<typename K>
void ReserveHashSet(std::unordered_set<K>& set, size_t size) {
    if (size > set.bucket_count()) {
        set.reserve(size);
    }
}

// Emplace operations for better performance
template<typename Container, typename... Args>
void EmplaceBack(Container& container, Args&&... args) {
    container.emplace_back(std::forward<Args>(args)...);
}

// ============================================================================
// Modern String Operations
// ============================================================================

// Efficient string concatenation
template<typename... Strings>
std::string ConcatenateStrings(Strings&&... strings) {
    std::string result;
    result.reserve((strings.size() + ...));
    (result.append(strings), ...);
    return result;
}

// String operations with string_view
std::string_view TrimStringView(std::string_view str);
bool StartsWith(std::string_view str, std::string_view prefix);
bool EndsWith(std::string_view str, std::string_view suffix);

// ============================================================================
// Container Iteration with Ranges
// ============================================================================

// Iterate over container with index using ranges (if available) or traditional approach
template<typename Container>
void ForEachWithIndex(const Container& container, std::function<void(size_t, const typename Container::value_type&)> func) {
        #if RDK_HAS_RANGES && RDK_HAS_ENUMERATE
            auto indexed = container | std::views::enumerate;
            for (auto [index, value] : indexed) {
                func(index, value);
            }
        #else
    // Fallback to traditional approach
    size_t index = 0;
    for (const auto& value : container) {
        func(index++, value);
    }
#endif
}

// ============================================================================
// Container Validation and Utilities
// ============================================================================

// Check if container contains element
template<typename Container, typename T>
bool Contains(const Container& container, const T& value) {
#if RDK_HAS_RANGES
    return std::ranges::find(container, value) != container.end();
#else
    return std::find(container.begin(), container.end(), value) != container.end();
#endif
}

// Find element with predicate
template<typename Container, typename Predicate>
auto FindIf(const Container& container, Predicate&& pred) {
#if RDK_HAS_RANGES
    return std::ranges::find_if(container, std::forward<Predicate>(pred));
#else
    return std::find_if(container.begin(), container.end(), std::forward<Predicate>(pred));
#endif
}

// Count elements with predicate
template<typename Container, typename Predicate>
size_t CountIf(const Container& container, Predicate&& pred) {
#if RDK_HAS_RANGES
    return std::ranges::count_if(container, std::forward<Predicate>(pred));
#else
    return std::count_if(container.begin(), container.end(), std::forward<Predicate>(pred));
#endif
}

// ============================================================================
// Specialized Container Types for RDK
// ============================================================================

} // namespace RDK

#endif // MODERN_CONTAINERS_H