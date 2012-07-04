#ifndef RDK_NEW_CPP
#define RDK_NEW_CPP

#include "rdk.h"

/*****************************************************************************/
extern RDK::UAContainerStorage* CreateNewStorage(void)
{
 return new RDK::UBAStorage;
}

extern RDK::UAContainerEnvironment* CreateNewEnvironment(void)
{
 return new RDK::UBAEnvironment;
}

extern RDK::UEngine* CreateNewEngine(void)
{
 return new RDK::UBEngine;
}
/*****************************************************************************/
#endif
