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

namespace RDK{

template<class T, unsigned Rows, unsigned Cols=Rows>
class MMatrix
{
public:
// Данные матрицы
union {
T Data[Rows][Cols];
T Data1D[1];
struct {
T x,y,z,d;
};
};

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MMatrix(void);
MMatrix(T defvalue);
MMatrix(const MMatrix<T,Rows,Cols> &copy);
MMatrix(const T* data);
~MMatrix(void);
// --------------------------

// --------------------------
// Операторы управления данными
// --------------------------
// Оператор присваивания
MMatrix<T,Rows,Cols>& operator = (const MMatrix<T,Rows,Cols> &copy);
//MMatrix<T,Rows,Cols>& operator = (const T* data);
MMatrix<T,Rows,Cols>& operator = (T value);
MMatrix<T,Rows,Cols>& operator = (const T data[Rows][Cols]);
MMatrix<T,Rows,Cols>& operator = (const T* data);

// Получение размерности матриц
unsigned GetCols(void) const;
unsigned GetRows(void) const;

// Доступ к элементу
T& operator () (int i, int j);
const T& operator () (int i, int j) const;

// Возвращает заданную строку матрицы
MMatrix<T,Cols,1> GetRow(int i) const;

// Возвращает заданный столбец матрицы
MMatrix<T,Rows,1> GetCol(int i) const;
// --------------------------

// --------------------------
// Матричные операторы
// --------------------------
MMatrix<T,Rows,Cols>& operator += (const MMatrix<T,Rows,Cols> &M);
MMatrix<T,Rows,Cols>& operator -= (const MMatrix<T,Rows,Cols> &M);
// --------------------------

// --------------------------
// Скалярные операторы
// --------------------------
MMatrix<T,Rows,Cols> operator - (void) const;

MMatrix<T,Rows,Cols>& operator *= (T v);

MMatrix<T,Rows,Cols>& operator /= (T v);

MMatrix<T,Rows,Cols>& operator += (T v);

MMatrix<T,Rows,Cols>& operator -= (T v);
// --------------------------

// --------------------------
// Преобразования матриц
// --------------------------
// Транспонирование
MMatrix<T,Cols,Rows>& Transpose(MMatrix<T,Cols,Rows> &res) const;
MMatrix<T,Cols,Rows> Transpose(void) const;

// Приведение матрицы к верхней треугольной форме
// Возрващает число перестановок
unsigned TriangleGauss(void);
unsigned TriangleBareis(void);

// Инвертирование
MMatrix<T,Rows,Cols>& Inverse(MMatrix<T,Cols,Rows> &res) const;
MMatrix<T,Rows,Cols> Inverse(void) const;

// Дискриминант
T Det(void) const;

// Вычисление минорной матрицы
MMatrix<T,Rows-1,Cols-1>& GetMinor(MMatrix<T,Rows-1,Cols-1> &res, unsigned row, unsigned col) const;
MMatrix<T,Rows-1,Cols-1> GetMinor(unsigned row, unsigned col) const;

// След
T Trace(void) const;

// Норма матрицы
T operator !(void) const;

// Нормализация матрицы
MMatrix<T,Rows,Cols>& Normalize(void);
// --------------------------

// --------------------------
// Определения матриц
// --------------------------
// Нулевая матрица
static MMatrix<T,Rows,Cols> Zero(void);

// Единичная матрица
static MMatrix<T,Rows,Cols> Eye(void);
// --------------------------

// --------------------------
// Скрытые методы счета
// --------------------------
protected:

};

// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(void)
{
};

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(T defvalue)
{
 if(defvalue == 0)
  memset(Data1D,0,Rows*Cols*sizeof(T));
 else
  for(int i=0;i<Rows*Cols;i++)
   Data1D[i]=defvalue;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(const MMatrix<T,Rows,Cols> &copy)
{ *this=copy; };
/*
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::MMatrix(const T* data)
{ *copy=data; };
  */
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>::~MMatrix(void) {};
// --------------------------

// --------------------------
// Операторы управления данными
// --------------------------
// Оператор присваивания
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator = (const MMatrix<T,Rows,Cols> &copy)
{
 memcpy(Data1D,copy.Data1D,sizeof(T)*Cols*Rows);
 return *this;
};
	 /*
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator = (const T* data)
{
 memcpy(Data1D,data,sizeof(T)*Cols*Rows);
 return *this;
};     */

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator = (T value)
{
 T* pm1=Data1D;

 for(int i=0;i<Cols*Rows;i++)
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
 T* pm1=Data1D;
 for(unsigned i=0;i<Cols*Rows;i++)
  *pm1++ = *data++;
 return *this;
}


// Получение размерности матриц
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

// Доступ к элементу
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

// Возвращает заданную строку матрицы
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Cols,1> MMatrix<T,Rows,Cols>::GetRow(int i) const
{
 MMatrix<T,Cols,1> res;
 for(int j=0;j<Cols;j++)
  res.Data[j][0]=Data[i][j];
 return res;
}

// Возвращает заданный столбец матрицы
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
// Матричные операторы
// --------------------------
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator += (const MMatrix<T,Rows,Cols> &M)
{
 T* pm1=Data1D;
 const T* pm2=M.Data1D;

 for(int i=0;i<Cols*Rows;i++)
  *pm1++ += *pm2++;

 return *this;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator + (const MMatrix<T,Rows,Cols> &M1, const MMatrix<T,Rows,Cols> &M2)
{
 MMatrix<T,Rows,Cols> res=M1;
 T* pm1=res.Data1D;
 const T* pm2=M2.Data1D;

 for(unsigned i=0;i<Cols*Rows;i++)
  *pm1++ += *pm2++;

 return res;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator -= (const MMatrix<T,Rows,Cols> &M)
{
 T* pm1=Data1D;
 const T* pm2=M.Data1D;

 for(unsigned i=0;i<Cols*Rows;i++)
  *pm1++ -= *pm2++;

 return *this;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> operator - (const MMatrix<T,Rows,Cols> &M1, const MMatrix<T,Rows,Cols> &M2)
{
 MMatrix<T,Rows,Cols> res=M1;
 T* pm1=res.Data1D;
 const T* pm2=M2.Data1D;

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
// Скалярные операторы
// --------------------------
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> MMatrix<T,Rows,Cols>::operator - (void) const
{
 MMatrix<T,Rows,Cols> res;

 const T* pm1=Data1D;
 T* pm2=res.Data1D;
 for(unsigned i=0;i<Cols*Rows;i++)
  *pm2++ = -*pm1++;

 return res;
}

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::operator *= (T v)
{
 T* pm1=Data1D;
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
 T* pm1=Data1D;
 for(int i=0;i<Cols*Rows;i++)
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
 T* pm1=Data1D;
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
 T* pm1=Data1D;
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
// Операторы сравнения
// --------------------------
// Оператор присваивания
template<class T, unsigned Rows, unsigned Cols>
bool operator == (const MMatrix<T,Rows,Cols> &M1, const MMatrix<T,Rows,Cols> &M2)
{
 return (memcmp(M1.Data1D,M2.Data1D,Cols*Rows*sizeof(T)) == 0)?true:false;
}

template<class T, unsigned Rows, unsigned Cols>
bool operator != (const MMatrix<T,Rows,Cols> &M1, const MMatrix<T,Rows,Cols> &M2)
{
 return (memcmp(M1.Data1D,M2.Data1D,Cols*Rows*sizeof(T)) != 0)?true:false;
}
// --------------------------


// --------------------------
// Преобразования матриц
// --------------------------
// Транспонирование
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

// Приведение матрицы к верхней треугольной форме
// Возрващает число перестановок
template<class T, unsigned Rows, unsigned Cols>
unsigned MMatrix<T,Rows,Cols>::TriangleGauss(void)
{
 unsigned numcombos=0;

 int i, j, k;
 T cWorkElem;

 for (i=0; i<Rows-1; i++)
 {
  cWorkElem = Data[i][i];

  if(cWorkElem == 0)
  {
   // пытаемся найти строку с ненулевым элементом,
   // среди строк, лежащих ниже
   for (j=i+1; i<Rows; i++)
   {
	cWorkElem = Data[j][i];
	if (cWorkElem!=0)
	{
	 // добавляем найденную строку к рабочей
	 for (k=0; k<Rows; k++)
	  Data[i][k]+=Data[j][k];
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
	 Data[j][k] -= Data[i][k] * Data[j][i] / cWorkElem;
   }
  }
 }

 return numcombos;
}


// Приведение матрицы к верхней треугольной форме
// Возрващает число перестановок
template<class T, unsigned Rows, unsigned Cols>
unsigned MMatrix<T,Rows,Cols>::TriangleBareis(void)
{
    T denom(1); //!
	int exchanges(0);

	for(int l1=0; l1<Rows-1; ++l1)
	{ //Перебираю все строки матрицы, кроме последней
		int maxN=l1;
		T maxValue=fabs(Data[l1][l1]);
		for(int l2=l1+1; l2<Rows; ++l2)
		{ //Нахожу строку с максимальным по модулю элементом
			T const value=fabs(Data[l2][l1]);
			if( value > maxValue ) { maxN=l2; maxValue=value; }
		}

        if( maxN > l1 )
		{ //Нужен обмен
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
		} else { //Обмен не нужен, но нужна проверка на ноль
            if(maxValue == T(0)) return exchanges;
        }

		T const value1=Data[l1][l1]; //!
		  //!

		for(int l2=l1+1; l2<Rows; ++l2)
		{ //Вычитаю строку из всех последующих
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


// Инвертирование
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::Inverse(MMatrix<T,Cols,Rows> &res) const
{
 T det=Det();

 if(!det)
  return res;

 // get the determinant of a
 det = 1.0/det;

 MMatrix<T,Rows-1,Cols-1> Minor;

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

template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> MMatrix<T,Rows,Cols>::Inverse(void) const
{
 MMatrix<T,Rows,Cols> res(0.0);

 return Inverse(res);
}

// Дискриминант
template<class T, unsigned Rows, unsigned Cols>
T MMatrix<T,Rows,Cols>::Det(void) const
{
 MMatrix<T,Rows,Cols> Temp=*this;

 unsigned numcombos=Temp.TriangleBareis();

 T det = Temp.Data[Rows-1][Cols-1];
// for(unsigned i=0;i<Rows;i++)
//  det*=Temp.Data[i][i];

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

// Вычисление минорной матрицы
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

// След
template<class T, unsigned Rows, unsigned Cols>
T MMatrix<T,Rows,Cols>::Trace(void) const
{
 unsigned crmin=(Cols<Rows)?Cols:Rows;

 T sum=0;
 for(unsigned i=0;i<crmin;i++)
  sum+=Data[i][i];

 return sum;
}

// Норма матрицы
template<class T, unsigned Rows, unsigned Cols>
T MMatrix<T,Rows,Cols>::operator !(void) const
{
 T res=0;
 const T* pm1=Data1D;
 for(unsigned i=0;i<Cols*Rows;i++,pm1++)
  res+=*pm1 * *pm1;
 return sqrt(res);
}

// Нормализация матрицы
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols>& MMatrix<T,Rows,Cols>::Normalize(void)
{
 T norm=this->operator!();

 if(!norm)
  return *this;

 T* pm1=Data1D;
 for(int i=0;i<Cols*Rows;i++,pm1++)
  *pm1/=norm;

 return *this;
}

// Возвращает часть матрицы заданной величины
// начиная с позиции i0,j0 в исходной матрицы
template<class T, unsigned Rows, unsigned Cols, unsigned Rows2, unsigned Cols2>
MMatrix<T,Rows2,Cols2>& GetSubMatrix(MMatrix<T,Rows,Cols>& source,unsigned i0, unsigned j0, MMatrix<T,Rows2,Cols2>& res)
{
 unsigned cols2=(Cols2<Cols)?Cols2:Cols;
 unsigned rows2=(Rows2<Rows)?Rows2:Rows;
 unsigned work_cols=((Cols-i0)<=cols2)?cols2:Cols-i0;
 unsigned work_rows=((Rows-i0)<=rows2)?rows2:Rows-j0;

 for(unsigned i=0;i<work_cols;i++)
  for(unsigned j=0;j<work_rows;j++)
  {
   res.Data[j][i]=source.Data[j+j0][i+i0];
  }

 return res;
}
// Модифицирует часть матрицы заданной величины
// начиная с позиции i0,j0 в исправляемой матрице
template<class T, unsigned Rows, unsigned Cols, unsigned Rows2, unsigned Cols2>
MMatrix<T,Rows,Cols>& SetSubMatrix(MMatrix<T,Rows,Cols>& dest,unsigned i0, unsigned j0, MMatrix<T,Rows2,Cols2>& source)
{
 unsigned cols2=(Cols2<Cols)?Cols2:Cols;
 unsigned rows2=(Rows2<Rows)?Rows2:Rows;
 unsigned work_cols=((Cols-i0)<=cols2)?cols2:Cols-i0;
 unsigned work_rows=((Rows-i0)<=rows2)?rows2:Rows-j0;

 for(unsigned i=0;i<work_cols;i++)
  for(unsigned j=0;j<work_rows;j++)
  {
   dest.Data[j0+j][i0+i]=source.Data[j][i];
  }

 return dest;
}
// --------------------------

// --------------------------
// Определения матриц
// --------------------------
// Нулевая матрица
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> MMatrix<T,Rows,Cols>::Zero(void)
{
 MMatrix<T,Rows,Cols> res;

 memset(res.Data1D,0,Rows*Cols*sizeof(T));
 return res;
}

// Единичная матрица
template<class T, unsigned Rows, unsigned Cols>
MMatrix<T,Rows,Cols> MMatrix<T,Rows,Cols>::Eye(void)
{
 MMatrix<T,Rows,Cols> res;
 memset(res.Data1D,0,Rows*Cols*sizeof(T));

 unsigned crmin=(Cols<Rows)?Cols:Rows;

 for(unsigned i=0;i<crmin;i++)
  res.Data[i][i]=1;

 return res;
}
// --------------------------




}
#endif
