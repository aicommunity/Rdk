#ifndef MDMatrixH
#define MDMatrixH

#include <limits>
#include <iostream>
#include <string.h>
#include "MMatrixBase.h"

/// �������� ��������� �� ������� ���������� ��������� ����� ����� ������� ��� �� ����������
/// (������������� ������ ������, �.�. ��� �������� ������������� �������. ��� ���������� ����� �������
/// �� �����������).
#define RDK_MMATRIX_RESIZE_ROW_LINEAR_CONSTANT 30

/// ��������� �����������, �� ������� ���������� ����������� ������ (� ��������� �������) ��� �� ����������
/// � ������, ���� �������� ����� �������, ��� ����� ����� ������������� �� �������� ������ ��� ResizeLinearConstant
#define RDK_MMATRIX_RESIZE_AMORTIZED_CONSTANT 1.5

namespace RDK{

template<class T>
class MDMatrix: public MMatrixBase
{
public:
typedef T value_type;
 /*
/// �������� ��������� �� ������� ���������� ��������� ����� ����� ������� ��� �� ����������
/// (������������� ������ ������, �.�. ��� �������� ������������� �������. ��� ���������� ����� �������
/// �� �����������).
static const int ResizeRowLinearConstant;

/// ��������� �����������, �� ������� ���������� ����������� ������ (� ��������� �������) ��� �� ����������
/// � ������, ���� �������� ����� �������, ��� ����� ����� ������������� �� �������� ������ ��� ResizeLinearConstant
static const float ResizeAmortizedConstant;
   */
union
{
 T* Data;
 double *Double;
 int *Int;
 unsigned char *UChar;
 char* Char;
 void* Void;
 void **PVoid;
};

protected:
// ������ �������
int Rows;
int Cols;

/// �������� ������ ���������� ������� ������
int Capacity;

public:
// --------------------------
// ������������ � �����������
// --------------------------
MDMatrix(void);
MDMatrix(int rows, int cols);
MDMatrix(int rows, int cols, T defvalue);
MDMatrix(const MDMatrix<T> &copy);
MDMatrix(int rows, int cols, const T* data);
MDMatrix(int rows, int cols, const void* data);
virtual ~MDMatrix();
// --------------------------

// --------------------------
// ������� ����������� � ������
// --------------------------
void Resize(int rows, int cols, T defvalue=0);

void Assign(int rows, int cols, const T *data);
void Assign(int rows, int cols, T value);
void Assign(int rows, int cols, const void *data);

/// ��������� ������ � ������� � ������� index
void InsertRows(int index, int num_rows=1);

/// ��������� ������� � ������� � ������� index
void InsertCols(int index, int num_cols=1);

/// ������� num_rows ����� ������� �� ������ index
void DeleteRows(int index, int num_rows=1);

/// ������� num_cols �������� ������� �� ������ index
void DeleteCols(int index, int num_cols=1);

/// ��������� ������ � ����� �������
void AddRows(int num_rows);
void AddRows(int num_rows, T def_value);
void AddRow(void);
void AddRow(T def_value);

/// ��������� ������� � ����� �������
void AddCols(int num_cols);
void AddCols(int num_cols, T def_value);
void AddCol(void);
void AddCol(T def_value);
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

// ���������� �������� ��� �������� �������
//virtual const type_info& GetLanguageType(void) const;
// -----------------------------------

// --------------------------
// ��������� ���������� �������
// --------------------------
// �������� ������������
const MDMatrix<T>& operator = (const MDMatrix<T> &copy);
MDMatrix<T>& operator = (T value);
MDMatrix<T>& operator = (const T* data);
MDMatrix<T>& operator = (const void* data);

// ��������� ����������� ������
int GetCols(void) const;
int GetRows(void) const;

// ������ � ��������
T& operator [] (int i);
const T& operator [] (int i) const;
T& operator [] (size_t i);
const T& operator [] (size_t i) const;

T& operator () (int i, int j);
const T& operator () (int i, int j) const;

// ���������� �������� ������ �������
MDMatrix<T> GetRow(int i) const;

// ���������� �������� ������� �������
MDMatrix<T> GetCol(int i) const;

/// ������������� ������ � ������ ������� ��� � ����������� ������� ����������
/// ����. �����������!
template<typename U>
U& As(int i)
{
 return reinterpret_cast<U*>(Data)[i];
}


template<typename U>
const U& As(int i) const
{
 return reinterpret_cast<U*>(Data)[i];
}
// --------------------------

// --------------------------
// ��������� ���������
// --------------------------
MDMatrix<T>& operator += (const MDMatrix<T> &M);
MDMatrix<T>& operator -= (const MDMatrix<T> &M);
// --------------------------

// --------------------------
// ��������� ���������
// --------------------------
MDMatrix<T> operator - (void) const;

MDMatrix<T>& operator *= (T v);

MDMatrix<T>& operator /= (T v);

MDMatrix<T>& operator += (T v);

MDMatrix<T>& operator -= (T v);
// --------------------------

// --------------------------
// �������������� ������
// --------------------------
// ����������������
MDMatrix<T>& Transpose(MDMatrix<T> &res) const;
MDMatrix<T> Transpose(void) const;

// ���������� ������� � ������� ����������� �����
// ���������� ����� ������������
int TriangleGauss(void);
int TriangleBareis(void);

// ��������������
MDMatrix<T>& Inverse(MDMatrix<T> &res) const;
MDMatrix<T> Inverse(void) const;

// �����������
protected:
T Det3x3(void) const;
public:
T Det(void) const;

// ���������� �������� �������
MDMatrix<T>& GetMinor(MDMatrix<T> &res, int row, int col) const;
MDMatrix<T> GetMinor(int row, int col) const;

// �������� ����� �������
MDMatrix<T>& Split(MDMatrix<T> &res, int row_beg, int row_end, int col_beg, int col_end) const;
MDMatrix<T> Split(int row_beg, int row_end, int col_beg, int col_end) const;

// ��������� � ������� ��������
MDMatrix<T>& Merge(MDMatrix<T> &res, int row_beg, int row_end, int col_beg, int col_end);
MDMatrix<T> Merge(int row_beg, int row_end, int col_beg, int col_end);

// ����
T Trace(void) const;

// ����� �������
T operator !(void) const;

// ������������ �������
MDMatrix<T>& Normalize(void);

// ��������������� ���������� �������
bool Orthogonolize(MDMatrix<T> &res);

/// ����� ������������ � ������������� �������� ��������
T FindMinValue(void) const;
T FindMaxValue(void) const;
// --------------------------

// --------------------------
// ����������� ������
// --------------------------
// ������� �������
MDMatrix<T> Zero(void) const;

// ���������� ������� ������� � 0
void ToZero(void);

// ��������� �������
MDMatrix<T> Eye(void) const;

// ���������� ������� ������� � ���������
void ToEye(void);
// --------------------------

// --------------------------
// ��������� ������
// --------------------------
bool operator == (const MDMatrix<T> &M) const;
bool operator != (const MDMatrix<T> &M) const;
// --------------------------

void Print(std::ostream &stream);
};
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
/*
/// �������� ��������� �� ������� ���������� ��������� ����� ����� ������� ��� �� ����������
/// (������������� ������ ������, �.�. ��� �������� ������������� �������. ��� ���������� ����� �������
/// �� �����������).
template<class T>
const int MDMatrix<T>::ResizeRowLinearConstant=30;

/// ��������� �����������, �� ������� ���������� ����������� ������ (� ��������� �������) ��� �� ����������
/// � ������, ���� �������� ����� �������, ��� ����� ����� ������������� �� �������� ������ ��� ResizeLinearConstant
template<class T>
const float MDMatrix<T>::ResizeAmortizedConstant=1.5;
*/

// --------------------------
// ������������ � �����������
// --------------------------
template<class T>
MDMatrix<T>::MDMatrix(void)
: Data(0),Rows(0),Cols(0),Capacity(0)
{
}

template<class T>
MDMatrix<T>::MDMatrix(int rows, int cols)
: Data(0),Rows(0),Cols(0),Capacity(0)
{
	Resize(rows, cols);
}

template<class T>
MDMatrix<T>::MDMatrix(int rows, int cols, T defvalue)
: Data(0),Rows(0),Cols(0),Capacity(0)
{
 Resize(rows, cols);
 for(int i=0;i<Rows*Cols;i++)
   Data[i]=defvalue;
}

template<class T>
MDMatrix<T>::MDMatrix(const MDMatrix<T> &copy)
: Data(0),Rows(0),Cols(0),Capacity(0)
{
 *this=copy;
}

template<class T>
MDMatrix<T>::MDMatrix(const int rows, const  int cols, const T* data)
: Data(0),Rows(0),Cols(0),Capacity(0)
{
 Assign(rows,cols,data);
}

template<class T>
MDMatrix<T>::MDMatrix(const int rows, const  int cols, const void* data)
: Data(0),Rows(0),Cols(0),Capacity(0)
{
 Assign(rows,cols,data);
}

template<class T>
MDMatrix<T>::~MDMatrix()
{
 if(Data)
 {
  delete[] Data;
  Rows=Cols=Capacity=0;
 }
}
// --------------------------

// --------------------------
// ������� �����������
template<class T>
void MDMatrix<T>::Resize(int rows, int cols, T defvalue)
{
 if(rows == Rows && cols == Cols)
  return;

 if(rows<0 || cols<0)
  return;

 if(rows == 0 || cols == 0)
 {
  Rows=rows;
  Cols=cols;
  return;
 }

 T* new_data(0);
 int new_capacity(0);

 if(rows && cols && rows*cols>Capacity)
 {
  if(cols>Cols || rows>Rows+RDK_MMATRIX_RESIZE_ROW_LINEAR_CONSTANT)
   new_capacity=int(rows*cols*RDK_MMATRIX_RESIZE_AMORTIZED_CONSTANT);
  else
   new_capacity=(rows+RDK_MMATRIX_RESIZE_ROW_LINEAR_CONSTANT)*cols;


  new_data = new T[new_capacity];
  if(!Data) // �������� ������ � ������� �� ����
  {
   if(!defvalue)
	memset(new_data,0,rows*cols*sizeof(T));
   else
   {
	for(int i=0; i<rows; i++)
	 for(int j=0;j<cols; j++)
	  new_data[i*cols+j]=defvalue;
   }
  }
  else
  if(new_data) // ������� �����������, �������� ����� ������ ��������� ������
  {
   int c_rows=(Rows<rows)?Rows:rows;
   int c_cols=(Cols<cols)?Cols:cols;
   for(int i=0; i<c_rows; i++)
	for(int j=0;j<c_cols; j++)
	 new_data[i*cols+j]=Data[i*Cols+j];

   for(int i=0; i<c_rows; i++)
	for(int j=c_cols;j<cols; j++)
	 new_data[i*cols+j]=defvalue;

   for(int i=c_rows; i<rows; i++)
	for(int j=0;j<cols; j++)
	 new_data[i*cols+j]=defvalue;
  }
 }
 else
 if(cols<Cols) // ������� ����������� �� ����� �������, ��������� ������ � ������ ������
 {
  T* old_pos=Data+1*Cols;
  T* new_pos=Data+cols;
  for(int i=1;i<rows;i++)
  {
   memmove(new_pos,old_pos,cols*sizeof(T));
   new_pos+=cols;
   old_pos+=Cols;
  }
  for(int i=Rows;i<rows;i++)
   for(int j=0;j<cols;j++)
    Data[i*cols+j]=defvalue;
 }
 else // ������� ����������� ��� ����������, ��������� ������ � ������ ������
 {
  int c_rows=(Rows<rows)?Rows:rows;
  T* old_pos=Data+(c_rows-1)*Cols;
  T* new_pos=Data+(c_rows-1)*cols;
  for(int i=c_rows;i>0;i--)
  {
   memmove(new_pos,old_pos,Cols*sizeof(T));
   new_pos-=cols;
   old_pos-=Cols;
  }
  for(int i=Rows;i<rows;i++)
   for(int j=0;j<cols;j++)
    Data[i*cols+j]=defvalue;

  for(int i=0;i<rows;i++)
   for(int j=Cols;j<cols;j++)
    Data[i*cols+j]=defvalue;
 }


 if(new_data)
 {
  delete []Data;
  Data=new_data;
  Capacity=new_capacity;
 }
 Rows=rows;
 Cols=cols;
}

template<class T>
void MDMatrix<T>::Assign(int rows, int cols, const T *data)
{
 Resize(rows, cols);
 *this=data;
}

template<class T>
void MDMatrix<T>::Assign(int rows, int cols, T value)
{
 Resize(rows, cols);
 *this=value;
}

template<class T>
void MDMatrix<T>::Assign(int rows, int cols, const void *data)
{
 Resize(rows,cols);
 *this=data;
}

template<class T>
void MDMatrix<T>::InsertRows(int index, int num_rows)
{
 if(num_rows<=0)
  return;
 int old_rows=Rows;
 Resize(Rows+num_rows,Cols);

 if(index>=0 && index<old_rows)
 {
  memmove(Data+Cols*index,Data+Cols*(index+num_rows),num_rows*Cols*sizeof(T));
  memset(Data+Cols*index,0,Cols*sizeof(T));
 }
}

template<class T>
void MDMatrix<T>::InsertCols(int index, int num_cols)
{
 if(num_cols<=0)
  return;

 int old_cols(Cols);
 Resize(Rows,Cols+num_cols);

 if(index>=0 && index<old_cols)
 {
  for(int i=0;i<Rows;i++)
  {
   for(int j=Cols-1;j>index;j--)
   {
	Data[i*Cols+j]=Data[i*Cols+j-num_cols];
   }
   for(int j=index;j<index+num_cols;j++)
	Data[i*Cols+j]=0;
  }
 }
}

template<class T>
void MDMatrix<T>::DeleteRows(int index, int num_rows)
{
 if(num_rows<=0)
  return;
 if(num_rows>=Rows)
 {
  Resize(0,Cols);
  return;
 }
 memmove(Data+Cols*index,Data+Cols*(index+num_rows),(Rows-index-num_rows)*Cols*sizeof(T));
 Resize(Rows-num_rows,Cols);
}

template<class T>
void MDMatrix<T>::DeleteCols(int index, int num_cols)
{
 if(num_cols<=0)
  return;

 if(num_cols>=Cols)
 {
  Resize(Rows,0);
  return;
 }

 for(int i=0;i<Rows;i++)
 {
  for(int j=index;j<=index+num_cols;j++)
  {
   Data[i*Cols+j]=Data[i*Cols+j+num_cols];
  }
 }
 Resize(Rows,Cols-num_cols);
}

/// ��������� ������ � ����� �������
template<class T>
void MDMatrix<T>::AddRows(int num_rows)
{
 Resize(Rows+num_rows,Cols);
}

template<class T>
void MDMatrix<T>::AddRows(int num_rows, T def_value)
{
 Resize(Rows+num_rows,Cols,def_value);
}

template<class T>
void MDMatrix<T>::AddRow(void)
{
 Resize(Rows+1,Cols);
}

template<class T>
void MDMatrix<T>::AddRow(T def_value)
{
 Resize(Rows+1,Cols,def_value);
}

/// ��������� ������� � ����� �������
template<class T>
void MDMatrix<T>::AddCols(int num_cols)
{
 Resize(Rows,Cols+num_cols);
}

template<class T>
void MDMatrix<T>::AddCols(int num_cols, T def_value)
{
 Resize(Rows,Cols+num_cols,def_value);
}

template<class T>
void MDMatrix<T>::AddCol(void)
{
 Resize(Rows,Cols+1);
}

template<class T>
void MDMatrix<T>::AddCol(T def_value)
{
 Resize(Rows,Cols+1,def_value);
}
// --------------------------

// -----------------------------------
// ����� ������ ������� � ���������� �������
// -----------------------------------
/// ���������� ����������� �������
template<class T>
int MDMatrix<T>::GetDimensions(void) const
{
 return 2;
}

/// ���������� ����� ��������� �� ������� �������
template<class T>
int MDMatrix<T>::GetSize(int i) const
{
 return (i==0)?Rows:((i==1)?Cols:0);
}

/// ���������� ��������� ����� ���� ���������
template<class T>
int MDMatrix<T>::GetSize(void) const
{
 return Rows*Cols;
}

/// ���������� true ���� ��������� ����� ���� ��������� == 0
template<class T>
bool MDMatrix<T>::IsEmpty(void) const
{
 return (!Rows || !Cols);
}

/// ���������� ����� ��������� �� ���� ������������
template<class T>
MMatrixSize MDMatrix<T>::GetMatrixSize(void) const
{
 MMatrixSize size(Rows,Cols);
 return size;
}

/// ������������� ����� ��������� �� ���� ������������
template<class T>
bool MDMatrix<T>::Resize(const MMatrixSize &size)
{
 if(size.GetDimensions() != 2)
  return false;

 Resize(size[0],size[1]);
 return true;
}

/// ���������� ��������� ������ ������ ������� � ������
template<class T>
int MDMatrix<T>::GetByteSize(void) const
{
 return Rows*Cols*sizeof(T);
}

/// ���������� ����� � ������ ������ �������� �����
template<class T>
int MDMatrix<T>::GetElementByteSize(void) const
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
template<class T>
const void* MDMatrix<T>::GetVoid(void) const
{
 return Void;
}

template<class T>
void* MDMatrix<T>::GetVoid(void)
{
 return Void;
}
// -----------------------------------

// --------------------------
// ��������� ���������� �������
// --------------------------
// �������� ������������

template<class T>
const MDMatrix<T>& MDMatrix<T>::operator = (const MDMatrix<T> &copy)
{
 Resize(copy.Rows, copy.Cols);
 memcpy(Data,copy.Data,Rows*Cols*sizeof(T));
 return *this;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator = (T value)
{
 T *p=Data;
 for(int i=0;i<Rows*Cols;i++)
   *p++=value;
 return *this;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator = (const T* data)
{
 memcpy(Data,data,Rows*Cols*sizeof(T));
 return *this;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator = (const void* data)
{
 memcpy(Data,data,Rows*Cols*sizeof(T));
 return *this;
}


// ��������� ����������� ������
template<class T>
int MDMatrix<T>::GetCols(void) const
{
 return Cols;
}

template<class T>
int MDMatrix<T>::GetRows(void) const
{
 return Rows;
}


// --------------------------
// ������ � ��������
template<class T>
T& MDMatrix<T>::operator [] (int i)
{
 return Data[i];
}

template<class T>
const T& MDMatrix<T>::operator [] (int i) const
{
 return Data[i];
}

template<class T>
T& MDMatrix<T>::operator [] (size_t i)
{
 return Data[i];
}

template<class T>
const T& MDMatrix<T>::operator [] (size_t i) const
{
 return Data[i];
}

template<class T>
T& MDMatrix<T>::operator () (int i, int j)
{
 return Data[i*Cols+j];
}

template<class T>
const T& MDMatrix<T>::operator () (int i, int j) const
{
 return Data[i*Cols+j];
}
// --------------------------

// ���������� �������� ������ �������
template<class T>
MDMatrix<T> MDMatrix<T>::GetRow(int i) const
{
 MDMatrix<T> res(1,Cols);
 for(int j=0;j<Cols;j++)
  res.Data[j]=Data[i*Cols+j];
 return res;
}

// ���������� �������� ������� �������
template<class T>
MDMatrix<T> MDMatrix<T>::GetCol(int i) const
{
 MDMatrix<T> res(Rows,1);
 for(int j=0;j<Rows;j++)
  res.Data[j]=Data[j*Cols+i];
 return res;
}

// -------------------------------------------------------------------------
// ��������� ���������
// --------------------------
template<class T>
MDMatrix<T>& MDMatrix<T>::operator += (const MDMatrix<T> &M)
{
 if(Rows != M.Rows || Cols != M.Cols)
  return *this;

 for(int i=0;i<Rows*Cols;i++)
   Data[i]+=M.Data[i];

 return *this;
}

template<class T>
MDMatrix<T> operator + (const MDMatrix<T> &M1, const MDMatrix<T> &M2)
{
 MDMatrix<T> res(M1);

 res+=M2;

 return res;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator -= (const MDMatrix<T> &M)
{
 if(Rows != M.Rows || Cols != M.Cols)
  return *this;

 for(int i=0;i<Rows*Cols;i++)
   Data[i]-=M.Data[i];

 return *this;
}

template<class T>
MDMatrix<T> operator - (const MDMatrix<T> &M1, const MDMatrix<T> &M2)
{
 MDMatrix<T> res(M1);

 res-=M2;

 return res;
}

template<class T>
MDMatrix<T> operator * (const MDMatrix<T> &M1, const MDMatrix<T> &M2)
{
 MDMatrix<T> res(M1.GetRows(),M2.GetCols());

 for(int j=0;j<M2.GetCols();j++)
 {
  for(int k=0;k<M1.GetRows();k++)
  {
   T sum=0;
   for(int i=0;i<M1.GetCols();i++)
	sum+=M1.Data[k*M1.GetCols()+i]*M2.Data[i*M2.GetCols()+j];
   res.Data[k*res.GetCols()+j]=sum;
  }
 }

 return res;
}
// --------------------------

// --------------------------
// ��������� ���������
// --------------------------
template<class T>
MDMatrix<T> MDMatrix<T>::operator - (void) const
{
 MDMatrix<T> res(Rows,Cols);

 for(int i=0;i<Rows*Cols;i++)
  res.Data[i]=-Data[i];

 return res;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator *= (T v)
{
 for(int i=0;i<Rows*Cols;i++)
  Data[i]*=v;

 return *this;
}

template<class T>
MDMatrix<T> operator * (const MDMatrix<T> &M, T v)
{
 MDMatrix<T> res(M);
 return res*=v;
}

template<class T>
MDMatrix<T> operator * (T v, const MDMatrix<T> &M)
{
 MDMatrix<T> res(M);
 return res*=v;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator /= (T v)
{
 for(int i=0;i<Rows*Cols;i++)
  Data[i]/=v;

 return *this;
}

template<class T>
MDMatrix<T> operator / (const MDMatrix<T> &M, T v)
{
 MDMatrix<T> res(M);
 return res/=v;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator += (T v)
{
 for(int i=0;i<Rows*Cols;i++)
  Data[i]+=v;

 return *this;
}

template<class T>
MDMatrix<T> operator + (const MDMatrix<T> &M, T v)
{
 MDMatrix<T> res(M);
 return res+=v;
}

template<class T>
MDMatrix<T> operator + (T v, const MDMatrix<T> &M)
{
 MDMatrix<T> res(M);
 return res+=v;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator -= (T v)
{
 for(int i=0;i<Rows*Cols;i++)
  Data[i]-=v;

 return *this;
}

template<class T>
MDMatrix<T> operator - (const MDMatrix<T> &M, T v)
{
 MDMatrix<T> res(M);
 return res-=v;
}

template<class T>
MDMatrix<T> operator - (T v, const MDMatrix<T> &M)
{
 MDMatrix<T> res(-M);
 return res+=v;
}
// --------------------------

// --------------------------
// ��������� ���������
// --------------------------
// �������� ������������
template<class T>
bool operator == (const MDMatrix<T> &M1, const MDMatrix<T> &M2)
{
 if(M1.GetRows() != M2.GetRows() || M1.GetCols() != M2.GetCols())
  return false;

 bool flag = 1;
 for(int i=0;i<M1.GetRows()*M1.GetCols();i++)
  if(M1.Data[i] != M2.Data[i])
   flag=0;
 return flag;
}

template<class T>
bool operator != (const MDMatrix<T> &M1, const MDMatrix<T> &M2)
{
 return !(M1==M2);
}
// --------------------------



// --------------------------
// �������������� ������
// --------------------------
// ����������������
template<class T>
MDMatrix<T>& MDMatrix<T>::Transpose(MDMatrix<T> &res) const
{
 res.Resize(Cols,Rows);
 for(int i=0;i<Rows;i++)
  for(int j=0;j<Cols;j++)
   res.Data[j*Rows+i]=Data[i*Cols+j];

 return res;
}

template<class T>
MDMatrix<T> MDMatrix<T>::Transpose(void) const
{
 MDMatrix<T> res(Cols,Rows);
 return Transpose(res);
}

// ���������� ������� � ������� ����������� �����
// ���������� ����� ������������
template<class T>
int MDMatrix<T>::TriangleGauss(void)
{
 double teEps=1e-5;
 int numcombos=0;

 int i, j, k;
 T cWorkElem;

 for (i=0; i<Rows-1; i++)
 {
  cWorkElem = (*this)(i,i);

  if(fabs(cWorkElem) <= teEps)
  {
   // �������� ����� ������ � ��������� ���������,
   // ����� �����, ������� ����
   for (j=i+1; i<Rows; i++)
   {
	cWorkElem = (*this)(j,i);
	if (fabs(cWorkElem) > teEps)
	{
	 // ��������� ��������� ������ � �������
	 for (k=0; k<Rows; k++)
	  (*this)(i,k)+=(*this)(j,k);
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
	 (*this)(j,k) -= (*this)(i,k) * (*this)(j,i) / cWorkElem;
   }
  }
 }

 return numcombos;
}


// ���������� ������� � ������� ����������� �����
// ���������� ����� ������������
template<class T>
int MDMatrix<T>::TriangleBareis(void)
{
    T denom(1); //!
	int exchanges(0);

	for(int l1=0; l1<Rows-1; ++l1)
	{ //��������� ��� ������ �������, ����� ���������
	 if(fabs(denom)<std::numeric_limits<T>::epsilon())
	  throw EMatrixZeroDiv();
		int maxN=l1;
		T maxValue=fabs((*this)(l1,l1));
		for(int l2=l1+1; l2<Rows; ++l2)
		{ //������ ������ � ������������ �� ������ ���������
			T const value=fabs((*this)(l2,l1));
			if( value > maxValue ) { maxN=l2; maxValue=value; }
		}

        if( maxN > l1 )
		{ //����� �����
		 for(int i=0;i<Cols;i++)
		 {
		  T temp=(*this)(l1,i);
		  (*this)(l1,i)=(*this)(maxN,i);
		  (*this)(maxN,i)=temp;
		 }

//			T *const temp=Data[l1];
//			Data[l1]=Data[maxN];
//			Data[maxN]=temp;
			++exchanges;
		} else { //����� �� �����, �� ����� �������� �� ����
            if(maxValue == T(0)) return exchanges;
        }

		T const value1=(*this)(l1,l1); //!
		  //!

		for(int l2=l1+1; l2<Rows; ++l2)
		{ //������� ������ �� ���� �����������
			T const value2=(*this)(l2,l1); //!
			(*this)(l2,l1) = T(0);
			for(int c=l1+1; c<Rows; ++c) //!
				(*this)(l2,c)=((*this)(l2,c)*value1-(*this)(l1,c)*value2)/denom;
        }

		denom = value1; //!
	}
	//!
   // if(exchanges%2) return -M[N-1][N-1]; else return M[N-1][N-1];
   return exchanges;
}


// ��������������
template<class T>
MDMatrix<T>& MDMatrix<T>::Inverse(MDMatrix<T> &res) const
{
 if(Rows == 1 && Cols == 1)
 {
  T det=(*this)(0,0);
  if(fabs(det)<std::numeric_limits<T>::epsilon())
   throw EMatrixZeroDet();
  res(0,0)=1.0/det;
  return res;
 }
 else
 if(Rows == 2 && Cols == 2)
 {
  T det=(*this)(0,0)*(*this)(1,1)-(*this)(0,1)*(*this)(1,0);
  if(fabs(det)<std::numeric_limits<T>::epsilon())
   throw EMatrixZeroDet();

  res(0,0)=(*this)(1,1)/det;
  res(0,1)=-(*this)(0,1)/det;
  res(1,0)=-(*this)(1,0)/det;
  res(1,1)=(*this)(0,0)/det;
  return res;
 }

 T det=Det();

 if(fabs(det)<std::numeric_limits<T>::epsilon())
  throw EMatrixZeroDet();

 // get the determinant of a
 det = 1.0/det;

 MDMatrix<T> Minor(Rows-1,Cols-1);

 for(int j=0;j<Rows;j++)
 {
  for(int i=0;i<Cols;i++)
  {
   // get the co-factor (matrix) of A(j,i)
   GetMinor(Minor,j,i);
   res(j,i) = det*Minor.Det();
   if( (i+j)%2 == 1)
	res(j,i) = -res(j,i);
   }
  }

 res=res.Transpose();
 return res;
}

template<class T>
MDMatrix<T> MDMatrix<T>::Inverse(void) const
{
 MDMatrix<T> res(Cols,Rows);

 return Inverse(res);
}

// ����������� 3x3
template<class T>
T MDMatrix<T>::Det3x3(void) const
{
	if(Rows != 3 || Cols != 3) return T(0.0);

	return
		 (*this)(0,0)*(*this)(1,1)*(*this)(2,2)
		+(*this)(0,1)*(*this)(1,2)*(*this)(2,0)
		+(*this)(1,0)*(*this)(2,1)*(*this)(0,2)
		-(*this)(2,0)*(*this)(1,1)*(*this)(0,2)
		-(*this)(0,1)*(*this)(1,0)*(*this)(2,2)
		-(*this)(0,0)*(*this)(1,2)*(*this)(2,1);
}

// �����������
template<class T>
T MDMatrix<T>::Det(void) const
{

 if(Rows == 0 || Cols == 0)
  return T(0.0);

 if(Rows == 1 && Cols == 1)
 {
  return (*this)(0,0);
 }

 if(Rows == 2 && Cols == 2)
 {
  return (*this)(0,0)*(*this)(1,1)-(*this)(0,1)*(*this)(1,0);
 }

 if(Rows == 3 && Cols == 3)
 {
  return this->Det3x3();
 }

 if(Rows == 4 && Cols == 4)
 {
  return (*this)(0,0)*this->GetMinor(0,0).Det3x3()
		-(*this)(0,1)*this->GetMinor(0,1).Det3x3()
		+(*this)(0,2)*this->GetMinor(0,2).Det3x3()
		-(*this)(0,3)*this->GetMinor(0,3).Det3x3();
 }

 MDMatrix<T> Temp(*this);
 Temp.TriangleGauss();

 T det=*Temp.Data;
 for(int i=1;i<((Rows<Cols)?Rows-1:Cols-1);i++)
  det*=(*this)(i,i);
 return det;
// T det = Temp(Rows-1,Cols-1);

// return (numcombos%2)?-det:det;
}

// ���������� �������� �������
template<class T>
MDMatrix<T>& MDMatrix<T>::GetMinor(MDMatrix<T> &res, int row, int col) const
{
	// indicate which col and row is being copied to dest
	int colCount=0,rowCount=0;
	for(int i = 0; i < Cols; i++ )
	{
		if( i != row )
		{
			rowCount = 0;
			for(int j = 0; j < Rows; j++ )
			{
				// when j is not the element
				if( j != col )
				{
					res(colCount,rowCount) = (*this)(i,j);
					rowCount++;
				}
			}
			colCount++;
		}

	}
	return res;
}

template<class T>
MDMatrix<T> MDMatrix<T>::GetMinor(int row, int col) const
{
 MDMatrix<T> res(Rows-1,Cols-1);
 return GetMinor(res,row,col);
}

// �������� ����� �������
template<class T>
MDMatrix<T>& MDMatrix<T>::Split(MDMatrix<T> &res, int row_beg, int row_end, int col_beg, int col_end) const
{
 res.Resize(row_end-row_beg+1,col_end-col_beg+1);
	for(int i=row_beg;i<=row_end;i++)
		for(int j=col_beg;j<=col_end;j++)
			res(i-row_beg,j-col_beg)=(*this)(i,j);
	return res;
}

template<class T>
MDMatrix<T> MDMatrix<T>::Split(int row_beg, int row_end, int col_beg, int col_end) const
{
 MDMatrix<T> res(row_end-row_beg+1,col_end-col_beg+1);
 return Split(res,row_beg, row_end, col_beg, col_end);
}

// ��������� � ������� ��������
template<class T>
MDMatrix<T>& MDMatrix<T>::Merge(MDMatrix<T> &res, int row_beg, int row_end, int col_beg, int col_end)
{
	for(int i=row_beg;i<=row_end;i++)
		for(int j=col_beg;j<=col_end;j++)
			(*this)(i,j)=res(i-row_beg,j-col_beg);
	return res;
}

template<class T>
MDMatrix<T> MDMatrix<T>::Merge(int row_beg, int row_end, int col_beg, int col_end)
{
 MDMatrix<T> res(row_end-row_beg+1,col_end-col_beg+1);
 return Merge(res,row_beg, row_end, col_beg, col_end);
}

// ����
template<class T>
T MDMatrix<T>::Trace(void) const
{
 int crmin=(Cols<Rows)?Cols:Rows;

 T sum=0;
 for(int i=0;i<crmin;i++)
  sum+=(*this)(i,i);

 return sum;
}

// ����� �������
template<class T>
T MDMatrix<T>::operator !(void) const
{
 T res=0;
  for(int i=0;i<Rows;i++)
	for(int j=0;j<Cols;j++)
		res+=(*this)(i,j) * (*this)(i,j);
 return T(sqrt(res));
}

// ������������ �������
template<class T>
MDMatrix<T>& MDMatrix<T>::Normalize(void)
{
 T norm=this->operator!();

 if(!norm)
  return *this;

 (*this)/=norm;

 return *this;
}


// ��������������� ���������� �������
template<class T>
bool MDMatrix<T>::Orthogonolize(MDMatrix<T> &res)
{
 MDMatrix<T> &src=*this;
 if(src.GetRows() != src.GetCols())
  return false;

 res.Resize(src.GetRows(),src.GetCols());

 if(src.GetRows() == 0 || src.GetCols() == 0)
  return true;

 if(!src.Det())
 	return false;

 int i, j ,n;
 T numerator, consequent;
 int size = src.GetRows();

 // ������ ������
 for(i = 0; i < size; i++)
	res(0,i) = src(0,i);
 if(size == 1) return true;

 // ��������� ������
 for(i = 1; i < size; i++)
 {
	for(j = 0; j < size; j++)
		res(i,j) = src(i,j);

	for(j = 0; j < i; j++)
	{
		numerator = 0;
		for(n = 0; n < size; n++)
			numerator += src(i,n) * res(j,n);
		consequent = 0;
		for(n = 0; n < size; n++)
			consequent += res(j,n) * res(j,n);
		if(consequent)
			for(n = 0; n < size; n++)
				res(i,n) = res(i,n) - (T((double)numerator/double(consequent))) * res(j,n);
		else
			for(n = 0; n < size; n++)
				res(i,n) = res(i,n);

	}
 }

 return true;
}


/// ����� ������������ � ������������� �������� ��������
template<class T>
T MDMatrix<T>::FindMinValue(void) const
{
 T res(0);
 if(Rows<=0 || Cols<=0)
  return res;

 T *pdata=Data;
 res=*pdata++;
 for(int i=1;i<Rows*Cols;i++,pdata++)
  if(res>*pdata)
   res=*pdata;
 return res;
}

template<class T>
T MDMatrix<T>::FindMaxValue(void) const
{
 T res(0);
 if(Rows<=0 || Cols<=0)
  return res;

 T *pdata=Data;
 res=*pdata++;
 for(int i=1;i<Rows*Cols;i++,pdata++)
  if(res<*pdata)
   res=*pdata;
 return res;
}
// --------------------------

// --------------------------
// ����������� ������
// --------------------------
// ������� �������
template<class T>
MDMatrix<T> MDMatrix<T>::Zero(void) const
{
 MDMatrix<T> res(Rows,Cols,T(0));

 return res;
}

// ���������� ������� ������� � 0
template<class T>
void MDMatrix<T>::ToZero(void)
{
 T* p=Data;
  for(int i=0;i<Rows*Cols;i++)
   *p++=T(0);
}

// ��������� �������
template<class T>
MDMatrix<T> MDMatrix<T>::Eye(void) const
{
 MDMatrix<T> res(Rows,Cols, T(0));

 int crmin=(Cols<Rows)?Cols:Rows;

 for(int i=0;i<crmin;i++)
  res(i,i)=1;

 return res;
}


// ���������� ������� ������� � ���������
template<class T>
void MDMatrix<T>::ToEye(void)
{
 ToZero();
 int crmin=(Cols<Rows)?Cols:Rows;

 for(int i=0;i<crmin;i++)
  (*this)(i,i)=1;
}
// --------------------------

// --------------------------
// ��������� ������
// --------------------------
template<class T>
bool MDMatrix<T>::operator == (const MDMatrix<T> &M) const
{
 if(Cols != M.Cols || Rows != M.Rows)
  return false;

 if(!Cols || !Rows)
  return true;

 if(!memcmp(Data,M.Data,sizeof(T)*Cols*Rows))
  return true;
 return false;
}

template<class T>
bool MDMatrix<T>::operator != (const MDMatrix<T> &M) const
{
 return !((*this) == M);
}
// --------------------------

template<class T>
void MDMatrix<T>::Print(std::ostream &stream)
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
	stream<<Data[i*Cols+j]<<"\t";
   }
   stream<<std::endl;
  }
 }
}

template<class T>
std::ostream& operator << (std::ostream &stream, MDMatrix<T> &m)
{
 m.Print(stream);
 return stream;
}

}

#endif
