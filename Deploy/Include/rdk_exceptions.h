#ifndef RDK_COMMON_EXCEPTIONS
#define RDK_COMMON_EXCEPTIONS

#ifdef __BORLANDC__
#include "rdk_exceptions.bcb.h"
#else
    #ifdef QT_VERSION
           #include "rdk_exceptions.win.h"
    #else
        #ifdef __GNUC__
           #include "rdk_exceptions.gcc.h"
        #else
           #include "rdk_exceptions.ansi.h"
        #endif
    #endif
#endif
                             
#endif

