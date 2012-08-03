#ifndef RDK_EXCEPTIONS_H
#define RDK_EXCEPTIONS_H
#include <Windows.h>
#include <excpt.h>
#define RDK_SYS_TRY try
#define RDK_SYS_CATCH __except (EXCEPTION_EXECUTE_HANDLER)
#endif

