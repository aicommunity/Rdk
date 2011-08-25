/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UENV_SUPPORT
#define UENV_SUPPORT

#include "../Utilities/USupport.h"
#include "UEnvSupport.h"


namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UVariable::UVariable(void)
{
 Id=ForbiddenId;
}

UVariable::UVariable(UId id, UIProperty *prop)
 : Id(id), Property(prop)
{
}

UVariable::~UVariable(void)
{
}
// --------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPVariable::UPVariable(void)
{
 Id=ForbiddenId;
}

UPVariable::UPVariable(UId id, UIPointer *prop)
 : Id(id), Pointer(prop)
{

}

UPVariable::~UPVariable(void)
{

}
// --------------------------



// Операторы ввода вывода идентификаторов
ULongId& operator << (ULongId& id, const std::string &str)
{
 id.Resize(0);

 std::size_t start=str.find_first_not_of("0123456789. ");
 std::size_t stop;
 if(start != string::npos)
  return id;

 start=stop=0;
 while(start != string::npos)
 {
  std::size_t start=str.find_first_of("0123456789",stop);
  if(start == string::npos)
   return id;

  std::size_t stop=str.find_first_of(".",start);
  if(stop == string::npos)
  {
   id.Add(atoi(str.substr(start)));
   return id;
  }
  else
   id.Add(atoi(str.substr(start,stop-start)));
 }

 return id;
}

std::string& operator >> (const ULongId& id, std::string &str)
{
 str.clear();
 for(int i=0;i<id.GetSize();i++)
 {
  str+=sntoa(id[i]);
  if(i < id.GetSize()-1)
   str+=".";
 }

 return str;
}

}

#endif


