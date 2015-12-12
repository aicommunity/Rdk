#ifndef RDK_BCB_EXCEPTIONS_CPP
#define RDK_BCB_EXCEPTIONS_CPP
#include "rdk_exceptions.bcb.h"
#include <except.h>
#include "../../Core/Utilities/USupport.h"

EXCEPTION_POINTERS * RdkExceptionsPointers;

namespace RDK {

char SysExceptionNames[][100]={"ACCESS_VIOLATION","ARRAY_BOUNDS_EXCEEDED","BREAKPOINT","DATATYPE_MISALIGNMENT",
		"FLT_DENORMAL_OPERAND","FLT_DIVIDE_BY_ZERO","FLT_INEXACT_RESULT",
		"FLT_INVALID_OPERATION","FLT_OVERFLOW","FLT_STACK_CHECK","FLT_UNDERFLOW",
		"ILLEGAL_INSTRUCTION","IN_PAGE_ERROR","INT_DIVIDE_BY_ZERO","INT_OVERFLOW",
		"INVALID_DISPOSITION","NONCONTINUABLE_EXCEPTION","PRIV_INSTRUCTION","SINGLE_STEP","STACK_OVERFLOW"};

DWORD SysExceptionCodes[]={EXCEPTION_ACCESS_VIOLATION,EXCEPTION_ARRAY_BOUNDS_EXCEEDED,EXCEPTION_BREAKPOINT,EXCEPTION_DATATYPE_MISALIGNMENT,
		EXCEPTION_FLT_DENORMAL_OPERAND,EXCEPTION_FLT_DIVIDE_BY_ZERO,EXCEPTION_FLT_INEXACT_RESULT,
		EXCEPTION_FLT_INVALID_OPERATION,EXCEPTION_FLT_OVERFLOW,EXCEPTION_FLT_STACK_CHECK,EXCEPTION_FLT_UNDERFLOW,
		EXCEPTION_ILLEGAL_INSTRUCTION,EXCEPTION_IN_PAGE_ERROR,EXCEPTION_INT_DIVIDE_BY_ZERO,EXCEPTION_INT_OVERFLOW,
		EXCEPTION_INVALID_DISPOSITION,EXCEPTION_NONCONTINUABLE_EXCEPTION,EXCEPTION_PRIV_INSTRUCTION,EXCEPTION_SINGLE_STEP,EXCEPTION_STACK_OVERFLOW};

// Формирует строку с описанием возникшего системного исключения
std::string GetSystemExceptionData(void)
{
 PEXCEPTION_RECORD pExceptionRecord=RdkExceptionsPointers->ExceptionRecord;

 int index=-1;
 for(size_t i=0;i<sizeof(SysExceptionCodes)/sizeof(DWORD);i++)
  if(pExceptionRecord->ExceptionCode == SysExceptionCodes[i])
  {
   index=i;
   break;
  }

 std::string str;
 if (pExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
 {
  // Access violation type - Write/Read.
  str = (pExceptionRecord->ExceptionInformation[0]) ? " Write address " : " Read address ";
  str+=RDK::sntoa(pExceptionRecord->ExceptionInformation[1]);
 }

 if(index>=0)
  return std::string("[SysExName: ")+std::string(SysExceptionNames[index])
		+std::string(" Address: ")+RDK::sntohex(pExceptionRecord->ExceptionAddress)+str+std::string("] ");
 else
  return std::string("[SysExCode: ")+RDK::sntohex(pExceptionRecord->ExceptionCode)
		+std::string(" Address: ")+RDK::sntohex(pExceptionRecord->ExceptionAddress)+str+std::string("] ");
}


// Формирует строку с описанием возникшего системного исключения
std::string GetSystemExceptionData(System::Sysutils::Exception &exception)
{
#if !defined(_WIN64)
 return AnsiString(String("[")+exception.Message+exception.StackTrace+String(" see ")+__ThrowFileName()+String(":")+__ThrowLineNumber()+String("] ")).c_str();
#else
 return AnsiString(String("[")+exception.Message+exception.StackTrace).c_str();
#endif}
}

}

#endif

