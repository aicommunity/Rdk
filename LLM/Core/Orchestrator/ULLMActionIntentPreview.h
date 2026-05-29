#ifndef RDK_ULLM_ACTION_INTENT_PREVIEW_H
#define RDK_ULLM_ACTION_INTENT_PREVIEW_H

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "../LlmTypes.h"

namespace RDK::LLM {

/// Optional connect hint lines (e.g. top-k from link-pattern catalog) for preview section 4.
struct LinkPreviewContext {
    std::vector<std::string> catalog_lines;
};

std::string formatActionIntentPreview(const std::string& tool_name,
                                      const nlohmann::json& normalized_args,
                                      const LLMGuiContextSnapshot& gui,
                                      const LLMSessionContext& session,
                                      const std::string& lang,
                                      const LinkPreviewContext* link_ctx = nullptr);

} // namespace RDK::LLM

#endif
