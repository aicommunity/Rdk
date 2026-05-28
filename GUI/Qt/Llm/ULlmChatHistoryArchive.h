#ifndef RDK_ULLM_CHAT_HISTORY_ARCHIVE_H
#define RDK_ULLM_CHAT_HISTORY_ARCHIVE_H

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

struct ChatArchiveMeta {
    std::string session_id;
    std::string provider_id;
    std::string project_path;
    std::string created_at_iso;
    bool archived = false;
};

struct ChatListEntry {
    std::filesystem::path file_path;
    std::string session_id;
    std::string created_at_iso;
    std::int64_t mtime_unix = 0;
    std::string preview;
};

class ULlmChatHistoryArchive {
public:
    explicit ULlmChatHistoryArchive(std::filesystem::path bin_root);

    std::filesystem::path resolveRoot() const;
    std::optional<std::filesystem::path> startNewChatFile(
        const ChatArchiveMeta& meta,
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now());

    bool appendHtmlFragment(const std::filesystem::path& chat_file,
                            const std::string& html_fragment);
    bool finalizeChat(const std::filesystem::path& chat_file);

    std::vector<ChatListEntry> listChats(std::size_t max_count = 200) const;
    std::string loadChatBodyHtml(const std::filesystem::path& chat_file) const;
    std::string parseSessionId(const std::filesystem::path& chat_file) const;
    ChatArchiveMeta parseMeta(const std::filesystem::path& chat_file) const;

    bool isWritable() const { return m_writable; }

private:
    std::filesystem::path m_bin_root;
    mutable bool m_writable = true;

    static std::string escapeHtmlAttribute(const std::string& value);
    static std::string formatFileName(std::chrono::system_clock::time_point now);
    static std::string formatDayDir(std::chrono::system_clock::time_point now);
    static std::string formatIsoLocal(std::chrono::system_clock::time_point now);
    static std::string wrapFragmentIfNeeded(const std::string& html_fragment);
    static std::string stripHtmlTags(const std::string& html);
    static std::string previewFromBody(const std::string& body_html);
    static bool insertBeforeBodyClose(std::string& content, const std::string& insert);
    static std::int64_t fileMtimeUnix(const std::filesystem::path& path);
    static std::string readFileToString(const std::filesystem::path& path);
    static bool writeStringToFile(const std::filesystem::path& path, const std::string& content);

    std::optional<std::filesystem::path> ensureDayDirectory(const std::string& day_dir) const;
    void markUnwritable() const;
};

#endif
