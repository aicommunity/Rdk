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
//typedef RDK::UAContainerStorage* (*PCreateNewStorage)(void);

// Создает новую среду
//typedef RDK::UAContainerEnvironment* (*PCreateNewEnvironment)(void);

// Прототип функции обратного вызова обработчика исключений
typedef void (*PExceptionHandler)(void);

protected: // Параметры инициализации
// Указатели на функции создания экземпляров хранилища и среды
//PCreateNewStorage FuncCreateNewStorage;
//PCreateNewEnvironment FuncCreateNewEnvironment;

// Имя файла инициализации
string OptionsFileName;

// Имя файла описаний параметров классов
string ClassesDescriptionFileName;

// Имя секции выбора основной библиотеки
//string MainLibrarySectionName;

// Имя переменной основной библиотеки
//string MainLibraryName;

// Имя секции выбора библиотек компонент
string ComponentLibrariesSectionName;

// Имя секции выбора классов компонент
string ComponentClassesSectionName;

// Индекс используемого хранилища в библиотеке
// Если < 0, то новое хранилище будет создано
//int StorageIndex;

// Индекс используемой среды в библиотеке
// Если < 0, то новая среда будет создана
//int EnvironmentIndex;


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
RDK::UEPtr<UAContainerStorage> Storage;

// Среда
RDK::UEPtr<UAContainerEnvironment> Environment;

protected: // Обработка исключений
// Лог исключений системы
//mutable vector<USharedPtr<UException> > ExceptionsLog;

// Текущее число исключений системы
mutable int CurrentExceptionsLogSize;

// Максимальное число хранимых исключений
// Если 0, то неограниченно
int MaxExceptionsLogSize;

// Внешний обработчик исключений
PExceptionHandler ExceptionHandler;

protected: // Временные переменные
// Список загруженных библиотек
std::list<UALibrary*> LibrariesList;

// Список загруженных образцов классов
std::list<UAContainer*> ClassesList;

RDK::Serialize::USerStorageXML XmlStorage;

// Временное хранилище строк
mutable string TempString;

// Временное хранилище буфера для лога
mutable string TempLogString;

// Временное хранилище имени компонента
mutable string CompName;

// Индекс последнего считанного символа лога исключений
int LastReadExceptionLogIndex;

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
/*UEngine::PCreateNewStorage GetFuncCreateNewStorage(void) const;
bool SetFuncCreateNewStorage(UEngine::PCreateNewStorage value);

UEngine::PCreateNewEnvironment GetFuncCreateNewEnvironment(void) const;
bool SetFuncCreateNewEnvironment(UEngine::PCreateNewEnvironment value);
  */
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

// Возвращает указатель на модель
virtual UAContainer* GetModel(void);
// --------------------------

// --------------------------
// Методы управления движком
// Методы пишут в лог по необходимости
// --------------------------
// Инициализирует данные движка
virtual void Init(void);
virtual bool Init(UEPtr<UAContainerStorage> storage, UEPtr<UAContainerEnvironment> env);

// Деинициализирует данные движка
// и сохраняет текущие настройки
virtual void UnInit(void);

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

// Возвращает описание класса по его id в формате xml
virtual const char* Storage_GetClassDescription(int classid);

// Устанавливает описание класса по его id, считывая его из формата xml
virtual bool Storage_SetClassDescription(int classid, const char* description);

// Сохраняет описание всех классов в xml
virtual const char* Storage_SaveClassesDescription(void);

// Загружает описание всех классов из xml
virtual bool Storage_LoadClassesDescription(const char* xmltext);

// Сохраняет общее описание всех классов в xml
virtual const char* Storage_SaveCommonClassesDescription(void);

// Загружает общее описание всех классов из xml
virtual bool Storage_LoadCommonClassesDescription(const char* xmltext);

// Сохраняет описание всех классов в xml включая общее описание
virtual const char* Storage_SaveAllClassesDescription(void);

// Загружает описание всех классов из xml включая общее описание
virtual bool Storage_LoadAllClassesDescription(const char* xmltext);

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

// Расчет всей модели в реальном времени
virtual void Env_RTCalculate(void);

// Метод сброса счета
// Если stringid == 0 то сбрасывает всю модель целиком,
// иначе - только указанный компонент модели
virtual int Env_Reset(const char* stringid=0);

// Производит увеличение времени модели на требуемую величину
virtual void Env_IncreaseModelTimeByStep(void);

// !!! Следующие методы управления текущим компонентом влияют на все
// методы, обращающиеся к компонентам по строковому id !!!
// Устанавливает текущий компонент (адресация относительно корня - модели)
virtual int Env_SelectCurrentComponent(const char *stringid);

// Сбрасывает текущий компонент в состояние по умолчению (модель)
virtual int Env_ResetCurrentComponent(const char *stringid);

// Меняет текущий компонент на его родителя (подъем на уровень вверх)
// Если уже на верхнем уровне, то не делает ничего
virtual int Env_UpCurrentComponent(void);

// Меняет текущий компонент на его дочерний на произвольном уровне вложенности
// (спуск на N уровней вниз относительно текущего компонента)
virtual int Env_DownCurrentComponent(const char *stringid);

// Возвращает длинное имя текущего компонента
virtual const char* Env_GetCurrentComponentName(void) const;

// Возвращает длинный строковой id текущего компонента
virtual const char* Env_GetCurrentComponentId(void) const;

// Методы управления моделью
// ----------------------------
// Удаляет модель
virtual int Model_Destroy(void);

// Создает новую модель по id класса в хранилище
// Предварительно удаляет существующую модель
virtual int Model_Create(const char *classid);

// Очищает модель
virtual int Model_Clear(void);

// Проверяет, существует ли модель
virtual bool Model_Check(void);

// Проверяет, существует ли в модели компонент с именем stringid)
virtual bool Model_CheckComponent(const char* stringid) const;

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр контейнера с заданным 'classid'
// если stringid - пустая строка, то добавляет в саму модель
// Возвращает длинное имя компонента в случае успеха
virtual const char*  Model_AddComponent(const char* stringid, int classid);

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр контейнера с заданным 'id'
// если stringid - пустая строка, то удаляет из самой модели
virtual int Model_DelComponent(const char* stringid, int id);

// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
virtual int Model_GetNumComponents(const char* stringid);

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
virtual int Model_GetComponentsList(const char* stringid, int *buffer);

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ','
virtual const char* Model_GetComponentsNameList(const char* stringid);

// Возвращает xml-список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
virtual const char* Model_GetConnectorsList(const char* stringid,
						  int sublevel=-1, const char* owner_level_stringid=0);

// Возвращает xml-список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
virtual const char* Model_GetItemsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// Возвращает xml-список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
virtual const char* Model_GetNetsList(const char* stringid,
							int sublevel=-1, const char* owner_level_stringid=0);

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
virtual const char* Model_GetComponentName(const char* stringid);

// Возвращает длинное имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
virtual const char* Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid=0);

// Возвращает длинный id компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
virtual const char* Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid=0);

// Возвращает имя класса компонента в хранилище по длинному 'stringid'
// если stringid - пустая строка, то возвращает имя класса модели
virtual const char* Model_GetComponentClassName(const char* stringid);

// Возвращает свойства компонента по идентификатору
// Память для buffer должна быть выделена!
virtual const char* Model_GetComponentProperties(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// Возвращает выборочные свойства компонента по идентификатору
// Память для buffer должна быть выделена!
virtual const char* Model_GetComponentSelectedProperties(const char *stringid);

// Возвращает свойства компонента по идентификатору с описаниями
// Память для buffer должна быть выделена!
virtual const char* Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// Возвращает значение свойства компонента по идентификатору компонента и имени свойства
virtual const char * Model_GetComponentPropertyValue(const char *stringid, const char *paramname);

// Устанавливает свойства компонента по идентификатору
virtual int Model_SetComponentProperties(const char *stringid, const char* buffer);

// Устанавливает значение свойства компонента по идентификатору компонента и имени свойства
virtual void Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
virtual void Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
virtual void Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer);

// Связывает выбранные контейнеры друг с другом
virtual int Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// Связывает все компоненты выбранного компонента по возрастанию id в формате: 0 выход к 0 входу
virtual int Model_ChainLinking(const char* stringid);

// Связывает все компоненты выбранного компонента параллельно, подключая их к необходимому числу выходов модели
// Используется для тестирования производительности
virtual int Model_ParallelLinking(const char* stringid);

// Разрывает выбранную связь
virtual int Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// Разрывает все связи
virtual int Model_BreakAllLinks(void);

// Разрывает все входные и выходные связи выбранного контейнера
virtual int Model_BreakAllComponentLinks(const char* stringid);

// Разрывает все входные связи выбранного контейнера
virtual int Model_BreakAllComponentInputLinks(const char* stringid);

// Разрывает все выходные связи выбранного контейнера
virtual int Model_BreakAllComponentOutputLinks(const char* stringid);

// Проверяет, существует ли заданна связь
virtual bool Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number);

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
virtual const char* Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid=0);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются с уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то применяется уровень текущего компонента
virtual int Model_SetComponentInternalLinks(const char* stringid,const char* buffer, const char* owner_level_stringid=0);

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
virtual const char* Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
virtual const char* Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid=0, int sublevel=-1);

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
virtual const char* Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid=0);

// Возвращает число входов у компонента
virtual int Model_GetComponentNumInputs(const char *stringid);

// Возвращает размер входа компонента в числе элементов
virtual int Model_GetComponentInputDataSize(const char *stringid, int index);

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
virtual int Model_GetComponentOutputDataSize(const char *stringid, int index);

// Возвращает размер элемента выхода в байтах
virtual int Model_GetComponentOutputElementSize(const char *stringid, int index);

// Возвращает размер выхода компонента в байтах элементов
virtual int Model_GetComponentOutputByteSize(const char *stringid, int index);

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
virtual unsigned char* Model_GetComponentOutputData(const char *stringid, int index);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
virtual const char* Model_SaveComponent(const char *stringid, unsigned int params_type_mask=0xFFFFFFFF);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
virtual int Model_LoadComponent(const char *stringid, char* buffer);

// Сохраняет все свойства компонента и его дочерних компонент в xml
virtual const char* Model_SaveComponentProperties(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// Загружает все свойства компонента и его дочерних компонент из xml
virtual int Model_LoadComponentProperties(const char *stringid, char* buffer);

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
virtual const char* Model_SaveComponentDrawInfo(const char *stringid);

// Управляет шагом счета модели по умолчанию
virtual int Model_GetDefaultTimeStep(void) const;
virtual void Model_SetDefaultTimeStep(int value);

// Управляет шагом счета компонента
virtual int Model_GetTimeStep(const char *stringid) const;
virtual void Model_SetTimeStep(const char *stringid, int value);

// Устанавливает шаг счета компонента и всех его дочерних компонент
virtual void Model_SetGlobalTimeStep(const char *stringid, int value);

// Возвращает текущее время модели
virtual long long Model_GetTime(void);
virtual double Model_GetDoubleTime(void);

// Устанавливает текущее время модели
virtual bool Model_SetTime(long long value);

// Возвращает реальное время
virtual long long Model_GetRealTime(void);
virtual double Model_GetDoubleRealTime(void);

// Устанавливает реальное время
virtual bool Model_SetRealTime(long long value);

// Увеличивает реальное время на заданную величину
virtual bool Model_IncreaseRealTime(long long value);

// Возвращает мгновенный шаг в реальном времени
virtual long long Model_GetRealTimeStep(void);
virtual double Model_GetDoubleRealTimeStep(void);

// Возвращает время расчета компонента без времени расчета дочерних компонент (мс)
long long Model_GetStepDuration(const char *stringid) const;

// Возвращает время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
long long Model_GetFullStepDuration(const char *stringid) const;

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double Model_GetInstantPerformance(const char *stringid) const;

// Время, прошедшее между двумя последними итерациями счета
long long Model_GetInterstepsInterval(const char *stringid) const;
// --------------------------

// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::MDMatrix*
// если выход не содержит данных такого типа, то возвращает 0
const /* RDK::MDMatrix* */void* const Model_GetComponentOutputAsMatrix(const char *stringid, int index);

// --------------------------
// Методы управления исключениями
// --------------------------
// Обрабатывает возникшее исключение
virtual void ProcessException(UException &exception) const;

// Формирует строку лога об исключении
//virtual string CreateLogMessage(UException &exception) const;

// Возвращает массив зарегистрированных исключений
//const vector<USharedPtr<UException> > GetExceptionsLog(void) const;

// Максимальное число хранимых исключений
// Если 0, то неограниченно
int GetMaxExceptionsLogSize(void) const;
void SetMaxExceptionsLogSize(int value);

// Возвращает массив строк лога
const char* GetLog(void) const;

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* GetUnreadLog(void);

// Управление функцией-обработчиком исключений
PExceptionHandler GetExceptionHandler(void) const;
bool SetExceptionHandler(PExceptionHandler value);
// --------------------------

// --------------------------
// Методы управления движком
// --------------------------
public:
// Инициализирует пустое хранилище
void CreateStorage(void);

// Инициализирует среду
// Если задано хранилище 'storage', то связывает его со средой.
// Если флаг 'isinit' == true, то инициализирует хранилище стандартными библиотеками
// Если указатель на массив external_libs != 0, дополнительно инициализирует хранилище этими бибилиотеками
void CreateEnvironment(bool isinit=true, list<UAContainer*>* external_classes=0, list<UALibrary*>* external_libs=0);

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
// Возвращает свойства компонента по идентификатору
virtual bool Model_GetComponentProperties(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage, unsigned int type_mask);

// Возвращает выборочные свойства компонента по идентификатору
virtual bool Model_GetComponentSelectedProperties(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// Возвращает свойства компонента по идентификатору с описаниями
virtual bool Model_GetComponentPropertiesEx(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage, unsigned int type_mask);

// Устанавливает свойства компонента по идентификатору
virtual int Model_SetComponentProperties(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
virtual void Model_SetGlobalComponentPropertyValue(RDK::UAContainer* cont, UId classid, const char *paramname, const char *buffer);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
virtual void Model_SetGlobalOwnerComponentPropertyValue(RDK::UAContainer* cont, UId classid, UId owner_classid, const char *paramname, const char *buffer);

// Возращает все связи внутри компонента cont в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int Model_GetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются до уровня компонента owner_level
// Если owner_level не задан, то имена применяются до уровня текущего компонента
virtual int Model_SetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level);

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int Model_GetComponentInputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level, int sublevel=-1);

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int Model_GetComponentOutputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level, int sublevel=-1);

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int Model_GetComponentPersonalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, RDK::UANet* owner_level);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
virtual int Model_SaveComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, bool links, unsigned int params_type_mask);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
virtual int Model_LoadComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, bool links);

// Сохраняет все свойства компонента и его дочерних компонент в xml
virtual int Model_SaveComponentProperties(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage, unsigned int type_mask);

// Загружает все свойства компонента и его дочерних компонент из xml
virtual int Model_LoadComponentProperties(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
virtual int Model_SaveComponentDrawInfo(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage);
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
UEPtr<UAContainer> FindComponent(const char *stringid) const;

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
	  /*
#ifndef RDK_CALL
#define RDK_CALL _cdecl
#endif

// Инициализация dll
typedef bool (RDK_CALL *DLLPDllInit)(void* pfstorage,void* pfenvironment,void* pfengine);
extern "C" DLLPDllInit DLLDllInit;

// Указатель на функцию возвращающую число хранилищ в библиотеке
typedef int (RDK_CALL *DLLPGetNumStorages)(void);
extern "C" DLLPGetNumStorages DLLGetNumStorages;

// Указатель на функцию возвращающую число сред в библиотеке
typedef int (RDK_CALL *DLLPGetNumEnvironments)(void);
extern "C" DLLPGetNumEnvironments DLLGetNumEnvironments;

// Указатель на функцию возвращающую число движков в библиотеке
typedef int (RDK_CALL *DLLPGetNumEngines)(void);
extern "C" DLLPGetNumEngines DLLGetNumEngines;

// Возвращает хранилище по индексу
typedef UAContainerStorage* (RDK_CALL *DLLPGetStorage)(size_t i);
extern "C" DLLPGetStorage DLLGetStorage;

// Возвращает среду по индексу
typedef UAContainerEnvironment* (RDK_CALL *DLLPGetEnvironment)(size_t i);
extern "C" DLLPGetEnvironment DLLGetEnvironment;

// Возвращает движок по индексу
typedef UEngine* (RDK_CALL *DLLPGetEngine)(size_t i);
extern "C" DLLPGetEngine DLLGetEngine;

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
typedef UAContainerStorage* (RDK_CALL *DLLPAddNewStorage)(void);
extern "C" DLLPAddNewStorage DLLAddNewStorage;

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
typedef UAContainerEnvironment* (RDK_CALL *DLLPAddNewEnvironment)(UAContainerStorage *storage,bool isinit,list<UAContainer*>* external_classes, list<UALibrary*>* external_libs);
extern "C" DLLPAddNewEnvironment DLLAddNewEnvironment;

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
typedef UEngine* (RDK_CALL *DLLPAddNewEngine)(void);
extern "C" DLLPAddNewEngine DLLAddNewEngine;
          */

}
#endif
