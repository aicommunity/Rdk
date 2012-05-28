/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MGeometryH
#define MGeometryH

#include <vector>
#include <string>
#include <stddef.h>
//#include "MTheormec.h"
#include "MVector.h"

namespace RDK {

template<class T, int Rows>
class MRay
{
public: // ������
// ������ ����
MVector<T,Rows> Origin;

// ����������� ����
MVector<T,Rows> Direction;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MRay(void)
{};
MRay(const MRay &copy)
{ *this=copy; };
MRay(MVector<T,Rows> &o, MVector<T,Rows> &d)
{ Origin=o; Direction=d; };
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
inline MRay& operator = (const MRay &copy)
{
 Origin=copy.Origin;
 Direction=copy.Direction;
 return *this;
}

// ��������� ���������
inline bool operator == (const MRay &v) const
{
 return (Origin == v.Origin) & (Direction == v.Direction);
}

inline bool operator != (const MRay &v) const
{
 return !operator == (v);
}
// --------------------------

// --------------------------
// ������ �����
// --------------------------
// ��������� ������ �����, ��������� ����� ���� �� ���������� t
inline MVector<T,Rows> CalcPoint(T t)
{ return Origin+Direction*t; };

// ��������� ����� ����������� �����
// ���������� true ���� ����������� ����������
bool CalcIntersection(const MRay<T,Rows> &r, MVector<T,Rows> &res)
{
 T t;
 MVector<T,Rows> v1,v2;
 int i;

 v1=r.Origin-Origin;
 v2=Direction-r.Direction;
 if(!v1 == 0)
  return false;

 for(i=0;i<3;i++)
  if(v1[i] != 0)
  {
   t=v1[i]/v2[i];
   break;
  }

 for(i=0;i<3;i++)
  if(v2[i]*t != v1[i])
   return false;

 res=CalcPoint(t);
 return true;
}
// --------------------------
};

// ����� ���������
template<class T, int Rows>
class MPlane
{
public: // ������
// ������� � ���������
MVector<T,Rows> Normal;

// �������� ����� ��������� ����� ������� �� ������ ���������
T Distance;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MPlane(void)
{ Normal=0; Distance=0; };
MPlane(const MPlane &copy)
{ *this=copy; };
MPlane(MVector<T,Rows> n, T dist)
{ Normal=n; Distance=dist; };
virtual ~MPlane(void){};
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
inline MPlane& operator = (const MPlane &copy)
{
 Normal=copy.Normal;
 Distance=copy.Distance;
 return *this;
}

// ��������� ���������
inline bool operator == (const MPlane &v) const
{
 return (Normal == v.Normal) & (Distance == v.Distance);
}

inline bool operator != (const MPlane &v) const
{
 return !operator == (v);
}
// --------------------------

// --------------------------
// ������ �����
// --------------------------
// ��������� ����� ����������� ���� � ����������
// ���������� true ���� ����������� ����������
bool CalcIntersection(MRay<T,Rows> ray, MVector<T,Rows> &p)
{
 T alfa,beta,t;

 if( (alfa=Normal&ray.dir)==0 ) return false;
 beta=-(Normal&ray.org)+Distance;
 if( (t=beta/alfa)<0 ) return false;

 p=ray.CalcPoint(t);
 return true;
};

// --------------------------
};


// ������ ������
class MBorder
{
protected: // ������
// ������� ������
std::vector<int> VertexIndex;

protected: // ��������� ����������
// ����� ������
size_t NumVertex;


// ��������� ������� �������
int* PVertexIndex;



public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MBorder(void);
MBorder(size_t numvertex);
MBorder(const MBorder &copy);
MBorder(const std::vector<int> &copy);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
void Clear(void);

// ������� ������
const std::vector<int>& GetVertexIndex(void) const;
bool SetVertexIndex(const std::vector<int> &copy);

// ����� ������
size_t GetNumVertex(void) const;
bool SetNumVertex(size_t value);
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
MBorder& operator = (const MBorder &copy);
MBorder& operator = (const std::vector<int> &copy);

// �������� ������� � �������� ������
int& operator [] (int index);

// �������� ������� � ������� �������
inline int& operator () (void);

// ��������� ���������������� ������� �������
inline int& operator = (int index);
inline int& operator += (int index);
inline int& operator -= (int index);
inline int& operator ++ (int);
inline int& operator ++ (void);
inline int& operator -- (int);
inline int& operator -- (void);

// ��������� ���������
bool operator == (const MBorder &v) const;
bool operator != (const MBorder &v) const;
// --------------------------

// --------------------------
// ��������� �������������� � ������� ���������
// --------------------------
// ����� � ������
friend unsigned char* operator >> (const MBorder &v, unsigned char* p);

// ���� �� �������
friend const unsigned char* operator << (MBorder &v, const unsigned char* p);
// --------------------------
};

// ����� ����� �������
template<class T, int Rows>
class MVertex
{
protected: // ������
// ������� ������
std::vector<MVector<T,Rows> > Vertex;

// ����� ������
std::vector<std::string> Names;

protected: // ��������� ����������
// ����� ������
size_t NumVertex;

// ��������� �� ������� �������
MVector<T,Rows>* PVertex;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MVertex(void);
MVertex(const MVertex &copy);
MVertex(const std::vector<MVector<T,Rows> > &copy);
MVertex(const std::vector<std::string> &copy);
MVertex(size_t na);
virtual ~MVertex(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
void Clear(void);

// ������� ������
const std::vector<MVector<T,Rows> >& GetVertex(void) const;

// ����� ������
const std::vector<std::string>& GetNames(void) const;
bool SetNames(const std::vector<std::string> &copy);

// ��� �������
std::string& UseName(size_t index);

// ������� ������
bool SetVertexVector(const std::vector<MVector<T,Rows> > &copy);

// ����� ������
size_t GetNumVertex(void) const;
bool SetNumVertex(size_t value);
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
MVertex& operator = (const MVertex &copy);
MVertex& operator = (const std::vector<MVector<T,Rows> > &copy);
MVertex& operator = (const std::vector<std::string> &copy);

// �������� ������� � �������� ������
inline MVector<T,Rows>& operator [] (int index);

// �������� ������� � ������� �������
inline MVector<T,Rows>& operator () (void);

// ��������� ���������������� ������� �������
inline MVector<T,Rows>& operator = (int index);
inline MVector<T,Rows>& operator += (int index);
inline MVector<T,Rows>& operator -= (int index);
inline MVector<T,Rows>& operator ++ (int);
inline MVector<T,Rows>& operator ++ (void);
inline MVector<T,Rows>& operator -- (int);
inline MVector<T,Rows>& operator -- (void);

// ��������� ���������
bool operator == (const MVertex &v) const;
bool operator != (const MVertex &v) const;
// --------------------------

// --------------------------
// ��������� �������������� � ������� ���������
// --------------------------
// ����� � ������
template<class U> friend unsigned char* operator >> (const MVertex<U,Rows> &v, unsigned char* p);

// ���� �� �������
template<class U> friend const unsigned char* operator << (MVertex<U,Rows> &v, const unsigned char* p);

// ������� �������� P
//MVertex& operator *= (const MRotationTensor<T> &P);

// ���������� �� ������ v
//MVertex& operator += (const MVector<T,Rows> &v);

// ���������� �� ������ -v
//MVertex& operator -= (const MVector<T,Rows> &v);
// --------------------------
};

// ��������� �������
template<class T, int Rows>
class MGeometry
{
protected: // ������
// ������� ������
MVertex<T,Rows> Vertex;

// ������� �������
std::vector<MBorder> Borders;

protected: // ��������� ����������
// ����� ������
size_t NumBorders;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MGeometry(void);
MGeometry(const MGeometry<T,Rows> &copy);
MGeometry(size_t na, size_t nf);
virtual ~MGeometry(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
void Clear(void);

// ������� ������
const MVertex<T,Rows>& GetVertex(void) const;
bool SetVertex(const MVertex<T,Rows> &copy);

// ������� �������
const std::vector<MBorder>& GetBorders(void) const;
bool SetBorders(const std::vector<MBorder> &copy);

// ����� ������
size_t GetNumBorders(void) const;
bool SetNumBorders(size_t value);
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
MGeometry<T,Rows>& operator = (const MGeometry<T,Rows> &copy);

// �������� ������� � �������� ������� �� ������� n � ������� ������
MBorder& operator[] (int n);

// �������� ������� � �������� ������� �� ������� n � ������� ��������
inline MVertex<T,Rows>& operator() (void);

// ��������� ���������
bool operator == (const MGeometry<T,Rows> &v) const;
bool operator != (const MGeometry<T,Rows> &v) const;
// --------------------------

// --------------------------
// ��������� �������������� � ������� ���������
// --------------------------
// ����� � ������
template<class U> friend unsigned char* operator >> (const MGeometry<U,Rows> &v, unsigned char* p);

// ���� �� �������
template<class U> friend const unsigned char* operator << (MGeometry<U,Rows> &v, const unsigned char* p);

// ������� �������� P
//MGeometry<T,Rows>& operator *= (const MRotationTensor<T> &P);

// ���������� �� ������ v
//MGeometry<T,Rows>& operator += (const MVector<T,Rows> &v);

// ���������� �� ������ -v
//MGeometry<T,Rows>& operator -= (const MVector<T,Rows> &v);
// --------------------------
};


// ****************************************************************************
// ****************************************************************************
// ���������� �������
// ****************************************************************************
// ****************************************************************************

// MVertex<T>
// --------------------------
// ������������ � �����������
// --------------------------
template<class T, int Rows>
MVertex<T,Rows>::MVertex(void)
{
 NumVertex=0;
 PVertex=0;
}

template<class T, int Rows>
MVertex<T,Rows>::MVertex(const MVertex<T,Rows> &copy)
{
 *this=copy;
}

template<class T, int Rows>
MVertex<T,Rows>::MVertex(const std::vector<MVector<T,Rows> > &copy)
{
 *this=copy;
}

template<class T, int Rows>
MVertex<T,Rows>::MVertex(const std::vector<std::string> &copy)
{
 *this=copy;
}

template<class T, int Rows>
MVertex<T,Rows>::MVertex(size_t na)
{
 NumVertex=0;
 PVertex=0;
 SetNumVertex(na);
}

template<class T, int Rows>
MVertex<T,Rows>::~MVertex(void)
{
 NumVertex=0;
 PVertex=0;
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
template<class T, int Rows>
void MVertex<T,Rows>::Clear(void)
{
 Vertex.clear();
 Names.clear();
 PVertex=0;
 NumVertex=0;
}

// ������� ������
template<class T, int Rows>
const std::vector<MVector<T,Rows> >& MVertex<T,Rows>::GetVertex(void) const
{
 return Vertex;
}

// ����� ������
template<class T, int Rows>
const std::vector<std::string>& MVertex<T,Rows>::GetNames(void) const
{
 return Names;
}

template<class T, int Rows>
bool MVertex<T,Rows>::SetNames(const std::vector<std::string> &copy)
{
   SetNumVertex(copy.size());
   Names=copy;
   return true;
}

// ��� �������
template<class T, int Rows>
std::string& MVertex<T,Rows>::UseName(size_t index)
{
 return Names[index];
}

// ������� ������
template<class T, int Rows>
bool MVertex<T,Rows>::SetVertexVector(const std::vector<MVector<T,Rows> > &copy)
{
   SetNumVertex(copy.size());
   Vertex=copy;
   return true;
}

// ����� ������
template<class T, int Rows>
size_t MVertex<T,Rows>::GetNumVertex(void) const
{
 return NumVertex;
}

template<class T, int Rows>
bool MVertex<T,Rows>::SetNumVertex(size_t value)
{
 if(NumVertex == value)
  return true;

 ptrdiff_t curr=0;
 if(PVertex)
  curr=PVertex-&Vertex[0];

 Vertex.resize(value);
 Names.resize(value);

 if(!value)
  PVertex=0;
 else
 if(curr < ptrdiff_t(value))
  PVertex=&Vertex[0]+curr;
 else
  PVertex=&Vertex[value-1];

 NumVertex=value;
 return true;
}
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
template<class T, int Rows>
MVertex<T,Rows>& MVertex<T,Rows>::operator = (const MVertex<T,Rows> &copy)
{
 Vertex=copy.Vertex;
 NumVertex=copy.NumVertex;
 Names=copy.Names;

 if(NumVertex > 0)
 {
  ptrdiff_t curr=copy.PVertex-&copy.Vertex[0];
  PVertex=&Vertex[0]+curr;
 }
 else
  PVertex=0;
 return *this;
}

template<class T, int Rows>
MVertex<T,Rows>& MVertex<T,Rows>::operator = (const std::vector<MVector<T,Rows> > &copy)
{
 ptrdiff_t curr=0;
 if(NumVertex)
  curr=PVertex-&Vertex[0];

 Vertex=copy;
 NumVertex=Vertex.size();
 Names.resize(NumVertex);

 if(!NumVertex)
  PVertex=0;
 else
 if(curr<ptrdiff_t(NumVertex))
  PVertex=&Vertex[0]+curr;
 else
  PVertex=&Vertex[0];
 return *this;
}

template<class T, int Rows>
MVertex<T,Rows>& MVertex<T,Rows>::operator = (const std::vector<std::string> &copy)
{
 SetNumVertex(copy.size());
 Names=copy;
 return *this;
}

// �������� ������� � �������� ������
template<class T, int Rows>
inline MVector<T,Rows>& MVertex<T,Rows>::operator [] (int index)
{
 PVertex=&Vertex[index];
 return *PVertex;
}

// �������� ������� � ������� �������
template<class T, int Rows>
inline MVector<T,Rows>& MVertex<T,Rows>::operator () (void)
{
 return *PVertex;
}

// ��������� ���������������� ������� �������
template<class T, int Rows>
inline MVector<T,Rows>& MVertex<T,Rows>::operator = (int index)
{
 return operator [] (index);
}

template<class T, int Rows>
inline MVector<T,Rows>& MVertex<T,Rows>::operator += (int index)
{
 PVertex+=index;
 return *PVertex;
}

template<class T, int Rows>
inline MVector<T,Rows>& MVertex<T,Rows>::operator -= (int index)
{
 PVertex-=index;
 return *PVertex;
}

template<class T, int Rows>
inline MVector<T,Rows>& MVertex<T,Rows>::operator ++ (int)
{
 return *PVertex++;
}

template<class T, int Rows>
inline MVector<T,Rows>& MVertex<T,Rows>::operator ++ (void)
{
 return *(++PVertex);
}

template<class T, int Rows>
inline MVector<T,Rows>& MVertex<T,Rows>::operator -- (int)
{
 return *PVertex--;
}

template<class T, int Rows>
inline MVector<T,Rows>& MVertex<T,Rows>::operator -- (void)
{
 return *(--PVertex);
}

// ��������� ���������
template<class T, int Rows>
bool MVertex<T,Rows>::operator == (const MVertex<T,Rows> &v) const
{
 return (Vertex == v.Vertex);
}

template<class T, int Rows>
bool MVertex<T,Rows>::operator != (const MVertex<T,Rows> &v) const
{
 return (Vertex != v.Vertex);
}
// --------------------------

// --------------------------
// ��������� �������������� � ������� ���������
// --------------------------
// ����� � ������
template<class T, int Rows>
unsigned char* operator >> (const MVertex<T,Rows> &v, unsigned char* p)
{
 if(!p)
  return 0;

 memcpy(p,&v.NumVertex,sizeof(v.NumVertex)); p+=sizeof(v.NumVertex);
 for(size_t i=0;i<v.NumVertex;i++)
  p=v.Vertex[i]>>p;
 for(size_t i=0;i<v.NumVertex;i++)
 {
  size_t size=v.Names[i].size();
  memcpy(p,&size,sizeof(size)); p+=sizeof(size);
  if(size>0)
   memcpy(p,v.Names[i].c_str(),size*sizeof(char)); p+=size*sizeof(char);
 }

 return p;
}

// ���� �� �������
template<class T, int Rows>
const unsigned char* operator << (MVertex<T,Rows> &v, const unsigned char* p)
{
 if(!p)
  return 0;

 size_t temp;
 memcpy(&temp,p,sizeof(v.NumVertex)); p+=sizeof(v.NumVertex);
 v.SetNumVertex(temp);
 for(size_t i=0;i<v.NumVertex;i++)
  p=v.Vertex[i]<<p;
 for(size_t i=0;i<v.NumVertex;i++)
 {
  size_t size;
  memcpy(&size,p,sizeof(size)); p+=sizeof(size);
  v.Names[i].resize(size);
  if(size>0)
   memcpy(&v.Names[i][0],p,size*sizeof(char)); p+=size*sizeof(char);
 }
 return p;
}
/*
// ������� �������� P
template<class T, int Rows>
MVertex<T,Rows>& MVertex<T,Rows>::operator *= (const MRotationTensor<T> &P)
{
 if(!NumVertex)
  return *this;

 MVector<T,Rows>* pv=&Vertex[0];
 for(size_t i=0;i<NumVertex;i++,pv++)
 {
  (*pv)=P*(*pv);
 }

 return *this;
}

// ���������� �� ������ v
template<class T, int Rows>
MVertex<T,Rows>& MVertex<T,Rows>::operator += (const MVector<T,Rows> &v)
{
 if(!NumVertex)
  return *this;

 MVector<T,Rows>* pv=&Vertex[0];
 for(size_t i=0;i<NumVertex;i++,pv++)
 {
  (*pv)+=v;
 }

 return *this;
}

// ���������� �� ������ -v
template<class T, int Rows>
MVertex<T,Rows>& MVertex<T,Rows>::operator -= (const MVector<T,Rows> &v)
{
 if(!NumVertex)
  return *this;

 MVector<T,Rows>* pv=&Vertex[0];
 for(size_t i=0;i<NumVertex;i++,pv++)
 {
  (*pv)-=v;
 }

 return *this;
}              */
// --------------------------


// MGeometry
// --------------------------
// ������������ � �����������
// --------------------------
template<class T, int Rows>
MGeometry<T,Rows>::MGeometry(void)
{
 // ����� ������
 NumBorders=0;
}

template<class T, int Rows>
MGeometry<T,Rows>::MGeometry(const MGeometry &copy)
{
 *this=copy;
}

template<class T, int Rows>
MGeometry<T,Rows>::MGeometry(size_t na, size_t nf)
{
 Vertex.SetNumVertex(na);
 SetNumBorders(nf);
}

template<class T, int Rows>
MGeometry<T,Rows>::~MGeometry(void)
{
 NumBorders=0;
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
template<class T, int Rows>
void MGeometry<T,Rows>::Clear(void)
{
 Vertex.Clear();
 Borders.clear();
}

// ������� ������
template<class T, int Rows>
const MVertex<T,Rows>& MGeometry<T,Rows>::GetVertex(void) const
{
 return Vertex;
}

template<class T, int Rows>
bool MGeometry<T,Rows>::SetVertex(const MVertex<T,Rows> &copy)
{
 Vertex=copy;
 return true;
}

// ������� �������
template<class T, int Rows>
const std::vector<MBorder>& MGeometry<T,Rows>::GetBorders(void) const
{
 return Borders;
}

template<class T, int Rows>
bool MGeometry<T,Rows>::SetBorders(const std::vector<MBorder> &copy)
{
 Borders=copy;
 NumBorders=Borders.size();
 return true;
}

// ����� ������
template<class T, int Rows>
size_t MGeometry<T,Rows>::GetNumBorders(void) const
{
 return NumBorders;
}

template<class T, int Rows>
bool MGeometry<T,Rows>::SetNumBorders(size_t value)
{
 if(NumBorders == value)
  return true;

 Borders.resize(value);
 NumBorders=value;
 return true;
}
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
template<class T, int Rows>
MGeometry<T,Rows>& MGeometry<T,Rows>::operator = (const MGeometry<T,Rows> &copy)
{
 // ������� ������
 Vertex=copy.Vertex;

 // ������� �������
 Borders=copy.Borders;

 // ����� ������
 NumBorders=copy.NumBorders;

 return *this;
}

// �������� ������� � �������� ������� �� ������� n � ������� ������
template<class T, int Rows>
MBorder& MGeometry<T,Rows>::operator [] (int n)
{
 return Borders[n];
};

// �������� ������� � ������� ������� �� ������� n � ������� ��������
template<class T, int Rows>
inline MVertex<T,Rows>& MGeometry<T,Rows>::operator () (void)
{
 return Vertex;
}

// ��������� ���������
template<class T, int Rows>
bool MGeometry<T,Rows>::operator == (const MGeometry<T,Rows> &v) const
{
 return (Vertex == v.Vertex) & (Borders == v.Borders);
}

template<class T, int Rows>
bool MGeometry<T,Rows>::operator != (const MGeometry<T,Rows> &v) const
{
 return (Vertex != v.Vertex) | (Borders != v.Borders);
}
// --------------------------


// --------------------------
// ��������� �������������� � ������� ���������
// --------------------------
// ����� � ������
template<class T, int Rows>
unsigned char* operator >> (const MGeometry<T,Rows> &v, unsigned char* p)
{
 if(!p)
  return 0;

 p=operator >> (v.Vertex,p);

 memcpy(p,&v.NumBorders,sizeof(v.NumBorders));
 p+=sizeof(v.NumBorders);
 for(size_t i=0;i<v.NumBorders;i++)
  p=v.Borders[i]>>p;

 return p;
}

// ���� �� �������
template<class T, int Rows>
const unsigned char* operator << (MGeometry<T,Rows> &v, const unsigned char* p)
{
 if(!p)
  return 0;

 p=operator << (v.Vertex,p);

 size_t temp;
 memcpy(&temp,p,sizeof(v.NumBorders));
 p+=sizeof(temp);
 v.SetNumBorders(temp);
 for(size_t i=0;i<v.NumBorders;i++)
  p=v.Borders[i]<<p;

 return p;
}
/*
// ������� �������� P
template<class T, int Rows>
MGeometry<T,Rows>& MGeometry<T,Rows>::operator *= (const MRotationTensor<T> &P)
{
 Vertex*=P;
 return *this;
}

// ���������� �� ������ v
template<class T, int Rows>
MGeometry<T,Rows>& MGeometry<T,Rows>::operator += (const MVector<T,Rows> &v)
{
 Vertex+=v;
 return *this;
}

// ���������� �� ������ -v
template<class T, int Rows>
MGeometry<T,Rows>& MGeometry<T,Rows>::operator -= (const MVector<T,Rows> &v)
{
 Vertex-=v;
 return *this;
}               */
// --------------------------
}
#endif
