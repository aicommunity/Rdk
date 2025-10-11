#ifndef MODERN_PROPERTIES_H
#define MODERN_PROPERTIES_H

#include <string>
#include <variant>
#include <vector>
#include <map>
#include "../System/ConditionalCompilation.h"

namespace RDK {

// Concept for types that can be used as a property value
#if RDK_HAS_CONCEPTS
template<typename T>
concept PropertyValueType = std::is_arithmetic_v<T> || std::is_same_v<T, std::string> || std::is_enum_v<T>;
#else
// Fallback for pre-concepts C++20
template<typename T>
constexpr bool PropertyValueType = std::is_arithmetic_v<T> || std::is_same_v<T, std::string> || std::is_enum_v<T>;
#endif

// A modern, type-safe property using std::variant
class ModernProperty {
public:
    using ValueType = std::variant<int, float, double, bool, std::string>;
    
    ModernProperty(std::string name, ValueType value) 
        : name_(std::move(name)), value_(std::move(value)) {}
    
    const std::string& name() const { return name_; }
    
    template<typename T>
    T get() const {
        static_assert(PropertyValueType<T>, "T must be a valid property value type");
        if (std::holds_alternative<T>(value_)) {
            return std::get<T>(value_);
        }
        // Handle error or conversion if types don't match
        throw std::bad_variant_access();
    }
    
    template<typename T>
    void set(T value) {
        static_assert(PropertyValueType<T>, "T must be a valid property value type");
        value_ = std::move(value);
    }
    
private:
    std::string name_;
    ValueType value_;
};

// Example of a property container using concepts
#if RDK_HAS_CONCEPTS
template<typename T>
concept HasProperties = requires(T t) {
    { t.getProperties() } -> std::same_as<std::vector<ModernProperty>>;
    { t.getProperty("name") } -> std::same_as<ModernProperty>;
};
#else
// Fallback for pre-concepts C++20
template<typename T>
constexpr bool HasProperties = std::is_same_v<decltype(std::declval<T>().getProperties()), std::vector<ModernProperty>>;
#endif

} // namespace RDK

#endif // MODERN_PROPERTIES_H