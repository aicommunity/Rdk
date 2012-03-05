/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UACONTAINERENVIRONMENT_H
#define UACONTAINERENVIRONMENT_H

#include "UAEnvironment.h"
#include "UAContainerStorage.h"
//#include "ULibrary.h"

namespace RDK {

class UAContainerEnvironment: public UAEnvironment
{
protected: // Параметры

protected: // Данные
// Массив имен загруженных классов
vector<string> CompletedClassNames;

// Массив имен не загруженных классов
vector<string> IncompletedClassNames;

protected: // Контроллеры
// Список действующих контроллеров
// <Контроллер, флаг, разрешающий удалять контроллер среде>
//map<NController*, bool> Controllers;

protected: // Данные графического интерфеса пользователя
// Визуализаторы модели
//map<LongIdT, NGraphics*> Graphics;

protected: // Флаги состояний

protected: // Переменные быстрого доступа


public: // Public methods
// --------------------------
// Constructors & destructors
// --------------------------
UAContainerEnvironment(void);
virtual ~UAContainerEnvironment(void);
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Возвращает указатель на хранилище
virtual UAContainerStorage* GetStorage(void);

// Устанавливает новое хранилище
// Указатель на старое хранилище более не используется средой
// Ответственность за освобождение памяти лежит на вызывающей стороне
// Текущая модель уничтожается.
//virtual bool SetStorage(UContainerStorage *storage);

// Возвращает указатель на модель
//UAComponent* GetModel(void);

// Создает новую модель из хранилища по id класса
virtual bool CreateModel(const UId& classid);

// Создает новую модель из хранилища по имени класса
virtual bool CreateModel(const NameT& classname);

// Уничтожает текущую модель
//virtual bool DestroyModel(void);

// Возвращает библиотеку по индексу
//ULibrary* GetClassLibrary(int index);

// Возвращает библиотеку по имени
UALibrary* GetClassLibrary(const string &name);

// Возвращает имя библиотеки по индексу
const string& GetClassLibraryName(int index);

// Возвращает версию библиотеки по индексу
const string& GetClassLibraryVersion(int index);

// Возвращает число библиотек
//size_t GetNumClassLibraries(void) const;

// Подключает динамическую библиотеку с набором образцов классов.
// Если бибилиотека с таким именем уже существует то возвращает false.
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
bool AddClassLibrary(UALibrary *library);

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool DelClassLibrary(int index);

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool DelClassLibrary(const string &name);

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool DelAllClassLibraries(void);

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool BuildStorage(void);
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

// --------------------------
// Операторы доступа к данным среды
// --------------------------
// Возвращает указатель на текущий компонент модели
UEPtr<UAContainer> GetCurrentComponent(void);

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

// --------------------------
// Данные графического интерфеса пользователя
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к флагам состояний
// --------------------------
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// --------------------------

// --------------------------
// Вспомогательные методы инициализации среды
// --------------------------
protected:
// Загружает динамическую библиотеку с экземпляром хранилища и возвращает
// указатель на него.
// В базовой версии не делает ничего и возвращает 0.
virtual UAContainerStorage* LoadStorageLibrary(const string &filename);

// Выгружает динамическую библиотеку с экземпляром хранилища.
// В базовой версии не делает ничего и возвращает true.
virtual bool UnLoadStorageLibrary(void);

// Загружает динамическую библиотеку с набором образцов классов
// в виде экземпляра хранилища и возвращает указатель на него.
// В базовой версии не делает ничего и возвращает 0.
virtual UAContainerStorage* LoadClassLibrary(const string &libname, const string &filename);

// Выгружает динамическую библиотеку с набором образцов классов.
// В базовой версии не делает ничего и возвращает true.
virtual bool UnLoadClassLibrary(const string &libname);
// --------------------------
};

}
#endif
