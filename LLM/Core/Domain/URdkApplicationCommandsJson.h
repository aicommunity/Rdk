#ifndef RDK_URDK_APPLICATION_COMMANDS_JSON_H
#define RDK_URDK_APPLICATION_COMMANDS_JSON_H

#include <nlohmann/json.hpp>

namespace RDK {
struct TProjectLoadDiagnostics;
}

namespace RDK::LLM {

nlohmann::json diagnosticsToJson(const RDK::TProjectLoadDiagnostics& diagnostics);

} // namespace RDK::LLM

#endif
