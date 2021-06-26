#ifndef RDK_EXCEPTIONS_CPP
#define RDK_EXCEPTIONS_CPP
#include "rdk_exceptions.ansi.h"
namespace RDK {                                  

// Формирует строку с описанием возникшего системного исключения
std::string GetSystemExceptionData(void)
{
 return std::string("information isn't available");
}

}

#endif

