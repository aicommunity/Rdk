#ifndef UCOMPONENT_DESCRIPTION_H
#define UCOMPONENT_DESCRIPTION_H

#include <string>
//#include <map>
#include "../Serialize/USerStorageXML.h"

namespace RDK {

//class UComponent;

class UComponentDescription
{
protected: // Данные
// Имя класса
std::string ClassName;

// Заголовок класса
std::string Header;

// Описание
std::string Description;

public: // Классы описания исключений
 /* Базовый класс описания исключений */
 class IException
 {
 public: // Данные исключения

 public: // Методы
 // --------------------------
 // Конструкторы и деструкторы
 // --------------------------
 IException(void);
// IException(const UContainer *cont);
 IException(const IException &copy);
 virtual ~IException(void);
 // --------------------------
 };
/* **************************** */


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UComponentDescription(void);
UComponentDescription(const UComponentDescription &copy);
//UComponentDescription(std::string classname, UComponent *prototype);
virtual ~UComponentDescription(void);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Имя класса
const std::string& GetClassNameValue(void) const;
void SetClassNameValue(const std::string& value);

// Заголовок класса
const std::string& GetHeader(void) const;
void SetHeader(const std::string& value);

// Описание
const std::string& GetDescription(void) const;
void SetDescription(const std::string& value);
// --------------------------

// --------------------------
// Методы ввода вывода данными
// --------------------------
// Сохраняет данные класса в XML
virtual bool Save(Serialize::USerStorageXML &xml);

// Загружает данные класса из XML
virtual bool Load(Serialize::USerStorageXML &xml);
// --------------------------

};


}

#endif

