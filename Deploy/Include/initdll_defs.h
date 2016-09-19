#ifndef INIT_DLL_DEFS_H
#define INIT_DLL_DEFS_H

#define RDK_CORE

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

#if defined(__BORLANDC__)
	#if defined(LIBRDK_LIBRARY_EXPORT)
		#define RDK_LIB_TYPE __declspec(dllexport)
		#define RDK_CALL __cdecl
	#elif defined(LIBRDK_LIBRARY_IMPORT)
		#define RDK_LIB_TYPE __declspec(dllimport)
		#define RDK_CALL __cdecl
	#else
		#define RDK_LIB_TYPE
		#define RDK_CALL
	#endif
#elif defined (QT_VERSION)
	#if defined(LIBRDK_LIBRARY_EXPORT)
		#define RDK_LIB_TYPE Q_DECL_EXPORT
		#define RDK_CALL
	#elif defined(LIBRDK_LIBRARY_IMPORT)
		#define RDK_LIB_TYPE Q_DECL_IMPORT
		#define RDK_CALL
	#else
		#define RDK_LIB_TYPE
		#define RDK_CALL
	#endif
#elif defined(__GNUC__)
	#define RDK_LIB_TYPE
	#define RDK_CALL
#elif defined(_MSC_VER)
    #if defined(LIBRDK_LIBRARY_EXPORT)
        #define RDK_LIB_TYPE __declspec(dllexport)
        #define RDK_CALL __cdecl
    #elif defined(LIBRDK_LIBRARY_IMPORT)
        #define RDK_LIB_TYPE __declspec(dllimport)
        #define RDK_CALL __cdecl
    #else
        #define RDK_LIB_TYPE
        #define RDK_CALL
    #endif
#else
	#if defined(LIBRDK_LIBRARY_EXPORT)
		#define RDK_LIB_TYPE __declspec(dllexport)
		#define RDK_CALL __cdecl
	#elif defined(LIBRDK_LIBRARY_IMPORT)
		#define RDK_LIB_TYPE __declspec(dllimport)
		#define RDK_CALL __cdecl
	#else
		#define RDK_LIB_TYPE
		#define RDK_CALL
	#endif
#endif

#endif

