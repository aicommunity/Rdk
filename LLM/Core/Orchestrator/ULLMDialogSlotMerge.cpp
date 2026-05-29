#include "ULLMDialogSlotMerge.h"

#include "ULLMConnectPlanParsing.h"

#include <cctype>
#include <regex>

namespace RDK::LLM {

namespace {

std::string trim(const std::string& s)
{
    size_t b = 0;
    while(b < s.size() && std::isspace(static_cast<unsigned char>(s[b])))
        ++b;
    size_t e = s.size();
    while(e > b && std::isspace(static_cast<unsigned char>(s[e - 1])))
        --e;
    return s.substr(b, e - b);
}

bool jsonStringFieldEmpty(const nlohmann::json& args, const char* key)
{
    if(!args.contains(key))
        return true;
    if(!args[key].is_string())
        return false;
    return trim(args[key].get<std::string>()).empty();
}

bool isNumericOnly(const std::string& s)
{
    const std::string trimmed = trim(s);
    if(trimmed.empty())
        return false;
    for(char c : trimmed)
    {
        if(!std::isdigit(static_cast<unsigned char>(c)))
            return false;
    }
    return true;
}

std::string lastUserMessageText(const std::vector<LLMMessage>& messages)
{
    for(auto it = messages.rbegin(); it != messages.rend(); ++it)
    {
        if(it->role == LLMMessage::Role::User)
            return trim(it->content);
    }
    return {};
}

std::vector<std::string> collectUserMessageTexts(const std::vector<LLMMessage>& messages,
                                                  std::size_t max_messages)
{
    std::vector<std::string> out;
    out.reserve(max_messages);
    for(auto it = messages.rbegin(); it != messages.rend() && out.size() < max_messages; ++it)
    {
        if(it->role != LLMMessage::Role::User)
            continue;
        const std::string text = trim(it->content);
        if(!text.empty())
            out.push_back(text);
    }
    std::reverse(out.begin(), out.end());
    return out;
}

void applyConnectLinkSpec(nlohmann::json& partial, const ConnectLinkSpec& link)
{
    if(!link.from.token.empty() && jsonStringFieldEmpty(partial, "from_long_name"))
        partial["from_long_name"] = link.from.token;
    if(!link.from.property.empty() && jsonStringFieldEmpty(partial, "from_property"))
        partial["from_property"] = link.from.property;
    if(!link.to.token.empty() && jsonStringFieldEmpty(partial, "to_long_name"))
        partial["to_long_name"] = link.to.token;
    if(!link.to.property.empty() && jsonStringFieldEmpty(partial, "to_property"))
        partial["to_property"] = link.to.property;
}

std::vector<std::string> extractDottedIdentifiers(const std::string& text)
{
    static const std::regex dotted(
        R"(([A-Za-z][A-Za-z0-9_]*(?:\.[A-Za-z0-9_]+)+))");
    std::vector<std::string> out;
    for(std::sregex_iterator it(text.begin(), text.end(), dotted), end; it != end; ++it)
    {
        const std::string token = (*it)[1].str();
        if(!token.empty())
            out.push_back(token);
    }
    return out;
}

void applyStandalonePortToken(nlohmann::json& partial, const std::string& token)
{
    const bool endpoints_known = !jsonStringFieldEmpty(partial, "from_long_name")
                                 || !jsonStringFieldEmpty(partial, "to_long_name");
    if(endpoints_known)
    {
        if(jsonStringFieldEmpty(partial, "from_property"))
            partial["from_property"] = token;
        else if(jsonStringFieldEmpty(partial, "to_property"))
            partial["to_property"] = token;
        return;
    }

    const size_t dot = token.rfind('.');
    if(dot == std::string::npos || dot + 1 >= token.size())
        return;

    const std::string left = token.substr(0, dot);
    const std::string right = token.substr(dot + 1);
    if(jsonStringFieldEmpty(partial, "from_property"))
    {
        if(jsonStringFieldEmpty(partial, "from_long_name"))
            partial["from_long_name"] = left;
        partial["from_property"] = right;
    }
    else if(jsonStringFieldEmpty(partial, "to_property"))
    {
        if(jsonStringFieldEmpty(partial, "to_long_name"))
            partial["to_long_name"] = left;
        partial["to_property"] = right;
    }
}

bool looksLikeComponentIdentifier(const std::string& token)
{
    if(token.empty())
        return false;
    if(!std::isalpha(static_cast<unsigned char>(token[0])))
        return false;
    for(char c : token)
    {
        if(std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '/' || c == '.')
            continue;
        return false;
    }
    return true;
}

void applyStandaloneComponentToken(nlohmann::json& partial, const std::string& token)
{
    if(!looksLikeComponentIdentifier(token))
        return;
    if(jsonStringFieldEmpty(partial, "from_long_name"))
        partial["from_long_name"] = token;
    else if(jsonStringFieldEmpty(partial, "to_long_name")
            && partial.value("from_long_name", "") != token)
        partial["to_long_name"] = token;
}

void mergeConnectFromUserText(nlohmann::json& partial, const std::string& user_text)
{
    if(isValidateConfigurationGoalText(user_text))
        return;

    const ParsedConnectGoal parsed = parseConnectGoal(user_text);
    for(const ConnectLinkSpec& link : parsed.explicit_links)
        applyConnectLinkSpec(partial, link);

    const std::vector<std::string> dotted = extractDottedIdentifiers(user_text);
    if(parsed.explicit_links.empty())
    {
        if(dotted.empty())
        {
            const std::string trimmed = trim(user_text);
            if(trimmed.find_first_of(" \t\n\r") == std::string::npos)
                applyStandaloneComponentToken(partial, trimmed);
        }
        for(const std::string& token : dotted)
            applyStandalonePortToken(partial, token);
    }
    else
    {
        for(const std::string& token : dotted)
        {
            bool used_in_link = false;
            for(const ConnectLinkSpec& link : parsed.explicit_links)
            {
                if(link.from.token == token || link.to.token == token
                   || (link.from.token + "." + link.from.property) == token
                   || (link.to.token + "." + link.to.property) == token)
                {
                    used_in_link = true;
                    break;
                }
            }
            if(!used_in_link)
                applyStandalonePortToken(partial, token);
        }
    }
}

bool hasConnectEndpointFields(const nlohmann::json& partial)
{
    return !jsonStringFieldEmpty(partial, "from_long_name")
           || !jsonStringFieldEmpty(partial, "to_long_name")
           || !jsonStringFieldEmpty(partial, "from_property")
           || !jsonStringFieldEmpty(partial, "to_property");
}

std::optional<std::string> classHintFromLongName(const std::string& long_name)
{
    if(long_name.empty())
        return std::nullopt;
    const size_t slash = long_name.rfind('/');
    const std::string tail =
        slash == std::string::npos ? long_name : long_name.substr(slash + 1);
    const size_t dot = tail.find('.');
    const std::string base = dot == std::string::npos ? tail : tail.substr(0, dot);
    if(base.empty())
        return std::nullopt;
    return base;
}

} // namespace

DialogSlotMergeResult mergeDialogSlotsIntoToolArgs(const DialogSlotMergeInput& in)
{
    DialogSlotMergeResult out;
    static const std::vector<LLMMessage> kEmptyMessages;
    const std::vector<LLMMessage>& messages =
        in.messages ? *in.messages : kEmptyMessages;

    nlohmann::json partial = nlohmann::json::object();
    if(in.pending && in.pending->partial_arguments.is_object())
        partial = in.pending->partial_arguments;

    if(in.pending && in.pending->tool_name == "open_recent_configuration")
    {
        const std::string last_user = lastUserMessageText(messages);
        if(isNumericOnly(last_user))
        {
            out.partial_arguments =
                mergeArgumentsFromUserText(*in.pending, last_user, nullptr);
            out.inferred_tool_name = "open_recent_configuration";
            return out;
        }
    }

    const std::vector<std::string> user_texts = collectUserMessageTexts(messages, 8);
    for(const std::string& user_text : user_texts)
        mergeConnectFromUserText(partial, user_text);

    if(in.pending && !in.pending->tool_name.empty() && in.pending->tool_name != "connect_components")
    {
        const std::string last_user = lastUserMessageText(messages);
        if(!last_user.empty())
        {
            const nlohmann::json lifecycle_merged =
                mergeArgumentsFromUserText(*in.pending, last_user, nullptr);
            for(auto it = lifecycle_merged.begin(); it != lifecycle_merged.end(); ++it)
            {
                if(!partial.contains(it.key()) || partial[it.key()].is_null()
                   || (partial[it.key()].is_string()
                       && trim(partial[it.key()].get<std::string>()).empty()))
                    partial[it.key()] = it.value();
            }
            out.inferred_tool_name = in.pending->tool_name;
        }
    }

    out.partial_arguments = partial;
    out.has_connect_endpoints = hasConnectEndpointFields(partial);
    if(out.has_connect_endpoints)
        out.inferred_tool_name =
            in.inferred_tool_name.empty() ? "connect_components" : in.inferred_tool_name;
    else if(out.inferred_tool_name.empty() && in.pending && !in.pending->tool_name.empty())
        out.inferred_tool_name = in.pending->tool_name;

    if(out.has_connect_endpoints)
    {
        if(!jsonStringFieldEmpty(partial, "from_long_name"))
            out.from_class = classHintFromLongName(partial["from_long_name"].get<std::string>());
        if(!jsonStringFieldEmpty(partial, "to_long_name"))
            out.to_class = classHintFromLongName(partial["to_long_name"].get<std::string>());
    }

    return out;
}

} // namespace RDK::LLM
