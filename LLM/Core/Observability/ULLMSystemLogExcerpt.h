#ifndef RDK_ULLM_SYSTEM_LOG_EXCERPT_H
#define RDK_ULLM_SYSTEM_LOG_EXCERPT_H

#include <nlohmann/json.hpp>
#include <string>

namespace RDK::LLM {

struct ToolGatewayResult;

nlohmann::json toolJsonWithSystemLogExcerpt(const ToolGatewayResult& tr, const std::string& excerpt);

} // namespace RDK::LLM

#endif
