#include "ULLMToolArgumentValidator.h"

namespace RDK::LLM {

bool ULLMToolArgumentValidator::validate(const nlohmann::json& args,
                                         const nlohmann::json& schema,
                                         std::string& error_out) const
{
    if(!schema.is_object())
        return true;
    if(schema.contains("required") && schema["required"].is_array())
    {
        for(const auto& req : schema["required"])
        {
            const std::string key = req.get<std::string>();
            if(!args.contains(key))
            {
                error_out = "Missing required field: " + key;
                return false;
            }
        }
    }
    if(schema.value("additionalProperties", true) == false)
    {
        for(auto it = args.begin(); it != args.end(); ++it)
        {
            if(!schema.contains("properties") || !schema["properties"].contains(it.key()))
            {
                error_out = "Unknown property: " + it.key();
                return false;
            }
        }
    }
    return true;
}

} // namespace RDK::LLM
