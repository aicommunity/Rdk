#ifndef RDK_UOLLAMA_CHAT_TEMPLATE_H
#define RDK_UOLLAMA_CHAT_TEMPLATE_H

#include <string>
#include <vector>

#include "../LlmTypes.h"

namespace RDK::LLM {

bool isOllamaProvider(const LLMProviderProfile& profile);

OllamaChatTemplateFamily detectChatTemplateFamily(const std::string& model_name,
                                                OllamaChatTemplateFamily hint =
                                                    OllamaChatTemplateFamily::Auto);

OllamaChatTemplateFamily resolveChatTemplateFamily(const LLMProviderProfile& profile);

const char* defaultRdkSystemPrompt();

std::vector<LLMMessage> ensureRdkSystemPrompt(std::vector<LLMMessage> messages);

std::vector<LLMMessage> prepareMessagesForOllama(const LLMProviderProfile& profile,
                                                 std::vector<LLMMessage> messages);

/// Single-string prompt for native /generate-style fallback (embedded).
std::string formatPromptWithTemplate(OllamaChatTemplateFamily family,
                                     const std::vector<LLMMessage>& messages);

nlohmann::json buildOpenAiChatMessagesJson(const std::vector<LLMMessage>& messages);

} // namespace RDK::LLM

#endif
