#ifndef RDK_CPP_INIT_CPP
#define RDK_CPP_INIT_CPP

#include "rdk_cpp_init.h"
#include "rdk_init.cpp"

// ¬озвращает ссылку на указатель управл€ющего €дра
RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void)
{
 return DllManager.GetEngine();
}

RDK::UEPtr<RDK::UEngine> RDK_CALL GetEngine(int engine_index)
{
 return DllManager.GetEngine(engine_index);
}


// ¬озвращает ссылку на указатель среды выполнени€
RDK::UEPtr<RDK::UEnvironment>& RDK_CALL GetEnvironment(void)
{
 return DllManager.GetEnvironment();
}

RDK::UEPtr<RDK::UEnvironment> RDK_CALL GetEnvironment(int engine_index)
{
 return DllManager.GetEnvironment(engine_index);
}

// ¬озвращает ссылку на указатель хранилища
RDK::UEPtr<RDK::UStorage>& RDK_CALL GetStorage(void)
{
 return DllManager.GetStorage();
}

RDK::UEPtr<RDK::UStorage> RDK_CALL GetStorage(int engine_index)
{
 return DllManager.GetStorage(engine_index);
}

// ¬озвращает указатель на текущую модель
RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(void)
{
 return DllManager.GetModel();
}

RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(int engine_index)
{
 return DllManager.GetModel(engine_index);
}

#endif
