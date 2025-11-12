#ifndef RDK_NEW_CPP
#define RDK_NEW_CPP

#include "rdk.h"

#ifdef __GNUC__
namespace RDK {
void RdkDummyFunc(void);
}
#endif
/*****************************************************************************/
extern std::unique_ptr<RDK::UStorage> CreateNewStorage(void)
{
#ifdef __GNUC__
// RDK::RdkDummyFunc();
#endif
 return std::make_unique<RDK::UStorage>();
}

extern std::unique_ptr<RDK::UEnvironment> CreateNewEnvironment(void)
{
 return std::make_unique<RDK::UEnvironment>();
}

extern std::unique_ptr<RDK::UEngine> CreateNewEngine(void)
{
 return std::make_unique<RDK::UEngine>();
}
/*****************************************************************************/
#endif
