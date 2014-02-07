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

//#include "ULibrary.h"
#include "UStorage.h"

namespace RDK {

class ULibrary//: public ULibrary
{
protected: // Данные единой коллекции библиотек
/// Список статически загруженных библиотек
//static std::list<ULibrary*> LibraryList;

protected: // Параметры
// Имя библиотеки
string Name;

// Версия библиотеки
string Version;

/// Зависимости библиотеки от других библиотек
/// вида <имя библиотеки, версия библиотеки>
std::vector<pair<string, string> > Dependencies;

protected: // Данные загрузки
/// Имена классов библиотеки
vector<string> ClassesList;

// Содержит имена всех успешно загруженных образцов
vector<string> Complete;

// Содержит имена всех не загруженных образцов
vector<string> Incomplete;

private: // Внутренние данные
// Хранилище для загрузки
UStorage *Storage;
// Массив образцов классов, формируемый библиотекой
//map<string, UComponent*> ClassSamples;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULibrary(const string &name, const string &version);
virtual ~ULibrary(void);
// --------------------------

// --------------------------
// Методы управления статически загруженными библиотеками
// --------------------------
/*
/// Возвращает коллекцию статически загруженных библиотек
static const std::list<ULibrary*>& GetLibraryList(void);

/// Очищает коллекцию библиотек
static void ClearLibraryList(void);

/// Проверяет наличие библиотеки по ее имени
static bool CheckLibrary(const std::string &name);

/// Добавляет библиотеку в список, если библиотеки с таким именем еще нет в списке
static bool AddUniqueLibrary(ULibrary* const lib);

/// Удаляет библиотеку из списка.
/// Если список становится пуст, то уничтожает его
static void RemoveLibrary(ULibrary* const lib);
*/
// --------------------------

// --------------------------
// Методы доступа к параметрам библиотеки
// --------------------------
// Возвращает имя библиотеки
const string& GetName(void) const;

// Возвращает версию библиотеки
const string& GetVersion(void) const;

/// Зависимости библиотеки от других библиотек
const std::vector<pair<string, string> > GetDependencies(void) const;
// --------------------------

// --------------------------
// Методы доступа к данным загрузки
// --------------------------
/// Возвращает true если коллекция предоставляет класс с таким именем
bool IsClassNamePresent(const std::string &class_name) const;

/// Имена классов библиотеки
const vector<string>& GetClassesList(void) const;

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
virtual int Upload(UStorage *storage);
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
protected:
/// Проверяет зависимости библиотеки от других библиотек
/// и возвращает список недостающих библиотек
/// Возвращает true если все необходимые библиотеки уже загружены
bool CheckDependencies(UStorage *storage, std::vector<pair<string, string> > &dependencies) const;

// Добавляет в хранилище очередной класс
virtual bool UploadClass(const UId &classid, UEPtr<UComponent> cont);
virtual bool UploadClass(const string &name, UEPtr<UComponent> cont);

// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
virtual void CreateClassSamples(UStorage *storage)=0;
// --------------------------
};

typedef ULibrary* PUALibrary;

}

#endif
