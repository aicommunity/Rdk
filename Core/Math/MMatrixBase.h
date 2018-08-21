#ifndef MMatrixBaseH
#define MMatrixBaseH

#include <typeinfo>
#include <vector>
#include "../Utilities/UException.h"

namespace RDK {

// Исключения
/// Общее исключение матричных операций
class EMatrixError: public EError
{
public:
EMatrixError(void) : EError() {};
};

/// Нулевой определитель. Исключение генерируется при обращении матриц и т.п.
class EMatrixZeroDet: public EMatrixError
{
public:
EMatrixZeroDet(void) : EMatrixError() {};
};

/// Попытка деления на ноль
class EMatrixZeroDiv: public EMatrixError
{
public:
EMatrixZeroDiv(void) : EMatrixError() {};
};

/// Domain error
class EMatrixDomainError: public EMatrixError
{
public:
EMatrixDomainError(void) : EMatrixError() {};
};

class RDK_LIB_TYPE MMatrixSize
{
private: // Данные
std::vector<int> Dims;
public: // Методы
// --------------------------
// Конструкторы и деструкторы
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
};

MMatrixSize(const MMatrixSize &copy);
explicit MMatrixSize(const std::vector<int> &dims);
// --------------------------

// --------------------------
// Операторы
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

// Возвращает размерность
int GetDimensions(void) const;

/// Возвращает сырые данные размерности
const std::vector<int> GetDims(void) const;
// --------------------------
};

class RDK_LIB_TYPE MMatrixBase
{
public: // Данные

public: // Методы
// -----------------------------------
// Общие методы доступа к параметрам матрицы
// -----------------------------------
/// Возвращает размерность матрицы
virtual int GetDimensions(void) const=0;

/// Возвращает число элементов по стороне матрицы
virtual int GetSize(int i) const=0;

/// Возвращает суммраное число всех элементов
virtual int GetSize(void) const=0;

/// Возвращает true если суммарное число всех элементов == 0
virtual bool IsEmpty(void) const=0;

/// Возвращает число элементов по всем размерностям
virtual MMatrixSize GetMatrixSize(void) const=0;

/// Устанавливает число элементов по всем размерностям
virtual bool Resize(const MMatrixSize &size)=0;

/// Возвращает суммарный размер данных матрицы в байтах
virtual int GetByteSize(void) const=0;

/// Возвращает длину в байтах одного элемента даных
virtual int GetElementByteSize(void) const=0;

/// Методы доступа к данным
virtual const void* GetVoid(void) const=0;
virtual void* GetVoid(void)=0;

// Возвращает языковой тип элемента матрицы
//virtual const type_info& GetLanguageType(void) const=0;

// Метод сравнивает матрицы по типу элементов
//bool CompareLanguageType(const MMatrixBase &dt) const;

/// Копирует данные в другую матрицу
virtual bool CopyTo(MMatrixBase &dest) const;


/// Предоставляет доступ к данным матрицы как к одномерному массиву выбранного
/// типа. Небезопасно!
template<typename U>
U& As(int i)
{
 return reinterpret_cast<U*>(GetVoid())[i];
};


template<typename U>
const U& As(int i) const
{
 return reinterpret_cast<U*>(GetVoid())[i];
};
// -----------------------------------
};


}
#endif
