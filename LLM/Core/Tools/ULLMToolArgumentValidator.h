#ifndef RDK_ULLM_TOOL_ARGUMENT_VALIDATOR_H
#define RDK_ULLM_TOOL_ARGUMENT_VALIDATOR_H

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

class ULLMToolArgumentValidator {
public:
    bool validate(const nlohmann::json& args, const nlohmann::json& schema,
                  std::string& error_out) const;
};

} // namespace RDK::LLM

#endif
