//---------------------------------------------------------------------------
// Safe Property Endpoints - GCC 11 Compatible
// Isolated operators to avoid conflicts with std::chrono
//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsSafeH
#define UPropertyEndpointsSafeH

#include "UProperty.h"
#include "../System/NamespaceIsolation.h"

namespace RDK {
    namespace Safe {
        // Safe arithmetic operators that don't conflict with std::chrono
        template<typename T>
        class SafeArithmetic {
        public:
            static T add(const T& a, const T& b) {
                return a + b;
            }
            
            static T subtract(const T& a, const T& b) {
                return a - b;
            }
            
            static T multiply(const T& a, const T& b) {
                return a * b;
            }
            
            static T divide(const T& a, const T& b) {
                return a / b;
            }
            
            static bool equals(const T& a, const T& b) {
                return a == b;
            }
            
            static bool not_equals(const T& a, const T& b) {
                return a != b;
            }
            
            static bool less_than(const T& a, const T& b) {
                return a < b;
            }
            
            static bool greater_than(const T& a, const T& b) {
                return a > b;
            }
        };
        
        // Specialized safe operators for different types
        template<>
        class SafeArithmetic<Isolated::TimePoint> {
        public:
            static Isolated::TimePoint add(const Isolated::TimePoint& a, const Isolated::Duration& b) {
                return a + b;
            }
            
            static Isolated::Duration subtract(const Isolated::TimePoint& a, const Isolated::TimePoint& b) {
                return a - b;
            }
            
            static bool equals(const Isolated::TimePoint& a, const Isolated::TimePoint& b) {
                return a == b;
            }
            
            static bool not_equals(const Isolated::TimePoint& a, const Isolated::TimePoint& b) {
                return a != b;
            }
            
            static bool less_than(const Isolated::TimePoint& a, const Isolated::TimePoint& b) {
                return a < b;
            }
            
            static bool greater_than(const Isolated::TimePoint& a, const Isolated::TimePoint& b) {
                return a > b;
            }
        };
        
        // Safe property wrapper that uses safe operators
        template<typename T, typename OwnerT, unsigned int type>
        class SafeProperty : public UPropertyLocal<T, OwnerT, type> {
        public:
            SafeProperty(const std::string& name, OwnerT* const owner, 
                        typename UPropertyLocal<T, OwnerT, type>::SetterRT setmethod = nullptr)
                : UPropertyLocal<T, OwnerT, type>(name, owner, setmethod) {}
            
            // Safe arithmetic operations
            SafeProperty<T, OwnerT, type> add(const SafeProperty<T, OwnerT, type>& other) const {
                SafeProperty<T, OwnerT, type> result(*this);
                result.v = SafeArithmetic<T>::add(this->v, other.v);
                return result;
            }
            
            SafeProperty<T, OwnerT, type> subtract(const SafeProperty<T, OwnerT, type>& other) const {
                SafeProperty<T, OwnerT, type> result(*this);
                result.v = SafeArithmetic<T>::subtract(this->v, other.v);
                return result;
            }
            
            SafeProperty<T, OwnerT, type> multiply(const SafeProperty<T, OwnerT, type>& other) const {
                SafeProperty<T, OwnerT, type> result(*this);
                result.v = SafeArithmetic<T>::multiply(this->v, other.v);
                return result;
            }
            
            SafeProperty<T, OwnerT, type> divide(const SafeProperty<T, OwnerT, type>& other) const {
                SafeProperty<T, OwnerT, type> result(*this);
                result.v = SafeArithmetic<T>::divide(this->v, other.v);
                return result;
            }
            
            // Safe comparison operations
            bool equals(const SafeProperty<T, OwnerT, type>& other) const {
                return SafeArithmetic<T>::equals(this->v, other.v);
            }
            
            bool not_equals(const SafeProperty<T, OwnerT, type>& other) const {
                return SafeArithmetic<T>::not_equals(this->v, other.v);
            }
            
            bool less_than(const SafeProperty<T, OwnerT, type>& other) const {
                return SafeArithmetic<T>::less_than(this->v, other.v);
            }
            
            bool greater_than(const SafeProperty<T, OwnerT, type>& other) const {
                return SafeArithmetic<T>::greater_than(this->v, other.v);
            }
        };
        
        // Safe matrix operations
        template<typename T>
        class SafeMatrixOperations {
        public:
            template<typename MatrixType>
            static MatrixType add_matrices(const MatrixType& a, const MatrixType& b) {
                return a + b;
            }
            
            template<typename MatrixType>
            static MatrixType subtract_matrices(const MatrixType& a, const MatrixType& b) {
                return a - b;
            }
            
            template<typename MatrixType>
            static MatrixType multiply_matrix_scalar(const MatrixType& a, T scalar) {
                return a * scalar;
            }
            
            template<typename MatrixType>
            static bool matrices_equal(const MatrixType& a, const MatrixType& b) {
                return a == b;
            }
        };
        
        // Safe vector operations
        template<typename T>
        class SafeVectorOperations {
        public:
            template<typename VectorType>
            static VectorType add_vectors(const VectorType& a, const VectorType& b) {
                return a + b;
            }
            
            template<typename VectorType>
            static VectorType subtract_vectors(const VectorType& a, const VectorType& b) {
                return a - b;
            }
            
            template<typename VectorType>
            static VectorType multiply_vector_scalar(const VectorType& a, T scalar) {
                return a * scalar;
            }
            
            template<typename VectorType>
            static bool vectors_equal(const VectorType& a, const VectorType& b) {
                return a == b;
            }
        };
    }
}

// Safe operator functions that can be used instead of global operators
namespace RDK {
    namespace Safe {
        // Safe addition
        template<typename T, typename OwnerT, unsigned int type>
        SafeProperty<T, OwnerT, type> safe_add(const SafeProperty<T, OwnerT, type>& a, const SafeProperty<T, OwnerT, type>& b) {
            return a.add(b);
        }
        
        // Safe subtraction
        template<typename T, typename OwnerT, unsigned int type>
        SafeProperty<T, OwnerT, type> safe_subtract(const SafeProperty<T, OwnerT, type>& a, const SafeProperty<T, OwnerT, type>& b) {
            return a.subtract(b);
        }
        
        // Safe multiplication
        template<typename T, typename OwnerT, unsigned int type>
        SafeProperty<T, OwnerT, type> safe_multiply(const SafeProperty<T, OwnerT, type>& a, const SafeProperty<T, OwnerT, type>& b) {
            return a.multiply(b);
        }
        
        // Safe division
        template<typename T, typename OwnerT, unsigned int type>
        SafeProperty<T, OwnerT, type> safe_divide(const SafeProperty<T, OwnerT, type>& a, const SafeProperty<T, OwnerT, type>& b) {
            return a.divide(b);
        }
        
        // Safe equality
        template<typename T, typename OwnerT, unsigned int type>
        bool safe_equals(const SafeProperty<T, OwnerT, type>& a, const SafeProperty<T, OwnerT, type>& b) {
            return a.equals(b);
        }
        
        // Safe inequality
        template<typename T, typename OwnerT, unsigned int type>
        bool safe_not_equals(const SafeProperty<T, OwnerT, type>& a, const SafeProperty<T, OwnerT, type>& b) {
            return a.not_equals(b);
        }
    }
}

#endif // UPropertyEndpointsSafeH




