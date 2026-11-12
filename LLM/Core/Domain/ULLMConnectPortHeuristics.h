#ifndef RDK_ULLM_CONNECT_PORT_HEURISTICS_H
#define RDK_ULLM_CONNECT_PORT_HEURISTICS_H

#include <optional>
#include <string>
#include <vector>

namespace RDK::LLM {

bool isInhibitoryLinkPortName(const std::string& port);
bool isGenericLinkPortName(const std::string& value);
std::optional<std::string> pickPreferredOutputPort(const std::vector<std::string>& outputs);
std::optional<std::string> pickPreferredInputPort(const std::vector<std::string>& inputs);

} // namespace RDK::LLM

#endif
