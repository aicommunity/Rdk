#ifndef RDK_NEW_CPP
#define RDK_NEW_CPP

#include "rdk.h"

/*****************************************************************************/
extern RDK::UAStorage* CreateNewStorage(void)
{
 return new RDK::UBAStorage;
}

extern RDK::UAEnvironment* CreateNewEnvironment(void)
{
 return new RDK::UBAEnvironment;
}

extern RDK::UEngine* CreateNewEngine(void)
{
 return new RDK::UBEngine;
}
/*****************************************************************************/
#endif
