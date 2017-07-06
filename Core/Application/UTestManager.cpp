//---------------------------------------------------------------------------

#pragma hdrstop

#include "UTestManager.h"
#include "UApplication.h"

namespace RDK {

/// --------------------
/// ������ �������������
/// --------------------
/// ���������� ��������� �� ����������� ����������
UEPtr<UApplication> UTest::GetApplication()
{
  return Application;
}

/// ������ ����������� ����������
void UTest::SetApplication(const UEPtr<UApplication> &value)
{
  if(Application == value)
   return;

  Application=value;
}

/// �������� ������
int UTest::LoadTest(string testFile)
{
  RDK::USerStorageXML testXML;
  if(testXML.Load(testFile, "TestCase"))
    return 1;

  if(testXML.SelectNode("Header"))
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

  testXML.SelectRoot();

  testXML.SelectNode("TestProperties");
  for(size_t i = 0; i < testXML.GetNumNodes("Property"); ++i)
  {
    testXML.SelectNode("Property", i);
    propertyTests.push_back(UPropertyTest{
                              testXML.GetNodeAttribute("Property"),
                              testXML.GetNodeAttribute("PropertyName"),
                              testXML.GetNodeAttribute("type"),
                              testXML.GetNodeAttribute("Delta"),
                              testXML.GetNodeText()
                            });
  }
}

/// �������� ������������
/// ���������� ��� ������ ������������
int UTest::ProcessTest()
{
  int returConde = RDK_SUCCESS;
  for(std::vector<UTest>::iterator i = propertyTests.begin(); i != propertyTests.end(); ++i)
  {

  }
}



/// --------------------
/// ������ �������������
/// --------------------
/// ���������� ��������� �� ����������� ����������
UEPtr<UApplication> UTestManager::GetApplication(void)
{
 return Application;
}

/// ������ ����������� ����������
void UTestManager::SetApplication(const UEPtr<UApplication> &value)
{
 if(Application == value)
  return;

 Application=value;
}
/// --------------------

// --------------------
// ������ ������������
// --------------------
/// �������� ������
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


/// �������� ������������
/// ���������� � �������� ������ ���������� ������
/// ���������� ��� ������ ������������
int UTestManager::ProcessTests(void)
{
 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "UTestManager::ProcessTests called");

 int returnCode = RDK_SUCCESS;

 for(std::vector<UTest>::iterator i = tests.begin(); i != tests.end(); ++i)
 {
   if ((*i)->ProcessTest() != RDK_SUCCESS)
     ++returnCode;
 }

 return returnCode;
}
// --------------------

// --------------------
// ��������������� ������
// --------------------
/// ���������� �������� ���� �������� ������
/// ���������� ������ ���� ������ � ���������� ������
int UTestManager::DecodeMainTestDescriptionFile(std::vector<std::string> &test_file_names)
{
 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "UTestManager::DecodeMainTestDescriptionFile called");
 return RDK_SUCCESS;
}

/// ���������� ����
int UTestManager::DecodeTestFile(const std::string &test_file_name, UTest &test)
{
 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, "UTestManager::DecodeTestFile called");
 return RDK_SUCCESS;
}



// --------------------

}
