#ifndef Curl_GLOBAL_H
#define Curl_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBRDK_LIBRARY)
#  define LIBRDKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBRDKSHARED_EXPORT Q_DECL_IMPORT
#endif

#include "../../../../Rdk/Deploy/Include/rdk_cpp_initdll.h"

#endif // Rdk-BasicLib_GLOBAL_H
