#ifndef RDK_CPP
#define RDK_CPP

#include "rdk.h"

#include "myrdk.cpp"
#include "../../Core/Engine/Engine.cpp"
#include "../../Core/Graphics/Graphics.cpp"

namespace RDK {

// —оздает пустое хранилище и возвращает указатель на него
UAContainerStorage* NewUStorage(void)
{
 return new UAContainerStorage;
}

// —оздает среду и возвращает указатель на нее
UAContainerEnvironment* NewUAContainerEnvironment(void)
{
 return new UAContainerEnvironment;
}

// —оздает движок и возвращает указатель на него
UEngine* NewUEngine(void)
{
 return new UEngine;
}

// —оздает пустое хранилище и возвращает указатель на него
UAContainerStorage* CreateStorage(UAContainerStorage *storage)
{
 return storage;
}

// —оздает среду и возвращает указатель на нее.
// ≈сли задано хранилище 'storage', то св€зывает его со средой.
// ≈сли флаг 'isinit' == true, то инициализирует хранилище стандартными библиотеками
// ≈сли указатель на массив external_libs != 0, дополнительно инициализирует хранилище этими бибилиотеками
UAContainerEnvironment* CreateEnvironment(UAContainerEnvironment *environment, UAContainerStorage *storage, bool isinit, list<UAContainer*>* external_classes, list<UALibrary*>* external_libs)
{
 UAContainerEnvironment *env=environment;

 if(!storage)
  return env;

 if(!env->SetStorage(storage) || !isinit)
  return env;

 if(external_classes != 0)
 {
  list<UAContainer*>::iterator I,J;
  I=external_classes->begin();
  J=external_classes->end();
  while(I != J)
  {
   env->AddClass(*I);
   ++I;
  }
 }

 env->AddClassLibrary(&BCLLibrary);
 env->AddClassLibrary(&IOLibrary);

 if(external_libs != 0)
 {
  list<UALibrary*>::iterator I,J;
  I=external_libs->begin();
  J=external_libs->end();
  while(I != J)
  {
   env->AddClassLibrary(*I);
   ++I;
  }
 }

 env->BuildStorage();

 return env;
}

}
#endif





