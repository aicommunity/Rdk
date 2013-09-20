#ifndef RDK_CPP_INIT_CPP
#define RDK_CPP_INIT_CPP

#include "rdk_cpp_init.h"
#include "rdk_init.cpp"

// ¬озвращает ссылку на указатель управл€ющего €дра
RDK::UEPtr<RDK::UEngine>& RDK_CALL GetEngine(void)
{
 return PEngine;
}

RDK::UEPtr<RDK::UEngine> RDK_CALL GetEngine(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 return DllManager.EngineList[engine_index];
}


// ¬озвращает ссылку на указатель среды выполнени€
RDK::UEPtr<RDK::UEnvironment>& RDK_CALL GetEnvironment(void)
{
 return PEnvironment;
}

RDK::UEPtr<RDK::UEnvironment> RDK_CALL GetEnvironment(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 UEPtr<UEngine> engine=DllManager.EngineList[engine_index];
 if(engine)
  return engine->GetEnvironment();

 return 0;
}

// ¬озвращает ссылку на указатель хранилища
RDK::UEPtr<RDK::UStorage>& RDK_CALL GetStorage(void)
{
 return PStorage;
}

RDK::UEPtr<RDK::UStorage> RDK_CALL GetStorage(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 UEPtr<UEngine> engine=DllManager.EngineList[engine_index];
 if(engine)
 {
  UEPtr<UEnvironment> environment=DllManager.EngineList[engine_index]->GetEnvironment();
  if(environment)
   return environment->GetStorage();
 }

 return 0;
}

// ¬озвращает указатель на текущую модель
RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(void)
{
 return PEngine->GetModel();
}

RDK::UEPtr<RDK::UContainer> RDK_CALL GetModel(int engine_index)
{
 if(engine_index<0 || engine_index>=GetNumEngines())
  return 0;

 UEPtr<UEngine> engine=DllManager.EngineList[engine_index];
 if(engine)
 {
  UEPtr<UEnvironment> environment=DllManager.EngineList[engine_index]->GetEnvironment();
  if(environment)
   return environment->GetModel();
 }

 return 0;
}

#endif
