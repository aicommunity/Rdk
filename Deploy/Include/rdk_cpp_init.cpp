#ifndef RDK_CPP_INIT_CPP
#define RDK_CPP_INIT_CPP

#include "rdk_cpp_init.h"
#include "rdk_init.cpp"

RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void)
{
 return PEngine;
}

RDK::UEPtr<RDK::UAEnvironment>& RDK_CALL GetEnvironment(void)
{
 return PEnvironment;
}

RDK::UEPtr<RDK::UAStorage>& RDK_CALL GetStorage(void)
{
 return PStorage;
}

RDK::UEPtr<RDK::UAContainer> RDK_CALL GetModel(void)
{
 return PEngine->GetModel();
}

#endif
