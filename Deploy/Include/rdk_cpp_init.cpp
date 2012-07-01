#ifndef RDK_CPP_INIT_CPP
#define RDK_CPP_INIT_CPP

#include "rdk_cpp_init.h"
#include "rdk_init.cpp"

// ¬озвращает ссылку на указатель управл€ющего €дра
RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void)
{
 return PEngine;
}

// ¬озвращает ссылку на указатель среды выполнени€
RDK::UEPtr<RDK::UAContainerEnvironment>& RDK_CALL GetEnvironment(void)
{
 return PEnvironment;
}

// ¬озвращает ссылку на указатель хранилища
RDK::UEPtr<RDK::UAContainerStorage>& RDK_CALL GetStorage(void)
{
 return PStorage;
}

// ¬озвращает указатель на текущую модель
RDK::UEPtr<RDK::UAContainer> RDK_CALL GetModel(void)
{
 return PEngine->GetModel();
}

#endif
