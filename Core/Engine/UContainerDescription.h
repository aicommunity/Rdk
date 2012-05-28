#ifndef UCONTAINER_DESCRIPTION_H
#define UCONTAINER_DESCRIPTION_H

#include <map>
#include "UComponentDescription.h"

namespace RDK {

// Класс описания параметра компонента
class UParameterDescription
{
public: // Данные
// Заголовок параметра
std::string Header;

// Описание параметра
std::string Description;


public: // Методы

};

class UContainerDescription: public UComponentDescription
{
protected: // Данные
// Описания общих параметров
static std::map<std::string, UParameterDescription> CommonParameters;

// Описания параметров
std::map<std::string, UParameterDescription> Parameters;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UContainerDescription(void);
UContainerDescription(const UContainerDescription &copy);
virtual ~UContainerDescription(void);
// --------------------------

// --------------------------
// Методы управления общими параметрами
// --------------------------
// Описание общего параметра
static const UParameterDescription& GetCommonParameter(const std::string &name);
static bool SetCommonParameter(const std::string &name, const UParameterDescription& value);

// Проверяет наличие общего параметра с заданным именем
static bool CheckCommonParameter(const std::string &name);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Описание параметра
const UParameterDescription& GetParameter(const std::string &name);
bool SetParameter(const std::string &name, const UParameterDescription& value, bool force=false);

// Удаляет параметр из этого описания, если он есть в общих описаниях
bool RemoveCommonDuplicatesParameter(const std::string &name);

// Удаляет все параметры из этого описания, если они есть в общих описаниях
bool RemoveCommonDuplicatesParameters(void);
// --------------------------

// --------------------------
// Методы ввода вывода данных
// --------------------------
// Сохраняет данные класса в XML
virtual bool Save(Serialize::USerStorageXML &xml);

// Загружает данные класса из XML
virtual bool Load(Serialize::USerStorageXML &xml);
// --------------------------

// --------------------------
// Методы ввода вывода общих данных
// --------------------------
// Сохраняет данные класса в XML
static bool SaveCommon(Serialize::USerStorageXML &xml);

// Загружает данные класса из XML
static bool LoadCommon(Serialize::USerStorageXML &xml);
// --------------------------
};


}

#endif

