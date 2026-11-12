#include "ULLMLongTermMemoryLoader.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#include <QCryptographicHash>

namespace RDK::LLM {

namespace {

std::string readFileLimited(const std::filesystem::path& path, const std::size_t max_bytes)
{
    std::ifstream in(path);
    if(!in)
        return {};
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if(content.size() > max_bytes)
    {
        content.resize(max_bytes);
        content += "\n...(truncated)";
    }
    return content;
}

std::string projectHash8(const std::string& project_xml_path)
{
    if(project_xml_path.empty())
        return {};
    const QByteArray hash =
        QCryptographicHash::hash(QByteArray::fromStdString(project_xml_path),
                                QCryptographicHash::Sha256);
    return hash.toHex().left(8).toStdString();
}

} // namespace

std::string loadLongTermMemoryBlock(const std::filesystem::path& repository_root,
                                    const int user_id, const std::string& project_xml_path)
{
    if(user_id < 0)
        return {};

    std::ostringstream block;
    const std::filesystem::path memory_root = repository_root / "LLM" / "memory";

    const std::filesystem::path prefs =
        memory_root / ("user_" + std::to_string(user_id)) / "preferences.json";
    if(std::filesystem::exists(prefs))
    {
        block << "## Long-term memory (user preferences)\n";
        block << readFileLimited(prefs, 1024) << "\n";
    }

    const std::string hash = projectHash8(project_xml_path);
    if(!hash.empty())
    {
        const std::filesystem::path rules = memory_root / ("project_" + hash) / "rules.md";
        if(std::filesystem::exists(rules))
        {
            block << "## Long-term memory (project rules)\n";
            block << readFileLimited(rules, 2048) << "\n";
        }
    }

    return block.str();
}

} // namespace RDK::LLM
