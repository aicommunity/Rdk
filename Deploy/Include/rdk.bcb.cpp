#define RDK_BUILDER
#define RDK_WINAPI
#include "rdk_exceptions.bcb.cpp"
#include "rdk.cpp"
//#include "../../Core/System/BCB/rdk_system.bcb.cpp"

extern "C" void tss_cleanup_implemented(void)
{
    /*
    This function's sole purpose is to cause a link
error in cases where
    automatic tss cleanup is not implemented by
Boost.Threads as a
    reminder that user code is responsible for calling
the necessary
    functions at the appropriate times (and for
implementing an a
    tss_cleanup_implemented() function to eliminate the
linker's
    missing symbol error).
    If Boost.Threads later implements automatic tss
cleanup in cases
    where it currently doesn't (which is the plan), the
duplicate
    symbol error will warn the user that their custom
solution is no
    longer needed and can be removed.
    */
}







