#include "UGlogGuiSink.h"

#include "UGlogFileTail.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace RDK
{

UGlogGuiSink& UGlogGuiSink::Instance()
{
 static UGlogGuiSink sink_instance;
 return sink_instance;
}

UGlogGuiSink::UGlogGuiSink()
{
 MaxMessages = 2000;
 TotalMessages = 0;
}

void UGlogGuiSink::StartSession(const std::string& directory, const std::string& base_name, std::time_t session_start)
{
 std::lock_guard<std::mutex> lock(FileMutex);
 if(directory.empty())
 {
  FileTail.reset();
  return;
 }
 if(!FileTail)
  FileTail = std::make_unique<UGlogFileTail>();
 FileTail->Reset(directory, base_name, session_start);
}

void UGlogGuiSink::AddDirectory(const std::string& directory)
{
 std::lock_guard<std::mutex> lock(FileMutex);
 if(!FileTail || directory.empty())
  return;
 FileTail->AddDirectory(directory);
}

std::string UGlogGuiSink::CreateMessageKey(int severity, const std::string& text) const
{
 // Создаем уникальный ключ на основе severity и текста сообщения
 // Используем простую конкатенацию для создания ключа
 std::ostringstream key;
 key << severity << "|" << text;
 return key.str();
}

void UGlogGuiSink::PushMessage(int severity, const std::string& text, bool check_duplicate) const
{
 // Проверяем дубликаты перед добавлением (если требуется)
 if(check_duplicate)
 {
  std::string message_key = CreateMessageKey(severity, text);
  
  std::lock_guard<std::mutex> dedup_lock(DedupMutex);
  // Проверяем, не было ли уже такого сообщения
  if(MessageKeys.find(message_key) != MessageKeys.end())
  {
   // Дубликат найден, пропускаем сообщение
   return;
  }
  
  // Добавляем ключ в set и очередь
  MessageKeys.insert(message_key);
  MessageKeysQueue.push_back(message_key);
  
  // Очищаем старые записи, если превышен лимит
  if(MessageKeysQueue.size() > MaxDedupKeys)
  {
   const std::string& oldest_key = MessageKeysQueue.front();
   MessageKeys.erase(oldest_key);
   MessageKeysQueue.pop_front();
  }
 }
 
 // Добавляем сообщение в очередь
 std::lock_guard<std::mutex> lock(QueueMutex);
 Messages.push_back({severity, text});
 if(Messages.size() > MaxMessages)
 {
  Messages.pop_front();
 }
 ++TotalMessages;
}

std::vector<UGlogGuiMessage> UGlogGuiSink::ReadMessages(std::size_t max_count) const
{
 DrainFileMessages();
 std::vector<UGlogGuiMessage> buffer;
 {
  std::lock_guard<std::mutex> lock(QueueMutex);
  if(max_count == 0 || max_count >= Messages.size())
  {
   buffer.assign(Messages.begin(), Messages.end());
   Messages.clear();
  }
  else
  {
   buffer.reserve(max_count);
   auto it = Messages.begin();
   for(std::size_t i=0; i<max_count && it != Messages.end(); ++i, ++it)
    buffer.push_back(*it);
   Messages.erase(Messages.begin(), Messages.begin() + static_cast<long>(max_count));
  }
 }
 return buffer;
}

void UGlogGuiSink::Clear()
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 Messages.clear();
 
 // Очищаем также ключи дедупликации
 std::lock_guard<std::mutex> dedup_lock(DedupMutex);
 MessageKeys.clear();
 MessageKeysQueue.clear();
}

void UGlogGuiSink::SetMaxMessages(std::size_t value)
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 MaxMessages = value;
 if(Messages.size() > MaxMessages)
 {
  const std::size_t overflow = Messages.size() - MaxMessages;
  for(std::size_t i=0; i<overflow; ++i)
   Messages.pop_front();
 }
}

std::size_t UGlogGuiSink::PendingMessages() const
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 return Messages.size();
}

std::size_t UGlogGuiSink::GetTotalMessages() const
{
 std::lock_guard<std::mutex> lock(QueueMutex);
 return TotalMessages;
}

std::string UGlogGuiSink::FormatTimestamp(std::time_t timestamp) const
{
 std::tm* time_struct = std::localtime(&timestamp);
 if(!time_struct)
  return std::string();

 std::ostringstream oss;
 oss << std::setfill('0')
     << std::setw(2) << time_struct->tm_mday << "/"
     << std::setw(2) << (time_struct->tm_mon + 1) << "/"
     << std::setw(4) << (time_struct->tm_year + 1900) << " "
     << std::setw(2) << time_struct->tm_hour << ":"
     << std::setw(2) << time_struct->tm_min << ":"
     << std::setw(2) << time_struct->tm_sec;
 return oss.str();
}

std::string UGlogGuiSink::SeverityToString(int severity) const
{
 switch(severity)
 {
  case RDK_EX_FATAL: return "FATAL";
  case RDK_EX_ERROR: return "ERROR";
  case RDK_EX_WARNING: return "WARNING";
  case RDK_EX_INFO: return "INFO";
  case RDK_EX_APP: return "APP";
  case RDK_EX_DEBUG: return "DEBUG";
  default: return "INFO";
 }
}

void UGlogGuiSink::DrainFileMessages() const
{
 std::unique_lock<std::mutex> lock(FileMutex);
 if(!FileTail)
  return;
 auto new_entries = FileTail->CollectNewMessages();
 lock.unlock();

 for(const auto& entry : new_entries)
 {
  // Форматируем сообщение
  std::string line = FormatTimestamp(entry.Timestamp);
  line.append(" [");
  line.append(SeverityToString(entry.Severity));
  line.append("] ");
  line.append(entry.Text);
  
  // PushMessage() выполнит проверку дубликатов на основе severity и текста
  // Используем оригинальный текст (entry.Text) для создания ключа дедупликации
  // Для этого создадим ключ из severity и оригинального текста перед форматированием
  std::string dedup_key = CreateMessageKey(entry.Severity, entry.Text);
  
  // Проверяем дубликаты перед добавлением
  {
   std::lock_guard<std::mutex> dedup_lock(DedupMutex);
   if(MessageKeys.find(dedup_key) != MessageKeys.end())
   {
    // Дубликат найден, пропускаем сообщение
    continue;
   }
   
   // Добавляем ключ в set и очередь
   MessageKeys.insert(dedup_key);
   MessageKeysQueue.push_back(dedup_key);
   
   // Очищаем старые записи, если превышен лимит
   if(MessageKeysQueue.size() > MaxDedupKeys)
   {
    const std::string& oldest_key = MessageKeysQueue.front();
    MessageKeys.erase(oldest_key);
    MessageKeysQueue.pop_front();
   }
  }
  
  // Добавляем сообщение (проверка дубликатов уже выполнена)
  PushMessage(entry.Severity, line, false);
 }
}

}

