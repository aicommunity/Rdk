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
template<class T, int Rows>
class MGraphics
{
protected: // ������
// ������ ���������
UAGraphics *Graphics;

// ������ �������� ��������� � �������� �������
std::vector<MGeometry<T,Rows> > Geometry;

// ������ �������� ���������������� �������
vector<MGeometryDescription> Description;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MGraphics(void);
MGraphics(const MGraphics<T,Rows> &copy);
~MGraphics(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ������ ���������
UAGraphics* GetGraphics(void) const;
bool SetGraphics(UAGraphics *graphics);

// ���������� �������� ������ ��������� �� �������
MGeometry<T,Rows>& GetGeometry(size_t index);

// ���������� �������� ��������� ������� ��������� �� �������
MGeometryDescription& GetDescription(size_t index);

// ���������� ����� �������� ���������
size_t GetNumGeometry(void) const;

// ��������� ����� ������ ���������
// ���������� ������ ������������ �������
size_t AddGeometry(const MGeometry<T,Rows>& geometry);

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
MGraphics<T,Rows>& operator = (const MGraphics<T,Rows> &copy);
// --------------------------
};


// --------------------------
// ������������ � �����������
// --------------------------
template<class T, int Rows>
MGraphics<T,Rows>::MGraphics(void)
{
 // ������ ���������
 Graphics=0;
}

template<class T, int Rows>
MGraphics<T,Rows>::MGraphics(const MGraphics<T,Rows> &copy)
{
 *this=copy;
}

template<class T, int Rows>
MGraphics<T,Rows>::~MGraphics(void)
{
 Graphics=0;
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ������ ���������
template<class T, int Rows>
UAGraphics* MGraphics<T,Rows>::GetGraphics(void) const
{
 return Graphics;
}

template<class T, int Rows>
bool MGraphics<T,Rows>::SetGraphics(UAGraphics *graphics)
{
 if(Graphics == graphics)
  return true;

 Graphics=graphics;
 return true;
}

// ���������� �������� ������ ��������� �� �������
template<class T, int Rows>
MGeometry<T,Rows>& MGraphics<T,Rows>::GetGeometry(size_t index)
{
 return Geometry[index];
}

// ���������� �������� ��������� ������� ��������� �� �������
template<class T, int Rows>
MGeometryDescription& MGraphics<T,Rows>::GetDescription(size_t index)
{
 return Description[index];
}

// ���������� ����� �������� ���������
template<class T, int Rows>
size_t MGraphics<T,Rows>::GetNumGeometry(void) const
{
 return Geometry.size();
}

// ��������� ����� ������ ���������
// ���������� ������ ������������ �������
template<class T, int Rows>
size_t MGraphics<T,Rows>::AddGeometry(const MGeometry<T,Rows>& geometry)
{
 Geometry.push_back(geometry);
 Description.resize(Geometry.size());

 return Geometry.size()-1;
}

// ������� ������������ ������ ��������� �� �������
template<class T, int Rows>
void MGraphics<T,Rows>::DelGeometry(size_t index)
{
 if(index >= Geometry.size())
  return;

 Geometry.erase(Geometry.begin()+index);
 Description.erase(Description.begin()+index);
}

// ������� ��� ������� ��������� �� �������
template<class T, int Rows>
void MGraphics<T,Rows>::DelAllGeometry(void)
{
 Geometry.clear();
 Description.clear();
}
// --------------------------

// --------------------------
// ������ ���������
// --------------------------
// ������� ������ ���������
template<class T, int Rows>
void MGraphics<T,Rows>::Clear(void)
{
 if(!Graphics)
  return;

}

// ������������ ��� �������
template<class T, int Rows>
void MGraphics<T,Rows>::Repaint(void)
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

   MGeometry<T,Rows> &geometry=Geometry[i];
   MVertex<T,Rows>& vertex=geometry();
   // ������������ �����
   vertex=0;
   for(size_t i=0;i<vertex.GetNumVertex();i++,vertex++)
    Graphics->Pixel(int(vertex().x),int(vertex().y));

   // ������������ �������
   for(size_t j=0;j<geometry.GetNumBorders();j++)
   {
    if(geometry[j].GetNumVertex()>0)
     for(size_t k=0;k<geometry[j].GetNumVertex()-1;k++)
     {
	  MVector<T,Rows> v1=vertex[geometry[j][k]];
      MVector<T,Rows> v2=vertex[geometry[j][k+1]];
      Graphics->Line(int(v1.x),int(v1.y),int(v2.x),int(v2.y));
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
template<class T, int Rows>
MGraphics<T,Rows>& MGraphics<T,Rows>::operator = (const MGraphics<T,Rows> &copy)
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
