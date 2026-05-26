#include "ULLMLibraryScopeHint.h"

#include <algorithm>
#include <cctype>
#include <vector>

namespace RDK::LLM {

namespace {

std::string toLower(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool containsToken(const std::string& hay_lower, const char* needle)
{
    return hay_lower.find(needle) != std::string::npos;
}

int levenshteinDistance(const std::string& a, const std::string& b)
{
    const size_t n = a.size();
    const size_t m = b.size();
    if(n == 0)
        return static_cast<int>(m);
    if(m == 0)
        return static_cast<int>(n);
    std::vector<int> prev(m + 1);
    std::vector<int> cur(m + 1);
    for(size_t j = 0; j <= m; ++j)
        prev[j] = static_cast<int>(j);
    for(size_t i = 1; i <= n; ++i)
    {
        cur[0] = static_cast<int>(i);
        for(size_t j = 1; j <= m; ++j)
        {
            const int cost = a[i - 1] == b[j - 1] ? 0 : 1;
            cur[j] = std::min({cur[j - 1] + 1, prev[j] + 1, prev[j - 1] + cost});
        }
        prev.swap(cur);
    }
    return prev[m];
}

const std::unordered_set<std::string>& pulseClasses()
{
    static const std::unordered_set<std::string> k = {
        "NPulseNeuron", "NPulseSynapseStdp", "NNeuronTrainer", "NNeuronLearner", "NPulseMembrane",
    };
    return k;
}

const std::unordered_set<std::string>& motionClasses()
{
    static const std::unordered_set<std::string> k = {
        "NManipulator", "NEngineMotionControl", "NNewMotionElement", "NEyeRetina", "NAstaticGyro",
    };
    return k;
}

std::string resolveInAllowlist(const std::string& query, const std::unordered_set<std::string>& allowed)
{
    const std::string trimmed = query;
    if(trimmed.empty())
        return trimmed;
    if(allowed.count(trimmed) > 0)
        return trimmed;

    std::string best = trimmed;
    int best_dist = 999;
    const std::string qlower = toLower(trimmed);
    for(const std::string& candidate : allowed)
    {
        const int d = levenshteinDistance(qlower, toLower(candidate));
        if(d < best_dist)
        {
            best_dist = d;
            best = candidate;
        }
    }
    if(best_dist <= 3)
        return best;
    return trimmed;
}

std::string resolveKnownClassAlias(const std::string& query)
{
    const std::string lower = toLower(query);
    if(lower == "nplneuron")
        return "NPulseNeuron";
    if(lower == "нейрон" || lower == "neuron")
        return "NPulseNeuron";
    if(lower == "синапс" || lower == "synapse")
        return "NPulseSynapseStdp";
    return query;
}

std::string resolveAcrossLibraryAllowlists(const std::string& query)
{
    const std::string aliased = resolveKnownClassAlias(query);
    if(aliased != query)
        return aliased;

    std::string best = query;
    int best_dist = 999;
    const std::string qlower = toLower(query);
    for(const std::unordered_set<std::string>* allow :
        {&pulseClasses(), &motionClasses()})
    {
        for(const std::string& candidate : *allow)
        {
            const int d = levenshteinDistance(qlower, toLower(candidate));
            if(d < best_dist)
            {
                best_dist = d;
                best = candidate;
            }
        }
    }
    if(best_dist <= 3)
        return best;
    return query;
}

} // namespace

LibraryScopeHint detectLibraryScopeFromUserText(const std::string& user_text)
{
    const std::string lower = toLower(user_text);
    if(containsToken(lower, "pulse") || containsToken(lower, "импульс") || containsToken(lower, "нейрон")
       || containsToken(lower, "stdp") || containsToken(lower, "snn") || containsToken(lower, "pulselib"))
        return LibraryScopeHint::Pulse;
    if(containsToken(lower, "motion") || containsToken(lower, "manipulator")
       || containsToken(lower, "манипулятор") || containsToken(lower, "retina")
       || containsToken(lower, "motioncontrol") || containsToken(lower, "гироскоп")
       || containsToken(lower, "gyro"))
        return LibraryScopeHint::Motion;
    if(containsToken(lower, "arduino") || containsToken(lower, "firmata") || containsToken(lower, "hardware")
       || containsToken(lower, "железо"))
        return LibraryScopeHint::Hardware;
    return LibraryScopeHint::None;
}

std::string libraryScopeHintManifestSection(LibraryScopeHint hint)
{
    switch(hint)
    {
    case LibraryScopeHint::Pulse:
        return "## Library focus (from user message)\n"
               "- Prefer list_pulse_component_classes and search_pulse_docs for class names.\n"
               "- Mutations: add_component / set_property only (class from pulse list or "
               "list_registered_classes).\n\n";
    case LibraryScopeHint::Motion:
        return "## Library focus (from user message)\n"
               "- Prefer list_motion_control_component_classes and search_motion_control_docs.\n"
               "- Mutations: add_component / set_property only.\n\n";
    case LibraryScopeHint::Hardware:
        return "## Library focus (from user message)\n"
               "- Prefer list_hardware_component_classes and search_hardware_docs.\n"
               "- Mutations: add_component / set_property only.\n\n";
    case LibraryScopeHint::None:
    default:
        return "## Components\n"
               "- Use list_registered_classes for any component class; add_component / set_property "
               "for mutations.\n"
               "- Optional library read tools (search_*_docs, list_*_classes) when user names a "
               "library.\n\n";
    }
}

const std::unordered_set<std::string>* componentClassAllowlistForScope(LibraryScopeHint hint)
{
    switch(hint)
    {
    case LibraryScopeHint::Pulse:
        return &pulseClasses();
    case LibraryScopeHint::Motion:
        return &motionClasses();
    case LibraryScopeHint::Hardware:
    case LibraryScopeHint::None:
    default:
        return nullptr;
    }
}

std::string resolveComponentClassName(const std::string& query, LibraryScopeHint scope)
{
    const std::string aliased = resolveKnownClassAlias(query);
    if(aliased != query)
        return aliased;
    if(const std::unordered_set<std::string>* allow = componentClassAllowlistForScope(scope))
        return resolveInAllowlist(query, *allow);
    return resolveAcrossLibraryAllowlists(query);
}

namespace {

bool isRegisteredClass(const std::vector<std::string>& registered, const std::string& name)
{
    return std::find(registered.begin(), registered.end(), name) != registered.end();
}

} // namespace

std::optional<std::string>
inferAddComponentClassFromUserText(const std::string& user_text,
                                   const std::vector<std::string>& registered)
{
    if(user_text.empty())
        return std::nullopt;

    const std::string lower = toLower(user_text);
    auto pick = [&](const char* class_name) -> std::optional<std::string> {
        if(registered.empty() || isRegisteredClass(registered, class_name))
            return std::string(class_name);
        return std::nullopt;
    };

    if((containsToken(lower, "нейрон") || containsToken(lower, "neuron"))
       && !containsToken(lower, "trainer") && !containsToken(lower, "learner"))
    {
        if(auto c = pick("NPulseNeuron"))
            return c;
    }
    if(containsToken(lower, "синапс") || containsToken(lower, "synapse") || containsToken(lower, "stdp"))
    {
        if(auto c = pick("NPulseSynapseStdp"))
            return c;
    }
    if(containsToken(lower, "membrane") || containsToken(lower, "мембран"))
    {
        if(auto c = pick("NPulseMembrane"))
            return c;
    }
    if(containsToken(lower, "manipulator") || containsToken(lower, "манипулятор"))
    {
        if(auto c = pick("NManipulator"))
            return c;
    }
    return std::nullopt;
}

} // namespace RDK::LLM
