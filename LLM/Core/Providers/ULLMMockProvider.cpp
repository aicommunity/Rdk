#include "ULLMMockProvider.h"

namespace RDK::LLM {

LLMProviderCapabilities ULLMMockProvider::capabilities() const
{
    LLMProviderCapabilities c;
    c.requires_network = false;
    c.supports_tool_calling = true;
    c.supports_streaming = false;
    return c;
}

LLMCompletionResult ULLMMockProvider::chat(const std::vector<LLMMessage>& messages,
                                           const LLMCompletionOptions& opts)
{
    (void)opts;
    ++m_invoke_count;
    m_last_provider_system_text.clear();
    for(const LLMMessage& msg : messages)
    {
        if(msg.role == LLMMessage::Role::System)
            m_last_provider_system_text += msg.content + '\n';
    }
    if(!m_queue.empty())
    {
        LLMCompletionResult r = m_queue.front();
        m_queue.pop_front();
        return r;
    }
    LLMCompletionResult r;
    r.ok = true;
    r.text = "Mock provider response.";
    return r;
}

void ULLMMockProvider::chatStream(const std::vector<LLMMessage>& messages,
                                  const LLMCompletionOptions& opts, LLMStreamCallback on_chunk,
                                  std::function<void(LLMCompletionResult)> on_done)
{
    LLMCompletionResult r = chat(messages, opts);
    if(on_chunk && !r.text.empty())
        on_chunk(r.text);
    if(on_done)
        on_done(r);
}

bool ULLMMockProvider::healthCheck(std::string& error_out)
{
    error_out.clear();
    return true;
}

} // namespace RDK::LLM
