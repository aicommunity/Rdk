/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2012.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information

Source of the matrix inversion based by code which got from
http://chi3x10.wordpress.com/2008/05/28/calculate-matrix-inversion-in-c/
http://www.rsdn.ru/forum/src/580658.1.aspx
http://iproc.ru/parallel-programming/lection-4/
*********************************************************** */
#ifndef MMatrixH
#define MMatrixH

#include "MDMatrix.h"

namespace RDK{

template<class T, unsigned Rows, unsigned Cols=Rows>
class MMatrix: public MMatrixBase
{
public:
// ������ �������
union
{
 T Data[Rows][Cols];
/* T Data[1];
 struct
 {
  T x,y,z,d;
 };*/
 double *Double;
 int *Int;
 unsigned char *UChar;
 char* Char;
 void* Void;
 void **PVoid;
};

protected:
// ������ �������
int VRows;
int VCols;

public:
// --------------------------
// ������������ � �����������
// --------------------------
MMatrix(void);
explicit MMatrix(T defvalue);
explicit MMatrix(const MDMatrix<T> &copy);
MMatrix(const MMatrix<T,Rows,Cols> &copy);
explicit MMatrix(const T data[Rows][Cols]);
explicit MMatrix(const T* data);
MMatrix(T v1, T v2, T v3);
MMatrix(T v1, T v2, T v3, T v4);
virtual ~MMatrix(void);
// --------------------------

// -----------------------------------
// ����� ������ ������� � ���������� �������
// -----------------------------------
/// ���������� ����������� �������
virtual int GetDimensions(void) const;

/// ���������� ����� ��������� �� ������� �������
virtual int GetSize(int i) const;

/// ���������� ��������� ����� ���� ���������
virtual int GetSize(void) const;

/// ���������� true ���� ��������� ����� ���� ��������� == 0
virtual bool IsEmpty(void) const;

/// ���������� ����� ��������� �� ���� ������������
virtual MMatrixSize GetMatrixSize(void) const;

/// ������������� ����� ��������� �� ���� ������������
virtual bool Resize(const MMatrixSize &size);

/// ���������� ��������� ������ ������ ������� � ������
virtual int GetByteSize(void) const;

/// ���������� ����� � ������ ������ �������� �����
virtual int GetElementByteSize(void) const;

/// ������ ������� � ������
virtual const void* GetVoid(void) const;
virtual void* GetVoid(void);

/// ������ ������� � ������
virtual const T* GetData(void) const;
virtual T* GetData(void);
// ���������� �������� ��� �������� �������
//virtual const type_info& GetLanguageType(void) const;
// -----------------------------------

// --------------------------
// ��������� ���������� �������
// --------------------------
// �������� ������������
MMatrix<T,Rows,Cols>& operator = (const MMatrix<T,Rows,Cols> &copy);
MMatrix<T,Rows,Cols>& operator = (const MDMatrix<T> &copy);
MMatrix<T,Rows,Cols>& operator = (T value);
MMatrix<T,Rows,Cols>& operator = (const T data[Rows][Cols]);
MMatrix<T,Rows,Cols>& operator = (const T* data);

void Assign(const T *data);
void Assign(T value);
void Assign(const MDMatrix<T> &copy);

/// �������� ������ � ��������� ������
void CopyTo(T data[Rows][Cols]);

/// �������� ������ � ���������� ������
bool CopyTo(T* data);
/// �������� ������ �� ���������� �������
void CopyFrom(T data[Rows][Cols]);
/// �������� ������ �� ����������� �������
bool CopyFrom(T* data);

/// �������� ������ � ������ �������
virtual bool CopyTo(MMatrixBase &dest) const;

// ��������� ����������� ������
unsigned GetCols(void) const;
unsigned GetRows(void) const;

// ������ � ��������
T& operator [] (int i);
const T& operator [] (int i) const;

T& operator () (int i, int j);
const T& operator () (int i, int j) const;

// ���������� �������� ������ �������
MMatrix<T,Cols,1> GetRow(int i) const;

// ���������� �������� ������� �������
MMatrix<T,Rows,1> GetCol(int i) const;

/// ������������� ������ � ������ ������� ��� � ����������� ������� ����������
/// ����. �����������!
template<typename U>
U& As(int i)
{
 return reinterpret_cast<U*>(Data)[i];
};


template<typename U>
const U& As(int i) const
{
 return reinterpret_cast<U*>(Data)[i];
};
// --------------------------

// --------------------------
// ��������� ���������
// --------------------------
MMatrix<T,Rows,Cols>& operator += (const MMatrix<T,Rows,Cols> &M);
MMatrix<T,Rows,Cols>& operator -= (const MMatrix<T,Rows,Cols> &M);
// --------------------------

// --------------------------
// ��������� ���������
// --------------------------
MMatrix<T,Rows,Cols> operator - (void) const;

MMatrix<T,Rows,Cols>& operator *= (T v);

MMatrix<T,Rows,Cols>& operator /= (T v);

MMatrix<T,Rows,Cols>& operator += (T v);

MMatrix<T,Rows,Cols>& operator -= (T v);
// --------------------------

// --------------------------
// �������������� ������
// --------------------------
// ����������������
MMatrix<T,Cols,Rows>& Transpose(MMatrix<T,Cols,Rows> &res) const;
MMatrix<T,Cols,Rows> Transpose(void) const;

// ���������� ������� � ������� ����������� �����
// ���������� ����� ������������
unsigned TriangleGauss(void);
unsigned TriangleBareis(void);

// ��������������
MMatrix<T,Rows,Cols>& Inverse(MMatrix<T,Cols,Rows> &res) const;
MMatrix<T,Rows,Cols> Inverse(void) const;

// �����������
T Det3x3(void) const;
T Det(void) const;

// ���������� �������� �������
MMatrix<T,Rows-1,Cols-1>& GetMinor(MMatrix<T,Rows-1,Cols-1> &res, unsigned row, unsigned col) const;
MMatrix<T,Rows-1,Cols-1> GetMinor(unsigned row, unsigned col) const;


// �������� ����� �������
template<unsigned row_beg, unsigned row_end, unsigned col_beg, unsigned col_end>
MMatrix<T,row_end-row_beg+1,col_end-col_beg+1>& Split(MMatrix<T,row_end-row_beg+1,col_end-col_beg+1> &res) const
{
	for(int i=row_beg;i<=row_end;i++)
		for(int j=col_beg;j<=col_end;j++)
			res(i-row_beg,j-col_beg)=(*this)(i,j);
	return res;
}

template<unsigned row_beg, unsigned row_end, unsigned col_beg, unsigned col_end>
MMatrix<T,row_end-row_beg+1,col_end-col_beg+1> Split() const
{
 MMatrix<T,row_end-row_beg+1,col_end-col_beg+1> res;
 return Split<row_beg, row_end, col_beg, col_end>(res);
}




// ����
T Trace(void) const;

// ����� �������
T operator !(void) const;

// ������������ �������
MMatrix<T,Rows,Cols>& Normalize(void);
// --------------------------

// --------------------------
// ����������� ������
// --------------------------
// ������� �������
static MMatrix<T,Rows,Cols> Zero(void);

// ��������� �������
static MMatrix<T,Rows,Cols> Eye(void);

// ���������� ������� ������� � 0
void ToZero(void);

// ���������� ������� ������� � ���������
void ToEye(void);
// --------------------------

// --------------------------
// ��������� ������
// --------------------------
bool operator == (const MMatrix<T,Rows,Cols> &M) const;
bool operator != (const MMatrix<T,Rows,Cols> &M) const;

void Print(std::ostream &stream);
};

// --------------------------
// ������������ � �����������
// --------------------------
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(void)
{
 VRows=Rows;
 VCols=Cols;
 memset(Data,0,sizeof(Data));
};

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(T defvalue)
{
 VRows=Rows;
 VCols=Cols;
 if(defvalue == 0)
  memset(Data,0,Rows*Cols*sizeof(T));
 else
 {
  T *data=&Data[0][0];
  for(unsigned i=0;i<Rows*Cols;i++,data++)
   *data=defvalue;
 }
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(const MMatrix<T,Rows,Cols> &copy)
{
 *this=copy;
};

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(const T data[Rows][Cols])
{
 *this=data;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(const MDMatrix<T> &copy)
{
 *this=copy;
}


template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(const T* data)
{ *this=data; };

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(T v1, T v2, T v3)
{
 double* PData=&Data[0][0];

 if(Rows*Cols>0)
  PData[0]=v1;

 if(Rows*Cols>1)
  PData[1]=v2;

 if(Rows*Cols>2)
  PData[2]=v3;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(T v1, T v2, T v3, T v4)
{
 double* PData=&Data[0][0];

 if(Rows*Cols>0)
  PData[0]=v1;

 if(Rows*Cols>1)
  PData[1]=v2;

 if(Rows*Cols>2)
  PData[2]=v3;

 if(Rows*Cols>3)
  PData[3]=v4;
}


template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::~MMatrix(void){};
// --------------------------


// -----------------------------------
// ����� ������ ������� � ���������� �������
// -----------------------------------
/// ���������� ����������� �������
template<class T, unsigned Rows, unsigned Cols>
int MMatrix<T,Rows,Cols>::GetDimensions(void) const
{
 return 2;
}

/// ���������� ����� ��������� �� ������� �������
template<class T, unsigned Rows, unsigned Cols>
int MMatrix<T,Rows,Cols>::GetSize(int i) const
{
 return (i==0)?Rows:((i==1)?Cols:0);
}

/// ���������� ��������� ����� ���� ���������
template<class T, unsigned Rows, unsigned Cols>
int MMatrix<T,Rows,Cols>::GetSize(void) const
{
 return Rows*Cols;
}

/// ���������� true ���� ��������� ����� ���� ��������� == 0
template<class T, unsigned Rows, unsigned Cols>
bool MMatrix<T,Rows,Cols>::IsEmpty(void) const
{
 return (!Rows || !Cols);
}

/// ���������� ����� ��������� �� ���� ������������
template<class T, unsigned Rows, unsigned Cols>
MMatrixSize MMatrix<T,Rows,Cols>::GetMatrixSize(void) const
{
 MMatrixSize size(Rows,Cols);
 return size;
}

/// ������������� ����� ��������� �� ���� ������������
template<class T, unsigned Rows, unsigned Cols>
bool MMatrix<T,Rows,Cols>::Resize(const MMatrixSize &size)
{
 if(size.GetDimensions() != 2)
  return false;

 return (unsigned(size[0]) == Rows) && (unsigned(size[1]) == Cols);
}

/// ���������� ��������� ������ ������ ������� � ������
template<class T, unsigned Rows, unsigned Cols>
int MMatrix<T,Rows,Cols>::GetByteSize(void) const
{
 return Rows*Cols*sizeof(T);
}

/// ���������� ����� � ������ ������ �������� �����
template<class T, unsigned Rows, unsigned Cols>
int MMatrix<T,Rows,Cols>::GetElementByteSize(void) const
{
 return sizeof(T);
}
   /*
// ���������� �������� ��� �������� �������
template<class T>
const type_info& MDMatrix<T>::GetLanguageType(void) const
{
 return typeid(T);
}  */

/// ������ ������� � ������
template<class T, unsigned Rows, unsigned Cols>
const void* MMatrix<T,Rows,Cols>::GetVoid(void) const
{
 return Void;
}

template<class T, unsigned Rows, unsigned Cols>
void* MMatrix<T,Rows,Cols>::GetVoid(void)
{
 return Void;
}

/// ������ ������� � ������
template<class T, unsigned Rows, unsigned Cols>
const T* MMatrix<T,Rows,Cols>::GetData(void) const
{
 return &Data[0][0];
}

template<class T, unsigned Rows, unsigned Cols>
T* MMatrix<T,Rows,Cols>::GetData(void)
{
 return &Data[0][0];
}

// -----------------------------------

// --------------------------
// ��������� ���������� �������
// --------------------------
// �������� ������������
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator = (const MMatrix<T,Rows,Cols> &copy)
{
 if (&copy == this)
  return *this;

 VRows=copy.VRows;
 VCols=copy.VCols;
 memcpy(Data,copy.Data,sizeof(T)*Cols*Rows);
 return *this;
};

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator = (const MDMatrix<T> &copy)
{
 if(Rows == copy.GetRows() && Cols == copy.GetCols())
 {
  memcpy(Data,copy.Data,sizeof(T)*Cols*Rows);
  VRows=copy.GetRows();
  VCols=copy.GetCols();
 }
 return *this;
}


template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator = (T value)
{
 T* pm1=&Data[0][0];

 for(unsigned i=0;i<Cols*Rows;i++)
  *pm1++ = value;
 return *this;
};

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator = (const T data[Rows][Cols])
{
 memcpy(Data,data,sizeof(Data));
 return *this;
};

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator = (const T* data)
{
 T* pm1=&Data[0][0];
 for(unsigned i=0;i<Cols*Rows;i++)
  *pm1++ = *data++;
 return *this;
}

template<class T, unsigned Rows, unsigned Cols>
void MMatrix<T,Rows,Cols>::Assign(const T *data)
{
 *this=data;
}

template<class T, unsigned Rows, unsigned Cols>
void MMatrix<T,Rows,Cols>::Assign(T value)
{
 *this=value;
}

template<class T, unsigned Rows, unsigned Cols>
void MMatrix<T,Rows,Cols>::Assign(const MDMatrix<T> &copy)
{
 *this=copy;
}

/// �������� ������ � ��������� ������
template<class T, unsigned Rows, unsigned Cols>
void MMatrix<T,Rows,Cols>::CopyTo(T data[Rows][Cols])
{
 memcpy(data,Data,sizeof(Data));
}

/// �������� ������ � ���������� ������
template<class T, unsigned Rows, unsigned Cols>
bool MMatrix<T,Rows,Cols>::CopyTo(T* data)
{
 if(!data)
  return false;
 memcpy(data,Data,sizeof(Data));
 return true;
}
/// �������� ������ �� ���������� �������
template<class T, unsigned Rows, unsigned Cols>
void MMatrix<T,Rows,Cols>::CopyFrom(T data[Rows][Cols])
{
 memcpy(Data,data,sizeof(Data));
}

/// �������� ������ �� ����������� �������
template<class T, unsigned Rows, unsigned Cols>
bool MMatrix<T,Rows,Cols>::CopyFrom(T* data)
{
 if(!data)
  return false;
 memcpy(Data,data,sizeof(Data));
 return true;
}

/// �������� ������ � ������ �������
template<class T, unsigned Rows, unsigned Cols>
bool MMatrix<T,Rows,Cols>::CopyTo(MMatrixBase &dest) const
{
 return MMatrixBase::CopyTo(dest);
}

// ��������� ����������� ������
template<class T, unsigned Rows, unsigned Cols>
unsigned MMatrix<T,Rows,Cols>::GetCols(void) const
{
 return Cols;
}

template<class T, unsigned Rows, unsigned Cols>
unsigned MMatrix<T,Rows,Cols>::GetRows(void) const
{
 return Rows;
}

// ������ � ��������
template<class T, unsigned Rows, unsigned Cols>
T& MMatrix<T,Rows,Cols>::operator [] (int i)
{
 return reinterpret_cast<T*>(Data)[i];
}

template<class T, unsigned Rows, unsigned Cols>
const T& MMatrix<T,Rows,Cols>::operator [] (int i) const
{
 return *(reinterpret_cast<const T*>(Data)+i);
}

template<class T, unsigned Rows, unsigned Cols>
T& MMatrix<T,Rows,Cols>::operator () (int i, int j)
{
 return Data[i][j];
}

template<class T, unsigned Rows, unsigned Cols>
const T& MMatrix<T,Rows,Cols>::operator () (int i, int j) const
{
 return Data[i][j];
}

// ���������� �������� ������ �������
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Cols,1> MMatrix<T,Rows,Cols>::GetRow(int i) const
{
 MMatrix<T,Cols,1> res;
 for(int j=0;j<Cols;j++)
  res.Data[j][0]=Data[i][j];
 return res;
}

// ���������� �������� ������� �������
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,1> MMatrix<T,Rows,Cols>::GetCol(int i) const
{
 MMatrix<T,Rows,1> res;
 for(int j=0;j<Rows;j++)
  res.Data[j][0]=Data[j][i];
 return res;
}
// --------------------------

// --------------------------
// ��������� ���������
// --------------------------
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator += (const MMatrix<T,Rows,Cols> &M)
{
 T* pm1=Data;
 const T* pm2=M.Data;

 for(int i=0;i<Cols*Rows;i++)
  *pm1++ += *pm2++;

 return *this;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator + (const MMatrix<T,Rows,Cols> &M1, const MMatrix<T,Rows,Cols> &M2)
{
 MMatrix<T,Rows,Cols> res=M1;
 T* pm1=&res.Data[0][0];
 const T* pm2=&M2.Data[0][0];

 for(unsigned i=0;i<Cols*Rows;i++)
  *pm1++ += *pm2++;

 return res;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator -= (const MMatrix<T,Rows,Cols> &M)
{
 T* pm1=Data[0];
 const T* pm2=M.Data[0];

 for(unsigned i=0;i<Cols*Rows;i++)
  *pm1++ -= *pm2++;

 return *this;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator - (const MMatrix<T,Rows,Cols> &M1, const MMatrix<T,Rows,Cols> &M2)
{
 MMatrix<T,Rows,Cols> res=M1;
 T* pm1=&res.Data[0][0];
 const T* pm2=&M2.Data[0][0];

 for(unsigned i=0;i<Cols*Rows;i++)
  *pm1++ -= *pm2++;

 return res;
}

template<class T, unsigned Rows, unsigned Cols, unsigned Cols2>
MMatrix<T,Rows,Cols2> operator * (const MMatrix<T,Rows,Cols> &M1, const MMatrix<T,Cols,Cols2> &M2)
{
 MMatrix<T,Rows,Cols2> res;

 for(unsigned j=0;j<Cols2;j++)
 {
  for(unsigned k=0;k<Rows;k++)
  {
   T sum=0;
   for(unsigned i=0;i<Cols;i++)
//	sum+=M1.Data[i][k]*M2.Data[j][i];
	sum+=M1.Data[k][i]*M2.Data[i][j];
   res.Data[k][j]=sum;
  }
 }

 return res;
}
// --------------------------

// --------------------------
// ��������� ���������
// --------------------------
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> MMatrix<T,Rows,Cols>::operator - (void) const
{
 MMatrix<T,Rows,Cols> res;

 const T* pm1=&Data[0][0];
 T* pm2=&res.Data[0][0];
 for(unsigned i=0;i<Cols*Rows;i++)
  *pm2++ = -*pm1++;

 return res;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator *= (T v)
{
 T* pm1=&Data[0][0];
 for(unsigned i=0;i<Cols*Rows;i++)
  *pm1++ *= v;

 return *this;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator * (const MMatrix<T,Rows,Cols> &M, T v)
{
 MMatrix<T,Rows,Cols> res=M;
 return res*=v;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator * (T v, const MMatrix<T,Rows,Cols> &M)
{
 MMatrix<T,Rows,Cols> res=M;
 return res*=v;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator /= (T v)
{
 T* pm1=&Data[0][0];
 for(unsigned int i=0;i<Cols*Rows;i++)
  *pm1++ /= v;

 return *this;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator / (const MMatrix<T,Rows,Cols> &M, T v)
{
 MMatrix<T,Rows,Cols> res=M;
 return res/=v;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator += (T v)
{
 T* pm1=Data;
 for(int i=0;i<Cols*Rows;i++)
  *pm1++ += v;

 return *this;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator + (const MMatrix<T,Rows,Cols> &M, T v)
{
 MMatrix<T,Rows,Cols> res=M;
 return res+=v;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator + (T v, const MMatrix<T,Rows,Cols> &M)
{
 MMatrix<T,Rows,Cols> res=M;
 return res+=v;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator -= (T v)
{
 T* pm1=Data;
 for(int i=0;i<Cols*Rows;i++)
  *pm1++ -= v;

 return *this;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator - (const MMatrix<T,Rows,Cols> &M, T v)
{
 MMatrix<T,Rows,Cols> res=M;
 return res-=v;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator - (T v, const MMatrix<T,Rows,Cols> &M)
{
 MMatrix<T,Rows,Cols> res=-M;
 return res+=v;
}
// --------------------------

// --------------------------
// ��������� ���������
// --------------------------
// �������� ������������
template<class T, unsigned Rows, unsigned Cols>
bool operator == (const MMatrix<T,Rows,Cols> &M1, const MMatrix<T,Rows,Cols> &M2)
{
 return (memcmp(M1.Data,M2.Data,Cols*Rows*sizeof(T)) == 0)?true:false;
}

template<class T, unsigned Rows, unsigned Cols>
bool operator != (const MMatrix<T,Rows,Cols> &M1, const MMatrix<T,Rows,Cols> &M2)
{
 return (memcmp(M1.Data,M2.Data,Cols*Rows*sizeof(T)) != 0)?true:false;
}
// --------------------------


// --------------------------
// �������������� ������
// --------------------------
// ����������������
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Cols,Rows>& MMatrix<T,Rows,Cols>::Transpose(MMatrix<T,Cols,Rows> &res) const
{
 for(unsigned i=0;i<Cols;i++)
  for(unsigned j=0;j<Rows;j++)
  {
   res.Data[j][i]=Data[i][j];
  }

 return res;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Cols,Rows> MMatrix<T,Rows,Cols>::Transpose(void) const
{
 MMatrix<T,Cols,Rows> res;
 return Transpose(res);
}

// ���������� ������� � ������� ����������� �����
// ���������� ����� ������������
template<class T, unsigned Rows, unsigned Cols>
unsigned MMatrix<T,Rows,Cols>::TriangleGauss(void)
{
 double teEps=1e-5;
 unsigned numcombos=0;

 int i, j, k;
 T cWorkElem;

 for (i=0; i<Rows-1; i++)
 {
  cWorkElem = Data[i][i];

  if(fabs(cWorkElem) <= teEps)
  {
   // �������� ����� ������ � ��������� ���������,
   // ����� �����, ������� ����
   for (j=i+1; i<Rows; i++)
   {
	cWorkElem = Data[j][i];
	if (fabs(cWorkElem) > teEps)
	{
	 // ��������� ��������� ������ � �������
	 for (k=0; k<Rows; k++)
	  Data[i][k]+=Data[j][k];
	 break;
	}
   }
  }

  if (fabs(cWorkElem) > teEps)
  {
   // ������� ������� ��������� -
   // ������������ ��� ���������� ������
   for (j=i+1; j<Rows; j++)
   {
	for (k=Rows-1;k>=i;k--)
	 Data[j][k] -= Data[i][k] * Data[j][i] / cWorkElem;
   }
  }
 }

 return numcombos;
}


// ���������� ������� � ������� ����������� �����
// ���������� ����� ������������
template<class T, unsigned Rows, unsigned Cols>
unsigned MMatrix<T,Rows,Cols>::TriangleBareis(void)
{
    T denom(1); //!
	int exchanges(0);

	for(unsigned l1=0; l1<Rows-1; ++l1)
	{ //��������� ��� ������ �������, ����� ���������
		unsigned maxN=l1;
		T maxValue=fabs(Data[l1][l1]);
		for(unsigned l2=l1+1; l2<Rows; ++l2)
		{ //������ ������ � ������������ �� ������ ���������
			T const value=fabs(Data[l2][l1]);
			if( value > maxValue ) { maxN=l2; maxValue=value; }
		}

        if( maxN > l1 )
		{ //����� �����
		 for(unsigned i=0;i<Cols;i++)
		 {
		  T temp=Data[l1][i];
		  Data[l1][i]=Data[maxN][i];
		  Data[maxN][i]=temp;
		 }

//			T *const temp=Data[l1];
//			Data[l1]=Data[maxN];
//			Data[maxN]=temp;
			++exchanges;
		} else { //����� �� �����, �� ����� �������� �� ����
            if(maxValue == T(0)) return exchanges;
        }

		T const value1=Data[l1][l1]; //!
		  //!

		for(unsigned l2=l1+1; l2<Rows; ++l2)
		{ //������� ������ �� ���� �����������
			T const value2=Data[l2][l1]; //!
			Data[l2][l1] = T(0);
			for(unsigned c=l1+1; c<Rows; ++c) //!
				Data[l2][c]=(Data[l2][c]*value1-Data[l1][c]*value2)/denom;
        }

        denom = value1; //!
	}
	//!
   // if(exchanges%2) return -M[N-1][N-1]; else return M[N-1][N-1];
   return exchanges;
}


// ��������������
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::Inverse(MMatrix<T,Cols,Rows> &res) const
{
 T det=Det();

 if(!det)
  return res;

 // get the determinant of a
 det = 1.0/det;

 MMatrix<T,Rows-1,Cols-1> Minor;

 for(unsigned j=0;j<Rows;j++)
 {
  for(unsigned i=0;i<Cols;i++)
  {
   // get the co-factor (matrix) of A(j,i)
   GetMinor(Minor,j,i);
   res.Data[j][i] = det*Minor.Det();
   if( (i+j)%2 == 1)
	res.Data[j][i] = -res.Data[j][i];
   }
  }

 res=res.Transpose();
 return res;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> MMatrix<T,Rows,Cols>::Inverse(void) const
{
 MMatrix<T,Rows,Cols> res(0.0);

 return Inverse(res);
}

// ����������� 3x3
template<class T, unsigned Rows, unsigned Cols>
T MMatrix<T,Rows,Cols>::Det3x3(void) const
{
	if(Rows != 3 || Cols != 3) return T(0.0);

	return
		 Data[0][0]*Data[1][1]*Data[2][2]
		+Data[0][1]*Data[1][2]*Data[2][0]
		+Data[1][0]*Data[2][1]*Data[0][2]
		-Data[2][0]*Data[1][1]*Data[0][2]
		-Data[0][1]*Data[1][0]*Data[2][2]
		-Data[0][0]*Data[1][2]*Data[2][1];
}

// �����������
template<class T, unsigned Rows, unsigned Cols>
T MMatrix<T,Rows,Cols>::Det(void) const
{

 if(Rows == 0 || Cols == 0)
  return T(0.0);

 if(Rows == 1 && Cols == 1)
 {
  return Data[0][0];
 }

 if(Rows == 2 && Cols == 2)
 {
  return Data[0][0]*Data[1][1]-Data[0][1]*Data[1][0];
 }

 if(Rows == 3 && Cols == 3)
 {
  return this->Det3x3();
 }

 if(Rows == 4 && Cols == 4)
 {
  return Data[0][0]*this->GetMinor(0,0).Det3x3()
		-Data[0][1]*this->GetMinor(0,1).Det3x3()
		+Data[0][2]*this->GetMinor(0,2).Det3x3()
		-Data[0][3]*this->GetMinor(0,3).Det3x3();
 }

 MMatrix<T,Rows,Cols> Temp=*this;

 unsigned numcombos=Temp.TriangleBareis();

 T det = Temp.Data[Rows-1][Cols-1];
 for(unsigned i=0;i<Rows;i++)
  if(fabs(Temp.Data[i][i])<1e-15)
   return T(0.0);

 return (numcombos%2)?-det:det;

/* if(Rows == 2)
  return Data[0][0]*Data[1][1]-Data[0][1]*Data[1][0];

 if(Rows == 1)
  return Data[0][0];

 // the determinant value
 T det = 0;

 for(unsigned i = 0; i < Rows; i++ )
 {
  // get minor of element (0,i)
  MMatrix<T,Cols-1,Rows-1> Res;

  GetMinor( Res, 0, i);

  // the recusion is here!
  det += (i%2==1?-1.0:1.0) * Data[0][i] * (Res.GetRows()>1)?Res.Det():Res.Data[0][0];
  //det += pow( -1.0, i ) * mat[0][i] * CalcDeterminant( minor,order-1 );
 }
 return det;     */
}

// ���������� �������� �������
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows-1,Cols-1>& MMatrix<T,Rows,Cols>::GetMinor(MMatrix<T,Rows-1,Cols-1> &res, unsigned row, unsigned col) const
{
	// indicate which col and row is being copied to dest
	unsigned colCount=0,rowCount=0;
	for(unsigned i = 0; i < Cols; i++ )
	{
		if( i != row )
		{
			rowCount = 0;
			for(unsigned j = 0; j < Rows; j++ )
			{
				// when j is not the element
				if( j != col )
				{
					res.Data[colCount][rowCount] = Data[i][j];
					rowCount++;
				}
			}
			colCount++;
		}

	}
	return res;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows-1,Cols-1> MMatrix<T,Rows,Cols>::GetMinor(unsigned row, unsigned col) const
{
 MMatrix<T,Rows-1,Cols-1> res;
 return GetMinor(res,row,col);
}

// ����
template<class T, unsigned Rows, unsigned Cols>
T MMatrix<T,Rows,Cols>::Trace(void) const
{
 unsigned crmin=(Cols<Rows)?Cols:Rows;

 T sum=0;
 for(unsigned i=0;i<crmin;i++)
  sum+=Data[i][i];

 return sum;
}

// ����� �������
template<class T, unsigned Rows, unsigned Cols>
T MMatrix<T,Rows,Cols>::operator !(void) const
{
 T res=0;
 const T* pm1=&Data[0][0];
 for(unsigned i=0;i<Cols*Rows;i++,pm1++)
  res+=*pm1 * *pm1;
 return sqrt(res);
}

// ������������ �������
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::Normalize(void)
{
 T norm=this->operator!();

 if(!norm)
  return *this;

 T* pm1=Data[0];
 for(int i=0;i<Cols*Rows;i++,pm1++)
  *pm1/=norm;

 return *this;
}

// ���������� ����� ������� �������� ��������
// ������� � ������� i0,j0 � �������� �������
template<class T, unsigned Rows, unsigned Cols, unsigned Rows2, unsigned Cols2>
MMatrix<T,Rows2,Cols2>& GetSubMatrix(const MMatrix<T,Rows,Cols>& source,unsigned j0, unsigned i0, MMatrix<T,Rows2,Cols2>& res)
{
 unsigned cols2=(Cols2<Cols)?Cols2:Cols;
 unsigned rows2=(Rows2<Rows)?Rows2:Rows;
 unsigned work_cols=((Cols-i0)<=cols2)?cols2:Cols-i0-1;
 unsigned work_rows=((Rows-i0)<=rows2)?rows2:Rows-j0-1;

 for(unsigned i=0;i<work_cols;i++)
  for(unsigned j=0;j<work_rows;j++)
  {
   res.Data[j][i]=source.Data[j+j0][i+i0];
  }

 return res;
}
// ������������ ����� ������� �������� ��������
// ������� � ������� i0,j0 � ������������ �������
template<class T, unsigned Rows, unsigned Cols, unsigned Rows2, unsigned Cols2>
MMatrix<T,Rows,Cols>& SetSubMatrix(MMatrix<T,Rows,Cols>& dest,unsigned j0, unsigned i0, const MMatrix<T,Rows2,Cols2>& source)
{
 unsigned cols2=(Cols2<Cols)?Cols2:Cols;
 unsigned rows2=(Rows2<Rows)?Rows2:Rows;
 unsigned work_cols=((Cols-i0)<=cols2)?cols2:Cols-i0-1;
 unsigned work_rows=((Rows-i0)<=rows2)?rows2:Rows-j0-1;

 for(unsigned i=0;i<work_cols;i++)
  for(unsigned j=0;j<work_rows;j++)
  {
   dest.Data[j0+j][i0+i]=source.Data[j][i];
  }

 return dest;
}
// --------------------------

// --------------------------
// ����������� ������
// --------------------------
// ������� �������
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> MMatrix<T,Rows,Cols>::Zero(void)
{
 MMatrix<T,Rows,Cols> res;

 memset(res.Data,0,Rows*Cols*sizeof(T));
 return res;
}

// ��������� �������
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> MMatrix<T,Rows,Cols>::Eye(void)
{
 MMatrix<T,Rows,Cols> res;
 memset(res.Data,0,Rows*Cols*sizeof(T));

 unsigned crmin=(Cols<Rows)?Cols:Rows;

 for(unsigned i=0;i<crmin;i++)
  res.Data[i][i]=1;

 return res;
}

// ���������� ������� ������� � 0
template<class T, unsigned Rows, unsigned Cols>
void MMatrix<T,Rows,Cols>::ToZero(void)
{
 memset(Data,0,Rows*Cols*sizeof(T));
}

// ���������� ������� ������� � ���������
template<class T, unsigned Rows, unsigned Cols>
void MMatrix<T,Rows,Cols>::ToEye(void)
{
 ToZero();
 int crmin=(Cols<Rows)?Cols:Rows;

 for(int i=0;i<crmin;i++)
  Data[i][i]=1;
}
// --------------------------


// --------------------------
// ��������� ������
// --------------------------
template<class T, unsigned Rows, unsigned Cols>
bool MMatrix<T,Rows,Cols>::operator == (const MMatrix<T,Rows,Cols> &M) const
{
 if(!memcmp(Data,M.Data,sizeof(T)*Cols*Rows))
  return true;
 return false;
}

template<class T, unsigned Rows, unsigned Cols>
bool MMatrix<T,Rows,Cols>::operator != (const MMatrix<T,Rows,Cols> &M) const
{
 return !((*this) == M);
}
// --------------------------

template<class T, unsigned Rows, unsigned Cols>
void MMatrix<T,Rows,Cols>::Print(std::ostream &stream)
{
 using namespace std;
 stream<<"["<<Rows<<"x"<<Cols<<"]"<<std::endl;
 if(Rows == 0 || Cols == 0)
 {
 }
 else
 {
  for(int i=0;i<Rows;i++)
  {
   for(int j=0;j<Cols;j++)
   {
	stream<<Data[i][j]<<"\t";
   }
   stream<<std::endl;
  }
 }
}

template<class T, unsigned Rows, unsigned Cols>
std::ostream& operator << (std::ostream &stream, MMatrix<T,Rows,Cols> &m)
{
 m.Print(stream);
 return stream;
}



}
#endif
