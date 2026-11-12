#ifndef RDK_ULLM_INDEX_CATALOGS_H
#define RDK_ULLM_INDEX_CATALOGS_H

#include <filesystem>

#include "ULLMConnectSemanticsCatalog.h"
#include "ULinkPatternCatalog.h"

namespace RDK::LLM {

std::filesystem::path defaultLlmIndexRoot();

const ULinkPatternCatalog& defaultLinkPatternCatalog();

} // namespace RDK::LLM

#endif
