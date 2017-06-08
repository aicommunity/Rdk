#ifndef UTestManagerH
#define UTestManagerH

#include "../../Deploy/Include/rdk_cpp_initdll.h"

namespace RDK {

class UApplication;

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


};


}

#endif
