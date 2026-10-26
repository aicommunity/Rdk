#ifndef UCLASS_IMAGE_REGISTRY_H
#define UCLASS_IMAGE_REGISTRY_H

#include "UContainer.h"
#include "UStorage.h"
#include <stdexcept>

namespace RDK {

class RDK_LIB_TYPE ClassImageError : public std::runtime_error
{
public:
 const std::string code;
 ClassImageError(const std::string& reason, const std::string& detail)
  : std::runtime_error(reason + ": " + detail), code(reason) {}
};

struct ClassImageRegistration
{
 bool created;
 // Exact process-local comparison descriptor, not a portable scientific hash.
 std::string descriptor;
};

// Read-only parameter/structure/binding description. Identity and dynamic state
// are excluded. V2 canonicalizes Environment-owned TimeStep visibility only;
// its numeric frequency remains significant. Comparison is process-local.
// The caller must stop calculation and graph editing.
RDK_LIB_TYPE std::string DescribeClassImage(UEPtr<UContainer> prototype);

// Construct a separate native parameter copy, initialize it, verify equivalence,
// and register it as a new immutable class image. The caller retains its source.
// Existing names are idempotent only for exactly equivalent supported images.
// Call before creating/running experiment instances; no shared image is edited.
RDK_LIB_TYPE ClassImageRegistration RegisterClassImage(
 UEPtr<UStorage> storage, const std::string& name, UEPtr<UContainer> prototype);

}
#endif
