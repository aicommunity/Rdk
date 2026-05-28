#include "ULLMToolArgumentValidator.h"

namespace RDK::LLM {

namespace {

bool matchesType(const nlohmann::json& value, const std::string& type_name)
{
    if(type_name == "string")
        return value.is_string();
    if(type_name == "number")
        return value.is_number();
    if(type_name == "integer")
        return value.is_number_integer();
    if(type_name == "boolean")
        return value.is_boolean();
    if(type_name == "array")
        return value.is_array();
    if(type_name == "object")
        return value.is_object();
    if(type_name == "null")
        return value.is_null();
    return true;
}

bool validateNumericRange(const nlohmann::json& value, const nlohmann::json& prop_schema,
                          std::string& error_out, const std::string& key)
{
    if(!value.is_number())
        return true;
    const double v = value.get<double>();
    if(prop_schema.contains("minimum") && v < prop_schema["minimum"].get<double>())
    {
        error_out = "Value below minimum for field: " + key;
        return false;
    }
    if(prop_schema.contains("maximum") && v > prop_schema["maximum"].get<double>())
    {
        error_out = "Value above maximum for field: " + key;
        return false;
    }
    return true;
}

} // namespace

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
    if(schema.contains("properties") && schema["properties"].is_object())
    {
        for(auto it = args.begin(); it != args.end(); ++it)
        {
            if(!schema["properties"].contains(it.key()))
                continue;
            const nlohmann::json& prop_schema = schema["properties"][it.key()];
            if(prop_schema.contains("type") && prop_schema["type"].is_string())
            {
                const std::string type_name = prop_schema["type"].get<std::string>();
                if(!matchesType(it.value(), type_name))
                {
                    error_out = "Invalid type for field: " + it.key() + " (expected " + type_name
                                + ")";
                    return false;
                }
            }
            if(prop_schema.contains("enum") && prop_schema["enum"].is_array())
            {
                bool match = false;
                for(const nlohmann::json& allowed : prop_schema["enum"])
                {
                    if(allowed == it.value())
                    {
                        match = true;
                        break;
                    }
                }
                if(!match)
                {
                    error_out = "Invalid enum value for field: " + it.key();
                    return false;
                }
            }
            if(!validateNumericRange(it.value(), prop_schema, error_out, it.key()))
                return false;
        }
    }
    return true;
}

} // namespace RDK::LLM
