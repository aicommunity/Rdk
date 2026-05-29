#ifndef RDK_ULLM_NAME_RESOLUTION_H
#define RDK_ULLM_NAME_RESOLUTION_H

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

enum class DisambiguationKind { None, Class, Component };

struct RegisteredClassResolution {
    enum class Status { Resolved, Ambiguous, NotFound };
    Status status = Status::NotFound;
    std::string class_name;
    std::vector<std::pair<std::string, double>> candidates;
};

struct ComponentEntityResolution {
    enum class Status { Resolved, Ambiguous, NotFound };
    Status status = Status::NotFound;
    std::string canonical_long_name;
    std::vector<std::pair<std::string, double>> candidates;
};

bool isRegisteredClassName(const std::vector<std::string>& registered, const std::string& name);
std::string canonicalRegisteredClassName(const std::vector<std::string>& registered,
                                         const std::string& name);

std::string extractClassNameQuery(const std::string& class_name_field, const std::string& user_text);
std::string extractClassNameTokenFromUserText(const std::string& user_text);

RegisteredClassResolution resolveRegisteredClassName(const std::string& query,
                                                     const std::vector<std::string>& registered);

ComponentEntityResolution resolveComponentEntity(
    const std::string& query, const nlohmann::json& snapshot_components,
    const std::optional<std::string>& class_filter = std::nullopt);

std::optional<std::string> pickFromNumberedList(const std::string& user_text,
                                                const nlohmann::json& candidates,
                                                const char* value_key);

bool looksLikeClassIdentifier(const std::string& token);

} // namespace RDK::LLM

#endif
