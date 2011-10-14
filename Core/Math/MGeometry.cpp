/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MGEOMETRY_CPP
#define MGEOMETRY_CPP
//#include <stdio.h>
#include <cstring>
#include "MGeometry.h"

namespace RDK {


// MBorder
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MBorder::MBorder(void)
{
 NumVertex=0;
 PVertexIndex=0;
}

MBorder::MBorder(size_t numvertex)
{
 NumVertex=0;
 PVertexIndex=0;
 SetNumVertex(numvertex);
}

MBorder::MBorder(const MBorder &copy)
{
 *this=copy;
}

MBorder::MBorder(const std::vector<int> &copy)
{
 *this=copy;
}

// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает фигуру
void MBorder::Clear(void)
{
 NumVertex=0;
 PVertexIndex=0;
 VertexIndex.clear();
}

// Индексы вершин
const std::vector<int>& MBorder::GetVertexIndex(void) const
{
 return VertexIndex;
}

// Число вершин
size_t MBorder::GetNumVertex(void) const
{
 return NumVertex;
}

bool MBorder::SetNumVertex(size_t value)
{
 if(NumVertex == value)
  return true;

 ptrdiff_t curr=0;
 if(PVertexIndex)
  curr=PVertexIndex-&VertexIndex[0];

 VertexIndex.resize(value);

 if(!value)
  PVertexIndex=0;
 else
 if(curr < ptrdiff_t(value))
  PVertexIndex=&VertexIndex[0]+curr;
 else
  PVertexIndex=&VertexIndex[value-1];

 NumVertex=value;
 return true;
}
// --------------------------

// --------------------------
// Операторы управления
// --------------------------
// Оператор копирования
MBorder& MBorder::operator = (const MBorder &copy)
{
 VertexIndex=copy.VertexIndex;
 NumVertex=copy.NumVertex;

 if(NumVertex > 0)
 {
  ptrdiff_t curr=copy.PVertexIndex-&copy.VertexIndex[0];
  PVertexIndex=&VertexIndex[0]+curr;
 }
 else
  PVertexIndex=0;
 return *this;
}

MBorder& MBorder::operator = (const std::vector<int> &copy)
{
 ptrdiff_t curr=0;
 if(NumVertex)
  curr=PVertexIndex-&VertexIndex[0];

 VertexIndex=copy;
 NumVertex=VertexIndex.size();

 if(!NumVertex)
  PVertexIndex=0;
 else
 if(curr<ptrdiff_t(NumVertex))
  PVertexIndex=&VertexIndex[0]+curr;
 else
  PVertexIndex=&VertexIndex[0];
 return *this;
}

// Оператор доступа к индексам вершин
int& MBorder::operator [] (int index)
{
 PVertexIndex=&VertexIndex[index];
 return *PVertexIndex;
}

// Оператор доступа к текущей вершине
inline int& MBorder::operator () (void)
{
 return *PVertexIndex;
}

// Операторы позиционирования текущей вершины
inline int& MBorder::operator = (int index)
{
 return operator [] (index);
}

inline int& MBorder::operator += (int index)
{
 PVertexIndex+=index;
 return *PVertexIndex;
}

inline int& MBorder::operator -= (int index)
{
 PVertexIndex-=index;
 return *PVertexIndex;
}

inline int& MBorder::operator ++ (int)
{
 return *(PVertexIndex++);
}

inline int& MBorder::operator ++ (void)
{
 return *(++PVertexIndex);
}

inline int& MBorder::operator -- (int)
{
 return *(PVertexIndex--);
}

inline int& MBorder::operator -- (void)
{
 return *(--PVertexIndex);
}

// Операторы сравнения
bool MBorder::operator == (const MBorder &v) const
{
 return (VertexIndex == v.VertexIndex);
}

bool MBorder::operator != (const MBorder &v) const
{
 return (VertexIndex != v.VertexIndex);
}
// --------------------------

// --------------------------
// Операторы взаимодействия с другими объектами
// --------------------------
// Вывод в массив
unsigned char* operator >> (const MBorder &v, unsigned char* p)
{
 if(!p)
  return 0;

 std::memcpy(p,&v.NumVertex,sizeof(v.NumVertex)); p+=sizeof(v.NumVertex);
 if(v.NumVertex)
  std::memcpy(p,&v.VertexIndex[0],sizeof(double)*v.NumVertex);
 return p+sizeof(double)*v.NumVertex;
}

// Ввод из массива
const unsigned char* operator << (MBorder &v, const unsigned char* p)
{
 if(!p)
  return 0;

 size_t temp;
 std::memcpy(&temp,p,sizeof(v.NumVertex)); p+=sizeof(v.NumVertex);
 v.SetNumVertex(temp);
 if(v.NumVertex)
  std::memcpy(&v.VertexIndex[0],p,sizeof(double)*v.NumVertex);
 return p+sizeof(double)*v.NumVertex;
}
// --------------------------

}

#endif
