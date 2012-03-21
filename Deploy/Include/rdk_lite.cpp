#ifndef RDK_LITE_CPP
#define RDK_LITE_CPP

#include "rdk_lite.h"

#include "../../ThirdParty/ThirdParty.cpp"
#include "../../Core/Math/UMath_lite.cpp"
#include "../../Core/Utilities/Utilities_lite.cpp"
#include "../../Core/Graphics/Graphics_lite.cpp"
#include "../../Core/Engine/Engine_lite.cpp"
#include "../../Core/Serialize/Serialize_lite.cpp"

namespace RDK {

// Создает пустое хранилище и возвращает указатель на него
UAStorage* NewUAStorage(void)
{
 return new UAStorage;
}

// Создает среду и возвращает указатель на нее
UAEnvironment* NewUAEnvironment(void)
{
 return new UAEnvironment;
}


// Создает пустое хранилище и возвращает указатель на него
UAStorage* CreateAStorage(UAStorage *storage)
{
 return storage;
}

// Создает среду и возвращает указатель на нее.
// Если задано хранилище 'storage', то связывает его со средой.
// Если флаг 'isinit' == true, то инициализирует хранилище стандартными библиотеками
// Если указатель на массив external_libs != 0, дополнительно инициализирует хранилище этими бибилиотеками
UAEnvironment* CreateAEnvironment(UAEnvironment *environment, UAStorage *storage, bool isinit, UALibrary** external_libs, int numlibs)
{
 UAEnvironment *env=environment;

 if(!storage)
  return env;

 if(!env->SetStorage(storage) || !isinit || !external_libs || !numlibs)
  return env;

 for(int i=0;i<numlibs;i++)
 {
  env->AddClassLibrary(external_libs[i]);
 }

 env->BuildStorage();

 return env;
}

}
#endif





