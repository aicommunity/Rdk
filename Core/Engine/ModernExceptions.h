#ifndef MODERN_EXCEPTIONS_H
#define MODERN_EXCEPTIONS_H

#include <exception>
#include <string>
#include <optional> // Using std::optional for C++20 compatibility

namespace RDK {

// Modern exception hierarchy
class ModernException : public std::exception {
public:
    explicit ModernException(const std::string& message) : message_(message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

// Specific exception types
class PropertyException : public ModernException {
public:
    explicit PropertyException(const std::string& message) : ModernException(message) {}
};

class ComponentException : public ModernException {
public:
    explicit ComponentException(const std::string& message) : ModernException(message) {}
};

class StorageException : public ModernException {
public:
    explicit StorageException(const std::string& message) : ModernException(message) {}
};

// Error handling without exceptions (C++20 compatible)
template<typename T>
using Result = std::optional<T>; // Using std::optional for C++20 compatibility

// Helper functions for error handling
template<typename T>
Result<T> Success(T&& value) {
    return std::optional<T>(std::forward<T>(value));
}

template<typename T>
Result<T> Error(const std::string& message) {
    // For C++20, we'll use a different approach - return empty optional
    // and store error message separately if needed
    return std::optional<T>();
}

// Check if result is successful
template<typename T>
bool IsSuccess(const Result<T>& result) {
    return result.has_value();
}

// Get value or throw exception
template<typename T>
T GetValueOrThrow(const Result<T>& result) {
    if (result.has_value()) {
        return result.value();
    } else {
        throw ModernException("No value available");
    }
}

} // namespace RDK

#endif // MODERN_EXCEPTIONS_H
