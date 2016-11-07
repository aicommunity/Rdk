/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MUSERIALIZE_H
#define MUSERIALIZE_H

#include <iostream>
#include "../Serialize/Serialize.h"
#include "MVector.h"
#include "MDVector.h"
#include "MDMatrix.h"
//#include "MDyad.h"
//#include "MTensor.h"
//#include "MTheormec.h"
//#include "MBody.h"
#include "MGeometry.h"
//#include "MCSystem.h"

namespace RDK {

// MVector
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MVector<T,3> &data)
{
 operator <<(storage,data.Data[0][0]);
 operator <<(storage,data.Data[1][0]);
 operator <<(storage,data.Data[2][0]);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MVector<T,3> &data)
{
 operator >>(storage,data.Data[0][0]);
 operator >>(storage,data.Data[1][0]);
 operator >>(storage,data.Data[2][0]);
 return storage;
}

template<typename T, unsigned Rows>
USerStorageBinary& operator << (USerStorageBinary& storage, const MVector<T,Rows> &data)
{
 for(unsigned i=0;i<Rows;i++)
  operator <<(storage,data.Data[i][0]);
 return storage;
}

template<typename T, unsigned Rows>
USerStorageBinary& operator >> (USerStorageBinary& storage, MVector<T,Rows> &data)
{
 for(unsigned i=0;i<Rows;i++)
  operator >>(storage,data.Data[i][0]);
 return storage;
}

// MMatrix
template<typename T, unsigned Rows, unsigned Cols>
USerStorageBinary& operator << (USerStorageBinary& storage, const MMatrix<T,Rows,Cols> &data)
{
 for(unsigned i=0;i<Rows;i++)
 {
  for(unsigned j=0;j<Cols;j++)
   operator <<(storage,data.Data[i][j]);
 }
 return storage;
}

template<typename T, unsigned Rows, unsigned Cols>
USerStorageBinary& operator >> (USerStorageBinary& storage, MMatrix<T,Rows,Cols> &data)
{
 for(unsigned i=0;i<Rows;i++)
 {
  for(unsigned j=0;j<Cols;j++)
   operator >>(storage,data.Data[i][j]);
 }
 return storage;
}

// MDVector
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MDVector<T> &data)
{
 unsigned int size=data.GetSize();
 operator <<(storage,size);

 for(int i=0;i<data.GetRows();i++)
  operator <<(storage,data.Data[i]);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MDVector<T> &data)
{
 unsigned int size=0;
 operator >>(storage,size);

 data.Resize(size);

 for(int i=0;i<data.GetRows();i++)
  operator >>(storage,data.Data[i]);
 return storage;
}

// MDMatrix
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MDMatrix<T> &data)
{
 int rows=data.GetRows();
 operator <<(storage,rows);

 int cols=data.GetCols();
 operator <<(storage,cols);

 for(int i=0;i<data.GetRows();i++)
 {
  for(int j=0;j<data.GetCols();j++)
   operator <<(storage,data(i,j));
 }
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MDMatrix<T> &data)
{
 unsigned int rows=0;
 operator >>(storage,rows);
 unsigned int cols=0;
 operator >>(storage,cols);

 data.Resize(rows,cols);

 for(unsigned i=0;i<rows;i++)
 {
  for(unsigned j=0;j<cols;j++)
   operator >>(storage,data(i,j));
 }
 return storage;
}
		  /*
// MDyad
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MDyad<T> &data)
{
 operator <<(storage,data.v1);
 operator <<(storage,data.v2);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MDyad<T> &data)
{
 operator >>(storage,data.v1);
 operator >>(storage,data.v2);
 return storage;
}

// MTensor
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MTensor<T> &data)
{
 operator <<(storage,data.Dyads);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MTensor<T> &data)
{
 operator >>(storage,data.Dyads);
 return storage;
}

// RotationTensor
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MRotationTensor<T> &data)
{
 operator <<(storage,data.m);
 operator <<(storage,data.angle);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MRotationTensor<T> &data)
{
 operator >>(storage,data.m);
 operator >>(storage,data.angle);
 return storage;
}

// InertiaTensor
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MInertiaTensor<T> &data)
{
 operator <<(storage,data.d1);
 operator <<(storage,data.d2);
 operator <<(storage,data.d3);
 operator <<(storage,data.m1);
 operator <<(storage,data.m2);
 operator <<(storage,data.m3);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MInertiaTensor<T> &data)
{
 operator >>(storage,data.d1);
 operator >>(storage,data.d2);
 operator >>(storage,data.d3);
 operator >>(storage,data.m1);
 operator >>(storage,data.m2);
 operator >>(storage,data.m3);
 return storage;
}

// MBody
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MBody<T> &data)
{
 operator <<(storage,data.Location);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MBody<T> &data)
{
 operator >>(storage,data.Location);
 return storage;
}

// MKinematicBody
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MKinematicBody<T> &data)
{
 operator <<(storage,static_cast<const MBody<T>&>(data));
 operator <<(storage,data.Rotation);
 operator <<(storage,data.TranslationSpeed);
 operator <<(storage,data.AngleSpeed);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MKinematicBody<T> &data)
{
 operator >>(storage,static_cast<MBody<T>&>(data));
 operator >>(storage,data.Rotation);
 operator >>(storage,data.TranslationSpeed);
 operator >>(storage,data.AngleSpeed);
 return storage;
}

// MMechanicalBody
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MMechanicalBody<T> &data)
{
 operator <<(storage,static_cast<const MKinematicBody<T>&>(data));
 operator <<(storage,data.MassCenter);
 operator <<(storage,data.Mass);
 operator <<(storage,data.CIT);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MMechanicalBody<T> &data)
{
 operator >>(storage,static_cast<MKinematicBody<T>&>(data));
 operator >>(storage,data.MassCenter);
 operator >>(storage,data.Mass);
 operator >>(storage,data.CIT);
 return storage;
}        */

// MRay
template<typename T, int Rows>
USerStorageBinary& operator << (USerStorageBinary& storage, const MRay<T, Rows> &data)
{
 operator << (storage,data.Origin);
 operator << (storage,data.Direction);
 return storage;
}

template<typename T, int Rows>
USerStorageBinary& operator >> (USerStorageBinary& storage, MRay<T, Rows> &data)
{
 operator >> (storage,data.Origin);
 operator >> (storage,data.Direction);
 return storage;
}

// MPlane
template<typename T, int Rows>
USerStorageBinary& operator << (USerStorageBinary& storage, const MPlane<T, Rows> &data)
{
 operator << (storage,data.Normal);
 operator << (storage,data.Distance);
 return storage;
}

template<typename T, int Rows>
USerStorageBinary& operator >> (USerStorageBinary& storage, MPlane<T, Rows> &data)
{
 operator >> (storage,data.Normal);
 operator >> (storage,data.Distance);
 return storage;
}
					/*
// MBorder
USerStorageBinary& operator << (USerStorageBinary& storage, const MBorder &data);

USerStorageBinary& operator >> (USerStorageBinary& storage, MBorder &data);

// MVertex
template<typename T, int Rows>
USerStorageBinary& operator << (USerStorageBinary& storage, const MVertex<T, Rows> &data)
{
 operator << (storage,data.GetVertex());
 operator << (storage,data.GetNames());
 return storage;
}

template<typename T, int Rows>
USerStorageBinary& operator >> (USerStorageBinary& storage, MVertex<T, Rows> &data)
{
 std::vector<MVector<T,3> > temp;
 std::vector<std::string> temp2;
 operator >> (storage,temp);
 operator >> (storage,temp2);
 data=temp;
 data=temp2;
 return storage;
}
               */
// MGeometry
template<typename T, int Rows>
USerStorageBinary& operator << (USerStorageBinary& storage, const MGeometry<T, Rows> &data)
{
 operator << (storage,data.GetVertices());
 operator << (storage,data.GetVerticesNames());
 operator << (storage,data.GetBorders());
 return storage;
}

template<typename T, int Rows>
USerStorageBinary& operator >> (USerStorageBinary& storage, MGeometry<T, Rows> &data)
{
 std::vector<MVector<T,Rows> > vtemp;
 std::vector<std::string> vntemp;
 std::vector<MBorder> btemp;

 operator >> (storage,vtemp);
 operator >> (storage,btemp);
 data.SetVertices(vtemp);
 data.SetVerticesNames(vntemp);
 data.SetBorders(btemp);

 return storage;
}
	  /*
// MCSystem
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MCSystem<T> &data)
{
 operator <<(storage,data.Location);
 operator <<(storage,data.Basis[0]);
 operator <<(storage,data.Basis[1]);
 operator <<(storage,data.Basis[2]);
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MCSystem<T> &data)
{
 operator >>(storage,data.Location);
 operator >>(storage,data.Basis[0]);
 operator >>(storage,data.Basis[1]);
 operator >>(storage,data.Basis[2]);
 return storage;
}                  */
/*
// MCartesianCSystem
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MCartesianCSystem<T> &data)
{
 operator <<(storage,static_cast<const MCSystem<T> &>(data));
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MCartesianCSystem<T> &data)
{
 operator >>(storage,static_cast<const MCSystem<T> &>(data));
 return storage;
}

// MEulerCSystem
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MEulerCSystem<T> &data)
{
 operator <<(storage,static_cast<const MCSystem<T> &>(data));
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MEulerCSystem<T> &data)
{
 operator >>(storage,static_cast<const MCSystem<T> &>(data));
 return storage;
}

// MPolarCSystem
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const MPolarCSystem<T> &data)
{
 operator <<(storage,static_cast<const MCSystem<T> &>(data));
 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, MPolarCSystem<T> &data)
{
 operator >>(storage,static_cast<const MCSystem<T> &>(data));
 return storage;
}                       */

}
#endif

