//---------------------------------------------------------------------------
// Guarded Property Endpoints - Prevents operator conflicts with std::chrono
// This file provides a way to include UPropertyEndpoints.h safely
//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsGuardedH
#define UPropertyEndpointsGuardedH

// Define macro to disable conflicting operators
#define RDK_DISABLE_CONFLICTING_OPERATORS

// Include the original file with guards
#include "UPropertyEndpoints.h"

// Undefine the macro
#undef RDK_DISABLE_CONFLICTING_OPERATORS

// Provide safe alternatives
namespace RDK {
    namespace Safe {
        // Safe arithmetic functions that don't conflict with std::chrono
        template<typename T>
        T safe_add(const T& a, const T& b) {
            return a + b;
        }
        
        template<typename T>
        T safe_subtract(const T& a, const T& b) {
            return a - b;
        }
        
        template<typename T>
        T safe_multiply(const T& a, const T& b) {
            return a * b;
        }
        
        template<typename T>
        T safe_divide(const T& a, const T& b) {
            return a / b;
        }
        
        template<typename T>
        bool safe_equals(const T& a, const T& b) {
            return a == b;
        }
        
        template<typename T>
        bool safe_not_equals(const T& a, const T& b) {
            return a != b;
        }
        
        template<typename T>
        bool safe_less_than(const T& a, const T& b) {
            return a < b;
        }
        
        template<typename T>
        bool safe_greater_than(const T& a, const T& b) {
            return a > b;
        }
    }
}

#endif // UPropertyEndpointsGuardedH




