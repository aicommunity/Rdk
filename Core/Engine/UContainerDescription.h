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


public: // Методы
UPropertyDescription(void);

};

class UContainerDescription: public UComponentDescription
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
const UPropertyDescription& GetDescription(const std::string &name);
bool SetDescription(const std::string &name, const UPropertyDescription& value, bool force=false);

// Удаляет свойство из этого описания, если он есть в общих описаниях
bool RemoveCommonDuplicatesDescription(const std::map<std::string, UPropertyDescription> &common_descriptions, const std::string &name);

// Удаляет все свойства из этого описания, если они есть в общих описаниях
bool RemoveCommonDuplicatesDescriptions(const std::map<std::string, UPropertyDescription> &common_descriptions);
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

