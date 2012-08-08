#ifndef RDK_EXCEPTIONS_CPP
#define RDK_EXCEPTIONS_CPP
#include "rdk_exceptions.bcb.h"
#include "../../Core/Utilities/USupport.h"
namespace RDK {                                  

LPEXCEPTION_POINTERS ExceptionsPointers;

// ��������� ������ � ��������� ���������� ���������� ����������
std::string GetSystemExceptionData(void)
{
 PEXCEPTION_RECORD pExceptionRecord=ExceptionsPointers->ExceptionRecord;

 return std::string(" Code: ")+sntoa(pExceptionRecord->ExceptionCode)
 		+std::string(" Address: ")+sntoa(pExceptionRecord->ExceptionAddress)+std::string(" ");
}

}

#endif

