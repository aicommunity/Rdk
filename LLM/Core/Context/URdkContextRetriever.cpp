#include "URdkContextRetriever.h"

namespace RDK::LLM {

URdkContextRetriever::URdkContextRetriever(URdkDomainAccess& domain,
                                           ILLMProjectContextProvider* project)
    : m_domain(domain)
    , m_project(project)
{
}

nlohmann::json URdkContextRetriever::buildSummary(int channel_index,
                                                 const std::string& focus_class,
                                                 const std::string& root_long_name) const
{
    nlohmann::json summary;
    nlohmann::json snap;
    m_domain.listNetSnapshot(snap, channel_index, 100, root_long_name);
    summary["net_snapshot"] = snap;
    if(m_project && !focus_class.empty())
        summary["cl_desc_fragment"] = m_project->clDescFragment(focus_class);
    return summary;
}

} // namespace RDK::LLM
