/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UGRAPHICS_BINARY_SERIALIZE_CPP
#define UGRAPHICS_BINARY_SERIALIZE_CPP

#include "UGraphicsBinarySerialize.h"

namespace RDK {
namespace Serialize {

USerStorageBinary& operator << (USerStorageBinary& storage, UBMColorModel data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBMColorModel &data)
{
 return USimpleFromStorage(storage,data);
}

}
}

#endif
