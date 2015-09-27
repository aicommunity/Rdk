#ifndef MMatrixBaseCPP
#define MMatrixBaseCPP

#include "MMatrixBase.h"

namespace RDK {

// --------------------------
// ������������ � �����������
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
// ���������
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

// ���������� �����������
int MMatrixSize::GetDimensions(void) const
{
 return int(Dims.size());
}
// --------------------------


	 /*
// ����� ���������� ������� �� ���� ���������
bool MMatrixBase::CompareLanguageType(const MMatrixBase &dt) const
{
 return GetLanguageType() == dt.GetLanguageType();
}    */

/// �������� ������ � ������ �������
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
