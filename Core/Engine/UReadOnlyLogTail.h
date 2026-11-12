#ifndef RDK_UREAD_ONLY_LOG_TAIL_H
#define RDK_UREAD_ONLY_LOG_TAIL_H

#include <cstddef>
#include <ctime>
#include <string>
#include <vector>

#include "../Application/UApplication.h"
#include "UGlogFileTail.h"

namespace RDK {

struct LogTailLine {
    int severity = 0;
    std::time_t timestamp = 0;
    std::string text;
};

class UReadOnlyLogTail {
public:
    void syncFromPaths(const ApplicationLogReadPaths& paths);
    void mark();
    std::vector<LogTailLine> collectDelta(std::size_t max_lines, int min_severity) const;
    std::vector<LogTailLine> readTail(std::size_t max_lines, int min_severity) const;

private:
    mutable UGlogFileTail m_tail;
    mutable std::vector<LogTailLine> m_staging;
    mutable std::size_t m_mark_index = 0;

    static LogTailLine fromEntry(const UGlogFileTailEntry& entry);
    static bool passesSeverity(int severity, int min_severity);
    void drainNewEntries() const;
    static std::vector<LogTailLine> filterAndTakeLast(const std::vector<LogTailLine>& source,
                                                      std::size_t from_index,
                                                      std::size_t max_lines,
                                                      int min_severity);
};

} // namespace RDK

#endif
