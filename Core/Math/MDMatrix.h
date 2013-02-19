#ifndef MDMatrixH
#define MDMatrixH

namespace RDK{

template<class T>
class MDMatrix
{
public:
union
{
 T* Data;
 T* Data1D;
 struct
 {
  T x,y,z,d;
 };
};

protected:
// Данные матрицы
int Rows;
int Cols;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MDMatrix(void);
MDMatrix(int rows, int cols);
MDMatrix(int rows, int cols, T defvalue);
MDMatrix(const MDMatrix<T> &copy);
MDMatrix(int rows, int cols, const T* data);
~MDMatrix();

// Задание размерности
void Resize(int rows, int cols);

// --------------------------
// Операторы управления данными
// --------------------------
// Оператор присваивания
const MDMatrix<T>& operator = (const MDMatrix<T> &copy);
MDMatrix<T>& operator = (T value);
MDMatrix<T>& operator = (const T* data);

// Получение размерности матриц
int GetCols(void) const;
int GetRows(void) const;

// Доступ к элементу
T& operator [] (int i);
const T& operator [] (int i) const;

T& operator () (int i, int j);
const T& operator () (int i, int j) const;

// Возвращает заданную строку матрицы
MDMatrix<T> GetRow(int i) const;

// Возвращает заданный столбец матрицы
MDMatrix<T> GetCol(int i) const;
// --------------------------

// --------------------------
// Матричные операторы
// --------------------------
MDMatrix<T>& operator += (const MDMatrix<T> &M);
MDMatrix<T>& operator -= (const MDMatrix<T> &M);
// --------------------------

// --------------------------
// Скалярные операторы
// --------------------------
MDMatrix<T> operator - (void) const;

MDMatrix<T>& operator *= (T v);

MDMatrix<T>& operator /= (T v);

MDMatrix<T>& operator += (T v);

MDMatrix<T>& operator -= (T v);
// --------------------------

// --------------------------
// Преобразования матриц
// --------------------------
// Транспонирование
MDMatrix<T>& Transpose(MDMatrix<T> &res) const;
MDMatrix<T> Transpose(void) const;

// Приведение матрицы к верхней треугольной форме
// Возрващает число перестановок
int TriangleGauss(void);
int TriangleBareis(void);

// Инвертирование
MDMatrix<T>& Inverse(MDMatrix<T> &res) const;
MDMatrix<T> Inverse(void) const;

// Дискриминант
T Det(void) const;

// Вычисление минорной матрицы
MDMatrix<T>& GetMinor(MDMatrix<T> &res, int row, int col) const;
MDMatrix<T> GetMinor(int row, int col) const;

// След
T Trace(void) const;

// Норма матрицы
T operator !(void) const;

// Нормализация матрицы
MDMatrix<T>& Normalize(void);
// --------------------------

// --------------------------
// Определения матриц
// --------------------------
// Нулевая матрица
MDMatrix<T> Zero(void);

// Единичная матрица
MDMatrix<T> Eye(void);
// --------------------------

};
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<class T>
MDMatrix<T>::MDMatrix(void)
: Rows(0),Cols(0),Data(0)
{
};

template<class T>
MDMatrix<T>::MDMatrix(int rows, int cols)
: Rows(0),Cols(0),Data(0)
{
	Resize(rows, cols);
};

template<class T>
MDMatrix<T>::MDMatrix(int rows, int cols, T defvalue)
: Rows(0),Cols(0),Data(0)
{
 Resize(rows, cols);
 for(int i=0;i<Rows*Cols;i++)
   Data[i]=defvalue;
};

template<class T>
MDMatrix<T>::MDMatrix(const MDMatrix<T> &copy)
: Rows(0),Cols(0),Data(0)
{
 *this=copy;
};

template<class T>
MDMatrix<T>::MDMatrix(const int rows, const  int cols, const T* data)
: Rows(0),Cols(0),Data(0)
{
 Resize(rows, cols);
 *this=data;
};

template<class T>
MDMatrix<T>::~MDMatrix()
{
 if(Data)
  delete[] Data;
};
// --------------------------

// --------------------------
// Задание размерности
template<class T>
void MDMatrix<T>::Resize(int rows, int cols)
{
 if(rows == Rows && cols == Cols)
  return;

 if(rows<0 || cols<0)
  return;

 T* new_data=0;
 if(rows && cols)
 {
  new_data = new T[rows*cols];
  int c_rows=(Rows<rows)?Rows:rows;
  int c_cols=(Cols<cols)?Cols:cols;
  for(int i=0; i<c_rows; i++)
   for(int j=0;j<c_cols; j++)
    new_data[i*cols+j]=Data[i*Cols+j];
 }
 delete []Data;
 Data=new_data;
 Rows=rows;
 Cols=cols;
};
// --------------------------

// --------------------------
// Операторы управления данными
// --------------------------
// Оператор присваивания

template<class T>
const MDMatrix<T>& MDMatrix<T>::operator = (const MDMatrix<T> &copy)
{
 Resize(copy.Rows, copy.Cols);
 memcpy(Data,copy.Data,Rows*Cols*sizeof(T));
 return *this;
};

template<class T>
MDMatrix<T>& MDMatrix<T>::operator = (T value)
{
 T *p=Data;
 for(int i=0;i<Rows*Cols;i++)
   *p++=value;
 return *this;
};

template<class T>
MDMatrix<T>& MDMatrix<T>::operator = (const T* data)
{
 memcpy(Data,data,Rows*Cols*sizeof(T));
 return *this;
}

// Получение размерности матриц
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
// Доступ к элементу
template<class T>
T& MDMatrix<T>::operator [] (int i)
{
 return Data1D[i];
}

template<class T>
const T& MDMatrix<T>::operator [] (int i) const
{
 return Data1D[i];
}

template<class T>
T& MDMatrix<T>::operator () (int i, int j)
{
 return Data[i*Cols+j];
};

template<class T>
const T& MDMatrix<T>::operator () (int i, int j) const
{
 return Data[i*Cols+j];
};
// --------------------------

// Возвращает заданную строку матрицы
template<class T>
MDMatrix<T> MDMatrix<T>::GetRow(int i) const
{
 MDMatrix<T> res(1,Cols);
 for(int j=0;j<Cols;j++)
  res.Data[j]=Data[i*Cols+j];
 return res;
};

// Возвращает заданный столбец матрицы
template<class T>
MDMatrix<T> MDMatrix<T>::GetCol(int i) const
{
 MDMatrix<T> res(Rows,1);
 for(int j=0;j<Rows;j++)
  res.Data[j]=Data[j*Cols+i];
 return res;
};



// -------------------------------------------------------------------------
// Матричные операторы
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
// Скалярные операторы
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
// Операторы сравнения
// --------------------------
// Оператор присваивания
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
// Преобразования матриц
// --------------------------
// Транспонирование
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

// Приведение матрицы к верхней треугольной форме
// Возрващает число перестановок
template<class T>
int MDMatrix<T>::TriangleGauss(void)
{
 int numcombos=0;

 int i, j, k;
 T cWorkElem;

 for (i=0; i<Rows-1; i++)
 {
  cWorkElem = (*this)(i,i);

  if(cWorkElem == 0)
  {
   // пытаемся найти строку с ненулевым элементом,
   // среди строк, лежащих ниже
   for (j=i+1; i<Rows; i++)
   {
	cWorkElem = (*this)(j,i);
	if (cWorkElem!=0)
	{
	 // добавляем найденную строку к рабочей
	 for (k=0; k<Rows; k++)
	  (*this)(i,k)+=(*this)(j,k);
	 break;
	}
   }
  }

  if (cWorkElem!=0)
  {
   // рабочий элемент корректен -
   // обрабатываем все нижлежащие строки
   for (j=i+1; j<Rows; j++)
   {
	for (k=Rows-1;k>=i;k--)
	 (*this)(j,k) -= (*this)(i,k) * (*this)(j,i) / cWorkElem;
   }
  }
 }

 return numcombos;
}


// Приведение матрицы к верхней треугольной форме
// Возрващает число перестановок
template<class T>
int MDMatrix<T>::TriangleBareis(void)
{
    T denom(1); //!
	int exchanges(0);

	for(int l1=0; l1<Rows-1; ++l1)
	{ //Перебираю все строки матрицы, кроме последней
		int maxN=l1;
		T maxValue=fabs((*this)(l1,l1));
		for(int l2=l1+1; l2<Rows; ++l2)
		{ //Нахожу строку с максимальным по модулю элементом
			T const value=fabs((*this)(l2,l1));
			if( value > maxValue ) { maxN=l2; maxValue=value; }
		}

        if( maxN > l1 )
		{ //Нужен обмен
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
		} else { //Обмен не нужен, но нужна проверка на ноль
            if(maxValue == T(0)) return exchanges;
        }

		T const value1=(*this)(l1,l1); //!
		  //!

		for(int l2=l1+1; l2<Rows; ++l2)
		{ //Вычитаю строку из всех последующих
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


// Инвертирование
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
   res(i,j) = det*Minor.Det();
   if( (i+j)%2 == 1)
	res(i,j) = -res(i,j);
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

// Дискриминант
template<class T>
T MDMatrix<T>::Det(void) const
{
 MDMatrix<T> Temp(*this);

 int numcombos=Temp.TriangleBareis();

 T det = Temp(Rows-1,Cols-1);

 return (numcombos%2)?-det:det;
}

// Вычисление минорной матрицы
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

// След
template<class T>
T MDMatrix<T>::Trace(void) const
{
 int crmin=(Cols<Rows)?Cols:Rows;

 T sum=0;
 for(int i=0;i<crmin;i++)
  sum+=(*this)(i,i);

 return sum;
}

// Норма матрицы
template<class T>
T MDMatrix<T>::operator !(void) const
{
 T res=0;
  for(int i=0;i<Rows;i++)
	for(int j=0;j<Cols;j++)
		res+=(*this)(i,j) * (*this)(i,j);
 return sqrt(res);
}

// Нормализация матрицы
template<class T>
MDMatrix<T>& MDMatrix<T>::Normalize(void)
{
 T norm=this->operator!();

 if(!norm)
  return *this;

 (*this)/=norm;

 return *this;
}
// --------------------------

// --------------------------
// Определения матриц
// --------------------------
// Нулевая матрица
template<class T>
MDMatrix<T> MDMatrix<T>::Zero(void)
{
 MDMatrix<T> res(Rows,Cols);

 res=T(0);

 return res;
}

// Единичная матрица
template<class T>
MDMatrix<T> MDMatrix<T>::Eye(void)
{
 MDMatrix<T> res(Rows,Cols);
 res=T(0);

 int crmin=(Cols<Rows)?Cols:Rows;

 for(int i=0;i<crmin;i++)
  res(i,i)=1;

 return res;
}



// --------------------------

}
#endif
