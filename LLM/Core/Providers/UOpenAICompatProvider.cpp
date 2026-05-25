#include "UOpenAICompatProvider.h"

#include "UOllamaChatTemplate.h"

#include <chrono>
#include <thread>

namespace RDK::LLM {

namespace {

bool shouldRetryHttpStatus(int status)
{
    return status == 408 || status == 429 || status >= 500;
}

} // namespace

UOpenAICompatProvider::UOpenAICompatProvider(LLMProviderProfile profile)
    : m_profile(std::move(profile))
{
}

LLMProviderCapabilities UOpenAICompatProvider::capabilities() const
{
    LLMProviderCapabilities c;
    c.supports_tool_calling = true;
    c.supports_streaming = true;
    c.requires_network = true;
    return c;
}

nlohmann::json UOpenAICompatProvider::buildRequestBody(const std::vector<LLMMessage>& messages,
                                                       const LLMCompletionOptions& opts) const
{
    std::vector<LLMMessage> prepared = messages;
    if(isOllamaProvider(m_profile))
        prepared = prepareMessagesForOllama(m_profile, std::move(prepared));

    nlohmann::json body;
    body["model"] = m_profile.model;
    body["temperature"] = opts.temperature;
    body["max_tokens"] = opts.max_tokens;
    body["messages"] = buildOpenAiChatMessagesJson(prepared);
    if(!opts.tools_for_api.empty())
    {
        body["tools"] = opts.tools_for_api;
        body["tool_choice"] = "auto";
    }
    return body;
}

LLMCompletionResult UOpenAICompatProvider::parseResponse(const std::string& body,
                                                         std::string& error) const
{
    LLMCompletionResult result;
    try
    {
        nlohmann::json j = nlohmann::json::parse(body);
        if(j.contains("error"))
        {
            result.ok = false;
            result.error_message = j["error"].dump();
            return result;
        }
        const auto& choice = j["choices"][0];
        const auto& message = choice["message"];
        if(message.contains("content") && !message["content"].is_null())
            result.text = message["content"].get<std::string>();
        if(message.contains("tool_calls"))
        {
            for(const auto& tc : message["tool_calls"])
            {
                LLMToolCall call;
                call.id = tc.value("id", "");
                call.name = tc["function"].value("name", "");
                const std::string args_str = tc["function"].value("arguments", "{}");
                call.arguments = nlohmann::json::parse(args_str);
                result.tool_calls.push_back(call);
            }
        }
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

LLMCompletionResult UOpenAICompatProvider::chat(const std::vector<LLMMessage>& messages,
                                                const LLMCompletionOptions& opts)
{
    LLMCompletionResult result;
    std::string url = m_profile.base_url;
    if(url.back() == '/')
        url.pop_back();
    url += "/chat/completions";

    const nlohmann::json body = buildRequestBody(messages, opts);
    for(int attempt = 0; attempt < 2; ++attempt)
    {
        auto resp = m_http.postJson(url, body.dump(), m_profile.api_key);
        if(!resp.error.empty())
        {
            result.ok = false;
            result.error_message = resp.error;
            if(attempt == 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(400));
                continue;
            }
            return result;
        }
        if(resp.status_code < 200 || resp.status_code >= 300)
        {
            if(attempt == 0 && shouldRetryHttpStatus(resp.status_code))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                continue;
            }
            result.ok = false;
            result.error_message = "HTTP " + std::to_string(resp.status_code) + ": " + resp.body;
            return result;
        }
        std::string parse_err;
        return parseResponse(resp.body, parse_err);
    }
    result.ok = false;
    result.error_message = "Provider request failed after retry";
    return result;
}

void UOpenAICompatProvider::chatStream(const std::vector<LLMMessage>& messages,
                                       const LLMCompletionOptions& opts, LLMStreamCallback on_chunk,
                                       std::function<void(LLMCompletionResult)> on_done)
{
    LLMCompletionResult r = chat(messages, opts);
    if(!r.text.empty() && on_chunk)
        on_chunk(r.text);
    if(on_done)
        on_done(r);
}

bool UOpenAICompatProvider::healthCheck(std::string& error_out)
{
    LLMCompletionOptions opts;
    opts.max_tokens = 1;
    std::vector<LLMMessage> msgs;
    LLMMessage m;
    m.role = LLMMessage::Role::User;
    m.content = "ping";
    msgs.push_back(m);
    LLMCompletionResult r = chat(msgs, opts);
    if(!r.ok)
    {
        error_out = r.error_message;
        return false;
    }
    return true;
}

void UOpenAICompatProvider::cancel()
{
    m_cancelled = true;
    m_http.cancel();
}

} // namespace RDK::LLM
