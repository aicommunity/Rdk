#ifndef RDK_ULLM_TOOL_ARGUMENT_VALIDATOR_H
#define RDK_ULLM_TOOL_ARGUMENT_VALIDATOR_H

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

class ULLMToolArgumentValidator {
public:
    /// Strip unknown keys (when additionalProperties=false) and coerce numeric types for schema.
    nlohmann::json normalizeForSchema(const nlohmann::json& args,
                                     const nlohmann::json& schema) const;

    bool validate(const nlohmann::json& args, const nlohmann::json& schema,
                  std::string& error_out) const;
};

} // namespace RDK::LLM

#endif
