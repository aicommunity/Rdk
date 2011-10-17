/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UENGINE_H
#define UENGINE_H

#include "UModule.h"
#include "UAContainerEnvironment.h"
#include "UAContainerStorage.h"
#include "../Utilities/UIniFile.h"
#include "UANet.h"

namespace RDK{

//using namespace RDK;


class UEngine: public UModule
{
public:
// Создает новое хранилище
typedef RDK::UAContainerStorage* (*PCreateNewStorage)(void);

// Создает новую среду
typedef RDK::UAContainerEnvironment* (*PCreateNewEnvironment)(void);

protected: // Параметры инициализации
// Указатели на функции создания экземпляров хранилища и среды
PCreateNewStorage FuncCreateNewStorage;
PCreateNewEnvironment FuncCreateNewEnvironment;

// Имя файла инициализации
string OptionsFileName;

// Имя секции выбора основной библиотеки
string MainLibrarySectionName;

// Имя переменной основной библиотеки
string MainLibraryName;

// Имя секции выбора библиотек компонент
string ComponentLibrariesSectionName;

// Имя секции выбора классов компонент
string ComponentClassesSectionName;

// Индекс используемого хранилища в библиотеке
// Если < 0, то новое хранилище будет создано
int StorageIndex;

// Индекс используемой среды в библиотеке
// Если < 0, то новая среда будет создана
int EnvironmentIndex;

protected: // Переменные состояния
// Флаг работы системы
// 1 - система запущена
// 0 - система приостановлена
// -1 - система отключена
int Runned;

protected: // Данные
// Данные инициализации
UIniFile<char> Options;

// Хранилище
UAContainerStorage* Storage;

// Среда
UAContainerEnvironment* Environment;

protected: // Временные переменные
// Список загруженных библиотек
std::list<UALibrary*> LibrariesList;

// Список загруженных образцов классов
std::list<UAContainer*> ClassesList;

RDK::Serialize::USerStorageXML XmlStorage;

// Временное хранилище строк
mutable string TempString;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngine(void);
virtual ~UEngine(void);
// --------------------------

// --------------------------
// Методы управления параметрами инициализации
// --------------------------
// Указатели на функции создания экземпляров хранилища и среды
UEngine::PCreateNewStorage GetFuncCreateNewStorage(void) const;
bool SetFuncCreateNewStorage(UEngine::PCreateNewStorage value);

UEngine::PCreateNewEnvironment GetFuncCreateNewEnvironment(void) const;
bool SetFuncCreateNewEnvironment(UEngine::PCreateNewEnvironment value);

// Имя файла инициализации
const string& GetOptionsFileName(void) const;
bool SetOptionsFileName(const string& value);
// --------------------------


// --------------------------
// Методы доступа к переменным состояния
// --------------------------
// Флаг работы системы
int IsRunned(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает указатель на среду
virtual UAContainerEnvironment* GetEnvironment(void);
// --------------------------

// --------------------------
// Методы управления движком
// Методы пишут в лог по необходимости
// --------------------------
// Инициализирует данные движка
virtual bool Init(void);

// Деинициализирует данные движка
// и сохраняет текущие настройки
virtual bool UnInit(void);

// Запускает систему
virtual bool Start(void);

// Приостанавливает систему
virtual bool Pause(void);

// Останавливает систему
virtual bool Stop(void);
// --------------------------

// --------------------------
// Методы управления средой
// --------------------------
// Методы управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
virtual int Storage_GetNumClasses(void);

// Возвращает id классов в хранилище. Память должна быть выделена
virtual void Storage_GetClassesList(int *buffer) const;

// Возвращает имя класса по его id.
virtual const char * Storage_GetClassName(int id) const;

// Возвращает Id класса по его имени
virtual int Storage_GetClassId(const char *name) const;

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
virtual bool Storage_DelClass(int classid);

// Удалаяет все свободные объекты из хранилища
virtual void Storage_FreeObjectsStorage(void);

// Удаляет все объекты из хранилища
virtual void Storage_ClearObjectsStorage(void);

// Вычисляет суммарное число объектов в хранилище
virtual int Storage_CalcNumObjects(void) const;
virtual int Storage_CalcNumObjectsById(int classid) const;
virtual int Storage_CalcNumObjectsByName(const char* classname) const;

// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
virtual int Env_GetPredefinedStructure(void) const;
virtual bool Env_SetPredefinedStructure(int value);

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
virtual bool Env_IsStoragePresent(void) const;

// Возвращает состояние инициализации
virtual bool Env_IsInit(void) const;

// Признак наличия сформированной структуры
virtual bool Env_IsStructured(void) const;

// Инициализация среды
virtual bool Env_Init(void);

// Деинициализация среды
virtual bool Env_UnInit(void);

// Формирует предварительно заданную модель обработки
virtual bool Env_CreateStructure(void);

// Уничтожает текущую модель обработки
virtual bool Env_DestroyStructure(void);

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
virtual void Env_Destroy(void);

// Загружает библиотеку по имени dll-файла
virtual int Env_LoadStorageLibrary(const char *filename);

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool Env_DelClassLibraryByIndex(int index);

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool Env_DelClassLibraryByName(const char *name);

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool Env_DelAllClassLibraries(void);

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
virtual bool Env_BuildStorage(void);

// Возвращает число библиотек
virtual int Env_GetNumClassLibraries(void) const;

// Возвращает имя библиотеки по индексу
virtual const char * Env_GetClassLibraryName(int index);

// Возвращает версию библиотеки по индексу
virtual const char * Env_GetClassLibraryVersion(int index);

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
// Возвращает id нового класса в хранилище
virtual int Env_CreateClass(const char* stringid);

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
virtual int Env_Calculate(const char* stringid=0);

// Методы управления моделью
// ----------------------------
// Удаляет модель
virtual int Model_Destroy(void);

// Создает новую модель по id класса в хранилище
// Предварительно удаляет существующую модель
virtual int Model_Create(int classid);

// Очищает модель
virtual int Model_Clear(void);

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр контейнера с заданным 'classid'
// если stringid - пустая строка, то добавляет в саму модель
virtual int Model_AddComponent(char* stringid, int classid);

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр контейнера с заданным 'id'
// если stringid - пустая строка, то удаляет из самой модели
virtual int Model_DelComponent(char* stringid, int id);

// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
virtual int Model_GetNumComponents(char* stringid);

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
virtual int Model_GetComponentsList(char* stringid, int *buffer);

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
virtual const char* Model_GetComponentName(char* stringid);

// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
virtual const char* Model_GetComponentParameters(const char *stringid);

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
virtual const char* Model_GetComponentSelectedParameters(const char *stringid);

// устанавливает параметры компонента по идентификатору
virtual bool Model_SetComponentParameters(const char *stringid, const char* buffer);

// Связывает выбранные контейнеры друг с другом
virtual int Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number);

// Разрывает выбранную связь
virtual int Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number);

// Разрывает все связи
virtual int Model_BreakAllLinks(void);

// Разрывает все входные и выходные связи выбранного контейнера
virtual int Model_BreakAllComponentLinks(char* stringid);

// Разрывает все входные связи выбранного контейнера
virtual int Model_BreakAllComponentInputLinks(char* stringid);

// Разрывает все выходные связи выбранного контейнера
virtual int Model_BreakAllComponentOutputLinks(char* stringid);

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
virtual const char* Model_GetComponentInternalLinks(char* stringid);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
virtual int Model_SetComponentInternalLinks(char* stringid,const char* buffer);

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
virtual const char* Model_GetComponentInputLinks(char* stringid);

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
virtual const char* Model_GetComponentOutputLinks(char* stringid);

// Возвращает состояние компонента по идентификатору
virtual const char* Model_GetComponentState(const char *stringid);

// Возвращает выборочные данные состояния компонента по идентификатору
virtual const char* Model_GetComponentSelectedState(const char *stringid);

// Устанавливает состояние компонента по идентификатору
virtual bool Model_SetComponentState(const char *stringid, const char* buffer);

// Возвращает число входов у компонента
virtual int Model_GetComponentNumInputs(const char *stringid);

// Возвращает размер входа компонента в числе элементов
virtual int Model_GetComponentInputSize(const char *stringid, int index);

// Возвращает размер элемента входа в байтах
virtual int Model_GetComponentInputElementSize(const char *stringid, int index);

// Возвращает размер входа компонента в байтах элементов
virtual int Model_GetComponentInputByteSize(const char *stringid, int index);

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
virtual unsigned char* Model_GetComponentInputData(const char *stringid, int index);

// Возвращает число выходов у компонента
virtual int Model_GetComponentNumOutputs(const char *stringid);

// Возвращает размер выхода компонента в числе элементов
virtual int Model_GetComponentOutputSize(const char *stringid, int index);

// Возвращает размер элемента выхода в байтах
virtual int Model_GetComponentOutputElementSize(const char *stringid, int index);

// Возвращает размер выхода компонента в байтах элементов
virtual int Model_GetComponentOutputByteSize(const char *stringid, int index);

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
virtual unsigned char* Model_GetComponentOutputData(const char *stringid, int index);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
virtual const char* Model_SaveComponent(const char *stringid);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
virtual int Model_LoadComponent(const char *stringid, char* buffer);

// Сохраняет все параметры компонента и его дочерних компонент в xml
virtual const char* Model_SaveComponentParameters(const char *stringid);

// Загружает все параметры компонента и его дочерних компонент из xml
virtual int Model_LoadComponentParameters(const char *stringid, char* buffer);

// Сохраняет состояние компонента и его дочерних компонент в xml
virtual const char* Model_SaveComponentState(const char *stringid);

// Загружает состояние компонента и его дочерних компонент из xml
virtual int Model_LoadComponentState(const char *stringid, char* buffer);
// --------------------------

// --------------------------
// Методы управления движком
// --------------------------
public:
// Загружает набор предустановленных библиотек
virtual int LoadPredefinedLibraries(void);

// Загружает набор классов
int LoadClasses(void);

// Загружает набор библиотек
int LoadLibraries(void);
// --------------------------

// --------------------------
// Скрытые методы управления средой
// --------------------------
protected:
// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
virtual bool Model_GetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
virtual bool Model_GetComponentSelectedParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// устанавливает параметры компонента по идентификатору
virtual bool Model_SetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
virtual int Model_GetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
virtual int Model_SetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
virtual int Model_GetComponentInputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
virtual int Model_GetComponentOutputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Возвращает состояние компонента по идентификатору
// Память для buffer должна быть выделена!
virtual bool Model_GetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
virtual bool Model_GetComponentSelectedState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// Устанавливает состояние компонента по идентификатору
virtual bool Model_SetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
virtual int Model_SaveComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
virtual int Model_LoadComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Сохраняет все параметры компонента и его дочерних компонент в xml
virtual int Model_SaveComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Загружает все параметры компонента и его дочерних компонент из xml
virtual int Model_LoadComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Сохраняет состояние компонента и его дочерних компонент в xml
virtual int Model_SaveComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Загружает состояние компонента и его дочерних компонент из xml
virtual int Model_LoadComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);
// --------------------------

// --------------------------
// Методы внутреннего управления консолью
// --------------------------
protected:
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Осуществляет поиск компонента по длинному строковому id
// Если строковое id не задано, то возвращает указатель на модель
UAContainer* FindComponent(const char *stringid);

// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета.
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

// Инициализация dll
typedef bool(*DLLPDllInit)(void* pfstorage,void* pfenvironment,void* pfengine);
extern DLLPDllInit DLLDllInit;

// Указатель на функцию возвращающую число хранилищ в библиотеке
typedef std::size_t(*DLLPGetNumStorages)(void);
extern DLLPGetNumStorages DLLGetNumStorages;

// Указатель на функцию возвращающую число сред в библиотеке
typedef std::size_t(*DLLPGetNumEnvironments)(void);
extern DLLPGetNumEnvironments DLLGetNumEnvironments;

// Указатель на функцию возвращающую число движков в библиотеке
typedef std::size_t(*DLLPGetNumEngines)(void);
extern DLLPGetNumEngines DLLGetNumEngines;

// Возвращает хранилище по индексу
typedef UAContainerStorage* (*DLLPGetStorage)(size_t i);
extern DLLPGetStorage DLLGetStorage;

// Возвращает среду по индексу
typedef UAContainerEnvironment* (*DLLPGetEnvironment)(size_t i);
extern DLLPGetEnvironment DLLGetEnvironment;

// Возвращает движок по индексу
typedef UEngine* (*DLLPGetEngine)(size_t i);
extern DLLPGetEngine DLLGetEngine;

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
typedef UAContainerStorage* (*DLLPAddNewStorage)(void);
extern DLLPAddNewStorage DLLAddNewStorage;

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
typedef UAContainerEnvironment* (*DLLPAddNewEnvironment)(UAContainerStorage *storage,bool isinit,list<UAContainer*>* external_classes, list<UALibrary*>* external_libs);
extern DLLPAddNewEnvironment DLLAddNewEnvironment;

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
typedef UEngine* (*DLLPAddNewEngine)(void);
extern DLLPAddNewEngine DLLAddNewEngine;


}
#endif
