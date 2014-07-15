#ifndef INIT_DLL_DEFS_H
#define INIT_DLL_DEFS_H

#ifdef __BORLANDC__
	#ifdef LIBRDK_LIBRARY
		#define RDK_LIB_TYPE __declspec(dllexport)
		#define RDK_CALL __cdecl
	#else
		#define RDK_LIB_TYPE __declspec(dllimport)
		#define RDK_CALL __cdecl
	#endif
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
			#ifdef LIBRDK_LIBRARY
				#define RDK_LIB_TYPE __declspec(dllexport)
				#define RDK_CALL __cdecl
			#else
				#define RDK_LIB_TYPE __declspec(dllimport)
				#define RDK_CALL __cdecl
			#endif
		#endif
	#endif
#endif

#endif

