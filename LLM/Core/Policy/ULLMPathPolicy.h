#ifndef RDK_ULLM_PATH_POLICY_H
#define RDK_ULLM_PATH_POLICY_H

#include <string>
#include <vector>

namespace RDK {
class UApplication;
}

namespace RDK::LLM {

class ULLMPathPolicy {
public:
    static bool isStrictEnabled();
    static std::string normalizePath(const std::string& path, std::string& err);
    static bool isAllowed(const std::string& path, const RDK::UApplication* app, std::string& err);
    static std::vector<std::string> allowedRoots(const RDK::UApplication* app);
};

} // namespace RDK::LLM

#endif
