#ifndef rdk_initdllH
#define rdk_initdllH

#ifdef BORLAND
#define RDK_LIB_TYPE __declspec(dllexport)
#else
#define RDK_LIB_TYPE __declspec(dllexport)
#endif
#include "rdk_init.h"

#endif
