/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UXML_ENV_SERIALIZE_H
#define UXML_ENV_SERIALIZE_H

#include <vector>
#include <map>
//#include <strstream>
#include <sstream>
#include "../Serialize/USerStorageXML.h"
#include "UEnvSupport.h"

namespace RDK {

// UIdVector
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UIdVector &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UIdVector &data);

// ULongIdVector
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const ULongIdVector &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, ULongIdVector &data);

// ULinkSide
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const ULinkSideT<T> &data)
{
 storage<<data.Id;
 storage.SetNodeAttribute("Type","ULinkSide");
 storage.SetNodeAttribute("Index",sntoa(data.Index));
 storage.SetNodeAttribute("Name",data.Name);

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, ULinkSideT<T> &data)
{
 if(storage.GetNodeAttribute("Type") != "ULinkSide")
  return storage;
 operator >>(storage,data.Id);
 data.Index=RDK::atoi(storage.GetNodeAttribute("Index"));
 data.Name=storage.GetNodeAttribute("Name");

 return storage;
}

// ULink
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const ULinkT<T> &data)
{
 storage.SetNodeAttribute("Type","ULink");

 storage.AddNode("Item");
 storage<<data.Item;
 storage.SelectUp();

 for(size_t i=0;i<data.Connector.size();i++)
 {
  storage.AddNode("Connector");
  storage<<data.Connector[i];
  storage.SelectUp();
 }

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, ULinkT<T> &data)
{
 if(storage.GetNodeAttribute("Type") != "ULink")
  return storage;

 if(!storage.SelectNode("Item"))
  return storage;
 operator >>(storage,data.Item);
 storage.SelectUp();

 int num_connectors=storage.GetNumNodes("Connector");

 data.Connector.resize(num_connectors);
 for(int i=0;i<num_connectors;i++)
 {
  if(!storage.SelectNode("Connector",i))
   return storage;
  operator >>(storage,data.Connector[i]);
  storage.SelectUp();
 }

 return storage;
}


// ULinksList
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const ULinksListT<T> &data)
{
 storage.SetNodeAttribute("Type","ULinksList");
 unsigned int size=data.GetSize();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size == 0)
  return storage;

 for(unsigned int i=0;i<size;i++)
 {
  storage.AddNode("elem");
  storage<<data[i];
  storage.SelectUp();
 }

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, ULinksListT<T> &data)
{
 if(storage.GetNodeAttribute("Type") != "ULinksList")
  return storage;

 int size=0;
 size=RDK::atoi(storage.GetNodeAttribute("Size"));

 if(size <= 0)
 {
  data.Resize(0);
  return storage;
 }
 data.Resize(size);

 ULinkT<T>* pdata=&data[0];

 for(int i=0;i<size;i++)
 {
  if(!storage.SelectNode("elem",i))
   return storage;
  operator >>(storage,*(pdata+i));
  storage.SelectUp();
 }

 return storage;
}

}
#endif

