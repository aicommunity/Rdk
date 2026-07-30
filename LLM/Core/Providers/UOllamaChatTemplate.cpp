#include "UOllamaChatTemplate.h"

#include "../Settings/ULLMResponseLanguage.h"
#include "UOllamaModelInfo.h"

#include <algorithm>
#include <cctype>
#include <sstream>

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

bool manifestHasResponseLanguageBlock(const std::vector<LLMMessage>& messages)
{
    for(const LLMMessage& m : messages)
    {
        if(m.role != LLMMessage::Role::System)
            continue;
        if(m.content.find("## Response language") != std::string::npos)
            return true;
    }
    return false;
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

std::string buildRdkSystemPrompt(const std::string& response_language)
{
    const std::string code =
        response_language.empty() ? "en" : response_language;
    const std::string display = responseLanguageDisplayName(code);
    std::ostringstream oss;
    oss << "You are the NeuroModeler AI assistant (RDK). Always use native function tool_calls "
           "when tools are available — never paste JSON tool examples in markdown. "
           "New/open project on disk (RU: создай проект/конфигурацию; EN: create project/config): "
           "create_configuration or load_configuration — never add_component. "
           "Diagram edits inside an open configuration: add_component, set_property, connect. "
           "Documentation: search_project_docs (scope docs|sources|all). "
           "Always respond in "
        << display << " (language code: " << code
        << "). Use this language for all user-facing text unless the user explicitly requests "
           "another language. Do not invent class names.";
    return oss.str();
}

std::vector<LLMMessage> ensureRdkSystemPrompt(std::vector<LLMMessage> messages,
                                              const std::string& response_language)
{
    if(hasSystemMessage(messages) && manifestHasResponseLanguageBlock(messages))
        return messages;
    if(hasSystemMessage(messages) && !manifestHasResponseLanguageBlock(messages))
    {
        const std::string code =
            response_language.empty() ? "en" : response_language;
        const std::string display = responseLanguageDisplayName(code);
        LLMMessage lang;
        lang.role = LLMMessage::Role::System;
        lang.content = "## Response language\nAlways respond to the user in " + display
                       + " (code: " + code + "). Tool arguments may stay in English.";
        messages.insert(messages.begin(), std::move(lang));
        return messages;
    }
    LLMMessage system;
    system.role = LLMMessage::Role::System;
    system.content = buildRdkSystemPrompt(response_language);
    messages.insert(messages.begin(), system);
    return messages;
}

OllamaChatTemplateFamily resolveChatTemplateFamily(const LLMProviderProfile& profile)
{
    if(profile.chat_template != OllamaChatTemplateFamily::Auto)
        return profile.chat_template;
    if(isOllamaProvider(profile))
        return fetchOllamaTemplateFamily(profile);
    return detectChatTemplateFamily(profile.model, profile.chat_template);
}

std::vector<LLMMessage> prepareMessagesForOllama(const LLMProviderProfile& profile,
                                                std::vector<LLMMessage> messages,
                                                const std::string& response_language)
{
    messages = ensureRdkSystemPrompt(std::move(messages), response_language);
    const OllamaChatTemplateFamily family = resolveChatTemplateFamily(profile);

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

std::string assistantMessageTextForPrompt(const LLMMessage& message)
{
    std::string text = message.content;
    if(message.assistant_tool_calls && !message.assistant_tool_calls->empty())
    {
        for(const LLMToolCall& call : *message.assistant_tool_calls)
        {
            if(!text.empty())
                text += '\n';
            text += nlohmann::json{{"name", call.name}, {"arguments", call.arguments}}.dump();
        }
    }
    return text;
}

std::string formatPromptWithTemplate(OllamaChatTemplateFamily family,
                                     const std::vector<LLMMessage>& messages)
{
    std::string prompt;
    for(const LLMMessage& m : messages)
    {
        const std::string text =
            m.role == LLMMessage::Role::Assistant ? assistantMessageTextForPrompt(m) : m.content;
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

        // Preserve thinking on assistant turns for multi-step tool loops (DD-THINK-002).
        if(m.role == LLMMessage::Role::Assistant && m.thinking && !m.thinking->empty())
            item["thinking"] = *m.thinking;

        if(m.tool_call_id)
            item["tool_call_id"] = *m.tool_call_id;
        if(m.tool_name && m.role == LLMMessage::Role::Tool)
            item["name"] = *m.tool_name;

        msgs.push_back(item);
    }
    return msgs;
}

} // namespace RDK::LLM
