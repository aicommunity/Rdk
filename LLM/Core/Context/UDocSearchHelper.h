#ifndef RDK_UDOC_SEARCH_HELPER_H
#define RDK_UDOC_SEARCH_HELPER_H

#include <filesystem>
#include <string>
#include <vector>

#include "ILLMProjectContextProvider.h"

namespace RDK::LLM {

class UDocSearchHelper {
public:
    static std::vector<DocSnippet> searchRoots(const std::vector<std::filesystem::path>& roots,
                                               const std::string& query, int top_k);
};

} // namespace RDK::LLM

#endif
