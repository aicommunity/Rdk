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
#include <cstddef>
#include "MTheormec.h"

namespace RDK {

template<class T>
class MRay
{
public: // ������
// ������ ����
MVector<T> Origin;

// ����������� ����
MVector<T> Direction;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MRay(void)
{};
MRay(const MRay &copy)
{ *this=copy; };
MRay(MVector<T> &o, MVector<T> &d)
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
inline MVector<T> CalcPoint(T t)
{ return Origin+Direction*t; };

// ��������� ����� ����������� �����
// ���������� true ���� ����������� ����������
bool CalcIntersection(const MRay<T> &r, MVector<T> &res)
{
 T t;
 MVector<T> v1,v2;
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
template<class T>
class MPlane
{
public: // ������
// ������� � ���������
MVector<T> Normal;

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
MPlane(MVector<T> n, T dist)
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
bool CalcIntersection(MRay<T> ray, MVector<T> &p)
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

//friend USerStorageXML& operator << (USerStorageXML& storage, const MBorder &data);
//friend USerStorageXML& operator >> (USerStorageXML& storage, MBorder &data);
// --------------------------
};

// ����� ����� �������
template<class T>
class MVertex
{
protected: // ������
// ������� ������
std::vector<MVector<T> > Vertex;

// ����� ������
std::vector<std::string> Names;

protected: // ��������� ����������
// ����� ������
size_t NumVertex;

// ��������� �� ������� �������
MVector<T>* PVertex;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MVertex(void);
MVertex(const MVertex &copy);
MVertex(const std::vector<MVector<T> > &copy);
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
const std::vector<MVector<T> >& GetVertex(void) const;

// ����� ������
const std::vector<std::string>& GetNames(void) const;
bool SetNames(const std::vector<std::string> &copy);

// ��� �������
std::string& UseName(size_t index);

// ������� ������
bool SetVertexVector(const std::vector<MVector<T> > &copy);

// ����� ������
size_t GetNumVertex(void) const;
bool SetNumVertex(size_t value);
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
MVertex& operator = (const MVertex &copy);
MVertex& operator = (const std::vector<MVector<T> > &copy);
MVertex& operator = (const std::vector<std::string> &copy);

// �������� ������� � �������� ������
inline MVector<T>& operator [] (int index);

// �������� ������� � ������� �������
inline MVector<T>& operator () (void);

// ��������� ���������������� ������� �������
inline MVector<T>& operator = (int index);
inline MVector<T>& operator += (int index);
inline MVector<T>& operator -= (int index);
inline MVector<T>& operator ++ (int);
inline MVector<T>& operator ++ (void);
inline MVector<T>& operator -- (int);
inline MVector<T>& operator -- (void);

// ��������� ���������
bool operator == (const MVertex &v) const;
bool operator != (const MVertex &v) const;
// --------------------------

// --------------------------
// ��������� �������������� � ������� ���������
// --------------------------
// ����� � ������
friend unsigned char* operator >> (const MVertex &v, unsigned char* p);

// ���� �� �������
friend const unsigned char* operator << (MVertex &v, const unsigned char* p);

// ������� �������� P
MVertex& operator *= (const MRotationTensor<T> &P);

// ���������� �� ������ v
MVertex& operator += (const MVector<T> &v);

// ���������� �� ������ -v
MVertex& operator -= (const MVector<T> &v);
// --------------------------
};

// ��������� �������
template<class T>
class MGeometry
{
protected: // ������
// ������� ������
MVertex<T> Vertex;

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
MGeometry(const MGeometry<T> &copy);
MGeometry(size_t na, size_t nf);
virtual ~MGeometry(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
void Clear(void);

// ������� ������
const MVertex<T>& GetVertex(void) const;
bool SetVertex(const MVertex<T> &copy);

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
MGeometry<T>& operator = (const MGeometry<T> &copy);

// �������� ������� � �������� ������� �� ������� n � ������� ������
MBorder& operator[] (int n);

// �������� ������� � �������� ������� �� ������� n � ������� ��������
inline MVertex<T>& operator() (void);

// ��������� ���������
bool operator == (const MGeometry<T> &v) const;
bool operator != (const MGeometry<T> &v) const;
// --------------------------

// --------------------------
// ��������� �������������� � ������� ���������
// --------------------------
// ����� � ������
friend unsigned char* operator >> (const MGeometry<T> &v, unsigned char* p);

// ���� �� �������
friend const unsigned char* operator << (MGeometry<T> &v, const unsigned char* p);

// ������� �������� P
MGeometry<T>& operator *= (const MRotationTensor<T> &P);

// ���������� �� ������ v
MGeometry<T>& operator += (const MVector<T> &v);

// ���������� �� ������ -v
MGeometry<T>& operator -= (const MVector<T> &v);
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
template<class T>
MVertex<T>::MVertex(void)
{
 NumVertex=0;
 PVertex=0;
}

template<class T>
MVertex<T>::MVertex(const MVertex<T> &copy)
{
 *this=copy;
}

template<class T>
MVertex<T>::MVertex(const std::vector<MVector<T> > &copy)
{
 *this=copy;
}

template<class T>
MVertex<T>::MVertex(const std::vector<std::string> &copy)
{
 *this=copy;
}

template<class T>
MVertex<T>::MVertex(size_t na)
{
 NumVertex=0;
 PVertex=0;
 SetNumVertex(na);
}

template<class T>
MVertex<T>::~MVertex(void)
{
 NumVertex=0;
 PVertex=0;
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
template<class T>
void MVertex<T>::Clear(void)
{
 Vertex.clear();
 Names.clear();
 PVertex=0;
 NumVertex=0;
}

// ������� ������
template<class T>
const std::vector<MVector<T> >& MVertex<T>::GetVertex(void) const
{
 return Vertex;
}

// ����� ������
template<class T>
const std::vector<std::string>& MVertex<T>::GetNames(void) const
{
 return Names;
}

template<class T>
bool MVertex<T>::SetNames(const std::vector<std::string> &copy)
{
   SetNumVertex(copy.size());
   Names=copy;
   return true;
}

// ��� �������
template<class T>
std::string& MVertex<T>::UseName(size_t index)
{
 return Names[index];
}

// ������� ������
template<class T>
bool MVertex<T>::SetVertexVector(const std::vector<MVector<T> > &copy)
{
   SetNumVertex(copy.size());
   Vertex=copy;
   return true;
}

// ����� ������
template<class T>
size_t MVertex<T>::GetNumVertex(void) const
{
 return NumVertex;
}

template<class T>
bool MVertex<T>::SetNumVertex(size_t value)
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
template<class T>
MVertex<T>& MVertex<T>::operator = (const MVertex<T> &copy)
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

template<class T>
MVertex<T>& MVertex<T>::operator = (const std::vector<MVector<T> > &copy)
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

template<class T>
MVertex<T>& MVertex<T>::operator = (const std::vector<std::string> &copy)
{
 SetNumVertex(copy.size());
 Names=copy;
 return *this;
}

// �������� ������� � �������� ������
template<class T>
inline MVector<T>& MVertex<T>::operator [] (int index)
{
 PVertex=&Vertex[index];
 return *PVertex;
}

// �������� ������� � ������� �������
template<class T>
inline MVector<T>& MVertex<T>::operator () (void)
{
 return *PVertex;
}

// ��������� ���������������� ������� �������
template<class T>
inline MVector<T>& MVertex<T>::operator = (int index)
{
 return operator [] (index);
}

template<class T>
inline MVector<T>& MVertex<T>::operator += (int index)
{
 PVertex+=index;
 return *PVertex;
}

template<class T>
inline MVector<T>& MVertex<T>::operator -= (int index)
{
 PVertex-=index;
 return *PVertex;
}

template<class T>
inline MVector<T>& MVertex<T>::operator ++ (int)
{
 return *PVertex++;
}

template<class T>
inline MVector<T>& MVertex<T>::operator ++ (void)
{
 return *(++PVertex);
}

template<class T>
inline MVector<T>& MVertex<T>::operator -- (int)
{
 return *PVertex--;
}

template<class T>
inline MVector<T>& MVertex<T>::operator -- (void)
{
 return *(--PVertex);
}

// ��������� ���������
template<class T>
bool MVertex<T>::operator == (const MVertex<T> &v) const
{
 return (Vertex == v.Vertex);
}

template<class T>
bool MVertex<T>::operator != (const MVertex<T> &v) const
{
 return (Vertex != v.Vertex);
}
// --------------------------

// --------------------------
// ��������� �������������� � ������� ���������
// --------------------------
// ����� � ������
template<class T>
unsigned char* operator >> (const MVertex<T> &v, unsigned char* p)
{
 if(!p)
  return 0;

 std::memcpy(p,&v.NumVertex,sizeof(v.NumVertex)); p+=sizeof(v.NumVertex);
 for(size_t i=0;i<v.NumVertex;i++)
  p=v.Vertex[i]>>p;
 for(size_t i=0;i<v.NumVertex;i++)
 {
  size_t size=v.Names[i].size();
  std::memcpy(p,&size,sizeof(size)); p+=sizeof(size);
  if(size>0)
   std::memcpy(p,v.Names[i].c_str(),size*sizeof(char)); p+=size*sizeof(char);
 }

 return p;
}

// ���� �� �������
template<class T>
const unsigned char* operator << (MVertex<T> &v, const unsigned char* p)
{
 if(!p)
  return 0;

 size_t temp;
 std::memcpy(&temp,p,sizeof(v.NumVertex)); p+=sizeof(v.NumVertex);
 v.SetNumVertex(temp);
 for(size_t i=0;i<v.NumVertex;i++)
  p=v.Vertex[i]<<p;
 for(size_t i=0;i<v.NumVertex;i++)
 {
  size_t size;
  std::memcpy(&size,p,sizeof(size)); p+=sizeof(size);
  v.Names[i].resize(size);
  if(size>0)
   std::memcpy(&v.Names[i][0],p,size*sizeof(char)); p+=size*sizeof(char);
 }
 return p;
}

// ������� �������� P
template<class T>
MVertex<T>& MVertex<T>::operator *= (const MRotationTensor<T> &P)
{
 if(!NumVertex)
  return *this;

 MVector<T>* pv=&Vertex[0];
 for(size_t i=0;i<NumVertex;i++,pv++)
 {
  (*pv)=P*(*pv);
 }

 return *this;
}

// ���������� �� ������ v
template<class T>
MVertex<T>& MVertex<T>::operator += (const MVector<T> &v)
{
 if(!NumVertex)
  return *this;

 MVector<T>* pv=&Vertex[0];
 for(size_t i=0;i<NumVertex;i++,pv++)
 {
  (*pv)+=v;
 }

 return *this;
}

// ���������� �� ������ -v
template<class T>
MVertex<T>& MVertex<T>::operator -= (const MVector<T> &v)
{
 if(!NumVertex)
  return *this;

 MVector<T>* pv=&Vertex[0];
 for(size_t i=0;i<NumVertex;i++,pv++)
 {
  (*pv)-=v;
 }

 return *this;
}
// --------------------------


// MGeometry
// --------------------------
// ������������ � �����������
// --------------------------
template<class T>
MGeometry<T>::MGeometry(void)
{
 // ����� ������
 NumBorders=0;
}

template<class T>
MGeometry<T>::MGeometry(const MGeometry &copy)
{
 *this=copy;
}

template<class T>
MGeometry<T>::MGeometry(size_t na, size_t nf)
{
 Vertex.SetNumVertex(na);
 SetNumBorders(nf);
}

template<class T>
MGeometry<T>::~MGeometry(void)
{
 NumBorders=0;
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
template<class T>
void MGeometry<T>::Clear(void)
{
 Vertex.Clear();
 Borders.clear();
}

// ������� ������
template<class T>
const MVertex<T>& MGeometry<T>::GetVertex(void) const
{
 return Vertex;
}

template<class T>
bool MGeometry<T>::SetVertex(const MVertex<T> &copy)
{
 Vertex=copy;
 return true;
}

// ������� �������
template<class T>
const std::vector<MBorder>& MGeometry<T>::GetBorders(void) const
{
 return Borders;
}

template<class T>
bool MGeometry<T>::SetBorders(const std::vector<MBorder> &copy)
{
 Borders=copy;
 NumBorders=Borders.size();
 return true;
}

// ����� ������
template<class T>
size_t MGeometry<T>::GetNumBorders(void) const
{
 return NumBorders;
}

template<class T>
bool MGeometry<T>::SetNumBorders(size_t value)
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
template<class T>
MGeometry<T>& MGeometry<T>::operator = (const MGeometry<T> &copy)
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
template<class T>
MBorder& MGeometry<T>::operator [] (int n)
{
 return Borders[n];
};

// �������� ������� � ������� ������� �� ������� n � ������� ��������
template<class T>
inline MVertex<T>& MGeometry<T>::operator () (void)
{
 return Vertex;
}

// ��������� ���������
template<class T>
bool MGeometry<T>::operator == (const MGeometry<T> &v) const
{
 return (Vertex == v.Vertex) & (Borders == v.Borders);
}

template<class T>
bool MGeometry<T>::operator != (const MGeometry<T> &v) const
{
 return (Vertex != v.Vertex) | (Borders != v.Borders);
}
// --------------------------


// --------------------------
// ��������� �������������� � ������� ���������
// --------------------------
// ����� � ������
template<class T>
unsigned char* operator >> (const MGeometry<T> &v, unsigned char* p)
{
 if(!p)
  return 0;

 p=operator >> (v.Vertex,p);

 std::memcpy(p,&v.NumBorders,sizeof(v.NumBorders));
 p+=sizeof(v.NumBorders);
 for(size_t i=0;i<v.NumBorders;i++)
  p=v.Borders[i]>>p;

 return p;
}

// ���� �� �������
template<class T>
const unsigned char* operator << (MGeometry<T> &v, const unsigned char* p)
{
 if(!p)
  return 0;

 p=operator << (v.Vertex,p);

 size_t temp;
 std::memcpy(&temp,p,sizeof(v.NumBorders));
 p+=sizeof(temp);
 v.SetNumBorders(temp);
 for(size_t i=0;i<v.NumBorders;i++)
  p=v.Borders[i]<<p;

 return p;
}

// ������� �������� P
template<class T>
MGeometry<T>& MGeometry<T>::operator *= (const MRotationTensor<T> &P)
{
 Vertex*=P;
 return *this;
}

// ���������� �� ������ v
template<class T>
MGeometry<T>& MGeometry<T>::operator += (const MVector<T> &v)
{
 Vertex+=v;
 return *this;
}

// ���������� �� ������ -v
template<class T>
MGeometry<T>& MGeometry<T>::operator -= (const MVector<T> &v)
{
 Vertex-=v;
 return *this;
}
// --------------------------
}
#endif
