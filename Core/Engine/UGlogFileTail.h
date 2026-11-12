#ifndef UGLOGFILETAIL_H
#define UGLOGFILETAIL_H

#include <ctime>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace RDK
{

struct UGlogFileTailEntry
{
 std::time_t Timestamp;
 int Severity;
 std::string Text;
};

class UGlogFileTail
{
public:
 void Reset(const std::string& directory, const std::string& base_name, std::time_t session_start);
 void AddDirectory(const std::string& directory);
 std::vector<UGlogFileTailEntry> CollectNewMessages();

private:
 struct FileState
 {
  std::string Path;
  std::string SeverityTag;
  std::ifstream Stream;
  std::streampos Offset{0};
  std::time_t FileStartTime{0};
 };

 bool MatchLogFile(const std::string& filename, std::string& severity, std::time_t& file_timestamp) const;
 void DiscoverFiles();
 void AppendFile(FileState& state, std::vector<UGlogFileTailEntry>& buffer);
 bool ParseLine(const std::string& line, const std::string& severity_tag, UGlogFileTailEntry& entry) const;
 bool ParseTwoDigits(const std::string& line, std::size_t pos, int& value) const;
 std::vector<std::string> Directories;
 std::string BaseName;
 std::time_t SessionStart{0};
 int SessionYear{1970};
 std::map<std::string, FileState> Files;
};

}

#endif

