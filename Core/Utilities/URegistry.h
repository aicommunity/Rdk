/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UREGISTRY_H
#define UREGISTRY_H

#include <map>
#include "UTree.h"

namespace RDK {

typedef map<string,string> URegData;
typedef map<string,string>::iterator URegDataIterator;
typedef UTree<URegData> URegistry;

typedef map<string,URegData> UClassRegData;
typedef map<string,URegData>::iterator UClassRegDataIterator;
typedef UTree<UClassRegData> UClassRegistry;


} // end namespace Utilities

#endif
