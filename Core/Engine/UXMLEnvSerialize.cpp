/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UXML_ENV_SERIALIZE_CPP
#define UXML_ENV_SERIALIZE_CPP

#include "UXMLEnvSerialize.h"
#include "../Serialize/UXMLStdSerialize.h"

namespace RDK {

// UIdVector
/*USerStorageXML& operator << (USerStorageXML& storage, const UIdVector &data)
{
 storage.SetNodeAttribute("Type","UIdVector");
 unsigned int size=data.GetSize();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size == 0)
  return storage;

 std::stringstream stream;
 for(unsigned int i=0;i<size;i++)
 {
  stream<<data[i];
  if(i<size-1)
   stream<<" ";
 }

// std::string str;
// str=stream.str();

 storage.SetNodeText(stream.str());

 return storage;

}

USerStorageXML& operator >> (USerStorageXML& storage, UIdVector &data)
{
// if(storage.GetNodeAttribute("Type") != "UIdVector")
//  return storage;

 int size=0;
 std::string str=storage.GetNodeAttribute("Size"); // TODO: заменить
 if(!str.empty())
  size=atoi(str);

 if(size <= 0)
 {
  data.Resize(0);
  return storage;
 }
 data.Resize(size);

 std::stringstream stream(storage.GetNodeText().c_str());

 for(int i=0;i<size;i++)
 {
  stream>>data[i];
 }

 return storage;

}

// ULongIdVector
USerStorageXML& operator << (USerStorageXML& storage, const ULongIdVector &data)
{
 storage.SetNodeAttribute("Type","ULongIdVector");
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

USerStorageXML& operator >> (USerStorageXML& storage, ULongIdVector &data)
{
// if(storage.GetNodeAttribute("Type") != "ULongIdVector")
//  return storage;

 int size=0;
// std::string str=storage.GetNodeAttribute("Size");
// if(!str.empty())
//  size=atoi(str);

 size=storage.GetNumNodes();

 if(size <= 0)
 {
  data.Resize(0);
  return storage;
 }
 data.Resize(size);

 ULongId* pdata=&data[0];

 for(int i=0;i<size;i++)
 {
  if(!storage.SelectNode("elem",i))
   return storage;
  operator >>(storage,*(pdata+i));
  storage.SelectUp();
 }

 return storage;
}    */
// ULinkSide
USerStorageXML& operator << (USerStorageXML& storage, const ULinkSide &data)
{
 storage<<data.ComponentName;
 storage.SetNodeAttribute("Type","ULinkSide");
 storage.SetNodeAttribute("Index",sntoa(data.Index));
 storage.SetNodeAttribute("PropertyName",data.PropertyName);

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, ULinkSide &data)
{
 if(storage.GetNodeAttribute("Type") != "ULinkSide")
  return storage;
 operator >>(storage,data.ComponentName);
 data.Index=RDK::atoi(storage.GetNodeAttribute("Index"));
 data.PropertyName=storage.GetNodeAttribute("PropertyName");

 return storage;
}

// ULink
USerStorageXML& operator << (USerStorageXML& storage, const ULink &data)
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

USerStorageXML& operator >> (USerStorageXML& storage, ULink &data)
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
USerStorageXML& operator << (USerStorageXML& storage, const ULinksList &data)
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

USerStorageXML& operator >> (USerStorageXML& storage, ULinksList &data)
{
 if(storage.GetNodeAttribute("Type") != "ULinksList")
  return storage;

 int size=0;
// size=RDK::atoi(storage.GetNodeAttribute("Size"));
 size=storage.GetNumNodes();

 if(size <= 0)
 {
  data.Resize(0);
  return storage;
 }
 data.Resize(size);

 ULink* pdata=&data[0];

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

