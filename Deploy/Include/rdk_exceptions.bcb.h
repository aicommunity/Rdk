#ifndef RDK_EXCEPTIONS_H
#define RDK_EXCEPTIONS_H
#include <vcl.h>
//#include <excpt.h>
#include <string>
//#define RDK_SYS_TRY __try
//#define RDK_SYS_CATCH __except (ExceptionsPointers=GetExceptionInformation(),EXCEPTION_EXECUTE_HANDLER)
#define RDK_SYS_TRY try
#define RDK_SYS_CATCH catch(System::Sysutils::Exception &exception)
#define GET_SYSTEM_EXCEPTION_DATA RDK::GetSystemExceptionData(exception)

namespace RDK {

//extern LPEXCEPTION_POINTERS ExceptionsPointers;

// Формирует строку с описанием возникшего системного исключения
std::string GetSystemExceptionData(System::Sysutils::Exception &exception);

}

#endif

