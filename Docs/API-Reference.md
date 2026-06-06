# Справочник API Rdk Core

## RU

### Обзор

Этот документ содержит справочник по всем публичным классам, методам, типам и константам Rdk Core. Организован по модулям для удобной навигации.

> **Audit:** статус синхронизации с заголовками `Rdk/Core/` — [Documentation-Status.md](Documentation-Status.md). Источник истины — публичные заголовки в `Rdk/Core/**` и `Rdk/GUI/Qt/**`.

### Структура справочника

1. [Core/Engine](#coreengine) - компонентная система и движок
2. [Core/Application](#coreapplication) - приложение, RPC, проекты
3. [Core/Graphics](#coregraphics) - графика и визуализация
4. [Core/Serialize](#coreserialize) - сериализация
5. [Core/System](#coresystem) - системные абстракции
6. [Core/Math](#coremath) - математические библиотеки
7. [Core/Utilities](#coreutilities) - утилиты
8. [Типы и константы](#типы-и-константы) - общие типы и константы

---

## Core/Engine

### UModule

Базовый абстрактный класс для всех модулей системы.

**Заголовок:** `Rdk/Core/Engine/UModule.h`

**Виртуальные методы:**
- `virtual bool ADefault(void) = 0` - установка значений по умолчанию
- `virtual bool ABuild(void) = 0` - построение внутренней структуры
- `virtual bool AReset(void) = 0` - сброс состояния
- `virtual bool ACalculate(void) = 0` - выполнение вычислений

**Публичные методы:**
- `bool IsReady(void) const` - проверка готовности модуля
- `bool IsInit(void) const` - проверка инициализации
- `bool Default(void)` - вызов ADefault с хуками
- `bool Build(void)` - вызов ABuild с хуками
- `bool Reset(void)` - вызов AReset с хуками
- `bool Calculate(void)` - вызов ACalculate с хуками

**Хуки (переопределяемые):**
- `virtual bool BeforeDefault(void)` - перед ADefault
- `virtual bool AfterDefault(void)` - после ADefault
- `virtual bool BeforeBuild(void)` - перед ABuild
- `virtual bool AfterBuild(void)` - после ABuild
- `virtual bool BeforeReset(void)` - перед AReset
- `virtual bool AfterReset(void)` - после AReset
- `virtual bool BeforeCalculate(void)` - перед ACalculate
- `virtual bool AfterCalculate(void)` - после ACalculate

### UComponent

Базовый класс для всех компонентов системы.

**Заголовок:** `Rdk/Core/Engine/UComponent.h`

**Публичные методы:**
- `NameT GetName(void) const` - получение имени компонента
- `UId GetId(void) const` - получение идентификатора компонента
- `UEPtr<UIProperty> GetProperty(const std::string& name)` - получение свойства по имени
- `template<typename T> UEPtr<T> GetProperty(const std::string& name)` - получение типизированного свойства
- `template<typename T> bool SetProperty(const std::string& name, const T& value)` - установка значения свойства
- `UEPtr<UIProperty> FindProperty(const std::string& name)` - поиск свойства

**Защищенные данные:**
- `UEPtr<UComponent> Owner` - родительский компонент
- `UEPtr<UComponent> MainOwner` - главный владелец
- `UEPtr<UStorage> Storage` - хранилище компонентов
- `UEPtr<UEnvironment> Environment` - окружение выполнения

### UContainer

Класс-контейнер для группировки других компонентов.

**Заголовок:** `Rdk/Core/Engine/UContainer.h`

**Публичные свойства:**
- `UProperty<NameT> Name` - имя контейнера
- `UProperty<UId> Id` - идентификатор контейнера
- `UProperty<bool> Activity` - флаг активности
- `UProperty<UTime> TimeStep` - шаг времени

**Публичные методы:**
- `bool AddComponent(UEPtr<UContainer> component)` - добавление дочернего компонента
- `bool DelComponent(UEPtr<UContainer> component)` - удаление дочернего компонента
- `UEPtr<UContainer> GetComponent(const std::string& name)` - получение компонента по имени
- `template<typename T> UEPtr<T> GetComponent(const std::string& name)` - получение компонента с приведением типа
- `void UpdateControllers(void)` - обновление всех контроллеров

**Виртуальные методы:**
- `virtual bool AAddComponent(UEPtr<UContainer> component)` - пользовательская логика при добавлении
- `virtual bool ADelComponent(UEPtr<UContainer> component)` - пользовательская логика при удалении

### UNet

Специализированный контейнер для сетей компонентов.

**Заголовок:** `Rdk/Core/Engine/UNet.h`

**Публичные методы:**
- `template<typename T> ULinksListT<T>& GetLinks(ULinksListT<T>& linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const` - получение всех связей
- `UContainer* New(void)` - создание нового экземпляра сети
- `bool Copy(UContainer* target, UStorage* storage)` - копирование сети
- `void Free(void)` - освобождение сети
- `bool CheckComponentType(UEPtr<UContainer> comp)` - проверка допустимости типа компонента

### UEngine

Главный класс движка системы.

**Заголовок:** `Rdk/Core/Engine/UEngine.h`

**Публичные методы:**
- `UStorage* GetStorage(void)` - получение хранилища компонентов
- `UEnvironment* GetEnvironment(void)` - получение окружения выполнения
- `bool Init(void)` - инициализация движка
- `bool Start(void)` - запуск движка
- `bool Stop(void)` - остановка движка
- `bool Reset(void)` - сброс состояния движка
- `bool Calculate(void)` - выполнение одного шага вычислений

### UEnvironment

Окружение выполнения компонентов.

**Заголовок:** `Rdk/Core/Engine/UEnvironment.h`

**Публичные методы:**
- `UEPtr<UContainer> GetModel(void)` - получение корневого компонента модели
- `bool SetModel(UEPtr<UContainer> model)` - установка корневого компонента модели
- `bool Reset(void)` - сброс окружения
- `bool Calculate(void)` - выполнение одного шага вычислений
- `bool RTCalculate(void)` - выполнение расчета в реальном времени

### UStorage

Хранилище компонентов и классов.

**Заголовок:** `Rdk/Core/Engine/UStorage.h`

**Публичные методы:**
- `UId FindClassId(const std::string& classname)` - поиск ID класса по имени
- `NameT FindClassName(UId classid)` - поиск имени класса по ID
- `UId AddClass(const std::string& classname, UComponentAbstractFactory* factory)` - регистрация класса компонента
- `UEPtr<UComponent> CreateComponent(const std::string& classname)` - создание экземпляра по имени класса
- `UEPtr<UComponent> CreateComponent(UId classid)` - создание экземпляра по ID класса
- `bool AddObject(UEPtr<UContainer> object)` - добавление экземпляра в хранилище
- `bool DelObject(UEPtr<UContainer> object)` - удаление экземпляра из хранилища
- `bool LoadLibrary(UEPtr<ULibrary> library)` - загрузка библиотеки компонентов
- `bool Build(void)` - сборка всех компонентов

### UIProperty

Интерфейс для свойств компонентов.

**Заголовок:** `Rdk/Core/Engine/UProperty.h`

**Публичные методы:**
- `std::string GetName(void) const` - получение имени свойства
- `UId GetId(void) const` - получение ID свойства
- `void* GetMemoryArea(void)` - получение указателя на данные
- `const std::type_info& GetLanguageType(void) const` - получение типа данных
- `bool Save(USerStorage* storage)` - сохранение свойства
- `bool Load(USerStorage* storage)` - загрузка свойства

### UIPropertyInput

Интерфейс для входных свойств.

**Заголовок:** `Rdk/Core/Engine/UPropertyInput.h`

**Публичные методы:**
- `bool IsConnected(void) const` - проверка подключения
- `bool SetPointer(UEPtr<UIPropertyOutput> output)` - установка указателя на выходное свойство
- `void* GetData(void)` - получение данных

### UIPropertyOutput

Интерфейс для выходных свойств.

**Заголовок:** `Rdk/Core/Engine/UPropertyOutput.h`

**Публичные методы:**
- `bool ConnectTo(UEPtr<UIPropertyInput> input)` - подключение к входному свойству
- `bool DisconnectFrom(UEPtr<UIPropertyInput> input)` - отключение от входного свойства
- `void SetData(void* data)` - установка данных

### UController

Базовый класс контроллеров для связи компонентов с GUI.

**Заголовок:** `Rdk/Core/Engine/UController.h`

**Публичные методы:**
- `bool IsEnabled(void) const` - проверка активности контроллера
- `bool IsEnabled(bool value)` - установка активности контроллера
- `std::string GetComponentName(void) const` - получение имени компонента
- `bool Link(UContainer* component, bool forchilds=false)` - связывание с компонентом
- `bool UnLink(bool forchilds=false)` - отвязывание от компонента
- `bool Update(void)` - обновление контроллера

**Виртуальные методы:**
- `virtual bool ALink(UContainer* component)` - пользовательская логика при связывании
- `virtual bool AUnLink(void)` - пользовательская логика при отвязывании
- `virtual bool AUpdate(void)` - пользовательская логика обновления

---

## Core/Application

### UApplication

Главный класс приложения.

**Заголовок:** `Rdk/Core/Application/UApplication.h`

**Публичные методы:**
- `bool OpenProject(const std::string& path)` - открытие проекта
- `bool CloseProject(void)` - закрытие проекта
- `bool SaveProject(void)` - сохранение проекта
- `UProject* GetProject(void)` - получение активного проекта
- `UEngineControl* GetEngineControl(void)` - получение контроллера движка
- `UServerControl* GetServerControl(void)` - получение контроллера сервера
- `URpcDispatcher* GetRpcDispatcher(void)` - получение RPC диспетчера

### URpcDispatcher

Диспетчер RPC команд.

**Заголовок:** `Rdk/Core/Application/URpcDispatcher.h`

**Публичные методы:**
- `bool IsCmdSupported(UEPtr<URpcCommand> command)` - проверка поддержки команды
- `void SetDecoderPrototype(UEPtr<URpcDecoder> decoder)` - установка прототипа декодера
- `bool SetCommonDecoder(UEPtr<URpcDecoder> decoder)` - установка общего декодера
- `void Dispatch(void)` - диспетчеризация команд из очереди
- `void StopDispatch(void)` - остановка диспетчеризации
- `bool SyncDispatchCommand(UEPtr<URpcCommand> command, int timeout_ms)` - синхронная диспетчеризация команды

### URpcDecoder

Абстрактный класс декодера RPC команд.

**Заголовок:** `Rdk/Core/Application/URpcDecoder.h`

**Публичные методы:**
- `URpcDecoder* New(void)` - создание копии декодера
- `bool IsCmdSupported(UEPtr<URpcCommand> command)` - проверка поддержки команды
- `void Process(void)` - обработка команд из очереди
- `bool ProcessCommand(UEPtr<URpcCommand> command)` - обработка одной команды

**Виртуальные методы:**
- `virtual bool AProcessCommand(UEPtr<URpcCommand> command) = 0` - абстрактный метод обработки команды

### UServerTransport

Абстрактный класс транспорта сервера.

**Заголовок:** `Rdk/Core/Application/UServerTransport.h`

**Публичные методы:**
- `void SetServerBinding(const std::string& address, int port)` - установка адреса и порта
- `int GetServerBindingPort(void) const` - получение порта
- `bool ServerIsActive(void) const` - проверка активности сервера
- `void ProcessIncomingData(void)` - обработка входящих данных

### UProject

Класс проекта.

**Заголовок:** `Rdk/Core/Application/UProject.h`

**Публичные методы:**
- `std::string GetProjectName(void) const` - получение имени проекта
- `std::string GetProjectPath(void) const` - получение пути проекта
- `TProjectConfig& GetConfig(void)` - получение конфигурации проекта
- `TProjectChannelConfig& GetChannelConfig(int channel)` - получение конфигурации канала
- `bool LoadProject(const std::string& path)` - загрузка проекта
- `bool SaveProject(const std::string& path)` - сохранение проекта
- `bool CreateChannel(int index)` - создание канала
- `bool DeleteChannel(int index)` - удаление канала
- `UEnvironment* GetEnvironment(int channel)` - получение окружения канала

### UEngineControl

Контроллер движка.

**Заголовок:** `Rdk/Core/Application/UEngineControl.h`

**Публичные методы:**
- `int GetThreadMode(void) const` - получение режима работы
- `void SetThreadMode(int mode)` - установка режима работы
- `int GetUpdateInterval(void) const` - получение интервала обновления
- `void SetUpdateInterval(int interval)` - установка интервала обновления
- `bool StartCalculation(int channel)` - запуск расчета канала
- `bool StopCalculation(int channel)` - остановка расчета канала
- `bool ResetCalculation(int channel)` - сброс расчета канала
- `UCalcState GetCalcState(int channel)` - получение состояния расчета
- `void UpdateInterface(void)` - обновление интерфейса

---

## Core/Graphics

### UGraphics

Конкретная реализация графического интерфейса.

**Заголовок:** `Rdk/Core/Graphics/UGraphics.h`

**Публичные методы:**
- `UBitmap* GetCanvas(void)` - получение канвы
- `bool SetCanvas(UBitmap* canvas)` - установка канвы
- `void Pixel(int x, int y, bool ispos=true)` - отрисовка пикселя
- `void Line(int x1, int y1, int x2, int y2)` - отрисовка линии
- `void Circle(int x, int y, int r, bool fill=false)` - отрисовка окружности
- `void Rect(int x1, int y1, int x2, int y2, bool fill=false)` - отрисовка прямоугольника
- `void Text(int x, int y, const std::string& text)` - вывод текста

### UBitmap

Класс для работы с растровыми изображениями.

**Заголовок:** `Rdk/Core/Graphics/UBitmap.h`

**Публичные методы:**
- `int GetWidth(void) const` - получение ширины
- `int GetHeight(void) const` - получение высоты
- `UBMColorModel GetColorModel(void) const` - получение цветовой модели
- `UBColor* GetData(void)` - получение данных изображения
- `bool Resize(int width, int height)` - изменение размера
- `void Clear(UColorT color)` - очистка изображения
- `bool Copy(const UBitmap& src, int x, int y)` - копирование изображения
- `UColorT GetPixel(int x, int y) const` - получение пикселя
- `void SetPixel(int x, int y, UColorT color)` - установка пикселя

### UAFont

Абстрактный базовый класс для шрифтов.

**Заголовок:** `Rdk/Core/Graphics/UFont.h`

**Публичные методы:**
- `int GetHeight(void) const` - получение высоты шрифта
- `int CalcWidth(char ch)` - вычисление ширины символа
- `int CalcWidth(wchar_t ch)` - вычисление ширины символа
- `void DrawText(UGraphics* graphics, int x, int y, const std::string& text)` - отрисовка текста

---

## Core/Serialize

### USerStorage

Абстрактный базовый класс для хранилища данных сериализации.

**Заголовок:** `Rdk/Core/Serialize/USerStorage.h`

**Виртуальные методы:**
- `virtual bool Create(const std::string& name) = 0` - создание узла
- `virtual bool SelectUp(void) = 0` - переход к родительскому узлу
- `virtual bool AddNode(const std::string& name) = 0` - добавление дочернего узла
- `virtual bool GetNode(const std::string& name) = 0` - получение дочернего узла
- `virtual std::string Read(const std::string& name) = 0` - чтение значения
- `virtual bool Write(const std::string& name, const std::string& value) = 0` - запись значения

### USerStorageXML

Реализация хранилища для XML формата.

**Заголовок:** `Rdk/Core/Serialize/USerStorageXML.h`

**Публичные методы:**
- `bool SaveToFile(const std::string& path)` - сохранение в файл
- `bool LoadFromFile(const std::string& path)` - загрузка из файла

### USerStorageBinary

Реализация хранилища для бинарного формата.

**Заголовок:** `Rdk/Core/Serialize/USerStorageBinary.h`

**Публичные методы:**
- `bool SaveToFile(const std::string& path)` - сохранение в файл
- `bool LoadFromFile(const std::string& path)` - загрузка из файла

---

## Core/System

### UGenericMutex

Абстрактный базовый класс для мьютексов.

**Заголовок:** `Rdk/Core/System/UGenericMutex.h`

**Виртуальные методы:**
- `virtual bool Lock(void) = 0` - блокировка мьютекса
- `virtual bool Unlock(void) = 0` - разблокировка мьютекса
- `virtual bool TryLock(void) = 0` - попытка блокировки без ожидания

### UGenericEvent

Абстрактный базовый класс для событий.

**Заголовок:** `Rdk/Core/System/UGenericEvent.h`

**Виртуальные методы:**
- `virtual bool Wait(int timeout_ms) = 0` - ожидание сигнала
- `virtual bool Signal(void) = 0` - отправка сигнала
- `virtual bool Reset(void) = 0` - сброс события

### UDllLoader

Абстрактный базовый класс для загрузки динамических библиотек.

**Заголовок:** `Rdk/Core/System/UDllLoader.h`

**Виртуальные методы:**
- `virtual bool Load(const std::string& path) = 0` - загрузка библиотеки
- `virtual bool Unload(void) = 0` - выгрузка библиотеки
- `virtual void* GetFunction(const std::string& name) = 0` - получение указателя на функцию

---

## Core/Math

### MMatrix\<T, Rows, Cols\>

Шаблонный класс для работы с матрицами фиксированного размера.

**Заголовок:** `Rdk/Core/Math/MMatrix.h`

**Публичные методы:**
- `unsigned GetRows(void) const` - получение числа строк
- `unsigned GetCols(void) const` - получение числа столбцов
- `T& operator()(int row, int col)` - доступ к элементу
- `MMatrix Transpose(void) const` - транспонирование
- `MMatrix Inverse(void) const` - обращение
- `T Det(void) const` - детерминант
- `MMatrix operator*(const MMatrix& other) const` - матричное умножение
- `static MMatrix Eye(void)` - создание единичной матрицы

### MDMatrix\<T\>

Класс для работы с матрицами динамического размера.

**Заголовок:** `Rdk/Core/Math/MDMatrix.h`

**Публичные методы:**
- `bool Resize(int rows, int cols, T defvalue=0)` - изменение размера
- `int GetRows(void) const` - получение числа строк
- `int GetCols(void) const` - получение числа столбцов
- `T& operator()(int row, int col)` - доступ к элементу

### MVector\<T, Size\>

Класс для работы с векторами фиксированной размерности.

**Заголовок:** `Rdk/Core/Math/MVector.h`

**Публичные методы:**
- `unsigned GetSize(void) const` - получение размерности
- `T& operator[](int index)` - доступ к элементу
- `T Norm(void) const` - норма вектора
- `void Normalize(void)` - нормализация
- `T operator&(const MVector& other) const` - скалярное произведение
- `MVector operator^(const MVector& other) const` - векторное произведение (3D)

### MDKalmanFilter\<T\>

Класс для фильтра Калмана динамического размера.

**Заголовок:** `Rdk/Core/Math/MDKalmanFilter.h`

**Публичные методы:**
- `void SetKalmanSize(int num_states, int num_measurements)` - установка размерности
- `void SetFM(const MDMatrix<T>&)` - установка матрицы F
- `void SetHM(const MDMatrix<T>&)` - установка матрицы H
- `void SetQM(const MDMatrix<T>&)` - установка матрицы Q
- `void SetRM(const MDMatrix<T>&)` - установка матрицы R
- `bool KalmanCalculate(int mode)` - выполнение одного шага фильтрации
- `MDMatrix<T> GetXk1(void) const` - получение текущего состояния

---

## Core/Utilities

### UException

Базовый класс для всех исключений.

**Заголовок:** `Rdk/Core/Utilities/UException.h`

**Публичные методы:**
- `int GetNumber(void) const` - получение номера исключения
- `int GetType(void) const` - получение типа исключения
- `std::time_t GetTime(void) const` - получение времени возникновения
- `const char* what(void) const throw()` - получение сообщения
- `void Wrap(const UException& ex, const std::string& message)` - оборачивание исключения

### UTimeStamp

Класс для работы с временными метками.

**Заголовок:** `Rdk/Core/Utilities/UTimeStamp.h`

**Публичные методы:**
- `double operator()(void) const` - преобразование в секунды
- `UTimeStamp operator+(const UTimeStamp& other) const` - сложение
- `UTimeStamp operator-(const UTimeStamp& other) const` - вычитание
- `std::string ToString(void) const` - преобразование в строку
- `bool FromString(const std::string& str)` - парсинг из строки

### UIniFile

Класс для работы с INI файлами.

**Заголовок:** `Rdk/Core/Utilities/UIniFile.h`

**Публичные методы:**
- `std::string ReadString(const std::string& section, const std::string& key, const std::string& default)` - чтение строки
- `int ReadInteger(const std::string& section, const std::string& key, int default)` - чтение целого
- `double ReadDouble(const std::string& section, const std::string& key, double default)` - чтение вещественного
- `bool ReadBool(const std::string& section, const std::string& key, bool default)` - чтение булева значения
- `bool WriteString(const std::string& section, const std::string& key, const std::string& value)` - запись строки
- `bool LoadFromFile(const std::string& path)` - загрузка из файла
- `bool SaveToFile(const std::string& path)` - сохранение в файл

---

## Типы и константы

### Основные типы

- `NameT` - тип для имен (обычно `std::string`)
- `UId` - тип для идентификаторов (обычно `unsigned int`)
- `UEPtr<T>` - умный указатель на компонент
- `UELockPtr<T>` - блокирующий указатель на компонент
- `UTime` - тип для времени
- `UColorT` - тип для цвета
- `UBColor` - тип для цвета пикселя

### Константы исключений

- `RDK_EX_UNKNOWN` (0) - Неизвестное исключение
- `RDK_EX_FATAL` (1) - Фатальная ошибка
- `RDK_EX_ERROR` (2) - Исправимая ошибка
- `RDK_EX_WARNING` (3) - Предупреждение
- `RDK_EX_INFO` (4) - Информационное сообщение
- `RDK_EX_APP` (5) - Событие уровня приложения
- `RDK_EX_DEBUG` (6) - Отладочное сообщение

### Флаги свойств

- `ptNone` (0) - Нет типа
- `ptParameter` (1) - Параметр
- `ptState` (2) - Переменная состояния
- `ptTemp` (4) - Временная переменная
- `ptInput` (8) - Вход
- `ptOutput` (16) - Выход
- `pgPublic` (0x100) - Общедоступный
- `pgSystem` (0x200) - Системный

### См. также

- [Engine-Detailed.md](Engine-Detailed.md) - детальная документация движка
- [Application-Detailed.md](Application-Detailed.md) - детальная документация приложения
- [Math-Detailed.md](Math-Detailed.md) - детальная документация математики
- [Utilities-Detailed.md](Utilities-Detailed.md) - детальная документация утилит

---

## EN

### Overview

This document contains a reference for all public classes, methods, types, and constants in Rdk Core. Organized by modules for easy navigation.

### Structure

1. Core/Engine - component system and engine
2. Core/Application - application, RPC, projects
3. Core/Graphics - graphics and visualization
4. Core/Serialize - serialization
5. Core/System - system abstractions
6. Core/Math - mathematical libraries
7. Core/Utilities - utilities
8. Types and Constants - common types and constants

### See Also

- [Engine-Detailed.md](Engine-Detailed.md) - engine detailed documentation
- [Application-Detailed.md](Application-Detailed.md) - application detailed documentation
- [Math-Detailed.md](Math-Detailed.md) - math detailed documentation
- [Utilities-Detailed.md](Utilities-Detailed.md) - utilities detailed documentation
