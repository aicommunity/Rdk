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
#include "../../Deploy/Include/rdk_version.h"

#ifndef RDK_LIB_CVS_REVISION
#define RDK_LIB_CVS_REVISION 0
#endif

namespace RDK {

class UMockLibrary;

class RDK_LIB_TYPE ULibrary//: public ULibrary
{
protected: // Данные единой коллекции библиотек
/// Список статически загруженных библиотек
//static std::list<ULibrary*> LibraryList;

protected: // Параметры
/// Имя библиотеки
string Name;

/// Версия библиотеки
string Version;

/// Ревизия контроля версий
int Revision;

/// Версия ядра, использованая при сборке библиотки
RDK::UEPtr<RDK::UVersion> CoreVersion;

/// Тип библиотеки
/// 0 - Внутренняя библиотека (собрана вместе с ядром)
/// 1 - Внешняя библиотека (загружена из внешней dll)
/// 2 - Библиотека, созданная во время выполнения
/// 3 - Библиотека-заглушка (все компоненты-заглушки)
int Type;

/// Ревизия ядра

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

// Хранилище для загрузки
UStorage *Storage;

private: // Внутренние данные
// Хранилище для загрузки
//UStorage *Storage;
// Массив образцов классов, формируемый библиотекой
//map<string, UComponent*> ClassSamples;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULibrary(const string &name, const string &version, int type=0, int revision=RDK_LIB_CVS_REVISION); // Deprecated
ULibrary(const string &name, const string &version, const RDK::UVersion &core_version, int type=0, int revision=RDK_LIB_CVS_REVISION);
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
/// Возвращает имя библиотеки
const string& GetName(void) const;

/// Возвращает версию библиотеки
const string& GetVersion(void) const;

/// Возвращает ревизию системы контроля версий
int GetRevision(void) const;

/// Возвращает версию ядра, использованного при сборке библиотеки
const UEPtr<RDK::UVersion> GetCoreVersion(void) const;

/// Тип библиотеки
/// 0 - Внутренняя библиотека (собрана вместе с ядром)
/// 1 - Внешняя библиотека (загружена из внешней dll)
/// 2 - Библиотека, созданная во время выполнения
/// 3 - Библиотека-заглушка (все компоненты-заглушки)
int GetType(void) const;

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

// Очищает оба списка Complete и Incomplete
void ClearIncompleteAndComplete(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Создает образец типа T с именем класса в хранилище class_name и именем
/// компонента по умолчанию component_name
/// Возвращает указатель на созданный образец
template<class T>
T* UploadClass(const std::string &class_name, const std::string &component_name, bool memory_measure=false);

template<class T>
void UploadClass(const std::string &class_name);

// Заполняет заданное хранилище набором образцов классов.
// Если класс с заданным именем уже существует, то он пропускается.
// Возвращает число реально загруженных классов.
virtual int Upload(UStorage *storage);
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
/// Проверяет зависимости библиотеки от других библиотек
/// и возвращает список недостающих библиотек
/// Возвращает true если все необходимые библиотеки уже загружены
bool CheckDependencies(UStorage *storage, std::vector<pair<string, string> > &dependencies) const;

/// Добавляет в хранилище очередной класс
virtual bool UploadClass(const UId &classid, UEPtr<UComponent> cont);
virtual bool UploadClass(const string &name, UEPtr<UComponent> cont);
virtual bool UploadClass(const std::string &class_name, const std::string &component_name, UComponent* (*funcPointer)(void));

/// Удаление заданного класса из списка успешно загруженных
/// Класс переносится в незагруженные (Incomplete)
virtual void RemoveClassFromCompletedList(const string &name);

/// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
/// Не требуется предварительная очистка массива и уборка памяти.
virtual void CreateClassSamples(UStorage *storage)=0;

// --------------------------
// Дополнительные методы
// --------------------------
/// Заполняет библиотеку-заглушку всеми XML описаниями собсвтенных компонентов
bool FillMockLibrary(UMockLibrary* lib);

// --------------------------
};

class RDK_LIB_TYPE URuntimeLibrary: public ULibrary
{
protected: // Данные единой коллекции библиотек

protected: // Параметры

protected: // Данные
/// Путь библиотеки
std::string LibPath;

/// Описание текущего компонента XML
USerStorageXML CurrentComponentStruct;

/// Описание компонент библиотеки в формате строк (xml)
vector<string> ClassesStructures;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URuntimeLibrary(const string &name, const string &version, const string& path);
virtual ~URuntimeLibrary(void);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------

/// Возращает путь библиотеки
const std::string& GetLibPath() const;

/// Добавляет новый компонент (описание, сохранение)
bool AddNewClass(const std::string &new_class_name, const std::string &new_comp_name, UContainer *newclass);

/// Удаляет класс из коллекции и Storage
bool DelClass(const std::string &class_name);

/// Загружает описание компонент из файлов в массив строк
bool LoadCompDescriptions(void);

/// Удаляет директорию библиотеки вместе со всеми файлами
bool DeleteOwnDirectory(void);
// --------------------------

// --------------------------
/// Создает компонент из описания xml
UEPtr<UContainer> CreateClassSample(UStorage *storage, USerStorageXML &xml);

// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------

};

class RDK_LIB_TYPE UMockLibrary: public ULibrary
{
protected: // Данные единой коллекции библиотек

protected: // Параметры

protected: // Данные
/// Путь библиотеки
std::string LibPath;

/// Описание текущего компонента XML
USerStorageXML CurrentComponentStruct;

/// Описание компонент библиотеки в формате строк (xml)
vector<string> ClassesStructures;
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UMockLibrary(const string &name, const string &version, const string& path);
virtual ~UMockLibrary(void);


// Добавляет описание компонента в ClassesStructures
bool AddNewCompDescription(USerStorageXML& descript);

// Сохранение в файл
bool SaveLibraryToFile();

// Загрузка описаний классов из xml файла в ClassesStructures
bool LoadFromXML(USerStorageXML& xml);
// --------------------------
/// Создает компонент из описания xml
UEPtr<UContainer> CreateClassSample(USerStorageXML &xml, UStorage *storage);

// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------

};


/// Создает образец типа T с именем класса в хранилище class_name и именем
/// компонента по умолчанию component_name
/// Возвращает указатель на созданный образец
template<class T>
T* ULibrary::UploadClass(const std::string &class_name, const std::string &component_name, bool memory_measure)
{
 unsigned long long total_used_memory_before(0);
 unsigned long long largest_free_block_before(0);
 if(memory_measure)
  ReadUsedMemoryInfo(total_used_memory_before, largest_free_block_before);

 T* cont=new T;
 cont->SetName(component_name);
 UploadClass(class_name, cont);
 cont->Default();
 unsigned long long total_used_memory_after(0);
 unsigned long long largest_free_block_after(0);
 if(memory_measure && ReadUsedMemoryInfo(total_used_memory_after, largest_free_block_after))
  Storage->GetLogger()->LogMessage(RDK_EX_DEBUG, class_name+std::string(" eats ")+sntoa(total_used_memory_after-total_used_memory_before)+std::string(" bytes of RAM. Largest RAM block decreased to ")+sntoa(largest_free_block_before-largest_free_block_after)+" bytes");
 return cont;
}
			   /*
template<class T>
void ULibrary::UploadClass(const std::string &class_name)
{
 UploadClass(class_name, T::NewStatic
}                */


typedef ULibrary* PUALibrary;

}


#endif
