#ifndef @NAMESPACE_NAME@_@HEADER_FILE_NAME@
#define @NAMESPACE_NAME@_@HEADER_FILE_NAME@

#include "../../Rdk/Deploy/Include/rdk.h"

namespace @NAMESPACE_NAME@ {

class RDK_LIB_TYPE @CLASS_NAME@: public RDK::ULibrary
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
@CLASS_NAME@(void);
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------
};

extern RDK_LIB_TYPE @CLASS_NAME@ @LIBRARY_NAME@;

}

#endif


