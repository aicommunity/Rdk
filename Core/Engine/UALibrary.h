/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UALibraryH
#define UALibraryH

//#include "UALibrary.h"
#include "UAContainerStorage.h"

namespace RDK {

class UALibrary//: public UALibrary
{
protected: // Параметры
// Имя библиотеки
string Name;

// Версия библиотеки
string Version;

protected: // Данные загрузки
// Содержит имена всех успешно загруженных образцов
vector<string> Complete;

// Содержит имена всех не загруженных образцов
vector<string> Incomplete;

private: // Внутренние данные
// Хранилище для загрузки
UAContainerStorage *Storage;
// Массив образцов классов, формируемый библиотекой
//map<string, UAComponent*> ClassSamples;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UALibrary(const string &name, const string &version);
virtual ~UALibrary(void);
// --------------------------

// --------------------------
// Методы доступа к параметрам библиотеки
// --------------------------
// Возвращает имя библиотеки
const string& GetName(void) const;

// Возвращает версию библиотеки
const string& GetVersion(void) const;
// --------------------------

// --------------------------
// Методы доступа к данным загрузки
// --------------------------
// Содержит имена всех успешно загруженных образцов
const vector<string>& GetComplete(void) const;

// Содержит имена всех не загруженных образцов
const vector<string>& GetIncomplete(void) const;
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Заполняет заданное хранилище набором образцов классов.
// Если класс с заданным именем уже существует, то он пропускается.
// Возвращает число реально загруженных классов.
virtual int Upload(UAStorage *storage);
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
protected:
// Добавляет в хранилище очередной класс
virtual bool UploadClass(const UId &classid, UAComponent *cont);
virtual bool UploadClass(const string &name, UAComponent *cont);

// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
virtual void CreateClassSamples(UAStorage *storage)=0;
// --------------------------
};

typedef UALibrary* PUALibrary;

}

#endif
