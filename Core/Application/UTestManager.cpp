//---------------------------------------------------------------------------

#pragma hdrstop

#include "UTestManager.h"
#include "UApplication.h"

namespace RDK {

/// --------------------
/// Методы инициализации
/// --------------------

UTest::UTest(const UEPtr<UApplication> &value)
{
  Application=value;
}

/// Возвращает указатель на тестируемое приложение
  UEPtr<UApplication> UTest::GetApplication()
{
  return Application;
}

/// Задает тестируемое приложение
void UTest::SetApplication(const UEPtr<UApplication> &value)
{
  if(Application == value)
   return;

  Application=value;
}

/// Загрузка тестов
int UTest::LoadTest(string testFile)
{
  testsFileName = testFile;
  RDK::USerStorageXML testXML;
  if(!testXML.LoadFromFile(testFile, "TestCase"))
    return 1;

  if(!testXML.SelectNode("Header"))
    return 2;

  testXML.SelectNode("CalculateDuration");
  std::string strDuration = testXML.GetNodeAttribute("Steps");

  if(strDuration.empty())
  {
    stepsMode = false;
    strDuration = testXML.GetNodeAttribute("Milliseconds");
  }
  else
  {
    stepsMode = true;
  }

  calcDuration = atoi(strDuration);

  testXML.SelectUp();
  testXML.SelectNode("ConfigFilePath");
  testProjectFileName = testXML.GetNodeText();

  testXML.SelectRoot();

  testXML.SelectNode("TestProperties");
  for(size_t i = 0; i < static_cast<size_t>(testXML.GetNumNodes("Property")); ++i)
  {
    testXML.SelectNode("Property", i);
    propertyTests.push_back(UPropertyTest{
                              testXML.GetNodeAttribute("Component"),
                              testXML.GetNodeAttribute("PropertyName"),
                              testXML.GetNodeAttribute("type"),
                              testXML.GetNodeAttribute("Delta"),
                              testXML.GetNodeText()
                            });
    testXML.SelectUp();
  }
  return RDK_SUCCESS;
}

/// Проводит тестирование
/// Возвращает код ошибки тестирования
int UTest::ProcessTest()
{
  int returnCode = RDK_SUCCESS;

  try
  {
    Application->OpenProject(testProjectFileName);
    if(stepsMode)
    {
      for(size_t i = 0; i < calcDuration; ++i)
        Application->StepChannel(-1);
    }

    for(std::vector<UPropertyTest>::iterator i = propertyTests.begin(); i != propertyTests.end(); ++i)
    {

    }
  }
  catch(RDK::UException& e)
  {
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UTest::ProcessTest - ")+e.what()).c_str());
  }
  catch(std::exception& e)
  {
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UTest::ProcessTest - ")+e.what()).c_str());
  }

  return returnCode;
}



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

 UTest singleTest(Application);
 int loadRet = singleTest.LoadTest(file_name);

 if(loadRet)
  return 1002;

 tests.push_back(singleTest);

 /*std::vector<std::string> test_file_names;
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
 }*/
 return RDK_SUCCESS;
}


/// Проводит тестирование
/// Записывает в выходной массив результаты тестов
/// Возвращает код ошибки тестирования
int UTestManager::ProcessTests(void)
{
 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "UTestManager::ProcessTests called");

 int returnCode = RDK_SUCCESS;

 for(std::vector<UTest>::iterator i = tests.begin(); i != tests.end(); ++i)
 {
   if ((*i).ProcessTest() != RDK_SUCCESS)
     ++returnCode;
 }

 return returnCode;
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
