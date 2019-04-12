#ifndef MMatrixBaseCPP
#define MMatrixBaseCPP

#include <cstring>
#include "MMatrixBase.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
MMatrixSize::MMatrixSize(void)
{

}

/*MMatrixSize::MMatrixSize(int rows)
{
 Dims.assign(1,rows);
} */

MMatrixSize::MMatrixSize(int rows, int cols)
{
 Dims.resize(2);
 Dims[0]=rows;
 Dims[1]=cols;
}
/*
MMatrixSize::MMatrixSize(size_t rows, size_t cols)
{
 Dims.resize(2);
 Dims[0]=int(rows);
 Dims[1]=int(cols);
}

MMatrixSize::MMatrixSize(unsigned rows, unsigned cols)
{
 Dims.resize(2);
 Dims[0]=int(rows);
 Dims[1]=int(cols);
}
*/
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

MMatrixSize& MMatrixSize::operator = (const std::vector<int> &copy)
{
 Dims=copy;
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

int MMatrixSize::operator [] (size_t i) const
{
 return Dims[i];
}

int& MMatrixSize::operator [] (size_t i)
{
 return Dims[i];
}

bool MMatrixSize::operator == (const MMatrixSize &copy) const
{
 return Dims == copy.Dims;
}

bool MMatrixSize::operator != (const MMatrixSize &copy) const
{
 return !((*this) == copy);
}

bool MMatrixSize::operator > (const MMatrixSize &copy) const
{
 if(Dims.size()>copy.Dims.size())
  return true;

 if(Dims.size()<copy.Dims.size())
  return false;

 int res1=1;
 int res2=1;
 for(size_t i=0;i<Dims.size();i++)
  res1*=Dims[i];
 for(size_t i=0;i<copy.Dims.size();i++)
  res2*=copy.Dims[i];

 return (res1>res2)?true:false;
}

bool MMatrixSize::operator < (const MMatrixSize &copy) const
{
 if(Dims.size()<copy.Dims.size())
  return true;

 if(Dims.size()>copy.Dims.size())
  return false;

 int res1=1;
 int res2=1;
 for(size_t i=0;i<Dims.size();i++)
  res1*=Dims[i];
 for(size_t i=0;i<copy.Dims.size();i++)
  res2*=copy.Dims[i];

 return (res1<res2)?true:false;}

bool MMatrixSize::operator >= (const MMatrixSize &copy) const
{
 if(Dims.size()>copy.Dims.size())
  return true;

 if(Dims.size()<copy.Dims.size())
  return false;

 int res1=1;
 int res2=1;
 for(size_t i=0;i<Dims.size();i++)
  res1*=Dims[i];
 for(size_t i=0;i<copy.Dims.size();i++)
  res2*=copy.Dims[i];

 return (res1>=res2)?true:false;
}

bool MMatrixSize::operator <= (const MMatrixSize &copy) const
{
 if(Dims.size()<copy.Dims.size())
  return true;

 if(Dims.size()>copy.Dims.size())
  return false;

 int res1=1;
 int res2=1;
 for(size_t i=0;i<Dims.size();i++)
  res1*=Dims[i];
 for(size_t i=0;i<copy.Dims.size();i++)
  res2*=copy.Dims[i];

 return (res1<=res2)?true:false;
}


// Возвращает размерность
int MMatrixSize::GetDimensions(void) const
{
 return int(Dims.size());
}

/// Возвращает сырые данные размерности
const std::vector<int> MMatrixSize::GetDims(void) const
{
 return Dims;
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

 if(!dest.Resize(GetMatrixSize()))
  return false;

 if(GetSize()>0)
  memcpy(dest.GetVoid(),GetVoid(),GetByteSize());
 return true;
}

}

#endif

