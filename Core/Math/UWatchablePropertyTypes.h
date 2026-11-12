#ifndef UWATCHABLE_PROPERTY_TYPES_H
#define UWATCHABLE_PROPERTY_TYPES_H

#include "MDMatrix.h"
#include "MDVector.h"

#include <typeinfo>

namespace RDK
{

/// Types accepted by Watch series binding / matrix picker.
inline bool isWatchableLanguageType(const std::type_info& ti)
{
    return ti == typeid(double) || ti == typeid(int) || ti == typeid(RDK::MDMatrix<double>)
           || ti == typeid(RDK::MDMatrix<int>) || ti == typeid(RDK::MDVector<double>)
           || ti == typeid(RDK::MDVector<int>);
}

inline bool isScalarWatchableLanguageType(const std::type_info& ti)
{
    return ti == typeid(double) || ti == typeid(int);
}

} // namespace RDK

#endif // UWATCHABLE_PROPERTY_TYPES_H
