#ifndef THIRD_PARTY_CPP
#define THIRD_PARTY_CPP

#ifndef _UNICODE
#define RDK_UNICODE_RUN
#define _UNICODE
#endif

#define _CRT_SECURE_NO_WARNINGS
#include "xmlParser/xmlParser.cpp"
#undef _CRT_SECURE_NO_WARNINGS

#ifdef RDK_UNICODE_RUN
#undef _UNICODE
#undef RDK_UNICODE_RUN
#endif

#endif
