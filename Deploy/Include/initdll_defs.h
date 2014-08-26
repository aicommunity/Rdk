#ifndef INIT_DLL_DEFS_H
#define INIT_DLL_DEFS_H

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

/*
#ifdef __BORLANDC__
	#ifdef LIBRDK_LIBRARY_EXPORT
		#define RDK_LIB_TYPE __declspec(dllexport)
		#define RDK_CALL __cdecl
	#else
		#ifdef LIBRDK_LIBRARY_IMPORT
			#define RDK_LIB_TYPE __declspec(dllimport)
			#define RDK_CALL __cdecl
		#else
			#define RDK_LIB_TYPE
			#define RDK_CALL
		#endif
	#endif
#else
	#ifdef QT_VERSION
		#ifdef LIBRDK_LIBRARY_EXPORT
			#define RDK_LIB_TYPE Q_DECL_EXPORT
			#define RDK_CALL
		#else
			#ifdef LIBRDK_LIBRARY_IMPORT
				#define RDK_LIB_TYPE Q_DECL_IMPORT
				#define RDK_CALL
			#else
				#define RDK_LIB_TYPE
				#define RDK_CALL
			#endif
		#endif
	#else
		#ifdef __GNUC__
			#define RDK_LIB_TYPE
			#define RDK_CALL
		#else
			#ifdef LIBRDK_LIBRARY_EXPORT
				#define RDK_LIB_TYPE __declspec(dllexport)
				#define RDK_CALL __cdecl
			#else
				#ifdef LIBRDK_LIBRARY_IMPORT
					#define RDK_LIB_TYPE __declspec(dllimport)
					#define RDK_CALL __cdecl
				#else
					#define RDK_LIB_TYPE
					#define RDK_CALL
				#endif
			#endif
		#endif
	#endif
#endif

*/

#endif

