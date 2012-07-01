#ifndef RDK_CPP_INIT_H
#define RDK_CPP_INIT_H

#include "rdk_init.h"
#include "rdk.h"

extern "C++"  {

// ¬озвращает ссылку на указатель управл€ющего €дра
RDK_LIB_TYPE RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void);

// ¬озвращает ссылку на указатель среды выполнени€
RDK_LIB_TYPE RDK::UEPtr<RDK::UAContainerEnvironment>& RDK_CALL GetEnvironment(void);

// ¬озвращает ссылку на указатель хранилища
RDK_LIB_TYPE RDK::UEPtr<RDK::UAContainerStorage>& RDK_CALL GetStorage(void);

// ¬озвращает указатель на текущую модель
RDK_LIB_TYPE RDK::UEPtr<RDK::UAContainer> RDK_CALL GetModel(void);
}

#endif

