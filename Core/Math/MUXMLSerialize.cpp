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

USerStorageXML& operator << (USerStorageXML& storage, const MMatrixSize &data)
{
 storage<<data.GetDims();
 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, MMatrixSize &data)
{
 std::vector<int> temp;
 storage>>temp;
 data=temp;
 return storage;
}

}
#endif

