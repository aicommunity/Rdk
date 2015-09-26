#ifndef MMatrixBaseCPP
#define MMatrixBaseCPP

#include "MMatrixBase.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
MMatrixSize::MMatrixSize(void)
{

}

MMatrixSize::MMatrixSize(int rows)
{
 Dims.assign(1,rows);
}

MMatrixSize::MMatrixSize(int rows, int cols)
{
 Dims.resize(2);
 Dims[0]=rows;
 Dims[1]=cols;
}

MMatrixSize::MMatrixSize(const MMatrixSize &copy)
{
 Dims=copy.Dims;
}

MMatrixSize::MMatrixSize(const std::vector<int> &dims)
{
 Dims=dims;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
MMatrixSize& MMatrixSize::operator = (const MMatrixSize &copy)
{
 Dims=copy.Dims;
 return *this;
}

int MMatrixSize::operator [] (int i) const
{
 return Dims[i];
}

int& MMatrixSize::operator [] (int i)
{
 return Dims[i];
}

// Возвращает размерность
int MMatrixSize::GetDimensions(void) const
{
 return int(Dims.size());
}
// --------------------------


	 /*
// Метод сравнивает матрицы по типу элементов
bool MMatrixBase::CompareLanguageType(const MMatrixBase &dt) const
{
 return GetLanguageType() == dt.GetLanguageType();
}    */

/// Копирует данные в другую матрицу
bool MMatrixBase::CopyTo(MMatrixBase &dest) const
{
 if(GetElementByteSize() != dest.GetElementByteSize())
  return false;

 if(!dest.SetMatrixSize(GetMatrixSize()))
  return false;

 if(GetSize()>0)
  memcpy(dest.GetVoid(),GetVoid(),GetByteSize());
 return true;
}

}

#endif

