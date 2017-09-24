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
// ������ ������� � ������
// --------------------------
// ���������� ������ ������� ������ InputData �� �������
// ��������� ������ �� ������������ � ���������� 0, ���� ������ ����� ��� ����������
MMatrixSize GetInputDataSize(size_t index) const;

// ���������� ��������� ������ ���� �������� ������
size_t GetFullInputDataSize(void) const;
// --------------------------

// ----------------------
// ������ ���������� ��������� �������
// ----------------------
// ������ ������� �������� ������
inline MMatrixSize GetOutputDataSize(int index) const
{ return GetOutputData(index).GetMatrixSize(); };
virtual bool SetOutputDataSize(int index, const MMatrixSize &size, bool nobuild=false);

// ���������� ������ ������� �������� ������ � ������
inline size_t GetByteOutputDataSize(int index) const
{ return GetOutputData(index).GetRows()*GetOutputData(index).GetCols()*sizeof(double); };

// ��������� �������� �������� ������ �������
void FillOutputData(int index, const void *data=0);

// ��������� ��� �������� ������� ��������� �������
void FillOutputData(const void *data=0);
// ----------------------


};

}
#endif
