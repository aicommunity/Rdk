#ifndef rdk_initdllH
#define rdk_initdllH

#ifdef __BORLANDC__
    #define RDK_LIB_TYPE __declspec(dllexport)
    #define RDK_CALL __cdecl
#else
    #ifdef QT_VERSION
        #ifdef LIBRDK_LIBRARY
            #define RDK_LIB_TYPE Q_DECL_EXPORT
            #define RDK_CALL
        #else
            #define RDK_LIB_TYPE Q_DECL_IMPORT
            #define RDK_CALL
        #endif
    #else
        #ifdef __GNUC__
            #define RDK_LIB_TYPE
            #define RDK_CALL
        #else
            #define RDK_LIB_TYPE __declspec(dllexport)
            #define RDK_CALL __cdecl
        #endif
    #endif
#endif

#include "rdk_init.h"

#endif
