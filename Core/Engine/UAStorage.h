/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UASTORAGE_H
#define UASTORAGE_H

#include <map>
#include "UEPtr.h"
#include "UAComponent.h"
#include "../Serialize/USerStorageXML.h"
#include "UComponentDescription.h"

namespace RDK {

typedef UEPtr<UAComponent> UClassStorageElement;
typedef std::map<UId, UClassStorageElement> UClassesStorage;
typedef std::map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;

typedef std::map<std::string, UEPtr<UComponentDescription> > UClassesDescription;
typedef std::map<std::string, UEPtr<UComponentDescription> >::iterator UClassesDescriptionIterator;
typedef std::map<std::string, UEPtr<UComponentDescription> >::const_iterator UClassesDescriptionCIterator;

class UAStorage
{
protected: // Системные свойства
// Таблица соответствий имен и Id образцов классов
std::map<std::string,UId> ClassesLookupTable;

// Хранилище образцов классов
UClassesStorage ClassesStorage;

protected: // Описания классов
// XML описания всех классов хранилища
UClassesDescription ClassesDescription;

protected: // Основные свойства
// Последний использованный Id образцов классов
UId LastClassId;

protected: // Временные переменные


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAStorage(void);
virtual ~UAStorage(void);
// --------------------------

// --------------------------
// Методы управления свойствами
// --------------------------
// Возвращает последний использованный Id классов
UId GetLastClassId(void) const;
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает Id класса по его имени
const UId& GetClassId(const std::string &name) const;

// Возвращает имя класса по его Id
const NameT GetClassName(const UId &id) const;
// --------------------------

// --------------------------
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
// Если classid == ForbiddenId, то id назначается автоматически
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const UId &classid=ForbiddenId);
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const std::string &classname, const UId &classid=ForbiddenId);

// Удаляет образец класса объекта из хранилища
virtual void DelClass(const UId &classid);

// Проверяет наличие образца класса объекта в хранилище
virtual bool CheckClass(const UId &classid) const;

// Возвращает образец класса
virtual UEPtr<UAComponent> GetClass(const UId &classid) const;

// Возвращает число классов
int GetNumClasses(void) const;

// Возвращает список идентификаторов всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassIdList(std::vector<UId> &buffer) const;

// Возвращает список имен всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
virtual void GetClassNameList(std::vector<std::string> &buffer) const;



// Удаляет все образцы классов из хранилища
virtual void ClearClassesStorage(void);
// --------------------------

// --------------------------
// Методы управления хранилищем объектов
// --------------------------
// Извлекает объект из хранилища
// Возвращает указатель на свободный объект по имени класса
// Выбранный объект помечается как занятый в хранилище
// Флаг 'Activity' объекта выставляется в true
// Если свободного объекта не существует он создается и добавляется
// в хранилище
virtual UEPtr<UAComponent> TakeObject(const UId &classid, const UEPtr<UAComponent> prototype=0);

// Возвращает Id класса, отвечающий объекту 'object'
virtual UId FindClass(UEPtr<UAComponent> object) const;
// --------------------------

// --------------------------
// Методы управления описанием классов
// --------------------------
// Возвращает XML описание класса
const UEPtr<UComponentDescription> GetClassDescription(const UId &classid) const;

// Устанавливает XML описание класса
// Класс в хранилище должен существовать
void SetClassDescription(const UId &classid, const UEPtr<UComponentDescription>& description);

// Сохраняет описание класса в xml
virtual void SaveClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml);

// Загружает описание класса из xml
virtual void LoadClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml);

// Сохраняет описание всех классов в xml
virtual void SaveClassesDescription(Serialize::USerStorageXML &xml);

// Загружает описание всех классов из xml
virtual void LoadClassesDescription(Serialize::USerStorageXML &xml);

// Сохраняет общее описание всех классов в xml
virtual bool SaveCommonClassesDescription(Serialize::USerStorageXML &xml);

// Загружает общее описание всех классов из xml
virtual bool LoadCommonClassesDescription(Serialize::USerStorageXML &xml);
// --------------------------

// --------------------------
// Скрытые методы управления хранилищем объектов
// --------------------------
protected:
// Возвращает объект в хранилище
// В текущей реализации всегда удаляет объект и возвращает true
virtual void ReturnObject(UEPtr<UAComponent> object);
// --------------------------

// --------------------------
// Скрытые методы таблицы соответствий классов
// --------------------------
protected:
// Добавляет класс с именем 'name' в таблицу соответствий
virtual UId AddLookupClass(const std::string &name);

// Удаляет класс с именем 'name' из таблицы соотвествий
virtual void DelLookupClass(const std::string &name);
// --------------------------


// --------------------------
// Исключения
// --------------------------
public:
class IException: public Exception {};

// Попытка работы с классом по идентификатору classid отсутствующим в хранилище
struct EClassIdNotExist: public EIdNotExist
{
EClassIdNotExist(UId id) : EIdNotExist(id) {};
};

// Класс с заданным именем уже существует
struct EClassIdAlreadyExist: public EIdAlreadyExist
{
EClassIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// Попытка работы с классом по имени, отсутствующему в хранилище
struct EClassNameNotExist: public ENameNotExist
{
EClassNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// Класс с заданным именем уже существует
struct EClassNameAlreadyExist: public ENameAlreadyExist
{
EClassNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// Некорректное имя класса
struct EInvalidClassName: public ENameError
{
EInvalidClassName(const std::string &name) : ENameError(name) {};
};
// --------------------------
};
	 /*
// Попытка работы с классом по идентификатору classid отсутствующим в хранилище
class UAStorage::EClassIdNotExist: public EError
{
public: // Данные
// Ошибочный идентификатор
UId Id;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EClassIdNotExist(UId id);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Попытка работы с классом по имени, отсутствующему в хранилище
class UAStorage::EClassNameNotExist: public EError
{
public: // Данные
// Ошибочный идентификатор
std::string Name;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EClassNameNotExist(const std::string &name);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Некорректное имя класса
class UAStorage::EInvalidClassName: public EError
{
public: // Данные
// Ошибочный идентификатор
std::string Name;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EInvalidClassName(const std::string &name);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Класс с заданным именем уже существует
class UAStorage::EClassNameAlreadyExist: public EError
{
public: // Данные
// Ошибочное имя
std::string Name;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EClassNameAlreadyExist(const std::string &name);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

                  */
}


#endif

