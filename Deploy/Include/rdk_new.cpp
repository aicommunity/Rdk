#ifndef RDK_NEW_CPP
#define RDK_NEW_CPP

#include "rdk.h"

#ifdef __GNUC__
namespace RDK {
void RdkDummyFunc(void);
}
#endif
/*****************************************************************************/
extern RDK::UStorage* CreateNewStorage(void)
{
#ifdef __GNUC__
 RDK::RdkDummyFunc();
#endif
 return new RDK::UStorage;
}

extern RDK::UEnvironment* CreateNewEnvironment(void)
{
 return new RDK::UEnvironment;
}

extern RDK::UEngine* CreateNewEngine(void)
{
 return new RDK::UEngine;
}
/*****************************************************************************/
#endif
