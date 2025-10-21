#ifndef MMatrixBaseH
#define MMatrixBaseH

#include <typeinfo>
#include <vector>
#include "../Utilities/UException.h"

namespace RDK {

// ����������
/// ����� ���������� ��������� ��������
class EMatrixError: public EError
{
public:
EMatrixError(void) : EError() {}
};

/// ������� ������������. ���������� ������������ ��� ��������� ������ � �.�.
class EMatrixZeroDet: public EMatrixError
{
public:
EMatrixZeroDet(void) : EMatrixError() {}
};

/// ������� ������� �� ����
class EMatrixZeroDiv: public EMatrixError
{
public:
EMatrixZeroDiv(void) : EMatrixError() {}
};

/// Domain error
class EMatrixDomainError: public EMatrixError
{
public:
EMatrixDomainError(void) : EMatrixError() {}
};

class RDK_LIB_TYPE MMatrixSize
{
private: // ������
std::vector<int> Dims;
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MMatrixSize(void);
//MMatrixSize(int rows);
MMatrixSize(int rows, int cols);
//MMatrixSize(size_t rows, size_t cols);
//MMatrixSize(unsigned rows, unsigned cols);

template<class T1, class T2>
MMatrixSize(T1 rows, T2 cols)
{
 Dims.resize(2);
 Dims[0]=static_cast<int>(rows);
 Dims[1]=static_cast<int>(cols);
}

MMatrixSize(const MMatrixSize &copy);
explicit MMatrixSize(const std::vector<int> &dims);
// --------------------------

// --------------------------
// ���������
// --------------------------
MMatrixSize& operator = (const MMatrixSize &copy);
MMatrixSize& operator = (const std::vector<int> &copy);

int operator [] (int i) const;
int& operator [] (int i);
int operator [] (size_t i) const;
int& operator [] (size_t i);

bool operator == (const MMatrixSize &copy) const;
bool operator != (const MMatrixSize &copy) const;
bool operator > (const MMatrixSize &copy) const;
bool operator < (const MMatrixSize &copy) const;
bool operator >= (const MMatrixSize &copy) const;
bool operator <= (const MMatrixSize &copy) const;

// ���������� �����������
int GetDimensions(void) const;

/// ���������� ����� ������ �����������
const std::vector<int> GetDims(void) const;
// --------------------------
};

class RDK_LIB_TYPE MMatrixBase
{
public: // ������

public: // ������
// -----------------------------------
// ����� ������ ������� � ���������� �������
// -----------------------------------
/// ���������� ����������� �������
virtual int GetDimensions(void) const=0;

/// ���������� ����� ��������� �� ������� �������
virtual int GetSize(int i) const=0;

/// ���������� ��������� ����� ���� ���������
virtual int GetSize(void) const=0;

/// ���������� true ���� ��������� ����� ���� ��������� == 0
virtual bool IsEmpty(void) const=0;

/// ���������� ����� ��������� �� ���� ������������
virtual MMatrixSize GetMatrixSize(void) const=0;

/// ������������� ����� ��������� �� ���� ������������
virtual bool Resize(const MMatrixSize &size)=0;

/// ���������� ��������� ������ ������ ������� � ������
virtual int GetByteSize(void) const=0;

/// ���������� ����� � ������ ������ �������� �����
virtual int GetElementByteSize(void) const=0;

/// ������ ������� � ������
virtual const void* GetVoid(void) const=0;
virtual void* GetVoid(void)=0;

// ���������� �������� ��� �������� �������
//virtual const type_info& GetLanguageType(void) const=0;

// ����� ���������� ������� �� ���� ���������
//bool CompareLanguageType(const MMatrixBase &dt) const;

/// �������� ������ � ������ �������
virtual bool CopyTo(MMatrixBase &dest) const;


/// ������������� ������ � ������ ������� ��� � ����������� ������� ����������
/// ����. �����������!
template<typename U>
U& As(int i)
{
 return reinterpret_cast<U*>(GetVoid())[i];
}


template<typename U>
const U& As(int i) const
{
 return reinterpret_cast<U*>(GetVoid())[i];
}
// -----------------------------------
};


}
#endif
