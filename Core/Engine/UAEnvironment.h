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

#include "UAStorage.h"
#include "UALibrary.h"

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

class UAEnvironment: virtual public UModule
{
protected: // Параметры
// Индекс предарительно заданной модели обработки
// 0 - Структура определяется извне
int PredefinedStructure;

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
UAStorage* Storage;

// Исследуемая модель
UEPtr<UAContainer> Model;

// Массив доступных библиотек
UClassLibraryList ClassLibraryList;

// Массив имен загруженных классов
//vector<string> CompletedClassNames;

// Массив имен не загруженных классов
//vector<string> IncompletedClassNames;

protected: // Контроллеры

protected: // Данные графического интерфеса пользователя


protected: // Переменные быстрого доступа
// Текущий компонент модели
UEPtr<UAComponent> CurrentComponent;

public: // Public methods
// --------------------------
// Constructors & destructors
// --------------------------
UAEnvironment(void);
virtual ~UAEnvironment(void);
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
// --------------------------

// --------------------------
// Методы управления состояниями
// --------------------------
public:
// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool IsStoragePresent(void) const;

// Возвращает состояние инициализации
virtual bool IsInit(void) const;

// Признак наличия сформированной структуры
virtual bool IsStructured(void) const;
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Возвращает указатель на хранилище
virtual UAStorage* GetStorage(void);

// Устанавливает новое хранилище
// Указатель на старое хранилище более не используется средой
// Ответственность за освобождение памяти лежит на вызывающей стороне
// Текущая модель уничтожается.
virtual bool SetStorage(UAStorage *storage);

// Возвращает указатель на модель
UEPtr<UAContainer> GetModel(void);

// Создает новую модель из хранилища по id класса
virtual bool CreateModel(const UId& classid);

// Уничтожает текущую модель
virtual bool DestroyModel(void);

// Возвращает библиотеку по индексу
UALibrary* GetClassLibrary(int index);

// Возвращает число библиотек
int GetNumClassLibraries(void) const;

// Непосредственно добавялет новый образец класса в хранилище
virtual bool AddClass(UAContainer *newclass);

// Подключает динамическую библиотеку с набором образцов классов.
// Если бибилиотека с таким именем уже существует то возвращает false.
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
virtual bool AddClassLibrary(UALibrary *library);

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool DelClassLibrary(int index);

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
virtual bool DelAllClassLibraries(void);

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
virtual bool BuildStorage(void);
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
// Возвращает указатель на модель
//NModel* const operator -> (void);

// Возвращает указатель на компонент модели по длинному имени
// Выбраный компонент становится текущим
// Если компонент не найден, то возвращает 0 и сбрасывает текущий компонент
//UAComponent* operator () (const string &longname);

// Возвращает указатель на компонент модели по длинному идентификатору
// Выбраный компонент становится текущим
// Если компонент не найден, то возвращает 0 и сбрасывает текущий компонент
//UAComponent* operator () (const LongUId &longid);

// Возвращает указатель на текущий компонент модели
UEPtr<UAComponent> operator () (void);
// --------------------------

// --------------------------
// Данные графического интерфеса пользователя
// --------------------------
// --------------------------


// --------------------------
// Методы управления
// --------------------------
public:
// Инициализация среды
virtual bool Init(void);

// Деинициализация среды
virtual bool UnInit(void);

// Формирует предварительно заданную модель обработки
virtual bool CreateStructure(void);

// Уничтожает текущую модель обработки
virtual bool DestroyStructure(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Инициализация среды
virtual bool AInit(void);

// Деинициализация среды
virtual bool AUnInit(void);

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

// --------------------------
// Вспомогательные методы инициализации среды
// --------------------------
protected:
// --------------------------
};

}
#endif
