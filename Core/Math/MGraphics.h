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
std::vector<MGeometry<T,Rows> > Geometry;

// Список описаний соответствующего объекта
vector<MGeometryDescription> Description;

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
MGeometry<T,Rows>& GetGeometry(size_t index);

// Возвращает описание заданного объекта отрисовки по индексу
MGeometryDescription& GetDescription(size_t index);

// Возвращает число объектов отрисовки
size_t GetNumGeometry(void) const;

// Добавляет новый объект отрисовки
// Возвращает индекс добавленного объекта
size_t AddGeometry(const MGeometry<T,Rows>& geometry);

// Удаляет существующий объект отрисовки по индексу
void DelGeometry(size_t index);

// Удаляет все объекты отрисовки по индексу
void DelAllGeometry(void);
// --------------------------

// --------------------------
// Методы рисования
// --------------------------
// Очищает модуль рисования
void Clear(void);

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
MGeometry<T,Rows>& MGraphics<T,Rows>::GetGeometry(size_t index)
{
 return Geometry[index];
}

// Возвращает описание заданного объекта отрисовки по индексу
template<class T, int Rows>
MGeometryDescription& MGraphics<T,Rows>::GetDescription(size_t index)
{
 return Description[index];
}

// Возвращает число объектов отрисовки
template<class T, int Rows>
size_t MGraphics<T,Rows>::GetNumGeometry(void) const
{
 return Geometry.size();
}

// Добавляет новый объект отрисовки
// Возвращает индекс добавленного объекта
template<class T, int Rows>
size_t MGraphics<T,Rows>::AddGeometry(const MGeometry<T,Rows>& geometry)
{
 Geometry.push_back(geometry);
 Description.resize(Geometry.size());

 return Geometry.size()-1;
}

// Удаляет существующий объект отрисовки по индексу
template<class T, int Rows>
void MGraphics<T,Rows>::DelGeometry(size_t index)
{
 if(index >= Geometry.size())
  return;

 Geometry.erase(Geometry.begin()+index);
 Description.erase(Description.begin()+index);
}

// Удаляет все объекты отрисовки по индексу
template<class T, int Rows>
void MGraphics<T,Rows>::DelAllGeometry(void)
{
 Geometry.clear();
 Description.clear();
}
// --------------------------

// --------------------------
// Методы рисования
// --------------------------
// Очищает модуль рисования
template<class T, int Rows>
void MGraphics<T,Rows>::Clear(void)
{
 if(!Graphics)
  return;

}

// Отрисовывает все объекты
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
   // Считаем всю геометрию двумерной

   MGeometry<T,Rows> &geometry=Geometry[i];
   MVertex<T,Rows>& vertex=geometry();
   // Отрисовываем точки
   vertex=0;
   for(size_t i=0;i<vertex.GetNumVertex();i++,vertex++)
    Graphics->Pixel(int(vertex().x),int(vertex().y));

   // Отрисовываем контуры
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
// Операторы
// --------------------------
// Оператор присваивания
// Не копирует модуль рисования
template<class T, int Rows>
MGraphics<T,Rows>& MGraphics<T,Rows>::operator = (const MGraphics<T,Rows> &copy)
{
 // Список объектов отрисовки в заданном порядке
 Geometry=copy.Geometry;

 // Список описаний соответствующего объекта
 Description=copy.Description;


 return *this;
}
// --------------------------

}
#endif
