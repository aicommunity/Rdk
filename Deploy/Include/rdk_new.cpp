#ifndef RDK_NEW_CPP
#define RDK_NEW_CPP

#include "rdk.h"

/*****************************************************************************/
extern RDK::UStorage* CreateNewStorage(void)
{
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
