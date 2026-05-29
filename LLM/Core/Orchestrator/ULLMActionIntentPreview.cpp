#include "ULLMActionIntentPreview.h"

#include <filesystem>
#include <sstream>

namespace RDK::LLM {

namespace {

bool isRussian(const std::string& lang) { return lang.rfind("ru", 0) == 0; }

std::string noneOr(const std::string& value)
{
    return value.empty() ? "—" : value;
}

std::string projectBasename(const LLMGuiContextSnapshot& gui)
{
    if(gui.project_xml_path.empty())
        return {};
    try
    {
        return std::filesystem::path(gui.project_xml_path).filename().string();
    }
    catch(...)
    {
        return gui.project_xml_path;
    }
}

std::string localizedToolLabel(const std::string& tool_name, bool ru)
{
    if(tool_name == "add_component")
        return ru ? "Добавить компонент" : "Add component";
    if(tool_name == "remove_component")
        return ru ? "Удалить компонент" : "Remove component";
    if(tool_name == "set_property")
        return ru ? "Изменить свойство" : "Set property";
    if(tool_name == "connect_components")
        return ru ? "Соединить компоненты" : "Connect components";
    if(tool_name == "disconnect_components")
        return ru ? "Разорвать связь" : "Disconnect components";
    if(tool_name == "open_recent_configuration")
        return ru ? "Открыть недавнюю конфигурацию" : "Open recent configuration";
    if(tool_name == "create_configuration")
        return ru ? "Создать конфигурацию" : "Create configuration";
    if(tool_name == "load_configuration" || tool_name == "load_project")
        return ru ? "Загрузить конфигурацию" : "Load configuration";
    if(tool_name == "save_configuration" || tool_name == "save_project")
        return ru ? "Сохранить конфигурацию" : "Save configuration";
    if(tool_name == "close_configuration")
        return ru ? "Закрыть конфигурацию" : "Close configuration";
    return tool_name;
}

std::string localizedArgLabel(const std::string& key, bool ru)
{
    if(key == "parent_long_name")
        return ru ? "Родитель" : "Parent";
    if(key == "long_name")
        return ru ? "Компонент" : "Component";
    if(key == "class_name")
        return ru ? "Класс" : "Class";
    if(key == "short_name")
        return ru ? "Краткое имя" : "Short name";
    if(key == "from_long_name")
        return ru ? "От" : "From";
    if(key == "to_long_name")
        return ru ? "К" : "To";
    if(key == "from_port")
        return ru ? "Порт (от)" : "From port";
    if(key == "to_port")
        return ru ? "Порт (к)" : "To port";
    if(key == "property_name")
        return ru ? "Свойство" : "Property";
    if(key == "value")
        return ru ? "Значение" : "Value";
    if(key == "configuration_path")
        return ru ? "Путь конфигурации" : "Configuration path";
    if(key == "index")
        return ru ? "Индекс" : "Index";
    return key;
}

std::string jsonArgValueString(const nlohmann::json& v)
{
    if(v.is_string())
        return v.get<std::string>();
    if(v.is_number_integer())
        return std::to_string(v.get<int>());
    if(v.is_number_unsigned())
        return std::to_string(v.get<unsigned>());
    if(v.is_number_float())
        return std::to_string(v.get<double>());
    if(v.is_boolean())
        return v.get<bool>() ? "true" : "false";
    if(v.is_null())
        return {};
    return v.dump();
}

bool argValueEmpty(const nlohmann::json& v)
{
    if(v.is_null())
        return true;
    if(v.is_string())
        return v.get<std::string>().empty();
    if(v.is_object())
        return v.empty();
    if(v.is_array())
        return v.empty();
    return false;
}

} // namespace

std::string formatActionIntentPreview(const std::string& tool_name,
                                      const nlohmann::json& normalized_args,
                                      const LLMGuiContextSnapshot& gui,
                                      const LLMSessionContext& session,
                                      const std::string& lang,
                                      const LinkPreviewContext* link_ctx)
{
    const bool ru = isRussian(lang);
    std::ostringstream out;

    out << (ru ? "## Действие\n" : "## Action\n");
    out << localizedToolLabel(tool_name, ru) << " (`" << tool_name << "`)\n\n";

    out << (ru ? "## Позиция в модели\n" : "## Model position\n");
    out << (ru ? "- Уровень схемы: " : "- Diagram scope: ")
        << noneOr(gui.diagram_scope_long_name) << "\n";
    const std::string current =
        !gui.current_component_long_name.empty() ? gui.current_component_long_name
                                                 : gui.focused_component_long_name;
    out << (ru ? "- Текущий компонент: " : "- Current component: ") << noneOr(current) << "\n";
    const int channel = gui.channel_index != 0 ? gui.channel_index : session.active_channel_index;
    out << (ru ? "- Канал: " : "- Channel: ") << channel << "\n";
    const std::string project = projectBasename(gui);
    out << (ru ? "- Проект: " : "- Project: ")
        << noneOr(project.empty() && !session.project_loaded ? std::string()
                                                             : project)
        << "\n\n";

    out << (ru ? "## Параметры\n" : "## Parameters\n");
    bool any_param = false;
    if(normalized_args.is_object())
    {
        for(auto it = normalized_args.begin(); it != normalized_args.end(); ++it)
        {
            if(argValueEmpty(it.value()))
                continue;
            any_param = true;
            out << "- " << localizedArgLabel(it.key(), ru) << ": "
                << jsonArgValueString(it.value()) << "\n";
        }
    }
    if(!any_param)
        out << (ru ? "- (нет параметров)\n" : "- (no parameters)\n");

    if(link_ctx && !link_ctx->catalog_lines.empty())
    {
        out << "\n" << (ru ? "## Типичные связи (каталог)\n" : "## Typical links (catalog)\n");
        for(const std::string& line : link_ctx->catalog_lines)
            out << "- " << line << "\n";
    }

    return out.str();
}

} // namespace RDK::LLM
