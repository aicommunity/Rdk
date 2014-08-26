#ifndef initlib_DEFS_H
#define initlib_DEFS_H

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

#endif
