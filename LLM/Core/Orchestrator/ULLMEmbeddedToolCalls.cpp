#include "ULLMEmbeddedToolCalls.h"

#include <nlohmann/json.hpp>

namespace RDK::LLM {

namespace {

std::optional<nlohmann::json> tryParseJsonObjectAt(const std::string& text, size_t start_brace)
{
    if(start_brace >= text.size() || text[start_brace] != '{')
        return std::nullopt;
    int depth = 0;
    for(size_t i = start_brace; i < text.size(); ++i)
    {
        if(text[i] == '{')
            ++depth;
        else if(text[i] == '}')
        {
            --depth;
            if(depth == 0)
            {
                try
                {
                    return nlohmann::json::parse(text.substr(start_brace, i - start_brace + 1));
                }
                catch(...)
                {
                    return std::nullopt;
                }
            }
        }
    }
    return std::nullopt;
}

} // namespace

std::vector<LLMToolCall> tryExtractEmbeddedToolCalls(const std::string& assistant_text,
                                                     const ULLMToolRegistry& registry)
{
    std::vector<LLMToolCall> out;
    size_t search_from = 0;
    while(true)
    {
        const size_t name_key = assistant_text.find("\"name\"", search_from);
        if(name_key == std::string::npos)
            break;
        search_from = name_key + 6;

        size_t brace = assistant_text.rfind('{', name_key);
        if(brace == std::string::npos)
            continue;

        const std::optional<nlohmann::json> obj = tryParseJsonObjectAt(assistant_text, brace);
        if(!obj || !obj->is_object() || !obj->contains("name"))
            continue;

        const std::string tool_name = (*obj)["name"].get<std::string>();
        if(!registry.find(tool_name))
            continue;

        LLMToolCall call;
        call.id = "embedded-" + std::to_string(out.size());
        call.name = tool_name;
        if(obj->contains("arguments") && (*obj)["arguments"].is_object())
            call.arguments = (*obj)["arguments"];
        else
            call.arguments = nlohmann::json::object();

        bool duplicate = false;
        for(const LLMToolCall& existing : out)
        {
            if(existing.name == call.name)
            {
                duplicate = true;
                break;
            }
        }
        if(!duplicate)
            out.push_back(std::move(call));
    }
    return out;
}

} // namespace RDK::LLM
