#ifndef @NAMESPACE_NAME@_@UPP@CLASS_NAME@_H
#define @NAMESPACE_NAME@_@UPP@CLASS_NAME@_H

#include "../../../Rdk/Deploy/Include/rdk.h"
//open file here

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
virtual void CreateClassSamples(RDK::UStorage *storage);
// --------------------------
};

extern RDK_LIB_TYPE @CLASS_NAME@ @LIBRARY_NAME@;

}

#endif


