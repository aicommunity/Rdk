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
#include "UAEnvSupport.h"

namespace RDK {
namespace Serialize {

// UIdVector
USerStorageXML& operator << (USerStorageXML& storage, const UIdVector &data);
USerStorageXML& operator >> (USerStorageXML& storage, UIdVector &data);

// ULongIdVector
USerStorageXML& operator << (USerStorageXML& storage, const ULongIdVector &data);
USerStorageXML& operator >> (USerStorageXML& storage, ULongIdVector &data);

// ULinkSide
USerStorageXML& operator << (USerStorageXML& storage, const ULinkSide &data);
USerStorageXML& operator >> (USerStorageXML& storage, ULinkSide &data);

// ULink
USerStorageXML& operator << (USerStorageXML& storage, const ULink &data);
USerStorageXML& operator >> (USerStorageXML& storage, ULink &data);

// ULinksList
USerStorageXML& operator << (USerStorageXML& storage, const ULinksList &data);
USerStorageXML& operator >> (USerStorageXML& storage, ULinksList &data);

}
}
#endif

