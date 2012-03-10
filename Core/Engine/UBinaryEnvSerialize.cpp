/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBINARY_ENV_SERIALIZE_CPP
#define UBINARY_ENV_SERIALIZE_CPP

#include "UBinaryEnvSerialize.h"

namespace RDK {
namespace Serialize {

// UBMColorModel
/*
USerStorageBinary& operator << (USerStorageBinary& storage, UBMColorModel data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBMColorModel &data)
{
 return USimpleFromStorage(storage,data);
} */

// UItemData
USerStorageBinary& operator << (USerStorageBinary& storage, const UItemData &data)
{
 storage<<data.GetSize();
 storage<<data.GetDataSize();
 for(int i=0;i<data.GetByteSize();i++)
  storage<<data.UChar[i];

 return storage;
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UItemData &data)
{
 int temp;
 storage>>temp;
 data.Resize(temp);
 storage>>temp;
 data.SetDataSize(temp);
 for(int i=0;i<data.GetByteSize();i++)
  storage>>data.UChar[i];

 return storage;
}


}
}

#endif
