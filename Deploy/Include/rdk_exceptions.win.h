#ifndef RDK_EXCEPTIONS_H
#define RDK_EXCEPTIONS_H
//#define RDK_SYS_TRY __try
//#define RDK_SYS_CATCH __except (EXCEPTION_EXECUTE_HANDLER)
#define RDK_SYS_TRY try
#define RDK_SYS_CATCH catch(...)
#endif

