#ifndef RDK_ULLM_THINKING_PARSE_H
#define RDK_ULLM_THINKING_PARSE_H

#include "../LlmTypes.h"

#include <nlohmann/json.hpp>
#include <string>

namespace RDK::LLM {

/// Extract Ollama/OpenAI thinking fields from a message or delta object.
/// qwen3 via `/v1/chat/completions` uses `reasoning`; native/OpenAI-style may use
/// `thinking` or `reasoning_content`.
inline void extractThinkingFields(const nlohmann::json& obj, std::string& thinking_out)
{
    auto append_if_string = [&](const char* key) {
        if(obj.contains(key) && obj[key].is_string())
        {
            const std::string piece = obj[key].get<std::string>();
            if(!piece.empty())
                thinking_out += piece;
        }
    };
    append_if_string("thinking");
    append_if_string("reasoning_content");
    append_if_string("reasoning");
}

/// If the model leaked `<think>...</think>` into content, split into thinking + answer.
inline void splitThinkTagsFromContent(std::string& text_inout, std::string& thinking_inout)
{
    constexpr const char* kOpen = "<think>";
    constexpr const char* kClose = "</think>";
    for(;;)
    {
        const auto open_pos = text_inout.find(kOpen);
        if(open_pos == std::string::npos)
            break;
        const auto close_pos = text_inout.find(kClose, open_pos + 7);
        if(close_pos == std::string::npos)
            break;
        const std::string inner = text_inout.substr(open_pos + 7, close_pos - (open_pos + 7));
        if(!thinking_inout.empty() && !inner.empty())
            thinking_inout += "\n";
        thinking_inout += inner;
        text_inout.erase(open_pos, (close_pos + 8) - open_pos);
    }
    // Trim leading whitespace left after stripping tags.
    while(!text_inout.empty() && (text_inout.front() == '\n' || text_inout.front() == '\r'))
        text_inout.erase(text_inout.begin());
}

inline bool shouldSendThinkTrue(const LLMCompletionOptions& opts,
                                const LLMProviderCapabilities& caps)
{
    if(!caps.supports_thinking)
        return false;
    return opts.think_mode == LLMThinkMode::On || opts.think_mode == LLMThinkMode::Auto;
}

inline void finalizeThinkingResult(LLMCompletionResult& result)
{
    if(result.thinking.empty() && !result.text.empty())
        splitThinkTagsFromContent(result.text, result.thinking);
}

/// Ollama may return tool `arguments` as a JSON object; OpenAI-compat often uses a string.
inline nlohmann::json parseToolCallArgumentsJson(const nlohmann::json& function_obj)
{
    if(!function_obj.contains("arguments"))
        return nlohmann::json::object();
    const auto& args = function_obj["arguments"];
    if(args.is_object() || args.is_array())
        return args;
    if(args.is_string())
    {
        const std::string s = args.get<std::string>();
        if(s.empty())
            return nlohmann::json::object();
        try
        {
            return nlohmann::json::parse(s);
        }
        catch(...)
        {
            return nlohmann::json::object();
        }
    }
    return nlohmann::json::object();
}

} // namespace RDK::LLM

#endif
