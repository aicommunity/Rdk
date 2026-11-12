#include "ULLMQueryNormalizer.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {

namespace {

bool hasCyrillicUtf8(const std::string& text)
{
    // Cyrillic UTF-8 bytes mostly start with 0xD0/0xD1.
    for(unsigned char c : text)
    {
        if(c == 0xD0 || c == 0xD1)
            return true;
    }
    return false;
}

std::string trim(const std::string& s)
{
    const auto is_ws = [](unsigned char c) { return std::isspace(c) != 0; };
    auto begin = std::find_if_not(s.begin(), s.end(), is_ws);
    auto end = std::find_if_not(s.rbegin(), s.rend(), is_ws).base();
    if(begin >= end)
        return {};
    return std::string(begin, end);
}

} // namespace

QueryNormalizeResult normalizeUserQueryForPlanning(ILLMProvider& provider,
                                                   const std::string& user_text,
                                                   bool translate_queries_to_en)
{
    QueryNormalizeResult out;
    out.text_en = user_text;
    out.detected_lang = hasCyrillicUtf8(user_text) ? "ru" : "en";

    if(out.detected_lang == "en" || !translate_queries_to_en)
        return out;

    LLMCompletionOptions opts;
    opts.max_tokens = 256;
    opts.temperature = 0.0f;
    opts.tools_for_api.clear();
    opts.response_language = "en";

    std::vector<LLMMessage> messages;
    LLMMessage sys;
    sys.role = LLMMessage::Role::System;
    sys.content =
        "You translate user requests for an engineering assistant. Output ONLY the English "
        "translation. Preserve class names (NSPNeuron), component short names (PNeuron), "
        "numbers, and file paths. No explanation.";
    messages.push_back(std::move(sys));
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = user_text;
    messages.push_back(std::move(user));

    const LLMCompletionResult tr = provider.chat(messages, opts);
    if(!tr.ok)
    {
        out.ok = false;
        out.text_en = user_text;
        return out;
    }

    const std::string translated = trim(tr.text);
    if(translated.empty())
    {
        out.ok = false;
        out.text_en = user_text;
        return out;
    }

    out.text_en = translated;
    out.used_llm_translate = true;
    return out;
}

} // namespace RDK::LLM
