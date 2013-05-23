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

// Класс описания особенностей отображения заданного объекта
class MGeometryDescription
{
public: // Данные
// Цвет
UColorT Color;

// Толщина пера
int PenWidth;

// Имя
string Name;

// Описание
string Description;

// Флаг видимости
bool Visible;

// Флаг отрисовки прицела на каждую отображемую точку
bool TargetPoints;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MGeometryDescription(void)
{
 // Цвет
 Color=UColorT(0,0,255,0);

 // Толщина пера
 PenWidth=4;

 // Флаг видимости
 Visible=true;

 TargetPoints=false;
};

MGeometryDescription(const MGeometryDescription &copy)
{
 // Цвет
 Color=copy.Color;

 // Толщина пера
 PenWidth=copy.PenWidth;

 // Имя
 Name=copy.Name;

 // Описание
 Description=copy.Description;

 // Флаг видимости
 Visible=copy.Visible;

 TargetPoints=copy.TargetPoints;
};

virtual ~MGeometryDescription(void)
{
};
// --------------------------

};

// Класс поддержки отрисовки графики
template<class T, int Rows>
class MGraphics
{
protected: // Данные
// Модуль рисования
UAGraphics *Graphics;

// Список объектов отрисовки в заданном порядке
std::vector<MGeometry<T,Rows> > Geometries;

// Список описаний соответствующего объекта
vector<MGeometryDescription> Descriptions;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MGraphics(void);
MGraphics(const MGraphics<T,Rows> &copy);
~MGraphics(void);
// --------------------------

// --------------------------
// Методы управления данныим
// --------------------------
// Модуль рисования
UAGraphics* GetGraphics(void) const;
bool SetGraphics(UAGraphics *graphics);

// Возвращает заданный объект отрисовки по индексу
const MGeometry<T,Rows>& Geometry(size_t index) const;
MGeometry<T,Rows>& Geometry(size_t index);

// Возвращает описание заданного объекта отрисовки по индексу
const MGeometryDescription& Description(size_t index) const;
MGeometryDescription& Description(size_t index);

// Возвращает число объектов отрисовки
size_t GetNumGeometries(void) const;
bool SetNumGeometries(size_t value);

// Удаляет выбранный объект отрисовки
bool DelGeometry(size_t value);

// Удаляет все объекты отрисовки по индексу
void Clear(void);
// --------------------------

// --------------------------
// Методы рисования
// --------------------------
// Отрисовывает все объекты
void Repaint(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
// Не копирует модуль рисования
MGraphics<T,Rows>& operator = (const MGraphics<T,Rows> &copy);
// --------------------------
};


// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<class T, int Rows>
MGraphics<T,Rows>::MGraphics(void)
{
 // Модуль рисования
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
// Методы управления данныим
// --------------------------
// Модуль рисования
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

// Возвращает заданный объект отрисовки по индексу
template<class T, int Rows>
const MGeometry<T,Rows>& MGraphics<T,Rows>::Geometry(size_t index) const
{
 return Geometries[index];
}

template<class T, int Rows>
MGeometry<T,Rows>& MGraphics<T,Rows>::Geometry(size_t index)
{
 return Geometries[index];
}

// Возвращает описание заданного объекта отрисовки по индексу
template<class T, int Rows>
const MGeometryDescription& MGraphics<T,Rows>::Description(size_t index) const
{
 return Descriptions[index];
}

template<class T, int Rows>
MGeometryDescription& MGraphics<T,Rows>::Description(size_t index)
{
 return Descriptions[index];
}

// Возвращает число объектов отрисовки
template<class T, int Rows>
size_t MGraphics<T,Rows>::GetNumGeometries(void) const
{
 return Geometries.size();
}

template<class T, int Rows>
bool MGraphics<T,Rows>::SetNumGeometries(size_t value)
{
 Geometries.resize(value);
 Descriptions.resize(Geometries.size());
 return true;
}

// Удаляет выбранный объект отрисовки
template<class T, int Rows>
bool MGraphics<T,Rows>::DelGeometry(size_t value)
{
 Geometries.erase(Geometries.begin()+value);
 Descriptions.erase(Descriptions.begin()+value);
 return true;
}

// Удаляет все объекты отрисовки по индексу
template<class T, int Rows>
void MGraphics<T,Rows>::Clear(void)
{
 Geometries.clear();
 Descriptions.clear();
}
// --------------------------

// --------------------------
// Методы рисования
// --------------------------
// Отрисовывает все объекты
template<class T, int Rows>
void MGraphics<T,Rows>::Repaint(void)
{
 if(!Graphics)
  return;

 for(size_t i=0;i<Geometries.size();i++)
 {
  if(Descriptions[i].Visible)
  {
   Graphics->SetPenColor(Descriptions[i].Color);
   Graphics->SetPenWidth(Descriptions[i].PenWidth);
   // Считаем всю геометрию двумерной

   MGeometry<T,Rows> &geometry=Geometries[i];
   const std::vector<MVector<T,Rows> >& vertices=geometry.GetVertices();
   // Отрисовываем точки
   for(size_t j=0;j<vertices.size();j++)
   {
	const MVector<T,Rows>& vertex=vertices[j];
	Graphics->SetPenWidth(1);
	Graphics->Pixel(int(vertex.x),int(vertex.y));
	if(Descriptions[i].TargetPoints)
	{
     Graphics->SetPenWidth(Descriptions[i].PenWidth);
	 Graphics->Line(int(vertex.x),int(vertex.y)-Descriptions[i].PenWidth*7,int(vertex.x),int(vertex.y)-Descriptions[i].PenWidth*2);
	 Graphics->Line(int(vertex.x),int(vertex.y)+Descriptions[i].PenWidth*2,int(vertex.x),int(vertex.y)+Descriptions[i].PenWidth*7);
	 Graphics->Line(int(vertex.x)-Descriptions[i].PenWidth*7,int(vertex.y),int(vertex.x)-Descriptions[i].PenWidth*2,int(vertex.y));
	 Graphics->Line(int(vertex.x)+Descriptions[i].PenWidth*2,int(vertex.y),int(vertex.x)+Descriptions[i].PenWidth*7,int(vertex.y));
 	 Graphics->SetPenWidth(1);
	}
   }

   // Отрисовываем контуры
   Graphics->SetPenWidth(Descriptions[i].PenWidth);
   for(size_t j=0;j<geometry.GetNumBorders();j++)
   {
	if(geometry.Border(j).size()>0)
	 for(size_t k=0;k<geometry.Border(j).size()-1;k++)
     {
	  MVector<T,Rows> v1=vertices[geometry.Border(j)[k]];
	  MVector<T,Rows> v2=vertices[geometry.Border(j)[k+1]];
      Graphics->Line(int(v1.x),int(v1.y),int(v2.x),int(v2.y));
     }
   }
  }
 }
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
// Не копирует модуль рисования
template<class T, int Rows>
MGraphics<T,Rows>& MGraphics<T,Rows>::operator = (const MGraphics<T,Rows> &copy)
{
 // Список объектов отрисовки в заданном порядке
 Geometries=copy.Geometries;

 // Список описаний соответствующего объекта
 Description=copy.Description;

 return *this;
}
// --------------------------

}
#endif
