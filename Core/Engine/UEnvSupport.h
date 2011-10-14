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


#include "../Serialize/Serialize.h"

namespace RDK {

typedef std::string NameT;
typedef Serialize::USerStorage UVariableData;
extern NameT ForbiddenName;

class UAContainer;

// Класс описания локальных указателей
class UIPointer
{
protected: // Данные

public:
virtual UAContainer* const Get(void) const=0;

virtual void Set(UAContainer* source)=0;

virtual void Del(UAContainer* source)=0;

// Проверяет, существует ли такой указатель в этом классе
// Возвращает 0 если да, и <0 если нет
virtual int Find(const UAContainer * cont) const=0;

// -----------------
// Операторы
// -----------------
UIPointer& operator = (UAContainer *source)
{
 Set(source);
 return *this;
};
// -----------------
};

// Класс сериализации свойств
class UIProperty
{
public:
 // Метод возвращает строковое имя свойства
 virtual const std::string& GetName(void) const=0;

 // Метод возвращает строковое имя класса-владельца свойства
 virtual std::string GetOwnerName(void) const=0;

 // Метод записывает значение свойства в поток
 virtual bool Save(Serialize::USerStorage *storage)=0;

 // Метод читает значение свойства из потока
 virtual bool Load(Serialize::USerStorage *storage)=0;
};



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
