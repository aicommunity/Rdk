#ifndef UTestManagerH
#define UTestManagerH

#include "../../Deploy/Include/rdk_cpp_initdll.h"

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

  UTest(const UEPtr<UApplication> &value);

  /// ���������� ��������� �� ����������� ����������
  UEPtr<UApplication> GetApplication(void);

  /// ������ ����������� ����������
  virtual void SetApplication(const UEPtr<UApplication> &value);
  // --------------------

  // --------------------
  // ������ ������������
  // --------------------
  virtual int LoadTest(std::string testFile);

  /// �������� ������������
  /// ���������� ����������� ��������� �������� ������
  virtual int ProcessTest(void);
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
