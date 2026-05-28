#include "UReadOnlyLogTail.h"

namespace RDK {

LogTailLine UReadOnlyLogTail::fromEntry(const UGlogFileTailEntry& entry)
{
    LogTailLine line;
    line.severity = entry.Severity;
    line.timestamp = entry.Timestamp;
    line.text = entry.Text;
    return line;
}

bool UReadOnlyLogTail::passesSeverity(int severity, int min_severity)
{
    if(severity <= 0)
        return false;
    return severity <= min_severity;
}

void UReadOnlyLogTail::syncFromPaths(const ApplicationLogReadPaths& paths)
{
    m_staging.clear();
    m_mark_index = 0;

    if(paths.directories.empty())
    {
        m_tail.Reset({}, paths.base_name, paths.session_start_unix);
        return;
    }

    m_tail.Reset(paths.directories.front(), paths.base_name, paths.session_start_unix);
    for(std::size_t i = 1; i < paths.directories.size(); ++i)
        m_tail.AddDirectory(paths.directories[i]);
}

void UReadOnlyLogTail::drainNewEntries() const
{
    const std::vector<UGlogFileTailEntry> batch = m_tail.CollectNewMessages();
    for(const UGlogFileTailEntry& entry : batch)
        m_staging.push_back(fromEntry(entry));
}

void UReadOnlyLogTail::mark()
{
    drainNewEntries();
    m_mark_index = m_staging.size();
}

std::vector<LogTailLine> UReadOnlyLogTail::filterAndTakeLast(const std::vector<LogTailLine>& source,
                                                             std::size_t from_index,
                                                             std::size_t max_lines,
                                                             int min_severity)
{
    std::vector<LogTailLine> filtered;
    filtered.reserve(source.size());
    for(std::size_t i = from_index; i < source.size(); ++i)
    {
        if(passesSeverity(source[i].severity, min_severity))
            filtered.push_back(source[i]);
    }
    if(filtered.size() > max_lines)
        filtered.erase(filtered.begin(), filtered.end() - static_cast<std::ptrdiff_t>(max_lines));
    return filtered;
}

std::vector<LogTailLine> UReadOnlyLogTail::collectDelta(std::size_t max_lines, int min_severity) const
{
    drainNewEntries();
    return filterAndTakeLast(m_staging, m_mark_index, max_lines, min_severity);
}

std::vector<LogTailLine> UReadOnlyLogTail::readTail(std::size_t max_lines, int min_severity) const
{
    drainNewEntries();
    return filterAndTakeLast(m_staging, 0, max_lines, min_severity);
}

} // namespace RDK
