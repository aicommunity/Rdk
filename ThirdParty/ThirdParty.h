#ifndef _UNICODE
//#define RDK_UNICODE_RUN
//#define _UNICODE
#endif

#ifdef RDK_UNICODE_RUN
#define XML_UNICODE_RUN
#endif

#define _USE_XMLPARSER_DLL
#define _CRT_SECURE_NO_WARNINGS
#include "xmlParser/xmlParser.h"
#undef _CRT_SECURE_NO_WARNINGS

#ifdef RDK_UNICODE_RUN
//#undef _UNICODE
//#undef RDK_UNICODE_RUN
#endif             

#ifdef __unix__
#include <curl/curl.h>
#else
#define CURL_STATICLIB
#include "curl/include/curl/curl.h"
#endif

#include "jpeg/JpegIntegration.h"

#ifdef RDK_USE_ODESOLVER
#include "ode-solver/include/OdeCpu.hpp"
#include "ode-solver/include/Ode.hpp"
#include "ode-solver/include/OdeSolverBase.hpp"
#include "ode-solver/include/OdeSolverFactory.hpp"
#endif
