#ifndef RDK_ULLM_AUDIT_SANITIZER_H
#define RDK_ULLM_AUDIT_SANITIZER_H

#include <nlohmann/json.hpp>

namespace RDK::LLM {

/// Removes raw prompt / message bodies from audit details (production default).
nlohmann::json sanitizeAuditDetails(nlohmann::json details);

} // namespace RDK::LLM

#endif
