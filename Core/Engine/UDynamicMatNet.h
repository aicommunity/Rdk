//---------------------------------------------------------------------------

#ifndef UDynamicMatNetH
#define UDynamicMatNetH

#include "UDynamicNet.h"
//---------------------------------------------------------------------------

namespace RDK {

class UDynamicMatNet: public UDynamicNet<MDMatrix<double> >
{
public:
// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает размер вектора входов InputData по индексу
// Проверяет индекс на корректность и возвращает 0, если такого входа нет фактически
MMatrixSize GetInputDataSize(size_t index) const;

// Возвращает суммарный размер всех векторов входов
size_t GetFullInputDataSize(void) const;
// --------------------------

// ----------------------
// Методы управления выходными данными
// ----------------------
// Размер вектора выходных данных
inline MMatrixSize GetOutputDataSize(int index) const
{ return GetOutputData(index).GetMatrixSize(); };
virtual bool SetOutputDataSize(int index, const MMatrixSize &size, bool nobuild=false);

// Возвращает размер вектора выходных данных в байтах
inline size_t GetByteOutputDataSize(int index) const
{ return GetOutputData(index).GetRows()*GetOutputData(index).GetCols()*sizeof(double); };

// Заполняет заданный выходной вектор данными
void FillOutputData(int index, const void *data=0);

// Заполняет все выходные вектора заданными данными
void FillOutputData(const void *data=0);
// ----------------------


};

}
#endif
