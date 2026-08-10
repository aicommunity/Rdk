#include "ULLMUserMessages.h"

namespace RDK::LLM {

namespace {

std::string lookup(const std::string& key, const std::string& lang)
{
    const bool ru = lang.rfind("ru", 0) == 0;
    if(key == "confirmation.apply_hint")
        return ru ? "Нажмите Apply для подтверждения." : "Press Apply to confirm.";
    if(key == "confirmation.required")
        return ru ? "Требуется подтверждение для: {tool_name}." : "Confirmation required for: {tool_name}.";
    if(key == "args.missing.configuration_path")
        return ru ? "Укажите путь к конфигурации." : "Specify configuration path.";
    if(key == "error.max_rounds")
        return ru ? "Остановлено: слишком много шагов." : "Stopped: too many steps.";
    if(key == "error.no_suitable_tool")
        return ru ? "Не найдено подходящее действие." : "Cannot find a suitable action.";
    if(key == "error.query_inspect_failed")
        return ru ? "Не удалось прочитать модель или документацию. Откройте конфигурацию "
                    "или уточните вопрос."
                  : "Could not read the model or docs. Open a configuration or clarify the question.";
    if(key == "error.description_write_required")
        return ru ? "Нужно записать описание в project description: после анализа конфигурации "
                    "вызовите update_configuration с текстом описания (без приветствий и "
                    "выдуманных названий продукта)."
                  : "Write the project description: after inspecting the configuration, call "
                    "update_configuration with concrete description text (no greetings or invented "
                    "product names).";
    if(key == "error.model_too_weak_for_request")
        return ru ? "Текущая модель слишком слаба для этого запроса: агент не смог надёжно "
                    "вызвать нужные инструменты. Смените модель в настройках LLM на более "
                    "сильную (например ollama-thinking / qwen3:14b), упросьте запрос или "
                    "уточните путь/цель."
                  : "The current model is too weak for this request: the agent could not reliably "
                    "invoke the required tools. Switch the LLM settings to a stronger model "
                    "(e.g. ollama-thinking / qwen3:14b), simplify the request, or clarify the "
                    "path/goal.";
    if(key == "error.index_missing")
        return ru ? "Индекс знаний недоступен." : "Knowledge index unavailable.";
    return key;
}

} // namespace

std::string formatUserMessage(const std::string& key, const std::string& lang,
                              const std::map<std::string, std::string>& placeholders)
{
    std::string out = lookup(key, lang);
    for(const auto& [name, value] : placeholders)
    {
        const std::string needle = "{" + name + "}";
        std::size_t pos = out.find(needle);
        while(pos != std::string::npos)
        {
            out.replace(pos, needle.size(), value);
            pos = out.find(needle, pos + value.size());
        }
    }
    return out;
}

} // namespace RDK::LLM
