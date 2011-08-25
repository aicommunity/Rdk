/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MUSERIALIZE_CPP
#define MUSERIALIZE_CPP

#include "../Serialize/UBinaryStdSerialize.h"
#include "MUBinarySerialize.h"

namespace RDK {
namespace Serialize {
                           
// MBorder
USerStorageBinary& operator << (USerStorageBinary& storage, const MBorder &data)
{
 operator << (storage,data.GetVertexIndex());
 return storage;
}

USerStorageBinary& operator >> (USerStorageBinary& storage, MBorder &data)
{
 std::vector<int> temp;
 operator >> (storage,temp);
 data=temp;
 return storage;
}

}
}
#endif

