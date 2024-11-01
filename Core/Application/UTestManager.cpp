//---------------------------------------------------------------------------
#ifndef UTESTMANAGER_CPP
#define UTESTMANAGER_CPP

#include "UTestManager.h"
#include "UApplication.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace RDK {

/// ������ ��� ���������� ����������� ����, �� ����� �������������� � ������ ������ ���������
/// ��������! �������� ��������� � ����������, ��������� ��� �������
#define TEST_PROPERTY_WITH_TYPE(type) \
  if(property->GetLanguageType() == typeid(type)) \
  { \
    try \
    { \
      if(compareProperties(*(reinterpret_cast<type*>(data)), \
                           testProperty.value, testProperty.delta)) \
      { \
        MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, ("SUCCESS - porperty: "+ testProperty.component + "." + testProperty.property).c_str()); \
        continue; \
      } \
      else \
      { \
        MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, ("FAIL - porperty: "+ testProperty.component + "." + testProperty.property +", not the same as the current value! expected value is: "+ testProperty.value).c_str()); \
        ++returnCode; \
        continue; \
      } \
    } \
    catch(boost::bad_lexical_cast &e) \
    { \
      MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, ("FAIL - porperty: "+ testProperty.component + "." + testProperty.property + " error, application can't parse value, message :" + e.what()).c_str()); \
      ++returnCode; \
      continue; \
    } \
  } \


// --------------------
// ������ �������������
// --------------------
UTest::UTest(void)
{
  calcDuration = 0;
  stepsMode = true;
}


UTest::UTest(const UEPtr<UApplication> &value)
{
  Application=value;
  calcDuration = 0;
  stepsMode = true;
}

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

UTest& UTest::operator = (const UTest &copy)
{
 Application=copy.Application;

 calcDuration=copy.calcDuration;
 stepsMode=copy.stepsMode;
 propertyTests=copy.propertyTests;
 testsFileName=copy.testsFileName;
 testProjectFileName=copy.testProjectFileName;
 return *this;
}

// --------------------
// ������ ������������
// --------------------

/// �������� ������
int UTest::LoadTest(string testFile)
{
  testsFileName = testFile;
  RDK::USerStorageXML testXML;
  if(!testXML.LoadFromFile(testFile, "TestCase"))
  {
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, (testsFileName + " UTest::LoadTest: can't find <TestCase> in Xml file").c_str());
    return 1;
  }

  if(!testXML.SelectNode("Header"))
  {
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, (testsFileName + " UTest::LoadTest: can't find <Header> in Xml file").c_str());
    return 2;
  }

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

  try
  {
    calcDuration = boost::lexical_cast<int>(strDuration);
  }
  catch(boost::bad_lexical_cast &e)
  {
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, (testsFileName + " calcDuration empty, boost::lexical_cast error, message :" + e.what()).c_str());
    calcDuration = 0;
  }

  testXML.SelectUp();
  if(testXML.SelectNode("ConfigFilePath"))
  {
    testProjectFileName = testXML.GetNodeText();
  }
  else
  {
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, (testsFileName + " UTest::LoadTest: can't find <ConfigFilePath> in Xml file").c_str());
    return 3;
  }

  testXML.SelectRoot();
  if(testXML.SelectNode("TestProperties"))
  {
    for(int i = 0; i < testXML.GetNumNodes("Property"); ++i)
    {
      testXML.SelectNode("Property", i);
      std::string sValue;

      // ���� ������ ���� ���� xml, �� ��������� ��� xml, ���� ��� �� ��������� ������ �� ����
      if(testXML.SelectNode(0))
      {
        testXML.SaveFromNode(sValue);
        testXML.SelectUp();
      }
      else
      {
        sValue = testXML.GetNodeText();
      }

      UPropertyTest pt = {
        testXML.GetNodeAttribute("Component"),
        testXML.GetNodeAttribute("PropertyName"),
        testXML.GetNodeAttribute("type"),
        testXML.GetNodeAttribute("Delta"),
        sValue
      };
      propertyTests.push_back(pt);
      testXML.SelectUp();
    }
  }
  else
  {
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, (testsFileName + " UTest::LoadTest: can't find <TestProperties> in Xml file").c_str());
  }
  return RDK_SUCCESS;
}

/// �������� ������������
/// ���������� ����������� ��������� �������� ������
int UTest::ProcessTest()
{
  int returnCode = RDK_SUCCESS;

  try
  {
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, std::string("Property test log: ").c_str());

    Application->OpenProject(testProjectFileName);
    /*std::vector<string> strs;
    boost::split(strs, testsFileName, boost::is_any_of("/"));
    strs.pop_back();
    strs.push_back("TestLog");

//    std::string oldLogDir = Application->GetLogDir();

    Application->SetLogDir(boost::join(strs, "/") + "/");

//    std::string newLogDir = Application->GetLogDir();*/

    if(stepsMode)
    {
      for(size_t i = 0; i < static_cast<size_t>(calcDuration); ++i)
        Application->StepChannel(-1);
    }
    else
    {
      Application->StartChannel(-1);
      Sleep(calcDuration);
      Application->PauseChannel(-1);
    }

    UELockPtr<UContainer> model = GetModelLock<UContainer>(Core_GetSelectedChannelIndex());

    if(!model)
    {
      MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, std::string("Model doesn't exist! Testing terminated!").c_str());
      return -1;
    }

    for(std::vector<UPropertyTest>::iterator i = propertyTests.begin(); i != propertyTests.end(); ++i)
    {
      UPropertyTest testProperty = (*i);
      RDK::UEPtr<RDK::UComponent> component;
      if (testProperty.component.empty())
          component = model.Get();
      else
          component = model->GetComponentL(testProperty.component, true);

      if(!component)
      {
        MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, ("FAIL - component: "+ testProperty.component +", doesn't exist!").c_str());
        ++returnCode;
        continue;
      }

      UEPtr<UIProperty> property = component->FindProperty(testProperty.property);

      if(!property)
      {
        MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, ("FAIL - porperty: "+ testProperty.component + "." + testProperty.property +", doesn't exist!").c_str());
        ++returnCode;
        continue;
      }

      void* data = const_cast<void*>(property->GetMemoryArea());

      if(!data)
      {
        MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, ("FAIL - porperty: "+ testProperty.component + "." + testProperty.property +", is NULL!").c_str());
        ++returnCode;
        continue;
      }

      TEST_PROPERTY_WITH_TYPE(bool)
      TEST_PROPERTY_WITH_TYPE(int)
      TEST_PROPERTY_WITH_TYPE(unsigned int)
      TEST_PROPERTY_WITH_TYPE(long long)
      TEST_PROPERTY_WITH_TYPE(unsigned long long)
      TEST_PROPERTY_WITH_TYPE(float)
      TEST_PROPERTY_WITH_TYPE(double)
      TEST_PROPERTY_WITH_TYPE(std::string)
      TEST_PROPERTY_WITH_TYPE(MDMatrix<double>)

//      TEST_PROPERTY_WITH_TYPE(std::vector<RTV::TZoneExt>)

      MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, ("WEIRD - porperty: "+ testProperty.component + "." + testProperty.property +", haven't compare function!").c_str());

    }
  }
  catch(RDK::UException& e)
  {
    ++returnCode;
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("FAIL, exception catched UTest::ProcessTest - ")+e.what()).c_str());
  }
  catch(std::exception& e)
  {
    ++returnCode;
    MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("FAIL, exception catched UTest::ProcessTest - ")+e.what()).c_str());
  }

  return returnCode;
}

// --------------------
// ��������������� ������
// --------------------

bool UTest::compareProperties(bool value, std::string str, std::string delta)
{
  if(!delta.empty())
    if(boost::lexical_cast<bool>(delta))
      return true;
  return boost::lexical_cast<bool>(str) == value;
}

bool UTest::compareProperties(int value, std::string str, std::string delta)
{
  int intDelta = 0;
  if(!delta.empty())
    intDelta = boost::lexical_cast<int>(delta);

  int intStr = boost::lexical_cast<int>(str);
  return abs(value - intStr) <= intDelta;
}

bool UTest::compareProperties(unsigned int value, std::string str, std::string delta)
{
  unsigned int intDelta = 0;
  if(!delta.empty())
    intDelta = boost::lexical_cast<unsigned int>(delta);

  unsigned int intStr = boost::lexical_cast<unsigned int>(str);
  unsigned int diff = value > intStr ? value - intStr: intStr - value;
  return diff <= intDelta;
}

bool UTest::compareProperties(long long value, std::string str, std::string delta)
{
  long long intDelta = 0;
  if(!delta.empty())
    intDelta = boost::lexical_cast<long long>(delta);

  long long intStr = boost::lexical_cast<long long>(str);
  return static_cast<long long>(abs(value - intStr)) <= intDelta;
}

bool UTest::compareProperties(unsigned long long value, std::string str, std::string delta)
{
  unsigned long long intDelta = 0;
  if(!delta.empty())
    intDelta = boost::lexical_cast<unsigned long long>(delta);

  unsigned long long intStr = boost::lexical_cast<unsigned long long>(str);
  unsigned long long diff = value > intStr ? value - intStr: intStr - value;
  return diff <= intDelta;
}

bool UTest::compareProperties(float value, std::string str, std::string delta)
{
  float floatDelta = 0.f;
  if(!delta.empty())
    floatDelta = boost::lexical_cast<float>(delta);

  float floatStr = boost::lexical_cast<float>(str);
  return fabs(value - floatStr) <= floatDelta;
}

bool UTest::compareProperties(double value, std::string str, std::string delta)
{
  double doubleDelta = 0.;
  if(!delta.empty())
    doubleDelta = boost::lexical_cast<double>(delta);

  double doubleStr = boost::lexical_cast<double>(str);
  return static_cast<double>(fabs(value - doubleStr)) <= doubleDelta;
}

bool UTest::compareProperties(std::string value, std::string str, std::string)
{
  return value.compare(str) == 0;
}

bool UTest::compareProperties(MDMatrix<double> value, string str, string delta)
{
  std::vector<std::string> strs;
  boost::split(strs, str, boost::is_any_of("\t"));

  return static_cast<size_t>(value.GetSize()) == strs.size();
}

/*bool UTest::compareProperties(std::vector<RTV::TZoneExt> value, string str, string delta)
{
  USerStorageXML storage;
  storage.Load(str, "");
  storage.SelectNode("Objects");

  std::string strS = storage.GetNodeAttribute("Size");
  size_t strVectorSize = 0;
  if(!strS.empty())
    strVectorSize = boost::lexical_cast<size_t>(strS);

  return value.size() == strVectorSize;
}*/



// --------------------
// ������ �������������
// --------------------
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

 RDK::USerStorageXML testsXML;
 if(!testsXML.LoadFromFile(file_name, "UTestCases"))
 {
   MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_INFO, (file_name + " UTestManager::LoadTest: can't find <UTestCases> in Xml file").c_str());
   return 1002;
 }

 int loadRet = 0;
 int testsCount = testsXML.GetNumNodes("UTest");
 for(int i = 0; i < testsCount; ++i)
 {
   if(!testsXML.SelectNode("UTest", i))
     continue;

   tests.push_back(UTest(Application));
   loadRet += tests[i].LoadTest(testsXML.GetNodeText());
   testsXML.SelectUp();
 }
 if(loadRet)
  return 1003;

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
  returnCode += (*i).ProcessTest();

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

#endif
