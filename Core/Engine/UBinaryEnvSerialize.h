/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBinaryEnvSerializeH
#define UBinaryEnvSerializeH

#include "UItemData.h"
#include "../Serialize/USerStorageBinary.h"
#include "UELockVar.h"

namespace RDK {

// UBMColorModel
//USerStorageBinary& operator << (USerStorageBinary& storage, UBMColorModel data);
//USerStorageBinary& operator >> (USerStorageBinary& storage, UBMColorModel &data);

// UItemData
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UItemData &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UItemData &data);


// UELockVar
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const UELockVar<T> &data)
{
 storage<<data.operator T();

 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, UELockVar<T> &data)
{
 T temp;
 storage>>temp;
 data=temp;

 return storage;
}


}
#endif
