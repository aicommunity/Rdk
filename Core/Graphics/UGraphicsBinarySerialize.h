/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UGraphicsBinarySerializeH
#define UGraphicsBinarySerializeH

#include "UBitmap.h"
#include "../Serialize/USerStorageBinary.h"

namespace RDK {


namespace Serialize {

USerStorageBinary& operator << (USerStorageBinary& storage, UBMColorModel data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBMColorModel &data);

}

}
#endif
