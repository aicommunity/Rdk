/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MUXML_SERIALIZE_H
#define MUXML_SERIALIZE_H

#include <vector>
#include <map>
#include <strstream>
#include <sstream>
#include "../Serialize/USerStorageXML.h"
#include "../Utilities/USupport.h"
#include "../Math/UMath.h"

namespace RDK {
namespace Serialize {

// MVector
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const MVector<T> &data)
{
 storage.SetNodeAttribute("Type","MVector");
 storage.AddNode("x");
 storage<<data.x;
 storage.SelectUp();

 storage.AddNode("y");
 storage<<data.y;
 storage.SelectUp();

 storage.AddNode("z");
 storage<<data.z;
 storage.SelectUp();

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, MVector<T> &data)
{
 if(storage.GetNodeAttribute("Type") != "MVector")
  return storage;

 if(!storage.SelectNode("x"))
  return storage;

 storage>>data.x;
 storage.SelectUp();

 if(!storage.SelectNode("y"))
  return storage;

 storage>>data.y;
 storage.SelectUp();

 if(!storage.SelectNode("z"))
  return storage;

 storage>>data.z;
 storage.SelectUp();

 return storage;
}

#ifdef MDVECTOR_H
USerStorageXML& operator << (USerStorageXML& storage, const MDVector &data)
{
  storage.SetNodeAttribute("Type","MDVector");
 storage.AddNode("x");
 storage<<data.x;
 storage.SelectUp();

 storage.AddNode("y");
 storage<<data.y;
 storage.SelectUp();

 storage.AddNode("z");
 storage<<data.z;
 storage.SelectUp();

 return storage;
}
USerStorageXML& operator >> (USerStorageXML& storage, MDVector &data)
{
 if(storage.GetNodeAttribute("Type") != "MDVector")
  return storage;

 if(!storage.SelectNode("x"))
  return storage;

 storage>>data.x;
 storage.SelectUp();

 if(!storage.SelectNode("y"))
  return storage;

 storage>>data.y;
 storage.SelectUp();

 if(!storage.SelectNode("z"))
  return storage;

 storage>>data.z;
 storage.SelectUp();

 return storage;
}
#endif

// MRotationTensor
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const MRotationTensor<T> &data)
{
 storage.SetNodeAttribute("Type","MRotationTensor");
 storage.AddNode("m");
 storage<<data.m;
 storage.SelectUp();

 storage.AddNode("angle");
 storage<<data.angle;
 storage.SelectUp();

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, MRotationTensor<T> &data)
{
 if(storage.GetNodeAttribute("Type") != "MRotationTensor")
  return storage;

 if(!storage.SelectNode("m"))
  return storage;

 storage>>data.m;
 storage.SelectUp();

 if(!storage.SelectNode("angle"))
  return storage;

 storage>>data.angle;
 storage.SelectUp();

 return storage;

}


// MKinematicBody
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const MKinematicBody<T> &data)
{
 storage.SetNodeAttribute("Type","MKinematicBody");
 storage.AddNode("Rotation");
 storage<<data.Rotation;
 storage.SelectUp();

 storage.AddNode("TranslationSpeed");
 storage<<data.TranslationSpeed;
 storage.SelectUp();

 storage.AddNode("AngleSpeed");
 storage<<data.AngleSpeed;
 storage.SelectUp();

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, MKinematicBody<T> &data)
{
 if(storage.GetNodeAttribute("Type") != "MKinematicBody")
  return storage;

 if(!storage.SelectNode("Rotation"))
  return storage;

 storage>>data.Rotation;
 storage.SelectUp();

 if(!storage.SelectNode("TranslationSpeed"))
  return storage;

 storage>>data.TranslationSpeed;
 storage.SelectUp();

 if(!storage.SelectNode("AngleSpeed"))
  return storage;

 storage>>data.AngleSpeed;
 storage.SelectUp();

 return storage;

}

// MCSystem
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const MCSystem<T> &data)
{
 storage.SetNodeAttribute("Type","MCSystem");
 storage.AddNode("Basis0");
 storage<<data.Basis[0];
 storage.SelectUp();

 storage.AddNode("Basis1");
 storage<<data.Basis[1];
 storage.SelectUp();

 storage.AddNode("Basis2");
 storage<<data.Basis[2];
 storage.SelectUp();

 storage.AddNode("Location");
 storage<<data.Location;
 storage.SelectUp();

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, MCSystem<T> &data)
{
 if(storage.GetNodeAttribute("Type") != "MCSystem")
  return storage;

 if(!storage.SelectNode("Basis0"))
  return storage;

 storage>>data.Basis[0];
 storage.SelectUp();

 if(!storage.SelectNode("Basis1"))
  return storage;

 storage>>data.Basis[1];
 storage.SelectUp();

 if(!storage.SelectNode("Basis2"))
  return storage;

 storage>>data.Basis[2];
 storage.SelectUp();

 if(!storage.SelectNode("Location"))
  return storage;

 storage>>data.Location;
 storage.SelectUp();

 return storage;
}


// MCartesianCSystem
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const MCartesianCSystem<T> &data)
{
 storage<<static_cast<const MCSystem<T>& >(data);

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, MCartesianCSystem<T> &data)
{
 storage>>static_cast<MCSystem<T>& >(data);

 return storage;
}

// MBorder
USerStorageXML& operator << (USerStorageXML& storage, const MBorder &data)
{
 storage.SetNodeAttribute("Type","MBorder");
 storage.AddNode("VertexIndex");
 storage<<data.GetVertexIndex();
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, MBorder &data)
{
 //��������� ����������
 std::vector<int> varVertexIndex;

 if(storage.GetNodeAttribute("Type") != "MBorder")
  return storage;

 if(!storage.SelectNode("VertexIndex"))
  return storage;

 storage>>varVertexIndex;
 data.SetVertexIndex(varVertexIndex);
 storage.SelectUp();

 return storage;

}

// MVertex
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const MVertex<T> &data)
{
 storage.SetNodeAttribute("Type","MVertex");
 storage.AddNode("Vertex");
 storage<<data.GetVertex();
 storage.SelectUp();

 storage.AddNode("Names");
 storage<<data.GetNames();
 storage.SelectUp();

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, MVertex<T> &data)
{
 //��������� ����������
 std::vector<MVector<T> > varVertexVector;
 std::vector<std::string> varNames;

 if(storage.GetNodeAttribute("Type") != "MVertex")
  return storage;

 if(!storage.SelectNode("Vertex"))
  return storage;

 storage>>varVertexVector;
 data.SetVertexVector(varVertexVector);
 storage.SelectUp();

 if(!storage.SelectNode("Names"))
  return storage;

 storage>>varNames;
 data.SetNames(varNames);
 storage.SelectUp();

 return storage;

}

// MGeometry
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const MGeometry<T> &data)
{
 storage.SetNodeAttribute("Type","MGeometry");
 storage.AddNode("Vertex");
 storage<<data.GetVertex();
 storage.SelectUp();

 storage.AddNode("Borders");
 storage<<data.GetBorders();
 storage.SelectUp();

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, MGeometry<T> &data)
{
 //��������� ����������
 MVertex<T> varVertexVector;
 std::vector<MBorder> varBorders;

 if(storage.GetNodeAttribute("Type") != "MGeometry")
  return storage;

 if(!storage.SelectNode("Vertex"))
  return storage;

 storage>>varVertexVector;
 data.SetVertex(varVertexVector);
 storage.SelectUp();

 if(!storage.SelectNode("Borders"))
  return storage;

 storage>>varBorders;
 data.SetBorders(varBorders);
 storage.SelectUp();

 return storage;

}

}
}
#endif

