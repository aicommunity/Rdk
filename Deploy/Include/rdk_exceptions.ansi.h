#ifndef RDK_EXCEPTIONS_H
#define RDK_EXCEPTIONS_H
#define RDK_SYS_TRY try
#define RDK_SYS_CATCH catch(...)
#define GET_SYSTEM_EXCEPTION_DATA RDK::GetSystemExceptionData()
#include <string>
                                  
namespace RDK {

// Формирует строку с описанием возникшего системного исключения
std::string GetSystemExceptionData(void);

}
#endif

