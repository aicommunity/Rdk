#ifndef RDK_CPP_INIT_H
#define RDK_CPP_INIT_H

#include "rdk_init.h"
#include "rdk.h"

extern "C++"  {

RDK_LIB_TYPE RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UAEnvironment>& RDK_CALL GetEnvironment(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UAStorage>& RDK_CALL GetStorage(void);
RDK_LIB_TYPE RDK::UEPtr<RDK::UAContainer> RDK_CALL GetModel(void);
}

#endif

