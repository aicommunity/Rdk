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
#include "UEnvironment.h"
#include "UStorage.h"
#include "../Utilities/UIniFile.h"
#include "UNet.h"

namespace RDK{

//using namespace RDK;


class RDK_LIB_TYPE UEngine: public UModule
{
public:
// Прототип функции обратного вызова обработчика исключений
//typedef void (*PExceptionHandler)(void);

protected: // Параметры инициализации
// Имя файла инициализации
string OptionsFileName;

// Имя файла описаний параметров классов
string ClassesDescriptionFileName;

// Имя секции выбора библиотек компонент
string ComponentLibrariesSectionName;

// Имя секции выбора классов компонент
string ComponentClassesSectionName;

/// Режим создания внутренних временных переменных для
/// возвращаемых значений
/// 0 - одна переменная для всех методов, возвращающих такой тип
/// 1 - уникальные переменные с необходимостью вызвова функции очистки
int BufObjectsMode;

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

/// Индекс текущего канала в многоканальной библиотеке
int ChannelIndex;

protected: // Данные
// Данные инициализации
UIniFile<char> Options;

// Хранилище
RDK::UEPtr<UStorage> Storage;

// Среда
RDK::UEPtr<UEnvironment> Environment;

protected: // Обработка исключений
// Лог исключений системы
//mutable vector<USharedPtr<UException> > ExceptionsLog;

// Текущее число исключений системы
//mutable int CurrentExceptionsLogSize;

// Максимальное число хранимых исключений
// Если 0, то неограниченно
//int MaxExceptionsLogSize;

// Внешний обработчик исключений
//PExceptionHandler ExceptionHandler;

protected: // Временные переменные
// Список загруженных библиотек
std::list<ULibrary*> LibrariesList;

// Список загруженных образцов классов
std::list<UContainer*> ClassesList;

RDK::USerStorageXML XmlStorage;

// Временное хранилище строк
mutable std::list<UEPtr<string> > TempStrings;
mutable string DummyTempString;

// Временное хранилище буфера для лога
//mutable string TempLogString;

// Временное хранилище имени компонента
mutable string CompName;

// Индекс последнего считанного символа лога исключений
//int LastReadExceptionLogIndex;

UBitmap TempBmp;

public:
// Временное хранилище строк
//mutable string PubTempString;

/// Кеш быстрого доступа к компонентам
mutable std::map<std::string, UEPtr<UContainer> > AccessCache;



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
/// Режим создания внутренних временных переменных для
/// возвращаемых значений
/// 0 - одна переменная для всех методов, возвращающих такой тип
/// 1 - уникальные переменные с необходимостью вызвова функции очистки
int GetBufObjectsMode(void) const;
bool SetBufObjectsMode(int mode);

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
// Методы управления временными переменными
// --------------------------
/// Создает в списке временных строку новую строку
/// и возвращает ссылку на нее
std::string& CreateTempString(void) const;

/// Возвращает временную строку
/// по указателю на ее данные
std::string& FindTempString(const char *str_data) const;

/// Удаляет временную строку
/// по указателю на ее данные
void DestroyTempString(const char *str_data) const;

/// Удаляет временную строку
/// по ссылке на нее
void DestroyTempString(const std::string &ref) const;

/// Удаляет все временные строк
void ClearAllTempStrings(void) const;

/// Возвращает число временных строк
int GetNumTempStrings(void) const;
// --------------------------

// --------------------------
// Методы доступа к переменным состояния
// --------------------------
// Флаг работы системы
int IsRunned(void);

/// Индекс текущего канала в многоканальной библиотеке
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает указатель на среду
virtual UEnvironment* GetEnvironment(void);

// Возвращает указатель на модель
virtual UContainer* GetModel(void);
// --------------------------

// --------------------------
// Методы управления движком
// Методы пишут в лог по необходимости
// --------------------------
// Инициализирует данные движка
virtual void Init(void);
virtual bool Init(UEPtr<UStorage> storage, UEPtr<UEnvironment> env);

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

// Возвращает имена классов в хранилище в виде строки, разделенной запятыми
virtual const char* Storage_GetClassesNameList(void) const;

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
virtual const char* Storage_GetClassDescription(const char* classname);

// Устанавливает описание класса по его id, считывая его из формата xml
virtual bool Storage_SetClassDescription(const char* classname, const char* description);

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
// ----------------------------

// ----------------------------
// Методы управления коллекциями компонент
// ----------------------------
// Возвращает число библиотек
virtual int Storage_GetNumClassLibraries(void) const;

// Возвращает список библиотек в виде строки, разделенной запятыми
virtual const char * Storage_GetClassLibrariesList(void) const;

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// library_name - имя библиотеки
virtual const char * Storage_GetLibraryClassNames(const char *library_name) const;

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// index - индекс библиотеки
virtual const char * Storage_GetLibraryClassNamesByIndex(int index) const;

// Возвращает имя библиотеки по индексу
virtual const char * Storage_GetClassLibraryNameByIndex(int index);

// Возвращает версию библиотеки по индексу
virtual const char * Storage_GetClassLibraryVersionByIndex(int index);

// Загружает коллекцию по имени dll-файла
virtual int Storage_LoadBinaryCollectionFromFile(const char *filename);

// Загружает runtime-коллекцию
virtual int Storage_LoadRuntimeCollectionFromFile(const char *filename);
virtual int Storage_LoadRuntimeCollectionFromString(const char *buffer);

// Сохраняет runtime-коллекцию
virtual int Storage_SaveRuntimeCollectionToFile(const char *filename);
virtual int Storage_SaveRuntimeCollectionToString(const char *buffer);

/// Создает новую runtime-библиотеку
virtual int Storage_CreateRuntimeCollection(const char *collection_name);

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
virtual int Storage_CreateClass(const char* stringid, const char *class_name, const char *collection_name);

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool Storage_DelClassLibraryByIndex(int index);

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool Storage_DelClassLibraryByName(const char *name);

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool Storage_DelAllClassLibraries(void);


// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
virtual bool Storage_BuildStorage(void);
// ----------------------------

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

/// Метод сброса параметров на значения по умолчанию
/// Если stringid == 0 то сбрасывает всю модель целиком,
/// иначе - только указанный компонент модели
/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
virtual int Env_Default(const char* stringid, bool subcomps=false);

// Производит увеличение времени модели на требуемую величину
virtual void Env_IncreaseModelTimeByStep(void);

/// Устанавливает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
virtual int Env_SetMinInterstepsInterval(unsigned long long value);

/// Возвращает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
virtual unsigned long long Env_GetMinInterstepsInterval(void) const;

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

// Возвращает имя текущего каталога для хранения данных
virtual const char* Env_GetCurrentDataDir(void) const;

// Устанавливает имя текущего каталога для хранения данных
virtual int Env_SetCurrentDataDir(const char *dir);

// Возвращает имя каталога бинарных файлов
virtual const char* Env_GetSystemDir(void) const;

// Устанавливает имя каталога бинарных файлов
virtual int Env_SetSystemDir(const char *dir);

/// Возвращает состояние флага отладочного режима среды
virtual int Env_GetDebugMode(void) const;

/// Устанавливает состояние флага отладочного режима среды
virtual int Env_SetDebugMode(bool value);

// Перенесено из UBEngine
// Задает число входов среды
virtual void Env_SetNumInputImages(int number);

// Задает число выходов среды
virtual void Env_SetNumOutputImages(int number);

// Возвращает число входов среды
virtual int Env_GetNumInputImages(void);

// Возвращает число выходов среды
virtual int Env_GetNumOutputImages(void);

// Задает разрешение по умолчанию (рабочее разрешение)
virtual void Env_SetInputRes(int number, int width, int height);

// Задает флаг отражения входного изображения вокруг горизонтальной оси
virtual void Env_SetReflectionXFlag(bool value);

// Возвращает разрешение по умолчанию (рабочее разрешение)
virtual int Env_GetInputImageWidth(int number);
virtual int Env_GetInputImageHeight(int number);
virtual int Env_GetInputImageColorModel(int number);

// Возвращает текущее выходное разрешение
virtual int Env_GetOutputImageWidth(int number);
virtual int Env_GetOutputImageHeight(int number);
virtual int Env_GetOutputImageColorModel(int number);

virtual void Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

virtual unsigned char* Env_GetInputImage(int index);

virtual unsigned char* Env_GetOutputImage(int index);

virtual unsigned char* Env_GetOutputImageY8(int index);

/// Инициирует извещение о сбое в работе источника данных
virtual bool Env_CallSourceController(void);

// Методы управления моделью
// ----------------------------
// Удаляет модель
virtual int Model_Destroy(void);

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
virtual int Model_Create(const char *classname);

// Очищает модель
virtual int Model_Clear(void);

// Проверяет, существует ли модель
virtual bool Model_Check(void);

// Проверяет, существует ли в модели компонент с именем stringid)
virtual bool Model_CheckComponent(const char* stringid) const;

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'classname'
// если stringid - пустая строка, то добавляет в саму модель
// Возвращает длинное имя компонента в случае успеха
virtual const char*  Model_AddComponent(const char* stringid, const char *classname);

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'name'
// если stringid - пустая строка, то удаляет из самой модели
virtual int Model_DelComponent(const char* stringid, const char *name);

/// Перемещает компоненту в другой компонент
/// Если comp не принадлежит этому компоненту, или target имеет отличный от
/// этого компонента storage, или target не может принять в себя компонент
/// то возвращает false и не делает ничего
virtual int Model_MoveComponent(const char* component, const char* target);

// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
virtual int Model_GetNumComponents(const char* stringid);

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
virtual int Model_GetComponentsList(const char* stringid, int *buffer);

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ','
virtual const char* Model_GetComponentsNameList(const char* stringid);

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ',' и имеющих имя класса 'class_name'
// Если find_all == true то поиск ведется и во всех сабкомпонентах
//virtual const char* Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all=false);
virtual const char* Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all=false);

// Перемещает компонент с текущим индексом index или именем 'name' вверх или
// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
// на эту границу
virtual int Model_ChangeComponentPosition(const char* stringid, int step);

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

// Возвращает список свойств компонента разделенный запятыми
virtual const char* Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask);

// Возвращает список имен и индексов свойств компонента разделенный запятыми
// каждый элемент имеет вид имя_свойства:индекс_входа(выхода)
virtual const char* Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask);

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

// Возвращает указатель на данные свойства компонента
template<class T>
const T& Model_GetComponentPropertyData(const char *stringid, const char *property_name);

// Возвращает указатель void* на данные свойства компонента
const void* Model_GetComponentPropertyData(const char *stringid, const char *property_name);

// Копирует данные 'data' в заданное свойство компонента
template<class T>
int Model_SetComponentPropertyData(const char *stringid, const char *property_name, const T &data);

// Копирует данные 'data' в заданное свойство компонента
int Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data);

// Связывает выбранные контейнеры друг с другом
virtual int Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
virtual int Model_CreateLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

// Связывает все компоненты выбранного компонента по возрастанию id в формате: 0 выход к 0 входу
virtual int Model_ChainLinking(const char* stringid);

// Связывает все компоненты выбранного компонента параллельно, подключая их к необходимому числу выходов модели
// Используется для тестирования производительности
virtual int Model_ParallelLinking(const char* stringid);

// Разрывает выбранную связь
virtual int Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
virtual int Model_BreakLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

// Разрывает все связи
virtual int Model_BreakAllLinks(void);

// Разрывает связь ко входу connector_index коннектора 'connectorid'
virtual int Model_BreakConnectorLink(const char* connectorname, int connector_index);

// Разрывает все входные и выходные связи выбранного контейнера
virtual int Model_BreakAllComponentLinks(const char* stringid);

// Разрывает все входные связи выбранного контейнера
virtual int Model_BreakAllComponentInputLinks(const char* stringid);

// Разрывает все выходные связи выбранного контейнера
virtual int Model_BreakAllComponentOutputLinks(const char* stringid);

// Проверяет, существует ли заданна связь
virtual bool Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number);
virtual bool Model_CheckLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);

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

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
virtual int Model_SaveComponentToFile(const char *stringid, const char* file_name, unsigned int params_type_mask=0xFFFFFFFF);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
virtual int Model_LoadComponent(const char *stringid, const char* buffer);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
virtual int Model_LoadComponentFromFile(const char *stringid, const char* file_name);

// Сохраняет все свойства компонента и его дочерних компонент в xml
virtual const char* Model_SaveComponentProperties(const char *stringid, unsigned int type_mask=0xFFFFFFFF);

// Сохраняет все свойства компонента и его дочерних компонент в xml
virtual int Model_SaveComponentPropertiesToFile(const char *stringid, const char* file_name, unsigned int type_mask=0xFFFFFFFF);

// Загружает все свойства компонента и его дочерних компонент из xml
virtual int Model_LoadComponentProperties(const char *stringid, const char* buffer);

// Загружает все свойства компонента и его дочерних компонент из xml
virtual int Model_LoadComponentPropertiesFromFile(const char *stringid, const char* file_name);

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
virtual const char* Model_SaveComponentDrawInfo(const char *stringid);

// Управляет шагом счета модели по умолчанию
virtual unsigned int Model_GetDefaultTimeStep(void) const;
virtual void Model_SetDefaultTimeStep(unsigned int value);

// Управляет шагом счета компонента
virtual unsigned int Model_GetTimeStep(const char *stringid) const;
virtual void Model_SetTimeStep(const char *stringid, unsigned int value);

// Устанавливает шаг счета компонента и всех его дочерних компонент
virtual void Model_SetGlobalTimeStep(const char *stringid, unsigned int value);

// Возвращает текущее время модели
virtual unsigned long long Model_GetTime(void);
virtual double Model_GetDoubleTime(void);

// Устанавливает текущее время модели
virtual bool Model_SetTime(unsigned long long value);

// Возвращает реальное время
virtual unsigned long long Model_GetRealTime(void);
virtual double Model_GetDoubleRealTime(void);

// Устанавливает реальное время
virtual bool Model_SetRealTime(unsigned long long value);

// Увеличивает реальное время на заданную величину
virtual bool Model_IncreaseRealTime(unsigned long long value);

// Возвращает мгновенный шаг в реальном времени
virtual unsigned long long Model_GetRealTimeStep(void);
virtual double Model_GetDoubleRealTimeStep(void);

// Текущее время внешних источников данных в микросекундах
virtual double Model_GetDoubleSourceTime(void) const;

// Устанавливает время внешних источников данных в днях
virtual bool Model_SetDoubleSourceTime(double value);

// Возвращает время расчета компонента без времени расчета дочерних компонент (мс)
unsigned long long Model_GetStepDuration(const char *stringid) const;

// Возвращает время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
unsigned long long Model_GetFullStepDuration(const char *stringid) const;

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double Model_GetInstantPerformance(const char *stringid) const;

// Время, прошедшее между двумя последними итерациями счета
unsigned long long Model_GetInterstepsInterval(const char *stringid) const;
// --------------------------

// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::MDMatrix*
// если выход не содержит данных такого типа, то возвращает 0
const /* RDK::MDMatrix* */void* Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name);
const /* RDK::MDMatrix* */void* Model_GetComponentOutputAsMatrix(const char *stringid, int index);

// Возвращает указатель на выход с индексом 'index' компонента 'id'
virtual const RDK::UBitmap* Model_GetComponentOutput(const char *stringid, const char *property_name);
virtual const RDK::UBitmap* Model_GetComponentOutput(const char *stringid, int index);

// Возвращает указатель на выход с индексом 'index' компонента 'id'
virtual const RDK::UBitmap* Model_GetComponentBitmapOutput(const char *stringid, const char *property_name);
virtual const RDK::UBitmap* Model_GetComponentBitmapOutput(const char *stringid, int index);

/// Копирует данные о разрешении изображения выхода с индексом 'index' компонента 'id'
/// в стрктуру bmp_param
virtual int Model_CopyComponentBitmapOutputHeader(const char *stringid, const char *property_name, RDK::UBitmapParam* bmp_param);
virtual int Model_CopyComponentBitmapOutputHeaderByIndex(const char *stringid, int index, RDK::UBitmapParam* bmp_param);

/// Копирует изображение выхода с индексом 'index' компонента 'id'
/// метод предполагает, что bmp уже имеет выделенную память под изобржение требуемого размера
virtual int Model_CopyComponentBitmapOutput(const char *stringid, const char *property_name, RDK::UBitmap* bmp);
virtual int Model_CopyComponentBitmapOutput(const char *stringid, int index, RDK::UBitmap* bmp);

// Возвращает указатель на вход с индексом 'index' компонента 'id'
virtual const RDK::UBitmap* Model_GetComponentBitmapInput(const char *stringid, const char *property_name);
virtual const RDK::UBitmap* Model_GetComponentBitmapInput(const char *stringid, int index);

// Замещает изображение выхода с индексом 'index' компонента 'id'
virtual void Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const RDK::UBitmap* bmp, bool reflect=false);
virtual void Model_SetComponentBitmapOutput(const char *stringid, int index, const RDK::UBitmap* bmp, bool reflect=false);

// Замещает изображение входа с индексом 'index' компонента 'id'
virtual void Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const RDK::UBitmap* const bmp, bool reflect=false);
virtual void Model_SetComponentBitmapInput(const char *stringid, int index, const RDK::UBitmap* const bmp, bool reflect=false);

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
const char* GetLog(int &error_level) const;

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* GetUnreadLog(int &error_level);

/// Записывает в лог новое сообщение
int Engine_LogMessage(int log_level, const char *message);

// Управление функцией-обработчиком исключений
UEnvironment::PExceptionHandler GetExceptionHandler(void) const;
bool SetExceptionHandler(UEnvironment::PExceptionHandler value);

/// Очищает лог
void ClearLog(void);

/// Возвращает число непрочитанных строк лога
int GetNumUnreadLogLines(void) const;

// Возвращает строку лога с индексом i из частичного массива строк лога с
// момента последнего считывания лога этой функцией
const char* GetUnreadLogLine(int &error_level);

// Помечает строку лога с индексом i из частичного массива строк лога с
// момента последнего считывания как прочитанную
//void MarkUnreadLogLineAsRead(int i);

/// Очищает лог прочитанных сообщений
void ClearReadLog(void);
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
void CreateEnvironment(bool isinit=true, list<UContainer*>* external_classes=0, list<ULibrary*>* external_libs=0);

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
/// Метод сброса параметров на значения по умолчанию
/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
//virtual bool Env_Default(RDK::UContainer* cont, bool subcomps);
     /*
// Возвращает свойства компонента по идентификатору
virtual bool Model_GetComponentProperties(RDK::UContainer* cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// Возвращает выборочные свойства компонента по идентификатору
virtual bool Model_GetComponentSelectedProperties(RDK::UContainer* cont, RDK::USerStorageXML *serstorage);

// Возвращает свойства компонента по идентификатору с описаниями
virtual bool Model_GetComponentPropertiesEx(RDK::UContainer* cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// Устанавливает свойства компонента по идентификатору
virtual int Model_SetComponentProperties(RDK::UContainer* cont, RDK::USerStorageXML *serstorage);
   */
// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
//virtual void Model_SetGlobalComponentPropertyValue(RDK::UContainer* cont, UId classid, const char *paramname, const char *buffer);

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
//virtual void Model_SetGlobalOwnerComponentPropertyValue(RDK::UContainer* cont, UId classid, UId owner_classid, const char *paramname, const char *buffer);
/*
// Возращает все связи внутри компонента cont в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int Model_GetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются до уровня компонента owner_level
// Если owner_level не задан, то имена применяются до уровня текущего компонента
virtual int Model_SetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int Model_GetComponentInputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int Model_GetComponentOutputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
virtual int Model_GetComponentPersonalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
virtual bool Model_SaveComponent(RDK::UNet* cont, RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask);

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
virtual bool Model_LoadComponent(RDK::UNet* cont, RDK::USerStorageXML *serstorage, bool links);

// Сохраняет все свойства компонента и его дочерних компонент в xml
virtual bool Model_SaveComponentProperties(RDK::UNet* cont, RDK::USerStorageXML *serstorage, unsigned int type_mask);

// Загружает все свойства компонента и его дочерних компонент из xml
virtual bool Model_LoadComponentProperties(RDK::UNet* cont, RDK::USerStorageXML *serstorage);
*/
// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
//virtual bool Model_SaveComponentDrawInfo(RDK::UNet* cont, RDK::USerStorageXML *serstorage);
// --------------------------

// --------------------------
// Методы внутреннего управления консолью
// --------------------------
protected:
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
public:
// Осуществляет поиск компонента по длинному строковому id
// Если строковое id не задано, то возвращает указатель на модель
virtual UEPtr<UContainer> FindComponent(const char *stringid) const;

// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

protected:
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

public: // Исключения
// Общая ошибка движка
struct EErrorEngine: public EError
{
protected: // Данные исключения
int EngineIndex;

public:
EErrorEngine(void)//int engine_index)
: EError(), EngineIndex(0) /*EngineIndex(engine_index) */{};
/*
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" EngineIndex=")+sntoa(EngineIndex);
} */
};

// Имя модели не совпадает с ожидаемым
struct EErrorEngineModelNameDontMatch: public EErrorEngine
{
protected: // Данные исключения
/// Полученное имя
std::string GotModelName;

/// Ожидаемое имя
std::string ExpectedModelName;

public:
EErrorEngineModelNameDontMatch(const std::string &got_name, const std::string &expected_name)
: EErrorEngine(), GotModelName(got_name), ExpectedModelName(expected_name) {};

// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const
{
 return EErrorEngine::CreateLogMessage()+std::string(" GotModelName=")+GotModelName+
 				std::string(" ExpectedModelName=")+ExpectedModelName;
}
};
/*
// Тип класса компонента не может быть приведен к требуемому
struct EErrorEngineIncompatibleCast: public EErrorEngine
{
protected: // Данные исключения
/// Полученное имя
std::string GotModelName;

/// Ожидаемое имя
std::string ExpectedModelName;

public:
EErrorEngineModelNameDontMatch(const std::string &got_name, const std::string &expected_name)
: EErrorEngine(), GotModelName(got_name), ExpectedModelName(expected_name) {};

// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const
{
 return EErrorEngine::CreateLogMessage()+std::string(" GotModelName=")+GotModelName+
				std::string(" ExpectedModelName=")+ExpectedModelName;
}
};  */
/*
// Имя модели не совпадает с ожидаемым
struct EErrorEngineModelNameDontMatch: public EErrorEngine
{
protected: // Данные исключения
/// Полученное имя
std::string GotModelName;

/// Ожидаемое имя
std::string ExpectedModelName;

public:
EErrorEngineModelNameDontMatch(const std::string &got_name, const std::string &expected_name)
: EErrorEngine(), GotModelName(got_name), ExpectedModelName(expected_name) {};

// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const
{
 return EErrorEngine::CreateLogMessage()+std::string(" GotModelName=")+GotModelName+
 				std::string(" ExpectedModelName=")+ExpectedModelName;
}
};
    */
};

// Возвращает ссылку на данные свойства компонента
template<class T>
const T& UEngine::Model_GetComponentPropertyData(const char *stringid, const char *property_name)
{
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  UEPtr<UVBaseDataProperty<T> > property=dynamic_pointer_cast<UVBaseDataProperty<T> >(iproperty);
  return property.GetData();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Копирует данные 'data' в заданное свойство компонента
template<class T>
int UEngine::Model_SetComponentPropertyData(const char *stringid, const char *property_name, const T &data)
{
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  UEPtr<UVBaseDataProperty<T> > property=dynamic_pointer_cast<UVBaseDataProperty<T> >(iproperty);
  property.SetData(data);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

}
#endif
