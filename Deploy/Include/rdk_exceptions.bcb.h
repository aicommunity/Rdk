#ifndef RDK_EXCEPTIONS_H
#define RDK_EXCEPTIONS_H
#include <Windows.h>
#include <excpt.h>
#include <string>
#define RDK_SYS_TRY __try
#define RDK_SYS_CATCH __except (ExceptionsPointers=GetExceptionInformation(),EXCEPTION_EXECUTE_HANDLER)
                                  
namespace RDK {

extern LPEXCEPTION_POINTERS ExceptionsPointers;

// Формирует строку с описанием возникшего системного исключения
std::string GetSystemExceptionData(void);

}

#endif

