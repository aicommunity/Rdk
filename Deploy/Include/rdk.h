#ifndef RDK_H
#define RDK_H

#include "../../ThirdParty/ThirdParty.h"
#include "../../Core/Math/UMath.h"
#include "../../Core/Utilities/Utilities.h"
#include "../../Core/Serialize/Serialize.h"
#include "../../Core/Graphics/Graphics.h"
#include "../../Core/Engine/Engine.h"

namespace RDK {

// —оздает пустое хранилище и возвращает указатель на него
UAContainerStorage* NewUStorage(void);

// —оздает среду и возвращает указатель на нее
UAContainerEnvironment* NewUAContainerEnvironment(void);

// —оздает движок и возвращает указатель на него
UEngine* NewUEngine(void);

// »нициализирует пустое хранилище и возвращает указатель на него
UAContainerStorage* CreateStorage(UAContainerStorage *storage);

// »нициализирует среду и возвращает указатель на нее.
// ≈сли задано хранилище 'storage', то св€зывает его со средой.
// ≈сли флаг 'isinit' == true, то инициализирует хранилище стандартными библиотеками
// ≈сли указатель на массив external_libs != 0, дополнительно инициализирует хранилище этими бибилиотеками
UAContainerEnvironment* CreateEnvironment(UAContainerEnvironment *environment, UAContainerStorage *storage=0, bool isinit=true, list<UAContainer*>* external_classes=0, list<UALibrary*>* external_libs=0);

}
#endif





