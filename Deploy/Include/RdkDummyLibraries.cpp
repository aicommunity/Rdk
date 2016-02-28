#include "RdkDummyLibraries.h"

namespace RDK {

/// Функция должна быть реализована в конечном проекте и загружать требуемые библиотеки
bool RDK_CALL RdkLoadPredefinedLibraries(std::list<RDK::ULibrary*> &libs_list)
{
 return true;
}

/// Функция должна быть реализована в конечном проекте
bool RDK_CALL RdkCreatePredefinedStructure(RDK::UEnvironment* env, int predefined_structure)
{
 return true;
}

}

