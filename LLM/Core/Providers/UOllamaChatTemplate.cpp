#include "UOllamaChatTemplate.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {

namespace {

std::string lower(const std::string& s)
{
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return out;
}

bool hasSystemMessage(const std::vector<LLMMessage>& messages)
{
    return std::any_of(messages.begin(), messages.end(),
                       [](const LLMMessage& m) { return m.role == LLMMessage::Role::System; });
}

void appendTurn(std::string& out, const char* header, const std::string& body,
                const char* footer)
{
    out += header;
    out += body;
    out += footer;
}

} // namespace

bool isOllamaProvider(const LLMProviderProfile& profile)
{
    if(profile.kind == LLMProviderKind::OllamaOpenAICompat
       || profile.kind == LLMProviderKind::OllamaNative)
        return true;
    const std::string url = lower(profile.base_url);
    return url.find("127.0.0.1:11434") != std::string::npos
           || url.find("localhost:11434") != std::string::npos;
}

OllamaChatTemplateFamily detectChatTemplateFamily(const std::string& model_name,
                                                OllamaChatTemplateFamily hint)
{
    if(hint != OllamaChatTemplateFamily::Auto)
        return hint;

    const std::string m = lower(model_name);
    if(m.find("qwen") != std::string::npos || m.find("qwq") != std::string::npos)
        return OllamaChatTemplateFamily::Qwen2;
    if(m.find("llama3") != std::string::npos || m.find("llama-3") != std::string::npos)
        return OllamaChatTemplateFamily::Llama3;
    if(m.find("mistral") != std::string::npos || m.find("mixtral") != std::string::npos)
        return OllamaChatTemplateFamily::Mistral;
    if(m.find("gemma") != std::string::npos)
        return OllamaChatTemplateFamily::Gemma2;
    if(m.find("phi") != std::string::npos)
        return OllamaChatTemplateFamily::ChatML;
    return OllamaChatTemplateFamily::ChatML;
}

const char* defaultRdkSystemPrompt()
{
    return "You are the NeuroModeler AI assistant (RDK). Use tools to inspect the project; "
           "answer in the user's language. For diagram changes, call tools — do not invent "
           "component names or property values.";
}

std::vector<LLMMessage> ensureRdkSystemPrompt(std::vector<LLMMessage> messages)
{
    if(hasSystemMessage(messages))
        return messages;
    LLMMessage system;
    system.role = LLMMessage::Role::System;
    system.content = defaultRdkSystemPrompt();
    messages.insert(messages.begin(), system);
    return messages;
}

std::vector<LLMMessage> prepareMessagesForOllama(const LLMProviderProfile& profile,
                                                std::vector<LLMMessage> messages)
{
    messages = ensureRdkSystemPrompt(std::move(messages));
    const OllamaChatTemplateFamily family =
        detectChatTemplateFamily(profile.model, profile.chat_template);

    std::vector<LLMMessage> out;
    out.reserve(messages.size());

    for(LLMMessage& m : messages)
    {
        if(m.role == LLMMessage::Role::Tool)
        {
            if(!m.tool_call_id && m.tool_name)
                m.tool_call_id = "call_" + *m.tool_name;
            if(m.content.empty())
                m.content = "{}";
        }

        if(m.role == LLMMessage::Role::Assistant && m.assistant_tool_calls
           && !m.assistant_tool_calls->empty() && m.content.empty())
        {
            m.content = "";
        }

        out.push_back(std::move(m));
    }

    (void)family;
    return out;
}

std::string formatPromptWithTemplate(OllamaChatTemplateFamily family,
                                     const std::vector<LLMMessage>& messages)
{
    std::string prompt;
    for(const LLMMessage& m : messages)
    {
        const std::string& text = m.content;
        switch(m.role)
        {
        case LLMMessage::Role::System:
            switch(family)
            {
            case OllamaChatTemplateFamily::Llama3:
                appendTurn(prompt,
                           "<|begin_of_text|><|start_header_id|>system<|end_header_id|>\n\n", text,
                           "<|eot_id|>");
                break;
            case OllamaChatTemplateFamily::Gemma2:
                appendTurn(prompt, "<start_of_turn>system\n", text, "<end_of_turn>\n");
                break;
            case OllamaChatTemplateFamily::Mistral:
                appendTurn(prompt, "[INST] ", text, " [/INST]");
                break;
            case OllamaChatTemplateFamily::Qwen2:
            case OllamaChatTemplateFamily::ChatML:
            default:
                appendTurn(prompt, "<|im_start|>system\n", text, "\n");
                break;
            }
            break;
        case LLMMessage::Role::User:
            switch(family)
            {
            case OllamaChatTemplateFamily::Llama3:
                appendTurn(prompt, "<|start_header_id|>user<|end_header_id|>\n\n", text, "<|eot_id|>");
                break;
            case OllamaChatTemplateFamily::Gemma2:
                appendTurn(prompt, "<start_of_turn>user\n", text, "<end_of_turn>\n");
                break;
            case OllamaChatTemplateFamily::Mistral:
                appendTurn(prompt, "[INST] ", text, " [/INST]");
                break;
            case OllamaChatTemplateFamily::Qwen2:
            case OllamaChatTemplateFamily::ChatML:
            default:
                appendTurn(prompt, "<|im_start|>user\n", text, "\n");
                break;
            }
            break;
        case LLMMessage::Role::Assistant:
            switch(family)
            {
            case OllamaChatTemplateFamily::Llama3:
                appendTurn(prompt, "<|start_header_id|>assistant<|end_header_id|>\n\n", text, "<|eot_id|>");
                break;
            case OllamaChatTemplateFamily::Gemma2:
                appendTurn(prompt, "<start_of_turn>model\n", text, "<end_of_turn>\n");
                break;
            case OllamaChatTemplateFamily::Mistral:
                appendTurn(prompt, "", text, " </s>");
                break;
            case OllamaChatTemplateFamily::Qwen2:
            case OllamaChatTemplateFamily::ChatML:
            default:
                appendTurn(prompt, "<|im_start|>assistant\n", text, "\n");
                break;
            }
            break;
        case LLMMessage::Role::Tool:
        {
            std::string tool_line = "Tool result";
            if(m.tool_name)
                tool_line += " (" + *m.tool_name + ")";
            tool_line += ":\n" + text;
            switch(family)
            {
            case OllamaChatTemplateFamily::Llama3:
            case OllamaChatTemplateFamily::Gemma2:
            case OllamaChatTemplateFamily::Mistral:
            case OllamaChatTemplateFamily::Qwen2:
            case OllamaChatTemplateFamily::ChatML:
            default:
                appendTurn(prompt, "<|im_start|>user\n", tool_line, "\n");
                break;
            }
            break;
        }
        }
    }

    switch(family)
    {
    case OllamaChatTemplateFamily::Llama3:
        prompt += "<|start_header_id|>assistant<|end_header_id|>\n\n";
        break;
    case OllamaChatTemplateFamily::Gemma2:
        prompt += "<start_of_turn>model\n";
        break;
    case OllamaChatTemplateFamily::Mistral:
        break;
    case OllamaChatTemplateFamily::Qwen2:
    case OllamaChatTemplateFamily::ChatML:
    default:
        prompt += "<|im_start|>assistant\n";
        break;
    }
    return prompt;
}

nlohmann::json buildOpenAiChatMessagesJson(const std::vector<LLMMessage>& messages)
{
    nlohmann::json msgs = nlohmann::json::array();
    for(const LLMMessage& m : messages)
    {
        nlohmann::json item;
        switch(m.role)
        {
        case LLMMessage::Role::System:
            item["role"] = "system";
            break;
        case LLMMessage::Role::User:
            item["role"] = "user";
            break;
        case LLMMessage::Role::Assistant:
            item["role"] = "assistant";
            break;
        case LLMMessage::Role::Tool:
            item["role"] = "tool";
            break;
        }

        if(m.role == LLMMessage::Role::Assistant && m.assistant_tool_calls
           && !m.assistant_tool_calls->empty())
        {
            if(m.content.empty())
                item["content"] = nullptr;
            else
                item["content"] = m.content;

            nlohmann::json tool_calls = nlohmann::json::array();
            for(const LLMToolCall& call : *m.assistant_tool_calls)
            {
                tool_calls.push_back({{"id", call.id},
                                      {"type", "function"},
                                      {"function",
                                       {{"name", call.name},
                                        {"arguments", call.arguments.dump()}}}});
            }
            item["tool_calls"] = tool_calls;
        }
        else
        {
            item["content"] = m.content;
        }

        if(m.tool_call_id)
            item["tool_call_id"] = *m.tool_call_id;
        if(m.tool_name && m.role == LLMMessage::Role::Tool)
            item["name"] = *m.tool_name;

        msgs.push_back(item);
    }
    return msgs;
}

} // namespace RDK::LLM
