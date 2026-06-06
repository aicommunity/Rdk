#include "ULLMQuantityResolver.h"

#include "../Domain/ULLMNameResolution.h"
#include "../LlmModuleInit.h"
#include "../LlmPublicApi.h"
#include "ULLMQuantityParser.h"

#include <QCryptographicHash>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>

namespace RDK::LLM {

namespace {

std::string trimCopy(const std::string& s)
{
    size_t b = 0;
    while(b < s.size() && std::isspace(static_cast<unsigned char>(s[b])))
        ++b;
    size_t e = s.size();
    while(e > b && std::isspace(static_cast<unsigned char>(s[e - 1])))
        --e;
    return s.substr(b, e - b);
}

bool envEnabled(const char* name)
{
    const char* v = std::getenv(name);
    if(!v || v[0] == '\0' || std::strcmp(v, "0") == 0)
        return false;
    std::string lower(v);
    for(char& c : lower)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return lower != "false";
}

int envInt(const char* name, int default_value)
{
    const char* v = std::getenv(name);
    if(!v || !*v)
        return default_value;
    try
    {
        return std::stoi(v);
    }
    catch(...)
    {
        return default_value;
    }
}

int clampPrimary(int n, int max_primary)
{
    return std::max(1, std::min(n, std::max(1, max_primary)));
}

bool llmFallbackEnabled()
{
    if(envEnabled("NMSDK_LLM_QUANTITY_LLM"))
        return true;
    if(LLMServices::instance().isInitialized())
        return LLMServices::instance().settings().runtime().quantity_llm_fallback;
    return false;
}

ResolvedUserQuantity fromParsed(const ParsedQuantity& parsed, const std::string& turn_hash)
{
    ResolvedUserQuantity out;
    out.bound_turn_hash = turn_hash;
    out.primary = std::max(1, parsed.count);
    out.valid = parsed.valid;
    out.source = parsed.valid ? QuantitySource::Heuristic : QuantitySource::None;
    return out;
}

ResolvedUserQuantity mergeHeuristic(const std::string& original, const std::string& en,
                                    const std::string& turn_hash)
{
    const ParsedQuantity from_original = extractQuantityHeuristic(original);
    const ParsedQuantity from_en = extractQuantityHeuristic(en);

    ParsedQuantity best;
    if(from_original.valid && from_en.valid)
        best = from_original.count >= from_en.count ? from_original : from_en;
    else if(from_original.valid)
        best = from_original;
    else if(from_en.valid)
        best = from_en;
    else
        best = ParsedQuantity{};

    return fromParsed(best, turn_hash);
}

std::optional<int> parsePrimaryCountFromLlmJson(const std::string& text)
{
    const auto start = text.find('{');
    const auto end = text.rfind('}');
    if(start == std::string::npos || end == std::string::npos || end <= start)
        return std::nullopt;
    try
    {
        const nlohmann::json j = nlohmann::json::parse(text.substr(start, end - start + 1));
        if(j.contains("primary_count") && !j["primary_count"].is_null())
        {
            if(j["primary_count"].is_number_integer())
                return j["primary_count"].get<int>();
            if(j["primary_count"].is_string())
                return std::stoi(j["primary_count"].get<std::string>());
        }
    }
    catch(...)
    {
    }
    return std::nullopt;
}

std::optional<int> extractQuantityLlm(ILLMProvider& provider, const std::string& combined_text)
{
    std::vector<LLMMessage> messages;
    LLMMessage system;
    system.role = LLMMessage::Role::System;
    system.content =
        "Extract the primary operation count from the user request. "
        "Reply with ONLY JSON: {\"primary_count\": <positive integer or null>}. "
        "Preserve class names and paths. If no explicit count, use null.";
    messages.push_back(std::move(system));
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = combined_text;
    messages.push_back(std::move(user));

    LLMCompletionOptions opts;
    opts.max_tokens = 48;
    opts.temperature = 0.f;
    opts.tools_for_api.clear();

    const LLMCompletionResult completion = provider.chat(messages, opts);
    if(!completion.ok)
        return std::nullopt;
    return parsePrimaryCountFromLlmJson(completion.text);
}

} // namespace

std::string quantityTurnBindingHash(const std::string& text_original, const std::string& text_en)
{
    const QByteArray data = QByteArray::fromStdString(text_original + "|" + text_en);
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex().toStdString();
}

QuantityResolveResult resolveUserQuantity(const QuantityResolveRequest& req, ILLMProvider* provider)
{
    QuantityResolveResult out;
    const int max_primary = envInt("NMSDK_LLM_QUANTITY_MAX", req.max_primary);
    const std::string turn_hash = quantityTurnBindingHash(req.text_original, req.text_en);

    out.quantity = mergeHeuristic(req.text_original, req.text_en, turn_hash);
    out.quantity.primary = clampPrimary(out.quantity.primary, max_primary);

    const std::string combined = req.text_original + "\n" + req.text_en;
    if(out.quantity.valid || !req.allow_llm_fallback || !provider || !llmFallbackEnabled())
        return out;

    if(!looksLikeQuantityCue(combined))
        return out;

    if(const std::optional<int> llm_count = extractQuantityLlm(*provider, combined))
    {
        if(*llm_count >= 1)
        {
            out.quantity.primary = clampPrimary(*llm_count, max_primary);
            out.quantity.valid = true;
            out.quantity.source = QuantitySource::Llm;
            GetAuditLog().append(
                "quantity_llm_fallback",
                {{"primary", out.quantity.primary}, {"turn_hash", turn_hash.substr(0, 16)}},
                "", "");
        }
    }

    return out;
}

bool isDisambiguationOnlyFollowUp(const std::string& user_text, const ConversationState& state)
{
    if(!state.pending_tool_arguments)
        return false;

    const std::string trimmed = trimCopy(user_text);
    if(trimmed.empty() || trimmed.find_first_of(" \t\n\r") != std::string::npos)
        return false;

    const PendingToolArguments& pending = *state.pending_tool_arguments;
    const nlohmann::json candidates =
        pending.disambiguation_candidates.is_array() && !pending.disambiguation_candidates.empty()
            ? pending.disambiguation_candidates
            : pending.class_disambiguation_candidates;

    if(candidates.is_array() && !candidates.empty())
    {
        bool all_digits = true;
        for(char c : trimmed)
        {
            if(!std::isdigit(static_cast<unsigned char>(c)))
            {
                all_digits = false;
                break;
            }
        }
        if(all_digits)
            return true;
    }

    if(looksLikeClassIdentifier(trimmed))
        return true;

    return false;
}

int primaryQuantityOr(const ConversationState& state, int default_value)
{
    if(state.last_quantity.valid)
        return std::max(1, state.last_quantity.primary);
    return default_value;
}

std::string quantitySourceName(QuantitySource source)
{
    switch(source)
    {
    case QuantitySource::Heuristic:
        return "heuristic";
    case QuantitySource::Llm:
        return "llm";
    case QuantitySource::None:
    default:
        return "none";
    }
}

} // namespace RDK::LLM
