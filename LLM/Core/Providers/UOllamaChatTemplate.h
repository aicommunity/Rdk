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

std::string buildRdkSystemPrompt(const std::string& response_language);

std::vector<LLMMessage> ensureRdkSystemPrompt(std::vector<LLMMessage> messages,
                                              const std::string& response_language = "en");

std::vector<LLMMessage> prepareMessagesForOllama(const LLMProviderProfile& profile,
                                                 std::vector<LLMMessage> messages,
                                                 const std::string& response_language = "en");

/// Text for a single assistant turn in string prompts (embedded / generate API).
std::string assistantMessageTextForPrompt(const LLMMessage& message);

/// Single-string prompt for native /generate-style fallback (embedded).
std::string formatPromptWithTemplate(OllamaChatTemplateFamily family,
                                     const std::vector<LLMMessage>& messages);

nlohmann::json buildOpenAiChatMessagesJson(const std::vector<LLMMessage>& messages);

} // namespace RDK::LLM

#endif
