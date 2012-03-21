#ifndef RDK_LITE_H
#define RDK_LITE_H

#include "../../ThirdParty/ThirdParty.h"
#include "../../Core/Math/UMath_lite.h"
#include "../../Core/Utilities/Utilities_lite.h"
#include "../../Core/Graphics/Graphics_lite.h"
#include "../../Core/Engine/Engine_lite.h"
#include "../../Core/Serialize/Serialize_lite.h"

namespace RDK {

// Создает пустое хранилище и возвращает указатель на него
UAStorage* NewUAStorage(void);

// Создает среду и возвращает указатель на нее
UAEnvironment* NewUAEnvironment(void);

// Инициализирует пустое хранилище и возвращает указатель на него
UAStorage* CreateAStorage(UAStorage *storage);

// Инициализирует среду и возвращает указатель на нее.
// Если задано хранилище 'storage', то связывает его со средой.
// Если флаг 'isinit' == true, то инициализирует хранилище стандартными библиотеками
// Если указатель на массив external_libs != 0, дополнительно инициализирует хранилище этими бибилиотеками
UAEnvironment* CreateAEnvironment(UAEnvironment *environment, UAStorage *storage=0, bool isinit=true, UALibrary** external_libs=0, int numlibs=0);

}
#endif





