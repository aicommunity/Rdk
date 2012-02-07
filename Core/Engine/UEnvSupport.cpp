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
 Property=0;
 DelEnable=true;
}

UVariable::UVariable(UId id, UIProperty *prop)
 : Id(id), Property(prop), DelEnable(true)
{
}

UVariable::UVariable(const UVariable &copy)
{
 Id=copy.Id;
 Property=copy.Property;
 DelEnable=copy.DelEnable;
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




}

#endif


