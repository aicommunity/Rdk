#pragma once
#include <string>
#include "../../Deploy/Include/initdll_defs.h"

namespace RDK {
// Nominal is the default for an arbitrary serialization document. The value
// domains are opt-in contracts of an owner that independently verifies the
// actual typed property and its exact value. They are not a binary ABI claim.
enum class UXmlIntegralDomain { Nominal, SignedValue, UnsignedValue };

RDK_LIB_TYPE bool SamePortableXmlType(const std::string& first,const std::string& second,
    UXmlIntegralDomain domain=UXmlIntegralDomain::Nominal,const std::string& value={});
}
