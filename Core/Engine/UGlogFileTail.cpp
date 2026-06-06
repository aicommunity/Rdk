#include "UGlogFileTail.h"

#include "../../Deploy/Include/rdk_logging.h"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <chrono>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

namespace RDK
{

namespace
{
int MapSeverityTag(const std::string& tag)
{
 if(tag == "FATAL")
  return RDK_EX_FATAL;
 if(tag == "ERROR")
  return RDK_EX_ERROR;
 if(tag == "WARNING")
  return RDK_EX_WARNING;
 if(tag == "INFO")
  return RDK_EX_INFO;
 return RDK_EX_INFO;
}
}

void UGlogFileTail::Reset(const std::string& directory, const std::string& base_name, std::time_t session_start)
{
 Directories.clear();
 if(!directory.empty())
  Directories.push_back(directory);
 BaseName = base_name.empty() ? std::string("rdk") : base_name;
 SessionStart = session_start;
 SessionYear = 1970;
 if(session_start != 0)
 {
  std::tm* tm = std::localtime(&session_start);
  if(tm)
   SessionYear = tm->tm_year + 1900;
 }
 Files.clear();
}

void UGlogFileTail::AddDirectory(const std::string& directory)
{
 if(directory.empty())
  return;
 auto it = std::find(Directories.begin(), Directories.end(), directory);
 if(it != Directories.end())
  return;
 Directories.push_back(directory);
}

std::vector<UGlogFileTailEntry> UGlogFileTail::CollectNewMessages()
{
 std::vector<UGlogFileTailEntry> collected;
 if(Directories.empty())
  return collected;

 DiscoverFiles();
 for(auto& entry : Files)
  AppendFile(entry.second, collected);

 std::sort(collected.begin(), collected.end(),
           [](const UGlogFileTailEntry& lhs, const UGlogFileTailEntry& rhs)
           {
             if(lhs.Timestamp == rhs.Timestamp)
               return lhs.Text < rhs.Text;
             return lhs.Timestamp < rhs.Timestamp;
           });
 return collected;
}

void UGlogFileTail::DiscoverFiles()
{
 std::error_code ec;
 for(const auto& directory : Directories)
 {
  ec.clear();
  fs::path dir_path = fs::path(directory);
  if(!fs::exists(dir_path, ec) || !fs::is_directory(dir_path, ec))
   continue;

  for(const auto& entry : fs::directory_iterator(dir_path, ec))
  {
   if(ec)
    break;
   if(!entry.is_regular_file(ec))
    continue;

   const std::string filename = entry.path().filename().string();
   std::string severity_tag;
   std::time_t file_timestamp = 0;
   if(!MatchLogFile(filename, severity_tag, file_timestamp))
    continue;
   if(file_timestamp + 1 < SessionStart)
    continue;
   const std::string key = entry.path().string();
   if(Files.find(key) != Files.end())
    continue;

   FileState state;
   state.Path = entry.path().string();
   state.SeverityTag = severity_tag;
   state.FileStartTime = file_timestamp;
   state.Offset = 0;
   state.Stream.open(state.Path);
   if(!state.Stream.is_open())
    continue;
   state.Stream.seekg(0, std::ios::beg);
   Files.emplace(key, std::move(state));
  }
 }
}

bool UGlogFileTail::MatchLogFile(const std::string& filename, std::string& severity, std::time_t& file_timestamp) const
{
 const std::string prefix = BaseName + ".";
 auto prefix_pos = filename.find(prefix);
 if(prefix_pos != 0)
  return false;

 const std::string middle_token = ".log.";
 auto log_pos = filename.find(middle_token);
 if(log_pos == std::string::npos)
  return false;

 const std::size_t severity_start = log_pos + middle_token.size();
 const std::size_t severity_end = filename.find('.', severity_start);
 if(severity_end == std::string::npos)
  return false;

 severity = filename.substr(severity_start, severity_end - severity_start);
 const std::size_t stamp_start = severity_end + 1;
 const std::size_t stamp_end = filename.find('.', stamp_start);
 if(stamp_end == std::string::npos)
  return false;

 const std::string stamp = filename.substr(stamp_start, stamp_end - stamp_start);
 if(stamp.size() < 15)
  return false;

 auto to_int = [](const std::string& str, std::size_t pos, std::size_t count) -> int
 {
  return std::stoi(str.substr(pos, count));
 };

 try
 {
  const int year = to_int(stamp, 0, 4);
  const int month = to_int(stamp, 4, 2);
  const int day = to_int(stamp, 6, 2);
  const int hour = to_int(stamp, 9, 2);
  const int minute = to_int(stamp, 11, 2);
  const int second = to_int(stamp, 13, 2);

  std::tm tm{};
  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_sec = second;
  tm.tm_isdst = -1;
  file_timestamp = std::mktime(&tm);
 }
 catch(...)
 {
  return false;
 }

 return true;
}

void UGlogFileTail::AppendFile(FileState& state, std::vector<UGlogFileTailEntry>& buffer)
{
 if(!state.Stream.is_open())
 {
  state.Stream.clear();
  state.Stream.open(state.Path);
  if(!state.Stream.is_open())
   return;
 }

 state.Stream.clear();
 state.Stream.seekg(state.Offset);

 std::string line;
 while(std::getline(state.Stream, line))
 {
  UGlogFileTailEntry entry;
  if(ParseLine(line, state.SeverityTag, entry))
   buffer.push_back(std::move(entry));
 }

 auto pos = state.Stream.tellg();
 if(pos != std::streampos(-1))
 {
  state.Offset = pos;
 }
 else
 {
  state.Stream.clear();
  state.Stream.seekg(0, std::ios::end);
  state.Offset = state.Stream.tellg();
 }
}

bool UGlogFileTail::ParseTwoDigits(const std::string& line, std::size_t pos, int& value) const
{
 if(pos + 1 >= line.size())
  return false;
 if(!std::isdigit(static_cast<unsigned char>(line[pos])) ||
    !std::isdigit(static_cast<unsigned char>(line[pos + 1])))
  return false;
 value = (line[pos] - '0') * 10 + (line[pos + 1] - '0');
 return true;
}

bool UGlogFileTail::ParseLine(const std::string& line, const std::string& severity_tag, UGlogFileTailEntry& entry) const
{
 if(line.size() < 20)
  return false;
 if(line[0] != 'I' && line[0] != 'W' && line[0] != 'E' && line[0] != 'F')
  return false;

 int month = 0;
 int day = 0;
 int hour = 0;
 int minute = 0;
 int second = 0;
 if(!ParseTwoDigits(line, 1, month) ||
    !ParseTwoDigits(line, 3, day) ||
    !ParseTwoDigits(line, 6, hour) ||
    !ParseTwoDigits(line, 9, minute) ||
    !ParseTwoDigits(line, 12, second))
  return false;

 std::tm tm{};
 tm.tm_year = SessionYear - 1900;
 tm.tm_mon = month - 1;
 tm.tm_mday = day;
 tm.tm_hour = hour;
 tm.tm_min = minute;
 tm.tm_sec = second;
 tm.tm_isdst = -1;
 std::time_t timestamp = std::mktime(&tm);

 if(timestamp < SessionStart)
  return false;

 auto payload_pos = line.find("] ");
 if(payload_pos == std::string::npos)
  return false;
 payload_pos += 2;
 std::string payload = line.substr(payload_pos);
 while(!payload.empty() && std::isspace(static_cast<unsigned char>(payload.front())))
  payload.erase(payload.begin());

 entry.Timestamp = timestamp;
 if(payload.rfind("[DEBUG] ", 0) == 0)
 {
  entry.Severity = RDK_EX_DEBUG;
  payload.erase(0, 8);
 }
 else
 {
  entry.Severity = MapSeverityTag(severity_tag);
 }
 entry.Text = payload;
 return true;
}

} // namespace RDK

