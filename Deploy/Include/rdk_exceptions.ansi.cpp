#ifndef RDK_EXCEPTIONS_CPP
#define RDK_EXCEPTIONS_CPP
#include "rdk_exceptions.ansi.h"
namespace RDK {                                  

// ��������� ������ � ��������� ���������� ���������� ����������
std::string GetSystemExceptionData(void)
{
 return std::string("information isn't available");
}

}

#endif

