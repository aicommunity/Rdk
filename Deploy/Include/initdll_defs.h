#ifndef INIT_DLL_DEFS_H
#define INIT_DLL_DEFS_H

#define RDK_CORE

// Suppress redefinition warnings for macros that may be defined via -D in command line
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-macro-redefined"
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4100)
#pragma warning(disable : 4355)
#pragma warning(disable : 4800)
#pragma warning(disable : 4503)
#pragma warning(disable : 4251)
#pragma warning(disable : 4221)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wswitch"
#endif

#ifndef CDECL
 #if defined(_WIN32) || defined(_WIN64)
  #define CDECL __cdecl
 #elif defined(__GNUC__) && defined(_LP64)
  #define CDECL
 #else
  #define CDECL __attribute__ ((CDECL__))
 #endif
#endif

#ifndef RDK_LIB_TYPE
#if defined(__BORLANDC__)
	#if defined(LIBRDK_LIBRARY_EXPORT)
		#ifndef RDK_LIB_TYPE
		#define RDK_LIB_TYPE __declspec(dllexport)
		#endif
		#ifndef RDK_CALL
		#define RDK_CALL __cdecl
		#endif
	#elif defined(LIBRDK_LIBRARY_IMPORT)
		#ifndef RDK_LIB_TYPE
		#define RDK_LIB_TYPE __declspec(dllimport)
		#endif
		#ifndef RDK_CALL
		#define RDK_CALL __cdecl
		#endif
	#else
		#ifndef RDK_LIB_TYPE
		#define RDK_LIB_TYPE
		#endif
		#ifndef RDK_CALL
		#define RDK_CALL
		#endif
	#endif
#elif defined (QT_VERSION)
	#if defined(LIBRDK_LIBRARY_EXPORT)
		#ifndef RDK_LIB_TYPE
		#define RDK_LIB_TYPE Q_DECL_EXPORT
		#endif
		#ifndef RDK_CALL
		#define RDK_CALL
		#endif
	#elif defined(LIBRDK_LIBRARY_IMPORT)
		#ifndef RDK_LIB_TYPE
		#define RDK_LIB_TYPE Q_DECL_IMPORT
		#endif
		#ifndef RDK_CALL
		#define RDK_CALL
		#endif
	#else
		#ifndef RDK_LIB_TYPE
		#define RDK_LIB_TYPE
		#endif
		#ifndef RDK_CALL
		#define RDK_CALL
		#endif
	#endif
#elif defined(__GNUC__)
	#ifndef RDK_LIB_TYPE
	#define RDK_LIB_TYPE
	#endif
	#ifndef RDK_CALL
	#define RDK_CALL
	#endif
#elif defined(_MSC_VER)
    #if defined(LIBRDK_LIBRARY_EXPORT)
        #ifndef RDK_LIB_TYPE
        #define RDK_LIB_TYPE __declspec(dllexport)
        #endif
        #ifndef RDK_CALL
        #define RDK_CALL __cdecl
        #endif
    #elif defined(LIBRDK_LIBRARY_IMPORT)
        #ifndef RDK_LIB_TYPE
        #define RDK_LIB_TYPE __declspec(dllimport)
        #endif
        #ifndef RDK_CALL
        #define RDK_CALL __cdecl
        #endif
    #else
        #ifndef RDK_LIB_TYPE
        #define RDK_LIB_TYPE
        #endif
        #ifndef RDK_CALL
        #define RDK_CALL
        #endif
    #endif
#else
	#if defined(LIBRDK_LIBRARY_EXPORT)
		#ifndef RDK_LIB_TYPE
		#define RDK_LIB_TYPE __declspec(dllexport)
		#endif
		#ifndef RDK_CALL
		#define RDK_CALL __cdecl
		#endif
	#elif defined(LIBRDK_LIBRARY_IMPORT)
		#ifndef RDK_LIB_TYPE
		#define RDK_LIB_TYPE __declspec(dllimport)
		#endif
		#ifndef RDK_CALL
		#define RDK_CALL __cdecl
		#endif
	#else
		#ifndef RDK_LIB_TYPE
		#define RDK_LIB_TYPE
		#endif
		#ifndef RDK_CALL
		#define RDK_CALL
		#endif
	#endif
#endif
#endif // RDK_LIB_TYPE

#ifdef __GNUC__
#pragma GCC diagnostic pop
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

