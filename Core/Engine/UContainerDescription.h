#ifndef UCONTAINER_DESCRIPTION_H
#define UCONTAINER_DESCRIPTION_H

#include <map>
#include <vector>
#include "UComponentDescription.h"

namespace RDK {

// Класс описания свойства компонента
class RDK_LIB_TYPE UPropertyDescription
{
public: // Данные
// Краткое описание (заголовок) свойства
std::string Header;

// Описание свойства
std::string Description;

// Тип данных свойства
std::string Type;

// Тип выбора вариантов данных свойства
// 0 - произвольные данные
// 1 - Checkbox
// 2 - Диапазон
// 3 - Список вариантов
// 4 - Диапазон с заданным шагом
int DataSelectionType;

/// Список значений
/// В режиме диапазона, список состоит из двух значений
/// начала и конца диапазона
std::vector<std::string> ValueList;

/// Шаг по диапазону
std::string Step;

/// Тип свойства
unsigned int PropertyType;


public: // Методы
UPropertyDescription(void);

};

class RDK_LIB_TYPE UContainerDescription: public UComponentDescription
{
protected: // Данные
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
// Методы управления данными
// --------------------------
// Описание свойства
UPropertyDescription& GetPropertyDescription(const std::string &name);
bool SetPropertyDescription(const std::string &name, const UPropertyDescription& value, bool force=false);

// Удаляет свойство из этого описания, если он есть в общих описаниях
bool RemoveCommonDuplicatesDescription(const std::map<std::string, UPropertyDescription> &common_descriptions, const std::string &name);

// Удаляет все свойства из этого описания, если они есть в общих описаниях
bool RemoveCommonDuplicatesDescriptions(const std::map<std::string, UPropertyDescription> &common_descriptions);

// Создание описаний свойств единожды (вызов к хранилищу)
virtual void CreateProperties();

std::map<std::string, UPropertyDescription>& GetProperties();
// --------------------------

// --------------------------
// Методы ввода вывода данных
// --------------------------
// Сохраняет данные класса в XML
virtual bool Save(USerStorageXML &xml);

// Загружает данные класса из XML
virtual bool Load(USerStorageXML &xml);
// --------------------------
};


}

#endif

