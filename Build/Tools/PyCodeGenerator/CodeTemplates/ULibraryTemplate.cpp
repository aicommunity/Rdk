#ifndef @NAMESPACE_NAME@_@CPP_FILE_NAME@
#define @NAMESPACE_NAME@_@CPP_FILE_NAME@

#include "@HEADER_FILE_NAME@"

namespace @NAMESPACE_NAME@ {

@CLASS_NAME@ @LIBRARY_NAME@;



// --------------------------
// Конструкторы и деструкторы
// --------------------------
@CLASS_NAME@::@CLASS_NAME@(void)
 : ULibrary("@LIBRARY_NAME@","1.0")
{
}
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
void @CLASS_NAME@::CreateClassSamples(UStorage *storage)
{
 UContainer *cont(0);
/*
 cont=new UStatisticMatrix<int>;
 cont->SetName("StatisticIntMatrix");
 cont->Default();
 UploadClass("UStatisticIntMatrix",cont);
*/
}
// --------------------------

}

#endif


