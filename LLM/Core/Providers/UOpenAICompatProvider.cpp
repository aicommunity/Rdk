#include "UOpenAICompatProvider.h"

#include "../Http/ULLMHttpRetry.h"
#include "ULLMThinkingParse.h"
#include "UOllamaChatTemplate.h"
#include "UOllamaModelInfo.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <map>
#include <thread>

namespace RDK::LLM {

namespace {

bool shouldRetryHttpStatus(int status)
{
    return status == 408 || status == 429 || status >= 500;
}

struct StreamToolPart {
    std::string id;
    std::string name;
    std::string arguments;
};

void applyStreamDelta(const nlohmann::json& delta, std::string& text_out,
                      std::string& thinking_out, std::map<int, StreamToolPart>& tools_out,
                      LLMStreamCallback& on_chunk,
                      const std::function<void(const std::string&)>& on_thinking)
{
    const std::size_t thinking_before = thinking_out.size();
    extractThinkingFields(delta, thinking_out);
    if(on_thinking && thinking_out.size() > thinking_before)
        on_thinking(thinking_out.substr(thinking_before));
    if(delta.contains("content") && delta["content"].is_string())
    {
        const std::string piece = delta["content"].get<std::string>();
        if(!piece.empty())
        {
            text_out += piece;
            if(on_chunk)
                on_chunk(piece);
        }
    }
    if(!delta.contains("tool_calls") || !delta["tool_calls"].is_array())
        return;
    for(const auto& tc : delta["tool_calls"])
    {
        const int index = tc.value("index", 0);
        StreamToolPart& part = tools_out[index];
        if(tc.contains("id"))
            part.id = tc["id"].get<std::string>();
        if(tc.contains("function"))
        {
            const auto& fn = tc["function"];
            if(fn.contains("name") && fn["name"].is_string())
                part.name = fn["name"].get<std::string>();
            if(fn.contains("arguments") && fn["arguments"].is_string())
                part.arguments += fn["arguments"].get<std::string>();
        }
    }
}

LLMCompletionResult buildStreamResult(const std::string& text, const std::string& thinking,
                                      const std::map<int, StreamToolPart>& tools)
{
    LLMCompletionResult result;
    result.ok = true;
    result.text = text;
    result.thinking = thinking;
    for(const auto& kv : tools)
    {
        if(kv.second.name.empty())
            continue;
        LLMToolCall call;
        call.id = kv.second.id;
        call.name = kv.second.name;
        try
        {
            call.arguments = nlohmann::json::parse(kv.second.arguments.empty() ? "{}" : kv.second.arguments);
        }
        catch(...)
        {
            call.arguments = nlohmann::json::object();
        }
        result.tool_calls.push_back(std::move(call));
    }
    finalizeThinkingResult(result);
    return result;
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
    c.supports_strict_json_schema = !isOllamaProvider(m_profile);
    c.requires_network = true;
    c.supports_thinking = isOllamaProvider(m_profile);
    return c;
}

nlohmann::json UOpenAICompatProvider::buildRequestBody(const std::vector<LLMMessage>& messages,
                                                       const LLMCompletionOptions& opts) const
{
    const std::string lang = opts.response_language.empty() ? "en" : opts.response_language;
    std::vector<LLMMessage> prepared = ensureRdkSystemPrompt(messages, lang);
    if(isOllamaProvider(m_profile))
        prepared = prepareMessagesForOllama(m_profile, std::move(prepared), lang);

    nlohmann::json body;
    body["model"] =
        opts.model_override && !opts.model_override->empty() ? *opts.model_override : m_profile.model;
    body["temperature"] = opts.temperature;
    body["max_tokens"] = opts.max_tokens;
    body["messages"] = buildOpenAiChatMessagesJson(prepared);
    if(shouldSendThinkTrue(opts, capabilities()))
        body["think"] = true;
    if(!opts.tools_for_api.empty())
    {
        body["tools"] = opts.tools_for_api;
        if(opts.tool_choice)
            body["tool_choice"] = *opts.tool_choice;
        else
            body["tool_choice"] = "auto";
    }
    if(opts.response_format)
        body["response_format"] = *opts.response_format;
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
        {
            if(message["content"].is_string())
                result.text = message["content"].get<std::string>();
            else
                result.text = message["content"].dump();
        }
        extractThinkingFields(message, result.thinking);
        if(message.contains("tool_calls"))
        {
            for(const auto& tc : message["tool_calls"])
            {
                LLMToolCall call;
                call.id = tc.value("id", "");
                call.name = tc["function"].value("name", "");
                call.arguments = parseToolCallArgumentsJson(tc["function"]);
                result.tool_calls.push_back(call);
            }
        }
        finalizeThinkingResult(result);
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
    m_cancelled = false;
    LLMCompletionResult result;
    std::string url = m_profile.base_url;
    if(url.empty())
    {
        result.ok = false;
        result.error_message = "Provider base_url is empty";
        return result;
    }
    if(url.back() == '/')
        url.pop_back();
    url += "/chat/completions";

    try
    {
    const nlohmann::json body = buildRequestBody(messages, opts);
    const std::string body_str =
        body.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
    constexpr int kMaxAttempts = 3;
    for(int attempt = 0; attempt < kMaxAttempts; ++attempt)
    {
        auto resp = m_http.postJson(url, body_str, m_profile.api_key);
        if(!resp.error.empty())
        {
            result.ok = false;
            result.error_message = resp.error;
            if(attempt < kMaxAttempts - 1)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    providerRetryDelayMs(attempt, resp.retry_after)));
                continue;
            }
            return result;
        }
        if(resp.status_code < 200 || resp.status_code >= 300)
        {
            if(attempt < kMaxAttempts - 1 && shouldRetryHttpStatus(resp.status_code))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    providerRetryDelayMs(attempt, resp.retry_after)));
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
    catch(const std::exception& ex)
    {
        result.ok = false;
        result.error_message = std::string("chat exception: ") + ex.what();
        return result;
    }
    catch(...)
    {
        result.ok = false;
        result.error_message = "chat exception: unknown";
        return result;
    }
}

void UOpenAICompatProvider::chatStream(const std::vector<LLMMessage>& messages,
                                       const LLMCompletionOptions& opts, LLMStreamCallback on_chunk,
                                       std::function<void(LLMCompletionResult)> on_done)
{
    try
    {
        m_cancelled = false;
        std::string url = m_profile.base_url;
        if(url.empty())
        {
            LLMCompletionResult err;
            err.ok = false;
            err.error_message = "Provider base_url is empty";
            if(on_done)
                on_done(err);
            return;
        }
        if(url.back() == '/')
            url.pop_back();
        url += "/chat/completions";

    nlohmann::json body = buildRequestBody(messages, opts);
    body["stream"] = true;
    const std::string body_str =
        body.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);

    std::string accumulated_text;
    std::string accumulated_thinking;
    std::map<int, StreamToolPart> tool_parts;
    std::string last_retry_after;
    LLMStreamCallback chunk_cb = on_chunk;
    if(!opts.tools_for_api.empty())
        chunk_cb = nullptr;
    const auto thinking_cb = opts.on_thinking_chunk;

    auto run_once = [&]() -> LLMCompletionResult {
        accumulated_text.clear();
        accumulated_thinking.clear();
        tool_parts.clear();
        const auto resp = m_http.postJsonStream(
            url, body_str, m_profile.api_key,
            [&](const std::string& payload) -> bool {
                if(m_cancelled.load())
                    return false;
                try
                {
                    const nlohmann::json j = nlohmann::json::parse(payload);
                    if(!j.contains("choices") || j["choices"].empty())
                        return true;
                    const auto& delta = j["choices"][0].value("delta", nlohmann::json::object());
                    applyStreamDelta(delta, accumulated_text, accumulated_thinking, tool_parts,
                                    chunk_cb, thinking_cb);
                }
                catch(...)
                {
                }
                return true;
            });
        last_retry_after = resp.retry_after;
        if(m_cancelled.load())
        {
            LLMCompletionResult cancelled;
            cancelled.ok = false;
            cancelled.error_message = "Cancelled";
            return cancelled;
        }
        if(!resp.error.empty())
        {
            LLMCompletionResult err;
            err.ok = false;
            err.error_message = resp.error;
            return err;
        }
        if(resp.status_code < 200 || resp.status_code >= 300)
        {
            LLMCompletionResult err;
            err.ok = false;
            err.error_message = "HTTP " + std::to_string(resp.status_code);
            if(!resp.body.empty())
                err.error_message += ": " + resp.body;
            return err;
        }
        return buildStreamResult(accumulated_text, accumulated_thinking, tool_parts);
    };

    LLMCompletionResult result;
    constexpr int kMaxAttempts = 3;
    for(int attempt = 0; attempt < kMaxAttempts; ++attempt)
    {
        result = run_once();
        if(result.ok || m_cancelled.load())
            break;
        if(attempt < kMaxAttempts - 1 && result.error_message.rfind("HTTP ", 0) == 0)
        {
            const int code = std::atoi(result.error_message.c_str() + 5);
            if(shouldRetryHttpStatus(code))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    providerRetryDelayMs(attempt, last_retry_after)));
                continue;
            }
        }
        break;
    }

    if(on_done)
        on_done(result);
    }
    catch(const std::exception& ex)
    {
        LLMCompletionResult err;
        err.ok = false;
        err.error_message = std::string("chatStream exception: ") + ex.what();
        if(on_done)
            on_done(err);
    }
    catch(...)
    {
        LLMCompletionResult err;
        err.ok = false;
        err.error_message = "chatStream exception: unknown";
        if(on_done)
            on_done(err);
    }
}

bool UOpenAICompatProvider::healthCheck(std::string& error_out)
{
    if(m_profile.kind == LLMProviderKind::OllamaOpenAICompat
       || m_profile.kind == LLMProviderKind::OllamaNative)
    {
        const auto models = listOllamaTagModels(m_profile);
        if(!models.empty())
        {
            const bool found = std::find(models.begin(), models.end(), m_profile.model) != models.end();
            if(!found)
            {
                error_out = "Model not found on Ollama server: " + m_profile.model;
                error_out += formatOllamaModelMismatchHint(m_profile);
                return false;
            }
        }
    }

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
    if(m_profile.kind == LLMProviderKind::OllamaOpenAICompat
       || m_profile.kind == LLMProviderKind::OllamaNative)
    {
        const int n_ctx = probeOllamaNumCtx(m_profile);
        if(n_ctx > 0 && n_ctx < 8192)
        {
            error_out = "Connected (n_ctx=" + std::to_string(n_ctx)
                        + "). For multi-tool agent turns, prefer models with n_ctx >= 8192.";
        }
    }
    return true;
}

void UOpenAICompatProvider::cancel()
{
    m_cancelled = true;
    m_http.cancel();
}

} // namespace RDK::LLM
