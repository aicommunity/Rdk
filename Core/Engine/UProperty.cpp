/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UPROPERTY_CPP
#define UPROPERTY_CPP

#include "UProperty.h"
//#include <strstream>

namespace RDK {

// -----------------------------
// Методы сериализации
// -----------------------------
// Метод возвращает строковое имя свойства
/*template<typename T,typename OwnerT>
string UVProperty<T,OwnerT>::GetName(void) const
{
 return reinterpret_cast<UContainer* const>(Owner)->FindPropertyName(this);
} */


// Метод возвращает строковое имя свойства без имени пространств имен
/*string UIProperty::GetName(void) const
{
 string name=typeid(this).name();
 int i=name.find_last_of(":");

 if(i != string::npos)
  return name.substr(i+1);

 return name;
} */

}
#endif


 
