/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MUXML_SERIALIZE_CPP
#define MUXML_SERIALIZE_CPP

#include "MUXMLSerialize.h"

namespace RDK {
				  /*
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
 //Временные переменные
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
                 */

}
#endif

