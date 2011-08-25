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
namespace Serialize {

// UIdVector
USerStorageXML& operator << (USerStorageXML& storage, const UIdVector &data)
{
 storage.SetNodeAttribute("Type","UIdVector");
 unsigned int size=data.GetSize();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size <= 0)
  return storage;

 for(unsigned int i=0;i<size;i++)
 {
  storage.AddNode("elem");
  storage<<data[i];
  storage.SelectUp();
 }

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UIdVector &data)
{
 if(storage.GetNodeAttribute("Type") != "UIdVector")
  return storage;

 int size=0;
 size=atoi(storage.GetNodeAttribute("Size"));

 if(size <= 0)
 {
  data.Resize(0);
  return storage;
 }
 data.Resize(size);

 UId* pdata=&data[0];

 for(int i=0;i<size;i++)
 {
  if(!storage.SelectNode("elem",i))
   return storage;
  operator >>(storage,*(pdata+i));
  storage.SelectUp();
 }

 return storage;
}

// ULongIdVector
USerStorageXML& operator << (USerStorageXML& storage, const ULongIdVector &data)
{
 storage.SetNodeAttribute("Type","ULongIdVector");
 unsigned int size=data.GetSize();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size <= 0)
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
 if(storage.GetNodeAttribute("Type") != "ULongIdVector")
  return storage;

 int size=0;
 size=atoi(storage.GetNodeAttribute("Size"));

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
}

// ULinkSide
USerStorageXML& operator << (USerStorageXML& storage, const ULinkSide &data)
{
 storage.SetNodeAttribute("Type","ULinkSide");

 storage.AddNode("Id");
 storage<<data.Id;
 storage.SelectUp();

 storage.AddNode("Index");
 storage<<data.Index;
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, ULinkSide &data)
{
 if(storage.GetNodeAttribute("Type") != "ULinkSide")
  return storage;

 if(!storage.SelectNode("Id"))
  return storage;
 operator >>(storage,data.Id);
 storage.SelectUp();

 if(!storage.SelectNode("Index"))
  return storage;
 operator >>(storage,data.Index);
 storage.SelectUp();

 return storage;
}

// ULink
USerStorageXML& operator << (USerStorageXML& storage, const ULink &data)
{
 storage.SetNodeAttribute("Type","ULink");

 storage.AddNode("Item");
 storage<<data.Item;
 storage.SelectUp();

 storage.AddNode("Connector");
 storage<<data.Connector;
 storage.SelectUp();

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

 if(!storage.SelectNode("Connector"))
  return storage;
 operator >>(storage,data.Connector);
 storage.SelectUp();

 return storage;
}

// ULinksList
USerStorageXML& operator << (USerStorageXML& storage, const ULinksList &data)
{
 storage.SetNodeAttribute("Type","ULinksList");
 unsigned int size=data.GetSize();
 storage.SetNodeAttribute("Size",sntoa(size));

 if(size <= 0)
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
 size=atoi(storage.GetNodeAttribute("Size"));

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
}
#endif

