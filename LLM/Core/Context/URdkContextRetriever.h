#ifndef RDK_URDK_CONTEXT_RETRIEVER_H
#define RDK_URDK_CONTEXT_RETRIEVER_H

#include <nlohmann/json.hpp>

#include "../Domain/URdkDomainAccess.h"
#include "ILLMProjectContextProvider.h"

namespace RDK::LLM {

class URdkContextRetriever {
public:
    URdkContextRetriever(URdkDomainAccess& domain, ILLMProjectContextProvider* project);

    nlohmann::json buildSummary(int channel_index, const std::string& focus_class = "") const;

private:
    URdkDomainAccess& m_domain;
    ILLMProjectContextProvider* m_project;
};

} // namespace RDK::LLM

#endif
