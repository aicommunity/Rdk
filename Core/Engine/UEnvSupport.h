/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UEnvSupportH
#define UEnvSupportH

#include <string>
#include "UAEnvSupport.h"
#include "UPointer.h"
#include "UProperty.h"
//#include "../USerialize/USerStorage.h"
#include "../Serialize/Serialize.h"

namespace RDK {

typedef std::string NameT;
typedef Serialize::USerStorage UVariableData;

// Хранилище свойств параметра
struct UVariable
{
 // Id параметра
 UId Id;

 // Указатель на свойство
 UIProperty* Property;

 // Флаг разрешения удаления данных на которых указывает Property
 bool DelEnable;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UVariable(void);
UVariable(UId id, UIProperty *prop);
UVariable(const UVariable &copy);
virtual ~UVariable(void);
// --------------------------
};

// Хранилище свойств указателя
struct UPVariable
{
 // Id указателя
 UId Id;

 // Указатель на свойство
 UIPointer* Pointer;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPVariable(void);
UPVariable(UId id, UIPointer *prop);
virtual ~UPVariable(void);
// --------------------------
};

// Операторы ввода вывода идентификаторов
ULongId& operator << (ULongId& id, const std::string &str);
std::string& operator >> (const ULongId& id, std::string &str);

}
#endif
