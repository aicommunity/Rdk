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

}
}
#endif


