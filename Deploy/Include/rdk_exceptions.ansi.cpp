#ifndef RDK_EXCEPTIONS_CPP
#define RDK_EXCEPTIONS_CPP
#include "rdk_exceptions.ansi.h"
namespace RDK {                                  

// ��������� ������ � ��������� ���������� ���������� ����������
std::string GetSystemExceptionData(void)
{
 return std::string("additional information about SEH exception isn't available");
}

}

#endif

