//---------------------------------------------------------------------------

#pragma hdrstop

#include "UTestManager.h"
#include "UApplication.h"
#include "boost/filesystem.hpp"

namespace RDK {

/// --------------------
/// Методы инициализации
/// --------------------
/// Возвращает указатель на тестируемое приложение
UEPtr<UApplication> UTestManager::GetApplication(void)
{
 return Application;
}

/// Задает тестируемое приложение
void UTestManager::SetApplication(const UEPtr<UApplication> &value)
{
 if(Application == value)
  return;

 Application=value;
}
/// --------------------

// --------------------
// Методы тестирования
// --------------------
/// Загрузка тестов
int UTestManager::LoadTests(const std::string &file_name)
{
 if(file_name.empty())
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "Tests description file name is empty!");
  return 1001;
 }

 /*boost::filesystem::path path(file_name);

 boost::filesystem::path p("/a/b/c/e/f");
 boost::filesystem::path base("/a/b/n/g");
 boost::filesystem::path rel = boost::filesystem::relative(p, base);
 std::string relStr = rel.string();

 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, relStr.c_str());*/

 std::vector<std::string> test_file_names;
 int decode_main_file_res=DecodeMainTestDescriptionFile(test_file_names);
 if(decode_main_file_res != RDK_SUCCESS)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, (std::string("UTestManager::LoadTests - decode main file tests description file FAILED! Error code: ")+sntoa(decode_main_file_res)).c_str());
  return 1002;
 }

 for(size_t i=0;i<test_file_names.size();i++)
 {
  UTest test;
  int decode_res=DecodeTestFile(test_file_names[i],test);
  if(decode_res != RDK_SUCCESS)
  {
   MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, (std::string("UTestManager::LoadTests - decode test file ")+test_file_names[i]+std::string(" FAILED! Error code: ")+sntoa(decode_res)).c_str());
   return 1003;
  }
 }
 return RDK_SUCCESS;
}


/// Проводит тестирование
/// Записывает в выходной массив результаты тестов
/// Возвращает код ошибки тестирования
int UTestManager::ProcessTests(void)
{
 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "UTestManager::ProcessTests called");
 return RDK_SUCCESS;
}
// --------------------

// --------------------
// Вспомогательные методы
// --------------------
/// Декодирует корневой файл описания тестов
/// Возвращает массив имен файлов с описаниями тестов
int UTestManager::DecodeMainTestDescriptionFile(std::vector<std::string> &test_file_names)
{
 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "UTestManager::DecodeMainTestDescriptionFile called");
 return RDK_SUCCESS;
}

/// Декодирует тест
int UTestManager::DecodeTestFile(const std::string &test_file_name, UTest &test)
{
 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "UTestManager::DecodeTestFile called");
 return RDK_SUCCESS;
}
// --------------------

}
