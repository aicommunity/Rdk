#ifndef rdk_initlibH
#define rdk_initlibH

#ifdef __BORLANDC__
#define RDK_LIB_TYPE
#define RDK_CALL
#else
    #ifdef QT_VERSION
    #define RDK_LIB_TYPE
    #define RDK_CALL
    #else
    #define RDK_LIB_TYPE
    #define RDK_CALL
    #endif
#endif


#include "rdk_init.h"

#endif
