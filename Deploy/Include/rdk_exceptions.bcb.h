#ifndef RDK_EXCEPTIONS_H
#define RDK_EXCEPTIONS_H
#include <vcl.h>

extern EXCEPTION_POINTERS * RdkExceptionsPointers;
//#include <excpt.h>
#include <string>
#define RDK_SYS_TRY try
//#define RDK_SYS_CATCH __except (ExceptionsPointers=GetExceptionInformation(),EXCEPTION_EXECUTE_HANDLER)
#define RDK_SYS_CATCH __except (RDK::xfilter(RdkExceptionsPointers = GetExceptionInformation()))
#define GET_SYSTEM_EXCEPTION_DATA RDK::GetSystemExceptionData()

//#define RDK_SYS_TRY try
//#define RDK_SYS_CATCH catch(System::Sysutils::Exception &exception)
#define GET_BCB_SYSTEM_EXCEPTION_DATA RDK::GetSystemExceptionData(exception)

namespace RDK {

static int xfilter(EXCEPTION_POINTERS *xp) {
	int rc;
	EXCEPTION_RECORD *xr = xp->ExceptionRecord;
	CONTEXT *xc = xp->ContextRecord;
	switch (xr->ExceptionCode) {
	case EXCEPTION_BREAKPOINT:
		// whoops, someone left an embedded breakpoint.
		// just step over it (1 byte on x86)
		++xc->Eip;
		rc = EXCEPTION_CONTINUE_EXECUTION;
		break;
//	case EXCEPTION_ACCESS_VIOLATION:
//		rc = EXCEPTION_EXECUTE_HANDLER;
//		break;
	default:
		// give up
//		rc = EXCEPTION_CONTINUE_SEARCH;
		rc = EXCEPTION_EXECUTE_HANDLER;
		break;
	};
	return rc;
}

//extern LPEXCEPTION_POINTERS ExceptionsPointers;

// Формирует строку с описанием возникшего системного исключения
std::string GetSystemExceptionData(System::Sysutils::Exception &exception);
std::string GetSystemExceptionData(void);


}

#endif

