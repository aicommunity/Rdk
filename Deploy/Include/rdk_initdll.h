#ifndef rdk_initdllH
#define rdk_initdllH

#ifdef __BORLANDC__
#define RDK_LIB_TYPE __declspec(dllexport)
#define RDK_CALL __cdecl
#else
#define RDK_LIB_TYPE __declspec(dllexport)
#define RDK_CALL __cdecl
#endif
#include "rdk_init.h"

#endif
