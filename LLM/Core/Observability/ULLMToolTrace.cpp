#include "ULLMToolTrace.h"

#include "../Session/ULLMConversationStore.h"

#include <cctype>

namespace RDK::LLM {

namespace {

constexpr std::size_t kMaxDisplayStringLen = 200;

bool isSensitiveFieldName(const std::string& key)
{
    std::string lower = key;
    for(char& c : lower)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return lower.find("password") != std::string::npos || lower.find("api_key") != std::string::npos
           || lower.find("secret") != std::string::npos || lower.find("token") != std::string::npos
           || lower.find("credential") != std::string::npos;
}

bool schemaMarksSensitive(const nlohmann::json& input_schema, const std::string& key)
{
    if(!input_schema.is_object() || !input_schema.contains("properties")
       || !input_schema["properties"].is_object())
        return false;
    const nlohmann::json& prop = input_schema["properties"].value(key, nlohmann::json{});
    if(!prop.is_object())
        return false;
    if(prop.value("x-llm-sensitive", false))
        return true;
    const std::string format = prop.value("format", "");
    return format == "password" || format == "secret";
}

std::string truncateDisplayString(const std::string& s)
{
    if(s.size() <= kMaxDisplayStringLen)
        return s;
    return s.substr(0, kMaxDisplayStringLen) + "…";
}

nlohmann::json sanitizeValue(const nlohmann::json& value, const std::string& key,
                            const nlohmann::json& input_schema)
{
    if(value.is_null())
        return nullptr;
    if(value.is_string())
    {
        if(isSensitiveFieldName(key) || schemaMarksSensitive(input_schema, key))
            return "***";
        return truncateDisplayString(value.get<std::string>());
    }
    if(value.is_array())
    {
        nlohmann::json out = nlohmann::json::array();
        for(const auto& item : value)
            out.push_back(sanitizeValue(item, key, input_schema));
        return out;
    }
    if(value.is_object())
    {
        nlohmann::json out = nlohmann::json::object();
        for(auto it = value.begin(); it != value.end(); ++it)
            out[it.key()] = sanitizeValue(it.value(), it.key(), input_schema);
        return out;
    }
    return value;
}

std::string escapeHtml(const std::string& text)
{
    std::string out;
    out.reserve(text.size());
    for(char c : text)
    {
        switch(c)
        {
        case '&':
            out += "&amp;";
            break;
        case '<':
            out += "&lt;";
            break;
        case '>':
            out += "&gt;";
            break;
        case '"':
            out += "&quot;";
            break;
        default:
            out += c;
            break;
        }
    }
    return out;
}

std::string formatArgumentsBlock(const nlohmann::json& args)
{
    if(!args.is_object() || args.empty())
        return "  (no arguments)\n";
    std::string out;
    for(auto it = args.begin(); it != args.end(); ++it)
    {
        out += "  ";
        out += it.key();
        out += ": ";
        if(it.value().is_string())
            out += it.value().get<std::string>();
        else
            out += it.value().dump();
        out += "\n";
    }
    return out;
}

} // namespace

nlohmann::json sanitizeToolArgumentsForDisplay(const nlohmann::json& args,
                                               const nlohmann::json& input_schema)
{
    if(args.is_object())
    {
        nlohmann::json out = nlohmann::json::object();
        for(auto it = args.begin(); it != args.end(); ++it)
            out[it.key()] = sanitizeValue(it.value(), it.key(), input_schema);
        return out;
    }
    return sanitizeValue(args, "", input_schema);
}

void recordTurnToolInvocation(ConversationState& state, const std::string& tool_name,
                              const nlohmann::json& arguments, const ToolGatewayResult& result,
                              int duration_ms, const nlohmann::json& input_schema)
{
    TurnToolInvocationView view;
    view.tool_name = tool_name;
    view.arguments = sanitizeToolArgumentsForDisplay(arguments, input_schema);
    view.ok = result.ok;
    view.error_code = result.error_code;
    view.message = result.message;
    view.duration_ms = duration_ms;
    view.pending_confirmation = result.pending_confirmation;
    state.current_turn_tool_trace.push_back(std::move(view));
}

std::string formatTurnToolTraceHtml(const std::vector<TurnToolInvocationView>& trace)
{
    if(trace.empty())
        return {};

    std::string body;
    for(const TurnToolInvocationView& entry : trace)
    {
        body += escapeHtml(entry.tool_name);
        body += "\n";
        body += escapeHtml(formatArgumentsBlock(entry.arguments));
        if(entry.pending_confirmation)
            body += "→ pending confirmation\n";
        else if(entry.ok)
            body += "→ ok";
        else
            body += "→ error";
        if(!entry.error_code.empty())
        {
            body += " (";
            body += escapeHtml(entry.error_code);
            body += ")";
        }
        if(!entry.message.empty())
        {
            body += ": ";
            body += escapeHtml(truncateDisplayString(entry.message));
        }
        if(entry.duration_ms > 0)
        {
            body += " [";
            body += std::to_string(entry.duration_ms);
            body += " ms]";
        }
        body += "\n\n";
    }

    std::string summary = "Tools (" + std::to_string(trace.size()) + ")";
    return "<details><summary>" + escapeHtml(summary) + "</summary><pre>" + body + "</pre></details>";
}

} // namespace RDK::LLM
