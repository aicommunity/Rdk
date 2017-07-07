/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef USERIALIZE_H
#define USERIALIZE_H

#include "Serialize_lite.h"
#include "UBinaryStdSerialize.h"
#include "UXMLStdSerialize.h"
#include "USerStorage.h"
#include "USerStorageBinary.h"
#include "USerStorageXML.h"
#include "UIOStream.h"

/*
namespace RDK {

template<typename T>
std::string VarToStr(const T &value)
{
 USerStorage xml;
 std::string str;
 xml.Create("Value");
 xml<<value;
 xml.Save(str);
 return str;
}

template<typename T>
std::string VarToStrRaw(const T &value)
{
 USerStorage xml;
 xml.Create("Value");
 xml<<value;
 return xml.GetNodeText();
}

template<typename T>
bool VarFromStr(const std::string &str, T &value)
{
 USerStorage xml;
 if(!xml.Load(str))
  return false;
 xml>>value;
 return true;
}

template<typename T>
bool VarFromStrRaw(const std::string &str, T &value)
{
 USerStorage xml;
 xml.Create("Value");
 xml.SetNodeText(str);
 xml>>value;
 return true;
}

}
*/
#endif



