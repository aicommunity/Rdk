#ifndef UAppProject_H
#define UAppProject_H

#include "UIVisualController.h"

namespace RDK
{

struct RDK_LIB_TYPE TProjectChannelConfig
{
/// Вариант источника модели для канала
/// 0 - по умолчанию (не изменять)
/// 1 - предустановленная конфигурация
/// 2 - модель из файла
/// 3 - модель из класса хранилища
int ModelMode;

/// Идентификатор предустановленной конфигурации
int PredefinedStructure;

/// Имя файла модели
std::string ModelFileName;

/// Имя класса компонента модели
std::string ClassName;

/// Глобальный шаг счета
int GlobalTimeStep;

/// Шаг счета по умолчанию
int DefaultTimeStep;

/// Режим расчета
int CalculationMode;

/// Минимальный интервал между шагами расчета, мс
int MinInterstepsInterval;

/// Инициализировать канал после загрузки
bool InitAfterLoad;

/// Сбросить расчет после загрузки
bool ResetAfterLoad;

/// Использовать отладочный режим
bool DebugMode;

/// Имя канала
std::string ChannelName;

TProjectChannelConfig(void);
TProjectChannelConfig(const TProjectChannelConfig& copy);

bool operator != (const TProjectChannelConfig& copy) const;
bool operator == (const TProjectChannelConfig& copy) const;
};


struct RDK_LIB_TYPE TProjectConfig
{
/// Папка проекта
std::string ProjectDirectory;

/// Имя проекта
std::string ProjectName;

/// Описание проекта
std::string ProjectDescription;

/// Флаг автоматического сохранения проекта при закрытии
bool ProjectAutoSaveFlag;

/// Флаг автоматического сохранения состояний моделей при закрытии
bool ProjectAutoSaveStatesFlag;

/// Флаг сохранения лога событий в файл
bool EventsLogFlag;

/// Режим проекта
/// 0 - простой
/// 1 - сервер
int ProjectMode;

/// Тип проекта
/// 0 - универсальный
/// 1 - видеоаналитика
int ProjectType;

/// Режим многопоточности
/// 0 - однопоточный режим
/// 1 - каждый канал работает в отдельном потоке
int MultiThreadingMode;

/// Режим привязки времени модели
/// 0 - привязка времени модели к системному времени
/// 1 - привязка времени модели ко времени источника данных
int CalcSourceTimeMode;

/// Флаг отображения состояния расчета каналов
int ShowChannelsStateFlag;

bool ReflectionFlag;

/// Число каналов
int NumChannels;

/// Флаг запрета останова источников видео-сигнала при остановке проекта
bool DisableStopVideoSources;

/// Массив конфигураций каналов
std::vector<TProjectChannelConfig> ChannelsConfig;


TProjectConfig(void);
TProjectConfig(const TProjectConfig& copy);

bool operator != (const TProjectConfig& copy) const;
bool operator == (const TProjectConfig& copy) const;
};

class RDK_LIB_TYPE UProject
{
protected: // Данные
/// Данные проекта
TProjectConfig Config;

/// Флаг наличия модификации в проекте
bool ModifiedFlag;

public: // Данные
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProject(void);
virtual ~UProject(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Данные проекта
const TProjectConfig& GetConfig(void) const;
bool SetConfig(const TProjectConfig& value);
// --------------------------

// --------------------------
// Методы управления проектом
// --------------------------
/// Проверяет наличие изменений в конфигурации проекта
bool IsModified(void) const;

/// Выставляет признак наличия изменений в конфигурации проекта
bool SetModified(void);

/// Сбрасывает признак наличия изменений в конфигурации проекта
bool ResetModified(void);

/// Сбрасывает конфигурацию проекта в состояние по умолчанию
/// Метод также сбрасывает ModifiedFlag
void ResetToDefault(void);
// --------------------------
};


}

#endif

