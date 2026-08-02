#include "ULLMIntentParser.h"

#include "ULLMChannelCalcCommand.h"
#include "ULLMComponentStructureGoal.h"
#include "ULLMConnectPlanParsing.h"
#include "ULLMWatchPlotGoal.h"

#include <cctype>
#include <cstdlib>
#include <algorithm>

namespace RDK::LLM {

namespace {

float scoreKeywords(const std::string& lower, const std::vector<const char*>& words, float weight)
{
    float score = 0.f;
    for(const char* w : words)
    {
        if(lower.find(w) != std::string::npos)
            score += weight;
    }
    return score;
}

} // namespace

IntentParseResult ULLMIntentParser::parseDetailed(const std::string& user_text) const
{
    std::string lower = user_text;
    for(char& c : lower)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    const float plan_s = scoreKeywords(lower, {"план", "спланируй", "шаги", "plan ", "steps", "roadmap", "сначала"}, 1.2f);
    // Do not use bare "проект"/"project" — they fire on «расскажи о проекте» (chat 17-11-11).
    // Lifecycle uses explicit phrases below.
    const float mutate_s =
        scoreKeywords(lower,
                      {"добав", "создай", "удали", "измени", "сохран", "загруз", "открой", "закрой", "конфиг",
                       "конфигурац", "configuration", "project.ini", "скопируй",
                       "создай проект", "новый проект", "открой проект", "сохрани проект", "закрой проект",
                       "create project", "new project", "open project", "save project", "close project",
                       "переимен", "запусти расч", "останови расч", "расчёт", "расчет",
                       "start calc", "run calculation", "pause calculation", "reset calculation",
                       "start calculation", "stop calculation",
                       "дендрит", "numsoma", "numdendrite", "membrane parts",
                       "график", "watch", "plot",
                       "add ", "create ", "create config", "new config", "new configuration", "remove ", "delete ",
                       "save ", "load ", "set ", "connect ", "open config", "close config", "copy config",
                       "rename config", "создай конфиг", "новый конфиг", "новая конфигурация"},
                      1.0f);

    // Russian graph-mutation verbs: "соедини", "подключи", "связать", etc.
    const float connect_kw_s =
        scoreKeywords(lower, {"соедин", "связ", "подключ", "линк", "link "}, 1.0f);
    const float mutate_s_with_connect = std::max(mutate_s, connect_kw_s);
    const float explain_s =
        scoreKeywords(lower, {"почему", "объясни", "explain", "why ", "как работает", "how does"}, 1.0f);
    const float query_s =
        scoreKeywords(lower,
                      {"arduino", "firmware", "hardware", "датчик", "плата", "что", "какие", "покажи",
                       "список", "опиши", "найди", "валидируй", "проверь конфиг", "validate configuration", "what",
                       "list", "show", "describe", "search", "find ", "tool", "tools", "имена инструментов",
                       "расскаж", "tell ", "about the", "о проекте", "о этом проект"},
                      1.2f);

    IntentParseResult result;
    result.method = "rules";
    float best = query_s;
    result.kind = LLMIntentKind::Query;

    if(plan_s > best)
    {
        best = plan_s;
        result.kind = LLMIntentKind::Plan;
    }
    if(mutate_s_with_connect > best)
    {
        best = mutate_s_with_connect;
        result.kind = LLMIntentKind::Mutate;
    }
    if(explain_s > best)
    {
        best = explain_s;
        result.kind = LLMIntentKind::Explain;
    }
    if(query_s > best)
    {
        best = query_s;
        result.kind = LLMIntentKind::Query;
    }

    // Connect/link phrasing must mutate even when stem scoring missed (e.g. past tense).
    if(isConnectGoalText(user_text) || isDisconnectGoalText(user_text))
    {
        result.kind = LLMIntentKind::Mutate;
        best = std::max(best, 1.0f);
    }

    // Channel calc verbs (запусти расчет / start calculation) — always Mutate.
    if(isChannelCalcGoalText(user_text))
    {
        result.kind = LLMIntentKind::Mutate;
        best = std::max(best, 1.0f);
    }

    if(isComponentStructureGoal(user_text) || isWatchPlotGoal(user_text))
    {
        result.kind = LLMIntentKind::Mutate;
        best = std::max(best, 1.0f);
    }

    const float total = plan_s + mutate_s + explain_s + query_s + connect_kw_s + 0.01f;
    result.confidence = std::min(1.f, best / total);
    return result;
}

LLMIntentKind ULLMIntentParser::parse(const std::string& user_text) const
{
    return parseDetailed(user_text).kind;
}

IntentParseResult ULLMIntentParser::parseWithOptionalLlm(ILLMProvider* provider,
                                                        const std::string& user_text) const
{
    IntentParseResult rules = parseDetailed(user_text);
    if(!provider)
        return rules;

    const char* env = std::getenv("NMSDK_LLM_INTENT_LLM");
    if(!env || env[0] != '1')
        return rules;

    std::vector<LLMMessage> msgs;
    LLMMessage system;
    system.role = LLMMessage::Role::System;
    system.content =
        "Classify user intent. Reply with exactly one word: query, mutate, explain, or plan.";
    msgs.push_back(system);
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = user_text;
    msgs.push_back(user);

    LLMCompletionOptions opts;
    opts.max_tokens = 8;
    opts.temperature = 0.f;
    opts.tools_for_api.clear();

    const LLMCompletionResult completion = provider->chat(msgs, opts);
    if(!completion.ok)
        return rules;

    std::string label = completion.text;
    for(char& c : label)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    IntentParseResult llm = rules;
    llm.method = "llm";
    llm.confidence = 0.85f;
    if(label.find("mutate") != std::string::npos)
        llm.kind = LLMIntentKind::Mutate;
    else if(label.find("plan") != std::string::npos)
        llm.kind = LLMIntentKind::Plan;
    else if(label.find("explain") != std::string::npos)
        llm.kind = LLMIntentKind::Explain;
    else
        llm.kind = LLMIntentKind::Query;

    if(rules.confidence >= 0.75f && rules.kind != llm.kind)
        return rules;
    return llm;
}

} // namespace RDK::LLM
