#ifndef UADATA_COMPONENT_H
#define UADATA_COMPONENT_H

#include "UAComponent.h"
#include "UAEnvSupport.h"
#include "UContainerDescription.h"
#include "../Serialize/Serialize.h"

namespace RDK {

typedef Serialize::USerStorage UVariableData;

class UADataComponent;

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



class UADataComponent: public UAComponent
{
public: // Типы данных
typedef std::map<NameT,UVariable> VariableMapT;
typedef std::map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

private: // Системные свойства
//protected: // Системные свойства
// Таблица соответствий имен и Id параметров объекта
VariableMapT PropertiesLookupTable;

// Таблица соответствий имен и Id данных состояния объекта
VariableMapT StateLookupTable;


// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
public:
// Возвращает имя параметра по его Id
const NameT& GetPropertyName(const UId &id) const;

// Возвращает Id параметра по его имени
const UId& GetPropertyId(const NameT &name) const;

// Возвращает полное имя параметра без префикса NMSDK, и суффикса '*'
//NameT GetPropertyLongName(const NameT &name) const;
//NameT GetPropertyLongName(const UId &id) const;

// Возвращает имя переменной состояния по его Id
const NameT& GetStateName(const UId &id) const;

// Возвращает Id переменной состояния по его имени
const UId& GetStateId(const NameT &name) const;

// Возвращает полное имя переменной состояния без префикса NMSDK, и суффикса '*'
//NameT GetStateLongName(const NameT &name) const;
//NameT GetStateLongName(const UId &id) const;
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Создает экземпляр описания класса
virtual UContainerDescription* NewDescription(void);
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Возвращает значение параметра по Id 'id'
UVariableData* GetProperty(const UId &id, UVariableData *values) const;

// Возвращает значение параметра по имени 'name'
UVariableData* GetProperty(const NameT &name, UVariableData *values) const;

// Устанавливает значение параметра по Id 'id'
bool SetProperty(const UId &id, UVariableData *values);

// Устанавливает значение параметра по имени 'name'
bool SetProperty(const NameT &name, UVariableData *values);

// Возвращает список Id параметров, содержащихся непосредственно
// в этом объекте
const UADataComponent::VariableMapT& GetPropertiesList(void) const;

// Ищет имя свойства по указателю на него
const NameT& FindPropertyName(const UIProperty *prop) const;

// Копирует все параметры этого объекта в объект 'comp', если возможно.
virtual bool CopyProperties(UEPtr<UADataComponent> comp) const;
// --------------------------

// --------------------------
// Методы доступа к переменным состояния
// --------------------------
// Возвращает значение переменной состояния по Id 'id'
virtual UVariableData* GetState(const UId &id, UVariableData *values) const;

// Возвращает значение переменной состояния по имени 'name'
UVariableData* GetState(const NameT &name, UVariableData *values) const;

// Устанавливает значение переменной состояния по Id 'id'
virtual bool SetState(const UId &id, UVariableData *values);

// Устанавливает значение переменной состояния по имени 'name'
bool SetState(const NameT &name, UVariableData *values);

// Возвращает список имен и Id переменных состояния, содержащихся непосредственно
// в этом объекте
const UADataComponent::VariableMapT& GetStateList(void) const;

// Ищет имя свойства по указателю на него
const NameT& FindStateName(const UIProperty *prop) const;

// Копирует все переменные состояния этого объекта в объект 'comp', если возможно.
virtual bool CopyState(UEPtr<UADataComponent> comp) const;
// --------------------------

// --------------------------
// Скрытые методы управления параметрами
// --------------------------
public:
// Добавляет параметр с именем 'name' в таблицу соотвествий
// параметров и назначает ему корректный индекс
// Должна вызываться в конструкторах классов
UId AddLookupProperty(const NameT &name, UIProperty *property, bool delenable=true);

protected:
// Удаляет параметр с именем 'name' из таблицы соотвествий
// параметров
bool DelLookupProperty(const NameT &name);

// Удаляет всю таблицу соответствий
void ClearLookupPropertyTable(void);

// Возвращает полное имя параметра без префикса RDK, и суффикса '*'
//NameT GetPropertyLongName(const UIProperty &property) const;
// --------------------------

protected:
// --------------------------
// Скрытые методы управления состоянием
// --------------------------
public:
// Добавляет переменную состояния с именем 'name' в таблицу соотвествий
// параметров и назначает ей корректный индекс
// Должна вызываться в конструкторах классов
UId AddLookupState(const NameT &name,UIProperty *property, bool delenable=true);

protected:
// Удаляет переменную состояния с именем 'name' из таблицы соотвествий
bool DelLookupState(const NameT &name);

// Удаляет всю таблицу соответствий
void ClearLookupStateTable(void);

// Возвращает полное имя переменной состояния без префикса NMSDK, и суффикса '*'
//NameT GetStateLongName(const UIProperty &property) const;
// --------------------------
};

}

#include "UProperty.h"
#include "ULocalProperty.h"

#endif 

