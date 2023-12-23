/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2012.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru
Version:        1.0.0

This file - part of the project: RDK

File License:        BSD License
Project License:    BSD License
See file license.txt for more information
*********************************************************** */

#ifndef ULocalProperty_H
#define ULocalProperty_H

#include <string>
#include <sstream>
#include "UProperty.h"

namespace RDK {

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using ULProperty = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using UCLProperty = UCProperty<T, OwnerT, type>;


}
#endif

