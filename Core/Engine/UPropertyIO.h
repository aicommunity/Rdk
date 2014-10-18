#ifndef UPROPERTY_IO_H
#define UPROPERTY_IO_H

#include "UPropertyOutput.h"
#include "UPropertyInput.h"

namespace RDK {
const T& operator * (void) const
{
 return this->v;
};


}


#endif
