#ifndef RDK_QT_CPP
#define RDK_QT_CPP

#ifdef __GNUC__
#include "rdk.gcc.cpp"
#elif defined WIN32
#include "rdk.win.cpp"
#else
#include "rdk.ansi.cpp"
#endif

#endif
