#ifndef UTestManagerH
#define UTestManagerH

#include "../../Deploy/Include/rdk_cpp_initdll.h"

namespace RDK {

class UApplication;

class UTest
{

};

/// �������� ������
class UTestManager
{
protected: // ������
/// ����������� ����������
UEPtr<UApplication> Application;

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
/// ���������� ��� ������ ������������
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
