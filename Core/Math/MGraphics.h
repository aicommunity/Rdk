/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MGraphicsH
#define MGraphicsH

#include <vector>
#include "../Graphics/UAGraphics.h"
#include "../Graphics/UBitmap.h"
#include "MGeometry.h"

namespace RDK {

// ����� �������� ������������ ����������� ��������� �������
class MGeometryDescription
{
public: // ������
// ����
UColorT Color;

// ������� ����
int PenWidth;

// ���
string Name;

// ��������
string Description;

// ���� ���������
bool Visible;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MGeometryDescription(void)
{
 // ����
 Color=UColorT(0,0,255,0);

 // ������� ����
 PenWidth=4;

 // ���� ���������
 Visible=true;
};

MGeometryDescription(const MGeometryDescription &copy)
{
 // ����
 Color=copy.Color;

 // ������� ����
 PenWidth=copy.PenWidth;

 // ���
 Name=copy.Name;

 // ��������
 Description=copy.Description;

 // ���� ���������
 Visible=copy.Visible;
};

virtual ~MGeometryDescription(void)
{
};
// --------------------------

};

// ����� ��������� ��������� �������
template<class T>
class MGraphics
{
protected: // ������
// ������ ���������
UAGraphics *Graphics;

// ������ �������� ��������� � �������� �������
std::vector<MGeometry<T> > Geometry;

// ������ �������� ���������������� �������
vector<MGeometryDescription> Description;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MGraphics(void);
MGraphics(const MGraphics<T> &copy);
~MGraphics(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ������ ���������
UAGraphics* GetGraphics(void) const;
bool SetGraphics(UAGraphics *graphics);

// ���������� �������� ������ ��������� �� �������
MGeometry<T>& GetGeometry(size_t index);

// ���������� �������� ��������� ������� ��������� �� �������
MGeometryDescription& GetDescription(size_t index);

// ���������� ����� �������� ���������
size_t GetNumGeometry(void) const;

// ��������� ����� ������ ���������
// ���������� ������ ������������ �������
size_t AddGeometry(const MGeometry<T>& geometry);

// ������� ������������ ������ ��������� �� �������
void DelGeometry(size_t index);

// ������� ��� ������� ��������� �� �������
void DelAllGeometry(void);
// --------------------------

// --------------------------
// ������ ���������
// --------------------------
// ������� ������ ���������
void Clear(void);

// ������������ ��� �������
void Repaint(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
// �� �������� ������ ���������
MGraphics<T>& operator = (const MGraphics<T> &copy);
// --------------------------
};


// --------------------------
// ������������ � �����������
// --------------------------
template<class T>
MGraphics<T>::MGraphics(void)
{
 // ������ ���������
 Graphics=0;
}

template<class T>
MGraphics<T>::MGraphics(const MGraphics<T> &copy)
{
 *this=copy;
}

template<class T>
MGraphics<T>::~MGraphics(void)
{
 Graphics=0;
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ������ ���������
template<class T>
UAGraphics* MGraphics<T>::GetGraphics(void) const
{
 return Graphics;
}

template<class T>
bool MGraphics<T>::SetGraphics(UAGraphics *graphics)
{
 if(Graphics == graphics)
  return true;

 Graphics=graphics;
 return true;
}

// ���������� �������� ������ ��������� �� �������
template<class T>
MGeometry<T>& MGraphics<T>::GetGeometry(size_t index)
{
 return Geometry[index];
}

// ���������� �������� ��������� ������� ��������� �� �������
template<class T>
MGeometryDescription& MGraphics<T>::GetDescription(size_t index)
{
 return Description[index];
}

// ���������� ����� �������� ���������
template<class T>
size_t MGraphics<T>::GetNumGeometry(void) const
{
 return Geometry.size();
}

// ��������� ����� ������ ���������
// ���������� ������ ������������ �������
template<class T>
size_t MGraphics<T>::AddGeometry(const MGeometry<T>& geometry)
{
 Geometry.push_back(geometry);
 Description.resize(Geometry.size());

 return Geometry.size()-1;
}

// ������� ������������ ������ ��������� �� �������
template<class T>
void MGraphics<T>::DelGeometry(size_t index)
{
 if(index >= Geometry.size())
  return;

 Geometry.erase(Geometry.begin()+index);
 Description.erase(Description.begin()+index);
}

// ������� ��� ������� ��������� �� �������
template<class T>
void MGraphics<T>::DelAllGeometry(void)
{
 Geometry.clear();
 Description.clear();
}
// --------------------------

// --------------------------
// ������ ���������
// --------------------------
// ������� ������ ���������
template<class T>
void MGraphics<T>::Clear(void)
{
 if(!Graphics)
  return;

}

// ������������ ��� �������
template<class T>
void MGraphics<T>::Repaint(void)
{
 if(!Graphics)
  return;

 for(size_t i=0;i<Geometry.size();i++)
 {
  if(Description[i].Visible)
  {
   Graphics->SetPenColor(Description[i].Color);
   Graphics->SetPenWidth(Description[i].PenWidth);
   // ������� ��� ��������� ���������

   MGeometry<T> &geometry=Geometry[i];
   MVertex<T>& vertex=geometry();
   // ������������ �����
   vertex=0;
   for(size_t i=0;i<vertex.GetNumVertex();i++,vertex++)
    Graphics->Pixel(vertex().x,vertex().y);

   // ������������ �������
   for(size_t j=0;j<geometry.GetNumBorders();j++)
   {
    if(geometry[j].GetNumVertex()>0)
     for(size_t k=0;k<geometry[j].GetNumVertex()-1;k++)
     {
      MVector<T> v1=vertex[geometry[j][k]];
      MVector<T> v2=vertex[geometry[j][k+1]];
      Graphics->Line(v1.x,v1.y,v2.x,v2.y);
     }
   }
  }
 }
}
// --------------------------


// --------------------------
// ���������
// --------------------------
// �������� ������������
// �� �������� ������ ���������
template<class T>
MGraphics<T>& MGraphics<T>::operator = (const MGraphics<T> &copy)
{
 // ������ �������� ��������� � �������� �������
 Geometry=copy.Geometry;

 // ������ �������� ���������������� �������
 Description=copy.Description;


 return *this;
}
// --------------------------

}
#endif
