#include "ULLMTaskPlanParsing.h"

#include <algorithm>
#include <cctype>
#include <regex>

namespace RDK::LLM {

namespace {

bool looksLikeComponentClassToken(const std::string& token)
{
    if(token.size() < 3)
        return false;
    const unsigned char first = static_cast<unsigned char>(token[0]);
    if(!std::isupper(first))
        return false;
    for(char c : token)
    {
        if(std::isalnum(static_cast<unsigned char>(c)) || c == '_')
            continue;
        return false;
    }
    return token[0] == 'N' || token.find("Model") != std::string::npos
           || token.find("Generator") != std::string::npos
           || token.find("Neuron") != std::string::npos;
}

} // namespace

std::vector<ClassAddSpec> extractClassAddSpecsFromGoal(const std::string& goal_en)
{
    std::vector<ClassAddSpec> specs;
    static const std::regex kPair(R"((\b)(\d{1,3})\s+([A-Z][A-Za-z0-9_]{2,})\b)");
    for(std::sregex_iterator it(goal_en.begin(), goal_en.end(), kPair), end; it != end; ++it)
    {
        const int count = std::stoi((*it)[2].str());
        const std::string class_name = (*it)[3].str();
        if(count < 1 || !looksLikeComponentClassToken(class_name))
            continue;
        specs.push_back({class_name, count});
    }

    if(specs.size() <= 1)
        return specs;

    std::vector<ClassAddSpec> merged;
    merged.reserve(specs.size());
    for(const ClassAddSpec& spec : specs)
    {
        auto existing = std::find_if(merged.begin(), merged.end(),
                                     [&](const ClassAddSpec& s) { return s.class_name == spec.class_name; });
        if(existing != merged.end())
            existing->count = std::max(existing->count, spec.count);
        else
            merged.push_back(spec);
    }
    return merged;
}

} // namespace RDK::LLM
