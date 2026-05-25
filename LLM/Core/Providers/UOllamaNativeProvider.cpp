#include "UOllamaNativeProvider.h"

namespace RDK::LLM {

UOllamaNativeProvider::UOllamaNativeProvider(LLMProviderProfile profile)
    : m_profile(std::move(profile))
{
}

LLMProviderCapabilities UOllamaNativeProvider::capabilities() const
{
    LLMProviderCapabilities c;
    c.supports_tool_calling = false;
    c.supports_streaming = false;
    c.requires_network = true;
    return c;
}

std::string UOllamaNativeProvider::ollamaHost() const
{
    std::string host = m_profile.base_url;
    while(!host.empty() && host.back() == '/')
        host.pop_back();
    const std::string suffix_v1 = "/v1";
    if(host.size() >= suffix_v1.size()
       && host.compare(host.size() - suffix_v1.size(), suffix_v1.size(), suffix_v1) == 0)
        host.erase(host.size() - suffix_v1.size());
    return host;
}

LLMCompletionResult UOllamaNativeProvider::parseResponse(const std::string& body,
                                                         std::string& error) const
{
    LLMCompletionResult result;
    try
    {
        const nlohmann::json j = nlohmann::json::parse(body);
        if(j.contains("error"))
        {
            result.ok = false;
            result.error_message = j["error"].dump();
            return result;
        }
        if(j.contains("message") && j["message"].contains("content"))
            result.text = j["message"]["content"].get<std::string>();
        result.ok = true;
    }
    catch(const std::exception& ex)
    {
        error = ex.what();
        result.ok = false;
        result.error_message = error;
    }
    return result;
}

LLMCompletionResult UOllamaNativeProvider::chat(const std::vector<LLMMessage>& messages,
                                                const LLMCompletionOptions& opts)
{
    (void)opts;
    LLMCompletionResult result;
    nlohmann::json body;
    body["model"] = m_profile.model;
    body["stream"] = false;
    nlohmann::json msgs = nlohmann::json::array();
    for(const LLMMessage& m : messages)
    {
        if(m.role == LLMMessage::Role::Tool)
            continue;
        nlohmann::json item;
        switch(m.role)
        {
        case LLMMessage::Role::System:
            item["role"] = "system";
            break;
        case LLMMessage::Role::Assistant:
            item["role"] = "assistant";
            break;
        default:
            item["role"] = "user";
            break;
        }
        item["content"] = m.content;
        msgs.push_back(item);
    }
    body["messages"] = msgs;

    const std::string url = ollamaHost() + "/api/chat";
    auto resp = m_http.postJson(url, body.dump(), m_profile.api_key);
    if(!resp.error.empty())
    {
        result.ok = false;
        result.error_message = resp.error;
        return result;
    }
    if(resp.status_code < 200 || resp.status_code >= 300)
    {
        result.ok = false;
        result.error_message = "HTTP " + std::to_string(resp.status_code) + ": " + resp.body;
        return result;
    }
    std::string parse_err;
    return parseResponse(resp.body, parse_err);
}

void UOllamaNativeProvider::chatStream(const std::vector<LLMMessage>& messages,
                                       const LLMCompletionOptions& opts, LLMStreamCallback on_chunk,
                                       std::function<void(LLMCompletionResult)> on_done)
{
    LLMCompletionResult r = chat(messages, opts);
    if(!r.text.empty() && on_chunk)
        on_chunk(r.text);
    if(on_done)
        on_done(r);
}

bool UOllamaNativeProvider::healthCheck(std::string& error_out)
{
    LLMCompletionOptions opts;
    opts.max_tokens = 8;
    std::vector<LLMMessage> msgs;
    LLMMessage m;
    m.role = LLMMessage::Role::User;
    m.content = "ping";
    msgs.push_back(m);
    const LLMCompletionResult r = chat(msgs, opts);
    if(!r.ok)
    {
        error_out = r.error_message;
        return false;
    }
    return true;
}

void UOllamaNativeProvider::cancel()
{
    m_cancelled = true;
    m_http.cancel();
}

} // namespace RDK::LLM
