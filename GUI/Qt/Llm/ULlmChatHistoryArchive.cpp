#include "ULlmChatHistoryArchive.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <regex>
#include <sstream>

namespace fs = std::filesystem;

namespace {

std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::size_t findCaseInsensitive(const std::string& haystack, const std::string& needle,
                                std::size_t from = 0)
{
    const std::string h = toLower(haystack);
    const std::string n = toLower(needle);
    return h.find(n, from);
}

std::size_t rfindCaseInsensitive(const std::string& haystack, const std::string& needle)
{
    const std::string h = toLower(haystack);
    const std::string n = toLower(needle);
    return h.rfind(n);
}

} // namespace

ULlmChatHistoryArchive::ULlmChatHistoryArchive(fs::path bin_root)
    : m_bin_root(std::move(bin_root))
{
}

void ULlmChatHistoryArchive::markUnwritable() const
{
    m_writable = false;
}

std::string ULlmChatHistoryArchive::escapeHtmlAttribute(const std::string& value)
{
    std::string out;
    out.reserve(value.size());
    for(char c : value)
    {
        switch(c)
        {
        case '&':
            out += "&amp;";
            break;
        case '"':
            out += "&quot;";
            break;
        case '<':
            out += "&lt;";
            break;
        case '>':
            out += "&gt;";
            break;
        default:
            out += c;
            break;
        }
    }
    return out;
}

std::string ULlmChatHistoryArchive::formatDayDir(std::chrono::system_clock::time_point now)
{
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &t);
#else
    localtime_r(&t, &local_tm);
#endif
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &local_tm);
    return buf;
}

std::string ULlmChatHistoryArchive::formatFileName(std::chrono::system_clock::time_point now)
{
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &t);
#else
    localtime_r(&t, &local_tm);
#endif
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S-NeuroModelerChat.html", &local_tm);
    return buf;
}

std::string ULlmChatHistoryArchive::formatIsoLocal(std::chrono::system_clock::time_point now)
{
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &t);
#else
    localtime_r(&t, &local_tm);
#endif
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &local_tm);
    return buf;
}

fs::path ULlmChatHistoryArchive::resolveRoot() const
{
    fs::path root = m_bin_root / "AiChats";
    std::error_code ec;
    fs::create_directories(root, ec);
    if(ec)
    {
        std::fprintf(stderr, "ULlmChatHistoryArchive: create_directories failed: %s\n",
                     ec.message().c_str());
        markUnwritable();
    }
    return root;
}

std::optional<fs::path> ULlmChatHistoryArchive::ensureDayDirectory(const std::string& day_dir) const
{
    const fs::path root = resolveRoot();
    if(!m_writable)
        return std::nullopt;
    const fs::path day_path = root / day_dir;
    std::error_code ec;
    fs::create_directories(day_path, ec);
    if(ec)
    {
        std::fprintf(stderr, "ULlmChatHistoryArchive: day directory failed: %s\n",
                     ec.message().c_str());
        markUnwritable();
        return std::nullopt;
    }
    return day_path;
}

std::optional<fs::path> ULlmChatHistoryArchive::startNewChatFile(const ChatArchiveMeta& meta,
                                                                 std::chrono::system_clock::time_point now)
{
    if(!m_writable)
        return std::nullopt;

    const std::string day = formatDayDir(now);
    const auto day_path = ensureDayDirectory(day);
    if(!day_path)
        return std::nullopt;

    std::string base_name = formatFileName(now);
    fs::path file_path = *day_path / base_name;
    int suffix = 2;
    while(fs::exists(file_path))
    {
        const std::string stem = base_name;
        const auto dot = stem.rfind(".html");
        const std::string without_ext =
            dot == std::string::npos ? stem : stem.substr(0, dot);
        file_path = *day_path / (without_ext + "-" + std::to_string(suffix) + ".html");
        ++suffix;
    }

    const std::string created_at =
        meta.created_at_iso.empty() ? formatIsoLocal(now) : meta.created_at_iso;
    const std::string title_date = created_at.substr(0, std::min(created_at.size(), std::size_t(16)));

    std::ostringstream html;
    html << "<!DOCTYPE html>\n"
         << "<html lang=\"ru\">\n"
         << "<head>\n"
         << "  <meta charset=\"utf-8\">\n"
         << "  <meta name=\"generator\" content=\"NeuroModeler LLM Assistant\">\n"
         << "  <meta name=\"nmsdk-session-id\" content=\"" << escapeHtmlAttribute(meta.session_id)
         << "\">\n"
         << "  <meta name=\"nmsdk-created-at\" content=\"" << escapeHtmlAttribute(created_at)
         << "\">\n"
         << "  <meta name=\"nmsdk-provider-id\" content=\"" << escapeHtmlAttribute(meta.provider_id)
         << "\">\n"
         << "  <meta name=\"nmsdk-project-path\" content=\""
         << escapeHtmlAttribute(meta.project_path) << "\">\n"
         << "  <meta name=\"nmsdk-archived\" content=\"false\">\n"
         << "  <title>NeuroModeler chat " << escapeHtmlAttribute(title_date) << "</title>\n"
         << "</head>\n"
         << "<body>\n"
         << "</body>\n"
         << "</html>\n";

    if(!writeStringToFile(file_path, html.str()))
    {
        markUnwritable();
        return std::nullopt;
    }
    return file_path;
}

std::string ULlmChatHistoryArchive::wrapFragmentIfNeeded(const std::string& html_fragment)
{
    static const std::regex block_start(R"(^\s*<(p|div|h[1-6])\b)", std::regex::icase);
    if(std::regex_search(html_fragment, block_start))
        return html_fragment;
    return "<p>" + html_fragment + "</p>";
}

bool ULlmChatHistoryArchive::insertBeforeBodyClose(std::string& content, const std::string& insert)
{
    const std::size_t close_pos = rfindCaseInsensitive(content, "</body>");
    if(close_pos == std::string::npos)
        return false;
    content.insert(close_pos, insert + "\n");
    return true;
}

bool ULlmChatHistoryArchive::appendHtmlFragment(const fs::path& chat_file,
                                                const std::string& html_fragment)
{
    if(!m_writable || html_fragment.empty())
        return false;

    std::string content = readFileToString(chat_file);
    if(content.empty())
    {
        markUnwritable();
        return false;
    }

    const std::string wrapped = wrapFragmentIfNeeded(html_fragment);
    if(!insertBeforeBodyClose(content, wrapped))
        return false;

    if(!writeStringToFile(chat_file, content))
    {
        markUnwritable();
        return false;
    }
    return true;
}

bool ULlmChatHistoryArchive::finalizeChat(const fs::path& chat_file)
{
    if(!fs::exists(chat_file))
        return false;

    std::string content = readFileToString(chat_file);
    if(content.empty())
        return false;

    const std::string from = R"(<meta name="nmsdk-archived" content="false">)";
    const std::string to = R"(<meta name="nmsdk-archived" content="true">)";
    const std::size_t pos = content.find(from);
    if(pos != std::string::npos)
        content.replace(pos, from.size(), to);
    else
    {
        const std::size_t head_close = findCaseInsensitive(content, "</head>");
        if(head_close == std::string::npos)
            return false;
        content.insert(head_close,
                       "  <meta name=\"nmsdk-archived\" content=\"true\">\n");
    }
    return writeStringToFile(chat_file, content);
}

std::string ULlmChatHistoryArchive::stripHtmlTags(const std::string& html)
{
    std::string out;
    out.reserve(html.size());
    bool in_tag = false;
    for(char c : html)
    {
        if(c == '<')
        {
            in_tag = true;
            continue;
        }
        if(c == '>')
        {
            in_tag = false;
            continue;
        }
        if(!in_tag)
            out += c;
    }
    return out;
}

std::string ULlmChatHistoryArchive::previewFromBody(const std::string& body_html)
{
    std::string plain = stripHtmlTags(body_html);
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    plain.erase(plain.begin(),
                std::find_if(plain.begin(), plain.end(), not_space));
    plain.erase(
        std::find_if(plain.rbegin(), plain.rend(), not_space).base(), plain.end());
    if(plain.size() > 80)
        plain.resize(80);
    return plain;
}

std::int64_t ULlmChatHistoryArchive::fileMtimeUnix(const fs::path& path)
{
    std::error_code ec;
    const auto ft = fs::last_write_time(path, ec);
    if(ec)
        return 0;
    return std::chrono::duration_cast<std::chrono::seconds>(
               ft.time_since_epoch())
        .count();
}

std::vector<ChatListEntry> ULlmChatHistoryArchive::listChats(std::size_t max_count) const
{
    std::vector<ChatListEntry> entries;
    const fs::path root = m_bin_root / "AiChats";
    if(!fs::exists(root))
        return entries;

    std::error_code ec;
    for(const auto& day_entry : fs::directory_iterator(root, ec))
    {
        if(ec)
            break;
        if(!day_entry.is_directory())
            continue;
        for(const auto& file_entry : fs::directory_iterator(day_entry.path(), ec))
        {
            if(ec)
                break;
            if(!file_entry.is_regular_file())
                continue;
            const fs::path p = file_entry.path();
            if(p.extension() != ".html")
                continue;

            ChatListEntry item;
            item.file_path = p;
            item.mtime_unix = fileMtimeUnix(p);
            const ChatArchiveMeta meta = parseMeta(p);
            item.session_id = meta.session_id;
            item.created_at_iso = meta.created_at_iso;
            item.preview = previewFromBody(loadChatBodyHtml(p));
            entries.push_back(std::move(item));
        }
    }

    std::sort(entries.begin(), entries.end(),
              [](const ChatListEntry& a, const ChatListEntry& b) {
                  if(a.mtime_unix != b.mtime_unix)
                      return a.mtime_unix > b.mtime_unix;
                  return a.created_at_iso > b.created_at_iso;
              });

    if(entries.size() > max_count)
        entries.resize(max_count);
    return entries;
}

std::string ULlmChatHistoryArchive::loadChatBodyHtml(const fs::path& chat_file) const
{
    const std::string content = readFileToString(chat_file);
    if(content.empty())
        return {};

    const std::size_t body_open = findCaseInsensitive(content, "<body");
    if(body_open == std::string::npos)
        return {};
    const std::size_t gt_after_body = content.find('>', body_open);
    if(gt_after_body == std::string::npos)
        return {};
    const std::size_t inner_start = gt_after_body + 1;
    const std::size_t body_close = rfindCaseInsensitive(content, "</body>");
    if(body_close == std::string::npos || body_close <= inner_start)
        return {};

    std::string inner = content.substr(inner_start, body_close - inner_start);
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    inner.erase(inner.begin(), std::find_if(inner.begin(), inner.end(), not_space));
    inner.erase(std::find_if(inner.rbegin(), inner.rend(), not_space).base(), inner.end());
    return inner;
}

std::string ULlmChatHistoryArchive::parseSessionId(const fs::path& chat_file) const
{
    return parseMeta(chat_file).session_id;
}

ChatArchiveMeta ULlmChatHistoryArchive::parseMeta(const fs::path& chat_file) const
{
    ChatArchiveMeta meta;
    const std::string content = readFileToString(chat_file);
    if(content.empty())
        return meta;

    auto extract = [&content](const char* name) -> std::string {
        const std::string pattern = std::string("<meta name=\"nmsdk-") + name
                                    + "\" content=\"([^\"]*)\"";
        std::regex re(pattern);
        std::smatch match;
        if(std::regex_search(content, match, re) && match.size() > 1)
            return match[1].str();
        return {};
    };

    meta.session_id = extract("session-id");
    meta.created_at_iso = extract("created-at");
    meta.provider_id = extract("provider-id");
    meta.project_path = extract("project-path");
    const std::string archived = extract("archived");
    meta.archived = (archived == "true");
    return meta;
}

std::string ULlmChatHistoryArchive::readFileToString(const fs::path& path)
{
    std::ifstream in(path, std::ios::binary);
    if(!in)
        return {};
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

bool ULlmChatHistoryArchive::writeStringToFile(const fs::path& path, const std::string& content)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if(!out)
        return false;
    out << content;
    return static_cast<bool>(out);
}
