#ifndef UCONTAINER_DESCRIPTION_H
#define UCONTAINER_DESCRIPTION_H

#include <map>
#include "UComponentDescription.h"

namespace RDK {

// Класс описания свойства компонента
class UPropertyDescription
{
public: // Данные
// Заголовок свойства
std::string Header;

// Описание свойства
std::string Description;


public: // Методы

};

class UContainerDescription: public UComponentDescription
{
protected: // Данные
// Описания общих свойств
static std::map<std::string, UPropertyDescription> CommonProperties;

// Описания свойств
std::map<std::string, UPropertyDescription> Properties;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UContainerDescription(void);
UContainerDescription(const UContainerDescription &copy);
virtual ~UContainerDescription(void);
// --------------------------

// --------------------------
// Методы управления общими свойствами
// --------------------------
// Описание общего свойства
static const UPropertyDescription& GetCommonProperty(const std::string &name);
static bool SetCommonProperty(const std::string &name, const UPropertyDescription& value);

// Проверяет наличие общего свойства с заданным именем
static bool CheckCommonProperty(const std::string &name);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Описание свойства
const UPropertyDescription& GetProperty(const std::string &name);
bool SetProperty(const std::string &name, const UPropertyDescription& value, bool force=false);

// Удаляет свойство из этого описания, если он есть в общих описаниях
bool RemoveCommonDuplicatesProperty(const std::string &name);

// Удаляет все свойства из этого описания, если они есть в общих описаниях
bool RemoveCommonDuplicatesProperties(void);
// --------------------------

// --------------------------
// Методы ввода вывода данных
// --------------------------
// Сохраняет данные класса в XML
virtual bool Save(USerStorageXML &xml);

// Загружает данные класса из XML
virtual bool Load(USerStorageXML &xml);
// --------------------------

// --------------------------
// Методы ввода вывода общих данных
// --------------------------
// Сохраняет данные класса в XML
static bool SaveCommon(USerStorageXML &xml);

// Загружает данные класса из XML
static bool LoadCommon(USerStorageXML &xml);
// --------------------------
};


}

#endif

