#ifndef initlib_DEFS_H
#define initlib_DEFS_H

#define RDK_CORE

#ifdef __BORLANDC__
#define RDK_LIB_TYPE
#define RDK_CALL
#else
    #ifdef QT_VERSION
        #ifndef RDK_LIB_TYPE
            #define RDK_LIB_TYPE
            #define RDK_CALL
        #endif
    #else
        #ifndef RDK_LIB_TYPE
            #define RDK_LIB_TYPE
            #define RDK_CALL
        #endif
    #endif
#endif

#if __cplusplus >= 201103L
    #include <cstdint>
#else
// WARNING! ILP64 & SILP64 platforms don't detected by code below!!!
    // Check windows
    #if defined(_MSC_VER)
        #include <stdint.h>
/*        typedef __int8 int8_t;
        typedef unsigned __int8 uint8_t;
        typedef __int16 int16_t;
        typedef unsigned __int16 uint16_t;
        typedef __int32 int32_t;
        typedef unsigned __int32 uint32_t;
        typedef __int64 int64_t;
        typedef unsigned __int64 uint64_t;
        */
// Check GCC
    #elif defined(__BORLANDC__)
        #include <stdint.h>
/*        typedef __int8 int8_t;
		typedef unsigned __int8 uint8_t;
		typedef __int16 int16_t;
		typedef unsigned __int16 uint16_t;
		typedef __int32 int32_t;
		typedef unsigned __int32 uint32_t;
		typedef __int64 int64_t;
		typedef unsigned __int64 uint64_t;
		*/
    #elif defined(__GNUC__)
        #include <stdint.h>
/*       #if __x86_64__ || __ppc64__
            typedef char int8_t;
            typedef unsigned char uint8_t;
            typedef short int16_t;
            typedef unsigned short uint16_t;
            typedef int int32_t;
            typedef unsigned int uint32_t;
            typedef long long int64_t;
            typedef unsigned long long uint64_t;
        #else
            typedef char int8_t;
            typedef unsigned char uint8_t;
            typedef short int16_t;
            typedef unsigned short uint16_t;
            typedef int int32_t;
            typedef unsigned int uint32_t;
            typedef long long int64_t;
            typedef unsigned long long uint64_t;
        #endif
        */
    #else
        #error "unknown platform"
    #endif

#endif


#endif
