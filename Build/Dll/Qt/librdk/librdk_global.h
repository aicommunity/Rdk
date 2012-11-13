#ifndef LIBRDK_GLOBAL_H
#define LIBRDK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBRDK_LIBRARY)
#  define LIBRDKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBRDKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBRDK_GLOBAL_H
