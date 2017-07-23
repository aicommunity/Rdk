#ifndef UTestManagerH
#define UTestManagerH

#include "../../Deploy/Include/rdk_cpp_initdll.h"
//#include "../../Libraries/Rtv-BasicLib/Core/RTVSupport.h"

namespace RDK {

class UApplication;

struct UPropertyTest
{
  std::string component;
  std::string property;
  std::string type;
  std::string delta;
  std::string value;
};

struct UTest
{
protected: // ������
  /// ����������� ����������
  UEPtr<UApplication> Application;

  /// ������������ �������
  int calcDuration;

  /// ����� ���������� ������
  /// true - ���������� ���������� �������� = calcDuration
  /// false - ������� ���������� � ������� calcDuration �����������
  bool stepsMode;

  /// ������ ����������� ������
  std::vector<UPropertyTest> propertyTests;

  /// ���� � ����, ����������� �������� ������������
  std::string testsFileName;

  /// ���� � ����� ������������ ������������ �������
  std::string testProjectFileName;

public: // ������
  // --------------------
  // ������ �������������
  // --------------------
  UTest(void);
  UTest(const UEPtr<UApplication> &value);

  /// ���������� ��������� �� ����������� ����������
  UEPtr<UApplication> GetApplication(void);

  /// ������ ����������� ����������
  virtual void SetApplication(const UEPtr<UApplication> &value);

  UTest& operator = (const UTest &copy);
  // --------------------

  // --------------------
  // ������ ������������
  // --------------------
  virtual int LoadTest(std::string testFile);

  /// �������� ������������
  /// ���������� ����������� ��������� �������� ������
  virtual int ProcessTest(void);

  // --------------------
  // ��������������� ������
  // --------------------
protected:
  // ������ ���������
  bool compareProperties(bool               value, std::string str, std::string delta);
  bool compareProperties(int                value, std::string str, std::string delta);
  bool compareProperties(unsigned int       value, std::string str, std::string delta);
  bool compareProperties(long long          value, std::string str, std::string delta);
  bool compareProperties(unsigned long long value, std::string str, std::string delta);
  bool compareProperties(float              value, std::string str, std::string delta);
  bool compareProperties(double             value, std::string str, std::string delta);
  bool compareProperties(std::string        value, std::string str, std::string);

//  bool compareProperties(std::vector<RTV::TZoneExt> value, std::string str, std::string delta);
};

/// �������� ������
class UTestManager
{
protected: // ������
/// ����������� ����������
UEPtr<UApplication> Application;

/// ������ ����������� ������
std::vector<UTest> tests;

public: // ������
// --------------------
// ������ �������������
// --------------------
/// ���������� ��������� �� ����������� ����������
UEPtr<UApplication> GetApplication(void);

/// ������ ����������� ����������
virtual void SetApplication(const UEPtr<UApplication> &value);
// --------------------

// --------------------
// ������ ������������
// --------------------
/// �������� ������
virtual int LoadTests(const std::string &file_name);

/// �������� ������������
/// ���������� � �������� ������ ���������� ������
/// ���������� ����������� ��������� ������
virtual int ProcessTests(void);
// --------------------


// --------------------
// ��������������� ������
// --------------------
protected:
/// ���������� �������� ���� �������� ������
/// ���������� ������ ���� ������ � ���������� ������
int DecodeMainTestDescriptionFile(std::vector<std::string> &test_file_names);

/// ���������� ����
int DecodeTestFile(const std::string &test_file_name, UTest &test);
// --------------------


};


}

#endif
