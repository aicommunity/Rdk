/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAENVIRONMENT_H
#define UAENVIRONMENT_H

#include "UStorage.h"
#include "ULibrary.h"

namespace RDK {

class UClassLibraryList
{
public:
protected: // Data
// Classes list
PUALibrary* Libraries;

// Number of libraries into storage
int Size;

// real number of allocated elements
int RealSize;

public: // Methods
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UClassLibraryList(void);
UClassLibraryList(const UClassLibraryList &copy);
virtual ~UClassLibraryList(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает хранилище
void Clear(void);

// Изменяет размер хранилища
// Метод сохраняет старые данные
void Resize(int newsize);

// Возвращает размер массива
int GetSize(void) const;

// Удаляет из массива библиотеку по индексу
// Возвращает указатель на удаленную библиотеку
PUALibrary Erase(int index);

// Добавляет новый элемент в конец хранилища
void PushBack(PUALibrary classelement);

// Возвращает указатель на массив классов
PUALibrary* GetLibraries(void) const;
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UClassLibraryList& operator = (const UClassLibraryList &copy);

// Оператор доступа
PUALibrary operator [] (int i);
// --------------------------
};

class UEnvironment: virtual public UModule
{
protected: // Параметры
// Индекс предарительно заданной модели обработки
// 0 - Структура определяется извне
int PredefinedStructure;

// Имя текущего каталога хранения данных
std::string CurrentDataDir;

protected: // Состояния
// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool StoragePresent;

// Признак успешной инициализации
bool Initialized;

// Признак наличия сформированной структуры
bool Structured;

// Идентификатор компонента модели, который будет обсчитываться
ULongId ModelCalculationComponent;

protected: // Данные
// Хранилище
UStorage* Storage;

// Исследуемая модель
UEPtr<UContainer> Model;

// Массив доступных библиотек
UClassLibraryList ClassLibraryList;

// Массив имен загруженных классов
vector<string> CompletedClassNames;

// Массив имен не загруженных классов
vector<string> IncompletedClassNames;

/// Время среды
UTimeControl Time;

// Максимальный интеревал времени, который систем пытается "догнать" в режиме
// расчета реального времени (мс)
double MaxModelDuration;

protected: // Контроллеры

protected: // Данные графического интерфеса пользователя


protected: // Переменные быстрого доступа
// Текущий компонент модели
UEPtr<UComponent> CurrentComponent;

protected: // Временные переменные
long long StartupTime;

long long /*StartProcTime,*/CurrentTime,LastDuration/*,LastSentTime*/, ProcEndTime;

public: // Public methods
// --------------------------
// Constructors & destructors
// --------------------------
UEnvironment(void);
virtual ~UEnvironment(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
public:
// Индекс предарительно заданной модели обработки
int GetPredefinedStructure(void) const;
bool SetPredefinedStructure(int value);

// Идентификатор компонента модели, который будет обсчитываться
const ULongId& GetModelCalculationComponent(void) const;
bool SetModelCalculationComponent(const ULongId& value);

// Имя текущего каталога хранения данных
const std::string& GetCurrentDataDir(void);
void SetCurrentDataDir(const std::string& dir);
// --------------------------

// --------------------------
// Методы управления состояниями
// --------------------------
public:
// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool IsStoragePresent(void) const;

// Признак наличия сформированной структуры
virtual bool IsStructured(void) const;
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Возвращает указатель на хранилище
virtual UStorage* GetStorage(void);

// Устанавливает новое хранилище
// Указатель на старое хранилище более не используется средой
// Ответственность за освобождение памяти лежит на вызывающей стороне
// Текущая модель уничтожается.
virtual bool SetStorage(UStorage *storage);

// Возвращает указатель на модель
UEPtr<UContainer> GetModel(void);

// Создает новую модель из хранилища по имени класса
virtual bool CreateModel(const NameT& classname);

// Создает новую модель из хранилища по id класса
virtual bool CreateModel(const UId& classid);

// Уничтожает текущую модель
virtual bool DestroyModel(void);

// Возвращает библиотеку по индексу
ULibrary* GetClassLibrary(int index);

// Возвращает число библиотек
int GetNumClassLibraries(void) const;

// Возвращает библиотеку по имени
ULibrary* GetClassLibrary(const string &name);

// Возвращает имя библиотеки по индексу
const string& GetClassLibraryName(int index);

// Возвращает версию библиотеки по индексу
const string& GetClassLibraryVersion(int index);

// Непосредственно добавялет новый образец класса в хранилище
virtual bool AddClass(UContainer *newclass);

// Подключает динамическую библиотеку с набором образцов классов.
// Если бибилиотека с таким именем уже существует то возвращает false.
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
virtual bool AddClassLibrary(ULibrary *library);

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool DelClassLibrary(int index);

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool DelClassLibrary(const string &name);

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool DelAllClassLibraries(void);

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
virtual bool BuildStorage(void);

/// Время среды
const UTimeControl& GetTime(void) const;
UTimeControl& GetTime(void);
// --------------------------

// --------------------------
// Методы управления моделью
// --------------------------
// --------------------------

// --------------------------
// Методы управления контроллерами
// --------------------------
// --------------------------

// --------------------------
// Операторы доступа к данным среды
// --------------------------
// Возвращает указатель на текущий компонент модели
UEPtr<UContainer> GetCurrentComponent(void);

// Устанавливает указатель на текущий компонент модели
// Если имя или id не задано, или Forbidden, то устанавливает
// указатель на модель
virtual void SelectCurrentComponent(const NameT &name);
virtual void SelectCurrentComponent(const ULongId &id);

// Устанавливает указатель на текущий компонент модели на саму модель
virtual void ResetCurrentComponent(void);

// Устанавливает указатель на текущий компонент модели на родительский компонент
// (переход на уровень вверх). Если уже указывает на модель, то не делает ничего
virtual void UpCurrentComponent(void);

// Устанавливает указатель на текущий компонент модели на дочерней компонент на
// любом уровне (переход на уровень вниз).
virtual void DownCurrentComponent(const NameT &name);
virtual void DownCurrentComponent(const ULongId &id);
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
public:
// Производит увеличение времени модели на требуемую величину
void IncreaseModelTimeByStep(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
public:
// Инициализация среды
virtual void Init(void);

// Деинициализация среды
virtual void UnInit(void);

// Формирует предварительно заданную модель обработки
virtual bool CreateStructure(void);

// Уничтожает текущую модель обработки
virtual bool DestroyStructure(void);

// Расчет модели в реальном времени
virtual void RTCalculate(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Инициализация среды
virtual void AInit(void);

// Деинициализация среды
virtual void AUnInit(void);

// Формирует предварительно заданную модель обработки
virtual bool ACreateStructure(void);

// Уничтожает текущую модель обработки
virtual bool ADestroyStructure(void);

protected:
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

}
#endif
