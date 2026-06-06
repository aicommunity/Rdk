#include "ULLMResolvedEntityStore.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {

namespace {

constexpr std::size_t kMaxResolvedEntities = 64;

void collapseSpaces(std::string& s)
{
    std::string out;
    out.reserve(s.size());
    bool space = false;
    for(unsigned char c : s)
    {
        if(std::isspace(c))
        {
            if(!space && !out.empty())
            {
                out.push_back(' ');
                space = true;
            }
        }
        else
        {
            out.push_back(static_cast<char>(std::tolower(c)));
            space = false;
        }
    }
    while(!out.empty() && out.back() == ' ')
        out.pop_back();
    s = std::move(out);
}

} // namespace

std::string normalizeEntityQuery(const std::string& query)
{
    std::string key = query;
    collapseSpaces(key);
    return key;
}

std::optional<std::string> lookupResolvedEntity(const ConversationState& state,
                                                const std::string& kind,
                                                const std::string& query,
                                                const int channel_index)
{
    const std::string key = normalizeEntityQuery(query);
    if(key.empty())
        return std::nullopt;
    for(const ResolvedEntityRecord& r : state.resolved_entities)
    {
        if(r.kind == kind && r.channel_index == channel_index && r.query_key == key)
            return r.canonical_value;
    }
    return std::nullopt;
}

void upsertResolvedEntity(ConversationState& state, const std::string& kind,
                          const std::string& query, const std::string& canonical_value,
                          const int channel_index)
{
    const std::string key = normalizeEntityQuery(query);
    if(key.empty() || canonical_value.empty())
        return;

    auto& records = state.resolved_entities;
    for(ResolvedEntityRecord& r : records)
    {
        if(r.kind == kind && r.channel_index == channel_index && r.query_key == key)
        {
            r.canonical_value = canonical_value;
            return;
        }
    }

    records.push_back(ResolvedEntityRecord{kind, key, canonical_value, channel_index});
    if(records.size() > kMaxResolvedEntities)
        records.erase(records.begin(), records.begin() + (records.size() - kMaxResolvedEntities));
}

} // namespace RDK::LLM
