#ifndef UCOMPONENT_DESCRIPTION_CPP
#define UCOMPONENT_DESCRIPTION_CPP

#include "UComponentDescription.h"
//#include "UComponent.h"
#include "../Utilities/USupport.h"
#include "UStorage.h"
namespace RDK {

/* Базовый класс описания исключений */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UComponentDescription::IException::IException(void)
{

}
// IException(const UContainer *cont);
UComponentDescription::IException::IException(const IException &copy)
{

}

UComponentDescription::IException::~IException(void)
{

}
// --------------------------
/* ******************************** */

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UComponentDescription::UComponentDescription(void)
    : Storage(NULL)
{

}

UComponentDescription::UComponentDescription(const UComponentDescription &copy)
 : 	ClassName(copy.ClassName),
	Header(copy.Header),
    Description(copy.Description),
    Storage(copy.Storage)
{

}

UComponentDescription::~UComponentDescription(void)
{

}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Имя класса
const std::string& UComponentDescription::GetClassNameValue(void) const
{
 return ClassName;
}

void UComponentDescription::SetClassNameValue(const std::string& value)
{
 if(ClassName == value)
  return;

 ClassName=value;
 CreateProperties();
}

// Заголовок класса
const std::string& UComponentDescription::GetHeader(void) const
{
 return Header;
}

void UComponentDescription::SetHeader(const std::string& value)
{
 if(Header == value)
  return;

 Header=value;
}

// Описание
const std::string& UComponentDescription::GetDescription(void) const
{
 return Description;
}

void UComponentDescription::SetDescription(const std::string& value)
{
 if(Description == value)
  return;

 Description=value;
}

// Установка указателя на хранилище
void UComponentDescription::SetStorage(UEPtr<UStorage> storage)
{
    Storage = storage;
}

// Создание описаний свойств единожды (вызов к хранилищу)
void UComponentDescription::CreateProperties()
{

}
// --------------------------

// --------------------------
// Методы ввода вывода данными
// --------------------------
// Сохраняет данные класса в XML
bool UComponentDescription::Save(USerStorageXML &xml)
{
 xml.AddNode("ClassName");
 xml.SetNodeText(ClassName);
 xml.SelectUp();

 xml.AddNode("Header");
 xml.SetNodeText(Header);
 xml.SelectUp();

 xml.AddNode("Description");
 xml.SetNodeText(Description);
 xml.SelectUp();
 return true;
}

// Загружает данные класса из XML
bool UComponentDescription::Load(USerStorageXML &xml)
{
 if(xml.SelectNode("ClassName"))
 {
  ClassName=xml.GetNodeText();
  xml.SelectUp();
 }

 if(xml.SelectNode("Header"))
 {
  Header=xml.GetNodeText();
  xml.SelectUp();
 }

 if(xml.SelectNode("Description"))
 {
  Description=xml.GetNodeText();
  xml.SelectUp();
 }

 return true;
}
// --------------------------

}
#endif


