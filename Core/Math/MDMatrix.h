#ifndef MDMatrixH
#define MDMatrixH

namespace RDK{

template<class T>
class MDMatrix
{
public:
// ������ �������
int Rows;
int Cols;
T** Data;
public:
// --------------------------
// ������������ � �����������
// --------------------------
MDMatrix(void);
MDMatrix(int rows, int cols);
MDMatrix(int rows, int cols, T defvalue);
MDMatrix(const MDMatrix<T> &copy);
MDMatrix(int rows, int cols, const T* data);
~MDMatrix();

// ������� �����������
void SetDim(int rows, int cols);
// �������� � ��������� �����������
void CheckDim(int rows, int cols);

// --------------------------
// ��������� ���������� �������
// --------------------------
// �������� ������������
const MDMatrix<T>& operator = (const MDMatrix<T> &copy);
MDMatrix<T>& operator = (T value);
MDMatrix<T>& operator = (const T* data);

// ��������� ����������� ������
int GetCols(void) const;
int GetRows(void) const;

// ������ � ��������
T& operator () (int i, int j);
const T& operator () (int i, int j) const;

// ���������� �������� ������ �������
MDMatrix<T> GetRow(int i) const;

// ���������� �������� ������� �������
MDMatrix<T> GetCol(int i) const;
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

// ������������
T Det(void) const;

// ���������� �������� �������
MDMatrix<T>& GetMinor(MDMatrix<T> &res, int row, int col) const;
MDMatrix<T> GetMinor(int row, int col) const;

// ����
T Trace(void) const;

// ����� �������
T operator !(void) const;

// ������������ �������
MDMatrix<T>& Normalize(void);
// --------------------------

// --------------------------
// ����������� ������
// --------------------------
// ������� �������
MDMatrix<T> Zero(void);

// ��������� �������
MDMatrix<T> Eye(void);
// --------------------------

// --------------------------
// ������� ������ �����
// --------------------------
protected:
};
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

// --------------------------
// ������������ � �����������
// --------------------------
template<class T>
MDMatrix<T>::MDMatrix(void)
{
	Data=NULL;
};

template<class T>
MDMatrix<T>::MDMatrix(int rows, int cols)
{
	SetDim(rows, cols);
};

template<class T>
MDMatrix<T>::MDMatrix(int rows, int cols, T defvalue)
{
	SetDim(rows, cols);
	for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
	Data[i][j]=defvalue;
};

template<class T>
MDMatrix<T>::MDMatrix(const MDMatrix<T> &copy)
{ 
	//*this=copy;
	SetDim(copy.Rows, copy.Cols);
	for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
	Data[i][j]=copy.Data[i][j]; };

template<class T>
MDMatrix<T>::MDMatrix(const int rows, const  int cols, const T* data)
{ 	SetDim(rows, cols);
	for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]=data[i*Cols+j]; };

template<class T>
MDMatrix<T>::~MDMatrix() 
{
	delete[] Data;
};
// --------------------------

// --------------------------
// ������� �����������
template<class T>
void MDMatrix<T>::SetDim(int rows, int cols)
{ 
	if(rows>0 && cols>0)
	{
		Rows=rows;
		Cols=cols; 
		Data = new T *[Rows];
		for(int i=0; i<Rows; i++)
			Data[i] = new T [Cols];
	}
};

// �������� � ��������� �����������
template<class T>
void MDMatrix<T>::CheckDim(int rows, int cols)
{ 
	if(Rows>rows && Cols>cols)
	{
		Rows=rows;
		Cols=cols;
	}
	else 
	{
		if(Rows>=rows && Cols<cols)
		{
			Cols=cols;
			Rows=rows;
			for(int i=0; i<Rows; i++)
			{
				delete[] Data[i];
				Data[i] = new T [Cols];
			}
		}
		else
		{
			delete[] Data;
			SetDim(rows, cols);
		}
	}
};
// --------------------------

// --------------------------
// ��������� ���������� �������
// --------------------------
// �������� ������������

template<class T>
const MDMatrix<T>& MDMatrix<T>::operator = (const MDMatrix<T> &copy)
{
	if(Data==NULL) SetDim(copy.Rows, copy.Cols);

	CheckDim(copy.Rows, copy.Cols);

	for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]=copy.Data[i][j];

	return *this;
};

template<class T>
MDMatrix<T>& MDMatrix<T>::operator = (T value)
{
 if(Data==NULL) SetDim(1, 1);
 for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]=value;
 return *this;
};

template<class T>
MDMatrix<T>& MDMatrix<T>::operator = (const T* data)
{
	for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]=data[i*Cols+j];
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
T& MDMatrix<T>::operator () (int i, int j)
{
 return Data[i][j];
};

template<class T>
const T& MDMatrix<T>::operator () (int i, int j) const
{
 return Data[i][j];
};
// --------------------------

// ���������� �������� ������ �������
template<class T>
MDMatrix<T> MDMatrix<T>::GetRow(int i) const
{
 MDMatrix<T> res(Cols,1);
 for(int j=0;j<Cols;j++)
  res.Data[j][0]=Data[i][j];
 return res;
};

// ���������� �������� ������� �������
template<class T>
MDMatrix<T> MDMatrix<T>::GetCol(int i) const
{
 MDMatrix<T> res(Rows,1);
 for(int j=0;j<Rows;j++)
  res.Data[j][0]=Data[j][i];
 return res;
};



// -------------------------------------------------------------------------
// ��������� ���������
// --------------------------
template<class T>
MDMatrix<T>& MDMatrix<T>::operator += (const MDMatrix<T> &M)
{
 for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]+=M.Data[i][j];

 return *this;
}

template<class T>
MDMatrix<T> operator + (const MDMatrix<T> &M1, const MDMatrix<T> &M2)
{
 MDMatrix<T> res(M1);
  for(int i=0;i<M1.Rows;i++)
		for(int j=0;j<M1.Cols;j++)
			res.Data[i][j]+=M2.Data[i][j];

 return res;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator -= (const MDMatrix<T> &M)
{
 for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]-=M.Data[i][j];

 return *this;
}

template<class T>
MDMatrix<T> operator - (const MDMatrix<T> &M1, const MDMatrix<T> &M2)
{
 MDMatrix<T> res(M1);
  for(int i=0;i<M1.Rows;i++)
		for(int j=0;j<M1.Cols;j++)
			res.Data[i][j]-=M2.Data[i][j];

 return res;
}

template<class T>
MDMatrix<T> operator * (const MDMatrix<T> &M1, const MDMatrix<T> &M2)
{
 MDMatrix<T> res(M1.Rows,M2.Cols);

 for(int j=0;j<M2.Cols;j++)
 {
  for(int k=0;k<M1.Rows;k++)
  {
   T sum=0;
   for(int i=0;i<M1.Cols;i++)
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
template<class T>
MDMatrix<T> MDMatrix<T>::operator - (void) const
{
 MDMatrix<T> res(Rows,Cols);

  for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			res.Data[i][j]=-Data[i][j];

 return res;
}

template<class T>
MDMatrix<T>& MDMatrix<T>::operator *= (T v)
{
 for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]*=v;

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
  for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]/=v;

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
 for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]+=v;

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
for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			Data[i][j]-=v;

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
	bool flag = 1;
	for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
			if(M1.Data[i][j]!=M2.Data[i][j]) flag=0;
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
	for(int i=0;i<Rows;i++)
		for(int j=0;j<Cols;j++)
		 {
			res.Data[j][i]=Data[i][j];
		}

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
 int numcombos=0;

 int i, j, k;
 T cWorkElem;

 for (i=0; i<Rows-1; i++)
 {
  cWorkElem = Data[i][i];

  if(cWorkElem == 0)
  {
   // �������� ����� ������ � ��������� ���������,
   // ����� �����, ������� ����
   for (j=i+1; i<Rows; i++)
   {
	cWorkElem = Data[j][i];
	if (cWorkElem!=0)
	{
	 // ��������� ��������� ������ � �������
	 for (k=0; k<Rows; k++)
	  Data[i][k]+=Data[j][k];
	 break;
	}
   }
  }

  if (cWorkElem!=0)
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
template<class T>
int MDMatrix<T>::TriangleBareis(void)
{
    T denom(1); //!
	int exchanges(0);

	for(int l1=0; l1<Rows-1; ++l1)
	{ //��������� ��� ������ �������, ����� ���������
		int maxN=l1;
		T maxValue=fabs(Data[l1][l1]);
		for(int l2=l1+1; l2<Rows; ++l2)
		{ //������ ������ � ������������ �� ������ ���������
			T const value=fabs(Data[l2][l1]);
			if( value > maxValue ) { maxN=l2; maxValue=value; }
		}

        if( maxN > l1 )
		{ //����� �����
		 for(int i=0;i<Cols;i++)
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

		for(int l2=l1+1; l2<Rows; ++l2)
		{ //������� ������ �� ���� �����������
			T const value2=Data[l2][l1]; //!
			Data[l2][l1] = T(0);
			for(int c=l1+1; c<Rows; ++c) //!
				Data[l2][c]=(Data[l2][c]*value1-Data[l1][c]*value2)/denom;
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
 T det=Det();

 if(!det)
  return res;

 // get the determinant of a
 det = 1.0/det;

 MDMatrix<T> Minor(Rows-1,Cols-1);

 for(int j=0;j<Rows;j++)
 {
  for(int i=0;i<Cols;i++)
  {
   // get the co-factor (matrix) of A(j,i)
   GetMinor(Minor,j,i);
   res.Data[i][j] = det*Minor.Det();
   if( (i+j)%2 == 1)
	res.Data[i][j] = -res.Data[i][j];
   }
  }

 return res;
}

template<class T>
MDMatrix<T> MDMatrix<T>::Inverse(void) const
{
 MDMatrix<T> res(Cols,Rows);

 return Inverse(res);
}

// ������������
template<class T>
T MDMatrix<T>::Det(void) const
{
 MDMatrix<T> Temp(*this);

 int numcombos=Temp.TriangleBareis();

 T det = Temp.Data[Rows-1][Cols-1];
// for(int i=0;i<Rows;i++)
//  det*=Temp.Data[i][i];

 return (numcombos%2)?-det:det;

// if(Rows == 2)
//  return Data[0][0]*Data[1][1]-Data[0][1]*Data[1][0];

// if(Rows == 1)
//  return Data[0][0];

 // the determinant value
// T det = 0;

// for(int i = 0; i < Rows; i++ )
// {
  // get minor of element (0,i)
 // MDMatrix<T,Cols-1,Rows-1> Res;

 // GetMinor( Res, 0, i);

  // the recusion is here!
 // det += (i%2==1?-1.0:1.0) * Data[0][i] * (Res.GetRows()>1)?Res.Det():Res.Data[0][0];
  //det += pow( -1.0, i ) * mat[0][i] * CalcDeterminant( minor,order-1 );
// }
 //return det;    
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
					res.Data[colCount][rowCount] = Data[i][j];
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

// ����
template<class T>
T MDMatrix<T>::Trace(void) const
{
 int crmin=(Cols<Rows)?Cols:Rows;

 T sum=0;
 for(int i=0;i<crmin;i++)
  sum+=Data[i][i];

 return sum;
}

// ����� �������
template<class T>
T MDMatrix<T>::operator !(void) const
{
 T res=0;
  for(int i=0;i<Rows;i++)
	for(int j=0;j<Cols;j++)
		res+=Data[i][j] * Data[i][j];
 return sqrt(res);
}

// ������������ �������
template<class T>
MDMatrix<T>& MDMatrix<T>::Normalize(void)
{
 T norm=this->operator!();

 if(!norm)
  return *this;

 for(int i=0;i<Rows;i++)
	for(int j=0;j<Cols;j++)
		Data[i][j]/=norm;

 return *this;
}
// --------------------------

// --------------------------
// ����������� ������
// --------------------------
// ������� �������
template<class T>
MDMatrix<T> MDMatrix<T>::Zero(void)
{
 MDMatrix<T> res(Rows,Cols);

  for(int i=0;i<Rows;i++)
	for(int j=0;j<Cols;j++)
		res.Data[i][j]=0;

 return res;
}

// ��������� �������
template<class T>
MDMatrix<T> MDMatrix<T>::Eye(void)
{
 MDMatrix<T> res(Rows,Cols);
  for(int i=0;i<Rows;i++)
	for(int j=0;j<Cols;j++)
		res.Data[i][j]=0;

 int crmin=(Cols<Rows)?Cols:Rows;

 for(int i=0;i<crmin;i++)
  res.Data[i][i]=1;

 return res;
}



// --------------------------

}
#endif