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

typedef std::map<UId, UEPtr<UComponentDescription> > UClassesDescription;
typedef std::map<UId, UEPtr<UComponentDescription> >::iterator UClassesDescriptionIterator;
typedef std::map<UId, UEPtr<UComponentDescription> >::const_iterator UClassesDescriptionCIterator;

class UAStorage
{
protected: // Системные свойства
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
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const UId &classid=ForbiddenId);

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
virtual bool DelClass(const UId &classid);

// Проверяет наличие образца класса объекта в хранилище
virtual bool CheckClass(const UId &classid) const;

// Возвращает образец класса
virtual UEPtr<UAComponent> GetClass(const UId &classid) const;

// Возвращает число классов
int GetNumClasses(void) const;

// Возвращает список идентификаторов всех классов хранилища
// max_num_classes - максимальная длина массива
virtual void GetClassIdList(UId* buffer, int max_num_classes) const;

// Удаляет все образцы классов из хранилища
virtual bool ClearClassesStorage(void);
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
virtual UEPtr<UAComponent> TakeObject(const UId &classid, const UAComponent *prototype=0);

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
bool SetClassDescription(const UId &classid, const UEPtr<UComponentDescription>& description);

// Сохраняет описание класса в xml
virtual bool SaveClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml);

// Загружает описание класса из xml
virtual bool LoadClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml);

// Сохраняет описание всех классов в xml
virtual bool SaveClassesDescription(Serialize::USerStorageXML &xml);

// Загружает описание всех классов из xml
virtual bool LoadClassesDescription(Serialize::USerStorageXML &xml);
// --------------------------

// --------------------------
// Скрытые методы управления хранилищем объектов
// --------------------------
protected:
// Возвращает объект в хранилище
// В текущей реализации всегда удаляет объект и возвращает true
virtual bool ReturnObject(UEPtr<UAComponent> object);
// --------------------------

// --------------------------
// Скрытые методы управления описанием классов
// --------------------------
// Формирует прототип XML описания для заданного класса
// Класс в хранилище должен существовать
// Очищает уже созданные поля описания
//virtual bool GenerateClassDescription(UClassesStorageIterator classI,
//										Serialize::USerStorageXML &xml);
// --------------------------

};

}


#endif

