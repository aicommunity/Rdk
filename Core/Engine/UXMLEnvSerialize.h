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
#include "../Serialize/UXMLStdSerialize.h"
#include "UEnvSupport.h"
#include "UELockVar.h"

namespace RDK {

// UIdVector
//RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UIdVector &data);
//RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UIdVector &data);

// ULongIdVector
//RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const ULongIdVector &data);
//RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, ULongIdVector &data);

// ULinkSide
USerStorageXML& operator << (USerStorageXML& storage, const ULinkSide &data);

USerStorageXML& operator >> (USerStorageXML& storage, ULinkSide &data);

// ULink
USerStorageXML& operator << (USerStorageXML& storage, const ULink &data);

USerStorageXML& operator >> (USerStorageXML& storage, ULink &data);

// ULinksList
USerStorageXML& operator << (USerStorageXML& storage, const ULinksList &data);

USerStorageXML& operator >> (USerStorageXML& storage, ULinksList &data);

// UELockVar
template<typename T>
USerStorageXML& operator << (USerStorageXML& storage, const UELockVar<T> &data)
{
 storage<<data.operator T();

 return storage;
}

template<typename T>
USerStorageXML& operator >> (USerStorageXML& storage, UELockVar<T> &data)
{
 T temp;
 storage>>temp;
 data=temp;

 return storage;
}

}
#endif

