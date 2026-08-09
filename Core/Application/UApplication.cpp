#ifndef UApplication_CPP
#define UApplication_CPP

#ifndef __BORLANDC__
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#endif

#include "UApplication.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <limits>
#include <sstream>
#include <system_error>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <ctime>

#include "../Engine/UGlogGuiSink.h"
#include "../Engine/UGlogMirrorSink.h"
#include "../Engine/UJsonLogSink.h"
#include "../Engine/UExceptionLogger.h"
#include "../Engine/UEnvSupport.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../Deploy/Include/rdk_cpp_init.h"
#include "../../Deploy/Include/rdk_logging.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#ifdef RDK_USE_GLOG
#include <glog/logging.h>
#endif

namespace
{

std::string NormalizeLogDir(const std::string& dir)
{
 if(dir.empty())
  return dir;

 std::string normalized = dir;
 std::replace(normalized.begin(), normalized.end(), '\\', '/');
 if(!normalized.empty() && normalized.back() != '/')
  normalized.push_back('/');
 return normalized;
}

std::string RemoveSpaces(const std::string& str)
{
 std::string result;
 result.reserve(str.size());
 for(char c : str)
 {
  if(c != ' ')
   result.push_back(c);
 }
 return result;
}

std::string EnsureDirectoryAndNormalize(const std::string& dir)
{
 if(dir.empty())
  return {};

 std::string normalized = NormalizeLogDir(dir);
 std::error_code ec;
 std::filesystem::create_directories(normalized, ec);
 if(ec)
 {
  // Если не удалось создать каталог, возвращаем пустую строку,
  // чтобы вызывающий код мог безопасно перейти в fallback-режим.
  return {};
 }
 return normalized;
}

std::string StripTrailingSeparators(std::string path)
{
 while(!path.empty())
 {
  char suffix = path.back();
  if(suffix == '/' || suffix == '\\')
  {
   path.pop_back();
   continue;
  }
  break;
 }
 return path;
}

std::string TrimCopy(const std::string& value)
{
 auto start = value.find_first_not_of(" \t");
 if(start == std::string::npos)
  return {};
 auto end = value.find_last_not_of(" \t");
 return value.substr(start, end - start + 1);
}

std::string ToLowerCopy(std::string value)
{
 std::transform(value.begin(), value.end(), value.begin(),
                [](unsigned char ch){ return static_cast<char>(std::tolower(ch)); });
 return value;
}

int ClampSeverityValue(int severity)
{
 if(severity < RDK_EX_FATAL)
  return RDK_EX_FATAL;
 if(severity > RDK_EX_DEBUG)
  return RDK_EX_DEBUG;
 return severity;
}

int ParseSeverityString(const std::string& token, int fallback)
{
 std::string normalized = ToLowerCopy(TrimCopy(token));
 if(normalized.empty())
  return fallback;

 if(normalized == "fatal")
  return RDK_EX_FATAL;
 if(normalized == "error")
  return RDK_EX_ERROR;
 if(normalized == "warn" || normalized == "warning")
  return RDK_EX_WARNING;
 if(normalized == "info")
  return RDK_EX_INFO;
 if(normalized == "app")
  return RDK_EX_APP;
 if(normalized == "debug" || normalized == "trace")
  return RDK_EX_DEBUG;

 char* end_ptr = nullptr;
 long numeric = std::strtol(normalized.c_str(), &end_ptr, 10);
 if(end_ptr && *end_ptr == '\0')
  return ClampSeverityValue(static_cast<int>(numeric));

 return fallback;
}

int ParseVerbosityString(const std::string& token, int fallback)
{
 std::string trimmed = TrimCopy(token);
 if(trimmed.empty())
  return fallback;
 char* end_ptr = nullptr;
 long numeric = std::strtol(trimmed.c_str(), &end_ptr, 10);
 if(end_ptr && *end_ptr == '\0')
 {
  if(numeric < 0)
   return 0;
  if(numeric > 10)
   return 10;
  return static_cast<int>(numeric);
 }
 return fallback;
}

int ParseChannelIdentifier(const std::string& token)
{
 std::string key = ToLowerCopy(TrimCopy(token));
 if(key == "sys" || key == "system")
  return RDK_SYS_MESSAGE;
 if(key == "glob" || key == "global")
  return RDK_GLOB_MESSAGE;
 if(key == "default")
  return RDK::Logging::kDefaultChannel.Index;

 char* end_ptr = nullptr;
 long numeric = std::strtol(key.c_str(), &end_ptr, 10);
 if(end_ptr && *end_ptr == '\0')
  return static_cast<int>(numeric);

 return std::numeric_limits<int>::max();
}

}

using namespace std;


#ifdef __BORLANDC__
//#include "Bcb/Application.bcb.cpp"
#endif

void ExceptionHandler(int channel_index)
{
 (void)channel_index;
}

namespace RDK {

#ifndef __BORLANDC__
namespace po = boost::program_options;

po::options_description CmdLineDescription("Allowed options");
po::variables_map CmdVariablesMap;
#endif

namespace {

std::string joinProjectPath(const std::string& dir, const std::string& file)
{
 if(dir.empty())
  return file;
 std::string d = dir;
 if(d.back() != '/' && d.back() != '\\')
  d.push_back('/');
 return d + file;
}

std::string canonicalFilesystemPath(const std::string& path)
{
 if(path.empty())
  return path;
 std::error_code ec;
 const std::filesystem::path canon = std::filesystem::weakly_canonical(std::filesystem::path(path), ec);
 if(ec)
  return path;
 return canon.generic_string();
}

/// Absolute path with trailing '/', suitable for ProjectPath and configs dirs.
std::string normalizeProjectDirPath(const std::string& path)
{
 if(path.empty())
  return path;
 std::string normalized = canonicalFilesystemPath(path);
 if(normalized.empty())
  normalized = path;
 std::replace(normalized.begin(), normalized.end(), '\\', '/');
 if(!normalized.empty() && normalized.back() != '/')
  normalized.push_back('/');
 return normalized;
}

} // namespace

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UApplication::UApplication(void)
{
 Name="Application";
 LastProjectsListMaxSize=10;
 ProjectOpenFlag=false;
 TestMode=false;
 CloseAfterTest=true;
 AppIsInit = false;
 ConfigsMainPath="../../Configs/";
 //ModelsMainPath="../../Models/";
 ModelsMainPath="../../../Models/";
 ChangeUseNewXmlFormatProjectFile(false);
 ChangeUseNewProjectFilesStructure(false);
 StorageBuildMode = 1;
 LogCreationMode=0;
 CalcStopLogLevel=RDK_EX_FATAL;
 UserName="";
 UserId=-1;
 SetCoutLogMode(false);
 MirrorLogsToWorkDirFlag=true;
 LoggingInitialized=false;
 GoogleLoggingInitialized=false;
 FuncProgressBarCallback=0;
 //SetStandartXMLInCatalog();

 // DebugMode=false;

 LoadEnvLogOverrides();
 CurrentLogSessionStart = std::time(nullptr);
}

UApplication::~UApplication(void)
{

}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Название приложения
const std::string& UApplication::GetProgramName(void) const
{
 return ProgramName;
}

void UApplication::SetProgramName(const std::string &value)
{
 ProgramName = value;
 CachedLogBaseName.clear();
}

/// Имя файла приложения
const std::string& UApplication::GetApplicationFileName(void) const
{
 return ApplicationFileName;
}

bool UApplication::SetApplicationFileName(const std::string& value)
{
 if(ApplicationFileName == value)
  return true;
 ApplicationFileName=value;
 return true;
}

/// Рабочий каталог
const std::string& UApplication::GetWorkDirectory(void) const
{
 return WorkDirectory;
}

bool UApplication::SetWorkDirectory(const std::string& value)
{
 if(WorkDirectory == value)
  return true;
 WorkDirectory=value;
 UpdateLoggers();
 return true;
}

/// Относительный путь до папки с хранилищем конфигураций (обычно /Bin/Configs)
const std::string& UApplication::GetConfigsMainPath(void) const
{
 return ConfigsMainPath;
}

bool UApplication::SetConfigsMainPath(const std::string &value)
{
 if(ConfigsMainPath == value)
  return true;
 ConfigsMainPath=value;
 return true;
}

/// Путь к папке конфигураций текущего пользователя относительно Configs (Configs/Users/UserName/)
std::string UApplication::GetUserConfigPath(void) const
{
 if(UserName.empty())
  return std::string();
 return std::string("Users/") + UserName + "/";
}

/// Относительный путь до папки с библиотеками (в данном пути сформируется две папки - MockLibs, RTlibs)
const std::string& UApplication::GetLibrariesPath(void) const
{
 return LibrariesPath;
}

bool UApplication::SetLibrariesPath(const std::string &value)
{
 if(LibrariesPath == value)
  return true;
 LibrariesPath=value;
 return true;
}

/// Относительный путь до папки с описаниями классов
const std::string& UApplication::GetClDescPath(void) const
{
 return ClDescPath;
}

bool UApplication::SetClDescPath(const std::string &value)
{
 if(ClDescPath == value)
  return true;
 ClDescPath=value;
 return true;
}

const std::string& UApplication::GetWatchPresetsPath(void) const
{
 return WatchPresetsPath;
}

bool UApplication::SetWatchPresetsPath(const std::string &value)
{
 if(WatchPresetsPath == value)
  return true;
 WatchPresetsPath=value;
 return true;
}

/// Относительный путь до папки с хранилищем конфигураций (обычно /Bin/Configs)
const std::string& UApplication::GetDatabaseMainPath(void) const
{
 return DatabaseMainPath;
}

bool UApplication::SetDatabaseMainPath(const std::string &value)
{
 if(DatabaseMainPath == value)
  return true;
 DatabaseMainPath=value;
 return true;
}

/// Относительный путь до папки с хранилищем конфигураций (обычно /Bin/Configs)
const std::string& UApplication::GetStorageMountPoint(void) const
{
 return StorageMountPoint;
}

bool UApplication::SetStorageMountPoint(const std::string &value)
{
 if(StorageMountPoint == value)
  return true;
 StorageMountPoint=value;
 return true;
}


/// Относительный путь до папки с хранилищем моделей  (обычно /Bin/Models)
const std::string& UApplication::GetModelsMainPath(void) const
{
 return ModelsMainPath;
}

bool UApplication::SetModelsMainPath(const std::string &value)
{
 if(ModelsMainPath == value)
  return true;
 ModelsMainPath=value;
 return true;
}

const std::string& UApplication::GetUserName(void) const
{
    return UserName;
}

bool UApplication::SetUserName(const std::string &value)
{
    if(UserName == value)
     return true;
    UserName=value;
    return true;
}

int UApplication::GetUserId(void) const
{
    return UserId;
}

bool UApplication::SetUserId(int value)
{
    if(UserId == value)
     return true;
    UserId=value;
    return true;
}


// Признак наличия открытого проекта
bool UApplication::GetProjectOpenFlag(void) const
{
 return ProjectOpenFlag;
}

bool UApplication::SetProjectOpenFlag(bool value)
{
 if(ProjectOpenFlag == value)
  return true;

 ProjectOpenFlag=value;
 if(!ProjectOpenFlag)
  MirrorLogsToWorkDirFlag=true;
 CalcAppCaption();
 UpdateLoggers();
 return true;
}

// Путь до папки проекта
const std::string& UApplication::GetProjectPath(void) const
{
 return ProjectPath;
}

bool UApplication::SetProjectPath(const std::string& value)
{
 const std::string normalized = normalizeProjectDirPath(value);
 if(ProjectPath == normalized)
  return true;
 ProjectPath=normalized;
 UpdateLoggers();
// EngineControl->GetEngineStateThread()->CloseEventsLogFile();
 CalcAppCaption();
 return true;
}

// Имя файла проекта
const std::string& UApplication::GetProjectFileName(void) const
{
 return ProjectFileName;
}

bool UApplication::SetProjectFileName(const std::string& value)
{
 if(ProjectFileName == value)
  return true;
 ProjectFileName=value;
 CalcAppCaption();
 return true;
}

/// Список последних открытых проектов
const std::list<std::string>& UApplication::GetLastProjectsList(void) const
{
 return LastProjectsList;
}

bool UApplication::SetLastProjectsList(const std::list<std::string>& value)
{
 if(LastProjectsList == value)
  return true;
 LastProjectsList=value;
 return true;
}

/// Размер истории последних открытых проектов
int UApplication::GetLastProjectsListMaxSize(void) const
{
 return LastProjectsListMaxSize;
}

bool UApplication::SetLastProjectsListMaxSize(int value)
{
 if(LastProjectsListMaxSize == value)
  return true;
 LastProjectsListMaxSize=value;
 return true;
}

/// Заголовок приложения
const std::string& UApplication::GetAppCaption(void) const
{
 return AppCaption;
}

// Файл настроек проекта
const RDK::USerStorageXML& UApplication::GetProjectXml(void) const
{
 return ProjectXml;
}

// Файл настроек интефрейса
const RDK::USerStorageXML& UApplication::GetInterfaceXml(void) const
{
 return InterfaceXml;
}


/// Каталог логов
std::string UApplication::GetLogDir(void) const
{
 return Core_GetLogDir();
}

bool UApplication::SetLogDir(const std::string& value)
{
 if(value == Core_GetLogDir())
  return true;

 if(Core_SetLogDir(value.c_str()) == RDK_SUCCESS)
 {
  SetFixedLogPath(value);
  return true;
 }
 return false;
}

/// Флаг включения отладочного режима логирования
bool UApplication::GetDebugMode(void) const
{
 return Core_GetDebugMode();
}

bool UApplication::SetDebugMode(bool value)
{
 if(value == Core_GetDebugMode())
  return true;
 if(Core_SetDebugMode(value) == RDK_SUCCESS)
  return true;
 return true;
}


/// Текущий каталог логов (с учетом переопределения в проекте)
std::string UApplication::CalcCurrentLogDir(void) const
{
 if(!ProjectPath.empty())
  return ProjectPath + "EventsLog/";

 if(!FixedLogPath.empty())
  return FixedLogPath;

 return GetWorkLogDir();
}


std::string UApplication::GetWorkLogDir(void) const
{
 if(WorkDirectory.empty())
  return std::string("EventsLog/");

 std::string base = WorkDirectory;
 char last = base.empty() ? 0 : base.back();
 if(last != '/' && last != '\\')
  base.push_back('/');
 base += "EventsLog/";
 return base;
}

ApplicationLogReadPaths UApplication::GetApplicationLogReadPaths(void) const
{
 ApplicationLogReadPaths out;
 out.base_name = GetLogFileBaseName();
 out.session_start_unix = CurrentLogSessionStart;

 auto add_unique = [&out](const std::string& dir) {
  if(dir.empty())
   return;
  if(std::find(out.directories.begin(), out.directories.end(), dir) == out.directories.end())
   out.directories.push_back(dir);
 };

 add_unique(GetWorkLogDir());
 if(GetProjectOpenFlag())
  add_unique(CalcCurrentLogDir());

 return out;
}

const std::string& UApplication::GetLogFileBaseName(void) const
{
 if(!CachedLogBaseName.empty())
  return CachedLogBaseName;

 std::string base_name;
 if(!ProgramName.empty())
  base_name = ProgramName;
 else
 {
  base_name = std::string(RDK_APP_NAME);
  // Если RDK_APP_NAME не переопределён (равен "RDK"), используем "NeuroModeler" как fallback
  if(base_name == "RDK")
   base_name = "NeuroModeler";
 }
 CachedLogBaseName = RemoveSpaces(base_name);
 // Если после удаления пробелов получилось "RDK", используем "NeuroModeler" как fallback
 if(CachedLogBaseName == "RDK")
  CachedLogBaseName = "NeuroModeler";
 return CachedLogBaseName;
}

void UApplication::ApplyPrimaryLogDestination(const std::string& directory)
{
 std::string normalized = NormalizeLogDir(directory);
 if(normalized.empty())
  return;

 UGlogGuiSink::Instance().AddDirectory(normalized);

 if(Project)
 {
  ApplyLogRouting(Project->GetConfig());
 }
 else
 {
  TProjectConfig default_config;
  ApplyLogRouting(default_config);
 }

 const char* json_path_env = std::getenv("RDK_LOG_JSON_PATH");
 const std::string desired_json_path = json_path_env ? std::string(json_path_env) : std::string();
 if(desired_json_path.empty())
 {
  if(JsonSinkHandle)
  {
   Logging::UnregisterLogSink(JsonSinkHandle);
   JsonSinkHandle.reset();
   ActiveJsonSinkPath.clear();
  }
 }
 else if(desired_json_path != ActiveJsonSinkPath)
 {
  if(JsonSinkHandle)
  {
   Logging::UnregisterLogSink(JsonSinkHandle);
   JsonSinkHandle.reset();
  }
  auto new_sink = UJsonLogSink::Create(desired_json_path);
  if(new_sink)
  {
   JsonSinkHandle = new_sink;
   ActiveJsonSinkPath = desired_json_path;
   Logging::RegisterLogSink(JsonSinkHandle);
  }
 }

 std::string work_dir_normalized = EnsureDirectoryAndNormalize(GetWorkLogDir());

 // Ensure glog always writes to the working directory to prevent message loss
#ifdef RDK_USE_GLOG
 if(work_dir_normalized.empty())
  FLAGS_log_dir.clear();
 else
  FLAGS_log_dir = StripTrailingSeparators(work_dir_normalized);
#endif

 bool is_project_dir = !ProjectPath.empty() &&
                       normalized != work_dir_normalized;

 if(is_project_dir)
 {
  if(!ProjectLogMirrorHandle)
  {
   ProjectLogMirrorHandle = std::shared_ptr<Logging::ILogSink>(&UGlogMirrorSink::Instance(), [](Logging::ILogSink*){});
   Logging::RegisterLogSink(ProjectLogMirrorHandle);
  }
  UGlogMirrorSink::Instance().Configure(normalized, GetLogFileBaseName(), CurrentLogSessionStart);
 }
 else if(ProjectLogMirrorHandle)
 {
  Logging::UnregisterLogSink(ProjectLogMirrorHandle);
  ProjectLogMirrorHandle.reset();
  UGlogMirrorSink::Instance().Disable();
 }
}

void UApplication::LoadEnvLogOverrides(void)
{
 EnvLogOverrides = {};

 if(const char* level = std::getenv("RDK_LOG_LEVEL"))
  EnvLogOverrides.GlobalLevel = ParseSeverityToken(level, EnvLogOverrides.GlobalLevel);

 if(const char* sys_level = std::getenv("RDK_LOG_SYS_LEVEL"))
  RegisterChannelOverrideToken(std::string("sys:") + sys_level, EnvLogOverrides);

 if(const char* glob_level = std::getenv("RDK_LOG_GLOB_LEVEL"))
  RegisterChannelOverrideToken(std::string("glob:") + glob_level, EnvLogOverrides);

 if(const char* channels = std::getenv("RDK_LOG_CHANNELS"))
 {
  std::stringstream stream(channels);
  std::string token;
  while(std::getline(stream, token, ','))
   RegisterChannelOverrideToken(token, EnvLogOverrides);
 }

 if(const char* verbosity = std::getenv("RDK_LOG_VERBOSITY"))
  EnvLogOverrides.Verbosity = ParseVerbosityString(verbosity, EnvLogOverrides.Verbosity);
}

void UApplication::ApplyCliLogOverrides(const std::vector<std::string>& args)
{
 auto parse_inline = [&](const std::string& flag, const std::string& token) -> std::optional<std::string>
 {
  const std::string prefix = flag + "=";
  if(token.rfind(prefix, 0) == 0)
   return token.substr(prefix.size());
  return std::nullopt;
 };

 for(size_t i=0; i<args.size(); ++i)
 {
  const std::string& token = args[i];

  if(token == "--log-level" && i + 1 < args.size())
  {
   CliLogOverrides.GlobalLevel = ParseSeverityToken(args[++i], CliLogOverrides.GlobalLevel);
   continue;
  }
  if(auto value = parse_inline("--log-level", token))
  {
   CliLogOverrides.GlobalLevel = ParseSeverityToken(*value, CliLogOverrides.GlobalLevel);
   continue;
  }

  if(token == "--log-verbosity" && i + 1 < args.size())
  {
   CliLogOverrides.Verbosity = ParseVerbosityString(args[++i], CliLogOverrides.Verbosity);
   continue;
  }
  if(auto value = parse_inline("--log-verbosity", token))
  {
   CliLogOverrides.Verbosity = ParseVerbosityString(*value, CliLogOverrides.Verbosity);
   continue;
  }

  auto handle_channel_flag = [&](const std::string& flag, const std::string& channel_tag) -> bool
  {
   if(token == flag && i + 1 < args.size())
   {
    RegisterChannelOverrideToken(channel_tag + ":" + args[++i], CliLogOverrides);
    return true;
   }
   if(auto value = parse_inline(flag, token))
   {
    RegisterChannelOverrideToken(channel_tag + ":" + *value, CliLogOverrides);
    return true;
   }
   return false;
  };

  if(handle_channel_flag("--log-sys-level", "sys"))
   continue;
  if(handle_channel_flag("--log-glob-level", "glob"))
   continue;

  if(token == "--log-channel-level" && i + 1 < args.size())
  {
   RegisterChannelOverrideToken(args[++i], CliLogOverrides);
   continue;
  }
  if(auto value = parse_inline("--log-channel-level", token))
  {
   RegisterChannelOverrideToken(*value, CliLogOverrides);
   continue;
  }
 }
}

#ifndef __BORLANDC__
void UApplication::ApplyCliLogOverrides(const boost::program_options::variables_map& vm)
{
 if(vm.count("log-level"))
  CliLogOverrides.GlobalLevel = ParseSeverityToken(vm["log-level"].as<std::string>(), CliLogOverrides.GlobalLevel);

 if(vm.count("log-verbosity"))
  CliLogOverrides.Verbosity = ParseVerbosityString(std::to_string(vm["log-verbosity"].as<int>()), CliLogOverrides.Verbosity);

 if(vm.count("log-sys-level"))
  RegisterChannelOverrideToken(std::string("sys:") + vm["log-sys-level"].as<std::string>(), CliLogOverrides);

 if(vm.count("log-glob-level"))
  RegisterChannelOverrideToken(std::string("glob:") + vm["log-glob-level"].as<std::string>(), CliLogOverrides);

 if(vm.count("log-channel-level"))
 {
  const auto values = vm["log-channel-level"].as<std::vector<std::string>>();
  for(const auto& entry : values)
   RegisterChannelOverrideToken(entry, CliLogOverrides);
 }
}
#endif

void UApplication::RegisterChannelOverrideToken(const std::string& token, LogRoutingOverrides& target)
{
 if(token.find(',') != std::string::npos)
 {
  std::stringstream stream(token);
  std::string part;
  while(std::getline(stream, part, ','))
   RegisterChannelOverrideToken(part, target);
  return;
 }

 const auto separator = token.find_first_of(":=");
 if(separator == std::string::npos)
  return;

 const std::string channel_part = token.substr(0, separator);
 const std::string level_part = token.substr(separator + 1);
 const int channel_index = ParseChannelIdentifier(channel_part);
 if(channel_index == std::numeric_limits<int>::max())
  return;

 const int severity = ParseSeverityToken(level_part, -1);
 if(severity >= 0)
  target.ChannelLevels[channel_index] = severity;
}

int UApplication::ParseSeverityToken(const std::string& token, int fallback) const
{
 return ParseSeverityString(token, fallback);
}

int UApplication::DetermineBaseLogLevel(bool debug_mode) const
{
 if(debug_mode)
  return RDK_EX_DEBUG;
 return RDK_EX_INFO;
}

int UApplication::ResolveChannelLevel(int channel_index, int base_level) const
{
 int level = base_level;
 if(EnvLogOverrides.GlobalLevel >= 0)
  level = EnvLogOverrides.GlobalLevel;

 auto apply_specific = [&](const LogRoutingOverrides& overrides)
 {
  const auto it = overrides.ChannelLevels.find(channel_index);
  if(it != overrides.ChannelLevels.end())
   level = it->second;
 };

 apply_specific(EnvLogOverrides);

 if(CliLogOverrides.GlobalLevel >= 0)
  level = CliLogOverrides.GlobalLevel;

 apply_specific(CliLogOverrides);

 return level;
}

int UApplication::ResolveVerbosityLevel(int base_level) const
{
 int verbosity = base_level;
 if(EnvLogOverrides.Verbosity >= 0)
  verbosity = EnvLogOverrides.Verbosity;
 if(CliLogOverrides.Verbosity >= 0)
  verbosity = CliLogOverrides.Verbosity;
 if(verbosity < 0)
  return 0;
 return verbosity;
}

void UApplication::ApplyLogRouting(const TProjectConfig& config)
{
 const int base_system_level = DetermineBaseLogLevel(config.DebugMode);
 const int base_default_level = base_system_level;
 const int base_verbosity = ResolveVerbosityLevel(config.DebugMode ? 1 : 0);

 RDK::Logging::ResetChannelRuntimeConfig(
  RDK::Logging::ChannelRuntimeConfig{ResolveChannelLevel(RDK::Logging::kDefaultChannel.Index, base_default_level), base_verbosity});

 RDK::Logging::SetChannelRuntimeConfig(
  RDK_SYS_MESSAGE,
  RDK::Logging::ChannelRuntimeConfig{ResolveChannelLevel(RDK_SYS_MESSAGE, base_system_level), base_verbosity});

 RDK::Logging::SetChannelRuntimeConfig(
  RDK_GLOB_MESSAGE,
  RDK::Logging::ChannelRuntimeConfig{ResolveChannelLevel(RDK_GLOB_MESSAGE, base_system_level), base_verbosity});

 const int num_channels = static_cast<int>(config.ChannelsConfig.size());
 for(int i=0; i<num_channels; ++i)
 {
  const auto& channel_cfg = config.ChannelsConfig[i];
  const bool channel_debug = channel_cfg.DebugMode || config.DebugMode;
  const int base_channel_level = DetermineBaseLogLevel(channel_debug);
  const int channel_verbosity = ResolveVerbosityLevel(channel_debug ? 1 : 0);
  RDK::Logging::SetChannelRuntimeConfig(
   i,
   RDK::Logging::ChannelRuntimeConfig{ResolveChannelLevel(i, base_channel_level), channel_verbosity});
 }
}


/// Флаг, выставляется если включен режим тестирования
bool UApplication::IsTestMode(void) const
{
 return TestMode;
}

/// Имя файла с описанием тестов
const std::string& UApplication::GetTestsDescriptionFileName(void) const
{
 return TestsDescriptionFileName;
}

void UApplication::SetTestsDescriptionFileName(const std::string& value)
{
 if(TestsDescriptionFileName == value)
  return;
 TestsDescriptionFileName=value;
}

/// Признак требования завершить работу приложения после тестирования
bool UApplication::IsCloseAfterTest(void) const
{
 return CloseAfterTest;
}

/// Приложение инициализированно
bool UApplication::IsInit(void) const
{
 return AppIsInit;
}

/// Фиксированный путь до логов
const std::string& UApplication::GetFixedLogPath(void) const
{
 return FixedLogPath;
}

bool UApplication::SetFixedLogPath(const std::string& value)
{
 if(FixedLogPath == value)
  return true;

 FixedLogPath=value;
 UpdateLoggers();
 return true;
}

/// Режим записи логов
/// 0 - запись по умолчанию (логи создаются заново при каждом вызове Reset в папке конфигурации)
/// 1 - файл лога создается заново только при открытии каждой новой конфигурации. В папке конфигурации
/// 2 - файл лога создается заново только при открытии каждой новой конфигурации. В системной папке
/// 3 - файл лога создается единожды на весь период работы приложения в системной папке
int UApplication::GetLogCreationMode(void) const
{
 return LogCreationMode;
}

bool UApplication::SetLogCreationMode(int mode)
{
 if(LogCreationMode == mode)
  return true;

 if(mode<0 || mode>3)
  return false;

 LogCreationMode=mode;
 return true;
}

/// Уровень сообщения в логгере при появлении которого осуществляется автоматический останов расчета
int UApplication::GetCalcStopLogLevel(void) const
{
 return CalcStopLogLevel;
}

bool UApplication::SetCalcStopLogLevel(int log_level)
{
    if(CalcStopLogLevel == log_level)
     return true;

    if(CalcStopLogLevel<-1)
     return false;

    CalcStopLogLevel=log_level;
    return true;
}

/// Включение вывода сообщений в cout
bool UApplication::GetCoutLogMode(void) const
{
 return CoutLogMode;
}

bool UApplication::SetCoutLogMode(bool value)
{
 if(CoutLogMode == value)
  return true;
 CoutLogMode=value;
#ifdef RDK_USE_GLOG
 FLAGS_alsologtostderr = CoutLogMode ? 1 : 0;
#endif
 return true;
}


/// Установка необходимого режима сборки
void UApplication::SetStorageBuildMode(int mode)
{
 // пересборка не нужна
 if(StorageBuildMode == mode)
     return;

 StorageBuildMode = mode;
 CloseProject();
 RDK::GetCoreLock()->SetStorageBuildMode(StorageBuildMode);

 int size = GetNumChannels();

 for(int i = 0; i<size;i++)
 {
     MCore_ChannelInit(i,0,(void*)ExceptionHandler);
 }
}

/// Получение текущего режима сборки
int UApplication::GetStorageBuildMode()
{
    return StorageBuildMode;
}

/// Установка callback для обновления прогресса инициализации
void UApplication::SetProgressBarCallback(ProgressBarCallback callback)
{
    FuncProgressBarCallback = callback;
}

/// Получение callback для обновления прогресса инициализации
ProgressBarCallback UApplication::GetProgressBarCallback() const
{
    return FuncProgressBarCallback;
}
// --------------------------
/// Создание библиотек-заглушек из статических библиотек с сохранением файлов
void UApplication::CreateSaveMockLibs()
{
    RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
    if(!storage->CreateMockLibs())
        return;
    storage->SaveMockLibs();
}
// --------------------------
// Методы инициализации
// --------------------------
/// Предоставляет доступ к диспетчеру команд
/*
UEPtr<URpcDispatcher> UApplication::GetRpcDispatcher(void)
{
 return RpcDispatcher;
}

/// Устанавливает новый диспетчер команд
/// Ответственность за освобождение памяти диспетчера лежит на вызывающей стороне
bool UApplication::SetRpcDispatcher(const UEPtr<URpcDispatcher> &value)
{
 if(RpcDispatcher == value)
  return true;

 if(RpcDispatcher)
  RpcDispatcher->SetApplication(0);
 RpcDispatcher=value;
 RpcDispatcher->SetApplication(this);
 return true;
}
*/
/// Предоставляет доступ к контроллеру движка
UEPtr<UEngineControl> UApplication::GetEngineControl(void)
{
 return EngineControl;
}

/// Устанавливает новый контроллер движка
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
bool UApplication::SetEngineControl(const UEPtr<UEngineControl> &value)
{
 if(EngineControl == value)
  return true;

 if(EngineControl)
 {
  EngineControl->PauseChannel(-1);
  EngineControl->SetApplication(0);
 }

 EngineControl=value;
 EngineControl->SetApplication(this);
 return true;
}

/// Предоставляет доступ к проекту
/*UEPtr<UProject> UApplication::GetProject(void)
{
 return Project;
} */

/// Устанавливает новый проект
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
bool UApplication::SetProject(const UEPtr<UProject> &value)
{
 if(Project == value)
  return true;

 // TODO: Здесь какие-то завершающие действия со старым проектом.
 Project=value;
 return true;
}

/// Возвращает конфигурацию проекта
const TProjectConfig& UApplication::GetProjectConfig(void) const
{
 return Project->GetConfig();
}

/// Устанавливает новую конфигурацию проекта
bool UApplication::SetProjectConfig(const TProjectConfig& value)
{
 if(!Project)
  return false;
 if(!Project->SetConfig(value))
  return false;
 MirrorLogsToWorkDirFlag=value.EventsLogFlag;
 UpdateLoggers();
 return true;
}

/// Предоставляет доступ к контроллеру серверной части
UEPtr<UServerControl> UApplication::GetServerControl(void) const
{
 return ServerControl;
}

/// Устанавливает новый контроллер сервера
/// Ответственность за освобождение памяти контроллера лежит на вызывающей стороне
bool UApplication::SetServerControl(const UEPtr<UServerControl> &value)
{
 if(ServerControl == value)
  return true;

 // TODO: Здесь какие-то завершающие действия со старым сервером
 if(ServerControl)
  ServerControl->SetApplication(0);
 ServerControl=value;
 ServerControl->SetApplication(this);
 return true;
}

/// Менеджер тестов
/// Ответственность за освобождение памяти менеджера лежит на вызывающей стороне
UEPtr<UTestManager> UApplication::GetTestManager(void)
{
 return TestManager;
}

bool UApplication::SetTestManager(const UEPtr<UTestManager> &value)
{
 if(TestManager == value)
  return true;

 if(TestManager)
  TestManager->SetApplication(0);

 TestManager=value;
 TestManager->SetApplication(this);
 return true;
}

/// Деплоер проекта (под кончретную задачу)
UEPtr<UProjectDeployer> UApplication::GetProjectDeployer(void)
{
 return ProjectDeployer;
}

 bool UApplication::SetProjectDeployer(const UEPtr<UProjectDeployer> &value)
{
     if(ProjectDeployer == value)
      return true;

     if(ProjectDeployer)
      ProjectDeployer->SetApplication(0);

     ProjectDeployer=value;
     ProjectDeployer->SetApplication(this);

     return true;
}

const std::list<StandartXMLInCatalog>&  UApplication::GetStandartXMLInCatalog(void) const
{
    return xmlInCatalog;
}

bool UApplication::SetStandartXMLInCatalog(void)
{
    //std::string path = "D:/VideoAnalytics/Rtv-VideoAnalytics/Bin/Models/";
    std::string path=GetWorkDirectory() + GetModelsMainPath();
    std::string mask = "*.xml";
    std::vector<std::string> results;
    //int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
    int a = FindFilesList(path, mask, true, results);
    for (size_t i=0; i< results.size(); i++)
    {
        StandartXMLInCatalog newXMLType;
        RDK::USerStorageXML XmlStorage;
        std::string tmp=results[i];
        newXMLType.XMLName=tmp;

        tmp = path + tmp;
        XmlStorage.LoadFromFile(tmp,"Save");
        std::string deskr= XmlStorage.GetNodeAttribute("ModelDescription");
        std::string name= XmlStorage.GetNodeAttribute("ModelName");

        newXMLType.XMLDescription=deskr;
        xmlInCatalog.push_back(newXMLType);
    }

    if (a>0)
        return false;
    return true;
}

/// Инициализирует приложение
bool UApplication::Init(void)
{
 std::string font_path=extract_file_path(ApplicationFileName);
 Core_SetSystemDir(font_path.c_str());
 SetWorkDirectory(font_path);
 CurrentLogSessionStart = std::time(nullptr);
 // Инициализируем кэш имени приложения для логов
 GetLogFileBaseName();
 std::string work_log_dir = EnsureDirectoryAndNormalize(GetWorkLogDir());
 if(!work_log_dir.empty())
 {
  UGlogGuiSink::Instance().StartSession(work_log_dir, GetLogFileBaseName(), CurrentLogSessionStart);
 }
#ifdef RDK_USE_GLOG
 std::string initial_log_dir = work_log_dir;
 FLAGS_log_prefix = true;
 if(initial_log_dir.empty())
 {
  // Если каталог для файловых логов создать не удалось,
  // включаем вывод только в stderr и не пытаемся создавать файлы.
  FLAGS_log_dir.clear();
  FLAGS_logtostderr = true;
  FLAGS_alsologtostderr = false;
 }
 else
 {
  FLAGS_log_dir = StripTrailingSeparators(initial_log_dir);
  FLAGS_logtostderr = false;
  FLAGS_alsologtostderr = false;
 }

google::InitGoogleLogging(GetLogFileBaseName().c_str());
GoogleLoggingInitialized = true;

 if(!initial_log_dir.empty())
  UGlogGuiSink::Instance().AddDirectory(initial_log_dir);

 // Set log level based on DebugMode
 // DebugMode включен по умолчанию для записи всех сообщений в лог до загрузки конфига
 if(GetLogger() && GetLogger()->GetDebugMode())
 {
  FLAGS_minloglevel = google::GLOG_INFO;
  FLAGS_v = 1; // Enable VLOG(1) for debug messages
 }
 else
 {
  // Если DebugMode отключен, все равно включаем INFO уровень для начальной диагностики
  FLAGS_minloglevel = google::GLOG_INFO;
  FLAGS_v = 1; // Enable VLOG(1) for debug messages
 }

// Install failure signal handler
google::InstallFailureSignalHandler();
#endif

 LoggingInitialized=true;
 UpdateLoggers();

 RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Application initialization has been started.");
 RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", std::string("Version: ") + GetCoreVersion().ToStringFull());
 Core_SetBufObjectsMode(1);

// SetLogDir(font_path);
 MLog_SetExceptionHandler(RDK_GLOB_MESSAGE,(void*)ExceptionHandler);
 MLog_SetExceptionHandler(RDK_SYS_MESSAGE,(void*)ExceptionHandler);
 Core_LoadFonts();

 // Устанавливаем флаг режима инициализации перед инициализацией движка
 // чтобы предотвратить фатальные краши при обработке исключений
 RDK::UExceptionLogger::SetInitializationMode(true);

 EngineControl->Init();
 RDK::GetCoreLock()->SetLibrariesPath(LibrariesPath);
 RDK::GetCoreLock()->SetClDescPath(ClDescPath);

 UApplication::SetNumChannels(1);

 // Передаем callback в Storage для обновления прогресса инициализации
 // Делаем это после SetNumChannels, когда Storage уже создан
 if(FuncProgressBarCallback)
 {
  try
  {
   RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
   if(storage)
    storage->SetProgressBarCallback(FuncProgressBarCallback);
  }
  catch(...)
  {
   // Игнорируем ошибки при установке callback
  }
 }

 // Сбрасываем флаг режима инициализации после инициализации каналов
 RDK::UExceptionLogger::SetInitializationMode(false);
// MCore_ChannelInit(0,0,(void*)ExceptionHandler);

 LoadProjectsHistory();
 RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Application initialization has been finished.");

 /*if(CommandLineArgs.size()<2)
  RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Command line parameters not found.");
 else
 {
  RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", std::string("Parsing command line parameters: ") + concat_strings(CommandLineArgs, std::string(" ")));
  ProcessCommandLineArgs();
  RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Finished parsing command line parameters");
 }*/
 SetStandartXMLInCatalog();

 AppIsInit = true;
 return true;
}

/// Деинициализирует приложение
bool UApplication::UnInit(void)
{
 if(!AppIsInit)
  return true;

 RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Application uninitialization has been started.");
 if(EngineControl)
 {
  EngineControl->PauseChannel(-1);
  EngineControl->StopEngineStateThread();
 }
 Sleep(10);
 CloseProject();
 EngineControl->UnInit();
 GetCoreLock()->Destroy();

 RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Application uninitialization has been finished.");
 if(JsonSinkHandle)
 {
  Logging::UnregisterLogSink(JsonSinkHandle);
  JsonSinkHandle.reset();
  ActiveJsonSinkPath.clear();
 }
 if(ProjectLogMirrorHandle)
 {
  Logging::UnregisterLogSink(ProjectLogMirrorHandle);
  ProjectLogMirrorHandle.reset();
 }
 UGlogMirrorSink::Instance().Disable();
#ifdef RDK_USE_GLOG
if (GoogleLoggingInitialized)
{
  google::ShutdownGoogleLogging();
  GoogleLoggingInitialized = false;
}
#endif
 LoggingInitialized=false;
 AppIsInit = false;
 return true;
}


/// Проводит тестирование приложения, если менеджер тестов инициализирован и
/// тестовый режим включен
/// Возвращает код ошибки тестирования.
/// Если exit_request == true,
/// то по завершении метода приложение должно быть закрыто с возвращенным кодом ошибки
int UApplication::Test(bool &exit_request)
{
 exit_request=false;
 int test_result_code(0);
 if(IsTestMode())
 {
  exit_request=CloseAfterTest;
  if(TestManager)
  {
   if(TestManager->LoadTests(TestsDescriptionFileName) != RDK_SUCCESS)
   {
	RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Failed to load tests!");
	test_result_code=1000;
	ChangeTestModeState(false);
	return test_result_code;
   }

   RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Testing started");
   test_result_code=TestManager->ProcessTests();
   RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", std::string("Testing finished with code: ") + sntoa(test_result_code));
  }
 }
 ChangeTestModeState(false);
 return test_result_code;
}

/// Осуществляет парсинг командной строки и соответствующую настройку приложение
void UApplication::ProcessCommandLineArgs(std::vector<std::string> commandLineArgs)
{
  InitCmdParser();
  if(commandLineArgs.empty())
    return;

  ApplyCliLogOverrides(commandLineArgs);

  std::vector<std::string>::iterator I=find(commandLineArgs.begin(),commandLineArgs.end(),"--test");
  if(I != commandLineArgs.end())
  {
    ++I;
    std::string configuration_name;
    if(I != commandLineArgs.end())
    {
      configuration_name = *I;
      SetTestsDescriptionFileName(configuration_name);
      ChangeTestModeState(true);
    }
    else
      ChangeTestModeState(false);
  }
  else
  {
    ChangeTestModeState(false);
  }

  I=find(commandLineArgs.begin(),commandLineArgs.end(),"--run");
  if(I != commandLineArgs.end())
    CloseAfterTest=true;
  else
    CloseAfterTest=false;

  UpdateLoggers();
}

#ifndef __BORLANDC__
/// Осуществляет парсинг командной строки и записывает результаты в CommandLineArgs
void UApplication::ProcessCommandLineArgs(int argc, char **argv)
{
  InitCmdParser();

  try
  {
   po::store(po::parse_command_line(argc, argv, CmdLineDescription), CmdVariablesMap);
   po::notify(CmdVariablesMap);
  }
  catch(po::unknown_option &ex)
  {
   RLOG(RDK_EX_WARNING, RDK_GLOB_MESSAGE, "glob", ex.what());
   throw ex;
   return;
  }

  if(CmdVariablesMap.count("test"))
  {
	ChangeTestModeState(true);
	SetTestsDescriptionFileName(CmdVariablesMap["test"].as<std::string>());
  }
  else
   ChangeTestModeState(false);

  if(CmdVariablesMap.count("run"))
	CloseAfterTest=false;
  else
	CloseAfterTest=true;

  ApplyCliLogOverrides(CmdVariablesMap);
  UpdateLoggers();
}
#endif
// --------------------------

// --------------------------
// Методы управления проектом
// --------------------------

std::string UApplication::GetDefaultConfigsDirectory() const
{
 std::string configs_path = GetWorkDirectory() + "/../../Configs/";
 std::error_code ec;
 if(!std::filesystem::exists(configs_path, ec))
 {
  configs_path = GetWorkDirectory() + "/../../../Configs/";
  if(!std::filesystem::exists(configs_path, ec))
   configs_path = GetWorkDirectory();
 }
 if(!GetUserName().empty())
 {
  const std::string user_rel = GetUserConfigPath();
  if(!user_rel.empty())
  {
   const std::string users_dir = configs_path + "Users";
   CreateNewDirectory(users_dir.c_str());
   const std::string user_path = configs_path + user_rel;
   CreateNewDirectory(user_path.c_str());
   configs_path = user_path;
  }
 }
 return normalizeProjectDirPath(configs_path);
}

std::string UApplication::PrepareNewProjectIniPath(bool autocreate_subdirectory,
                                                   const std::string& parent_directory,
                                                   std::string* err_out) const
{
 auto set_err = [&](const std::string& msg) {
  if(err_out)
   *err_out = msg;
 };
 set_err({});

 std::string parent = parent_directory;
 if(parent.empty())
  parent = GetDefaultConfigsDirectory();
 if(parent.empty())
 {
  set_err("configs directory is not available");
  return {};
 }

 if(autocreate_subdirectory)
 {
  const std::time_t now = std::time(nullptr);
  const std::string folder = RDK::get_text_time(now, '.', '_');
  parent = joinProjectPath(parent, "Autocreate" + folder);
  if(CreateNewDirectory(parent.c_str()) != 0)
  {
   set_err("failed to create configuration directory");
   return {};
  }
  parent = canonicalFilesystemPath(parent);
 }

 const std::string ini_path = joinProjectPath(parent, "project.ini");
 return canonicalFilesystemPath(ini_path);
}

bool UApplication::CreateAutocreatedProject(const std::string& model_classname,
                                          bool autocreate_subdirectory,
                                          const std::string& parent_directory)
{
 std::string err;
 const std::string ini_path =
  PrepareNewProjectIniPath(autocreate_subdirectory, parent_directory, &err);
 if(ini_path.empty())
  return false;
 return CreateProject(ini_path, model_classname);
}

/// Создает проект (через сохранение и открытие)
bool UApplication::CreateProject(const std::string &file_name, RDK::TProjectConfig &project_config)
{
 CloseProject();

 UApplication::SetNumChannels(project_config.NumChannels);

 for(int i=0;i<project_config.NumChannels;i++)
 {
  RDK::TProjectChannelConfig &channel=project_config.ChannelsConfig[i];

  if(!MCore_IsChannelInit(i))
   MCore_ChannelInit(i,channel.PredefinedStructure,(void*)ExceptionHandler);
  else
   MEnv_SetPredefinedStructure(i,channel.PredefinedStructure);

  if(channel.PredefinedStructure == 0 && !channel.ClassName.empty())
  {
   MModel_Create(i,channel.ClassName.c_str());
  }

  if(channel.PredefinedStructure == 0 && channel.ClassName.empty()
     && !channel.ModelFileName.empty())
  {
   std::string modelXml;
   if(LoadFile(channel.ModelFileName, modelXml))
   {
	 MModel_LoadComponent(i, "", modelXml.c_str());
	 if(Project->GetForceNewConfigFilesStructure())
	  channel.ModelFileName = std::string("Model_")+RDK::sntoa(i,2)+".xml";
	 else
	  channel.ModelFileName = (i == 0) ? std::string("model.xml") : std::string("model_")+RDK::sntoa(i)+".xml";
   }
  }
 }

 SetProjectPath(extract_file_path(file_name));
 MirrorLogsToWorkDirFlag=project_config.EventsLogFlag;
 SetProjectOpenFlag(true);
 Project->SetConfig(project_config);
 Project->SetForceNewConfigFilesStructure(true);
 Project->SetProjectPath(ProjectPath);
 ProjectFileName=extract_file_name(file_name);

 if(SaveProject())
 {
  fstream file((ProjectPath+project_config.DescriptionFileName).c_str(),ios::out | ios::trunc);
  file<<project_config.ProjectDescription;
  file.flush();
  file.close();
  return OpenProject(ProjectPath+ProjectFileName);
 }
 else
  return false;
}

bool UApplication::CreateProject(const std::string &file_name, const std::string &model_classname)
{
 RDK::TProjectConfig project_config;

 project_config.DebugMode=false;
 project_config.SetNumChannels(1);
 project_config.ProjectMode=0;
 project_config.ProjectName="Autocreated configuration";
 project_config.UserId = UserId;
 project_config.UserName = UserName;
 time_t time_data;
 time(&time_data);
 project_config.CreationTime = RDK::get_text_time(time_data, '.', '_');
 project_config.ProjectType=0;
 project_config.EventsLogFlag=true;
 project_config.CalcSourceTimeMode=0;
 project_config.MultiThreadingMode=1;
 project_config.MTUpdateInterfaceInterval=100;
 project_config.ChannelsConfig[0].CalculationMode=1;
 project_config.ChannelsConfig[0].ClassName=model_classname;
 project_config.ChannelsConfig[0].ModelMode=3;
 project_config.ChannelsConfig[0].InitAfterLoad=true;
 project_config.ChannelsConfig[0].GlobalTimeStep=2000;
 project_config.ChannelsConfig[0].ResetAfterLoad=true;
// project_config.ChannelsConfig[0].CalculationMode
 project_config.ChannelsConfig[0].DefaultTimeStep=2000;
 project_config.ChannelsConfig[0].MinInterstepsInterval=1;
 project_config.ChannelsConfig[0].MaxCalculationModelTime=0;
 project_config.ChannelsConfig[0].DebugMode=false;

 return CreateProject(file_name,project_config);
}

/// Обновляет проект по новой конфигурации
bool UApplication::UpdateProject(RDK::TProjectConfig &project_config)
{
 if(!ProjectOpenFlag)
  return false;

 // Если необходима перезагрузка конфигурации, то выполняем перезагрузку
 bool is_reload_needed(false);

 const TProjectConfig old_project_config=Project->GetConfig();
 Project->SetConfig(project_config);

 // Первый проход. Определяем необходима ли перезагрузка конфигурации

 if(old_project_config.ProjectName != project_config.ProjectName)
 {
 }

 if(old_project_config.ProjectDescription != project_config.ProjectDescription)
 {
 }

 if(old_project_config.DescriptionFileName != project_config.DescriptionFileName)
 {
 }

 if(old_project_config.ProjectAutoSaveFlag != project_config.ProjectAutoSaveFlag)
 {
 }

 if(old_project_config.ProjectAutoSaveStatesFlag != project_config.ProjectAutoSaveStatesFlag)
 {
 }

 if(old_project_config.EventsLogFlag != project_config.EventsLogFlag)
 {
  MirrorLogsToWorkDirFlag=project_config.EventsLogFlag;
  UpdateLoggers();
 }

 if(old_project_config.ProjectMode != project_config.ProjectMode)
 {
  is_reload_needed=true;
 }

 if(old_project_config.ProjectType != project_config.ProjectType)
 {
  is_reload_needed=true;
 }

 if(old_project_config.MultiThreadingMode != project_config.MultiThreadingMode)
 {
  is_reload_needed=true;
 }

 if(old_project_config.CalcSourceTimeMode != project_config.CalcSourceTimeMode)
 {

 }

 if(old_project_config.MTUpdateInterfaceInterval != project_config.MTUpdateInterfaceInterval)
 {

 }

 if(old_project_config.GuiUpdateMode != project_config.GuiUpdateMode)
 {

 }

 if(old_project_config.ShowChannelsStateFlag != project_config.ShowChannelsStateFlag)
 {

 }

 if(old_project_config.ReflectionFlag != project_config.ReflectionFlag)
 {

 }

 if(old_project_config.DisableStopVideoSources != project_config.DisableStopVideoSources)
 {

 }

 if(old_project_config.DebugMode != project_config.DebugMode)
 {

 }

 if(old_project_config.DebugSysEventsMask != project_config.DebugSysEventsMask)
 {

 }

 if(old_project_config.DebuggerMessageFlag != project_config.DebuggerMessageFlag)
 {

 }

 if(old_project_config.EventsLogMode != project_config.EventsLogMode)
 {
  is_reload_needed=true;
 }

 if(old_project_config.OverrideLogParameters != project_config.OverrideLogParameters)
 {
  is_reload_needed=true;
 }

 if(old_project_config.ServerInterfaceAddress != project_config.ServerInterfaceAddress)
 {

 }

 if(old_project_config.ServerInterfacePort != project_config.ServerInterfacePort)
 {

 }

 if(old_project_config.ProjectShowChannelsStates != project_config.ProjectShowChannelsStates)
 {

 }

 if(old_project_config.InterfaceFileName != project_config.InterfaceFileName)
 {
  is_reload_needed=true;
 }

 if(old_project_config.NumChannels != project_config.NumChannels)
 {
  is_reload_needed=true;
 }

 int min_num_channels=(project_config.NumChannels<old_project_config.NumChannels)?project_config.NumChannels:old_project_config.NumChannels;
 for(int i=0;i<min_num_channels;i++)
 {
  if(old_project_config.ChannelsConfig[i].ModelMode != old_project_config.ChannelsConfig[i].ModelMode)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].PredefinedStructure != old_project_config.ChannelsConfig[i].PredefinedStructure)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ModelFileName != old_project_config.ChannelsConfig[i].ModelFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ParametersFileName != old_project_config.ChannelsConfig[i].ParametersFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].StatesFileName != old_project_config.ChannelsConfig[i].StatesFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ClassName != old_project_config.ChannelsConfig[i].ClassName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].GlobalTimeStep != old_project_config.ChannelsConfig[i].GlobalTimeStep)
  {
  }

  if(old_project_config.ChannelsConfig[i].DefaultTimeStep != old_project_config.ChannelsConfig[i].DefaultTimeStep)
  {
  }

  if(old_project_config.ChannelsConfig[i].CalculationMode != old_project_config.ChannelsConfig[i].CalculationMode)
  {
  }

  if(old_project_config.ChannelsConfig[i].MinInterstepsInterval != old_project_config.ChannelsConfig[i].MinInterstepsInterval)
  {
  }

  if(old_project_config.ChannelsConfig[i].InitAfterLoad != old_project_config.ChannelsConfig[i].InitAfterLoad)
  {
  }

  if(old_project_config.ChannelsConfig[i].ResetAfterLoad != old_project_config.ChannelsConfig[i].ResetAfterLoad)
  {
  }

  if(old_project_config.ChannelsConfig[i].DebugMode != old_project_config.ChannelsConfig[i].DebugMode)
  {
  }

  if(old_project_config.ChannelsConfig[i].DebugSysEventsMask != old_project_config.ChannelsConfig[i].DebugSysEventsMask)
  {
  }

  if(old_project_config.ChannelsConfig[i].DebuggerMessageFlag != old_project_config.ChannelsConfig[i].DebuggerMessageFlag)
  {
  }

  if(old_project_config.ChannelsConfig[i].EventsLogMode != old_project_config.ChannelsConfig[i].EventsLogMode)
  {
  }

  if(old_project_config.ChannelsConfig[i].ChannelName != old_project_config.ChannelsConfig[i].ChannelName)
  {
  }

  if(old_project_config.ChannelsConfig[i].MaxCalculationModelTime != old_project_config.ChannelsConfig[i].MaxCalculationModelTime)
  {
  }

 }

 // Если необходима перезагрузка конфигурации то выполняем
 if(is_reload_needed)
 {
  if(!SaveProject())
   return false;

  if(!OpenProject(GetProjectPath()+GetProjectFileName()))
   return false;

  return true;
 }

 // ... иначе применяем отдельные настройки
 if(old_project_config.ProjectName != project_config.ProjectName)
 {
 }

 if(old_project_config.ProjectDescription != project_config.ProjectDescription)
 {
 }

 if(old_project_config.DescriptionFileName != project_config.DescriptionFileName)
 {
 }

 if(old_project_config.ProjectAutoSaveFlag != project_config.ProjectAutoSaveFlag)
 {
 }

 if(old_project_config.ProjectAutoSaveStatesFlag != project_config.ProjectAutoSaveStatesFlag)
 {
 }

 if(old_project_config.ProjectMode != project_config.ProjectMode)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 if(old_project_config.ProjectType != project_config.ProjectType)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 if(old_project_config.MultiThreadingMode != project_config.MultiThreadingMode)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 if(old_project_config.CalcSourceTimeMode != project_config.CalcSourceTimeMode)
 {
  for(int i=0;i<project_config.NumChannels;i++)
  {
   EngineControl->SetCalculationTimeSource(i, project_config.CalcSourceTimeMode);
  }
 }

 if(old_project_config.MTUpdateInterfaceInterval != project_config.MTUpdateInterfaceInterval)
 {

 }

 if(old_project_config.GuiUpdateMode != project_config.GuiUpdateMode)
 {
  EngineControl->SetGuiUpdateMode(project_config.GuiUpdateMode);
 }

 if(old_project_config.ShowChannelsStateFlag != project_config.ShowChannelsStateFlag)
 {

 }

 if(old_project_config.ReflectionFlag != project_config.ReflectionFlag)
 {

 }

 if(old_project_config.DisableStopVideoSources != project_config.DisableStopVideoSources)
 {

 }

 if(old_project_config.DebugMode != project_config.DebugMode)
 {
  for(int i=0;i<project_config.NumChannels;i++)
  {
   MLog_SetDebugMode(i,project_config.DebugMode);
  }
 }

 if(old_project_config.DebugSysEventsMask != project_config.DebugSysEventsMask)
 {
  for(int i=0;i<project_config.NumChannels;i++)
  {
   MLog_SetDebugSysEventsMask(i,project_config.DebugSysEventsMask);
  }
 }

 if(old_project_config.DebuggerMessageFlag != project_config.DebuggerMessageFlag)
 {
  for(int i=0;i<project_config.NumChannels;i++)
  {
   MLog_SetDebuggerMessageFlag(i,project_config.DebuggerMessageFlag);
  }
 }

 if(old_project_config.EventsLogMode != project_config.EventsLogMode)
 {
  is_reload_needed=true;
 }

 if(old_project_config.OverrideLogParameters != project_config.OverrideLogParameters)
 {
  is_reload_needed=true;
 }

 if(old_project_config.ServerInterfaceAddress != project_config.ServerInterfaceAddress)
 {

 }

 if(old_project_config.ServerInterfacePort != project_config.ServerInterfacePort)
 {

 }

  if(old_project_config.HttpServerInterfaceAddress != project_config.HttpServerInterfaceAddress)
 {

 }

 if(old_project_config.HttpServerInterfacePort != project_config.HttpServerInterfacePort)
 {

 }


  if(old_project_config.HttpServerLogin != project_config.HttpServerLogin)
 {

 }

 if(old_project_config.HttpServerPassword != project_config.HttpServerPassword)
 {

 }


 if(old_project_config.ProjectShowChannelsStates != project_config.ProjectShowChannelsStates)
 {

 }

 if(old_project_config.InterfaceFileName != project_config.InterfaceFileName)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 if(old_project_config.NumChannels != project_config.NumChannels)
 {
  // нет действий - приводит к повторному открытию конфигурации ранее
 }

 // обработка каналов
 for(int i=0;i<project_config.NumChannels;i++)
 {
  if(old_project_config.ChannelsConfig[i].ModelMode != project_config.ChannelsConfig[i].ModelMode)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].PredefinedStructure != project_config.ChannelsConfig[i].PredefinedStructure)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].ModelFileName != project_config.ChannelsConfig[i].ModelFileName)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].ParametersFileName != project_config.ChannelsConfig[i].ParametersFileName)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].StatesFileName != project_config.ChannelsConfig[i].StatesFileName)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].ClassName != project_config.ChannelsConfig[i].ClassName)
  {
   // нет действий - приводит к повторному открытию конфигурации ранее
  }

  if(old_project_config.ChannelsConfig[i].GlobalTimeStep != project_config.ChannelsConfig[i].GlobalTimeStep)
  {
   if(MModel_Check(i))
   {
    MModel_SetGlobalTimeStep(i,"",project_config.ChannelsConfig[i].GlobalTimeStep);
   }
  }

  if(old_project_config.ChannelsConfig[i].DefaultTimeStep != project_config.ChannelsConfig[i].DefaultTimeStep)
  {
   if(MModel_Check(i))
   {
    MModel_SetDefaultTimeStep(i,project_config.ChannelsConfig[i].DefaultTimeStep);
   }
  }

  if(old_project_config.ChannelsConfig[i].CalculationMode != project_config.ChannelsConfig[i].CalculationMode)
  {
   EngineControl->SetCalculateMode(i, project_config.ChannelsConfig[i].CalculationMode);
  }

  if(old_project_config.ChannelsConfig[i].MinInterstepsInterval != project_config.ChannelsConfig[i].MinInterstepsInterval)
  {
   EngineControl->SetMinInterstepsInterval(i,project_config.ChannelsConfig[i].MinInterstepsInterval);
  }

  if(old_project_config.ChannelsConfig[i].InitAfterLoad != project_config.ChannelsConfig[i].InitAfterLoad)
  {
  }

  if(old_project_config.ChannelsConfig[i].ResetAfterLoad != project_config.ChannelsConfig[i].ResetAfterLoad)
  {
  }

  if(old_project_config.ChannelsConfig[i].DebugMode != project_config.ChannelsConfig[i].DebugMode)
  {
   MLog_SetDebugMode(i,project_config.ChannelsConfig[i].DebugMode);
  }

  if(old_project_config.ChannelsConfig[i].DebugSysEventsMask != project_config.ChannelsConfig[i].DebugSysEventsMask)
  {
   MLog_SetDebugSysEventsMask(i,project_config.ChannelsConfig[i].DebugSysEventsMask);
  }

  if(old_project_config.ChannelsConfig[i].DebuggerMessageFlag != project_config.ChannelsConfig[i].DebuggerMessageFlag)
  {
   MLog_SetDebuggerMessageFlag(i,project_config.ChannelsConfig[i].DebuggerMessageFlag);
  }

  if(old_project_config.ChannelsConfig[i].ChannelName != project_config.ChannelsConfig[i].ChannelName)
  {
  }

  if(old_project_config.ChannelsConfig[i].MaxCalculationModelTime != project_config.ChannelsConfig[i].MaxCalculationModelTime)
  {
   GetEnvironmentLock()->SetMaxCalcTime(project_config.ChannelsConfig[i].MaxCalculationModelTime);
  }

  if(old_project_config.ChannelsConfig[i].UseIndTimeStepFlag != project_config.ChannelsConfig[i].UseIndTimeStepFlag)
  {
   GetEnvironmentLock()->SetUseIndTimeStepFlag(project_config.ChannelsConfig[i].UseIndTimeStepFlag);
  }

 }

 return SaveProject();
}

/// Открывает проект с диагностикой
bool UApplication::OpenProject(const std::string &filename, TProjectLoadDiagnostics* diagnostics)
{
 CloseProject();

 if(diagnostics)
 {
  *diagnostics = TProjectLoadDiagnostics();
 }

 bool is_loaded(false);
 if(!ProjectXml.LoadFromFile(filename,""))
 {
  std::string error_msg = std::string("Can't read project file ") + filename;
  RLOG(RDK_EX_WARNING, RDK_SYS_MESSAGE, "sys", error_msg);
  if(diagnostics)
  {
   diagnostics->errors.push_back(error_msg);
   diagnostics->isValid = false;
  }
  return false;
 }

RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", std::string("Open configuration ") + filename + "...");
 SetProjectPath(extract_file_path(filename));
 ProjectFileName=extract_file_name(filename);
 Project->SetProjectPath(ProjectPath);
 Project->ReadFromXml(ProjectXml);
 if(LogCreationMode != 3)
  UpdateLoggers();

 TProjectConfig config=Project->GetConfig();
// EngineControl->GetEngineStateThread()->SetLogFlag(config.EventsLogFlag);
// EngineControl->GetEngineStateThread()->CloseEventsLogFile();
// EngineControl->GetEngineStateThread()->SetLogDir(ProjectPath);

 EngineControl->SetThreadMode(config.MultiThreadingMode);
 CalcAppCaption();

 if(diagnostics)
 {
  diagnostics->channelsTotal = config.NumChannels;
 }

try{

 EngineControl->SetNumChannels(config.NumChannels);

 ProjectXml.SelectNodeRoot("Project/MultiGeneral");
 int selected_channel_index=ProjectXml.ReadInteger("SelectedChannelIndex",0);

 ProjectXml.SelectNodeRoot("Project/General");

 // Приоритет: сначала README.md, затем Description.rtf (для обратной совместимости)
 std::string description_file_path;
 bool description_loaded = false;

 // Пытаемся загрузить README.md
 std::string readme_path = ProjectPath + "README.md";
 if(LoadFile(readme_path, config.ProjectDescription))
 {
  config.DescriptionFileName = "README.md";
  description_file_path = readme_path;
  description_loaded = true;
 }
 else
 {
  // Fallback на Description.rtf
  std::string rtf_path;
  if(!config.DescriptionFileName.empty())
  {
   rtf_path = ProjectPath + config.DescriptionFileName;
  }
  else
  {
   rtf_path = ProjectPath + "Description.rtf";
   config.DescriptionFileName = "Description.rtf";
  }

  if(LoadFile(rtf_path, config.ProjectDescription))
  {
   description_file_path = rtf_path;
   description_loaded = true;
   if(diagnostics)
   {
    std::string info_msg = std::string("Using legacy Description.rtf. Consider migrating to README.md");
    diagnostics->warnings.push_back(info_msg);
   }
  }
 }

 if(description_loaded)
 {
  Project->SetConfig(config);
  MirrorLogsToWorkDirFlag=config.EventsLogFlag;
  Project->ResetModified();
 }
 else if(diagnostics)
 {
  std::string warning_msg = std::string("Can't load description file (tried README.md and Description.rtf)");
  diagnostics->warnings.push_back(warning_msg);
 }

 for(int i=0;i<config.NumChannels;i++)
 {
  try
  {
   TProjectChannelConfig &channel_config=config.ChannelsConfig[i];
   EngineControl->SetCalculationTimeSource(i, config.CalcSourceTimeMode);
   EngineControl->SetMinInterstepsInterval(i,channel_config.MinInterstepsInterval);

   Core_SelectChannel(i);
   if(!Core_IsChannelInit())
	Core_ChannelInit(channel_config.PredefinedStructure,(void*)ExceptionHandler);
   else
   {
	Env_SetPredefinedStructure(channel_config.PredefinedStructure);
	RDK_ASSERT_LOG(MEnv_CreateStructure(i));
	RDK_ASSERT_LOG(MEnv_Init(i));
   }

   // TODO: Реалиовать загрузку описаний классов
   // Загрузка описаний классов
   Model_SetDefaultTimeStep(channel_config.DefaultTimeStep);
   Log_SetDebugMode(config.DebugMode);
   Log_SetDebugSysEventsMask(config.DebugSysEventsMask);
   Log_SetDebuggerMessageFlag(config.DebuggerMessageFlag);
   Env_SetCurrentDataDir(ProjectPath.c_str());
   Env_CreateStructure();
   Env_Init();

   if(channel_config.PredefinedStructure == 0 && !channel_config.ModelFileName.empty())
   {
	std::string model_file_path;
	if(extract_file_path(channel_config.ModelFileName).empty())
	{
	 model_file_path = ProjectPath+channel_config.ModelFileName;
	 is_loaded=LoadModelFromFile(i,model_file_path, diagnostics);
	}
	else
	{
	 model_file_path = channel_config.ModelFileName;
	 is_loaded=LoadModelFromFile(i,model_file_path, diagnostics);
	}

	if(!is_loaded)
	{
	 std::string error_msg = std::string("Channel ") + RDK::sntoa(i) + std::string(": Can't open model file: ") + channel_config.ModelFileName;
	 RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", error_msg);
	 if(diagnostics)
	 {
	  diagnostics->errors.push_back(error_msg);
	  diagnostics->missingFiles.push_back(model_file_path);
	  diagnostics->failedChannels.push_back(i);
	 }
	}
   }


   if(!channel_config.ParametersFileName.empty())
   {
	std::string params_file_path;
	if(extract_file_path(channel_config.ParametersFileName).empty())
	{
	 params_file_path = ProjectPath+channel_config.ParametersFileName;
	 is_loaded=LoadParametersFromFile(i,params_file_path);
	}
	else
	{
	 params_file_path = channel_config.ParametersFileName;
	 is_loaded=LoadParametersFromFile(i,params_file_path);
	}

 	if(!is_loaded)
	{
	 std::string error_msg = std::string("Channel ") + RDK::sntoa(i) + std::string(": Can't open parameters file: ") + channel_config.ParametersFileName;
	 RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", error_msg);
	 if(diagnostics)
	 {
	  diagnostics->errors.push_back(error_msg);
	  diagnostics->missingFiles.push_back(params_file_path);
	 }
	}
   }

   if(config.ProjectAutoSaveStatesFlag)
   {
	if(!channel_config.StatesFileName.empty())
	{
	 std::string states_file_path;
	 if(extract_file_path(channel_config.StatesFileName).empty())
	 {
	  states_file_path = ProjectPath+channel_config.StatesFileName;
	  is_loaded=LoadStatesFromFile(i,states_file_path);
	 }
	 else
	 {
	  states_file_path = channel_config.StatesFileName;
	  is_loaded=LoadStatesFromFile(i,states_file_path);
	 }

	 if(!is_loaded)
	 {
	  std::string error_msg = std::string("Channel ") + RDK::sntoa(i) + std::string(": Can't open states file: ") + channel_config.StatesFileName;
	  RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", error_msg);
	  if(diagnostics)
	  {
	   diagnostics->errors.push_back(error_msg);
	   diagnostics->missingFiles.push_back(states_file_path);
	  }
	 }
	}
   }

   // Проверка модели после загрузки
   bool channel_has_model = Model_Check();
   int channel_components = 0;
   if(channel_has_model)
   {
	channel_components = Model_GetNumComponents("");
	if(diagnostics)
	{
	 diagnostics->componentsCount.push_back(channel_components);
	 if(channel_components == 0)
	 {
	  std::string warning_msg = std::string("Channel ") + RDK::sntoa(i) + std::string(": Model is empty (no components)");
	  diagnostics->warnings.push_back(warning_msg);
	  diagnostics->modelEmpty = true;
	 }
	 else
	 {
	  diagnostics->modelExists = true;
	 }
	}

	if(!channel_config.UseIndTimeStepFlag)
	 Model_SetGlobalTimeStep("",channel_config.GlobalTimeStep);
	if(channel_config.InitAfterLoad)
     MEnv_ModelInit(i,0);
	if(channel_config.ResetAfterLoad)
	 MEnv_Reset(i,0);
   }
   else if(diagnostics)
   {
	diagnostics->componentsCount.push_back(0);
	std::string error_msg = std::string("Channel ") + RDK::sntoa(i) + std::string(": Model does not exist after loading");
	diagnostics->errors.push_back(error_msg);
	diagnostics->failedChannels.push_back(i);
   }

   if(diagnostics && channel_has_model && channel_components > 0)
   {
	diagnostics->channelsLoaded++;
   }

   EngineControl->SetCalculateMode(i, channel_config.CalculationMode);
   GetEnvironmentLock()->SetMaxCalcTime(channel_config.MaxCalculationModelTime);
   GetEnvironmentLock()->SetUseIndTimeStepFlag(channel_config.UseIndTimeStepFlag);
  }
  catch(RDK::UException &exception)
  {
   std::string error_msg = std::string("Channel ") + RDK::sntoa(i) + std::string(": Exception during loading: ") + exception.what();
   RLOG(exception.GetType(), RDK_SYS_MESSAGE, "sys", std::string("Core-OpenProject(Load Channel) Exception: (Name=") + std::string(Name.c_str()) + std::string(") ") + exception.what());
   if(diagnostics)
   {
	diagnostics->errors.push_back(error_msg);
	diagnostics->failedChannels.push_back(i);
   }
  }
  Sleep(0);
 }

 EngineControl->SetGuiUpdateMode(config.GuiUpdateMode);

 if(selected_channel_index>=GetNumChannels())
  selected_channel_index=0;

 Core_SelectChannel(selected_channel_index);
 InterfaceXml.Destroy();

 if(!config.InterfaceFileName.empty())
 {
  std::string interface_file_path;
  if(extract_file_path(config.InterfaceFileName).empty())
  {
   interface_file_path = ProjectPath+config.InterfaceFileName;
   is_loaded=InterfaceXml.LoadFromFile(interface_file_path,"Interfaces");
  }
  else
  {
   interface_file_path = config.InterfaceFileName;
   is_loaded=InterfaceXml.LoadFromFile(interface_file_path,"Interfaces");
  }

  if(!is_loaded)
  {
   std::string error_msg = std::string("Can't open interface file: ") + config.InterfaceFileName;
   RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", error_msg);
   if(diagnostics)
   {
	diagnostics->errors.push_back(error_msg);
	diagnostics->missingFiles.push_back(interface_file_path);
   }
  }

  InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 }
 ServerControl->SetNumChannels(config.NumChannels);

 RDK::UIVisualControllerStorage::LoadParameters(InterfaceXml);

 SetProjectOpenFlag(true);
 EngineControl->StartEngineStateThread();

 RDK::UIVisualControllerStorage::UpdateInterface();
 RDK::UIVisualControllerStorage::AfterLoadProject();
}
catch(RDK::UException &exception)
{
// UShowProgressBarForm->Hide();
 std::string error_msg = std::string("Exception during project loading: ") + exception.what();
 RLOG(exception.GetType(), RDK_SYS_MESSAGE, "sys", std::string("Core-OpenProject Exception: (Name=") + Name + std::string(") ") + exception.what());
 if(diagnostics)
 {
  diagnostics->errors.push_back(error_msg);
  diagnostics->isValid = false;
 }
}

 std::list<std::string> last_list=LastProjectsList;
 last_list.push_front(filename);
 while(int(last_list.size())>LastProjectsListMaxSize
  && !LastProjectsList.empty())
 {
  last_list.pop_back();
 }

 LastProjectsList=last_list;

 SaveProjectsHistory();

 if(diagnostics)
 {
  // Определяем общую валидность
  diagnostics->isValid = (diagnostics->errors.empty() && diagnostics->channelsLoaded > 0);
 }

RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", std::string("Configuration ") + filename + " has been opened.");
 return true;
}

/// Открывает проект (без диагностики)
bool UApplication::OpenProject(const std::string &filename)
{
 return OpenProject(filename, nullptr);
}

/// Сохраняет проект
bool UApplication::SaveProject(void)
{
 if(!ProjectOpenFlag)
  return false;

 int selected_channel_index=Core_GetSelectedChannelIndex();
 bool is_saved(false);

 ProjectXml.Create("Project");
 Project->WriteToXml(ProjectXml);

 HistoryXml.LoadFromFile(ProjectPath+"History.xml", "History");
 FixSavePoint(HistoryXml);
try
{
 InterfaceXml.Create(std::string("Interfaces"));
 InterfaceXml.SelectNodeRoot(std::string("Interfaces"));
 RDK::UIVisualControllerStorage::SaveParameters(InterfaceXml);

 const size_t n_ctrl = RDK::UIVisualControllerStorage::InterfaceUpdaters.size();
 InterfaceXml.SelectRoot();
 const int n_nodes = InterfaceXml.GetNumNodes();
 RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys",
      std::string("Core-SaveProject: Interface controllers=")
          + RDK::sntoa(static_cast<int>(n_ctrl))
          + std::string(" xml_nodes=")
          + RDK::sntoa(n_nodes));

 // Защита: при выходе Qt сначала уничтожает главное окно (DelInterface),
 // затем UAppCore::UnInit → CloseProject → autosave. К этому моменту в
 // InterfaceUpdaters часто остаётся только UCurlFtpClientTestWidget(parent=NULL),
 // и пустой Interface.xml затирает нормальный Save. Не перезаписываем файл.
 const bool skip_interface_file =
     (n_ctrl <= 1) || (n_nodes <= 1);
 if(skip_interface_file)
 {
  RLOG(RDK_EX_WARNING, RDK_SYS_MESSAGE, "sys",
       "Core-SaveProject: skip Interface.xml write — GUI controllers already destroyed "
       "(would wipe Watch/charts). Save while the main window is still alive.");
 }

 TProjectConfig config=Project->GetConfig();
 ProjectXml.SelectNodeRoot("Project/General");

 SaveFileSafe(ProjectPath+config.DescriptionFileName,config.ProjectDescription,"save.tmp",3);

 if(!config.InterfaceFileName.empty() && !skip_interface_file)
 {
  if(extract_file_path(config.InterfaceFileName).empty())
   is_saved=InterfaceXml.SaveToFile(ProjectPath+config.InterfaceFileName);
  else
   is_saved=InterfaceXml.SaveToFile(config.InterfaceFileName);
 }
 else if(config.InterfaceFileName.empty() && !skip_interface_file)
 {
  ProjectXml.WriteString("InterfaceFileName","Interface.xml");
  is_saved=InterfaceXml.SaveToFile(ProjectPath+config.InterfaceFileName);
 }

 if(!skip_interface_file && !is_saved)
 RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("Core-SaveProject: Can't save interface file: ") + config.InterfaceFileName);

 for(int i=0;i<config.NumChannels;i++)
 {
  Core_SelectChannel(i);

  TProjectChannelConfig &channel_config=config.ChannelsConfig[i];
  if(extract_file_path(channel_config.ModelFileName).empty())
   is_saved=SaveModelToFile(i, ProjectPath+channel_config.ModelFileName);
  else
   is_saved=SaveModelToFile(i, channel_config.ModelFileName);

  if(!is_saved)
   RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("Core-SaveProject: Can't save model file: ") + channel_config.ModelFileName);

  if(extract_file_path(channel_config.ParametersFileName).empty())
   is_saved=SaveParametersToFile(i, ProjectPath+channel_config.ParametersFileName);
  else
   is_saved=SaveParametersToFile(i,channel_config.ParametersFileName);

  if(!is_saved)
   RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("Core-SaveProject: Can't save parameters file: ") + channel_config.ParametersFileName);

  channel_config.UseIndTimeStepFlag=GetEnvironmentLock()->GetUseIndTimeStepFlag();

  if(config.ProjectAutoSaveStatesFlag)
  {
   if(extract_file_path(channel_config.StatesFileName).empty())
	is_saved=SaveStatesToFile(i, ProjectPath+channel_config.StatesFileName);
   else
	is_saved=SaveStatesToFile(i, channel_config.StatesFileName);

   if(!is_saved)
    RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("Core-SaveProject: Can't save states file: ") + channel_config.StatesFileName);
  }


  Sleep(0);
 }

 Core_SelectChannel(selected_channel_index);


 is_saved=ProjectXml.SaveToFile(ProjectPath+ProjectFileName);

 if(!is_saved)
  RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("Core-SaveProject: Can't save configuration: ") + ProjectFileName);
 else
 {
  std::string filename=ProjectPath+ProjectFileName;
  RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", std::string("Configuration ") + filename + " has been saved.");
 }

 is_saved=HistoryXml.SaveToFile(ProjectPath+"History.xml");

 if(!is_saved)
  RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("Core-SaveProject: Can't save history file: ") + ProjectFileName);
 else
 {
  std::string filename=ProjectPath+"history.xml";
  RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", std::string("History file ") + filename + " has been saved.");
 }

}
catch(RDK::UException &exception)
{
RLOG(exception.GetType(), RDK_SYS_MESSAGE, "sys", std::string("Core-SaveProject Exception: (Name=") + Name + std::string(") ") + exception.what());
}

 return true;
}

bool UApplication::SaveProjectAs(const std::string &filename)
{
 return true;
}

/// Закрывает проект
bool UApplication::CloseProject(void)
{
 if(!ProjectOpenFlag)
  return true;

 PauseChannel(-1);

 RDK::TProjectConfig config=GetProjectConfig();

 if(config.ProjectAutoSaveFlag)
  SaveProject();

 std::string filename=ProjectPath+ProjectFileName;


 RDK::UIVisualControllerStorage::ClearInterface();
// if(UServerControlForm)
//  UServerControlForm->ServerRestartTimer->Enabled=false;

 if(ProjectOpenFlag)
 {
  SetProjectFileName("");
  SetProjectPath("");
 }
 SetProjectOpenFlag(false);
 EngineControl->StopEngineStateThread();
 for(int i=GetNumChannels()-1;i>=0;i--)
 {
  Core_SelectChannel(i);
  if(GetEngine())
  {
   Env_ModelUnInit(0);
   Env_DestroyStructure();
   Env_UnInit();
   Model_Destroy();
  }
  Storage_FreeObjectsStorage();
 }

RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", std::string("Configuration ") + filename + " has been closed.");
 return true;
}

/// Клонирует проект в новое расположение
bool UApplication::CloneProject(const std::string &filename)
{
 return true;
}

/// Переименовывает папку проекта
bool UApplication::RenameProject(const std::string &filename)
{
 if(!ProjectOpenFlag)
  return false;

 if(filename.empty())
  return false;

 PauseChannel(-1);

 std::string resfilename=filename;

 int res=RdkMoveFile(ProjectPath, resfilename);

 if(filename.find_last_of("\\/") != filename.size()-1)
  resfilename+="/";

 if(res == 0)
 {
  SetProjectPath(resfilename);
  Project->SetProjectPath(ProjectPath);
  Env_SetCurrentDataDir(ProjectPath.c_str());

  std::list<std::string> last_list=LastProjectsList;
  last_list.push_front(ProjectPath+ProjectFileName);
  while(int(last_list.size())>LastProjectsListMaxSize
   && !LastProjectsList.empty())
  {
   last_list.pop_back();
  }

  LastProjectsList=last_list;

  SaveProjectsHistory();
  return true;
 }

 return false;
}

void UApplication::ReloadParameters(void)
{
 if(!ProjectOpenFlag)
  return;

 int channel_index=Core_GetSelectedChannelIndex();

 TProjectConfig config=Project->GetConfig();
 std::string params_file_name=config.ChannelsConfig[Core_GetSelectedChannelIndex()].ParametersFileName;
 if(params_file_name.empty())
 {
  config.ChannelsConfig[channel_index].ParametersFileName="Parameters.xml";
  Project->SetConfig(config);
 }

 if(!params_file_name.empty())
 {
  if(extract_file_path(params_file_name).empty())
   LoadParametersFromFile(channel_index, ProjectPath+params_file_name);
  else
   LoadParametersFromFile(channel_index, params_file_name);
 }
}

bool UApplication::CopyProject(const std::string &new_path)
{
 if(!ProjectOpenFlag)
  return false;

 if(new_path.empty())
  return false;

 if(new_path == ProjectPath)
  return true;

 SaveProject();

 if(RDK::CreateNewDirectory(new_path.c_str()) != 0)
  return false;

 RDK::CopyDir(ProjectPath, new_path, "*.*");
 return true;
}

/// Сохраняет только файл настроек проекта
bool UApplication::SaveProjectConfig(void)
{
 if(!ProjectOpenFlag)
  return false;

 bool is_saved(false);
 Project->WriteToXml(ProjectXml);

 try
 {
  TProjectConfig config=Project->GetConfig();
  ProjectXml.SelectNodeRoot("Project/General");

  if(config.InterfaceFileName.empty())
  {
   ProjectXml.WriteString("InterfaceFileName","Interface.xml");
  }

  is_saved=ProjectXml.SaveToFile(ProjectPath+ProjectFileName);
  if(!is_saved)
  RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("Core-SaveProject: Can't save project config file: ") + ProjectFileName);
 }
 catch(RDK::UException &exception)
 {
 RLOG(exception.GetType(), RDK_SYS_MESSAGE, "sys", std::string("Core-SaveProjectConfig Exception: (Name=") + Name + std::string(") ") + exception.what());
 }

 return true;
}
// --------------------------


// --------------------------
// Методы управления движком
// --------------------------
/// Управление числом каналов
int UApplication::GetNumChannels(void) const
{
 return EngineControl->GetNumChannels();
}

bool UApplication::SetNumChannels(int num)
{
 int old_num=GetNumChannels();
 if(!EngineControl->SetNumChannels(num))
  return false;

 if(!ServerControl->SetNumChannels(old_num))
  return false;

 Project->SetNumChannels(num);

 for(int i=old_num;i<num;i++)
 {
  if(!MCore_IsChannelInit(i))
   MCore_ChannelInit(i,0,(void*)ExceptionHandler);
 }

 return true;
}

bool UApplication::InsertChannel(int index)
{
 if(!EngineControl->InsertChannel(index))
  return false;

 if(!ServerControl->InsertChannel(index))
  return false;

 Project->InsertChannel(index);

 if(!MCore_IsChannelInit(index))
  MCore_ChannelInit(index,0,(void*)ExceptionHandler);
 return true;
}

bool UApplication::DeleteChannel(int index)
{
if (index == 0)
    return false;

 if(!EngineControl->DeleteChannel(index))
  return false;

 if(!ServerControl->DeleteChannel(index))
  return false;

 Project->DeleteChannel(index);

 return true;
}

/// Клонирует канал source_id в cloned_id
bool UApplication::CloneChannel(int source_id, int cloned_id)
{
 if(source_id<0 || cloned_id <0)
  return false;

 if(cloned_id>=Core_GetNumChannels())
 {
  if(!SetNumChannels(cloned_id+1))
   return false;
 }

 try
 {
  RDK::TProjectConfig config=GetProjectConfig();
  config.ChannelsConfig[cloned_id]=config.ChannelsConfig[source_id];

  TProjectChannelConfig &source_channel=config.ChannelsConfig[source_id];
  TProjectChannelConfig &cloned_channel=config.ChannelsConfig[cloned_id];

  // Меняем номера индексов в именах файлов модели, параметров и состояний
  if(cloned_id>0)
   cloned_channel.ModelFileName=std::string("model_")+sntoa(cloned_id+1)+".xml";
  else
   cloned_channel.ModelFileName=std::string("model")+".xml";

  if(cloned_id>0)
   cloned_channel.ParametersFileName=std::string("parameters_")+sntoa(cloned_id+1)+".xml";
  else
   cloned_channel.ParametersFileName=std::string("parameters")+".xml";

  if(cloned_id>0)
   cloned_channel.StatesFileName=std::string("states_")+sntoa(cloned_id+1)+".xml";
  else
   cloned_channel.StatesFileName=std::string("states")+".xml";

  int selected_engine=Core_GetSelectedChannelIndex();
  Core_SelectChannel(cloned_id);

  if(!Core_IsChannelInit())
   Core_ChannelInit(cloned_channel.PredefinedStructure,reinterpret_cast<void*>(ExceptionHandler));
  else
   Env_SetPredefinedStructure(cloned_channel.PredefinedStructure);

   Model_SetDefaultTimeStep(cloned_channel.DefaultTimeStep);
   Log_SetDebugMode(config.DebugMode);
   Log_SetDebugSysEventsMask(config.DebugSysEventsMask);
   Log_SetDebuggerMessageFlag(config.DebuggerMessageFlag);
   Env_SetCurrentDataDir(ProjectPath.c_str());
   Env_CreateStructure();
   Env_Init();

 if(cloned_channel.PredefinedStructure == 0 && !cloned_channel.ModelFileName.empty())
 {
  if(extract_file_path(cloned_channel.ModelFileName).empty())
  {
   RdkCopyFile(ProjectPath+source_channel.ModelFileName, ProjectPath+cloned_channel.ModelFileName);
   LoadModelFromFile(cloned_id, ProjectPath+cloned_channel.ModelFileName);
  }
  else
  {
   RdkCopyFile(source_channel.ModelFileName, ProjectPath+cloned_channel.ModelFileName);
   LoadModelFromFile(cloned_id, cloned_channel.ModelFileName);
  }
 }

 if(!cloned_channel.ParametersFileName.empty())
 {
  if(extract_file_path(cloned_channel.ParametersFileName).empty())
  {
   RdkCopyFile(ProjectPath+source_channel.ParametersFileName, ProjectPath+cloned_channel.ParametersFileName);
   LoadParametersFromFile(cloned_id, ProjectPath+cloned_channel.ParametersFileName);
  }
  else
  {
   RdkCopyFile(source_channel.ParametersFileName, ProjectPath+cloned_channel.ParametersFileName);
   LoadParametersFromFile(cloned_id, cloned_channel.ParametersFileName);
  }
 }

 if(config.ProjectAutoSaveStatesFlag)
 {
  if(!cloned_channel.StatesFileName.empty())
  {
   if(extract_file_path(cloned_channel.StatesFileName).empty())
   {
	RdkCopyFile(ProjectPath+source_channel.StatesFileName, ProjectPath+cloned_channel.StatesFileName);
	LoadStatesFromFile(cloned_id, ProjectPath+cloned_channel.StatesFileName);
   }
   else
   {
	RdkCopyFile(source_channel.StatesFileName, ProjectPath+cloned_channel.StatesFileName);
	LoadStatesFromFile(cloned_id, cloned_channel.StatesFileName);
   }
  }
 }

 if(Model_Check())
 {
  Model_SetGlobalTimeStep("",cloned_channel.GlobalTimeStep);
  if(cloned_channel.InitAfterLoad)
   MEnv_Init(cloned_id);
  if(cloned_channel.ResetAfterLoad)
   MEnv_Reset(cloned_id,0);
 }
 EngineControl->SetCalculateMode(cloned_id, cloned_channel.CalculationMode);

 Core_SelectChannel(selected_engine);
 SetProjectConfig(config);
}
catch(RDK::UException &exception)
{
RLOG(exception.GetType(), RDK_SYS_MESSAGE, "sys", std::string("Core-OpenCloneChannel Exception: (Name=") + Name + std::string(") ") + exception.what());
}
catch(...)
{
 throw;
}

 return true;
}


// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
void UApplication::StartChannel(int channel_index)
{
 EngineControl->StartChannel(channel_index);
}

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
void UApplication::PauseChannel(int channel_index)
{
 EngineControl->PauseChannel(channel_index);
}

/// Сбрасывает аналитику выбранного канала, или всех, если channel_index == -1
void UApplication::ResetChannel(int channel_index)
{
 EngineControl->ResetChannel(channel_index);
}

/// Делает шаг расчета выбранного канала, или всех, если channel_index == -1
void UApplication::StepChannel(int channel_index)
{
 EngineControl->StepChannel(channel_index);
}

/// Возвращает true если канал запущен
bool UApplication::IsChannelStarted(int channel_index)
{
 if(!EngineControl)
  return false;
 return (EngineControl->CheckCalcState(channel_index) == UEngineControl::csRunning);
}

/// Проверяет состояние расчета по id канала
UEngineControl::UCalcState UApplication::CheckCalcState(int channel_id) const
{
 if(!EngineControl)
  return UEngineControl::csUnknown;
 return EngineControl->CheckCalcState(channel_id);
}
// --------------------------

// --------------------------
// Методы загрузки сохранения данных в файл
// --------------------------
bool UApplication::LoadModelFromFile(int channel_index, const std::string &file_name, TProjectLoadDiagnostics* diagnostics)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
 {
  RLOG(RDK_EX_ERROR, channel_index, nullptr, std::string("Failed to load model file: ") + file_name);
  return false;
 }

 if(!data.empty())
 {
  MModel_Destroy(channel_index);
  // Передаем диагностику в Model_LoadComponent через Engine
  // Используем GetEngineLock для получения правильного Engine для канала
  // ВАЖНО: GetEngineLock должен возвращать указатель на тот же Engine из EngineList[channel_index]
  RDK::UELockPtr<RDK::UEngine> engine_lock = RDK::GetEngineLock(channel_index);
  RDK::UEngine* engine_ptr = engine_lock.Get();

  if(engine_ptr && diagnostics)
  {
   // Временно сохраняем диагностику в Engine для использования в Model_LoadComponent
   // Это не идеально, но позволяет избежать изменения C API
   // Проверяем, что Engine действительно тот же, что и в EngineList[channel_index]
   RDK::UEPtr<RDK::UEngine> engine_direct = RDK::GetEngine(channel_index);
   if(engine_direct.Get() == engine_ptr)
   {
	engine_ptr->SetLoadDiagnostics(diagnostics);
   }
   else
   {
	// Это не должно происходить, но на всякий случай логируем
	RLOG(RDK_EX_WARNING, channel_index, nullptr,
		 std::string("Warning: Engine pointer mismatch in LoadModelFromFile for channel ") + RDK::sntoa(channel_index));
   }
  }

  int result = MModel_LoadComponent(channel_index, "",data.c_str());

  // Проверяем, что Engine все еще тот же, и очищаем диагностику
  if(engine_ptr && diagnostics)
  {
   RDK::UEPtr<RDK::UEngine> engine_direct = RDK::GetEngine(channel_index);
   if(engine_direct.Get() == engine_ptr)
   {
	engine_ptr->SetLoadDiagnostics(nullptr);
   }
  }
  if(result == RDK_SUCCESS)
   return true;
 }
 return false;
}

bool UApplication::SaveModelToFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 const char *p_buf=MModel_SaveComponent(channel_index, "");
 bool res=false;
 if(p_buf)
 {
  SaveBuffer=p_buf;
  Engine_FreeBufString(p_buf);
  if(SaveBuffer.empty())
  {
   RLOG(RDK_EX_ERROR, channel_index, nullptr, std::string("SaveModelToFile in") + file_name + " error: model size iz zero! File not changed.");
   return false;
  }

  if(SaveBuffer[0]!='<')
  {
   SaveBuffer[0]='<';
   RLOG(RDK_EX_WARNING, channel_index, nullptr, std::string("SaveModelToFile in") + file_name + " warning: first symbol INVALID. Fixed.");
  }

  res=SaveFileSafe(file_name,SaveBuffer,"save.tmp",3);
 }
 return res;
}

bool UApplication::LoadParametersFromFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
 {
  RLOG(RDK_EX_ERROR, channel_index, nullptr, std::string("Failed to load parameters file: ") + file_name);
  return false;
 }

 if(!data.empty())
 {
  MModel_LoadComponentParameters(channel_index, "",data.c_str());
  return true;
 }
 return false;
}

bool UApplication::SaveParametersToFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 const char *p_buf=MModel_SaveComponentParameters(channel_index, "");
 bool res=false;
 if(p_buf)
 {
  SaveBuffer=p_buf;
  Engine_FreeBufString(p_buf);
  if(SaveBuffer.empty())
  {
   RLOG(RDK_EX_ERROR, channel_index, nullptr, std::string("SaveParametersToFile in") + file_name + " error: model size iz zero! File not changed.");
   return false;
  }

  if(SaveBuffer[0]!='<')
  {
   SaveBuffer[0]='<';
   RLOG(RDK_EX_WARNING, channel_index, nullptr, std::string("SaveParametersToFile in") + file_name + " warning: first symbol INVALID. Fixed.");
  }

  res=SaveFileSafe(file_name,SaveBuffer,"save.tmp",3);
 }
 return res;
}

bool UApplication::LoadStatesFromFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
  return false;

 if(!data.empty())
 {
  int i=Core_GetSelectedChannelIndex();
  Core_SelectChannel(channel_index);
  Model_LoadComponentState("",&data[0]);
  Core_SelectChannel(i);
  return true;
 }
 return false;
}

bool UApplication::SaveStatesToFile(int channel_index, const std::string &file_name)
{
 if(!MCore_IsChannelInit(channel_index))
  return false;

 int i=Core_GetSelectedChannelIndex();
 Core_SelectChannel(channel_index);
 const char *p_buf=Model_SaveComponentState("");
 bool res=true;
 if(p_buf)
 {
  res=SaveFileSafe(file_name,p_buf,"save.tmp",3);
 }
 Engine_FreeBufString(p_buf);
 Core_SelectChannel(i);
 return res;
}

bool UApplication::LoadDescriptionFromFile(int channel_index, const std::string &file_name)
{
 return false;
}

bool UApplication::SaveDescriptionToFile(int channel_index, const std::string &file_name)
{
 return false;
}

bool UApplication::LoadClassesDescriptionsFromFile(const std::string &file_name)
{
 if(!Core_IsChannelInit())
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
  return false;

 if(!data.empty())
 {
  Storage_LoadClassesDescription(data.c_str());
  return true;
 }
 return false;
}

bool UApplication::SaveClassesDescriptionsToFile(const std::string &file_name)
{
 const char *p=Storage_SaveClassesDescription();
 bool res=true;
 if(p)
 {
  res=SaveFileSafe(file_name,p,"save.tmp",3);
  Engine_FreeBufString(p);
 }
 return res;
}

bool UApplication::LoadCommonClassesDescriptionsFromFile(const std::string &file_name)
{
 if(!Core_IsChannelInit())
  return false;

 std::string data;
 if(!LoadFile(file_name,data))
  return false;

 if(!data.empty())
 {
  Storage_LoadCommonClassesDescription(data.c_str());
  return true;
 }
 return false;
}

bool UApplication::SaveCommonClassesDescriptionsToFile(const std::string &file_name)
{
 const char *p=Storage_SaveCommonClassesDescription();
 bool res=true;
 if(p)
 {
  res=SaveFileSafe(file_name,p,"save.tmp",3);
  Engine_FreeBufString(p);
 }

 return res;
}

/// Загружает историю проектов из файла
void UApplication::LoadProjectsHistory(void)
{
 std::string opt_name=extract_file_name(ApplicationFileName);
 if(opt_name.size()>4)
 opt_name=opt_name.substr(0,opt_name.size()-4);
 opt_name=opt_name+".projecthist";
 RDK::UIniFile<char> history_ini;
 history_ini.LoadFromFile((WorkDirectory+opt_name).c_str());
 std::vector<std::string> history;
 history_ini.GetVariableList("General",history);
 sort(history.begin(),history.end());

 LastProjectsList.clear();
 for(size_t i=0;i<history.size();i++)
 {
  LastProjectsList.push_back(history_ini("General",history[i],""));
  if(int(i)>=LastProjectsListMaxSize)
   break;
 }
}

/// Сохраняет историю проектов в файл
void UApplication::SaveProjectsHistory(void)
{
 RDK::UIniFile<char> history_ini;

 std::list<std::string>::iterator I=LastProjectsList.begin();
 int i=0;
 for(;I != LastProjectsList.end();I++)
 {
  history_ini("General",std::string("Hist")+RDK::sntoa(i++),*I);
 }

 std::string opt_name=extract_file_name(ApplicationFileName);
 if(opt_name.size()>4)
 opt_name=opt_name.substr(0,opt_name.size()-4);
 opt_name=opt_name+".projecthist";
 history_ini.SaveToFile(WorkDirectory+opt_name);
}

/// Флаг принудительного сохранения конфигураций в старом формате
bool UApplication::IsUseNewXmlFormatProjectFile(void) const
{
 return UseNewXmlFormatProjectFile;
}

bool UApplication::ChangeUseNewXmlFormatProjectFile(bool value)
{
 if(UseNewXmlFormatProjectFile == value)
  return true;

 UseNewXmlFormatProjectFile=value;
 if(Project)
  Project->SetForceOldXmlFormat(!value);
 return true;
}

/// Флаг включения нового представления файловой структуры конфигурации
/// (только при сохранении данных конфигурации в новом формате)
bool UApplication::IsUseNewProjectFilesStructure(void) const
{
 return UseNewProjectFilesStructure;
}

bool UApplication::ChangeUseNewProjectFilesStructure(bool value)
{
 if(UseNewProjectFilesStructure == value)
  return true;

 UseNewProjectFilesStructure=value;
 if(Project)
  Project->SetForceNewConfigFilesStructure(value);
 return true;
}
// --------------------------

// --------------------------
// Вспомогательные методы управления счетом
// --------------------------
/// Сохраняет точки в истории изменений конфигурации
bool UApplication::FixSavePoint(USerStorageXML &xml)
{
    xml.SelectNodeRoot("History");

    int size = 0;

    std::string size_str = xml.GetNodeAttribute("Size");
    if(!size_str.empty())
        size = RDK::atoi(size_str);

    size++;

    xml.SetNodeAttribute("Size",RDK::sntoa(size));

    xml.SelectNodeForce("save_point_"+RDK::sntoa(size));
    xml.SetNodeAttribute("UserName",UserName);
    xml.SetNodeAttribute("UserId",  RDK::sntoa(UserId));

    time_t time_data;
    time(&time_data);
    xml.SetNodeAttribute("Time",  RDK::get_text_time(time_data, '.', '_'));

    xml.SelectRoot();
    return true;
}

/// Включает и выключает тестовый режим
void UApplication::ChangeTestModeState(bool state)
{
 if(TestMode == state)
  return;

 TestMode=state;
 if(TestMode == true)
  RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Test mode is ON.");
 else
  RLOG(RDK_EX_DEBUG, RDK_SYS_MESSAGE, "sys", "Test mode is OFF.");
}


/// Инициализация парсера командной строки
void UApplication::InitCmdParser(void)
{
#ifndef __BORLANDC__
 CmdLineDescription.add_options()
    ("help", "produce help message")
    ("standalone", "standalone vesrion of server without network")
    ("conf", po::value<string>(), "Configuration file name")
    ("ctime", po::value<double>(), "Calculation time interval, in seconds")
    ("info", po::value<string>(), "Information about core, possible: CollectionsList, ClassesList, CollectionClassesList, ClassProperties")
    ("class", po::value<string>(), "Class name")
    ("collection", po::value<string>(), "collection name")
    ("mask", po::value<unsigned>(), "Property mask")
    ("save_model_bmp", po::value<string>(), "Component name")
    ("session", po::value<unsigned>(), "Session Id")
    ("log-level", po::value<string>(), "Global log severity (fatal|error|warning|info|debug)")
    ("log-verbosity", po::value<int>(), "Maximum verbosity for debug logging (VLOG level)")
    ("log-channel-level", po::value<std::vector<std::string>>()->composing(), "Channel overrides in the form index:level")
    ("log-sys-level", po::value<string>(), "System channel log severity override")
    ("log-glob-level", po::value<string>(), "Global channel log severity override")
;
#endif
}
/*
int UApplication::ParseArgs(const std::vector<std::string> &args, std::map<std::string,std::string> &parsed_args)
{
 if(args.empty())
  return RDK_SUCCESS;

 parsed_args["Application"]=args[0];
 size_t index=1;
 for(size_t i=1;i<args.size()-1;i++)
 {
  parsed_args[args[i]]=args[i+1];
  index+=2;
 }

 return RDK_SUCCESS;
} */


/// Вычисляет заголовок приложения
void UApplication::CalcAppCaption(void)
{
 AppCaption=std::string("[")+Project->GetConfig().ProjectName+std::string(": ")+ProjectPath+ProjectFileName+"]";
}

/// Обновляет состояние средств логгирования
void UApplication::UpdateLoggers(void)
{
 std::string primary_dir = EnsureDirectoryAndNormalize(CalcCurrentLogDir());
 if(primary_dir.empty())
  return;

 PendingPrimaryLogDir = primary_dir;
 RdkCoreManager.SetLogDir(primary_dir.c_str());

 if(!LoggingInitialized)
  return;

 ApplyPrimaryLogDestination(primary_dir);
}




/// Сохраняет файл из строки, через временный файл. Делает n_pass попыток сохранить с чтением результата и сразвнением с оригиналом.
/// Если сохранение не удалось, то старый файл остается как был.
/// Если сохранение удалось, то временный файл заменяет старый
bool UApplication::SaveFileSafe(const std::string &file_name, const std::string &buffer, const std::string &temp_file_name, int n_pass)
{
 if(temp_file_name.empty())
  return false;

 if(n_pass<1)
  return false;

 if(temp_file_name == file_name)
  return false;

 bool is_temp_saved(false);

 for(int i=0;i<n_pass;i++)
 {
  bool is_saved=SaveFile(temp_file_name,buffer);
  if(!is_saved)
  {
   RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("SaveFileSafe file: ") + file_name + std::string(" as ") + temp_file_name + std::string(" save attepmt #") + sntoa(i + 1) + " FAILED.");
   continue;
  }

  std::string temp_buffer;
  bool is_loaded=LoadFile(temp_file_name,temp_buffer);
  if(!is_loaded)
  {
   RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("SaveFileSafe file: ") + file_name + std::string(" as ") + temp_file_name + std::string(" test load attepmt #") + sntoa(i + 1) + " FAILED.");
   continue;
  }

  if(buffer != temp_buffer)
  {
   RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("SaveFileSafe file: ") + file_name + std::string(" as ") + temp_file_name + std::string(" compare attepmt #") + sntoa(i + 1) + " FAILED.");
   continue;
  }
  is_temp_saved=true;
  break;
 }

 if(!is_temp_saved)
 {
  RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("SaveFileSafe file: ") + file_name + std::string(" as ") + temp_file_name + std::string(" all ") + sntoa(n_pass) + " attepmts FAILED.");
  return false;
 }

 int copy_error=RdkCopyFile(temp_file_name, file_name);

 if(!copy_error)
  return true;

RLOG(RDK_EX_ERROR, RDK_SYS_MESSAGE, "sys", std::string("SaveFileSafe file: ") + temp_file_name + std::string(" copy to ") + file_name + std::string(" FAILED with error code ") + sntoa(copy_error));
 return false;
}
// --------------------------

/// Валидирует конфигурацию проекта
TProjectLoadDiagnostics UApplication::ValidateProject(const std::string &filename)
{
 TProjectLoadDiagnostics diagnostics;

 // Сохраняем оригинальный путь к project.ini
 std::string original_project_file = filename;
 std::string project_path_orig = extract_file_path(original_project_file);

 // Создаем временную директорию для копий файлов проекта
 // Это гарантирует, что оригинальные файлы не будут изменены при валидации
 std::string temp_dir;
 std::string temp_project_file;
 std::vector<std::string> temp_files_to_cleanup;

 try
 {
  // Создаем временную директорию
  std::filesystem::path temp_base = std::filesystem::temp_directory_path();
  std::string project_name = extract_file_name(original_project_file);
  if(project_name.size() > 4 && project_name.substr(project_name.size() - 4) == ".ini")
  {
   project_name = project_name.substr(0, project_name.size() - 4);
  }

  // Создаем уникальное имя директории с временной меткой
  std::time_t now = std::time(nullptr);
  char time_str[64];
  std::strftime(time_str, sizeof(time_str), "%Y%m%d_%H%M%S", std::localtime(&now));
  std::string temp_dir_name = std::string("nmsdk_validate_") + project_name + "_" + time_str;
  temp_dir = (temp_base / temp_dir_name).string();

  if(!std::filesystem::create_directories(temp_dir))
  {
   RLOG(RDK_EX_WARNING, RDK_SYS_MESSAGE, "sys", std::string("Failed to create temp directory for validation: ") + temp_dir);
   // Продолжаем без временных копий (fallback к оригинальной логике)
   temp_dir.clear();
  }
  else
  {
   RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", std::string("Created temp directory for validation: ") + temp_dir);
  }
 }
 catch(const std::exception& e)
 {
  RLOG(RDK_EX_WARNING, RDK_SYS_MESSAGE, "sys", std::string("Exception creating temp directory: ") + e.what());
  temp_dir.clear();
 }

 // Если удалось создать временную директорию, копируем файлы
 if(!temp_dir.empty())
 {
  try
  {
   RDK::USerStorageXML project_xml_orig;

   if(project_xml_orig.LoadFromFile(original_project_file, ""))
   {
	// Копируем project.ini
	std::filesystem::path orig_project_path(original_project_file);
	std::filesystem::path temp_project_path = std::filesystem::path(temp_dir) / orig_project_path.filename();
	std::filesystem::copy_file(orig_project_path, temp_project_path, std::filesystem::copy_options::overwrite_existing);
	temp_project_file = temp_project_path.string();
	temp_files_to_cleanup.push_back(temp_project_file);

	// Читаем project.ini для извлечения путей к файлам
	project_xml_orig.SelectNodeRoot("Project");

	// Копируем InterfaceFileName (если есть)
	if(project_xml_orig.SelectNode("General"))
	{
	 std::string interface_file = project_xml_orig.ReadString("InterfaceFileName", "");
	 if(!interface_file.empty())
	 {
	  std::string interface_path;
	  if(extract_file_path(interface_file).empty())
	  {
	   interface_path = project_path_orig + interface_file;
	  }
	  else
	  {
	   interface_path = interface_file;
	  }

	  if(std::filesystem::exists(interface_path))
	  {
	   std::filesystem::path temp_interface = std::filesystem::path(temp_dir) / std::filesystem::path(interface_file).filename();
	   std::filesystem::copy_file(interface_path, temp_interface, std::filesystem::copy_options::overwrite_existing);
	   temp_files_to_cleanup.push_back(temp_interface.string());
	  }
	 }

     // Старый формат project.ini (без Channels): Model/Parameters/States лежат в <General>
     // Важно скопировать именно те имена, которые указаны в project.ini, чтобы относительные пути
     // разрешались внутри temp_dir.
     {
      // ModelFileName (опционально в старом формате)
      std::string model_file = project_xml_orig.ReadString("ModelFileName", "");
      if(!model_file.empty())
      {
       std::string model_path;
       if(extract_file_path(model_file).empty())
        model_path = project_path_orig + model_file;
       else
        model_path = model_file;

       if(std::filesystem::exists(model_path))
       {
        std::filesystem::path temp_model = std::filesystem::path(temp_dir) / std::filesystem::path(model_file).filename();
        std::filesystem::copy_file(model_path, temp_model, std::filesystem::copy_options::overwrite_existing);
        temp_files_to_cleanup.push_back(temp_model.string());
       }
      }

      // ParametersFileName (опционально в старом формате)
      std::string params_file = project_xml_orig.ReadString("ParametersFileName", "");
      if(!params_file.empty())
      {
       std::string params_path;
       if(extract_file_path(params_file).empty())
        params_path = project_path_orig + params_file;
       else
        params_path = params_file;

       if(std::filesystem::exists(params_path))
       {
        std::filesystem::path temp_params = std::filesystem::path(temp_dir) / std::filesystem::path(params_file).filename();
        std::filesystem::copy_file(params_path, temp_params, std::filesystem::copy_options::overwrite_existing);
        temp_files_to_cleanup.push_back(temp_params.string());
       }
      }

      // StatesFileName (опционально в старом формате)
      std::string states_file = project_xml_orig.ReadString("StatesFileName", "");
      if(!states_file.empty())
      {
       std::string states_path;
       if(extract_file_path(states_file).empty())
        states_path = project_path_orig + states_file;
       else
        states_path = states_file;

       if(std::filesystem::exists(states_path))
       {
        std::filesystem::path temp_states = std::filesystem::path(temp_dir) / std::filesystem::path(states_file).filename();
        std::filesystem::copy_file(states_path, temp_states, std::filesystem::copy_options::overwrite_existing);
        temp_files_to_cleanup.push_back(temp_states.string());
       }
      }

      // ProjectDescriptionFileName (не критично для валидации, но полезно для чистоты логов)
      std::string desc_file = project_xml_orig.ReadString("ProjectDescriptionFileName", "");
      if(!desc_file.empty())
      {
       std::string desc_path;
       if(extract_file_path(desc_file).empty())
        desc_path = project_path_orig + desc_file;
       else
        desc_path = desc_file;

       if(std::filesystem::exists(desc_path))
       {
        std::filesystem::path temp_desc = std::filesystem::path(temp_dir) / std::filesystem::path(desc_file).filename();
        std::filesystem::copy_file(desc_path, temp_desc, std::filesystem::copy_options::overwrite_existing);
        temp_files_to_cleanup.push_back(temp_desc.string());
       }
      }
     }

	 project_xml_orig.SelectUp();
	}

	// Копируем файлы для каждого канала
	if(project_xml_orig.SelectNode("Channels"))
	{
	 int channels_count = project_xml_orig.GetNumNodes();
	 for(int ch_idx = 0; ch_idx < channels_count; ch_idx++)
	 {
	  if(project_xml_orig.SelectNode(ch_idx))
	  {
	   // ModelFileName
	   std::string model_file = project_xml_orig.ReadString("ModelFileName", "");
	   if(!model_file.empty())
	   {
		std::string model_path;
		if(extract_file_path(model_file).empty())
		{
		 model_path = project_path_orig + model_file;
		}
		else
		{
		 model_path = model_file;
		}

		if(std::filesystem::exists(model_path))
		{
		 std::filesystem::path temp_model = std::filesystem::path(temp_dir) / std::filesystem::path(model_file).filename();
		 std::filesystem::copy_file(model_path, temp_model, std::filesystem::copy_options::overwrite_existing);
		 temp_files_to_cleanup.push_back(temp_model.string());
		}
	   }

	   // ParametersFileName
	   std::string params_file = project_xml_orig.ReadString("ParametersFileName", "");
	   if(!params_file.empty())
	   {
		std::string params_path;
		if(extract_file_path(params_file).empty())
		{
		 params_path = project_path_orig + params_file;
		}
		else
		{
		 params_path = params_file;
		}

		if(std::filesystem::exists(params_path))
		{
		 std::filesystem::path temp_params = std::filesystem::path(temp_dir) / std::filesystem::path(params_file).filename();
		 std::filesystem::copy_file(params_path, temp_params, std::filesystem::copy_options::overwrite_existing);
		 temp_files_to_cleanup.push_back(temp_params.string());
		}
	   }

	   // StatesFileName (опционально)
	   std::string states_file = project_xml_orig.ReadString("StatesFileName", "");
	   if(!states_file.empty())
	   {
		std::string states_path;
		if(extract_file_path(states_file).empty())
		{
		 states_path = project_path_orig + states_file;
		}
		else
		{
		 states_path = states_file;
		}

		if(std::filesystem::exists(states_path))
		{
		 std::filesystem::path temp_states = std::filesystem::path(temp_dir) / std::filesystem::path(states_file).filename();
		 std::filesystem::copy_file(states_path, temp_states, std::filesystem::copy_options::overwrite_existing);
		 temp_files_to_cleanup.push_back(temp_states.string());
		}
	   }

	   project_xml_orig.SelectUp();
	  }
	 }
	 project_xml_orig.SelectUp();
	}

	RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", std::string("Copied ") + RDK::sntoa(static_cast<int>(temp_files_to_cleanup.size())) + std::string(" files to temp directory for validation"));
   }
  }
  catch(const std::exception& e)
  {
   RLOG(RDK_EX_WARNING, RDK_SYS_MESSAGE, "sys", std::string("Exception copying files for validation: ") + e.what());
   // Продолжаем с оригинальным файлом
   temp_dir.clear();
   temp_project_file.clear();
   temp_files_to_cleanup.clear();
  }
 }

 // Определяем, какой файл использовать для валидации
 std::string validation_project_file = temp_project_file.empty() ? original_project_file : temp_project_file;

 // Сначала парсим XML проекта для предварительной проверки классов компонентов и связей
 // Важно: делаем это ДО загрузки проекта, чтобы XML не был изменен
 // Используем ОРИГИНАЛЬНЫЙ файл для парсинга, чтобы получить правильные пути к оригинальным файлам
 std::string project_path = extract_file_path(original_project_file);
 RDK::USerStorageXML project_xml_check;
 std::map<int, std::set<std::string> > xml_components_by_channel; // Компоненты из XML по каналам
 std::map<int, std::map<std::string, std::string> > xml_component_classes_by_channel; // Классы компонентов из XML по каналам
 std::map<int, RDK::UStringLinksList> xml_links_by_channel; // Связи из XML по каналам

 if(project_xml_check.LoadFromFile(original_project_file, ""))
 {
  project_xml_check.SelectNodeRoot("Project");
  if(project_xml_check.SelectNode("Channels"))
  {
   int channels_count = project_xml_check.GetNumNodes();
   for(int ch_idx = 0; ch_idx < channels_count; ch_idx++)
   {
	if(project_xml_check.SelectNode(ch_idx))
	{
	 std::string model_file = project_xml_check.ReadString("ModelFileName", "");
	 if(!model_file.empty())
	 {
	  std::string model_file_path;
	  if(extract_file_path(model_file).empty())
	  {
	   model_file_path = project_path + model_file;
	  }
	  else
	  {
	   model_file_path = model_file;
	  }

	   // Парсим XML модели для проверки классов компонентов
	   RDK::USerStorageXML model_xml;
	   if(model_xml.LoadFromFile(model_file_path, "Save"))
	   {
		model_xml.SelectNodeRoot("Save");
		if(model_xml.SelectNode("Model"))
		{
		 // Рекурсивная функция для парсинга компонентов (как в UNet::LoadComponent)
		 std::function<void(RDK::USerStorageXML&, const std::string&)> parseComponents = [&](RDK::USerStorageXML& xml, const std::string& parent_path) {
		  if(xml.SelectNode("Components"))
		  {
		   int components_count = xml.GetNumNodes();
		   for(int comp_idx = 0; comp_idx < components_count; comp_idx++)
		   {
			if(xml.SelectNode(comp_idx))
			{
			 std::string comp_name = xml.GetNodeName();
			 std::string comp_class = xml.GetNodeAttribute("Class");
			 std::string full_comp_name = parent_path.empty() ? comp_name : parent_path + "." + comp_name;

			 xml_components_by_channel[ch_idx].insert(full_comp_name);
			 xml_component_classes_by_channel[ch_idx][full_comp_name] = comp_class;

			 // Класс будет проверен после загрузки проекта, когда Storage будет инициализирован
			 // Пока просто сохраняем информацию

			 // Рекурсивно парсим дочерние компоненты
			 parseComponents(xml, full_comp_name);
			 xml.SelectUp();
			}
		   }
		  xml.SelectUp();
		 }
		};

		parseComponents(model_xml, "");

		// Возвращаемся к узлу Model для парсинга связей
		model_xml.SelectNodeRoot("Save");
		model_xml.SelectNode("Model");

		// Парсим связи из XML модели
		if(model_xml.SelectNode("Links"))
		{
		 model_xml >> xml_links_by_channel[ch_idx];
		 model_xml.SelectUp();
		}

		model_xml.SelectUp();
	   }
	  }
	 }
	 project_xml_check.SelectUp();
	}
   }
  }
 }

 // Загружаем проект с диагностикой
 // Используем временный project.ini, если он был создан, иначе оригинальный
 bool loaded = OpenProject(validation_project_file, &diagnostics);

 // Флаг для отслеживания, нужно ли выполнить cleanup
 bool need_cleanup = !temp_dir.empty();

 if(!loaded)
 {
  if(diagnostics.errors.empty())
  {
   diagnostics.errors.push_back(std::string("Failed to open project file: ") + original_project_file);
  }
  diagnostics.isValid = false;
  // Продолжаем выполнение для cleanup временных файлов
 }

 // Дополнительные проверки после загрузки
 const TProjectConfig& config = GetProjectConfig();

 // Проверяем каждый канал на наличие компонентов и связей
 for(int i = 0; i < config.NumChannels; i++)
 {
  Core_SelectChannel(i);

  // Объявляем переменную до проверки Model_Check, чтобы она была доступна в конце цикла
  std::set<std::string> loaded_components;

  if(!Model_Check())
  {
   std::string error_msg = std::string("Channel ") + RDK::sntoa(i) + std::string(": Model does not exist");
   diagnostics.errors.push_back(error_msg);
   continue;
  }

  // Получаем загруженные компоненты
  const char* components_list = Model_GetComponentsNameList("");
  if(components_list && strlen(components_list) > 0)
  {
   std::string comp_list_str = components_list;
   std::istringstream iss(comp_list_str);
   std::string component;
   while(std::getline(iss, component, ','))
   {
	loaded_components.insert(component);
   }
  }

  // Проверяем классы компонентов из XML для этого канала
  if(xml_component_classes_by_channel.find(i) != xml_component_classes_by_channel.end())
  {
   const std::map<std::string, std::string>& xml_classes = xml_component_classes_by_channel[i];
   std::set<std::string> checked_top_level_components; // Чтобы не проверять один компонент дважды

   for(const auto& pair : xml_classes)
   {
	const std::string& comp_name = pair.first;
	const std::string& comp_class = pair.second;

	// Извлекаем имя компонента верхнего уровня
	std::string top_level_comp = comp_name;
	size_t dot_pos = comp_name.find('.');
	if(dot_pos != std::string::npos)
	{
	 top_level_comp = comp_name.substr(0, dot_pos);
	}

	// Проверяем класс только для компонентов верхнего уровня и только один раз
	if(checked_top_level_components.find(top_level_comp) == checked_top_level_components.end())
	{
	 checked_top_level_components.insert(top_level_comp);

	 // Проверяем класс независимо от того, загрузился ли компонент
	 if(!comp_class.empty())
	 {
	  // Используем CheckClass вместо GetClassId для более надежной проверки
	  // CheckClass не бросает исключения, просто возвращает bool
	  RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock(i);
	  if(storage)
	  {
	   if(!storage->CheckClass(comp_class))
	   {
		std::string error_msg = std::string("Channel ") + RDK::sntoa(i) +
								 std::string(", Component '") + top_level_comp +
								 std::string("': Class '") + comp_class + std::string("' does not exist in storage");
		diagnostics.errors.push_back(error_msg);
	   }
	  }
	 }

	 // Если компонент не загрузился, это дополнительная ошибка
	 if(loaded_components.find(top_level_comp) == loaded_components.end())
	 {
	  std::string error_msg = std::string("Channel ") + RDK::sntoa(i) +
							   std::string(", Component '") + top_level_comp +
							   std::string("': Component failed to load (possibly due to invalid class)");
	  diagnostics.errors.push_back(error_msg);
	 }
	}
   }
  }

  // Проверяем связи из XML для этого канала
  if(xml_links_by_channel.find(i) != xml_links_by_channel.end())
  {
   const RDK::UStringLinksList& xml_linkslist = xml_links_by_channel[i];
   const std::set<std::string>& xml_components = xml_components_by_channel[i];

   // Проверяем каждую связь из XML
   for(int link_idx = 0; link_idx < xml_linkslist.GetSize(); link_idx++)
   {
	const RDK::UStringLink& link = xml_linkslist[link_idx];

	// Извлекаем имя компонента-источника (до первой точки)
	std::string source_comp = link.Item.Id;
	size_t dot_pos = source_comp.find('.');
	if(dot_pos != std::string::npos)
	{
	 source_comp = source_comp.substr(0, dot_pos);
	}

	// Проверяем, существует ли компонент-источник в XML
	bool source_exists = false;
	for(const std::string& xml_comp : xml_components)
	{
	 std::string top_level = xml_comp;
	 size_t dot = xml_comp.find('.');
	 if(dot != std::string::npos)
	 {
	  top_level = xml_comp.substr(0, dot);
	 }
	 if(top_level == source_comp)
	 {
	  source_exists = true;
	  break;
	 }
	}

	if(!source_exists)
	{
	 std::string error_msg = std::string("Channel ") + RDK::sntoa(i) +
							 std::string(", Link ") + RDK::sntoa(link_idx) +
							 std::string(": Source component '") + source_comp + std::string("' does not exist in model");
	 diagnostics.errors.push_back(error_msg);
	}
	else if(loaded_components.find(source_comp) == loaded_components.end())
	{
	 // Компонент есть в XML, но не загрузился
	 std::string error_msg = std::string("Channel ") + RDK::sntoa(i) +
							 std::string(", Link ") + RDK::sntoa(link_idx) +
							 std::string(": Source component '") + source_comp + std::string("' failed to load");
	 diagnostics.errors.push_back(error_msg);
	}

	// Проверяем приемники связи
	for(size_t conn_idx = 0; conn_idx < link.Connector.size(); conn_idx++)
	{
	 const RDK::ULinkSideT<std::string>& connector = link.Connector[conn_idx];
	 std::string dest_comp_full = connector.Id;

	 // Извлекаем имя компонента-приемника (до первой точки)
	 std::string dest_comp = dest_comp_full;
	 dot_pos = dest_comp_full.find('.');
	 if(dot_pos != std::string::npos)
	 {
	  dest_comp = dest_comp_full.substr(0, dot_pos);
	 }

	 // Проверяем, существует ли компонент-приемник в XML
	 bool dest_exists = false;
	 for(const std::string& xml_comp : xml_components)
	 {
	  std::string top_level = xml_comp;
	  size_t dot = xml_comp.find('.');
	  if(dot != std::string::npos)
	  {
	   top_level = xml_comp.substr(0, dot);
	  }
	  if(top_level == dest_comp)
	  {
	   dest_exists = true;
	   break;
	  }
	 }

	 if(!dest_exists)
	 {
	  std::string error_msg = std::string("Channel ") + RDK::sntoa(i) +
							  std::string(", Link ") + RDK::sntoa(link_idx) +
							  std::string(", Connector ") + RDK::sntoa(conn_idx) +
							  std::string(": Destination component '") + dest_comp + std::string("' does not exist in model");
	  diagnostics.errors.push_back(error_msg);
	 }
	 else if(loaded_components.find(dest_comp) == loaded_components.end())
	 {
	  // Компонент есть в XML, но не загрузился
	  std::string error_msg = std::string("Channel ") + RDK::sntoa(i) +
							  std::string(", Link ") + RDK::sntoa(link_idx) +
							  std::string(", Connector ") + RDK::sntoa(conn_idx) +
							  std::string(": Destination component '") + dest_comp + std::string("' failed to load");
	  diagnostics.errors.push_back(error_msg);
	 }
	}
   }
  }

  // Проверяем классы загруженных компонентов
  for(const std::string& comp_name : loaded_components)
  {
	// Проверяем класс компонента
	const char* class_name = Model_GetComponentClassName(comp_name.c_str());
	if(class_name)
	{
	 // Используем CheckClass вместо GetClassId для более надежной проверки
	 RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock(i);
	 if(storage)
	 {
	  if(!storage->CheckClass(std::string(class_name)))
	  {
	   std::string error_msg = std::string("Channel ") + RDK::sntoa(i) +
								std::string(", Component ") + comp_name +
								std::string(": Class '") + class_name + std::string("' does not exist in storage");
	   diagnostics.errors.push_back(error_msg);
	  }
	 }
	 Engine_FreeBufString(class_name);
	}

	// Проверяем связи для каждого компонента
	const char* links_xml = Model_GetComponentInternalLinks(comp_name.c_str(), nullptr);
	if(links_xml)
	{
	 // Собираем множество непосредственных дочерних компонентов данного comp_name по данным XML текущего канала.
	 // (нужно для корректной валидации внутренних ссылок, где имена могут быть относительными)
	 std::set<std::string> internal_children;
	 auto xml_it = xml_components_by_channel.find(i);
	 if(xml_it != xml_components_by_channel.end())
	 {
	  const std::set<std::string>& xml_components = xml_it->second;
	  const std::string prefix = comp_name + ".";
	  for(const std::string& xml_comp : xml_components)
	  {
	   if(xml_comp.rfind(prefix, 0) != 0)
		continue;
	   // xml_comp = "<comp_name>.<child>...."
	   const size_t child_start = prefix.size();
	   const size_t dot = xml_comp.find('.', child_start);
	   const std::string child = (dot == std::string::npos)
								? xml_comp.substr(child_start)
								: xml_comp.substr(child_start, dot - child_start);
	   if(!child.empty())
		internal_children.insert(child);
	  }
	 }

	 // Парсим XML связей используя UStringLinksList
	 RDK::USerStorageXML links_xml_storage;
	 if(links_xml_storage.Load(links_xml, "Links"))
	 {
	  RDK::UStringLinksList linkslist;
	  links_xml_storage >> linkslist;

	  // Проверяем каждую связь
	  for(int link_idx = 0; link_idx < linkslist.GetSize(); link_idx++)
	  {
	   const RDK::UStringLink& link = linkslist[link_idx];

	   // Извлекаем имя компонента-источника (до первой точки, если есть)
	   std::string source_comp = link.Item.Id;
	   size_t dot_pos = source_comp.find('.');
	   if(dot_pos != std::string::npos)
	   {
		source_comp = source_comp.substr(0, dot_pos);
	   }

	   // Проверяем, существует ли компонент-источник
	   if(!source_comp.empty() && internal_children.find(source_comp) == internal_children.end())
	   {
		std::string error_msg = std::string("Channel ") + RDK::sntoa(i) +
								std::string(", Component ") + comp_name +
								std::string(", Link ") + RDK::sntoa(link_idx) +
								std::string(": Source component '") + source_comp + std::string("' does not exist");
		diagnostics.errors.push_back(error_msg);
	   }

	   // Проверяем приемники связи
	   for(size_t conn_idx = 0; conn_idx < link.Connector.size(); conn_idx++)
	   {
		const RDK::ULinkSideT<std::string>& connector = link.Connector[conn_idx];
		std::string dest_comp_full = connector.Id;

		// Извлекаем имя компонента-приемника (до первой точки)
		std::string dest_comp = dest_comp_full;
		dot_pos = dest_comp_full.find('.');
		if(dot_pos != std::string::npos)
		{
		 dest_comp = dest_comp_full.substr(0, dot_pos);
		}

		// Проверяем, существует ли компонент-приемник
		if(!dest_comp.empty() && internal_children.find(dest_comp) == internal_children.end())
		{
		 std::string error_msg = std::string("Channel ") + RDK::sntoa(i) +
								 std::string(", Component ") + comp_name +
								 std::string(", Link ") + RDK::sntoa(link_idx) +
								 std::string(", Connector ") + RDK::sntoa(conn_idx) +
								 std::string(": Destination component '") + dest_comp + std::string("' does not exist");
		 diagnostics.errors.push_back(error_msg);
		}
	   }
	  }
	 }
	 else
	 {
	  std::string warning_msg = std::string("Channel ") + RDK::sntoa(i) +
								std::string(", Component ") + comp_name +
								std::string(": Invalid links XML");
	  diagnostics.warnings.push_back(warning_msg);
	 }
	 Engine_FreeBufString(links_xml);
	}
  }

  // Проверяем, что модель существует, но список компонентов пуст
  if(Model_Check() && loaded_components.empty() && i < static_cast<int>(diagnostics.componentsCount.size()) && diagnostics.componentsCount[i] > 0)
  {
   // Модель существует, но список компонентов пуст - это предупреждение
   std::string warning_msg = std::string("Channel ") + RDK::sntoa(i) +
							std::string(": Model exists but component list is empty");
   diagnostics.warnings.push_back(warning_msg);
  }
 }

 // Финальная проверка валидности
 if(diagnostics.errors.empty() && diagnostics.channelsLoaded > 0)
 {
  diagnostics.isValid = true;
 }
 else
 {
  diagnostics.isValid = false;
 }

 // Удаляем временные файлы и директорию (гарантированно, даже при ошибках)
 if(need_cleanup && !temp_dir.empty())
 {
  try
  {
   // Закрываем проект перед удалением временных файлов
   CloseProject();

   // Удаляем все временные файлы
   for(const std::string& temp_file : temp_files_to_cleanup)
   {
	try
	{
	 if(std::filesystem::exists(temp_file))
	 {
	  std::filesystem::remove(temp_file);
	 }
	}
	catch(const std::exception& e)
	{
	 RLOG(RDK_EX_WARNING, RDK_SYS_MESSAGE, "sys", std::string("Failed to remove temp file: ") + temp_file + " - " + e.what());
	}
   }

   // Удаляем временную директорию
   if(std::filesystem::exists(temp_dir))
   {
	std::filesystem::remove_all(temp_dir);
	RLOG(RDK_EX_INFO, RDK_SYS_MESSAGE, "sys", std::string("Removed temp validation directory: ") + temp_dir);
   }
  }
  catch(const std::exception& e)
  {
   RLOG(RDK_EX_WARNING, RDK_SYS_MESSAGE, "sys", std::string("Exception cleaning up temp files: ") + e.what());
  }
 }

 return diagnostics;
}

}

#endif

