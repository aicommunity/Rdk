#include "ULLMAuditSanitizer.h"

#include <set>

namespace RDK::LLM {

namespace {

const std::set<std::string> kRedactedKeys = {
    "content", "user_text", "prompt", "messages", "system_prompt", "raw_prompt", "completion_text"};

bool isRedactedKey(const std::string& key)
{
    return kRedactedKeys.count(key) > 0;
}

void sanitizeInPlace(nlohmann::json& node)
{
    if(node.is_object())
    {
        for(auto it = node.begin(); it != node.end();)
        {
            if(isRedactedKey(it.key()))
            {
                if(it.value().is_string())
                    it.value() = "<redacted:" + std::to_string(it.value().get<std::string>().size()) + " chars>";
                else
                    it.value() = "<redacted>";
                ++it;
            }
            else
            {
                sanitizeInPlace(it.value());
                ++it;
            }
        }
    }
    else if(node.is_array())
    {
        for(auto& element : node)
            sanitizeInPlace(element);
    }
}

} // namespace

nlohmann::json sanitizeAuditDetails(nlohmann::json details)
{
    sanitizeInPlace(details);
    return details;
}

} // namespace RDK::LLM
