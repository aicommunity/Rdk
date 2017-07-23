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
//#include <strstream>
#include <sstream>
#include "../Serialize/USerStorageXML.h"
#include "../Utilities/USupport.h"
#include "../Math/UMath.h"
#include "../Serialize/UXMLStdSerialize.h"

namespace RDK {

template<typename T, unsigned Rows>
USerStorageXML& operator << (USerStorageXML& storage, const MVector<T,Rows> &data)
{
 std::stringstream stream;

 for(unsigned i=0;i<Rows;i++)
 {
  stream<<data.Data[i][0];
  if(i<Rows-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<typename T, unsigned Rows>
USerStorageXML& operator >> (USerStorageXML& storage, MVector<T,Rows> &data)
{
// std::string rvalue=storage.GetNodeText();
 std::stringstream stream(storage.GetNodeText().c_str());

 for(unsigned i=0;i<Rows;i++)
  stream>>data.Data[i][0];

 return storage;
}

// MMatrix
template<typename T, unsigned Rows, unsigned Cols>
USerStorageXML& operator << (USerStorageXML& storage, const MMatrix<T,Rows,Cols> &data)
{
 std::stringstream stream;

 stream<<endl;
 for(unsigned i=0;i<Rows;i++)
 {
  for(unsigned j=0;j<Cols;j++)
  {
   stream<<data.Data[i][j]<<"\t";
  }
  if(i<Rows-1)
   stream<<endl;
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<typename T, unsigned Rows, unsigned Cols>
USerStorageXML& operator >> (USerStorageXML& storage, MMatrix<T,Rows,Cols> &data)
{
// std::string rvalue=storage.GetNodeText();
 std::stringstream stream(storage.GetNodeText().c_str());

 for(unsigned i=0;i<Rows;i++)
 {
  for(unsigned j=0;j<Cols;j++)
  {
   stream>>data.Data[i][j];
  }
 }

 return storage;
}


// MDVector
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const MDVector<T> &data)
{
 std::stringstream stream;

 int size=data.GetSize();
 storage.SetNodeAttribute("Size",sntoa(size));
 for(int i=0;i<data.GetRows();i++)
 {
  stream<<data.Data[i];
  if(i<data.GetRows()-1)
   stream<<" ";
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, MDVector<T> &data)
{
 if(storage.GetNodeAttribute("Type") == "std::vector")
 {
  unsigned int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size"));

  if(size == 0)
  {
   data.Resize(0);
   return storage;
  }

  data.Resize(size);
  for(size_t i=0;i<size;i++)
  {
   if(!storage.SelectNode("elem",i))
	return storage;
   operator >>(storage,data.Data[i]);
   storage.SelectUp();
  }
 }
 else
 {
  int size=0;
  size=RDK::atoi(storage.GetNodeAttribute("Size"));

  data.Resize(size);

  if(size>0)
  {
//   std::string rvalue=storage.GetNodeText();
   std::stringstream stream(storage.GetNodeText().c_str());

   for(int i=0;i<data.GetRows();i++)
	stream>>data.Data[i];
  }
 }
 return storage;
}

// MDMatrix
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const MDMatrix<T> &data)
{
 int rows=data.GetRows();
 int cols=data.GetCols();
 storage.SetNodeAttribute("Rows",sntoa(rows));
 storage.SetNodeAttribute("Cols",sntoa(cols));

 std::stringstream stream;

 stream<<endl;
 for(int i=0;i<rows;i++)
 {
  for(int j=0;j<cols;j++)
  {
   stream<<data(i,j)<<"\t";
  }
  if(i<rows-1)
   stream<<endl;
 }

 storage.SetNodeText(stream.str());

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, MDMatrix<T> &data)
{
 int rows=0;
 rows=RDK::atoi(storage.GetNodeAttribute("Rows"));
 int cols=0;
 cols=RDK::atoi(storage.GetNodeAttribute("Cols"));

 data.Resize(rows,cols);

// std::string rvalue=storage.GetNodeText();
 std::stringstream stream(storage.GetNodeText().c_str());

 for(int i=0;i<rows;i++)
 {
  for(int j=0;j<cols;j++)
  {
   stream>>data.Data[i*cols+j];
  }
 }

 return storage;
}
			/*

// MBorder
USerStorageXML& operator << (USerStorageXML& storage, const MBorder &data);
USerStorageXML& operator >> (USerStorageXML& storage, MBorder &data);


// MVertex
template<typename T, int Rows>
USerStorageXML& operator << (USerStorageXML& storage, const MVertex<T, Rows> &data)
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

template<typename T, int Rows>
USerStorageXML& operator >> (USerStorageXML& storage, MVertex<T, Rows> &data)
{
 //Временные переменные
 std::vector<MVector<T,Rows> > varVertexVector;
 std::vector<std::string> varNames;

// if(storage.GetNodeAttribute("Type") != "MVertex")
//  return storage;

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
            */
// MGeometry
template<typename T, int Rows>
USerStorageXML& operator << (USerStorageXML& storage, const MGeometry<T, Rows> &data)
{
 storage.SetNodeAttribute("Type","MGeometry");
 storage.AddNode("Vertices");
 storage<<data.GetVertices();
 storage.SelectUp();

 storage.AddNode("VerticesNames");
 storage<<data.GetVerticesNames();
 storage.SelectUp();

 storage.AddNode("Borders");
 storage<<data.GetBorders();
 storage.SelectUp();

 return storage;
}

template<typename T, int Rows>
USerStorageXML& operator >> (USerStorageXML& storage, MGeometry<T, Rows> &data)
{
 //Временные переменные
 std::vector<MVector<T,Rows> > varVertices;
 std::vector<std::string> varVerticesNames;
 std::vector<MBorder> varBorders;

// if(storage.GetNodeAttribute("Type") != "MGeometry")
//  return storage;

 if(!storage.SelectNode("Vertices"))
  return storage;

 storage>>varVertices;
 data.SetVertices(varVertices);
 storage.SelectUp();

 if(!storage.SelectNode("VerticesNames"))
  return storage;

 storage>>varVertices;
 data.SetVerticesNames(varVerticesNames);
 storage.SelectUp();

 if(!storage.SelectNode("Borders"))
  return storage;

 storage>>varBorders;
 data.SetBorders(varBorders);
 storage.SelectUp();

 return storage;

}

/*
/// Предупреждение: эта функция предполагает, что в строке содержится матрица
/// той же размерности, что и принимающая переменная
template<typename T>
bool VarFromStrRaw(const std::string &str, MDMatrix<T> &value)
{
 USerStorage xml;
 xml.Create("Value");
 xml.SetNodeAttribute("Rows",sntoa(value.GetRows()));
 xml.SetNodeAttribute("Cols",sntoa(value.GetCols()));
 xml.SetNodeText(str);
 xml>>value;
 return true;
}
*/

}
#endif

